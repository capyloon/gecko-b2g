/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=8 sts=2 et sw=2 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#define LOG_TAG "SupplicantStaManager"

#include "SupplicantStaManagerAidl.h"
#include <cutils/properties.h>
#include <utils/Log.h>
#include <string.h>
#include <mozilla/ClearOnShutdown.h>

#include <android/binder_manager.h>

#define ANY_MAC_STR "any"
#define WPS_DEV_TYPE_LEN 8
#define WPS_DEV_TYPE_DUAL_SMARTPHONE "10-0050F204-5"

#define EVENT_SUPPLICANT_TERMINATING u"SUPPLICANT_TERMINATING"_ns

#define AIDL_SERVICE "android.hardware.wifi.supplicant.ISupplicant/default"

using namespace mozilla::dom::wifi;

static const char SUPPLICANT_INTERFACE_NAME_V1_0[] =
    "android.hardware.wifi.supplicant@1.0::ISupplicant";

static const char SUPPLICANT_INTERFACE_NAME_V1_1[] =
    "android.hardware.wifi.supplicant@1.1::ISupplicant";

static const char SUPPLICANT_INTERFACE_NAME_V1_2[] =
    "android.hardware.wifi.supplicant@1.2::ISupplicant";

static const char HAL_INSTANCE_NAME[] = "default";

mozilla::Mutex SupplicantStaManager::sLock("supplicant-sta");
mozilla::Mutex SupplicantStaManager::sHashLock("supplicant-hash");

static StaticAutoPtr<SupplicantStaManager> sSupplicantManager;

SupplicantStaManager::SupplicantStaManager()
    : mServiceManager(nullptr),
      mSupplicant(nullptr),
      mSupplicantStaIface(nullptr),
      mSupplicantStaIfaceCallback(nullptr),
      mServiceManagerDeathRecipient(nullptr),
      mSupplicantDeathRecipient(nullptr),
      mDeathEventHandler(nullptr),
      mDeathRecipientCookie(0) {}

SupplicantStaManager* SupplicantStaManager::Get() {
  MOZ_ASSERT(NS_IsMainThread());

  if (!sSupplicantManager) {
    sSupplicantManager = new SupplicantStaManager();
    ClearOnShutdown(&sSupplicantManager);
  }
  return sSupplicantManager;
}

void SupplicantStaManager::CleanUp() {
  if (sSupplicantManager != nullptr) {
    sSupplicantManager->DeinitInterface();
  }
}

void SupplicantStaManager::ServiceManagerDeathRecipient::serviceDied(
    uint64_t, const android::wp<IBase>&) {
  WIFI_LOGE(LOG_TAG, "IServiceManager HAL died, cleanup instance.");
  MutexAutoLock lock(sLock);

  if (mOuter != nullptr) {
    mOuter->SupplicantServiceDiedHandler(mOuter->mDeathRecipientCookie);
    mOuter->mServiceManager = nullptr;
  }
}

void SupplicantStaManager::SupplicantDeathRecipient::serviceDied(
    uint64_t, const android::wp<IBase>&) {
  WIFI_LOGE(LOG_TAG, "ISupplicant HAL died, cleanup instance.");
  MutexAutoLock lock(sLock);

  if (mOuter != nullptr) {
    mOuter->SupplicantServiceDiedHandler(mOuter->mDeathRecipientCookie);
    mOuter->mSupplicant = nullptr;
  }
}

void SupplicantStaManager::RegisterEventCallback(
    const android::sp<WifiEventCallback>& aCallback) {
  mCallback = aCallback;
}

void SupplicantStaManager::UnregisterEventCallback() { mCallback = nullptr; }

void SupplicantStaManager::RegisterPasspointCallback(
    PasspointEventCallback* aCallback) {
  mPasspointCallback = aCallback;
}

void SupplicantStaManager::UnregisterPasspointCallback() {
  mPasspointCallback = nullptr;
}

Result_t SupplicantStaManager::InitInterface() {
  if (mSupplicant != nullptr) {
    return nsIWifiResult::SUCCESS;
  }
  return InitServiceManager();
}

Result_t SupplicantStaManager::DeinitInterface() { return TearDownInterface(); }

Result_t SupplicantStaManager::InitServiceManager() {
  WIFI_LOGE(LOG_TAG, "InitServiceManager start");
  MutexAutoLock lock(sLock);
  if (mServiceManager != nullptr) {
    // Service already exists.
    return nsIWifiResult::SUCCESS;
  }

  mServiceManager =
      ::android::hidl::manager::V1_0::IServiceManager::getService();
  if (mServiceManager == nullptr) {
    WIFI_LOGE(LOG_TAG, "Failed to get HIDL service manager");
    return nsIWifiResult::ERROR_COMMAND_FAILED;
  }

  if (mServiceManagerDeathRecipient == nullptr) {
    mServiceManagerDeathRecipient =
        new ServiceManagerDeathRecipient(sSupplicantManager);
  }

  Return<bool> linked =
      mServiceManager->linkToDeath(mServiceManagerDeathRecipient, 0);
  if (!linked || !linked.isOk()) {
    WIFI_LOGE(LOG_TAG, "Error on linkToDeath to IServiceManager");
    SupplicantServiceDiedHandler(mDeathRecipientCookie);
    mServiceManager = nullptr;
    return nsIWifiResult::ERROR_COMMAND_FAILED;
  }

  return InitSupplicantInterface();
}

Result_t SupplicantStaManager::InitSupplicantInterface() {
  bool isDeclared = AServiceManager_isDeclared(AIDL_SERVICE);

  if (!isDeclared) {
    WIFI_LOGE(LOG_TAG, "%s is not available!", AIDL_SERVICE);
    return nsIWifiResult::ERROR_COMMAND_FAILED;
  }

  mSupplicant = aidl_sup::ISupplicant::fromBinder(
    ndk::SpAIBinder(AServiceManager_waitForService(AIDL_SERVICE)));

  WIFI_LOGE(LOG_TAG, "mSupplicant=%p", mSupplicant.get());

  if (mSupplicant != nullptr) {
    if (mSupplicantDeathRecipient == nullptr) {
      mSupplicantDeathRecipient =
          new SupplicantDeathRecipient(sSupplicantManager);
    }

    // TODO: rewrite
    // if (mSupplicantDeathRecipient != nullptr) {
    //   mDeathRecipientCookie = mDeathRecipientCookie + 1;
    //   auto linked = AIBinder_linkToDeath(mSupplicant->asBinder().get(),
    //                                 mSupplicantDeathRecipient,
    //                                 mDeathRecipientCookie);
    //   if (!linked.isOk()) {
    //     WIFI_LOGE(LOG_TAG,
    //               "Failed to link to supplicant hal death notifications");
    //     SupplicantServiceDiedHandler(mDeathRecipientCookie);
    //     mSupplicant = nullptr;
    //     return nsIWifiResult::ERROR_COMMAND_FAILED;
    //   }
    // }

    // SupplicantStatus response;
    // mSupplicant->registerCallback(
    //     this, [&](const SupplicantStatus& status) { response = status; });
    // if (response.code != SupplicantStatusCode::SUCCESS) {
    //   WIFI_LOGE(LOG_TAG, "registerCallback failed: %d, reason: %s",
    //             response.code, response.debugMessage.c_str());
    //   mSupplicant = nullptr;
    // }

    // Signature is: registerCallback(const std::shared_ptr<aidl_sup::ISupplicantCallback>& in_callback)
    // auto response = mSupplicant->registerCallback(this);
    // if (!response.isOk()) {
    //    WIFI_LOGE(LOG_TAG, "registerCallback failed: %d, reason: %s",
    //             response.getServiceSpecificError(), response.getDescription.c_str());
    //   mSupplicant = nullptr;
    // }
  } else {
    WIFI_LOGE(LOG_TAG, "get supplicant hal failed");
    return nsIWifiResult::ERROR_COMMAND_FAILED;
  }
  return nsIWifiResult::SUCCESS;
}

