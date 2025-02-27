/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=8 sts=2 et sw=2 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "base/basictypes.h"
#include "BluetoothOppManager.h"

#include "BluetoothService.h"
#include "BluetoothSocket.h"
#include "BluetoothUtils.h"
#include "BluetoothUuidHelper.h"
#include "ObexBase.h"

#include "mozilla/dom/bluetooth/BluetoothTypes.h"
#include "mozilla/dom/File.h"
#include "mozilla/ErrorResult.h"
#include "mozilla/RefPtr.h"
#include "mozilla/Services.h"
#include "mozilla/StaticPtr.h"
#include "mozilla/UniquePtr.h"
#include "nsCExternalHandlerService.h"
#include "nsIObserver.h"
#include "nsIObserverService.h"
#include "nsIFile.h"
#include "nsIInputStream.h"
#include "nsIMIMEService.h"
#include "nsIOutputStream.h"
#include "nsIThread.h"
#include "nsIVolumeService.h"
#include "nsNetUtil.h"
#include "nsServiceManagerUtils.h"

#define TARGET_SUBDIR "downloads/Bluetooth/"

USING_BLUETOOTH_NAMESPACE
using namespace mozilla;
using namespace mozilla::dom;
using namespace mozilla::ipc;

namespace {
// Sending system message "bluetooth-opp-update-progress" every 50kb
static const uint32_t kUpdateProgressBase = 50 * 1024;

/*
 * The format of the header of an PUT request is
 * [opcode:1][packet length:2][headerId:1][header length:2]
 */
static const uint32_t kPutRequestHeaderSize = 6;

/*
 * The format of the appended header of an PUT request is
 * [headerId:1][header length:4]
 * P.S. Length of name header is 4 since unicode is 2 bytes per char.
 */
static const uint32_t kPutRequestAppendHeaderSize = 5;

// The default timeout we permit to wait for SDP updating if we can't get
// service channel.
static const double kSdpUpdatingTimeoutMs = 3000.0;

// UUID of OBEX Object Push
static const BluetoothUuid kObexObjectPush(OBJECT_PUSH);

StaticRefPtr<BluetoothOppManager> sBluetoothOppManager;
}  // namespace

BEGIN_BLUETOOTH_NAMESPACE

bool BluetoothOppManager::sInShutdown = false;

NS_IMETHODIMP
BluetoothOppManager::Observe(nsISupports* aSubject, const char* aTopic,
                             const char16_t* aData) {
  MOZ_ASSERT(sBluetoothOppManager);

  if (!strcmp(aTopic, NS_VOLUME_STATE_CHANGED)) {
    HandleVolumeStateChanged(aSubject);
    return NS_OK;
  }

  if (!strcmp(aTopic, NS_XPCOM_SHUTDOWN_OBSERVER_ID)) {
    HandleShutdown();
    return NS_OK;
  }

  MOZ_ASSERT(false, "BluetoothOppManager got unexpected topic!");
  return NS_ERROR_UNEXPECTED;
}

class BluetoothOppManager::SendSocketDataTask final : public Runnable {
 public:
  SendSocketDataTask(UniquePtr<uint8_t[]> aStream, uint32_t aSize)
      : Runnable("SendSocketDataTask"),
        mStream(std::move(aStream)),
        mSize(aSize) {
    MOZ_ASSERT(!NS_IsMainThread());
  }

  NS_IMETHOD Run() {
    MOZ_ASSERT(NS_IsMainThread());

    sBluetoothOppManager->SendPutRequest(mStream.get(), mSize);

    return NS_OK;
  }

 private:
  UniquePtr<uint8_t[]> mStream;
  uint32_t mSize;
};

class BluetoothOppManager::ReadFileTask final : public Runnable {
 public:
  ReadFileTask(nsIInputStream* aInputStream, uint32_t aRemoteMaxPacketSize)
      : Runnable("ReadFileTask"), mInputStream(aInputStream) {
    MOZ_ASSERT(NS_IsMainThread());

    mAvailablePacketSize = aRemoteMaxPacketSize - kPutRequestHeaderSize;
  }

  NS_IMETHOD Run() {
    MOZ_ASSERT(!NS_IsMainThread());

    uint32_t numRead;
    auto buf = MakeUnique<uint8_t[]>(mAvailablePacketSize);

    // function inputstream->Read() only works on non-main thread
    nsresult rv =
        mInputStream->Read((char*)buf.get(), mAvailablePacketSize, &numRead);
    if (NS_FAILED(rv)) {
      // Needs error handling here
      BT_WARNING("Failed to read from input stream");
      return NS_ERROR_FAILURE;
    }

    if (numRead > 0) {
      sBluetoothOppManager->CheckPutFinal(numRead);

      RefPtr<SendSocketDataTask> task =
          new SendSocketDataTask(std::move(buf), numRead);
      if (NS_FAILED(NS_DispatchToMainThread(task))) {
        BT_WARNING("Failed to dispatch to main thread!");
        return NS_ERROR_FAILURE;
      }
    }

    return NS_OK;
  };

 private:
  nsCOMPtr<nsIInputStream> mInputStream;
  uint32_t mAvailablePacketSize;
};

class BluetoothOppManager::CloseSocketTask final : public Runnable {
 public:
  explicit CloseSocketTask(BluetoothSocket* aSocket)
      : Runnable("BluetoothOppManager"), mSocket(aSocket) {
    MOZ_ASSERT(aSocket);
  }

  NS_IMETHOD Run() override {
    MOZ_ASSERT(NS_IsMainThread());
    mSocket->Close();
    return NS_OK;
  }

 private:
  RefPtr<BluetoothSocket> mSocket;
};

BluetoothOppManager::BluetoothOppManager()
    : mConnected(false),
      mRemoteObexVersion(0),
      mRemoteConnectionFlags(0),
      mRemoteMaxPacketLength(0),
      mLastCommand(0),
      mPacketLength(0),
      mPutPacketReceivedLength(0),
      mBodySegmentLength(0),
      mNeedsUpdatingSdpRecords(false),
      mAbortFlag(false),
      mNewFileFlag(false),
      mPutFinalFlag(false),
      mTransferCompleteFlag(true),
      mSuccessFlag(false),
      mIsServer(true),
      mWaitingForConfirmationFlag(false),
      mFileLength(0),
      mSentFileLength(0),
      mWaitingToSendPutFinal(false),
      mCurrentBlobIndex(-1) {}

BluetoothOppManager::~BluetoothOppManager() {}

nsresult BluetoothOppManager::Init() {
  nsCOMPtr<nsIObserverService> obs = services::GetObserverService();
  NS_ENSURE_TRUE(obs, NS_ERROR_NOT_AVAILABLE);

  auto rv = obs->AddObserver(this, NS_XPCOM_SHUTDOWN_OBSERVER_ID, false);
  if (NS_FAILED(rv)) {
    BT_WARNING("Failed to add shutdown observer!");
    return rv;
  }

  rv = obs->AddObserver(this, NS_VOLUME_STATE_CHANGED, false);
  if (NS_FAILED(rv)) {
    BT_WARNING("Failed to add ns volume observer!");
    return rv;
  }

  /**
   * We don't start listening here as BluetoothServiceBluedroid calls Listen()
   * immediately when BT stops.
   *
   * If we start listening here, the listening fails when device boots up since
   * Listen() is called again and restarts server socket. The restart causes
   * absence of read events when device boots up.
   */

  return NS_OK;
}

