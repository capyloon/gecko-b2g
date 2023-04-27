/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=8 sts=2 et sw=2 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef nsRilResponse_H
#define nsRilResponse_H
#include <nsISupportsImpl.h>
#include <nsTArray.h>

#include "nsRadioVersion.h"

#if RADIO_HAL >= 14
#  include <android/hardware/radio/1.4/IRadioResponse.h>
using RADIO_1_4::IRadioResponse;
#else
#  include <android/hardware/radio/1.1/IRadioResponse.h>
using RADIO_1_1::IRadioResponse;
#endif

class nsRilWorker;
class nsRilResponse : public IRadioResponse {
 public:
  explicit nsRilResponse(nsRilWorker* aRil);
  ~nsRilResponse();

  RadioResponseInfo rspInfo;

  Return<void> getIccCardStatusResponse(
      const RadioResponseInfo& info, const RADIO_1_0::CardStatus& cardStatus);

  Return<void> supplyIccPinForAppResponse(const RadioResponseInfo& info,
                                          int32_t remainingRetries);

  Return<void> supplyIccPukForAppResponse(const RadioResponseInfo& info,
                                          int32_t remainingRetries);

  Return<void> supplyIccPin2ForAppResponse(const RadioResponseInfo& info,
                                           int32_t remainingRetries);

  Return<void> supplyIccPuk2ForAppResponse(const RadioResponseInfo& info,
                                           int32_t remainingRetries);

  Return<void> changeIccPinForAppResponse(const RadioResponseInfo& info,
                                          int32_t remainingRetries);

  Return<void> changeIccPin2ForAppResponse(const RadioResponseInfo& info,
                                           int32_t remainingRetries);

  Return<void> supplyNetworkDepersonalizationResponse(
      const RadioResponseInfo& info, int32_t remainingRetries);

  Return<void> getCurrentCallsResponse(const RadioResponseInfo& info,
                                       const hidl_vec<RADIO_1_0::Call>& calls);

  Return<void> dialResponse(const RadioResponseInfo& info);

  Return<void> getIMSIForAppResponse(const RadioResponseInfo& info,
                                     const hidl_string& imsi);

  Return<void> hangupConnectionResponse(const RadioResponseInfo& info);

  Return<void> hangupWaitingOrBackgroundResponse(const RadioResponseInfo& info);

  Return<void> hangupForegroundResumeBackgroundResponse(
      const RadioResponseInfo& info);

  Return<void> switchWaitingOrHoldingAndActiveResponse(
      const RadioResponseInfo& info);

  Return<void> conferenceResponse(const RadioResponseInfo& info);

  Return<void> rejectCallResponse(const RadioResponseInfo& info);

  Return<void> getLastCallFailCauseResponse(
      const RadioResponseInfo& info,
      const LastCallFailCauseInfo& failCauseInfo);

  Return<void> getSignalStrengthResponse(
      const RadioResponseInfo& info,
      const RADIO_1_0::SignalStrength& sigStrength);

  Return<void> getVoiceRegistrationStateResponse(
      const RadioResponseInfo& info,
      const RADIO_1_0::VoiceRegStateResult& voiceRegResponse);

  Return<void> getDataRegistrationStateResponse(
      const RadioResponseInfo& info,
      const RADIO_1_0::DataRegStateResult& dataRegResponse);

  Return<void> getOperatorResponse(const RadioResponseInfo& info,
                                   const hidl_string& longName,
                                   const hidl_string& shortName,
                                   const hidl_string& numeric);

  Return<void> setRadioPowerResponse(const RadioResponseInfo& info);

  Return<void> sendDtmfResponse(const RadioResponseInfo& info);

  Return<void> sendSmsResponse(const RadioResponseInfo& info,
                               const SendSmsResult& sms);

  Return<void> sendSMSExpectMoreResponse(const RadioResponseInfo& info,
                                         const SendSmsResult& sms);

  Return<void> setupDataCallResponse(
      const RadioResponseInfo& info,
      const RADIO_1_0::SetupDataCallResult& dcResponse);

  Return<void> iccIOForAppResponse(const RadioResponseInfo& info,
                                   const IccIoResult& iccIo);