bool SupplicantStaManager::IsInterfaceInitializing() {
  MutexAutoLock lock(sLock);
  return mServiceManager != nullptr;
}

bool SupplicantStaManager::IsInterfaceReady() {
  MutexAutoLock lock(sLock);
  return mSupplicant != nullptr;
}

Result_t SupplicantStaManager::TearDownInterface() {
  MutexAutoLock lock(sLock);

  if (mSupplicant) {
    aidl_sup::IfaceInfo info;
    info.name = mInterfaceName;
    info.type = aidl_sup::IfaceType::STA;

    auto response = mSupplicant->removeInterface(info);

    if (!response.isOk()) {
      WIFI_LOGE(LOG_TAG, "Failed to remove sta interface");
      return nsIWifiResult::ERROR_COMMAND_FAILED;
    }
  }

  ModifyConfigurationHash(CLEAN_ALL, mDummyNetworkConfiguration);
  mCurrentNetwork.clear();
  mSupplicant = nullptr;
  mSupplicantStaIface = nullptr;
  mSupplicantStaIfaceCallback = nullptr;

  return nsIWifiResult::SUCCESS;
}

Result_t SupplicantStaManager::GetMacAddress(nsAString& aMacAddress) {
  MutexAutoLock lock(sLock);
  if (!mSupplicantStaIface) {
    return nsIWifiResult::ERROR_INVALID_INTERFACE;
  }

  std::vector<uint8_t> macAddr;
  macAddr.reserve(6);
  auto response = mSupplicantStaIface->getMacAddress(&macAddr);
  if (response.isOk()) {
    std::string addressStr = ConvertMacToString(macAddr);
    nsString address(NS_ConvertUTF8toUTF16(addressStr.c_str()));
    aMacAddress.Assign(address);
  }
  return response.isOk();
}

Result_t SupplicantStaManager::GetSupportedFeatures(
    uint32_t& aSupportedFeatures) {
  MutexAutoLock lock(sLock);
  uint32_t capabilities = 0;

  // TODO !!
  // SupplicantStatus response;
  // if (IsSupplicantV1_2()) {
  //   android::sp<ISupplicantStaIfaceV1_2> supplicantV1_2 =
  //       GetSupplicantStaIfaceV1_2();
  //   if (!supplicantV1_2) {
  //     return nsIWifiResult::ERROR_INVALID_INTERFACE;
  //   }

  //   supplicantV1_2->getKeyMgmtCapabilities(
  //       [&](const SupplicantStatus& status, uint32_t keyMgmtMask) {
  //         capabilities = keyMgmtMask;
  //         response = status;
  //       });

  //   if (response.code != SupplicantStatusCode::SUCCESS) {
  //     return nsIWifiResult::ERROR_COMMAND_FAILED;
  //   }

  //   if (capabilities & ISupplicantStaNetworkV1_2::KeyMgmtMask::SAE) {
  //     // SAE supported
  //     aSupportedFeatures |= nsIWifiResult::FEATURE_WPA3_SAE;
  //   }
  //   if (capabilities & ISupplicantStaNetworkV1_2::KeyMgmtMask::SUITE_B_192) {
  //     // SUITE_B supported
  //     aSupportedFeatures |= nsIWifiResult::FEATURE_WPA3_SUITE_B;
  //   }
  //   if (capabilities & ISupplicantStaNetworkV1_2::KeyMgmtMask::OWE) {
  //     // OWE supported
  //     aSupportedFeatures |= nsIWifiResult::FEATURE_OWE;
  //   }
  //   if (capabilities & ISupplicantStaNetworkV1_2::KeyMgmtMask::DPP) {
  //     // DPP supported
  //     aSupportedFeatures |= nsIWifiResult::FEATURE_DPP;
  //   }
  // }
  return nsIWifiResult::SUCCESS;
}

Result_t SupplicantStaManager::GetSupplicantDebugLevel(uint32_t& aLevel) {
  MutexAutoLock lock(sLock);
  if (!mSupplicant) {
    return nsIWifiResult::ERROR_INVALID_INTERFACE;
  }
  aidl_sup::DebugLevel level = aidl_sup::DebugLevel::ERROR;
  auto _response = mSupplicant->getDebugLevel(&level);
  aLevel = (uint32_t) level;
  return nsIWifiResult::SUCCESS;
}

Result_t SupplicantStaManager::SetSupplicantDebugLevel(
    SupplicantDebugLevelOptions* aLevel) {
  MutexAutoLock lock(sLock);
  if (!mSupplicant) {
    return nsIWifiResult::ERROR_INVALID_INTERFACE;
  }

  auto response = mSupplicant->setDebugParams(
    static_cast<aidl_sup::DebugLevel>(aLevel->mLogLevel),
    aLevel->mShowTimeStamp, aLevel->mShowKeys
  );
  
  if (!response.isOk()) {
    WIFI_LOGE(LOG_TAG, "Failed to set suppliant debug level");
  };
  return nsIWifiResult::SUCCESS;
}

Result_t SupplicantStaManager::SetConcurrencyPriority(bool aEnable) {
  MutexAutoLock lock(sLock);
  if (!mSupplicant) {
    return nsIWifiResult::ERROR_INVALID_INTERFACE;
  }

  auto type = aEnable ? aidl_sup::IfaceType::STA : aidl_sup::IfaceType::P2P;
  auto response = mSupplicant->setConcurrencyPriority(type);
  return response.isOk() ? nsIWifiResult::SUCCESS
                          : nsIWifiResult::ERROR_COMMAND_FAILED;
}