void BluetoothOppManager::Uninit() {
  if (mServerSocket) {
    mServerSocket->SetObserver(nullptr);

    if (mServerSocket->GetConnectionStatus() != SOCKET_DISCONNECTED) {
      mServerSocket->Close();
    }
    mServerSocket = nullptr;
  }

  if (mSocket) {
    mSocket->SetObserver(nullptr);

    if (mSocket->GetConnectionStatus() != SOCKET_DISCONNECTED) {
      mSocket->Close();
    }
    mSocket = nullptr;
  }

  nsCOMPtr<nsIObserverService> obs = services::GetObserverService();
  NS_ENSURE_TRUE_VOID(obs);

  if (NS_FAILED(obs->RemoveObserver(this, NS_XPCOM_SHUTDOWN_OBSERVER_ID))) {
    BT_WARNING("Failed to remove shutdown observer!");
  }

  if (NS_FAILED(obs->RemoveObserver(this, NS_VOLUME_STATE_CHANGED))) {
    BT_WARNING("Failed to remove volume observer!");
  }
}

// static
void BluetoothOppManager::InitOppInterface(
    BluetoothProfileResultHandler* aRes) {
  MOZ_ASSERT(NS_IsMainThread());

  if (aRes) {
    aRes->Init();
  }
}

// static
void BluetoothOppManager::DeinitOppInterface(
    BluetoothProfileResultHandler* aRes) {
  MOZ_ASSERT(NS_IsMainThread());

  if (sBluetoothOppManager) {
    sBluetoothOppManager->Uninit();
    sBluetoothOppManager = nullptr;
  }

  if (aRes) {
    aRes->Deinit();
  }
}

// static
BluetoothOppManager* BluetoothOppManager::Get() {
  MOZ_ASSERT(NS_IsMainThread());

  // If sBluetoothOppManager already exists, exit early
  if (sBluetoothOppManager) {
    return sBluetoothOppManager;
  }

  // If we're in shutdown, don't create a new instance
  NS_ENSURE_FALSE(sInShutdown, nullptr);

  // Create a new instance, register, and return
  RefPtr<BluetoothOppManager> manager = new BluetoothOppManager();
  NS_ENSURE_SUCCESS(manager->Init(), nullptr);

  sBluetoothOppManager = manager;

  return sBluetoothOppManager;
}

void BluetoothOppManager::ConnectInternal(
    const BluetoothAddress& aDeviceAddress) {
  MOZ_ASSERT(NS_IsMainThread());

  // Stop listening because currently we only support one connection at a time.
  if (mServerSocket &&
      mServerSocket->GetConnectionStatus() != SOCKET_DISCONNECTED) {
    mServerSocket->Close();
  }
  mServerSocket = nullptr;

  mIsServer = false;

  BluetoothService* bs = BluetoothService::Get();
  if (!bs || sInShutdown || mSocket) {
    OnSocketConnectError(mSocket);
    return;
  }

  mNeedsUpdatingSdpRecords = true;

  if (NS_FAILED(bs->GetServiceChannel(aDeviceAddress, kObexObjectPush, this))) {
    OnSocketConnectError(mSocket);
    return;
  }

  mSocket = new BluetoothSocket(this);
}

void BluetoothOppManager::HandleShutdown() {
  MOZ_ASSERT(NS_IsMainThread());
  sInShutdown = true;
  Disconnect(nullptr);
  Uninit();

  sBluetoothOppManager = nullptr;
}

void BluetoothOppManager::HandleVolumeStateChanged(nsISupports* aSubject) {
  MOZ_ASSERT(NS_IsMainThread());

  if (!mConnected) {
    return;
  }

  /**
   * Disconnect ongoing OPP connection when a volume becomes non-mounted, in
   * case files of transfer are located on that volume. |OnSocketDisconnect|
   * will handle incomplete file transfers.
   */

  nsCOMPtr<nsIVolume> vol = do_QueryInterface(aSubject);
  if (!vol) {
    return;
  }

  int32_t state;
  vol->GetState(&state);
  if (nsIVolume::STATE_MOUNTED != state) {
    BT_LOGR("Volume becomes non-mounted. Abort ongoing OPP connection");
    Disconnect(nullptr);
  }
}

bool BluetoothOppManager::Listen() {
  MOZ_ASSERT(NS_IsMainThread());

  if (mSocket) {
    BT_WARNING("mSocket exists. Failed to listen.");
    return false;
  }

  /**
   * Restart server socket since its underlying fd becomes invalid when
   * BT stops; otherwise no more read events would be received even if
   * BT restarts.
   */
  if (mServerSocket &&
      mServerSocket->GetConnectionStatus() != SOCKET_DISCONNECTED) {
    mServerSocket->Close();
  }
  mServerSocket = nullptr;

  mServerSocket = new BluetoothSocket(this);

  nsresult rv = mServerSocket->Listen(
      u"OBEX Object Push"_ns, kObexObjectPush, BluetoothSocketType::RFCOMM,
      BluetoothReservedChannels::CHANNEL_OPUSH, false, true);
  if (NS_FAILED(rv)) {
    BT_WARNING("[OPP] Can't listen on RFCOMM socket!");
    mServerSocket = nullptr;
    return false;
  }

  mIsServer = true;

  return true;
}

void BluetoothOppManager::StartSendingNextFile() {
  MOZ_ASSERT(NS_IsMainThread());

  MOZ_ASSERT(!mBatches.IsEmpty());
  MOZ_ASSERT((int)mBatches[0].mBlobs.Length() > mCurrentBlobIndex + 1);

  mBlob = mBatches[0].mBlobs[++mCurrentBlobIndex];

  // Before sending content, we have to send a header including
  // information such as file name, file length and content type.
  ExtractBlobHeaders();
  StartFileTransfer();

  if (mCurrentBlobIndex == 0) {
    // We may have more than one file waiting for transferring, but only one
    // CONNECT request would be sent. Therefore check if this is the very first
    // file at the head of queue.
    SendConnectRequest();
  } else {
    SendPutHeaderRequest(mFileName, mFileLength);
    AfterFirstPut();
  }
}

bool BluetoothOppManager::SendFile(const BluetoothAddress& aDeviceAddress,
                                   BlobImpl* aBlob) {
  MOZ_ASSERT(NS_IsMainThread());

  AppendBlobToSend(aDeviceAddress, aBlob);
  if (!mSocket) {
    ProcessNextBatch();
  }

  return true;
}