  Return<void> sendUssdResponse(const RadioResponseInfo& info);

  Return<void> cancelPendingUssdResponse(const RadioResponseInfo& info);

  Return<void> getClirResponse(const RadioResponseInfo& info, int32_t n,
                               int32_t m);

  Return<void> setClirResponse(const RadioResponseInfo& info);

  Return<void> getCallForwardStatusResponse(
      const RadioResponseInfo& info,
      const hidl_vec<CallForwardInfo>& call_forwardInfos);

  Return<void> setCallForwardResponse(const RadioResponseInfo& info);

  Return<void> getCallWaitingResponse(const RadioResponseInfo& info,
                                      bool enable, int32_t serviceClass);

  Return<void> setCallWaitingResponse(const RadioResponseInfo& info);

  Return<void> acknowledgeLastIncomingGsmSmsResponse(
      const RadioResponseInfo& info);

  Return<void> acceptCallResponse(const RadioResponseInfo& info);

  Return<void> deactivateDataCallResponse(const RadioResponseInfo& info);

  Return<void> getFacilityLockForAppResponse(const RadioResponseInfo& info,
                                             int32_t response);

  Return<void> setFacilityLockForAppResponse(const RadioResponseInfo& info,
                                             int32_t retry);

  Return<void> setBarringPasswordResponse(const RadioResponseInfo& info);

  Return<void> getNetworkSelectionModeResponse(const RadioResponseInfo& info,
                                               bool manual);

  Return<void> setNetworkSelectionModeAutomaticResponse(
      const RadioResponseInfo& info);

  Return<void> setNetworkSelectionModeManualResponse(
      const RadioResponseInfo& info);

  Return<void> getAvailableNetworksResponse(
      const RadioResponseInfo& info,
      const hidl_vec<OperatorInfo>& networkInfos);

  Return<void> startDtmfResponse(const RadioResponseInfo& info);

  Return<void> stopDtmfResponse(const RadioResponseInfo& info);

  Return<void> getBasebandVersionResponse(const RadioResponseInfo& info,
                                          const hidl_string& version);

  Return<void> separateConnectionResponse(const RadioResponseInfo& info);

  Return<void> setMuteResponse(const RadioResponseInfo& info);

  Return<void> getMuteResponse(const RadioResponseInfo& info, bool enable);

  Return<void> getClipResponse(const RadioResponseInfo& info,
                               ClipStatus status);

  Return<void> getDataCallListResponse(
      const RadioResponseInfo& info,
      const hidl_vec<RADIO_1_0::SetupDataCallResult>& dcResponse);

  Return<void> sendOemRilRequestRawResponse(const RadioResponseInfo& info,
                                            const hidl_vec<uint8_t>& data);

  Return<void> sendOemRilRequestStringsResponse(
      const RadioResponseInfo& info, const hidl_vec<hidl_string>& data);

  Return<void> setSuppServiceNotificationsResponse(
      const RadioResponseInfo& info);

  Return<void> writeSmsToSimResponse(const RadioResponseInfo& info,
                                     int32_t index);

  Return<void> deleteSmsOnSimResponse(const RadioResponseInfo& info);

  Return<void> setBandModeResponse(const RadioResponseInfo& info);

  Return<void> getAvailableBandModesResponse(
      const RadioResponseInfo& info, const hidl_vec<RadioBandMode>& bandModes);

  Return<void> sendEnvelopeResponse(const RadioResponseInfo& info,
                                    const hidl_string& commandResponse);

  Return<void> sendTerminalResponseToSimResponse(const RadioResponseInfo& info);

  Return<void> handleStkCallSetupRequestFromSimResponse(
      const RadioResponseInfo& info);

  Return<void> explicitCallTransferResponse(const RadioResponseInfo& info);

  Return<void> setPreferredNetworkTypeResponse(const RadioResponseInfo& info);

  Return<void> getPreferredNetworkTypeResponse(const RadioResponseInfo& info,
                                               PreferredNetworkType nwType);

  Return<void> getNeighboringCidsResponse(
      const RadioResponseInfo& info, const hidl_vec<NeighboringCell>& cells);

