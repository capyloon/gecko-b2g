/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=8 sts=2 et sw=2 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef nsRilIndication_H
#define nsRilIndication_H
#include <nsISupportsImpl.h>
#include <nsTArray.h>

#include "nsRadioVersion.h"

#if RADIO_HAL >= 14
#  include <android/hardware/radio/1.4/IRadio.h>
#  include <android/hardware/radio/1.4/IRadioResponse.h>
#  include <android/hardware/radio/1.4/IRadioIndication.h>
using RADIO_1_4::IRadioIndication;
#else
#  include <android/hardware/radio/1.1/IRadio.h>
#  include <android/hardware/radio/1.1/IRadioResponse.h>
#  include <android/hardware/radio/1.1/IRadioIndication.h>
using RADIO_1_1::IRadioIndication;
#endif

class nsRilWorker;

class nsRilIndication : public IRadioIndication {
 public:
  RefPtr<nsRilWorker> mRIL;
  explicit nsRilIndication(nsRilWorker* aRil);
  ~nsRilIndication();

  Return<void> radioStateChanged(RadioIndicationType type,
                                 RadioState radioState);

  Return<void> callStateChanged(RadioIndicationType type);

  Return<void> networkStateChanged(RadioIndicationType type);

  Return<void> newSms(RadioIndicationType type, const hidl_vec<uint8_t>& pdu);

  Return<void> newSmsStatusReport(RadioIndicationType type,
                                  const hidl_vec<uint8_t>& pdu);

  Return<void> newSmsOnSim(RadioIndicationType type, int32_t recordNumber);

  Return<void> onUssd(RadioIndicationType type, UssdModeType modeType,
                      const hidl_string& msg);

  Return<void> nitzTimeReceived(RadioIndicationType type,
                                const hidl_string& nitzTime,
                                uint64_t receivedTime);

  Return<void> currentSignalStrength(
      RadioIndicationType type, const RADIO_1_0::SignalStrength& sig_strength);

  Return<void> dataCallListChanged(
      RadioIndicationType type,
      const hidl_vec<RADIO_1_0::SetupDataCallResult>& dcList);

  Return<void> suppSvcNotify(RadioIndicationType type,
                             const SuppSvcNotification& suppSvc);

  Return<void> stkSessionEnd(RadioIndicationType type);

  Return<void> stkProactiveCommand(RadioIndicationType type,
                                   const hidl_string& cmd);

  Return<void> stkEventNotify(RadioIndicationType type, const hidl_string& cmd);

  Return<void> stkCallSetup(RadioIndicationType type, int64_t timeout);

  Return<void> simSmsStorageFull(RadioIndicationType type);

  Return<void> simRefresh(RadioIndicationType type,
                          const SimRefreshResult& refreshResult);

  Return<void> callRing(RadioIndicationType type, bool isGsm,
                        const CdmaSignalInfoRecord& record);

  Return<void> simStatusChanged(RadioIndicationType type);

  Return<void> cdmaNewSms(RadioIndicationType type, const CdmaSmsMessage& msg);

  Return<void> newBroadcastSms(RadioIndicationType type,
                               const hidl_vec<uint8_t>& data);

  Return<void> cdmaRuimSmsStorageFull(RadioIndicationType type);

  Return<void> restrictedStateChanged(RadioIndicationType type,
                                      PhoneRestrictedState state);

  Return<void> enterEmergencyCallbackMode(RadioIndicationType type);

  Return<void> cdmaCallWaiting(RadioIndicationType type,
                               const CdmaCallWaiting& callWaitingRecord);

  Return<void> cdmaOtaProvisionStatus(RadioIndicationType type,
                                      CdmaOtaProvisionStatus status);

  Return<void> cdmaInfoRec(RadioIndicationType type,
                           const CdmaInformationRecords& records);

  Return<void> indicateRingbackTone(RadioIndicationType type, bool start);

  Return<void> resendIncallMute(RadioIndicationType type);

  Return<void> cdmaSubscriptionSourceChanged(RadioIndicationType type,
                                             CdmaSubscriptionSource cdmaSource);

  Return<void> cdmaPrlChanged(RadioIndicationType type, int32_t version);