void BluetoothOppManager::AppendBlobToSend(
    const BluetoothAddress& aDeviceAddress, BlobImpl* aBlob) {
  MOZ_ASSERT(NS_IsMainThread());

  int indexTail = mBatches.Length() - 1;

  /**
   * Create a new batch if
   * - mBatches is empty, or
   * - aDeviceAddress differs from mDeviceAddress of the last batch
   */
  if (mBatches.IsEmpty() ||
      aDeviceAddress != mBatches[indexTail].mDeviceAddress) {
    SendFileBatch batch(aDeviceAddress, aBlob);
    mBatches.AppendElement(std::move(batch));
  } else {
    mBatches[indexTail].mBlobs.AppendElement(aBlob);
  }
}

void BluetoothOppManager::DiscardBlobsToSend() {
  MOZ_ASSERT(NS_IsMainThread());

  MOZ_ASSERT(!mBatches.IsEmpty());
  MOZ_ASSERT(!mIsServer);

  int length = (int)mBatches[0].mBlobs.Length();
  while (length > mCurrentBlobIndex + 1) {
    mBlob = mBatches[0].mBlobs[++mCurrentBlobIndex];

    BT_LOGR("idx %d", mCurrentBlobIndex);
    ExtractBlobHeaders();
    StartFileTransfer();
    FileTransferComplete();
  }
}

bool BluetoothOppManager::ProcessNextBatch() {
  MOZ_ASSERT(NS_IsMainThread());
  MOZ_ASSERT(!IsConnected());

  // Remove the processed batch.
  // A batch is processed if we've incremented mCurrentBlobIndex for it.
  if (mCurrentBlobIndex >= 0) {
    ClearQueue();
    mBatches.RemoveElementAt(0);
    BT_LOGR("REMOVE. %zu remaining", mBatches.Length());
  }

  // Process the next batch
  if (!mBatches.IsEmpty()) {
    ConnectInternal(mBatches[0].mDeviceAddress);
    return true;
  }

  // No more batch to process
  return false;
}

void BluetoothOppManager::ClearQueue() {
  MOZ_ASSERT(NS_IsMainThread());

  MOZ_ASSERT(!mIsServer);
  MOZ_ASSERT(!mBatches.IsEmpty());
  MOZ_ASSERT(!mBatches[0].mBlobs.IsEmpty());

  mCurrentBlobIndex = -1;
  mBlob = nullptr;
  mBatches[0].mBlobs.Clear();
}

bool BluetoothOppManager::StopSendingFile() {
  MOZ_ASSERT(NS_IsMainThread());

  if (mIsServer) {
    mAbortFlag = true;
  } else {
    Disconnect(nullptr);
  }

  return true;
}

bool BluetoothOppManager::ConfirmReceivingFile(bool aConfirm) {
  NS_ENSURE_TRUE(mConnected, false);
  NS_ENSURE_TRUE(mWaitingForConfirmationFlag, false);

  mWaitingForConfirmationFlag = false;

  // For the first packet of first file
  bool success = false;
  if (aConfirm) {
    StartFileTransfer();
    if (CreateFile()) {
      success = WriteToFile(mBodySegment.get(), mBodySegmentLength);
    }
  }

  if (success && mPutFinalFlag) {
    mSuccessFlag = true;
    RestoreReceivedFileAndNotify();
    FileTransferComplete();
  }

  ReplyToPut(mPutFinalFlag, success);

  return true;
}

void BluetoothOppManager::AfterFirstPut() {
  mUpdateProgressCounter = 1;
  mPutFinalFlag = false;
  mPutPacketReceivedLength = 0;
  mSentFileLength = 0;
  mWaitingToSendPutFinal = false;
  mSuccessFlag = false;
  mBodySegmentLength = 0;
}

void BluetoothOppManager::AfterOppConnected() {
  MOZ_ASSERT(NS_IsMainThread());

  mConnected = true;
  mAbortFlag = false;
  mWaitingForConfirmationFlag = true;
  AfterFirstPut();
  // Get a mount lock to prevent the sdcard from being shared with
  // the PC while we're doing a OPP file transfer. After OPP transaction
  // were done, the mount lock will be freed.
  if (!AcquireSdcardMountLock()) {
    // If we fail to get a mount lock, abort this transaction
    // Directly sending disconnect-request is better than abort-request
    BT_WARNING("BluetoothOPPManager couldn't get a mount lock!");
    Disconnect(nullptr);
  }
}

void BluetoothOppManager::AfterOppDisconnected() {
  MOZ_ASSERT(NS_IsMainThread());

  mConnected = false;
  mLastCommand = 0;
  mPutPacketReceivedLength = 0;
  mDsFile = nullptr;
  mDummyDsFile = nullptr;

  // We can't reset mSuccessFlag here since this function may be called
  // before we send system message of transfer complete
  // mSuccessFlag = false;

  if (mInputStream) {
    mInputStream->Close();
    mInputStream = nullptr;
  }

  if (mOutputStream) {
    mOutputStream->Close();
    mOutputStream = nullptr;
  }

  // Store local pointer of |mReadFileThread| to avoid shutdown reentry crash
  // See bug 1191715 comment 19 for more details.
  nsCOMPtr<nsIThread> thread = mReadFileThread.forget();
  if (thread) {
    thread->Shutdown();
  }

  // Release the mount lock if file transfer completed
  if (mMountLock) {
    // The mount lock will be implicitly unlocked
    mMountLock = nullptr;
  }
}

void BluetoothOppManager::RestoreReceivedFileAndNotify() {
  // Remove the empty dummy file
  if (mDummyDsFile && mDummyDsFile->mFile) {
    mDummyDsFile->mFile->Remove(false);
    mDummyDsFile = nullptr;
  }

  // Remove the trailing ".part" file name from mDsFile by two steps
  // 1. mDsFile->SetPath() so that the notification sent to Gaia will carry
  //    correct information of the file.
  // 2. mDsFile->mFile->RenameTo() so that the file name would actually be
  //    changed in file system.
  if (mDsFile && mDsFile->mFile) {
    nsString path;
    path.AssignLiteral(TARGET_SUBDIR);
    path.Append(mFileName);

    mDsFile->SetPath(path);
    mDsFile->mFile->RenameTo(nullptr, mFileName);
  }

  // Notify about change of received file
  NotifyAboutFileChange();
}

void BluetoothOppManager::DeleteReceivedFile() {
  if (mOutputStream) {
    mOutputStream->Close();
    mOutputStream = nullptr;
  }

  if (mDsFile && mDsFile->mFile) {
    mDsFile->mFile->Remove(false);
    mDsFile = nullptr;
  }

  // Remove the empty dummy file
  if (mDummyDsFile && mDummyDsFile->mFile) {
    mDummyDsFile->mFile->Remove(false);
    mDummyDsFile = nullptr;
  }
}