Result_t SupplicantStaManager::SetPowerSave(bool aEnable) {
  MutexAutoLock lock(sLock);
  if (!mSupplicantStaIface) {
    return nsIWifiResult::ERROR_INVALID_INTERFACE;
  }
  return mSupplicantStaIface->setPowerSave(aEnable).isOk();
}

Result_t SupplicantStaManager::SetSuspendMode(bool aEnable) {
  MutexAutoLock lock(sLock);
  if (!mSupplicantStaIface) {
    return nsIWifiResult::ERROR_INVALID_INTERFACE;
  }
  return mSupplicantStaIface->setSuspendModeEnabled(aEnable).isOk();
}

Result_t SupplicantStaManager::SetExternalSim(bool aEnable) {
  MutexAutoLock lock(sLock);
  if (!mSupplicantStaIface) {
    return nsIWifiResult::ERROR_INVALID_INTERFACE;
  }
  return mSupplicantStaIface->setExternalSim(aEnable).isOk();
}

Result_t SupplicantStaManager::SetAutoReconnect(bool aEnable) {
  MutexAutoLock lock(sLock);
  if (!mSupplicantStaIface) {
    return nsIWifiResult::ERROR_INVALID_INTERFACE;
  }
  return mSupplicantStaIface->enableAutoReconnect(aEnable).isOk();
}

Result_t SupplicantStaManager::SetCountryCode(const std::string& aCountryCode) {
  MutexAutoLock lock(sLock);
  if (!mSupplicantStaIface) {
    return nsIWifiResult::ERROR_INVALID_INTERFACE;
  }
  if (aCountryCode.length() != 2) {
    WIFI_LOGE(LOG_TAG, "Invalid country code: %s", aCountryCode.c_str());
    return nsIWifiResult::ERROR_INVALID_ARGS;
  }
  std::vector<uint8_t> countryCode;
  countryCode.reserve(2);
  countryCode.push_back(aCountryCode.at(0));
  countryCode.push_back(aCountryCode.at(1));

  return mSupplicantStaIface->setCountryCode(countryCode).isOk();
}

Result_t SupplicantStaManager::SetBtCoexistenceMode(uint8_t aMode) {
  MutexAutoLock lock(sLock);
  if (!mSupplicantStaIface) {
    return nsIWifiResult::ERROR_INVALID_INTERFACE;
  }
  return mSupplicantStaIface->setBtCoexistenceMode(
    (aidl_sup::BtCoexistenceMode)aMode).isOk();
}

Result_t SupplicantStaManager::SetBtCoexistenceScanMode(bool aEnable) {
  MutexAutoLock lock(sLock);
  if (!mSupplicantStaIface) {
    return nsIWifiResult::ERROR_INVALID_INTERFACE;
  }
  return mSupplicantStaIface->setBtCoexistenceScanModeEnabled(aEnable).isOk();
}

// Helper function to find any iface of the desired type exposed.
Result_t SupplicantStaManager::FindIfaceOfType(
    aidl_sup::IfaceType aDesired,
    aidl_sup::IfaceInfo* aInfo) {
  if (mSupplicant == nullptr) {
    return nsIWifiResult::ERROR_INVALID_INTERFACE;
  }

  SupplicantStatus response;
  std::vector<aidl_sup::IfaceInfo> iface_infos;
  mSupplicant->listInterfaces(&iface_infos);

  if (response.code != SupplicantStatusCode::SUCCESS) {
    return nsIWifiResult::ERROR_COMMAND_FAILED;
  }

  for (const auto& info : iface_infos) {
    if (info.type == aDesired) {
      *aInfo = info;
      return nsIWifiResult::SUCCESS;
    }
  }
  return nsIWifiResult::ERROR_COMMAND_FAILED;
}

Result_t SupplicantStaManager::SetupStaInterface(
    const std::string& aInterfaceName) {
  MutexAutoLock lock(sLock);
  mInterfaceName = aInterfaceName;

  if (!mSupplicantStaIface) {
    // AIDL interface exposes AddSupplicantStaIface()
    mSupplicantStaIface = AddSupplicantStaIface();
  }

  if (!mSupplicantStaIface) {
    WIFI_LOGE(LOG_TAG, "Failed to create STA interface in supplicant");
    return nsIWifiResult::ERROR_COMMAND_FAILED;
  }

  // TODO: implement callback
  SupplicantStatus response;
  // Instantiate supplicant callback
  // android::sp<SupplicantStaIfaceCallback> supplicantCallback =
  //     new SupplicantStaIfaceCallback(mInterfaceName, mCallback,
  //                                    mPasspointCallback, this);
  // if (IsSupplicantV1_2()) {
  //   android::sp<SupplicantStaIfaceCallbackV1_1> supplicantCallbackV1_1 =
  //       new SupplicantStaIfaceCallbackV1_1(mInterfaceName, mCallback,
  //                                          supplicantCallback);
  //   android::sp<SupplicantStaIfaceCallbackV1_2> supplicantCallbackV1_2 =
  //       new SupplicantStaIfaceCallbackV1_2(mInterfaceName, mCallback,
  //                                          supplicantCallbackV1_1);
  //   HIDL_SET(GetSupplicantStaIfaceV1_2(), registerCallback_1_2,
  //            SupplicantStatus, response, supplicantCallbackV1_2);
  //   mSupplicantStaIfaceCallback = supplicantCallbackV1_2;
  // } else if (IsSupplicantV1_1()) {
  //   android::sp<SupplicantStaIfaceCallbackV1_1> supplicantCallbackV1_1 =
  //       new SupplicantStaIfaceCallbackV1_1(mInterfaceName, mCallback,
  //                                          supplicantCallback);
  //   HIDL_SET(GetSupplicantStaIfaceV1_1(), registerCallback_1_1,
  //            SupplicantStatus, response, supplicantCallbackV1_1);
  //   mSupplicantStaIfaceCallback = supplicantCallbackV1_1;
  // } else {
  //   HIDL_SET(mSupplicantStaIface, registerCallback, SupplicantStatus, response,
  //            supplicantCallback);
  //   mSupplicantStaIfaceCallback = supplicantCallback;
  // }

  if (response.code != SupplicantStatusCode::SUCCESS) {
    WIFI_LOGE(LOG_TAG, "registerCallback failed: %d", response.code);
    return nsIWifiResult::ERROR_COMMAND_FAILED;
  }

  return CHECK_SUCCESS(mSupplicantStaIface != nullptr &&
                       mSupplicantStaIfaceCallback != nullptr);
}

std::shared_ptr<aidl_sup::ISupplicantStaIface> SupplicantStaManager::GetSupplicantStaIface() {
  if (mSupplicant == nullptr) {
    return nullptr;
  }

  aidl_sup::IfaceInfo info;
  if (FindIfaceOfType(aidl_sup::IfaceType::STA, &info) !=
      nsIWifiResult::SUCCESS) {
    return nullptr;
  }

  
  std::shared_ptr<aidl_sup::ISupplicantStaIface> staIface;
  auto response = mSupplicant->getStaInterface(info.name, &staIface);

  if (!response.isOk()) {
    WIFI_LOGE(LOG_TAG, "Get supplicant STA iface failed: %u", response.getServiceSpecificError());
    return nullptr;
  }
  return staIface;
}