  Return<void> exitEmergencyCallbackMode(RadioIndicationType type);

  Return<void> rilConnected(RadioIndicationType type);

  Return<void> voiceRadioTechChanged(RadioIndicationType type,
                                     RADIO_1_0::RadioTechnology rat);

  Return<void> cellInfoList(RadioIndicationType type,
                            const hidl_vec<RADIO_1_0::CellInfo>& records);

  Return<void> imsNetworkStateChanged(RadioIndicationType type);

  Return<void> subscriptionStatusChanged(RadioIndicationType type,
                                         bool activate);

  Return<void> srvccStateNotify(RadioIndicationType type, SrvccState state);

  Return<void> hardwareConfigChanged(RadioIndicationType type,
                                     const hidl_vec<HardwareConfig>& configs);

  Return<void> radioCapabilityIndication(RadioIndicationType type,
                                         const RADIO_1_0::RadioCapability& rc);

  Return<void> onSupplementaryServiceIndication(RadioIndicationType type,
                                                const StkCcUnsolSsResult& ss);

  Return<void> stkCallControlAlphaNotify(RadioIndicationType type,
                                         const hidl_string& alpha);

  Return<void> lceData(RadioIndicationType type, const LceDataInfo& lce);

  Return<void> pcoData(RadioIndicationType type, const PcoDataInfo& pco);

  Return<void> modemReset(RadioIndicationType type, const hidl_string& reason);

  Return<void> carrierInfoForImsiEncryption(RadioIndicationType type);

  Return<void> networkScanResult(RadioIndicationType type,
                                 const RADIO_1_1::NetworkScanResult& result);

  Return<void> keepaliveStatus(RadioIndicationType type,
                               const KeepaliveStatus& status);

#if RADIO_HAL >= 14
  Return<void> currentSignalStrength_1_4(RadioIndicationType type,
                                         const SignalStrength& signalStrength);

  Return<void> dataCallListChanged_1_4(
      RadioIndicationType type, const hidl_vec<SetupDataCallResult>& dcList);

  Return<void> currentPhysicalChannelConfigs_1_4(
      RadioIndicationType type, const hidl_vec<PhysicalChannelConfig>& configs);

  Return<void> networkScanResult_1_4(RadioIndicationType type,
                                     const NetworkScanResult& result);

  Return<void> cellInfoList_1_4(RadioIndicationType type,
                                const hidl_vec<CellInfo>& records);

  Return<void> currentEmergencyNumberList(
      RadioIndicationType type,
      const hidl_vec<RADIO_1_4::EmergencyNumber>& emergencyNumberList);

  Return<void> currentSignalStrength_1_2(
      RadioIndicationType type,
      const RADIO_1_2::SignalStrength& signalStrength);

  Return<void> currentPhysicalChannelConfigs(
      RadioIndicationType type,
      const hidl_vec<RADIO_1_2::PhysicalChannelConfig>& configs);

  Return<void> currentLinkCapacityEstimate(
      RadioIndicationType type, const RADIO_1_2::LinkCapacityEstimate& lce);

  Return<void> cellInfoList_1_2(RadioIndicationType type,
                                const hidl_vec<RADIO_1_2::CellInfo>& records);

  Return<void> networkScanResult_1_2(
      RadioIndicationType type, const RADIO_1_2::NetworkScanResult& result);
#endif

 private:
  void defaultResponse(const RadioIndicationType type,
                       const nsString& rilmessageType);
  int32_t convertRadioStateToNum(RadioState state);
  int32_t convertSimRefreshType(SimRefreshType type);
  int32_t convertPhoneRestrictedState(PhoneRestrictedState state);
  int32_t convertUssdModeType(UssdModeType type);
  int32_t convertSrvccState(SrvccState state);
  int32_t convertHardwareConfigType(HardwareConfigType state);
  int32_t convertHardwareConfigState(HardwareConfigState state);

  int32_t convertRadioCapabilityPhase(RadioCapabilityPhase value);
  int32_t convertRadioAccessFamily(RadioAccessFamily value);
  int32_t convertRadioCapabilityStatus(RadioCapabilityStatus state);
};

#endif