bool BluetoothOppManager::CreateFile() {
  MOZ_ASSERT(mPutPacketReceivedLength == mPacketLength);

  // Create one dummy file to be a placeholder for the target file name, and
  // create another file with a meaningless file extension to write the received
  // data. By doing this, we can prevent applications from parsing incomplete
  // data in the middle of the receiving process.
  nsString path;
  path.AssignLiteral(TARGET_SUBDIR);
  path.Append(mFileName);

  // Use an empty dummy file object to occupy the file name, so that after the
  // whole file has been received successfully by using mDsFile, we could just
  // remove mDummyDsFile and rename mDsFile to the file name of mDummyDsFile.
  mDummyDsFile =
      DeviceStorageFile::CreateUnique(path, nsIFile::NORMAL_FILE_TYPE, 0644);
  NS_ENSURE_TRUE(mDummyDsFile, false);

  // The function CreateUnique() may create a file with a different file
  // name from the original mFileName. Therefore we have to retrieve
  // the file name again.
  mDummyDsFile->mFile->GetLeafName(mFileName);

  BT_LOGR("mFileName: %s, mContentType: %s",
          NS_ConvertUTF16toUTF8(mFileName).get(),
          NS_ConvertUTF16toUTF8(mContentType).get());

  // If the filename doesn't include file extension, append a file extension
  // based on the MIME type of OBEX header.
  if (!mContentType.IsEmpty()) {
    // find seperator '.' from the last 6 characters.
    int32_t offset = mFileName.RFindChar('.');

    // whether the file extension is between 1 to 5 characters
    if (offset == kNotFound ||
        static_cast<uint32_t>(offset + 1) == mFileName.Length()) {
      nsCOMPtr<nsIMIMEService> mimeSvc =
          do_GetService(NS_MIMESERVICE_CONTRACTID);
      if (mimeSvc) {
        nsCString extension;
        nsresult rv = mimeSvc->GetPrimaryExtension(
            NS_LossyConvertUTF16toASCII(mContentType), EmptyCString(),
            extension);
        if (NS_SUCCEEDED(rv)) {
          mFileName.Append('.');
          AppendUTF8toUTF16(extension, mFileName);
        }
      }
    }
  }

  // Prepare the entire file path for the .part file
  path.Truncate();
  path.AssignLiteral(TARGET_SUBDIR);
  path.Append(mFileName);
  path.AppendLiteral(".part");

  mDsFile =
      DeviceStorageFile::CreateUnique(path, nsIFile::NORMAL_FILE_TYPE, 0644);
  NS_ENSURE_TRUE(mDsFile, false);

  NS_NewLocalFileOutputStream(getter_AddRefs(mOutputStream), mDsFile->mFile);
  NS_ENSURE_TRUE(mOutputStream, false);

  return true;
}

bool BluetoothOppManager::WriteToFile(const uint8_t* aData, int aDataLength) {
  NS_ENSURE_TRUE(mOutputStream, false);

  uint32_t wrote = 0;
  mOutputStream->Write((const char*)aData, aDataLength, &wrote);
  NS_ENSURE_TRUE(aDataLength == (int)wrote, false);

  return true;
}

// Virtual function of class SocketConsumer
void BluetoothOppManager::ExtractPacketHeaders(const ObexHeaderSet& aHeader) {
  if (aHeader.Has(ObexHeaderId::Name)) {
    aHeader.GetName(mFileName);
  }

  if (aHeader.Has(ObexHeaderId::Type)) {
    aHeader.GetContentType(mContentType);
  }

  if (aHeader.Has(ObexHeaderId::Length)) {
    aHeader.GetLength(&mFileLength);
  }

  if (aHeader.Has(ObexHeaderId::Body) || aHeader.Has(ObexHeaderId::EndOfBody)) {
    uint8_t* bodyPtr;
    aHeader.GetBody(&bodyPtr, &mBodySegmentLength);
    mBodySegment.reset(bodyPtr);
  }
}

bool BluetoothOppManager::ExtractBlobHeaders() {
  RetrieveSentFileName();
  mBlob->GetType(mContentType);

  ErrorResult rv;
  uint64_t fileLength = mBlob->GetSize(rv);
  if (NS_WARN_IF(rv.Failed())) {
    SendDisconnectRequest();
    rv.SuppressException();
    return false;
  }

  // Currently we keep the size of files which were sent/received via
  // Bluetooth not exceed UINT32_MAX because the Length header in OBEX
  // is only 4-byte long. Although it is possible to transfer a file
  // larger than UINT32_MAX, it needs to parse another OBEX Header
  // and I would like to leave it as a feature.
  if (fileLength > (uint64_t)UINT32_MAX) {
    BT_WARNING("The file size is too large for now");
    SendDisconnectRequest();
    return false;
  }

  mFileLength = fileLength;
  rv = NS_NewNamedThread("Opp File Reader", getter_AddRefs(mReadFileThread));
  if (NS_WARN_IF(rv.Failed())) {
    SendDisconnectRequest();
    rv.SuppressException();
    return false;
  }

  return true;
}

void BluetoothOppManager::RetrieveSentFileName() {
  mFileName.Truncate();

  RefPtr<Blob> blob = Blob::Create(nullptr, mBlob);
  RefPtr<File> file = blob.get() ? blob.get()->ToFile() : nullptr;
  if (file) {
    file->GetName(mFileName);
  } else if (mBlob) {
    // If the Blob can't be converted to File, use blob name as filename
    mBlob->GetName(mFileName);
  }

  /**
   * We try our best to get the file extension to avoid interoperability issues.
   * However, once we found that we are unable to get suitable extension or
   * information about the content type, sending a pre-defined file name without
   * extension would be fine.
   */
  if (mFileName.IsEmpty()) {
    mFileName.AssignLiteral("Unknown");
  }

  int32_t offset = mFileName.RFindChar('/');
  if (offset != kNotFound) {
    mFileName = Substring(mFileName, offset + 1);
  }

  // find seperator '.' from the last 6 characters.
  offset = mFileName.RFindChar('.');

  // whether the length of file extension is between 1 to 5
  if (offset == kNotFound ||
      static_cast<uint32_t>(offset + 1) == mFileName.Length()) {
    nsCOMPtr<nsIMIMEService> mimeSvc = do_GetService(NS_MIMESERVICE_CONTRACTID);

    if (mimeSvc) {
      nsString mimeType;
      mBlob->GetType(mimeType);

      nsCString extension;
      nsresult rv = mimeSvc->GetPrimaryExtension(
          NS_LossyConvertUTF16toASCII(mimeType), EmptyCString(), extension);
      if (NS_SUCCEEDED(rv)) {
        mFileName.Append('.');
        AppendUTF8toUTF16(extension, mFileName);
      }
    }
  }
}

bool BluetoothOppManager::IsReservedChar(char16_t c) {
  return (c < 0x0020 || c == char16_t('?') || c == char16_t('|') ||
          c == char16_t('<') || c == char16_t('>') || c == char16_t('"') ||
          c == char16_t(':') || c == char16_t('/') || c == char16_t('*') ||
          c == char16_t('\\'));
}