  Return<void> setLocationUpdatesResponse(const RadioResponseInfo& info);

  Return<void> setCdmaSubscriptionSourceResponse(const RadioResponseInfo& info);

  Return<void> setCdmaRoamingPreferenceResponse(const RadioResponseInfo& info);

  Return<void> getCdmaRoamingPreferenceResponse(const RadioResponseInfo& info,
                                                CdmaRoamingType type);

  Return<void> setTTYModeResponse(const RadioResponseInfo& info);

  Return<void> getTTYModeResponse(const RadioResponseInfo& info, TtyMode mode);

  Return<void> setPreferredVoicePrivacyResponse(const RadioResponseInfo& info);

  Return<void> getPreferredVoicePrivacyResponse(const RadioResponseInfo& info,
                                                bool enable);

  Return<void> sendCDMAFeatureCodeResponse(const RadioResponseInfo& info);

  Return<void> sendBurstDtmfResponse(const RadioResponseInfo& info);

  Return<void> sendCdmaSmsResponse(const RadioResponseInfo& info,
                                   const SendSmsResult& sms);

  Return<void> acknowledgeLastIncomingCdmaSmsResponse(
      const RadioResponseInfo& info);

  Return<void> getGsmBroadcastConfigResponse(
      const RadioResponseInfo& info,
      const hidl_vec<GsmBroadcastSmsConfigInfo>& configs);

  Return<void> setGsmBroadcastConfigResponse(const RadioResponseInfo& info);

  Return<void> setGsmBroadcastActivationResponse(const RadioResponseInfo& info);

  Return<void> getCdmaBroadcastConfigResponse(
      const RadioResponseInfo& info,
      const hidl_vec<CdmaBroadcastSmsConfigInfo>& configs);

  Return<void> setCdmaBroadcastConfigResponse(const RadioResponseInfo& info);

  Return<void> setCdmaBroadcastActivationResponse(
      const RadioResponseInfo& info);

  Return<void> getCDMASubscriptionResponse(const RadioResponseInfo& info,
                                           const hidl_string& mdn,
                                           const hidl_string& hSid,
                                           const hidl_string& hNid,
                                           const hidl_string& min,
                                           const hidl_string& prl);

  Return<void> writeSmsToRuimResponse(const RadioResponseInfo& info,
                                      uint32_t index);

  Return<void> deleteSmsOnRuimResponse(const RadioResponseInfo& info);

  Return<void> getDeviceIdentityResponse(const RadioResponseInfo& info,
                                         const hidl_string& imei,
                                         const hidl_string& imeisv,
                                         const hidl_string& esn,
                                         const hidl_string& meid);

  Return<void> exitEmergencyCallbackModeResponse(const RadioResponseInfo& info);

  Return<void> getSmscAddressResponse(const RadioResponseInfo& info,
                                      const hidl_string& smsc);

  Return<void> setSmscAddressResponse(const RadioResponseInfo& info);

  Return<void> reportSmsMemoryStatusResponse(const RadioResponseInfo& info);

  Return<void> reportStkServiceIsRunningResponse(const RadioResponseInfo& info);

  Return<void> getCdmaSubscriptionSourceResponse(const RadioResponseInfo& info,
                                                 CdmaSubscriptionSource source);

  Return<void> requestIsimAuthenticationResponse(const RadioResponseInfo& info,
                                                 const hidl_string& response);

  Return<void> acknowledgeIncomingGsmSmsWithPduResponse(
      const RadioResponseInfo& info);

  Return<void> sendEnvelopeWithStatusResponse(const RadioResponseInfo& info,
                                              const IccIoResult& iccIo);

  Return<void> getVoiceRadioTechnologyResponse(const RadioResponseInfo& info,
                                               RADIO_1_0::RadioTechnology rat);

  Return<void> getCellInfoListResponse(
      const RadioResponseInfo& info,
      const hidl_vec<RADIO_1_0::CellInfo>& cellInfo);

  Return<void> setCellInfoListRateResponse(const RadioResponseInfo& info);

