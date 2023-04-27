/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=8 sts=2 et sw=2 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "nsRilWorker.h"
#include "mozilla/Preferences.h"

/* Logging related */
#if !defined(RILWORKER_LOG_TAG)
#  define RILWORKER_LOG_TAG "RilWorker"
#endif

#undef INFO
#undef ERROR
#undef DEBUG
#define INFO(args...) \
  __android_log_print(ANDROID_LOG_INFO, RILWORKER_LOG_TAG, ##args)
#define ERROR(args...) \
  __android_log_print(ANDROID_LOG_ERROR, RILWORKER_LOG_TAG, ##args)
#define ERROR_NS_OK(args...)                                           \
  {                                                                    \
    __android_log_print(ANDROID_LOG_ERROR, RILWORKER_LOG_TAG, ##args); \
    return NS_OK;                                                      \
  }
#define DEBUG(args...)                                                   \
  do {                                                                   \
    if (gRilDebug_isLoggingEnabled) {                                    \
      __android_log_print(ANDROID_LOG_DEBUG, RILWORKER_LOG_TAG, ##args); \
    }                                                                    \
  } while (0)

NS_IMPL_ISUPPORTS(nsRilWorker, nsIRilWorker)
static hidl_string HIDL_SERVICE_NAME[3] = {"slot1", "slot2", "slot3"};

/**
 *
 */
nsRilWorker::nsRilWorker(uint32_t aClientId) {
  DEBUG("init nsRilWorker");
  mRadioProxy = nullptr;
  mDeathRecipient = nullptr;
  mRilCallback = nullptr;
  mClientId = aClientId;
  mRilResponse = new nsRilResponse(this);
  mRilIndication = new nsRilIndication(this);
  updateDebug();
}

void nsRilWorker::updateDebug() {
  gRilDebug_isLoggingEnabled =
      mozilla::Preferences::GetBool("ril.debugging.enabled", false);
}

/**
 * nsIRadioInterface implementation
 */
NS_IMETHODIMP nsRilWorker::SendRilRequest(JS::HandleValue message) {
  return NS_OK;
}

NS_IMETHODIMP nsRilWorker::InitRil(nsIRilCallback* callback) {
  mRilCallback = callback;
  GetRadioProxy();
  return NS_OK;
}

NS_IMETHODIMP nsRilWorker::SetRadioPower(int32_t serial, bool enabled) {
  DEBUG("nsRilWorker: [%d] > RIL_REQUEST_RADIO_POWER on = %d", serial, enabled);

  GetRadioProxy();
  if (mRadioProxy == nullptr) {
    ERROR_NS_OK("No Radio HAL exist");
  }
  Return<void> ret = mRadioProxy->setRadioPower(serial, enabled);

  if (!ret.isOk()) {
    ERROR_NS_OK("SetRadioPower Error.");
  }

  return NS_OK;
}

NS_IMETHODIMP nsRilWorker::GetDeviceIdentity(int32_t serial) {
  DEBUG("nsRilWorker: [%d] > RIL_REQUEST_DEVICE_IDENTITY", serial);

  GetRadioProxy();
  if (mRadioProxy == nullptr) {
    ERROR_NS_OK("No Radio HAL exist");
  }
  Return<void> ret = mRadioProxy->getDeviceIdentity(serial);

  if (!ret.isOk()) {
    ERROR_NS_OK("getDeviceIdentity Error.");
  }

  return NS_OK;
}

NS_IMETHODIMP nsRilWorker::GetVoiceRegistrationState(int32_t serial) {
  DEBUG("nsRilWorker: [%d] > RIL_REQUEST_VOICE_REGISTRATION_STATE", serial);

  GetRadioProxy();
  if (mRadioProxy == nullptr) {
    ERROR_NS_OK("No Radio HAL exist");
  }
  Return<void> ret = mRadioProxy->getVoiceRegistrationState(serial);

  if (!ret.isOk()) {
    ERROR_NS_OK("getVoiceRegistrationState Error.");
  }

  return NS_OK;
}

NS_IMETHODIMP nsRilWorker::GetDataRegistrationState(int32_t serial) {
  DEBUG("nsRilWorker: [%d] > RIL_REQUEST_DATA_REGISTRATION_STATE", serial);

  GetRadioProxy();
  if (mRadioProxy == nullptr) {
    ERROR_NS_OK("No Radio HAL exist");
  }
  Return<void> ret = mRadioProxy->getDataRegistrationState(serial);

  if (!ret.isOk()) {
    ERROR_NS_OK("getDataRegistrationState Error.");
  }

  return NS_OK;
}

NS_IMETHODIMP nsRilWorker::GetOperator(int32_t serial) {
  DEBUG("nsRilWorker: [%d] > RIL_REQUEST_OPERATOR", serial);

  GetRadioProxy();
  if (mRadioProxy == nullptr) {
    ERROR_NS_OK("No Radio HAL exist");
  }
  Return<void> ret = mRadioProxy->getOperator(serial);

  if (!ret.isOk()) {
    ERROR_NS_OK("getOperator Error.");
  }

  return NS_OK;
}

NS_IMETHODIMP nsRilWorker::GetNetworkSelectionMode(int32_t serial) {
  DEBUG("nsRilWorker: [%d] > RIL_REQUEST_QUERY_NETWORK_SELECTION_MODE", serial);

  GetRadioProxy();
  if (mRadioProxy == nullptr) {
    ERROR_NS_OK("No Radio HAL exist");
  }
  Return<void> ret = mRadioProxy->getNetworkSelectionMode(serial);

  if (!ret.isOk()) {
    ERROR_NS_OK("getNetworkSelectionMode Error.");
  }

  return NS_OK;
}

NS_IMETHODIMP nsRilWorker::GetSignalStrength(int32_t serial) {
  DEBUG("nsRilWorker: [%d] > RIL_REQUEST_SIGNAL_STRENGTH", serial);

  GetRadioProxy();
  if (mRadioProxy == nullptr) {
    ERROR_NS_OK("No Radio HAL exist");
  }
  Return<void> ret = mRadioProxy->getSignalStrength(serial);

  if (!ret.isOk()) {
    ERROR_NS_OK("getSignalStrength Error.");
  }

  return NS_OK;
}

NS_IMETHODIMP nsRilWorker::GetVoiceRadioTechnology(int32_t serial) {
  DEBUG("nsRilWorker: [%d] > RIL_REQUEST_VOICE_RADIO_TECH", serial);

  GetRadioProxy();
  if (mRadioProxy == nullptr) {
    ERROR_NS_OK("No Radio HAL exist");
  }
  Return<void> ret = mRadioProxy->getVoiceRadioTechnology(serial);

  if (!ret.isOk()) {
    ERROR_NS_OK("getVoiceRadioTechnology Error.");
  }

  return NS_OK;
}

NS_IMETHODIMP nsRilWorker::GetIccCardStatus(int32_t serial) {
  DEBUG("nsRilWorker: [%d] > RIL_REQUEST_GET_SIM_STATUS", serial);

  GetRadioProxy();
  if (mRadioProxy == nullptr) {
    ERROR_NS_OK("No Radio HAL exist");
  }
  Return<void> ret = mRadioProxy->getIccCardStatus(serial);

  if (!ret.isOk()) {
    ERROR_NS_OK("getIccCardStatus Error.");
  }

  return NS_OK;
}

NS_IMETHODIMP nsRilWorker::ReportSmsMemoryStatus(int32_t serial,
                                                 bool available) {
  DEBUG(
      "nsRilWorker: [%d] > RIL_REQUEST_REPORT_SMS_MEMORY_STATUS available = %d",
      serial, available);

  GetRadioProxy();
  if (mRadioProxy == nullptr) {
    ERROR_NS_OK("No Radio HAL exist");
  }
  Return<void> ret = mRadioProxy->reportSmsMemoryStatus(serial, available);

  if (!ret.isOk()) {
    ERROR_NS_OK("reportSmsMemoryStatus Error.");
  }

  return NS_OK;
}

NS_IMETHODIMP nsRilWorker::SetCellInfoListRate(int32_t serial,
                                               int32_t rateInMillis) {
  DEBUG(
      "nsRilWorker: [%d] > RIL_REQUEST_SET_CELL_INFO_LIST_RATE rateInMillis = "
      "%d",
      serial, rateInMillis);

  GetRadioProxy();
  if (mRadioProxy == nullptr) {
    ERROR_NS_OK("No Radio HAL exist");
  }

  if (rateInMillis == 0) {
    rateInMillis = INT32_MAX;
  }
  Return<void> ret = mRadioProxy->setCellInfoListRate(serial, rateInMillis);

  if (!ret.isOk()) {
    ERROR_NS_OK("setCellInfoListRate Error.");
  }

  return NS_OK;
}

NS_IMETHODIMP nsRilWorker::SetDataAllowed(int32_t serial, bool allowed) {
  DEBUG("nsRilWorker: [%d] > RIL_REQUEST_ALLOW_DATA allowed = %d", serial,
        allowed);

  GetRadioProxy();
  if (mRadioProxy == nullptr) {
    ERROR_NS_OK("No Radio HAL exist");
  }
  Return<void> ret = mRadioProxy->setDataAllowed(serial, allowed);

  if (!ret.isOk()) {
    ERROR_NS_OK("setDataAllowed Error.");
  }

  return NS_OK;
}

NS_IMETHODIMP nsRilWorker::GetBasebandVersion(int32_t serial) {
  DEBUG("nsRilWorker: [%d] > RIL_REQUEST_BASEBAND_VERSION", serial);

  GetRadioProxy();
  if (mRadioProxy == nullptr) {
    ERROR_NS_OK("No Radio HAL exist");
  }
  Return<void> ret = mRadioProxy->getBasebandVersion(serial);

  if (!ret.isOk()) {
    ERROR_NS_OK("getBasebandVersion Error.");
  }

  return NS_OK;
}

NS_IMETHODIMP nsRilWorker::SetUiccSubscription(int32_t serial, int32_t slotId,
                                               int32_t appIndex, int32_t subId,
                                               int32_t subStatus) {
  DEBUG(
      "nsRilWorker: [%d] > RIL_REQUEST_SET_UICC_SUBSCRIPTION slotId = %d "
      "appIndex = %d subId = %d subStatus = %d",
      serial, slotId, appIndex, subId, subStatus);

  GetRadioProxy();
  if (mRadioProxy == nullptr) {
    ERROR_NS_OK("No Radio HAL exist");
  }

  SelectUiccSub info;
  info.slot = slotId;
  info.appIndex = appIndex;
  info.subType = SubscriptionType(subId);
  info.actStatus = UiccSubActStatus(subStatus);

  Return<void> ret = mRadioProxy->setUiccSubscription(serial, info);

  if (!ret.isOk()) {
    ERROR_NS_OK("setUiccSubscription Error.");
  }

  return NS_OK;
}

NS_IMETHODIMP nsRilWorker::SetMute(int32_t serial, bool enableMute) {
  DEBUG("nsRilWorker: [%d] > RIL_REQUEST_SET_MUTE enableMute = %d", serial,
        enableMute);

  GetRadioProxy();
  if (mRadioProxy == nullptr) {
    ERROR_NS_OK("No Radio HAL exist");
  }
  Return<void> ret = mRadioProxy->setMute(serial, enableMute);

  if (!ret.isOk()) {
    ERROR_NS_OK("setMute Error.");
  }

  return NS_OK;
}

NS_IMETHODIMP nsRilWorker::GetMute(int32_t serial) {
  DEBUG("nsRilWorker: [%d] > RIL_REQUEST_GET_MUTE ", serial);

  GetRadioProxy();
  if (mRadioProxy == nullptr) {
    ERROR_NS_OK("No Radio HAL exist");
  }
  Return<void> ret = mRadioProxy->getMute(serial);

  if (!ret.isOk()) {
    ERROR_NS_OK("getMute Error.");
  }

  return NS_OK;
}

NS_IMETHODIMP nsRilWorker::GetSmscAddress(int32_t serial) {
  DEBUG("nsRilWorker: [%d] > RIL_REQUEST_GET_SMSC_ADDRESS", serial);

  GetRadioProxy();
  if (mRadioProxy == nullptr) {
    ERROR_NS_OK("No Radio HAL exist");
  }
  Return<void> ret = mRadioProxy->getSmscAddress(serial);

  if (!ret.isOk()) {
    ERROR_NS_OK("getSmscAddress Error.");
  }

  return NS_OK;
}

NS_IMETHODIMP nsRilWorker::RequestDial(int32_t serial, const nsAString& address,
                                       int32_t clirMode, int32_t uusType,
                                       int32_t uusDcs,
                                       const nsAString& uusData) {
  DEBUG("nsRilWorker: [%d] > RIL_REQUEST_DIAL", serial);

  GetRadioProxy();
  if (mRadioProxy == nullptr) {
    ERROR_NS_OK("No Radio HAL exist");
  }

  UusInfo info;
  std::vector<UusInfo> uusInfo;
  info.uusType = UusType(uusType);
  info.uusDcs = UusDcs(uusDcs);
  if (uusData.Length() == 0) {
    info.uusData = NULL;
  } else {
    info.uusData = NS_ConvertUTF16toUTF8(uusData).get();
  }
  uusInfo.push_back(info);

  Dial dialInfo;
  dialInfo.address = NS_ConvertUTF16toUTF8(address).get();
  dialInfo.clir = Clir(clirMode);
  dialInfo.uusInfo = uusInfo;
  Return<void> ret = mRadioProxy->dial(serial, dialInfo);

  if (!ret.isOk()) {
    ERROR_NS_OK("dial Error.");
  }

  return NS_OK;
}

NS_IMETHODIMP nsRilWorker::GetCurrentCalls(int32_t serial) {
  DEBUG("nsRilWorker: [%d] > RIL_REQUEST_GET_CURRENT_CALLS", serial);

  GetRadioProxy();
  if (mRadioProxy == nullptr) {
    ERROR_NS_OK("No Radio HAL exist");
  }
  Return<void> ret = mRadioProxy->getCurrentCalls(serial);

  if (!ret.isOk()) {
    ERROR_NS_OK("getCurrentCalls Error.");
  }

  return NS_OK;
}

NS_IMETHODIMP nsRilWorker::HangupConnection(int32_t serial, int32_t callIndex) {
  DEBUG("nsRilWorker: [%d] > RIL_REQUEST_HANGUP callIndex = %d", serial,
        callIndex);

  GetRadioProxy();
  if (mRadioProxy == nullptr) {
    ERROR_NS_OK("No Radio HAL exist");
  }
  Return<void> ret = mRadioProxy->hangup(serial, callIndex);

  if (!ret.isOk()) {
    ERROR_NS_OK("hangup Error.");
  }

  return NS_OK;
}

NS_IMETHODIMP nsRilWorker::HangupWaitingOrBackground(int32_t serial) {
  DEBUG("nsRilWorker: [%d] > RIL_REQUEST_HANGUP_WAITING_OR_BACKGROUND", serial);

  GetRadioProxy();
  if (mRadioProxy == nullptr) {
    ERROR_NS_OK("No Radio HAL exist");
  }
  Return<void> ret = mRadioProxy->hangupWaitingOrBackground(serial);

  if (!ret.isOk()) {
    ERROR_NS_OK("hangupWaitingOrBackground Error.");
  }

  return NS_OK;
}

NS_IMETHODIMP nsRilWorker::HangupForegroundResumeBackground(int32_t serial) {
  DEBUG("nsRilWorker: [%d] > RIL_REQUEST_HANGUP_FOREGROUND_RESUME_BACKGROUND",
        serial);

  GetRadioProxy();
  if (mRadioProxy == nullptr) {
    ERROR_NS_OK("No Radio HAL exist");
  }
  Return<void> ret = mRadioProxy->hangupForegroundResumeBackground(serial);

  if (!ret.isOk()) {
    ERROR_NS_OK("hangupForegroundResumeBackground Error.");
  }

  return NS_OK;
}

NS_IMETHODIMP nsRilWorker::SwitchWaitingOrHoldingAndActive(int32_t serial) {
  DEBUG("nsRilWorker: [%d] > RIL_REQUEST_SWITCH_WAITING_OR_HOLDING_AND_ACTIVE",
        serial);

  GetRadioProxy();
  if (mRadioProxy == nullptr) {
    ERROR_NS_OK("No Radio HAL exist");
  }
  Return<void> ret = mRadioProxy->switchWaitingOrHoldingAndActive(serial);

  if (!ret.isOk()) {
    ERROR_NS_OK("switchWaitingOrHoldingAndActive Error.");
  }

  return NS_OK;
}

NS_IMETHODIMP nsRilWorker::Conference(int32_t serial) {
  DEBUG("nsRilWorker: [%d] > RIL_REQUEST_CONFERENCE", serial);

  GetRadioProxy();
  if (mRadioProxy == nullptr) {
    ERROR_NS_OK("No Radio HAL exist");
  }
  Return<void> ret = mRadioProxy->conference(serial);

  if (!ret.isOk()) {
    ERROR_NS_OK("conference Error.");
  }

  return NS_OK;
}

NS_IMETHODIMP nsRilWorker::GetLastCallFailCause(int32_t serial) {
  DEBUG("nsRilWorker: [%d] > RIL_REQUEST_LAST_CALL_FAIL_CAUSE", serial);
  GetRadioProxy();
  if (mRadioProxy == nullptr) {
    ERROR_NS_OK("No Radio HAL exist");
  }
  Return<void> ret = mRadioProxy->getLastCallFailCause(serial);

  if (!ret.isOk()) {
    ERROR_NS_OK("getLastCallFailCause Error.");
  }

  return NS_OK;
}

NS_IMETHODIMP nsRilWorker::AcceptCall(int32_t serial) {
  DEBUG("nsRilWorker: [%d] > RIL_REQUEST_ANSWER", serial);
  GetRadioProxy();
  if (mRadioProxy == nullptr) {
    ERROR_NS_OK("No Radio HAL exist");
  }
  Return<void> ret = mRadioProxy->acceptCall(serial);

  if (!ret.isOk()) {
    ERROR_NS_OK("acceptCall Error.");
  }

  return NS_OK;
}

NS_IMETHODIMP nsRilWorker::SetPreferredNetworkType(int32_t serial,
                                                   int32_t networkType) {
  DEBUG(
      "nsRilWorker: [%d] > RIL_REQUEST_SET_PREFERRED_NETWORK_TYPE "
      "networkType=%d",
      serial, networkType);
  GetRadioProxy();
  if (mRadioProxy == nullptr) {
    ERROR_NS_OK("No Radio HAL exist");
  }
  Return<void> ret = mRadioProxy->setPreferredNetworkType(
      serial, PreferredNetworkType(networkType));

  if (!ret.isOk()) {
    ERROR_NS_OK("setPreferredNetworkType Error.");
  }

  return NS_OK;
}

NS_IMETHODIMP nsRilWorker::GetPreferredNetworkType(int32_t serial) {
  DEBUG("nsRilWorker: [%d] > RIL_REQUEST_GET_PREFERRED_NETWORK_TYPE", serial);
  GetRadioProxy();
  if (mRadioProxy == nullptr) {
    ERROR_NS_OK("No Radio HAL exist");
  }
  Return<void> ret = mRadioProxy->getPreferredNetworkType(serial);

  if (!ret.isOk()) {
    ERROR_NS_OK("getPreferredNetworkType Error.");
  }

  return NS_OK;
}

NS_IMETHODIMP nsRilWorker::SetNetworkSelectionModeAutomatic(int32_t serial) {
  DEBUG("nsRilWorker: [%d] > RIL_REQUEST_SET_NETWORK_SELECTION_AUTOMATIC",
        serial);
  GetRadioProxy();
  if (mRadioProxy == nullptr) {
    ERROR_NS_OK("No Radio HAL exist");
  }
  Return<void> ret = mRadioProxy->setNetworkSelectionModeAutomatic(serial);

  if (!ret.isOk()) {
    ERROR_NS_OK("setNetworkSelectionModeAutomatic Error.");
  }

  return NS_OK;
}

NS_IMETHODIMP nsRilWorker::SetNetworkSelectionModeManual(
    int32_t serial, const nsAString& operatorNumeric) {
  DEBUG(
      "nsRilWorker: [%d] > RIL_REQUEST_SET_NETWORK_SELECTION_MANUAL "
      "operatorNumeric = %s",
      serial, NS_ConvertUTF16toUTF8(operatorNumeric).get());
  GetRadioProxy();
  if (mRadioProxy == nullptr) {
    ERROR_NS_OK("No Radio HAL exist");
  }
  Return<void> ret = mRadioProxy->setNetworkSelectionModeManual(
      serial, NS_ConvertUTF16toUTF8(operatorNumeric).get());

  if (!ret.isOk()) {
    ERROR_NS_OK("setNetworkSelectionModeManual Error.");
  }

  return NS_OK;
}

NS_IMETHODIMP nsRilWorker::GetAvailableNetworks(int32_t serial) {
  DEBUG("nsRilWorker: [%d] > RIL_REQUEST_QUERY_AVAILABLE_NETWORKS", serial);
  GetRadioProxy();
  if (mRadioProxy == nullptr) {
    ERROR_NS_OK("No Radio HAL exist");
  }
  Return<void> ret = mRadioProxy->getAvailableNetworks(serial);

  if (!ret.isOk()) {
    ERROR_NS_OK("getAvailableNetworks Error.");
  }

  return NS_OK;
}

NS_IMETHODIMP nsRilWorker::SetInitialAttachApn(int32_t serial,
                                               nsIDataProfile* profile,
                                               bool isRoaming) {
  DEBUG("nsRilWorker: [%d] > RIL_REQUEST_SET_INITIAL_ATTACH_APN", serial);
  GetRadioProxy();
  if (mRadioProxy == nullptr) {
    ERROR_NS_OK("No Radio HAL exist");
  }

  bool modemCognitive;
  profile->GetModemCognitive(&modemCognitive);

#if RADIO_HAL >= 14
  Return<void> ret = mRadioProxy->setInitialAttachApn_1_4(
      serial, convertToHalDataProfile(profile));
#else
  Return<void> ret = mRadioProxy->setInitialAttachApn(
      serial, convertToHalDataProfile(profile), modemCognitive, isRoaming);
#endif

  if (!ret.isOk()) {
    ERROR_NS_OK("setInitialAttachApn Error.");
  }

  return NS_OK;
}

NS_IMETHODIMP nsRilWorker::SetDataProfile(
    int32_t serial, const nsTArray<RefPtr<nsIDataProfile>>& profileList,
    bool isRoaming) {
  DEBUG("nsRilWorker: [%d] > RIL_REQUEST_SET_DATA_PROFILE", serial);
  GetRadioProxy();
  if (mRadioProxy == nullptr) {
    ERROR_NS_OK("No Radio HAL exist");
  }

  std::vector<DataProfileInfo> dataProfileInfoList;
  for (uint32_t i = 0; i < profileList.Length(); i++) {
    DataProfileInfo profile = convertToHalDataProfile(profileList[i]);
    dataProfileInfoList.push_back(profile);
  }
#if RADIO_HAL >= 14
  Return<void> ret =
      mRadioProxy->setDataProfile_1_4(serial, dataProfileInfoList);
#else
  Return<void> ret =
      mRadioProxy->setDataProfile(serial, dataProfileInfoList, isRoaming);
#endif

  if (!ret.isOk()) {
    ERROR_NS_OK("setDataProfile Error.");
  }

  return NS_OK;
}

NS_IMETHODIMP nsRilWorker::SetupDataCall(int32_t serial,
                                         int32_t radioTechnology,
                                         nsIDataProfile* profile,
                                         bool isRoaming, bool allowRoaming) {
  DEBUG("nsRilWorker: [%d] > RIL_REQUEST_SETUP_DATA_CALL", serial);
  GetRadioProxy();
  if (mRadioProxy == nullptr) {
    ERROR_NS_OK("No Radio HAL exist");
  }

  bool modemCognitive;
  profile->GetModemCognitive(&modemCognitive);

#if RADIO_HAL >= 14
  hidl_vec<hidl_string> addresses;
  hidl_vec<hidl_string> dnses;
  Return<void> ret = mRadioProxy->setupDataCall_1_4(
      serial,
      AccessNetwork::EUTRAN,  // TODO: don't hardcode.
      convertToHalDataProfile(profile), allowRoaming, DataRequestReason::NORMAL,
      addresses, dnses);
#else
  Return<void> ret =
      mRadioProxy->setupDataCall(serial, RadioTechnology(radioTechnology),
                                 convertToHalDataProfile(profile),
                                 modemCognitive, allowRoaming, isRoaming);
#endif

  if (!ret.isOk()) {
    ERROR_NS_OK("setupDataCall Error.");
  }

  return NS_OK;
}

NS_IMETHODIMP nsRilWorker::DeactivateDataCall(int32_t serial, int32_t cid,
                                              int32_t reason) {
  DEBUG("nsRilWorker: [%d] > RIL_REQUEST_DEACTIVATE_DATA_CALL", serial);
  GetRadioProxy();
  if (mRadioProxy == nullptr) {
    ERROR_NS_OK("No Radio HAL exist");
  }

  Return<void> ret = mRadioProxy->deactivateDataCall(
      serial, cid, (reason == 0 ? false : true));

  if (!ret.isOk()) {
    ERROR_NS_OK("deactivateDataCall Error.");
  }

  return NS_OK;
}

NS_IMETHODIMP nsRilWorker::GetDataCallList(int32_t serial) {
  DEBUG("nsRilWorker: [%d] > RIL_REQUEST_DATA_CALL_LIST", serial);
  GetRadioProxy();
  if (mRadioProxy == nullptr) {
    ERROR_NS_OK("No Radio HAL exist");
  }
  Return<void> ret = mRadioProxy->getDataCallList(serial);

  if (!ret.isOk()) {
    ERROR_NS_OK("getDataCallList Error.");
  }

  return NS_OK;
}

NS_IMETHODIMP nsRilWorker::GetCellInfoList(int32_t serial) {
  DEBUG("nsRilWorker: [%d] > RIL_REQUEST_GET_CELL_INFO_LIST", serial);
  GetRadioProxy();
  if (mRadioProxy == nullptr) {
    ERROR_NS_OK("No Radio HAL exist");
  }
  Return<void> ret = mRadioProxy->getCellInfoList(serial);

  if (!ret.isOk()) {
    ERROR_NS_OK("getCellInfoList Error.");
  }

  return NS_OK;
}

NS_IMETHODIMP nsRilWorker::GetIMSI(int32_t serial, const nsAString& aid) {
  DEBUG("nsRilWorker: [%d] > RIL_REQUEST_GET_IMSI aid = %s", serial,
        NS_ConvertUTF16toUTF8(aid).get());
  GetRadioProxy();
  if (mRadioProxy == nullptr) {
    ERROR_NS_OK("No Radio HAL exist");
  }
  Return<void> ret =
      mRadioProxy->getImsiForApp(serial, NS_ConvertUTF16toUTF8(aid).get());

  if (!ret.isOk()) {
    ERROR_NS_OK("getImsiForApp Error.");
  }

  return NS_OK;
}

NS_IMETHODIMP nsRilWorker::IccIOForApp(int32_t serial, int32_t command,
                                       int32_t fileId, const nsAString& path,
                                       int32_t p1, int32_t p2, int32_t p3,
                                       const nsAString& data,
                                       const nsAString& pin2,
                                       const nsAString& aid) {
  DEBUG(
      "nsRilWorker: [%d] > RIL_REQUEST_SIM_IO command = %d, fileId = %d, path "
      "= %s, p1 = %d, p2 = %d, p3 = %d, data = %s, pin2 = %s, aid = %s",
      serial, command, fileId, NS_ConvertUTF16toUTF8(path).get(), p1, p2, p3,
      NS_ConvertUTF16toUTF8(data).get(), NS_ConvertUTF16toUTF8(pin2).get(),
      NS_ConvertUTF16toUTF8(aid).get());
  GetRadioProxy();
  if (mRadioProxy == nullptr) {
    ERROR_NS_OK("No Radio HAL exist");
  }

  IccIo iccIo;
  iccIo.command = command;
  iccIo.fileId = fileId;
  iccIo.path = NS_ConvertUTF16toUTF8(path).get();
  iccIo.p1 = p1;
  iccIo.p2 = p2;
  iccIo.p3 = p3;
  iccIo.data = NS_ConvertUTF16toUTF8(data).get();
  iccIo.pin2 = NS_ConvertUTF16toUTF8(pin2).get();
  iccIo.aid = NS_ConvertUTF16toUTF8(aid).get();

  Return<void> ret = mRadioProxy->iccIOForApp(serial, iccIo);

  if (!ret.isOk()) {
    ERROR_NS_OK("iccIOForApp Error.");
  }

  return NS_OK;
}

NS_IMETHODIMP nsRilWorker::GetClir(int32_t serial) {
  DEBUG("nsRilWorker: [%d] > RIL_REQUEST_GET_CLIR", serial);

  GetRadioProxy();
  if (mRadioProxy == nullptr) {
    ERROR_NS_OK("No Radio HAL exist");
  }
  Return<void> ret = mRadioProxy->getClir(serial);

  if (!ret.isOk()) {
    ERROR_NS_OK("getClir Error.");
  }

  return NS_OK;
}

NS_IMETHODIMP nsRilWorker::SetClir(int32_t serial, int32_t clirMode) {
  DEBUG("nsRilWorker: [%d] > RIL_REQUEST_SET_CLIR clirMode = %d", serial,
        clirMode);

  GetRadioProxy();
  if (mRadioProxy == nullptr) {
    ERROR_NS_OK("No Radio HAL exist");
  }
  Return<void> ret = mRadioProxy->setClir(serial, clirMode);

  if (!ret.isOk()) {
    ERROR_NS_OK("setClir Error.");
  }

  return NS_OK;
}

NS_IMETHODIMP nsRilWorker::SendDtmf(int32_t serial, const nsAString& dtmfChar) {
  DEBUG("nsRilWorker: [%d] > RIL_REQUEST_DTMF dtmfChar = %s", serial,
        NS_ConvertUTF16toUTF8(dtmfChar).get());
  GetRadioProxy();
  if (mRadioProxy == nullptr) {
    ERROR_NS_OK("No Radio HAL exist");
  }
  Return<void> ret =
      mRadioProxy->sendDtmf(serial, NS_ConvertUTF16toUTF8(dtmfChar).get());

  if (!ret.isOk()) {
    ERROR_NS_OK("sendDtmf Error.");
  }

  return NS_OK;
}

NS_IMETHODIMP nsRilWorker::StartDtmf(int32_t serial,
                                     const nsAString& dtmfChar) {
  DEBUG("nsRilWorker: [%d] > RIL_REQUEST_DTMF_START dtmfChar = %s", serial,
        NS_ConvertUTF16toUTF8(dtmfChar).get());
  GetRadioProxy();
  if (mRadioProxy == nullptr) {
    ERROR_NS_OK("No Radio HAL exist");
  }
  Return<void> ret =
      mRadioProxy->startDtmf(serial, NS_ConvertUTF16toUTF8(dtmfChar).get());

  if (!ret.isOk()) {
    ERROR_NS_OK("startDtmf Error.");
  }

  return NS_OK;
}

NS_IMETHODIMP nsRilWorker::StopDtmf(int32_t serial) {
  DEBUG("nsRilWorker: [%d] > RIL_REQUEST_DTMF_STOP", serial);

  GetRadioProxy();
  if (mRadioProxy == nullptr) {
    ERROR_NS_OK("No Radio HAL exist");
  }
  Return<void> ret = mRadioProxy->stopDtmf(serial);

  if (!ret.isOk()) {
    ERROR_NS_OK("stopDtmf Error.");
  }

  return NS_OK;
}

NS_IMETHODIMP nsRilWorker::RejectCall(int32_t serial) {
  DEBUG("nsRilWorker: [%d] > RIL_REQUEST_UDUB", serial);

  GetRadioProxy();
  if (mRadioProxy == nullptr) {
    ERROR_NS_OK("No Radio HAL exist");
  }
  Return<void> ret = mRadioProxy->rejectCall(serial);

  if (!ret.isOk()) {
    ERROR_NS_OK("rejectCall Error.");
  }

  return NS_OK;
}

NS_IMETHODIMP nsRilWorker::SendUssd(int32_t serial, const nsAString& ussd) {
  DEBUG("nsRilWorker: [%d] > RIL_REQUEST_SEND_USSD ussd = %s", serial,
        NS_ConvertUTF16toUTF8(ussd).get());
  GetRadioProxy();
  if (mRadioProxy == nullptr) {
    ERROR_NS_OK("No Radio HAL exist");
  }
  Return<void> ret =
      mRadioProxy->sendUssd(serial, NS_ConvertUTF16toUTF8(ussd).get());

  if (!ret.isOk()) {
    ERROR_NS_OK("sendUssd Error.");
  }

  return NS_OK;
}

NS_IMETHODIMP nsRilWorker::CancelPendingUssd(int32_t serial) {
  DEBUG("nsRilWorker: [%d] > RIL_REQUEST_CANCEL_USSD", serial);
  GetRadioProxy();
  if (mRadioProxy == nullptr) {
    ERROR_NS_OK("No Radio HAL exist");
  }
  Return<void> ret = mRadioProxy->cancelPendingUssd(serial);

  if (!ret.isOk()) {
    ERROR_NS_OK("cancelPendingUssd Error.");
  }

  return NS_OK;
}

NS_IMETHODIMP nsRilWorker::GetCallForwardStatus(int32_t serial,
                                                int32_t cfReason,
                                                int32_t serviceClass,
                                                const nsAString& number,
                                                int32_t toaNumber) {
  DEBUG(
      "nsRilWorker: [%d] > RIL_REQUEST_QUERY_CALL_FORWARD_STATUS cfReason = %d "
      ", serviceClass = %d, number = %s",
      serial, cfReason, serviceClass, NS_ConvertUTF16toUTF8(number).get());
  GetRadioProxy();
  if (mRadioProxy == nullptr) {
    ERROR_NS_OK("No Radio HAL exist");
  }

  CallForwardInfo cfInfo;
  cfInfo.reason = cfReason;
  cfInfo.serviceClass = serviceClass;
  cfInfo.toa = toaNumber;
  cfInfo.number = NS_ConvertUTF16toUTF8(number).get();
  cfInfo.timeSeconds = 0;

  Return<void> ret = mRadioProxy->getCallForwardStatus(serial, cfInfo);

  if (!ret.isOk()) {
    ERROR_NS_OK("getCallForwardStatus Error.");
  }

  return NS_OK;
}

NS_IMETHODIMP nsRilWorker::SetCallForwardStatus(
    int32_t serial, int32_t action, int32_t cfReason, int32_t serviceClass,
    const nsAString& number, int32_t toaNumber, int32_t timeSeconds) {
  DEBUG(
      "nsRilWorker: [%d] > RIL_REQUEST_SET_CALL_FORWARD action = %d, cfReason "
      "= %d , serviceClass = %d, number = %s, timeSeconds = %d",
      serial, action, cfReason, serviceClass,
      NS_ConvertUTF16toUTF8(number).get(), timeSeconds);
  GetRadioProxy();
  if (mRadioProxy == nullptr) {
    ERROR_NS_OK("No Radio HAL exist");
  }

  CallForwardInfo cfInfo;
  cfInfo.status = CallForwardInfoStatus(action);
  cfInfo.reason = cfReason;
  cfInfo.serviceClass = serviceClass;
  cfInfo.toa = toaNumber;
  cfInfo.number = NS_ConvertUTF16toUTF8(number).get();
  cfInfo.timeSeconds = timeSeconds;

  Return<void> ret = mRadioProxy->setCallForward(serial, cfInfo);

  if (!ret.isOk()) {
    ERROR_NS_OK("setCallForward Error.");
  }

  return NS_OK;
}

NS_IMETHODIMP nsRilWorker::GetCallWaiting(int32_t serial,
                                          int32_t serviceClass) {
  DEBUG("nsRilWorker: [%d] > RIL_REQUEST_QUERY_CALL_WAITING serviceClass = %d",
        serial, serviceClass);
  GetRadioProxy();
  if (mRadioProxy == nullptr) {
    ERROR_NS_OK("No Radio HAL exist");
  }
  Return<void> ret = mRadioProxy->getCallWaiting(serial, serviceClass);

  if (!ret.isOk()) {
    ERROR_NS_OK("getCallWaiting Error.");
  }

  return NS_OK;
}

NS_IMETHODIMP nsRilWorker::SetCallWaiting(int32_t serial, bool enable,
                                          int32_t serviceClass) {
  DEBUG(
      "nsRilWorker: [%d] > RIL_REQUEST_SET_CALL_WAITING enable = %d, "
      "serviceClass = %d",
      serial, enable, serviceClass);
  GetRadioProxy();
  if (mRadioProxy == nullptr) {
    ERROR_NS_OK("No Radio HAL exist");
  }
  Return<void> ret = mRadioProxy->setCallWaiting(serial, enable, serviceClass);

  if (!ret.isOk()) {
    ERROR_NS_OK("setCallWaiting Error.");
  }

  return NS_OK;
}

NS_IMETHODIMP nsRilWorker::SetBarringPassword(int32_t serial,
                                              const nsAString& facility,
                                              const nsAString& oldPwd,
                                              const nsAString& newPwd) {
  DEBUG(
      "nsRilWorker: [%d] > RIL_REQUEST_CHANGE_BARRING_PASSWORD facility = %s, "
      "oldPwd = %s, newPwd = %s",
      serial, NS_ConvertUTF16toUTF8(facility).get(),
      NS_ConvertUTF16toUTF8(oldPwd).get(), NS_ConvertUTF16toUTF8(newPwd).get());
  GetRadioProxy();
  if (mRadioProxy == nullptr) {
    ERROR_NS_OK("No Radio HAL exist");
  }
  Return<void> ret = mRadioProxy->setBarringPassword(
      serial, NS_ConvertUTF16toUTF8(facility).get(),
      NS_ConvertUTF16toUTF8(oldPwd).get(), NS_ConvertUTF16toUTF8(newPwd).get());

  if (!ret.isOk()) {
    ERROR_NS_OK("setBarringPassword Error.");
  }

  return NS_OK;
}

NS_IMETHODIMP nsRilWorker::SeparateConnection(int32_t serial,
                                              int32_t gsmIndex) {
  DEBUG("nsRilWorker: [%d] > RIL_REQUEST_SEPARATE_CONNECTION gsmIndex = %d",
        serial, gsmIndex);
  GetRadioProxy();
  if (mRadioProxy == nullptr) {
    ERROR_NS_OK("No Radio HAL exist");
  }
  Return<void> ret = mRadioProxy->separateConnection(serial, gsmIndex);

  if (!ret.isOk()) {
    ERROR_NS_OK("separateConnection Error.");
  }

  return NS_OK;
}

NS_IMETHODIMP nsRilWorker::GetClip(int32_t serial) {
  DEBUG("nsRilWorker: [%d] > RIL_REQUEST_QUERY_CLIP", serial);
  GetRadioProxy();
  if (mRadioProxy == nullptr) {
    ERROR_NS_OK("No Radio HAL exist");
  }
  Return<void> ret = mRadioProxy->getClip(serial);

  if (!ret.isOk()) {
    ERROR_NS_OK("getClip Error.");
  }

  return NS_OK;
}

NS_IMETHODIMP nsRilWorker::ExplicitCallTransfer(int32_t serial) {
  DEBUG("nsRilWorker: [%d] > RIL_REQUEST_EXPLICIT_CALL_TRANSFER", serial);
  GetRadioProxy();
  if (mRadioProxy == nullptr) {
    ERROR_NS_OK("No Radio HAL exist");
  }
  Return<void> ret = mRadioProxy->explicitCallTransfer(serial);

  if (!ret.isOk()) {
    ERROR_NS_OK("explicitCallTransfer Error.");
  }

  return NS_OK;
}

NS_IMETHODIMP nsRilWorker::GetNeighboringCids(int32_t serial) {
  DEBUG("nsRilWorker: [%d] > RIL_REQUEST_GET_NEIGHBORING_CELL_IDS", serial);
  GetRadioProxy();
  if (mRadioProxy == nullptr) {
    ERROR_NS_OK("No Radio HAL exist");
  }
  Return<void> ret = mRadioProxy->getNeighboringCids(serial);

  if (!ret.isOk()) {
    ERROR_NS_OK("getNeighboringCids Error.");
  }

  return NS_OK;
}

NS_IMETHODIMP nsRilWorker::SetTTYMode(int32_t serial, int32_t ttyMode) {
  DEBUG("nsRilWorker: [%d] > RIL_REQUEST_SET_TTY_MODE ttyMode = %d", serial,
        ttyMode);
  GetRadioProxy();
  if (mRadioProxy == nullptr) {
    ERROR_NS_OK("No Radio HAL exist");
  }
  Return<void> ret = mRadioProxy->setTTYMode(serial, TtyMode(ttyMode));

  if (!ret.isOk()) {
    ERROR_NS_OK("setTTYMode Error.");
  }

  return NS_OK;
}

NS_IMETHODIMP nsRilWorker::QueryTTYMode(int32_t serial) {
  DEBUG("nsRilWorker: [%d] > RIL_REQUEST_QUERY_TTY_MODE ", serial);
  GetRadioProxy();
  if (mRadioProxy == nullptr) {
    ERROR_NS_OK("No Radio HAL exist");
  }
  Return<void> ret = mRadioProxy->getTTYMode(serial);

  if (!ret.isOk()) {
    ERROR_NS_OK("getTTYMode Error.");
  }

  return NS_OK;
}

NS_IMETHODIMP nsRilWorker::ExitEmergencyCallbackMode(int32_t serial) {
  DEBUG("nsRilWorker: [%d] > RIL_REQUEST_EXIT_EMERGENCY_CALLBACK_MODE ",
        serial);
  GetRadioProxy();
  if (mRadioProxy == nullptr) {
    ERROR_NS_OK("No Radio HAL exist");
  }
  Return<void> ret = mRadioProxy->exitEmergencyCallbackMode(serial);

  if (!ret.isOk()) {
    ERROR_NS_OK("exitEmergencyCallbackMode Error.");
  }

  return NS_OK;
}

NS_IMETHODIMP nsRilWorker::SupplyIccPinForApp(int32_t serial,
                                              const nsAString& pin,
                                              const nsAString& aid) {
  DEBUG("nsRilWorker: [%d] > RIL_REQUEST_ENTER_SIM_PIN pin = %s , aid = %s",
        serial, NS_ConvertUTF16toUTF8(pin).get(),
        NS_ConvertUTF16toUTF8(aid).get());
  GetRadioProxy();
  if (mRadioProxy == nullptr) {
    ERROR_NS_OK("No Radio HAL exist");
  }
  Return<void> ret =
      mRadioProxy->supplyIccPinForApp(serial, NS_ConvertUTF16toUTF8(pin).get(),
                                      NS_ConvertUTF16toUTF8(aid).get());

  if (!ret.isOk()) {
    ERROR_NS_OK("supplyIccPinForApp Error.");
  }

  return NS_OK;
}

NS_IMETHODIMP nsRilWorker::SupplyIccPin2ForApp(int32_t serial,
                                               const nsAString& pin,
                                               const nsAString& aid) {
  DEBUG("nsRilWorker: [%d] > RIL_REQUEST_ENTER_SIM_PIN2 pin = %s , aid = %s",
        serial, NS_ConvertUTF16toUTF8(pin).get(),
        NS_ConvertUTF16toUTF8(aid).get());
  GetRadioProxy();
  if (mRadioProxy == nullptr) {
    ERROR_NS_OK("No Radio HAL exist");
  }
  Return<void> ret =
      mRadioProxy->supplyIccPin2ForApp(serial, NS_ConvertUTF16toUTF8(pin).get(),
                                       NS_ConvertUTF16toUTF8(aid).get());

  if (!ret.isOk()) {
    ERROR_NS_OK("supplyIccPin2ForApp Error.");
  }

  return NS_OK;
}

NS_IMETHODIMP nsRilWorker::SupplyIccPukForApp(int32_t serial,
                                              const nsAString& puk,
                                              const nsAString& newPin,
                                              const nsAString& aid) {
  DEBUG(
      "nsRilWorker: [%d] > RIL_REQUEST_ENTER_SIM_PUK puk = %s , newPin = %s "
      ",aid = %s",
      serial, NS_ConvertUTF16toUTF8(puk).get(),
      NS_ConvertUTF16toUTF8(newPin).get(), NS_ConvertUTF16toUTF8(aid).get());
  GetRadioProxy();
  if (mRadioProxy == nullptr) {
    ERROR_NS_OK("No Radio HAL exist");
  }
  Return<void> ret = mRadioProxy->supplyIccPukForApp(
      serial, NS_ConvertUTF16toUTF8(puk).get(),
      NS_ConvertUTF16toUTF8(newPin).get(), NS_ConvertUTF16toUTF8(aid).get());

  if (!ret.isOk()) {
    ERROR_NS_OK("supplyIccPukForApp Error.");
  }

  return NS_OK;
}

NS_IMETHODIMP nsRilWorker::SupplyIccPuk2ForApp(int32_t serial,
                                               const nsAString& puk,
                                               const nsAString& newPin,
                                               const nsAString& aid) {
  DEBUG(
      "nsRilWorker: [%d] > RIL_REQUEST_ENTER_SIM_PUK2 puk = %s , newPin = %s "
      ",aid = %s",
      serial, NS_ConvertUTF16toUTF8(puk).get(),
      NS_ConvertUTF16toUTF8(newPin).get(), NS_ConvertUTF16toUTF8(aid).get());
  GetRadioProxy();
  if (mRadioProxy == nullptr) {
    ERROR_NS_OK("No Radio HAL exist");
  }
  Return<void> ret = mRadioProxy->supplyIccPuk2ForApp(
      serial, NS_ConvertUTF16toUTF8(puk).get(),
      NS_ConvertUTF16toUTF8(newPin).get(), NS_ConvertUTF16toUTF8(aid).get());

  if (!ret.isOk()) {
    ERROR_NS_OK("supplyIccPuk2ForApp Error.");
  }

  return NS_OK;
}

NS_IMETHODIMP nsRilWorker::SetFacilityLockForApp(
    int32_t serial, const nsAString& facility, bool lockState,
    const nsAString& password, int32_t serviceClass, const nsAString& aid) {
  DEBUG("nsRilWorker: [%d] > RIL_REQUEST_SET_FACILITY_LOCK ", serial);
  GetRadioProxy();
  if (mRadioProxy == nullptr) {
    ERROR_NS_OK("No Radio HAL exist");
  }
  Return<void> ret = mRadioProxy->setFacilityLockForApp(
      serial, NS_ConvertUTF16toUTF8(facility).get(), lockState,
      NS_ConvertUTF16toUTF8(password).get(), serviceClass,
      NS_ConvertUTF16toUTF8(aid).get());

  if (!ret.isOk()) {
    ERROR_NS_OK("setFacilityLockForApp Error.");
  }

  return NS_OK;
}

NS_IMETHODIMP nsRilWorker::GetFacilityLockForApp(int32_t serial,
                                                 const nsAString& facility,
                                                 const nsAString& password,
                                                 int32_t serviceClass,
                                                 const nsAString& aid) {
  DEBUG("nsRilWorker: [%d] > RIL_REQUEST_GET_FACILITY_LOCK ", serial);
  GetRadioProxy();
  if (mRadioProxy == nullptr) {
    ERROR_NS_OK("No Radio HAL exist");
  }
  Return<void> ret = mRadioProxy->getFacilityLockForApp(
      serial, NS_ConvertUTF16toUTF8(facility).get(),
      NS_ConvertUTF16toUTF8(password).get(), serviceClass,
      NS_ConvertUTF16toUTF8(aid).get());

  if (!ret.isOk()) {
    ERROR_NS_OK("getFacilityLockForApp Error.");
  }

  return NS_OK;
}

NS_IMETHODIMP nsRilWorker::ChangeIccPinForApp(int32_t serial,
                                              const nsAString& oldPin,
                                              const nsAString& newPin,
                                              const nsAString& aid) {
  DEBUG(
      "nsRilWorker: [%d] > RIL_REQUEST_CHANGE_SIM_PIN oldPin = %s , newPin = "
      "%s ,aid = %s",
      serial, NS_ConvertUTF16toUTF8(oldPin).get(),
      NS_ConvertUTF16toUTF8(newPin).get(), NS_ConvertUTF16toUTF8(aid).get());
  GetRadioProxy();
  if (mRadioProxy == nullptr) {
    ERROR_NS_OK("No Radio HAL exist");
  }
  Return<void> ret = mRadioProxy->changeIccPinForApp(
      serial, NS_ConvertUTF16toUTF8(oldPin).get(),
      NS_ConvertUTF16toUTF8(newPin).get(), NS_ConvertUTF16toUTF8(aid).get());

  if (!ret.isOk()) {
    ERROR_NS_OK("changeIccPinForApp Error.");
  }

  return NS_OK;
}

NS_IMETHODIMP nsRilWorker::ChangeIccPin2ForApp(int32_t serial,
                                               const nsAString& oldPin,
                                               const nsAString& newPin,
                                               const nsAString& aid) {
  DEBUG(
      "nsRilWorker: [%d] > RIL_REQUEST_CHANGE_SIM_PIN2 oldPin = %s , newPin = "
      "%s ,aid = %s",
      serial, NS_ConvertUTF16toUTF8(oldPin).get(),
      NS_ConvertUTF16toUTF8(newPin).get(), NS_ConvertUTF16toUTF8(aid).get());
  GetRadioProxy();
  if (mRadioProxy == nullptr) {
    ERROR_NS_OK("No Radio HAL exist");
  }
  Return<void> ret = mRadioProxy->changeIccPin2ForApp(
      serial, NS_ConvertUTF16toUTF8(oldPin).get(),
      NS_ConvertUTF16toUTF8(newPin).get(), NS_ConvertUTF16toUTF8(aid).get());

  if (!ret.isOk()) {
    ERROR_NS_OK("changeIccPin2ForApp Error.");
  }

  return NS_OK;
}

NS_IMETHODIMP nsRilWorker::ReportStkServiceIsRunning(int32_t serial) {
  DEBUG("nsRilWorker: [%d] > RIL_REQUEST_REPORT_STK_SERVICE_IS_RUNNING ",
        serial);
  GetRadioProxy();
  if (mRadioProxy == nullptr) {
    ERROR_NS_OK("No Radio HAL exist");
  }
  Return<void> ret = mRadioProxy->reportStkServiceIsRunning(serial);

  if (!ret.isOk()) {
    ERROR_NS_OK("reportStkServiceIsRunning Error.");
  }

  return NS_OK;
}

NS_IMETHODIMP nsRilWorker::SetGsmBroadcastActivation(int32_t serial,
                                                     bool activate) {
  DEBUG("nsRilWorker: [%d] > RIL_REQUEST_GSM_BROADCAST_ACTIVATION ", serial);
  GetRadioProxy();
  if (mRadioProxy == nullptr) {
    ERROR_NS_OK("No Radio HAL exist");
  }
  Return<void> ret = mRadioProxy->setGsmBroadcastActivation(serial, activate);

  if (!ret.isOk()) {
    ERROR_NS_OK("setGsmBroadcastActivation Error.");
  }

  return NS_OK;
}

NS_IMETHODIMP nsRilWorker::SetGsmBroadcastConfig(
    int32_t serial, const nsTArray<int32_t>& ranges) {
  DEBUG("nsRilWorker: [%d] > RIL_REQUEST_GSM_SET_BROADCAST_CONFIG ", serial);
  GetRadioProxy();
  if (mRadioProxy == nullptr) {
    ERROR_NS_OK("No Radio HAL exist");
  }

  std::vector<GsmBroadcastSmsConfigInfo> broadcastInfo;
  for (uint32_t i = 0; i < ranges.Length();) {
    GsmBroadcastSmsConfigInfo info;
    // convert [from, to) to [from, to - 1]
    info.fromServiceId = ranges[i++];
    info.toServiceId = ranges[i++] - 1;
    info.fromCodeScheme = 0x00;
    info.toCodeScheme = 0xFF;
    info.selected = 1;
    broadcastInfo.push_back(info);
  }
  Return<void> ret = mRadioProxy->setGsmBroadcastConfig(serial, broadcastInfo);

  if (!ret.isOk()) {
    ERROR_NS_OK("setGsmBroadcastConfig Error.");
  }

  return NS_OK;
}

NS_IMETHODIMP nsRilWorker::GetPreferredVoicePrivacy(int32_t serial) {
  DEBUG(
      "nsRilWorker: [%d] > "
      "RIL_REQUEST_CDMA_QUERY_PREFERRED_VOICE_PRIVACY_MODE ",
      serial);
  GetRadioProxy();
  if (mRadioProxy == nullptr) {
    ERROR_NS_OK("No Radio HAL exist");
  }
  Return<void> ret = mRadioProxy->getPreferredVoicePrivacy(serial);

  if (!ret.isOk()) {
    ERROR_NS_OK("getPreferredVoicePrivacy Error.");
  }

  return NS_OK;
}

NS_IMETHODIMP nsRilWorker::SetPreferredVoicePrivacy(int32_t serial,
                                                    bool enable) {
  DEBUG(
      "nsRilWorker: [%d] > RIL_REQUEST_CDMA_SET_PREFERRED_VOICE_PRIVACY_MODE "
      "enable = %d",
      serial, enable);
  GetRadioProxy();
  if (mRadioProxy == nullptr) {
    ERROR_NS_OK("No Radio HAL exist");
  }
  Return<void> ret = mRadioProxy->setPreferredVoicePrivacy(serial, enable);

  if (!ret.isOk()) {
    ERROR_NS_OK("setPreferredVoicePrivacy Error.");
  }

  return NS_OK;
}

NS_IMETHODIMP nsRilWorker::RequestIccSimAuthentication(int32_t serial,
                                                       int32_t authContext,
                                                       const nsAString& data,
                                                       const nsAString& aid) {
  DEBUG("nsRilWorker: [%d] > RIL_REQUEST_SIM_AUTHENTICATION ", serial);
  GetRadioProxy();
  if (mRadioProxy == nullptr) {
    ERROR_NS_OK("No Radio HAL exist");
  }
  Return<void> ret = mRadioProxy->requestIccSimAuthentication(
      serial, authContext, NS_ConvertUTF16toUTF8(data).get(),
      NS_ConvertUTF16toUTF8(aid).get());

  if (!ret.isOk()) {
    ERROR_NS_OK("requestIccSimAuthentication Error.");
  }

  return NS_OK;
}

NS_IMETHODIMP nsRilWorker::SendSMS(int32_t serial, const nsAString& smsc,
                                   const nsAString& pdu) {
  GetRadioProxy();
  if (mRadioProxy == nullptr) {
    ERROR_NS_OK("No Radio HAL exist");
  }

  GsmSmsMessage smsMessage;
  smsMessage.smscPdu = NS_ConvertUTF16toUTF8(smsc).get();
  smsMessage.pdu = NS_ConvertUTF16toUTF8(pdu).get();
  DEBUG("nsRilWorker: [%d] > RIL_REQUEST_SEND_SMS %s", serial,
        NS_ConvertUTF16toUTF8(pdu).get());
  Return<void> ret = mRadioProxy->sendSms(serial, smsMessage);

  if (!ret.isOk()) {
    ERROR_NS_OK("sendSms Error.");
  }

  return NS_OK;
}

NS_IMETHODIMP nsRilWorker::AcknowledgeLastIncomingGsmSms(int32_t serial,
                                                         bool success,
                                                         int32_t cause) {
  DEBUG("nsRilWorker: [%d] > RIL_REQUEST_SMS_ACKNOWLEDGE ", serial);
  GetRadioProxy();
  if (mRadioProxy == nullptr) {
    ERROR_NS_OK("No Radio HAL exist");
  }
  Return<void> ret = mRadioProxy->acknowledgeLastIncomingGsmSms(
      serial, success, SmsAcknowledgeFailCause(cause));

  if (!ret.isOk()) {
    ERROR_NS_OK("acknowledgeLastIncomingGsmSms Error.");
  }

  return NS_OK;
}

NS_IMETHODIMP nsRilWorker::SetSuppServiceNotifications(int32_t serial,
                                                       bool enable) {
  DEBUG(
      "nsRilWorker: [%d] > RIL_REQUEST_SET_SUPP_SVC_NOTIFICATION "
      "enable = %d",
      serial, enable);
  GetRadioProxy();
  if (mRadioProxy == nullptr) {
    ERROR_NS_OK("No Radio HAL exist");
  }
  Return<void> ret = mRadioProxy->setSuppServiceNotifications(serial, enable);

  if (!ret.isOk()) {
    ERROR_NS_OK("setSuppServiceNotifications Error.");
  }

  return NS_OK;
}

NS_IMETHODIMP nsRilWorker::HandleStkCallSetupRequestFromSim(int32_t serial,
                                                            bool accept) {
  DEBUG(
      "nsRilWorker: [%d] > "
      "RIL_REQUEST_STK_HANDLE_CALL_SETUP_REQUESTED_FROM_SIM on = %d",
      serial, accept);

  GetRadioProxy();
  if (mRadioProxy == nullptr) {
    ERROR_NS_OK("No Radio HAL exist");
  }
  Return<void> ret =
      mRadioProxy->handleStkCallSetupRequestFromSim(serial, accept);

  if (!ret.isOk()) {
    ERROR_NS_OK("handleStkCallSetupRequestFromSim Error.");
  }

  return NS_OK;
}

NS_IMETHODIMP nsRilWorker::SendTerminalResponseToSim(
    int32_t serial, const nsAString& contents) {
  DEBUG(
      "nsRilWorker: [%d] > RIL_REQUEST_STK_SEND_TERMINAL_RESPONSE contents = "
      "%s",
      serial, NS_ConvertUTF16toUTF8(contents).get());
  GetRadioProxy();
  if (mRadioProxy == nullptr) {
    ERROR_NS_OK("No Radio HAL exist");
  }
  Return<void> ret = mRadioProxy->sendTerminalResponseToSim(
      serial, NS_ConvertUTF16toUTF8(contents).get());

  if (!ret.isOk()) {
    ERROR_NS_OK("sendTerminalResponseToSim Error.");
  }

  return NS_OK;
}

NS_IMETHODIMP nsRilWorker::SendEnvelope(int32_t serial,
                                        const nsAString& contents) {
  DEBUG(
      "nsRilWorker: [%d] > RIL_REQUEST_STK_SEND_ENVELOPE_COMMAND contents = "
      "%s",
      serial, NS_ConvertUTF16toUTF8(contents).get());
  GetRadioProxy();
  if (mRadioProxy == nullptr) {
    ERROR_NS_OK("No Radio HAL exist");
  }
  Return<void> ret =
      mRadioProxy->sendEnvelope(serial, NS_ConvertUTF16toUTF8(contents).get());

  if (!ret.isOk()) {
    ERROR_NS_OK("sendEnvelope Error.");
  }

  return NS_OK;
}

NS_IMETHODIMP nsRilWorker::GetRadioCapability(int32_t serial) {
  DEBUG("nsRilWorker: [%d] > RIL_REQUEST_GET_RADIO_CAPABILITY", serial);

  GetRadioProxy();
  if (mRadioProxy == nullptr) {
    ERROR_NS_OK("No Radio HAL exist");
  }
  Return<void> ret = mRadioProxy->getRadioCapability(serial);

  if (!ret.isOk()) {
    ERROR_NS_OK("getRadioCapability Error.");
  }

  return NS_OK;
}

NS_IMETHODIMP nsRilWorker::StopNetworkScan(int32_t serial) {
  DEBUG("nsRilWorker: [%d] > RIL_REQUEST_STOP_NETWORK_SCAN", serial);
  GetRadioProxy();
  if (mRadioProxy == nullptr) {
    ERROR_NS_OK("No Radio HAL exist");
  }
  Return<void> ret = mRadioProxy->stopNetworkScan(serial);

  if (!ret.isOk()) {
    ERROR_NS_OK("stopNetworkScan Error.");
  }

  return NS_OK;
}

NS_IMETHODIMP nsRilWorker::SetUnsolResponseFilter(int32_t serial,
                                                  int32_t filter) {
  DEBUG("nsRilWorker: [%d] > RIL_REQUEST_SET_UNSOLICITED_RESPONSE_FILTER",
        serial);
  GetRadioProxy();
  if (mRadioProxy == nullptr) {
    ERROR_NS_OK("No Radio HAL exist");
  }
  Return<void> ret = mRadioProxy->setIndicationFilter(serial, filter);

  if (!ret.isOk()) {
    ERROR_NS_OK("setIndicationFilter Error.");
  }

  return NS_OK;
}

nsRilWorker::~nsRilWorker() {
  DEBUG("Destructor nsRilWorker");
  mRilResponse = nullptr;
  MOZ_ASSERT(!mRilResponse);
  mRilIndication = nullptr;
  MOZ_ASSERT(!mRilIndication);
  mRadioProxy = nullptr;
  MOZ_ASSERT(!mRadioProxy);
  mDeathRecipient = nullptr;
  MOZ_ASSERT(!mDeathRecipient);
}

void nsRilWorker::RadioProxyDeathRecipient::serviceDied(
    uint64_t, const ::android::wp<IBase>&) {
  INFO("nsRilWorker HAL died, cleanup instance.");
}

void nsRilWorker::GetRadioProxy() {
  if (mRadioProxy != nullptr) {
    return;
  }
  DEBUG("GetRadioProxy");

  mRadioProxy = IRadio::getService(HIDL_SERVICE_NAME[mClientId]);

  if (mRadioProxy != nullptr) {
    if (mDeathRecipient == nullptr) {
      mDeathRecipient = new RadioProxyDeathRecipient();
    }
    if (mDeathRecipient != nullptr) {
      Return<bool> linked =
          mRadioProxy->linkToDeath(mDeathRecipient, 0 /*cookie*/);
      if (!linked || !linked.isOk()) {
        ERROR("Failed to link to radio hal death notifications");
      }
    }
    DEBUG("setResponseFunctions");
    mRadioProxy->setResponseFunctions(mRilResponse, mRilIndication);
  } else {
    ERROR("Get Radio hal failed");
  }
}

void nsRilWorker::processIndication(RadioIndicationType indicationType) {
  DEBUG("processIndication, type= %d", indicationType);
  if (indicationType == RadioIndicationType::UNSOLICITED_ACK_EXP) {
    sendAck();
    DEBUG("Unsol response received; Sending ack to ril.cpp");
  } else {
    // ack is not expected to be sent back. Nothing is required to be done here.
  }
}

void nsRilWorker::processResponse(RadioResponseType responseType) {
  DEBUG("processResponse, type= %d", responseType);
  if (responseType == RadioResponseType::SOLICITED_ACK_EXP) {
    sendAck();
    DEBUG("Solicited response received; Sending ack to ril.cpp");
  } else {
    // ack is not expected to be sent back. Nothing is required to be done here.
  }
}

void nsRilWorker::sendAck() {
  DEBUG("sendAck");
  GetRadioProxy();
  if (mRadioProxy != nullptr) {
    mRadioProxy->responseAcknowledgement();
  } else {
    ERROR("sendAck mRadioProxy == nullptr");
  }
}

void nsRilWorker::sendRilIndicationResult(nsRilIndicationResult* aIndication) {
  DEBUG("nsRilWorker: [USOL]< %s",
        NS_LossyConvertUTF16toASCII(aIndication->mRilMessageType).get());

  RefPtr<nsRilIndicationResult> indication = aIndication;
  nsCOMPtr<nsIRunnable> r = NS_NewRunnableFunction(
      "nsRilWorker::sendRilIndicationResult", [this, indication]() {
        if (mRilCallback) {
          mRilCallback->HandleRilIndication(indication);
        } else {
          DEBUG("sendRilIndicationResult: no callback");
        }
      });
  NS_DispatchToMainThread(r);
}

void nsRilWorker::sendRilResponseResult(nsRilResponseResult* aResponse) {
  DEBUG("nsRilWorker: [%d] < %s", aResponse->mRilMessageToken,
        NS_LossyConvertUTF16toASCII(aResponse->mRilMessageType).get());

  if (aResponse->mRilMessageToken > 0) {
    RefPtr<nsRilResponseResult> response = aResponse;
    nsCOMPtr<nsIRunnable> r = NS_NewRunnableFunction(
        "nsRilWorker::sendRilResponseResult", [this, response]() {
          if (mRilCallback) {
            mRilCallback->HandleRilResponse(response);
          } else {
            DEBUG("sendRilResponseResult: no callback");
          }
        });
    NS_DispatchToMainThread(r);
  } else {
    DEBUG("ResponseResult internal reqeust.");
  }
}

// Helper function
MvnoType nsRilWorker::convertToHalMvnoType(const nsAString& mvnoType) {
  if (u"imsi"_ns.Equals(mvnoType)) {
    return MvnoType::IMSI;
  } else if (u"gid"_ns.Equals(mvnoType)) {
    return MvnoType::GID;
  } else if (u"spn"_ns.Equals(mvnoType)) {
    return MvnoType::SPN;
  } else {
    return MvnoType::NONE;
  }
}

#if RADIO_HAL >= 14
PdpProtocolType convertToHalPdpType(const nsAString& val) {
  if (u"IP"_ns.Equals(val)) {
    return PdpProtocolType::IP;
  } else if (u"IPV6"_ns.Equals(val)) {
    return PdpProtocolType::IPV6;
  } else if (u"IPV4V6n"_ns.Equals(val)) {
    return PdpProtocolType::IPV4V6;
  } else if (u"PPP"_ns.Equals(val)) {
    return PdpProtocolType::PPP;
  } else if (u"NON_IP"_ns.Equals(val)) {
    return PdpProtocolType::NON_IP;
  } else {
    return PdpProtocolType::UNKNOWN;
  }
}

DataProfileInfo nsRilWorker::convertToHalDataProfile(nsIDataProfile* profile) {
  DataProfileInfo dataProfileInfo;

  int32_t profileId;
  profile->GetProfileId(&profileId);
  dataProfileInfo.profileId = DataProfileId(profileId);

  nsString apn;
  profile->GetApn(apn);
  dataProfileInfo.apn = NS_ConvertUTF16toUTF8(apn).get();

  nsString protocol;
  profile->GetProtocol(protocol);
  dataProfileInfo.protocol = convertToHalPdpType(protocol);

  nsString roamingProtocol;
  profile->GetRoamingProtocol(roamingProtocol);
  dataProfileInfo.roamingProtocol = convertToHalPdpType(roamingProtocol);

  int32_t authType;
  profile->GetAuthType(&authType);
  dataProfileInfo.authType = ApnAuthType(authType);

  nsString user;
  profile->GetUser(user);
  dataProfileInfo.user = NS_ConvertUTF16toUTF8(user).get();

  nsString password;
  profile->GetPassword(password);
  dataProfileInfo.password = NS_ConvertUTF16toUTF8(password).get();

  int32_t type;
  profile->GetType(&type);
  dataProfileInfo.type = DataProfileInfoType(type);

  int32_t maxConnsTime;
  profile->GetMaxConnsTime(&maxConnsTime);
  dataProfileInfo.maxConnsTime = maxConnsTime;

  int32_t maxConns;
  profile->GetMaxConns(&maxConns);
  dataProfileInfo.maxConns = maxConns;

  int32_t waitTime;
  profile->GetWaitTime(&waitTime);
  dataProfileInfo.waitTime = waitTime;

  bool enabled;
  profile->GetEnabled(&enabled);
  dataProfileInfo.enabled = enabled;

  int32_t supportedApnTypesBitmap;
  profile->GetSupportedApnTypesBitmap(&supportedApnTypesBitmap);
  dataProfileInfo.supportedApnTypesBitmap =
      (int32_t)ApnTypes(supportedApnTypesBitmap);

  int32_t bearerBitmap;
  profile->GetBearerBitmap(&bearerBitmap);
  dataProfileInfo.bearerBitmap = (int32_t)RadioAccessFamily(bearerBitmap);

  int32_t mtu;
  profile->GetMtu(&mtu);
  dataProfileInfo.mtu = mtu;

  return dataProfileInfo;
}

#else

DataProfileInfo nsRilWorker::convertToHalDataProfile(nsIDataProfile* profile) {
  DataProfileInfo dataProfileInfo;

  int32_t profileId;
  profile->GetProfileId(&profileId);
  dataProfileInfo.profileId = DataProfileId(profileId);

  nsString apn;
  profile->GetApn(apn);
  dataProfileInfo.apn = NS_ConvertUTF16toUTF8(apn).get();

  nsString protocol;
  profile->GetProtocol(protocol);
  dataProfileInfo.protocol = NS_ConvertUTF16toUTF8(protocol).get();

  nsString roamingProtocol;
  profile->GetRoamingProtocol(roamingProtocol);
  dataProfileInfo.roamingProtocol =
      NS_ConvertUTF16toUTF8(roamingProtocol).get();

  int32_t authType;
  profile->GetAuthType(&authType);
  dataProfileInfo.authType = ApnAuthType(authType);

  nsString user;
  profile->GetUser(user);
  dataProfileInfo.user = NS_ConvertUTF16toUTF8(user).get();

  nsString password;
  profile->GetPassword(password);
  dataProfileInfo.password = NS_ConvertUTF16toUTF8(password).get();

  int32_t type;
  profile->GetType(&type);
  dataProfileInfo.type = DataProfileInfoType(type);

  int32_t maxConnsTime;
  profile->GetMaxConnsTime(&maxConnsTime);
  dataProfileInfo.maxConnsTime = maxConnsTime;

  int32_t maxConns;
  profile->GetMaxConns(&maxConns);
  dataProfileInfo.maxConns = maxConns;

  int32_t waitTime;
  profile->GetWaitTime(&waitTime);
  dataProfileInfo.waitTime = waitTime;

  bool enabled;
  profile->GetEnabled(&enabled);
  dataProfileInfo.enabled = enabled;

  int32_t supportedApnTypesBitmap;
  profile->GetSupportedApnTypesBitmap(&supportedApnTypesBitmap);
  dataProfileInfo.supportedApnTypesBitmap =
      (int32_t)ApnTypes(supportedApnTypesBitmap);

  int32_t bearerBitmap;
  profile->GetBearerBitmap(&bearerBitmap);
  dataProfileInfo.bearerBitmap = (int32_t)RadioAccessFamily(bearerBitmap);

  int32_t mtu;
  profile->GetMtu(&mtu);
  dataProfileInfo.mtu = mtu;

  nsString mvnoType;
  profile->GetMvnoType(mvnoType);
  dataProfileInfo.mvnoType = convertToHalMvnoType(mvnoType);

  nsString mvnoMatchData;
  profile->GetMvnoMatchData(mvnoMatchData);
  dataProfileInfo.mvnoMatchData = NS_ConvertUTF16toUTF8(mvnoMatchData).get();

  return dataProfileInfo;
}

#endif  // RADIO_HAL >= 14