void BluetoothOppManager::ValidateFileName() {
  int length = mFileName.Length();

  for (int i = 0; i < length; ++i) {
    // Replace reserved char of fat file system with '_'
    if (IsReservedChar(mFileName.CharAt(i))) {
      mFileName.Replace(i, 1, char16_t('_'));
    }
  }
}

bool BluetoothOppManager::ComposePacket(uint8_t aOpCode,
                                        UnixSocketBuffer* aMessage) {
  MOZ_ASSERT(NS_IsMainThread());
  MOZ_ASSERT(aMessage);

  const uint8_t* data = aMessage->GetData();
  int frameHeaderLength = 0;

  // See if this is the first part of each Put packet
  if (mPutPacketReceivedLength == 0) {
    // Section 3.3.3 "Put", IrOBEX 1.2
    // [opcode:1][length:2][Headers:var]
    frameHeaderLength = 3;

    mPacketLength = BigEndian::readUint16(&data[1]) - frameHeaderLength;

    /**
     * A PUT request from remote devices may be divided into multiple parts.
     * In other words, one request may need to be received multiple times,
     * so here we keep a variable mPutPacketReceivedLength to indicate if
     * current PUT request is done.
     */
    mReceivedDataBuffer.reset(new uint8_t[mPacketLength]);
    mPutFinalFlag = (aOpCode == ObexRequestCode::PutFinal);
  }

  int dataLength = aMessage->GetSize() - frameHeaderLength;

  // Check length before memcpy to prevent from memory pollution
  if (dataLength < 0 || mPutPacketReceivedLength + dataLength > mPacketLength) {
    BT_LOGR("Received packet size is unreasonable");

    ReplyToPut(mPutFinalFlag, false);
    DeleteReceivedFile();
    FileTransferComplete();

    return false;
  }

  memcpy(mReceivedDataBuffer.get() + mPutPacketReceivedLength,
         &data[frameHeaderLength], dataLength);

  mPutPacketReceivedLength += dataLength;

  return (mPutPacketReceivedLength == mPacketLength);
}

void BluetoothOppManager::ServerDataHandler(UnixSocketBuffer* aMessage) {
  MOZ_ASSERT(NS_IsMainThread());

  /**
   * Ensure
   * - valid access to data[0], i.e., opCode
   * - received packet length smaller than max packet length
   */
  int receivedLength = aMessage->GetSize();
  if (receivedLength < 1 || receivedLength > MAX_PACKET_LENGTH) {
    ReplyError(ObexResponseCode::BadRequest);
    return;
  }

  uint8_t opCode;
  const uint8_t* data = aMessage->GetData();
  if (mPutPacketReceivedLength > 0) {
    opCode = mPutFinalFlag ? ObexRequestCode::PutFinal : ObexRequestCode::Put;
  } else {
    opCode = data[0];

    // When there's a Put packet right after a PutFinal packet,
    // which means it's the start point of a new file.
    if (mPutFinalFlag && (opCode == ObexRequestCode::Put ||
                          opCode == ObexRequestCode::PutFinal)) {
      mNewFileFlag = true;
      AfterFirstPut();
    }
  }

  ObexHeaderSet pktHeaders;
  if (opCode == ObexRequestCode::Connect) {
    // Section 3.3.1 "Connect", IrOBEX 1.2
    // [opcode:1][length:2][version:1][flags:1][MaxPktSizeWeCanReceive:2]
    // [Headers:var]
    if (receivedLength < 7 ||
        !ParseHeaders(&data[7], receivedLength - 7, &pktHeaders)) {
      ReplyError(ObexResponseCode::BadRequest);
      return;
    }

    ReplyToConnect();
    AfterOppConnected();
  } else if (opCode == ObexRequestCode::Abort) {
    // Section 3.3.5 "Abort", IrOBEX 1.2
    // [opcode:1][length:2][Headers:var]
    if (receivedLength < 3 ||
        !ParseHeaders(&data[3], receivedLength - 3, &pktHeaders)) {
      ReplyError(ObexResponseCode::BadRequest);
      return;
    }

    ReplyToDisconnectOrAbort();
    DeleteReceivedFile();
  } else if (opCode == ObexRequestCode::Disconnect) {
    // Section 3.3.2 "Disconnect", IrOBEX 1.2
    // [opcode:1][length:2][Headers:var]
    if (receivedLength < 3 ||
        !ParseHeaders(&data[3], receivedLength - 3, &pktHeaders)) {
      ReplyError(ObexResponseCode::BadRequest);
      return;
    }

    ReplyToDisconnectOrAbort();
    AfterOppDisconnected();
    FileTransferComplete();
  } else if (opCode == ObexRequestCode::Put ||
             opCode == ObexRequestCode::PutFinal) {
    if (!ComposePacket(opCode, aMessage)) {
      return;
    }

    // A Put packet is received completely
    ParseHeaders(mReceivedDataBuffer.get(), mPutPacketReceivedLength,
                 &pktHeaders);
    ExtractPacketHeaders(pktHeaders);
    ValidateFileName();

    // When we cancel the transfer, delete the file and notify completion
    if (mAbortFlag) {
      ReplyToPut(mPutFinalFlag, false);
      mSentFileLength += mBodySegmentLength;
      DeleteReceivedFile();
      FileTransferComplete();
      return;
    }

    // Wait until get confirmation from user, then create file and write to it
    if (mWaitingForConfirmationFlag) {
      ReceivingFileConfirmation();
      mSentFileLength += mBodySegmentLength;
      return;
    }

    // Already get confirmation from user, create a new file if needed and
    // write to output stream
    if (mNewFileFlag) {
      StartFileTransfer();
      if (!CreateFile()) {
        ReplyToPut(mPutFinalFlag, false);
        return;
      }
      mNewFileFlag = false;
    }

    if (!WriteToFile(mBodySegment.get(), mBodySegmentLength)) {
      ReplyToPut(mPutFinalFlag, false);
      return;
    }

    ReplyToPut(mPutFinalFlag, true);

    // Send progress update
    mSentFileLength += mBodySegmentLength;
    if (mSentFileLength > kUpdateProgressBase * mUpdateProgressCounter) {
      UpdateProgress();
      mUpdateProgressCounter = mSentFileLength / kUpdateProgressBase + 1;
    }

    // Success to receive a file and notify completion
    if (mPutFinalFlag) {
      mSuccessFlag = true;
      RestoreReceivedFileAndNotify();
      FileTransferComplete();
    }
  } else if (opCode == ObexRequestCode::Get ||
             opCode == ObexRequestCode::GetFinal ||
             opCode == ObexRequestCode::SetPath) {
    ReplyError(ObexResponseCode::BadRequest);
    BT_WARNING("Unsupported ObexRequestCode");
  } else {
    ReplyError(ObexResponseCode::NotImplemented);
    BT_WARNING("Unrecognized ObexRequestCode");
  }
}