  Return<void> setInitialAttachApnResponse(const RadioResponseInfo& info);

  Return<void> getImsRegistrationStateResponse(const RadioResponseInfo& info,
                                               bool isRegistered,
                                               RadioTechnologyFamily ratFamily);

  Return<void> sendImsSmsResponse(const RadioResponseInfo& info,
                                  const SendSmsResult& sms);

  Return<void> iccTransmitApduBasicChannelResponse(
      const RadioResponseInfo& info, const IccIoResult& result);

  Return<void> iccOpenLogicalChannelResponse(
      const RadioResponseInfo& info, int32_t channelId,
      const hidl_vec<int8_t>& selectResponse);

  Return<void> iccCloseLogicalChannelResponse(const RadioResponseInfo& info);

  Return<void> iccTransmitApduLogicalChannelResponse(
      const RadioResponseInfo& info, const IccIoResult& result);

  Return<void> nvReadItemResponse(const RadioResponseInfo& info,
                                  const hidl_string& result);

  Return<void> nvWriteItemResponse(const RadioResponseInfo& info);

  Return<void> nvWriteCdmaPrlResponse(const RadioResponseInfo& info);

  Return<void> nvResetConfigResponse(const RadioResponseInfo& info);

  Return<void> setUiccSubscriptionResponse(const RadioResponseInfo& info);

  Return<void> setDataAllowedResponse(const RadioResponseInfo& info);

  Return<void> getHardwareConfigResponse(
      const RadioResponseInfo& info, const hidl_vec<HardwareConfig>& config);

  Return<void> requestIccSimAuthenticationResponse(
      const RadioResponseInfo& info, const IccIoResult& result);

  Return<void> setDataProfileResponse(const RadioResponseInfo& info);

  Return<void> requestShutdownResponse(const RadioResponseInfo& info);

  Return<void> getRadioCapabilityResponse(const RadioResponseInfo& info,
                                          const RADIO_1_0::RadioCapability& rc);

  Return<void> setRadioCapabilityResponse(const RadioResponseInfo& info,
                                          const RADIO_1_0::RadioCapability& rc);

  Return<void> startLceServiceResponse(const RadioResponseInfo& info,
                                       const LceStatusInfo& statusInfo);

  Return<void> stopLceServiceResponse(const RadioResponseInfo& info,
                                      const LceStatusInfo& statusInfo);

  Return<void> pullLceDataResponse(const RadioResponseInfo& info,
                                   const LceDataInfo& lceInfo);

  Return<void> getModemActivityInfoResponse(
      const RadioResponseInfo& info, const ActivityStatsInfo& activityInfo);

  Return<void> setAllowedCarriersResponse(const RadioResponseInfo& info,
                                          int32_t numAllowed);

  Return<void> getAllowedCarriersResponse(const RadioResponseInfo& info,
                                          bool allAllowed,
                                          const CarrierRestrictions& carriers);

  Return<void> sendDeviceStateResponse(const RadioResponseInfo& info);

  Return<void> setIndicationFilterResponse(const RadioResponseInfo& info);

  Return<void> setSimCardPowerResponse(const RadioResponseInfo& info);

  Return<void> acknowledgeRequest(int32_t serial);

  Return<void> setCarrierInfoForImsiEncryptionResponse(
      const RadioResponseInfo& info);

  Return<void> setSimCardPowerResponse_1_1(const RadioResponseInfo& info);

  Return<void> startNetworkScanResponse(const RadioResponseInfo& info);

  Return<void> stopNetworkScanResponse(const RadioResponseInfo& info);

  Return<void> startKeepaliveResponse(const RadioResponseInfo& info,
                                      const KeepaliveStatus& status);