std::shared_ptr<aidl_sup::ISupplicantStaIface> SupplicantStaManager::AddSupplicantStaIface() {
  if (mSupplicant == nullptr) {
    return nullptr;
  }

  aidl_sup::IfaceInfo info;
  info.name = mInterfaceName;
  info.type = aidl_sup::IfaceType::STA;
  std::shared_ptr<aidl_sup::ISupplicantStaIface> staIface;
  // Here add a STA interface in supplicant.
  auto response = mSupplicant->addStaInterface(info.name, &staIface);

  if (!staIface) {
    WIFI_LOGE(LOG_TAG, "Add supplicant STA iface failed: %u", response.getServiceSpecificError());
    return nullptr;
  }
  return staIface;
}

android::sp<SupplicantStaNetwork> SupplicantStaManager::CreateStaNetwork() {
  MutexAutoLock lock(sLock);
  if (mSupplicantStaIface == nullptr) {
    return nullptr;
  }

  std::shared_ptr<aidl_sup::ISupplicantStaNetwork> staNetwork;
  auto response = mSupplicantStaIface->addNetwork(&staNetwork);

  if (!response.isOk()) {
    WIFI_LOGE(LOG_TAG, "Failed to add network with status code: %d",
              response.getServiceSpecificError());
    return nullptr;
  }
  return new SupplicantStaNetwork(mInterfaceName, mCallback, staNetwork);
}

android::sp<SupplicantStaNetwork> SupplicantStaManager::GetStaNetwork(
    uint32_t aNetId) const {
  if (mSupplicantStaIface == nullptr) {
    return nullptr;
  }

  std::shared_ptr<aidl_sup::ISupplicantStaNetwork> staNetwork;
  auto response = mSupplicantStaIface->getNetwork(aNetId, &staNetwork);
  if (!response.isOk()) {
    WIFI_LOGE(LOG_TAG, "Failed to get network with status code: %d",
              response.getServiceSpecificError());
    return nullptr;
  }
  return new SupplicantStaNetwork(mInterfaceName, mCallback, staNetwork);
}

android::sp<SupplicantStaNetwork> SupplicantStaManager::GetCurrentNetwork()
    const {
  std::unordered_map<std::string,
                     android::sp<SupplicantStaNetwork>>::const_iterator found =
      mCurrentNetwork.find(mInterfaceName);
  if (found == mCurrentNetwork.end()) {
    return nullptr;
  }
  return mCurrentNetwork.at(mInterfaceName);
}

NetworkConfiguration SupplicantStaManager::GetCurrentConfiguration() const {
  MutexAutoLock lock(sHashLock);
  std::unordered_map<std::string, NetworkConfiguration>::const_iterator config =
      mCurrentConfiguration.find(mInterfaceName);
  if (config == mCurrentConfiguration.end()) {
    return mDummyNetworkConfiguration;
  }
  return mCurrentConfiguration.at(mInterfaceName);
}

void SupplicantStaManager::ModifyConfigurationHash(
    int aAction, const NetworkConfiguration& aConfig) {
  MutexAutoLock lock(sHashLock);
  switch (aAction) {
    case CLEAN_ALL:
      mCurrentConfiguration.clear();
      break;
    case ERASE_CONFIG:
      mCurrentConfiguration.erase(mInterfaceName);
      break;
    case ADD_CONFIG:
      mCurrentConfiguration.insert(std::make_pair(mInterfaceName, aConfig));
      break;
  }
}

int32_t SupplicantStaManager::GetCurrentNetworkId() const {
  return GetCurrentConfiguration().mNetworkId;
}

bool SupplicantStaManager::IsCurrentEapNetwork() {
  NetworkConfiguration current = GetCurrentConfiguration();
  return current.IsValidNetwork() ? current.IsEapNetwork() : false;
}

bool SupplicantStaManager::IsCurrentPskNetwork() {
  NetworkConfiguration current = GetCurrentConfiguration();
  return current.IsValidNetwork() ? current.IsPskNetwork() : false;
}

bool SupplicantStaManager::IsCurrentSaeNetwork() {
  NetworkConfiguration current = GetCurrentConfiguration();
  return current.IsValidNetwork() ? current.IsSaeNetwork() : false;
}

bool SupplicantStaManager::IsCurrentWepNetwork() {
  NetworkConfiguration current = GetCurrentConfiguration();
  return current.IsValidNetwork() ? current.IsWepNetwork() : false;
}

Result_t SupplicantStaManager::ConnectToNetwork(ConfigurationOptions* aConfig) {
  Result_t result = nsIWifiResult::ERROR_UNKNOWN;
  android::sp<SupplicantStaNetwork> staNetwork;
  NetworkConfiguration config(aConfig);
  NetworkConfiguration existConfig = GetCurrentConfiguration();

  if (!CompareConfiguration(existConfig, mDummyNetworkConfiguration) &&
      CompareConfiguration(existConfig, config)) {
    staNetwork = GetCurrentNetwork();
    if (staNetwork == nullptr) {
      WIFI_LOGE(LOG_TAG, "Network is not available");
      return nsIWifiResult::ERROR_COMMAND_FAILED;
    }
    if (existConfig.mBssid.compare(config.mBssid)) {
      WIFI_LOGD(LOG_TAG, "Network is already saved, but need to update BSSID.");
      result = staNetwork->UpdateBssid(config.mBssid);
      if (result != nsIWifiResult::SUCCESS) {
        WIFI_LOGE(LOG_TAG, "Failed to update BSSID.");
        return result;
      }

      // update configuration
      ModifyConfigurationHash(ERASE_CONFIG, mDummyNetworkConfiguration);
      ModifyConfigurationHash(ADD_CONFIG, config);
    } else {
      WIFI_LOGD(LOG_TAG, "Same network, do not need to create a new one");
    }
  } else {
    ModifyConfigurationHash(ERASE_CONFIG, mDummyNetworkConfiguration);
    mCurrentNetwork.erase(mInterfaceName);

    // remove all supplicant networks
    result = RemoveNetworks();
    if (result != nsIWifiResult::SUCCESS) {
      WIFI_LOGE(LOG_TAG, "Failed to remove supplicant networks");
      return result;
    }

    // create network in supplicant and set configuration
    staNetwork = CreateStaNetwork();
    if (!staNetwork) {
      WIFI_LOGE(LOG_TAG, "Failed to create STA network");
      return nsIWifiResult::ERROR_INVALID_INTERFACE;
    }

    // set network configuration into supplicant
    result = staNetwork->SetConfiguration(config);
    if (result != nsIWifiResult::SUCCESS) {
      WIFI_LOGE(LOG_TAG, "Failed to set wifi configuration");
      ModifyConfigurationHash(CLEAN_ALL, mDummyNetworkConfiguration);
      mCurrentNetwork.clear();
      return result;
    }
    ModifyConfigurationHash(ADD_CONFIG, config);
    mCurrentNetwork.insert(std::make_pair(mInterfaceName, staNetwork));
  }

  // success, start to make connection
  staNetwork->SelectNetwork();
  return nsIWifiResult::SUCCESS;
}