void BluetoothOppManager::ClientDataHandler(UnixSocketBuffer* aMessage) {
  MOZ_ASSERT(NS_IsMainThread());

  // Ensure valid access to data[0], i.e., opCode
  int receivedLength = aMessage->GetSize();
  if (receivedLength < 1) {
    BT_LOGR("Receive empty response packet");
    return;
  }

  const uint8_t* data = aMessage->GetData();
  uint8_t opCode = data[0];

  if (opCode == 0) {
    BT_LOGR("Receive invalid OBEX response code 0x%X", opCode);
    return;
  }

  // Check response code and send out system message as finished if the response
  // code is somehow incorrect.
  uint8_t expectedOpCode = ObexResponseCode::Success;
  if (mLastCommand == ObexRequestCode::Put) {
    expectedOpCode = ObexResponseCode::Continue;
  }

  if (opCode != expectedOpCode) {
    if (mLastCommand == ObexRequestCode::Put ||
        mLastCommand == ObexRequestCode::Abort ||
        mLastCommand == ObexRequestCode::PutFinal) {
      SendDisconnectRequest();
    }
    nsAutoCString str;
    str += "[OPP] 0x";
    str.AppendInt(mLastCommand, 16);
    str += " failed";
    BT_WARNING("%s", str.get());
    FileTransferComplete();
    return;
  }

  if (mLastCommand == ObexRequestCode::PutFinal) {
    mSuccessFlag = true;
    FileTransferComplete();

    if (mInputStream) {
      mInputStream->Close();
      mInputStream = nullptr;
    }

    if (mCurrentBlobIndex + 1 == (int)mBatches[0].mBlobs.Length()) {
      SendDisconnectRequest();
    } else {
      StartSendingNextFile();
    }
  } else if (mLastCommand == ObexRequestCode::Abort) {
    SendDisconnectRequest();
    FileTransferComplete();
  } else if (mLastCommand == ObexRequestCode::Disconnect) {
    AfterOppDisconnected();
    // Most devices will directly terminate connection after receiving
    // Disconnect request, so we make a delay here. If the socket hasn't been
    // disconnected, we will close it.
    if (mSocket) {
      RefPtr<CloseSocketTask> task = new CloseSocketTask(mSocket);
      MessageLoop::current()->PostDelayedTask(task.forget(), 1000);
    }
  } else if (mLastCommand == ObexRequestCode::Connect) {
    MOZ_ASSERT(!mFileName.IsEmpty());
    MOZ_ASSERT(mBlob);

    AfterOppConnected();

    // Ensure valid access to remote information
    if (receivedLength < 7) {
      BT_LOGR("The length of connect response packet is invalid");
      SendDisconnectRequest();
      return;
    }

    // Keep remote information
    mRemoteObexVersion = data[3];
    mRemoteConnectionFlags = data[4];
    mRemoteMaxPacketLength = BigEndian::readUint16(&data[5]);

    // The length of file name exceeds maximum length.
    int fileNameByteLen = (mFileName.Length() + 1) * 2;
    int headerLen = kPutRequestHeaderSize + kPutRequestAppendHeaderSize;
    if (fileNameByteLen > mRemoteMaxPacketLength - headerLen) {
      BT_WARNING("The length of file name is aberrant.");
      SendDisconnectRequest();
      return;
    }

    SendPutHeaderRequest(mFileName, mFileLength);
  } else if (mLastCommand == ObexRequestCode::Put) {
    if (mWaitingToSendPutFinal) {
      SendPutFinalRequest();
      return;
    }

    if (kUpdateProgressBase * mUpdateProgressCounter < mSentFileLength) {
      UpdateProgress();
      mUpdateProgressCounter = mSentFileLength / kUpdateProgressBase + 1;
    }

    ErrorResult rv;
    if (!mInputStream) {
      mBlob->CreateInputStream(getter_AddRefs(mInputStream), rv);
      if (NS_WARN_IF(rv.Failed())) {
        SendDisconnectRequest();
        rv.SuppressException();
        return;
      }
    }

    RefPtr<ReadFileTask> task =
        new ReadFileTask(mInputStream, mRemoteMaxPacketLength);
    rv = mReadFileThread->Dispatch(task, NS_DISPATCH_NORMAL);
    if (NS_WARN_IF(rv.Failed())) {
      SendDisconnectRequest();
      rv.SuppressException();
      return;
    }
  } else {
    BT_WARNING("Unhandled ObexRequestCode");
  }
}

// Virtual function of class SocketConsumer
void BluetoothOppManager::ReceiveSocketData(
    BluetoothSocket* aSocket, UniquePtr<UnixSocketBuffer>& aBuffer) {
  if (mIsServer) {
    ServerDataHandler(aBuffer.get());
  } else {
    ClientDataHandler(aBuffer.get());
  }
}

void BluetoothOppManager::SendConnectRequest() {
  if (mConnected) {
    return;
  }

  // Section 3.3.1 "Connect", IrOBEX 1.2
  // [opcode:1][length:2][version:1][flags:1][MaxPktSizeWeCanReceive:2]
  // [Headers:var]
  uint8_t req[255];
  int index = 7;

  req[3] = 0x10;  // version=1.0
  req[4] = 0x00;  // flag=0x00
  req[5] = BluetoothOppManager::MAX_PACKET_LENGTH >> 8;
  req[6] = (uint8_t)BluetoothOppManager::MAX_PACKET_LENGTH;

  SendObexData(req, ObexRequestCode::Connect, index);
}

void BluetoothOppManager::SendPutHeaderRequest(const nsAString& aFileName,
                                               int aFileSize) {
  if (!mConnected) {
    return;
  }

  int len = aFileName.Length();
  auto fileName = MakeUnique<uint8_t[]>((len + 1) * 2);

  for (int i = 0; i < len; i++) {
    BigEndian::writeUint16(&fileName[i * 2], aFileName[i]);
  }
  BigEndian::writeUint16(&fileName[len * 2], 0);

  // Prepare packet
  auto req = MakeUnique<uint8_t[]>(mRemoteMaxPacketLength);
  int index = 3;
  index += AppendHeaderName(&req[index], mRemoteMaxPacketLength - index,
                            fileName.get(), (len + 1) * 2);
  index += AppendHeaderLength(&req[index], aFileSize);

  // This is final put packet if file size equals to 0
  uint8_t opcode =
      (aFileSize > 0) ? ObexRequestCode::Put : ObexRequestCode::PutFinal;
  SendObexData(std::move(req), opcode, index);
}

void BluetoothOppManager::SendPutRequest(uint8_t* aFileBody,
                                         int aFileBodyLength) {
  if (!mConnected) {
    return;
  }

  int packetLeftSpace = mRemoteMaxPacketLength - kPutRequestHeaderSize;
  if (aFileBodyLength > packetLeftSpace) {
    BT_WARNING("Not allowed such a small MaxPacketLength value");
    return;
  }

  // Section 3.3.3 "Put", IrOBEX 1.2
  // [opcode:1][length:2][Headers:var]
  auto req = MakeUnique<uint8_t[]>(mRemoteMaxPacketLength);

  int index = 3;
  index += AppendHeaderBody(&req[index], mRemoteMaxPacketLength - index,
                            aFileBody, aFileBodyLength);

  SendObexData(std::move(req), ObexRequestCode::Put, index);

  mSentFileLength += aFileBodyLength;
}

