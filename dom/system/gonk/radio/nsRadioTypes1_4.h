/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

// Radio types when using HAL 1.4

#ifndef nsRadioTypes1_4_H
#define nsRadioTypes1_4_H

#include "android/hardware/radio/1.4/types.h"

#define RADIO_1_0 ::android::hardware::radio::V1_0
#define RADIO_1_1 ::android::hardware::radio::V1_1
#define RADIO_1_2 ::android::hardware::radio::V1_2
#define RADIO_1_4 ::android::hardware::radio::V1_4

using RADIO_1_0::ActivityStatsInfo;
using RADIO_1_0::ApnAuthType;
using RADIO_1_0::AppState;
using RADIO_1_0::AppType;
using RADIO_1_0::CallForwardInfo;
using RADIO_1_0::CallForwardInfoStatus;
using RADIO_1_0::CallPresentation;
using RADIO_1_0::CallState;
using RADIO_1_0::CardState;
using RADIO_1_0::CarrierRestrictions;
using RADIO_1_0::CdmaBroadcastSmsConfigInfo;
using RADIO_1_0::CdmaCallWaiting;
using RADIO_1_0::CdmaInformationRecords;
using RADIO_1_0::CdmaOtaProvisionStatus;
using RADIO_1_0::CdmaRoamingType;
using RADIO_1_0::CdmaSignalInfoRecord;
using RADIO_1_0::CdmaSignalStrength;
using RADIO_1_0::CdmaSmsMessage;
using RADIO_1_0::CdmaSubscriptionSource;
using RADIO_1_0::CellInfoType;
using RADIO_1_0::ClipStatus;
using RADIO_1_0::Clir;
using RADIO_1_0::DataProfileId;
using RADIO_1_0::DataProfileInfoType;
using RADIO_1_0::Dial;
using RADIO_1_0::EvdoSignalStrength;
using RADIO_1_0::GsmBroadcastSmsConfigInfo;
using RADIO_1_0::GsmSignalStrength;
using RADIO_1_0::GsmSmsMessage;
using RADIO_1_0::HardwareConfig;
using RADIO_1_0::HardwareConfigState;
using RADIO_1_0::HardwareConfigType;
using RADIO_1_0::IccIo;
using RADIO_1_0::IccIoResult;
using RADIO_1_0::LastCallFailCause;
using RADIO_1_0::LastCallFailCauseInfo;
using RADIO_1_0::LceDataInfo;
using RADIO_1_0::LceStatusInfo;
using RADIO_1_0::LteSignalStrength;
using RADIO_1_0::MvnoType;
using RADIO_1_0::NeighboringCell;
using RADIO_1_0::OperatorInfo;
using RADIO_1_0::OperatorStatus;
using RADIO_1_0::PcoDataInfo;
using RADIO_1_0::PersoSubstate;
using RADIO_1_0::PhoneRestrictedState;
using RADIO_1_0::PinState;
using RADIO_1_0::PreferredNetworkType;
using RADIO_1_0::RadioBandMode;
using RADIO_1_0::RadioCapabilityPhase;
using RADIO_1_0::RadioCapabilityStatus;
using RADIO_1_0::RadioError;
using RADIO_1_0::RadioIndicationType;
using RADIO_1_0::RadioResponseInfo;
using RADIO_1_0::RadioResponseType;
using RADIO_1_0::RadioState;
using RADIO_1_0::RadioTechnologyFamily;
using RADIO_1_0::RegState;
using RADIO_1_0::SelectUiccSub;
using RADIO_1_0::SendSmsResult;
using RADIO_1_0::SimRefreshResult;
using RADIO_1_0::SimRefreshType;
using RADIO_1_0::SmsAcknowledgeFailCause;
using RADIO_1_0::SrvccState;
using RADIO_1_0::StkCcUnsolSsResult;
using RADIO_1_0::SubscriptionType;
using RADIO_1_0::SuppSvcNotification;
using RADIO_1_0::TimeStampType;
using RADIO_1_0::TtyMode;
using RADIO_1_0::UiccSubActStatus;
using RADIO_1_0::UssdModeType;
using RADIO_1_0::UusDcs;
using RADIO_1_0::UusInfo;
using RADIO_1_0::UusType;

using RADIO_1_1::KeepaliveStatus;

using RADIO_1_2::Call;
using RADIO_1_2::CellIdentity;
using RADIO_1_2::CellIdentityCdma;
using RADIO_1_2::CellIdentityGsm;
using RADIO_1_2::CellIdentityLte;
using RADIO_1_2::CellIdentityTdscdma;
using RADIO_1_2::CellIdentityWcdma;
using RADIO_1_2::CellInfoCdma;
using RADIO_1_2::CellInfoGsm;
using RADIO_1_2::CellInfoTdscdma;
using RADIO_1_2::CellInfoWcdma;
using RADIO_1_2::DataRegStateResult;
using RADIO_1_2::DataRequestReason;
using RADIO_1_2::TdscdmaSignalStrength;
using RADIO_1_2::VoiceRegStateResult;
using RADIO_1_2::WcdmaSignalStrength;

// This type was renamed...
typedef TdscdmaSignalStrength TdScdmaSignalStrength;

using RADIO_1_4::AccessNetwork;
using RADIO_1_4::ApnTypes;
using RADIO_1_4::CardStatus;
using RADIO_1_4::CellInfo;
using RADIO_1_4::CellInfoLte;
using RADIO_1_4::DataCallFailCause;
using RADIO_1_4::DataProfileInfo;
using RADIO_1_4::NetworkScanResult;
using RADIO_1_4::PdpProtocolType;
using RADIO_1_4::PhysicalChannelConfig;
using RADIO_1_4::RadioAccessFamily;
using RADIO_1_4::RadioCapability;
using RADIO_1_4::RadioTechnology;
using RADIO_1_4::SetupDataCallResult;
using RADIO_1_4::SignalStrength;

#endif  // nsRadioTypes1_4_H