Result_t SupplicantStaManager::Reconnect() {
  MutexAutoLock lock(sLock);
  if (mSupplicantStaIface == nullptr) {
    return nsIWifiResult::ERROR_INVALID_INTERFACE;
  }
  return mSupplicantStaIface->reconnect().isOk();
}

Result_t SupplicantStaManager::Reassociate() {
  MutexAutoLock lock(sLock);
  if (mSupplicantStaIface == nullptr) {
    return nsIWifiResult::ERROR_INVALID_INTERFACE;
  }
  return mSupplicantStaIface->reassociate().isOk();
}

Result_t SupplicantStaManager::Disconnect() {
  MutexAutoLock lock(sLock);
  if (mSupplicantStaIface == nullptr) {
    return nsIWifiResult::ERROR_INVALID_INTERFACE;
  }
  return mSupplicantStaIface->disconnect().isOk();
}

Result_t SupplicantStaManager::EnableNetwork() {
  android::sp<SupplicantStaNetwork> network;

  network = GetCurrentNetwork();
  if (network == nullptr) {
    return nsIWifiResult::ERROR_COMMAND_FAILED;
  }
  return network->EnableNetwork();
}

Result_t SupplicantStaManager::DisableNetwork() {
  android::sp<SupplicantStaNetwork> network;

  network = GetCurrentNetwork();
  if (network == nullptr) {
    return nsIWifiResult::ERROR_COMMAND_FAILED;
  }
  return network->DisableNetwork();
}

Result_t SupplicantStaManager::GetNetwork(nsWifiResult* aResult) {
  MutexAutoLock lock(sLock);
  if (!mSupplicantStaIface) {
    return nsIWifiResult::ERROR_INVALID_INTERFACE;
  }

  std::vector<int32_t> netIds;
  auto response = mSupplicantStaIface->listNetworks(&netIds);

  if (!response.isOk()) {
    WIFI_LOGE(LOG_TAG, "Failed to query saved networks in supplicant");
    return nsIWifiResult::ERROR_COMMAND_FAILED;
  }

  // By current design, network selection logic is processed in framework,
  // so there should only be at most one network in supplicant.
  if (netIds.size() != 1) {
    WIFI_LOGE(LOG_TAG, "Should only keep one network in supplicant");
    return nsIWifiResult::ERROR_UNKNOWN;
  }

  android::sp<SupplicantStaNetwork> network = GetStaNetwork(netIds.at(0));
  if (!network) {
    WIFI_LOGE(LOG_TAG, "Failed to get network %d", netIds.at(0));
    return nsIWifiResult::ERROR_COMMAND_FAILED;
  }

  // Load network configuration from supplicant
  NetworkConfiguration config;
  network->LoadConfiguration(config);

  // Assign to nsWifiConfiguration
  RefPtr<nsWifiConfiguration> configuration = new nsWifiConfiguration();
  config.ConvertConfigurations(configuration);
  aResult->updateWifiConfiguration(configuration);
  return nsIWifiResult::SUCCESS;
}

Result_t SupplicantStaManager::RemoveNetworks() {
  MutexAutoLock lock(sLock);
  if (!mSupplicantStaIface) {
    return nsIWifiResult::ERROR_INVALID_INTERFACE;
  }

  // first, get network id list from supplicant
  std::vector<int32_t> netIds;
  auto response = mSupplicantStaIface->listNetworks(&netIds);

  if (!response.isOk()) {
    WIFI_LOGE(LOG_TAG, "Failed to query saved networks in supplicant");
    return nsIWifiResult::ERROR_COMMAND_FAILED;
  }

  // remove network
  for (uint32_t id : netIds) {
    response = mSupplicantStaIface->removeNetwork(id);
    if (!response.isOk()) {
      WIFI_LOGE(LOG_TAG, "Failed to remove network %d", id);
      return nsIWifiResult::ERROR_COMMAND_FAILED;
    }
  }

  ModifyConfigurationHash(ERASE_CONFIG, mDummyNetworkConfiguration);
  mCurrentNetwork.erase(mInterfaceName);
  return nsIWifiResult::SUCCESS;
}

Result_t SupplicantStaManager::RoamToNetwork(ConfigurationOptions* aConfig) {
  NetworkConfiguration config(aConfig);
  NetworkConfiguration current = GetCurrentConfiguration();

  if (config.mNetworkId == INVALID_NETWORK_ID) {
    return nsIWifiResult::ERROR_INVALID_ARGS;
  }

  if (current.mNetworkId == INVALID_NETWORK_ID ||
      config.mNetworkId != current.mNetworkId ||
      config.GetNetworkKey().compare(current.GetNetworkKey())) {
    return ConnectToNetwork(aConfig);
  }

  // now we are ready to roam to the target network.
  android::sp<SupplicantStaNetwork> network =
      mCurrentNetwork.at(mInterfaceName);

  Result_t result = network->UpdateBssid(config.mBssid);

  WIFI_LOGD(LOG_TAG, "Trying to roam to network %s[%s]", config.mSsid.c_str(),
            config.mBssid.c_str());
  return (result == nsIWifiResult::SUCCESS) ? Reassociate() : result;
}

Result_t SupplicantStaManager::SendEapSimIdentityResponse(
    SimIdentityRespDataOptions* aIdentity) {
  android::sp<SupplicantStaNetwork> network;

  network = GetCurrentNetwork();
  if (network == nullptr) {
    return nsIWifiResult::ERROR_COMMAND_FAILED;
  }
  return network->SendEapSimIdentityResponse(aIdentity);
}

Result_t SupplicantStaManager::SendEapSimGsmAuthResponse(
    const nsTArray<SimGsmAuthRespDataOptions>& aGsmAuthResp) {
  android::sp<SupplicantStaNetwork> network;

  network = GetCurrentNetwork();
  if (network == nullptr) {
    return nsIWifiResult::ERROR_COMMAND_FAILED;
  }
  return network->SendEapSimGsmAuthResponse(aGsmAuthResp);
}