void BluetoothOppManager::SendPutFinalRequest() {
  if (!mConnected) {
    return;
  }

  /**
   * Section 2.2.9, "End-of-Body", IrObex 1.2
   * End-of-Body is used to identify the last chunk of the object body.
   * For most platforms, a PutFinal packet is sent with an zero length
   * End-of-Body header.
   */

  // [opcode:1][length:2]
  int index = 3;
  auto req = MakeUnique<uint8_t[]>(mRemoteMaxPacketLength);
  index += AppendHeaderEndOfBody(&req[index]);

  SendObexData(std::move(req), ObexRequestCode::PutFinal, index);

  mWaitingToSendPutFinal = false;
}

void BluetoothOppManager::SendDisconnectRequest() {
  if (!mConnected) {
    return;
  }

  // Section 3.3.2 "Disconnect", IrOBEX 1.2
  // [opcode:1][length:2][Headers:var]
  uint8_t req[255];
  int index = 3;

  SendObexData(req, ObexRequestCode::Disconnect, index);
}

void BluetoothOppManager::CheckPutFinal(uint32_t aNumRead) {
  if (mSentFileLength + aNumRead >= mFileLength) {
    mWaitingToSendPutFinal = true;
  }
}

bool BluetoothOppManager::IsConnected() { return mConnected; }

bool BluetoothOppManager::ReplyToConnectionRequest(bool aAccept) {
  MOZ_ASSERT(false,
             "BluetoothOppManager hasn't implemented this function yet.");
  return false;
}

void BluetoothOppManager::GetAddress(BluetoothAddress& aDeviceAddress) {
  return mSocket->GetAddress(aDeviceAddress);
}

void BluetoothOppManager::ReplyToConnect() {
  if (mConnected) {
    return;
  }

  // Section 3.3.1 "Connect", IrOBEX 1.2
  // [opcode:1][length:2][version:1][flags:1][MaxPktSizeWeCanReceive:2]
  // [Headers:var]
  uint8_t req[255];
  int index = 7;

  req[3] = 0x10;  // version=1.0
  req[4] = 0x00;  // flag=0x00
  req[5] = BluetoothOppManager::MAX_PACKET_LENGTH >> 8;
  req[6] = (uint8_t)BluetoothOppManager::MAX_PACKET_LENGTH;

  SendObexData(req, ObexResponseCode::Success, index);
}

void BluetoothOppManager::ReplyToDisconnectOrAbort() {
  if (!mConnected) {
    return;
  }

  // Section 3.3.2 "Disconnect" and Section 3.3.5 "Abort", IrOBEX 1.2
  // The format of response packet of "Disconnect" and "Abort" are the same
  // [opcode:1][length:2][Headers:var]
  uint8_t req[255];
  int index = 3;

  SendObexData(req, ObexResponseCode::Success, index);
}

void BluetoothOppManager::ReplyToPut(bool aFinal, bool aContinue) {
  if (!mConnected) {
    return;
  }

  // The received length can be reset here because this is where we reply to a
  // complete put packet.
  mPutPacketReceivedLength = 0;

  // Section 3.3.2 "Disconnect", IrOBEX 1.2
  // [opcode:1][length:2][Headers:var]
  uint8_t req[255];
  int index = 3;
  uint8_t opcode;

  if (aContinue) {
    opcode = (aFinal) ? ObexResponseCode::Success : ObexResponseCode::Continue;
  } else {
    opcode = (aFinal) ? ObexResponseCode::Unauthorized
                      : ObexResponseCode::Unauthorized & (~FINAL_BIT);
  }

  SendObexData(req, opcode, index);
}

void BluetoothOppManager::ReplyError(uint8_t aError) {
  BT_LOGR("error: %d", aError);

  // Section 3.2 "Response Format", IrOBEX 1.2
  // [opcode:1][length:2][Headers:var]
  uint8_t req[255];
  int index = 3;

  SendObexData(req, aError, index);
}

void BluetoothOppManager::SendObexData(uint8_t* aData, uint8_t aOpcode,
                                       int aSize) {
  if (!mIsServer) {
    mLastCommand = aOpcode;
  }

  SetObexPacketInfo(aData, aOpcode, aSize);
  mSocket->SendSocketData(new UnixSocketRawData(aData, aSize));
}

void BluetoothOppManager::SendObexData(UniquePtr<uint8_t[]> aData,
                                       uint8_t aOpcode, int aSize) {
  if (!mIsServer) {
    mLastCommand = aOpcode;
  }

  SetObexPacketInfo(aData.get(), aOpcode, aSize);
  mSocket->SendSocketData(new UnixSocketRawData(std::move(aData), aSize));
}

void BluetoothOppManager::FileTransferComplete() {
  if (mTransferCompleteFlag) {
    return;
  }

  constexpr auto type = u"bluetooth-opp-transfer-complete"_ns;
  nsTArray<BluetoothNamedValue> parameters;

  AppendNamedValue(parameters, "address", mDeviceAddress);
  AppendNamedValue(parameters, "success", mSuccessFlag);
  AppendNamedValue(parameters, "received", mIsServer);
  AppendNamedValue(parameters, "fileName", mFileName);
  AppendNamedValue(parameters, "fileLength", mSentFileLength);
  AppendNamedValue(parameters, "contentType", mContentType);

  BT_ENSURE_TRUE_VOID_BROADCAST_SYSMSG(type, parameters);

  mTransferCompleteFlag = true;
  mContentType = EmptyString();
}

void BluetoothOppManager::StartFileTransfer() {
  constexpr auto type = u"bluetooth-opp-transfer-start"_ns;
  nsTArray<BluetoothNamedValue> parameters;

  AppendNamedValue(parameters, "address", mDeviceAddress);
  AppendNamedValue(parameters, "received", mIsServer);
  AppendNamedValue(parameters, "fileName", mFileName);
  AppendNamedValue(parameters, "fileLength", mFileLength);
  AppendNamedValue(parameters, "contentType", mContentType);

  BT_ENSURE_TRUE_VOID_BROADCAST_SYSMSG(type, parameters);

  mTransferCompleteFlag = false;
}

void BluetoothOppManager::UpdateProgress() {
  constexpr auto type = u"bluetooth-opp-update-progress"_ns;
  nsTArray<BluetoothNamedValue> parameters;

  AppendNamedValue(parameters, "address", mDeviceAddress);
  AppendNamedValue(parameters, "received", mIsServer);
  AppendNamedValue(parameters, "processedLength", mSentFileLength);
  AppendNamedValue(parameters, "fileLength", mFileLength);

  BT_ENSURE_TRUE_VOID_BROADCAST_SYSMSG(type, parameters);
}

