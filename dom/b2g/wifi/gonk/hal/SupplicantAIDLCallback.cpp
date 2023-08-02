#define LOG_TAG "SupplicantAIDLCallback"

#include "WifiCommon.h"
#include "SupplicantAIDLCallback.h"

#define EVENT_SUPPLICANT_STATE_CHANGED u"SUPPLICANT_STATE_CHANGED"_ns
#define EVENT_SUPPLICANT_NETWORK_CONNECTED u"SUPPLICANT_NETWORK_CONNECTED"_ns

/**
 * SupplicantStaIfaceCallback implementation
 */
SupplicantStaIfaceCallback::SupplicantStaIfaceCallback(
    const std::string& aInterfaceName,
    const android::sp<WifiEventCallback>& aCallback,
    const android::sp<SupplicantStaManager> aSupplicantManager)
    : mStateBeforeDisconnect(::android::hardware::wifi::supplicant::StaIfaceCallbackState::INACTIVE),
      mInterfaceName(aInterfaceName),
      mCallback(aCallback),
      mSupplicantManager(aSupplicantManager) {}

::android::binder::Status SupplicantStaIfaceCallback::onStateChanged(
    ::android::hardware::wifi::supplicant::StaIfaceCallbackState newState,
    const ::std::vector<uint8_t> &bssid, int32_t id,
    const ::std::vector<uint8_t> &ssid, bool filsHlpSent) {

  WIFI_LOGD(LOG_TAG, "ISupplicantStaIfaceCallback.onStateChanged()");

  std::string bssidStr = ConvertMacToString(bssid);
  std::string ssidStr(ssid.begin(), ssid.end());

  if (newState != ::android::hardware::wifi::supplicant::StaIfaceCallbackState::DISCONNECTED) {
    mStateBeforeDisconnect = newState;
  }
  if (newState == ::android::hardware::wifi::supplicant::StaIfaceCallbackState::COMPLETED) {
    NotifyConnected(ssidStr, bssidStr);
  }

  NotifyStateChanged((uint32_t)newState, ssidStr, bssidStr);
  return ::android::binder::Status::fromStatusT(::android::OK);
}

void SupplicantStaIfaceCallback::NotifyConnected(const std::string& aSsid,
                                                 const std::string& aBssid) {
  nsCString iface(mInterfaceName);
  RefPtr<nsWifiEvent> event =
      new nsWifiEvent(EVENT_SUPPLICANT_NETWORK_CONNECTED);
  event->mBssid = NS_ConvertUTF8toUTF16(aBssid.c_str());

  INVOKE_CALLBACK(mCallback, event, iface);
}

void SupplicantStaIfaceCallback::NotifyStateChanged(uint32_t aState,
                                                    const std::string& aSsid,
                                                    const std::string& aBssid) {
  int32_t networkId = INVALID_NETWORK_ID;
  if (mSupplicantManager) {
    networkId = mSupplicantManager->GetCurrentNetworkId();
  }

  nsCString iface(mInterfaceName);
  RefPtr<nsWifiEvent> event = new nsWifiEvent(EVENT_SUPPLICANT_STATE_CHANGED);
  RefPtr<nsStateChanged> stateChanged = new nsStateChanged(
      aState, networkId, NS_ConvertUTF8toUTF16(aBssid.c_str()),
      NS_ConvertUTF8toUTF16(aSsid.c_str()));
  event->updateStateChanged(stateChanged);

  INVOKE_CALLBACK(mCallback, event, iface);
}