Result_t SupplicantStaManager::SendEapSimGsmAuthFailure() {
  android::sp<SupplicantStaNetwork> network;

  network = GetCurrentNetwork();
  if (network == nullptr) {
    return nsIWifiResult::ERROR_COMMAND_FAILED;
  }
  return network->SendEapSimGsmAuthFailure();
}

Result_t SupplicantStaManager::SendEapSimUmtsAuthResponse(
    SimUmtsAuthRespDataOptions* aUmtsAuthResp) {
  android::sp<SupplicantStaNetwork> network;

  network = GetCurrentNetwork();
  if (network == nullptr) {
    return nsIWifiResult::ERROR_COMMAND_FAILED;
  }
  return network->SendEapSimUmtsAuthResponse(aUmtsAuthResp);
}

Result_t SupplicantStaManager::SendEapSimUmtsAutsResponse(
    SimUmtsAutsRespDataOptions* aUmtsAutsResp) {
  android::sp<SupplicantStaNetwork> network;

  network = GetCurrentNetwork();
  if (network == nullptr) {
    return nsIWifiResult::ERROR_COMMAND_FAILED;
  }
  return network->SendEapSimUmtsAutsResponse(aUmtsAutsResp);
}

Result_t SupplicantStaManager::SendEapSimUmtsAuthFailure() {
  android::sp<SupplicantStaNetwork> network;

  network = GetCurrentNetwork();
  if (network == nullptr) {
    return nsIWifiResult::ERROR_COMMAND_FAILED;
  }
  return network->SendEapSimUmtsAuthFailure();
}

Result_t SupplicantStaManager::SendAnqpRequest(
    const std::vector<uint8_t>& aBssid,
    const std::vector<aidl_sup::AnqpInfoId>& aInfoElements,
    const std::vector<aidl_sup::Hs20AnqpSubtypes>& aHs20SubTypes) {
  MutexAutoLock lock(sLock);
  if (!mSupplicantStaIface) {
    return nsIWifiResult::ERROR_INVALID_INTERFACE;
  }

  auto response = mSupplicantStaIface->initiateAnqpQuery(aBssid, aInfoElements, aHs20SubTypes);
  return response.isOk();
}

/**
 * Methods to handle WPS connection
 */
Result_t SupplicantStaManager::InitWpsDetail() {
  char value[PROPERTY_VALUE_MAX];

  property_get("ro.product.name", value, "");
  std::string deviceName(value);
  if (SetWpsDeviceName(deviceName) != nsIWifiResult::SUCCESS) {
    WIFI_LOGE(LOG_TAG, "Failed to set device name: %s", value);
  }

  property_get("ro.product.manufacturer", value, "");
  std::string manufacturer(value);
  if (SetWpsManufacturer(manufacturer) != nsIWifiResult::SUCCESS) {
    WIFI_LOGE(LOG_TAG, "Failed to set manufacturer: %s", value);
  }

  property_get("ro.product.model", value, "");
  std::string modelName(value);
  if (SetWpsModelName(modelName) != nsIWifiResult::SUCCESS) {
    WIFI_LOGE(LOG_TAG, "Failed to set model name: %s", value);
  }

  property_get("ro.product.model", value, "");
  std::string modelNumber(value);
  if (SetWpsModelNumber(modelNumber) != nsIWifiResult::SUCCESS) {
    WIFI_LOGE(LOG_TAG, "Failed to set model number: %s", value);
  }

  property_get("ro.serialno", value, "");
  std::string serialNumber(value);
  if (SetWpsSerialNumber(serialNumber) != nsIWifiResult::SUCCESS) {
    WIFI_LOGE(LOG_TAG, "Failed to set serial number: %s", value);
  }

  if (SetWpsDeviceType(WPS_DEV_TYPE_DUAL_SMARTPHONE) !=
      nsIWifiResult::SUCCESS) {
    WIFI_LOGE(LOG_TAG, "Failed to set device type");
  }

  std::string configMethods("keypad physical_display virtual_push_button");
  if (SetWpsConfigMethods(configMethods) != nsIWifiResult::SUCCESS) {
    WIFI_LOGE(LOG_TAG, "Failed to set WPS config methods");
  }
  return nsIWifiResult::SUCCESS;
}

Result_t SupplicantStaManager::StartWpsRegistrar(const std::string& aBssid,
                                                 const std::string& aPinCode) {
  MutexAutoLock lock(sLock);
  if (!mSupplicantStaIface) {
    return nsIWifiResult::ERROR_INVALID_INTERFACE;
  }

  std::vector<uint8_t> bssid;
  bssid.reserve(6);

  if (aBssid.empty() || aBssid.compare(ANY_MAC_STR) == 0) {
    for (int i = 0; i < 6; i++) {
      bssid[i] = 0;
    }
  } else {
    ConvertMacToByteArray(aBssid, bssid);
  }

  auto response = mSupplicantStaIface->startWpsRegistrar(bssid, aPinCode);
  return response.isOk();
}

Result_t SupplicantStaManager::StartWpsPbc(const std::string& aBssid) {
  MutexAutoLock lock(sLock);
  if (!mSupplicantStaIface) {
    return nsIWifiResult::ERROR_INVALID_INTERFACE;
  }

  std::vector<uint8_t> bssid;
  bssid.reserve(6);

  if (aBssid.empty() || aBssid.compare(ANY_MAC_STR) == 0) {
    for (int i = 0; i < 6; i++) {
      bssid[i] = 0;
    }
  } else {
    ConvertMacToByteArray(aBssid, bssid);
  }

  auto response = mSupplicantStaIface->startWpsPbc(bssid);
  return response.isOk();
}

Result_t SupplicantStaManager::StartWpsPinKeypad(const std::string& aPinCode) {
  MutexAutoLock lock(sLock);
  if (!mSupplicantStaIface) {
    return nsIWifiResult::ERROR_INVALID_INTERFACE;
  }

  auto response = mSupplicantStaIface->startWpsPinKeypad(aPinCode);
  return response.isOk();
}

Result_t SupplicantStaManager::StartWpsPinDisplay(const std::string& aBssid,
                                                  nsAString& aGeneratedPin) {
  MutexAutoLock lock(sLock);
  if (!mSupplicantStaIface) {
    return nsIWifiResult::ERROR_INVALID_INTERFACE;
  }

  std::vector<uint8_t> bssid;
  bssid.reserve(6);

  if (aBssid.empty() || aBssid.compare(ANY_MAC_STR) == 0) {
    for (int i = 0; i < 6; i++) {
      bssid[i] = 0;
    }
  } else {
    ConvertMacToByteArray(aBssid, bssid);
  }

  std::string generatedPin;
  auto response = mSupplicantStaIface->startWpsPinDisplay(bssid, &generatedPin);
  if (response.isOk()) {
    nsString pin(NS_ConvertUTF8toUTF16(generatedPin.c_str()));
    aGeneratedPin.Assign(pin);
  }

  return response.isOk();
}