  Return<void> stopKeepaliveResponse(const RadioResponseInfo& info);

#if RADIO_HAL >= 14
  Return<void> getSignalStrengthResponse_1_4(
      const RadioResponseInfo& info,
      const RADIO_1_4::SignalStrength& signalStrength);
  Return<void> getAllowedCarriersResponse_1_4(
      const RadioResponseInfo& info,
      const RADIO_1_4::CarrierRestrictionsWithPriority& carriers,
      RADIO_1_4::SimLockMultiSimPolicy multiSimPolicy);
  Return<void> setAllowedCarriersResponse_1_4(const RadioResponseInfo& info);
  Return<void> setupDataCallResponse_1_4(
      const RadioResponseInfo& info,
      const RADIO_1_4::SetupDataCallResult& dcResponse);
  Return<void> getDataCallListResponse_1_4(
      const RadioResponseInfo& info,
      const hidl_vec<RADIO_1_4::SetupDataCallResult>& dcResponse);
  Return<void> setPreferredNetworkTypeBitmapResponse(
      const RadioResponseInfo& info);
  Return<void> getPreferredNetworkTypeBitmapResponse(
      const RadioResponseInfo& info,
      ::android::hardware::hidl_bitfield<RADIO_1_4::RadioAccessFamily>
          networkTypeBitmap);
  Return<void> getIccCardStatusResponse_1_4(
      const RadioResponseInfo& info, const RADIO_1_4::CardStatus& cardStatus);
  Return<void> getDataRegistrationStateResponse_1_4(
      const RadioResponseInfo& info,
      const RADIO_1_4::DataRegStateResult& dataRegResponse);
  Return<void> getCellInfoListResponse_1_4(
      const RadioResponseInfo& info,
      const hidl_vec<RADIO_1_4::CellInfo>& cellInfo);
  Return<void> startNetworkScanResponse_1_4(const RadioResponseInfo& info);
  Return<void> emergencyDialResponse(const RadioResponseInfo& info);
  Return<void> getModemStackStatusResponse(const RadioResponseInfo& info,
                                           bool isEnabled);
  Return<void> enableModemResponse(const RadioResponseInfo& info);
  Return<void> setSystemSelectionChannelsResponse(
      const RadioResponseInfo& info);

  Return<void> getDataRegistrationStateResponse_1_2(
      const RadioResponseInfo& info,
      const RADIO_1_2::DataRegStateResult& dataRegResponse);
  Return<void> getVoiceRegistrationStateResponse_1_2(
      const RadioResponseInfo& info,
      const RADIO_1_2::VoiceRegStateResult& voiceRegResponse);
  Return<void> getSignalStrengthResponse_1_2(
      const RadioResponseInfo& info,
      const RADIO_1_2::SignalStrength& signalStrength);
  Return<void> getCurrentCallsResponse_1_2(
      const RadioResponseInfo& info, const hidl_vec<RADIO_1_2::Call>& calls);
  Return<void> setLinkCapacityReportingCriteriaResponse(
      const RadioResponseInfo& info);
  Return<void> setSignalStrengthReportingCriteriaResponse(
      const RadioResponseInfo& info);
  Return<void> getIccCardStatusResponse_1_2(
      const RadioResponseInfo& info, const RADIO_1_2::CardStatus& cardStatus);
  Return<void> getCellInfoListResponse_1_2(
      const RadioResponseInfo& info,
      const hidl_vec<RADIO_1_2::CellInfo>& cellInfo);
#endif

 private:
  void defaultResponse(const RadioResponseInfo& rspInfo,
                       const nsString& rilmessageType);
  int32_t convertRadioErrorToNum(RadioError error);
  int32_t covertLastCallFailCause(LastCallFailCause cause);
  int32_t convertAppType(AppType type);
  int32_t convertAppState(AppState state);
  int32_t convertPersoSubstate(PersoSubstate state);
  int32_t convertPinState(PinState state);
  int32_t convertCardState(CardState state);
  int32_t convertRegState(RegState state);
  int32_t convertUusType(UusType type);
  int32_t convertUusDcs(UusDcs dcs);
  int32_t convertCallPresentation(CallPresentation state);
  int32_t convertCallState(CallState state);
  int32_t convertPreferredNetworkType(PreferredNetworkType type);
  int32_t convertOperatorState(OperatorStatus status);
  int32_t convertCallForwardState(CallForwardInfoStatus status);
  int32_t convertClipState(ClipStatus status);
  int32_t convertTtyMode(TtyMode mode);
  RefPtr<nsRilWorker> mRIL;
};

#endif