void BluetoothOppManager::ReceivingFileConfirmation() {
  constexpr auto type = u"bluetooth-opp-receiving-file-confirmation"_ns;
  nsTArray<BluetoothNamedValue> parameters;

  AppendNamedValue(parameters, "address", mDeviceAddress);
  AppendNamedValue(parameters, "fileName", mFileName);
  AppendNamedValue(parameters, "fileLength", mFileLength);
  AppendNamedValue(parameters, "contentType", mContentType);

  BT_ENSURE_TRUE_VOID_BROADCAST_SYSMSG(type, parameters);

  mTransferCompleteFlag = false;
}

void BluetoothOppManager::NotifyAboutFileChange() {
  constexpr auto data = u"modified"_ns;

  nsCOMPtr<nsIObserverService> obs = mozilla::services::GetObserverService();
  NS_ENSURE_TRUE_VOID(obs);

  BT_LOGR("Notify observers about the modified file via 'file-watcher-notify'");
  obs->NotifyObservers(mDsFile, "file-watcher-notify", data.get());
}

void BluetoothOppManager::OnSocketConnectSuccess(BluetoothSocket* aSocket) {
  BT_LOGR("[%s]", (mIsServer) ? "server" : "client");
  MOZ_ASSERT(aSocket);

  /**
   * If the created connection is an inbound connection, close server socket
   * because currently only one file-transfer session is allowed. After that,
   * we need to make sure that server socket would be nulled out.
   * As for outbound connections, we just notify the controller that it's done.
   */
  if (aSocket == mServerSocket) {
    MOZ_ASSERT(!mSocket);
    mServerSocket.swap(mSocket);
  }

  // Cache device address since we can't get socket address when a remote
  // device disconnect with us.
  mSocket->GetAddress(mDeviceAddress);

  // Start sending file if we connect as a client
  if (!mIsServer) {
    StartSendingNextFile();
  }
}

void BluetoothOppManager::OnSocketConnectError(BluetoothSocket* aSocket) {
  BT_LOGR("[%s]", (mIsServer) ? "server" : "client");

  if (mServerSocket &&
      mServerSocket->GetConnectionStatus() != SOCKET_DISCONNECTED) {
    mServerSocket->Close();
  }
  mServerSocket = nullptr;

  if (mSocket && mSocket->GetConnectionStatus() != SOCKET_DISCONNECTED) {
    mSocket->Close();
  }
  mSocket = nullptr;

  if (!mIsServer) {
    // Inform gaia of remaining blobs' sending failure
    DiscardBlobsToSend();
  }

  // Listen as a server if there's no more batch to process
  if (!ProcessNextBatch()) {
    Listen();
  }
}

void BluetoothOppManager::OnSocketDisconnect(BluetoothSocket* aSocket) {
  MOZ_ASSERT(aSocket);
  if (aSocket != mSocket) {
    // Do nothing when a listening server socket is closed.
    return;
  }
  BT_LOGR("[%s]", (mIsServer) ? "server" : "client");

  /**
   * It is valid for a bluetooth device which is transfering file via OPP
   * closing socket without sending OBEX disconnect request first. So we
   * delete the broken file when we failed to receive a file from the remote,
   * and notify the transfer has been completed (but failed). We also call
   * AfterOppDisconnected here to ensure all variables will be cleaned.
   */
  if (!mSuccessFlag) {
    if (mIsServer) {
      // Remove received file and inform gaia of receiving failure
      DeleteReceivedFile();
      FileTransferComplete();
    } else {
      // Inform gaia of current blob transfer failure
      if (mCurrentBlobIndex >= 0) {
        FileTransferComplete();
      }
      // Inform gaia of remaining blobs' sending failure
      DiscardBlobsToSend();
    }
  }

  AfterOppDisconnected();
  mDeviceAddress.Clear();
  mSuccessFlag = false;

  mSocket = nullptr;  // should already be closed

  // Listen as a server if there's no more batch to process
  if (!ProcessNextBatch()) {
    Listen();
  }
}

void BluetoothOppManager::Disconnect(BluetoothProfileController* aController) {
  if (mSocket) {
    SendDisconnectRequest();

    mSocket->Close();
  } else {
    BT_WARNING("%s: No ongoing file transfer to stop", __FUNCTION__);
  }
}

NS_IMPL_ISUPPORTS(BluetoothOppManager, nsIObserver)

bool BluetoothOppManager::AcquireSdcardMountLock() {
  nsCOMPtr<nsIVolumeService> volumeSrv =
      do_GetService(NS_VOLUMESERVICE_CONTRACTID);
  NS_ENSURE_TRUE(volumeSrv, false);

  NS_ENSURE_SUCCESS(
      volumeSrv->CreateMountLock(u"sdcard"_ns, getter_AddRefs(mMountLock)),
      false);

  return true;
}

void BluetoothOppManager::OnGetServiceChannel(
    const BluetoothAddress& aDeviceAddress, const BluetoothUuid& aServiceUuid,
    int aChannel) {
  if (aChannel < 0) {
    BluetoothService* bs = BluetoothService::Get();
    if (!bs || sInShutdown) {
      OnSocketConnectError(mSocket);
      return;
    }

    if (mNeedsUpdatingSdpRecords) {
      mNeedsUpdatingSdpRecords = false;
      mLastServiceChannelCheck = TimeStamp::Now();
    } else {
      // Refresh SDP records until it gets valid service channel
      // unless timeout is hit.
      TimeDuration duration = TimeStamp::Now() - mLastServiceChannelCheck;
      if (duration.ToMilliseconds() >= kSdpUpdatingTimeoutMs) {
        OnSocketConnectError(mSocket);
        return;
      }
    }

    if (!bs->UpdateSdpRecords(aDeviceAddress, this)) {
      OnSocketConnectError(mSocket);
      return;
    }

    return;  // We update the service records before we connect.
  }

  mSocket->Connect(aDeviceAddress, aServiceUuid, BluetoothSocketType::RFCOMM,
                   aChannel, false, true);
}

void BluetoothOppManager::OnUpdateSdpRecords(
    const BluetoothAddress& aDeviceAddress) {
  BluetoothService* bs = BluetoothService::Get();
  if (!bs || sInShutdown) {
    OnSocketConnectError(mSocket);
    return;
  }

  if (NS_FAILED(bs->GetServiceChannel(aDeviceAddress, kObexObjectPush, this))) {
    OnSocketConnectError(mSocket);
    return;
  }
}

void BluetoothOppManager::Connect(const BluetoothAddress& aDeviceAddress,
                                  BluetoothProfileController* aController) {
  MOZ_ASSERT(false);
}

void BluetoothOppManager::OnConnect(const nsAString& aErrorStr) {
  MOZ_ASSERT(false);
}

void BluetoothOppManager::OnDisconnect(const nsAString& aErrorStr) {
  MOZ_ASSERT(false);
}

void BluetoothOppManager::Reset() { MOZ_ASSERT(false); }

END_BLUETOOTH_NAMESPACE