Result_t SupplicantStaManager::CancelWps() {
  MutexAutoLock lock(sLock);
  if (!mSupplicantStaIface) {
    return nsIWifiResult::ERROR_INVALID_INTERFACE;
  }

  auto response = mSupplicantStaIface->cancelWps();
  return response.isOk();
}

Result_t SupplicantStaManager::SetWpsDeviceName(
    const std::string& aDeviceName) {
  MutexAutoLock lock(sLock);
  if (!mSupplicantStaIface) {
    return nsIWifiResult::ERROR_INVALID_INTERFACE;
  }

  auto response = mSupplicantStaIface->setWpsDeviceName(aDeviceName);
  return response.isOk();
}

Result_t SupplicantStaManager::SetWpsDeviceType(
    const std::string& aDeviceType) {
  MutexAutoLock lock(sLock);
  if (!mSupplicantStaIface) {
    return nsIWifiResult::ERROR_INVALID_INTERFACE;
  }
  std::vector<std::string> tokens;
  std::string token;
  std::stringstream stream(aDeviceType);
  while (std::getline(stream, token, '-')) {
    tokens.push_back(token);
  }

  std::array<uint8_t, 2> category;
  std::array<uint8_t, 4> oui;
  std::array<uint8_t, 2> subCategory;

  uint16_t cat = std::stoi(tokens.at(0));
  category.at(0) = (cat >> 8) & 0xFF;
  category.at(1) = (cat >> 0) & 0xFF;

  ConvertHexStringToByteArray(tokens.at(1), oui);

  uint16_t subCat = std::stoi(tokens.at(2));
  subCategory.at(0) = (subCat >> 8) & 0xFF;
  subCategory.at(1) = (subCat >> 0) & 0xFF;

  std::vector<uint8_t> type;
  type.reserve(8);
  std::copy(category.cbegin(), category.cend(), type.begin());
  std::copy(oui.cbegin(), oui.cend(), type.begin() + 2);
  std::copy(subCategory.cbegin(), subCategory.cend(), type.begin() + 6);

  auto response = mSupplicantStaIface->setWpsDeviceType(type);
  return response.isOk();
}

Result_t SupplicantStaManager::SetWpsManufacturer(
    const std::string& aManufacturer) {
  MutexAutoLock lock(sLock);
  if (!mSupplicantStaIface) {
    return nsIWifiResult::ERROR_INVALID_INTERFACE;
  }

  auto response = mSupplicantStaIface->setWpsManufacturer(aManufacturer);
  return response.isOk();
}

Result_t SupplicantStaManager::SetWpsModelName(const std::string& aModelName) {
  MutexAutoLock lock(sLock);
  if (!mSupplicantStaIface) {
    return nsIWifiResult::ERROR_INVALID_INTERFACE;
  }

  auto response = mSupplicantStaIface->setWpsModelName(aModelName);
  return response.isOk();
}

Result_t SupplicantStaManager::SetWpsModelNumber(
    const std::string& aModelNumber) {
  MutexAutoLock lock(sLock);
  if (!mSupplicantStaIface) {
    return nsIWifiResult::ERROR_INVALID_INTERFACE;
  }

  auto response = mSupplicantStaIface->setWpsModelNumber(aModelNumber);
  return response.isOk();
}

Result_t SupplicantStaManager::SetWpsSerialNumber(
    const std::string& aSerialNumber) {
  MutexAutoLock lock(sLock);
  if (!mSupplicantStaIface) {
    return nsIWifiResult::ERROR_INVALID_INTERFACE;
  }

  auto response = mSupplicantStaIface->setWpsSerialNumber(aSerialNumber);
  return response.isOk();
}

Result_t SupplicantStaManager::SetWpsConfigMethods(
    const std::string& aConfigMethods) {
  MutexAutoLock lock(sLock);
  if (!mSupplicantStaIface) {
    return nsIWifiResult::ERROR_INVALID_INTERFACE;
  }

  aidl_sup::WpsConfigMethods configMethodMask = ConvertToWpsConfigMethod(aConfigMethods);
  auto response = mSupplicantStaIface->setWpsConfigMethods(configMethodMask);
  return response.isOk();
}

aidl_sup::WpsConfigMethods SupplicantStaManager::ConvertToWpsConfigMethod(
    const std::string& aConfigMethod) {
  int16_t mask = 0;
  std::string method;
  std::stringstream stream(aConfigMethod);
  while (std::getline(stream, method, ' ')) {
    if (method.compare("usba") == 0) {
      mask |= (int16_t)aidl_sup::WpsConfigMethods::USBA;
    } else if (method.compare("ethernet") == 0) {
      mask |= (int16_t)aidl_sup::WpsConfigMethods::ETHERNET;
    } else if (method.compare("label") == 0) {
      mask |= (int16_t)aidl_sup::WpsConfigMethods::LABEL;
    } else if (method.compare("display") == 0) {
      mask |= (int16_t)aidl_sup::WpsConfigMethods::DISPLAY;
    } else if (method.compare("int_nfc_token") == 0) {
      mask |= (int16_t)aidl_sup::WpsConfigMethods::INT_NFC_TOKEN;
    } else if (method.compare("ext_nfc_token") == 0) {
      mask |= (int16_t)aidl_sup::WpsConfigMethods::EXT_NFC_TOKEN;
    } else if (method.compare("nfc_interface") == 0) {
      mask |= (int16_t)aidl_sup::WpsConfigMethods::NFC_INTERFACE;
    } else if (method.compare("push_button") == 0) {
      mask |= (int16_t)aidl_sup::WpsConfigMethods::PUSHBUTTON;
    } else if (method.compare("keypad") == 0) {
      mask |= (int16_t)aidl_sup::WpsConfigMethods::KEYPAD;
    } else if (method.compare("virtual_push_button") == 0) {
      mask |= (int16_t)aidl_sup::WpsConfigMethods::VIRT_PUSHBUTTON;
    } else if (method.compare("physical_push_button") == 0) {
      mask |= (int16_t)aidl_sup::WpsConfigMethods::PHY_PUSHBUTTON;
    } else if (method.compare("p2ps") == 0) {
      mask |= (int16_t)aidl_sup::WpsConfigMethods::P2PS;
    } else if (method.compare("virtual_display") == 0) {
      mask |= (int16_t)aidl_sup::WpsConfigMethods::VIRT_DISPLAY;
    } else if (method.compare("physical_display") == 0) {
      mask |= (int16_t)aidl_sup::WpsConfigMethods::PHY_DISPLAY;
    } else {
      WIFI_LOGE(LOG_TAG, "Unknown wps config method: %s", method.c_str());
    }
  }
  return (aidl_sup::WpsConfigMethods)mask;
}

/**
 * P2P functions
 */
std::shared_ptr<aidl_sup::ISupplicantP2pIface> SupplicantStaManager::GetSupplicantP2pIface() {
  if (mSupplicant == nullptr) {
    return nullptr;
  }

  aidl_sup::IfaceInfo info;
  if (FindIfaceOfType(aidl_sup::IfaceType::P2P, &info) !=
      nsIWifiResult::SUCCESS) {
    return nullptr;
  }

  std::shared_ptr<aidl_sup::ISupplicantP2pIface> p2pIface;
  auto response = mSupplicant->getP2pInterface(info.name, &p2pIface);

  if (!response.isOk()) {
    WIFI_LOGE(LOG_TAG, "Get supplicant P2P iface failed: %u", response.getServiceSpecificError());
    return nullptr;
  }
  return p2pIface;
}

Result_t SupplicantStaManager::SetupP2pInterface() {
  MutexAutoLock lock(sLock);
  std::shared_ptr<aidl_sup::ISupplicantP2pIface> p2p_iface = GetSupplicantP2pIface();
  if (!p2p_iface) {
    return nsIWifiResult::ERROR_INVALID_INTERFACE;
  }

  auto response = p2p_iface->saveConfig();
  WIFI_LOGD(LOG_TAG, "[P2P] save config: %d", response.getServiceSpecificError());
  return nsIWifiResult::SUCCESS;
}

void SupplicantStaManager::RegisterDeathHandler(
    SupplicantDeathEventHandler* aHandler) {
  mDeathEventHandler = aHandler;
}

void SupplicantStaManager::UnregisterDeathHandler() {
  mDeathEventHandler = nullptr;
}

void SupplicantStaManager::SupplicantServiceDiedHandler(int32_t aCookie) {
  if (mDeathRecipientCookie != aCookie) {
    WIFI_LOGD(LOG_TAG, "Ignoring stale death recipient notification");
    return;
  }

  // TODO: notify supplicant has died.
  if (mDeathEventHandler != nullptr) {
    mDeathEventHandler->OnDeath();
  }
}

bool SupplicantStaManager::CompareConfiguration(
    const NetworkConfiguration& aOld, const NetworkConfiguration& aNew) {
  if (aOld.mNetworkId != aNew.mNetworkId) {
    return false;
  }
  if (aOld.mSsid.compare(aNew.mSsid)) {
    return false;
  }
  if (!CompareCredential(aOld, aNew)) {
    return false;
  }

  return true;
}

bool SupplicantStaManager::CompareCredential(const NetworkConfiguration& aOld,
                                             const NetworkConfiguration& aNew) {
  if (aOld.mKeyMgmt.compare(aNew.mKeyMgmt)) {
    return false;
  }
  if (aOld.mPsk.compare(aNew.mPsk)) {
    return false;
  }
  // TODO: complete the rest credentials comparation.

  return true;
}

/**
 * Hal wrapper functions
 */
android::sp<IServiceManager> SupplicantStaManager::GetServiceManager() {
  return mServiceManager ? mServiceManager : IServiceManager::getService();
}

std::shared_ptr<aidl_sup::ISupplicant> SupplicantStaManager::GetSupplicant() {
  return mSupplicant; // ? mSupplicant : ISupplicant::getService();
}

// android::sp<ISupplicantV1_1> SupplicantStaManager::GetSupplicantV1_1() {
//   return nullptr; //ISupplicantV1_1::castFrom(GetSupplicant());
// }

// android::sp<ISupplicantV1_2> SupplicantStaManager::GetSupplicantV1_2() {
//   return nullptr; //ISupplicantV1_2::castFrom(GetSupplicant());
// }

// android::sp<ISupplicantStaIfaceV1_1>
// SupplicantStaManager::GetSupplicantStaIfaceV1_1() {
//   return ISupplicantStaIfaceV1_1::castFrom(mSupplicantStaIface);
// }

// android::sp<ISupplicantStaIfaceV1_2>
// SupplicantStaManager::GetSupplicantStaIfaceV1_2() {
//   return ISupplicantStaIfaceV1_2::castFrom(mSupplicantStaIface);
// }

// bool SupplicantStaManager::IsSupplicantV1_1() {
//   WIFI_LOGE(LOG_TAG, "IsSupplicantV1_1 %d", SupplicantVersionSupported(SUPPLICANT_INTERFACE_NAME_V1_1));
//   return SupplicantVersionSupported(SUPPLICANT_INTERFACE_NAME_V1_1);
// }

// bool SupplicantStaManager::IsSupplicantV1_2() {
//   WIFI_LOGE(LOG_TAG, "IsSupplicantV1_2 %d", SupplicantVersionSupported(SUPPLICANT_INTERFACE_NAME_V1_2));
//   return SupplicantVersionSupported(SUPPLICANT_INTERFACE_NAME_V1_2);
// }

// bool SupplicantStaManager::SupplicantVersionSupported(const std::string& name) {
//   if (!mServiceManager) {
//     return false;
//   }

//   return mServiceManager->getTransport(name, HAL_INSTANCE_NAME) !=
//          IServiceManager::Transport::EMPTY;
// }

/**
 * Helper functions to notify event callback for ISupplicantCallback.
 */
void SupplicantStaManager::NotifyTerminating() {
  nsCString iface(mInterfaceName);
  RefPtr<nsWifiEvent> event = new nsWifiEvent(EVENT_SUPPLICANT_TERMINATING);

  INVOKE_CALLBACK(mCallback, event, iface);
}

/**
 * ISupplicantCallback implementation
 */
Return<void> SupplicantStaManager::onInterfaceCreated(
    const hidl_string& ifName) {
  WIFI_LOGD(LOG_TAG, "SupplicantCallback.onInterfaceCreated(): %s",
            ifName.c_str());
  return android::hardware::Void();
}

Return<void> SupplicantStaManager::onInterfaceRemoved(
    const hidl_string& ifName) {
  WIFI_LOGD(LOG_TAG, "SupplicantCallback.onInterfaceRemoved(): %s",
            ifName.c_str());
  return android::hardware::Void();
}

Return<void> SupplicantStaManager::onTerminating() {
  MutexAutoLock lock(sLock);
  WIFI_LOGD(LOG_TAG, "SupplicantCallback.onTerminating()");

  NotifyTerminating();
  return android::hardware::Void();
}

// ndk::SpAIBinder SupplicantStaManager::asBinder() {
//   return mSupplicant->asBinder();
// }

// bool SupplicantStaManager::isRemote() {
//   return false;
// }

// ndk::ScopedAStatus SupplicantStaManager::getInterfaceVersion(int32_t* aidl_return) {
//   *aidl_return = 1;
//   return ndk::ScopedAStatus::ok();
// }

// ndk::ScopedAStatus SupplicantStaManager::getInterfaceHash(std::string* _aidl_return) {
//   // TODO: implement properly
//   return ndk::ScopedAStatus::ok();
// }