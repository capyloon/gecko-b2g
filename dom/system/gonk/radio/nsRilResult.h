/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=8 sts=2 et sw=2 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef nsRilResult_H
#define nsRilResult_H

#include "nsISupports.h"
#include <nsISupportsImpl.h>
#include "nsTArray.h"
#include "nsCOMArray.h"
#include "nsIRilResult.h"
#include "nsString.h"

#include "nsRadioVersion.h"

#if RADIO_HAL >= 14
#  include <android/hardware/radio/1.4/IRadio.h>
#  include <android/hardware/radio/1.4/IRadioResponse.h>
#else
#  include <android/hardware/radio/1.1/IRadio.h>
#  include <android/hardware/radio/1.1/IRadioResponse.h>
#endif

class nsRilResult;

class nsGsmSignalStrength final : public nsIGsmSignalStrength {
 public:
  NS_DECL_THREADSAFE_ISUPPORTS
  NS_DECL_NSIGSMSIGNALSTRENGTH
  nsGsmSignalStrength(int32_t aSignalStrength, int32_t aBitErrorRate,
                      int32_t aTimingAdvance);

 private:
  ~nsGsmSignalStrength(){};
  int32_t mSignalStrength;
  int32_t mBitErrorRate;
  int32_t mTimingAdvance;
};

class nsWcdmaSignalStrength final : public nsIWcdmaSignalStrength {
 public:
  NS_DECL_THREADSAFE_ISUPPORTS
  NS_DECL_NSIWCDMASIGNALSTRENGTH
  nsWcdmaSignalStrength(int32_t aSignalStrength, int32_t aBitErrorRate);

 private:
  ~nsWcdmaSignalStrength(){};
  int32_t mSignalStrength;
  int32_t mBitErrorRate;
};

class nsCdmaSignalStrength final : public nsICdmaSignalStrength {
 public:
  NS_DECL_THREADSAFE_ISUPPORTS
  NS_DECL_NSICDMASIGNALSTRENGTH
  nsCdmaSignalStrength(int32_t aDbm, int32_t aEcio);

 private:
  ~nsCdmaSignalStrength(){};
  int32_t mDbm;
  int32_t mEcio;
};

class nsEvdoSignalStrength final : public nsIEvdoSignalStrength {
 public:
  NS_DECL_THREADSAFE_ISUPPORTS
  NS_DECL_NSIEVDOSIGNALSTRENGTH
  nsEvdoSignalStrength(int32_t aDbm, int32_t aEcio, int32_t aSignalNoiseRatio);

 private:
  ~nsEvdoSignalStrength(){};
  int32_t mDbm;
  int32_t mEcio;
  int32_t mSignalNoiseRatio;
};

class nsLteSignalStrength final : public nsILteSignalStrength {
 public:
  NS_DECL_THREADSAFE_ISUPPORTS
  NS_DECL_NSILTESIGNALSTRENGTH
  nsLteSignalStrength(int32_t aSignalStrength, int32_t aRsrp, int32_t aRsrq,
                      int32_t aRssnr, int32_t aCqi, int32_t aTimingAdvance);

 private:
  ~nsLteSignalStrength(){};
  int32_t mSignalStrength;
  int32_t mRsrp;
  int32_t mRsrq;
  int32_t mRssnr;
  int32_t mCqi;
  int32_t mTimingAdvance;
};

class nsTdScdmaSignalStrength final : public nsITdScdmaSignalStrength {
 public:
  NS_DECL_THREADSAFE_ISUPPORTS
  NS_DECL_NSITDSCDMASIGNALSTRENGTH
  explicit nsTdScdmaSignalStrength(int32_t aRscp);

 private:
  ~nsTdScdmaSignalStrength(){};
  int32_t mRscp;
};

class nsSignalStrength final : public nsISignalStrength {
 public:
  NS_DECL_THREADSAFE_ISUPPORTS
  NS_DECL_NSISIGNALSTRENGTH
  nsSignalStrength(nsGsmSignalStrength* aGsmSignalStrength,
                   nsCdmaSignalStrength* aCdmaSignalStrength,
                   nsEvdoSignalStrength* aEvdoSignalStrength,
                   nsLteSignalStrength* aLteSignalStrength,
                   nsTdScdmaSignalStrength* aTdScdmaSignalStrength);

 private:
  ~nsSignalStrength(){};
  RefPtr<nsGsmSignalStrength> mGsmSignalStrength;
  RefPtr<nsCdmaSignalStrength> mCdmaSignalStrength;
  RefPtr<nsEvdoSignalStrength> mEvdoSignalStrength;
  RefPtr<nsLteSignalStrength> mLteSignalStrength;
  RefPtr<nsTdScdmaSignalStrength> mTdScdmaSignalStrength;
};

class nsSetupDataCallResult final : public nsISetupDataCallResult {
 public:
  NS_DECL_THREADSAFE_ISUPPORTS
  NS_DECL_NSISETUPDATACALLRESULT
  nsSetupDataCallResult(int32_t aFailCause, int32_t aSuggestedRetryTime,
                        int32_t aCid, int32_t aActive,
                        const nsAString& aPdpType, const nsAString& aIfname,
                        const nsAString& aAddresses, const nsAString& aDnses,
                        const nsAString& aGateways, const nsAString& aPcscf,
                        int32_t aMtu);

 private:
  ~nsSetupDataCallResult(){};
  int32_t mFailCause;
  int32_t mSuggestedRetryTime;
  int32_t mCid;
  int32_t mActive;
  nsString mPdpType;
  nsString mIfname;
  nsString mAddresses;
  nsString mDnses;
  nsString mGateways;
  nsString mPcscf;
  int32_t mMtu;
};

class nsSuppSvcNotification final : public nsISuppSvcNotification {
 public:
  NS_DECL_THREADSAFE_ISUPPORTS
  NS_DECL_NSISUPPSVCNOTIFICATION
  nsSuppSvcNotification(bool aNotificationType, int32_t aCode, int32_t aIndex,
                        int32_t aType, const nsAString& aNumber);

 private:
  ~nsSuppSvcNotification(){};
  bool mNotificationType;
  int32_t mCode;
  int32_t mIndex;
  int32_t mType;
  nsString mNumber;
};

class nsSimRefreshResult final : public nsISimRefreshResult {
 public:
  NS_DECL_THREADSAFE_ISUPPORTS
  NS_DECL_NSISIMREFRESHRESULT
  nsSimRefreshResult(int32_t aType, int32_t aEfId, const nsAString& aAid);

 private:
  ~nsSimRefreshResult(){};
  int32_t mType;
  int32_t mEfId;
  nsString mAid;
};

class nsCellIdentityGsm final : public nsICellIdentityGsm {
 public:
  NS_DECL_THREADSAFE_ISUPPORTS
  NS_DECL_NSICELLIDENTITYGSM
  nsCellIdentityGsm(const nsAString& aMcc, const nsAString& aMnc, int32_t aLac,
                    int32_t aCid, int32_t aArfcn, int32_t aBsic);

 private:
  ~nsCellIdentityGsm(){};
  nsString mMcc;
  nsString mMnc;
  int32_t mLac;
  int32_t mCid;
  int32_t mArfcn;
  int32_t mBsic;
};

class nsCellIdentityCdma final : public nsICellIdentityCdma {
 public:
  NS_DECL_THREADSAFE_ISUPPORTS
  NS_DECL_NSICELLIDENTITYCDMA
  nsCellIdentityCdma(int32_t aNetworkId, int32_t aSystemId,
                     int32_t aBaseStationId, int32_t aLongitude,
                     int32_t aLatitude);

 private:
  ~nsCellIdentityCdma(){};
  int32_t mNetworkId;
  int32_t mSystemId;
  int32_t mBaseStationId;
  int32_t mLongitude;
  int32_t mLatitude;
};

class nsCellIdentityLte final : public nsICellIdentityLte {
 public:
  NS_DECL_THREADSAFE_ISUPPORTS
  NS_DECL_NSICELLIDENTITYLTE
  nsCellIdentityLte(const nsAString& aMcc, const nsAString& aMnc, int32_t aCi,
                    int32_t aPci, int32_t aTac, int32_t aEarfcn);

 private:
  ~nsCellIdentityLte(){};
  nsString mMcc;
  nsString mMnc;
  int32_t mCi;
  int32_t mPci;
  int32_t mTac;
  int32_t mEarfcn;
};

class nsCellIdentityWcdma final : public nsICellIdentityWcdma {
 public:
  NS_DECL_THREADSAFE_ISUPPORTS
  NS_DECL_NSICELLIDENTITYWCDMA
  nsCellIdentityWcdma(const nsAString& aMcc, const nsAString& aMnc,
                      int32_t aLac, int32_t aCid, int32_t aPsc,
                      int32_t aUarfcn);

 private:
  ~nsCellIdentityWcdma(){};
  nsString mMcc;
  nsString mMnc;
  int32_t mLac;
  int32_t mCid;
  int32_t mPsc;
  int32_t mUarfcn;
};

class nsCellIdentityTdScdma final : public nsICellIdentityTdScdma {
 public:
  NS_DECL_THREADSAFE_ISUPPORTS
  NS_DECL_NSICELLIDENTITYTDSCDMA
  nsCellIdentityTdScdma(const nsAString& aMcc, const nsAString& aMnc,
                        int32_t aLac, int32_t aCid, int32_t aCpid);

 private:
  ~nsCellIdentityTdScdma(){};
  nsString mMcc;
  nsString mMnc;
  int32_t mLac;
  int32_t mCid;
  int32_t mCpid;
};

class nsCellIdentity final : public nsICellIdentity {
 public:
  NS_DECL_THREADSAFE_ISUPPORTS
  NS_DECL_NSICELLIDENTITY
  nsCellIdentity(int32_t aCellInfoType, nsCellIdentityGsm* aCellIdentityGsm,
                 nsCellIdentityWcdma* aCellIdentityWcdma,
                 nsCellIdentityCdma* aCellIdentityCdma,
                 nsCellIdentityLte* aCellIdentityLte,
                 nsCellIdentityTdScdma* aCellIdentityTdScdma);
  nsCellIdentity(int32_t aCellInfoType, nsCellIdentityGsm* aCellIdentityGsm);
  nsCellIdentity(int32_t aCellInfoType,
                 nsCellIdentityWcdma* aCellIdentityWcdma);
  nsCellIdentity(int32_t aCellInfoType, nsCellIdentityCdma* aCellIdentityCdma);
  nsCellIdentity(int32_t aCellInfoType, nsCellIdentityLte* aCellIdentityLte);
  nsCellIdentity(int32_t aCellInfoType,
                 nsCellIdentityTdScdma* aCellIdentityTdScdma);

 private:
  ~nsCellIdentity(){};
  int32_t mCellInfoType;
  // nsTArray<RefPtr<nsCellIdentityGsm>> mGsm;
  RefPtr<nsCellIdentityGsm> mCellIdentityGsm;
  // nsTArray<RefPtr<nsCellIdentityWcdma>> mWcdma;
  RefPtr<nsCellIdentityWcdma> mCellIdentityWcdma;
  // nsTArray<RefPtr<nsCellIdentityCdma>> mCdma;
  RefPtr<nsCellIdentityCdma> mCellIdentityCdma;
  // nsTArray<RefPtr<nsCellIdentityLte>> mLte;
  RefPtr<nsCellIdentityLte> mCellIdentityLte;
  // nsTArray<RefPtr<nsCellIdentityTdScdma>> mTdScdma;
  RefPtr<nsCellIdentityTdScdma> mCellIdentityTdScdma;
};

class nsCellInfoGsm final : public nsICellInfoGsm {
 public:
  NS_DECL_THREADSAFE_ISUPPORTS
  NS_DECL_NSICELLINFOGSM
  nsCellInfoGsm(nsCellIdentityGsm* aCellIdentityGsm,
                nsGsmSignalStrength* aSignalStrengthGsm);

 private:
  ~nsCellInfoGsm(){};
  RefPtr<nsCellIdentityGsm> mCellIdentityGsm;
  RefPtr<nsGsmSignalStrength> mSignalStrengthGsm;
};

class nsCellInfoCdma final : public nsICellInfoCdma {
 public:
  NS_DECL_THREADSAFE_ISUPPORTS
  NS_DECL_NSICELLINFOCDMA
  nsCellInfoCdma(nsCellIdentityCdma* aCellIdentityCdma,
                 nsCdmaSignalStrength* aSignalStrengthCdma,
                 nsEvdoSignalStrength* aSignalStrengthEvdo);

 private:
  ~nsCellInfoCdma(){};
  RefPtr<nsCellIdentityCdma> mCellIdentityCdma;
  RefPtr<nsCdmaSignalStrength> mSignalStrengthCdma;
  RefPtr<nsEvdoSignalStrength> mSignalStrengthEvdo;
};

class nsCellInfoLte final : public nsICellInfoLte {
 public:
  NS_DECL_THREADSAFE_ISUPPORTS
  NS_DECL_NSICELLINFOLTE
  nsCellInfoLte(nsCellIdentityLte* aCellIdentityLte,
                nsLteSignalStrength* aSignalStrengthLte);

 private:
  ~nsCellInfoLte(){};
  RefPtr<nsCellIdentityLte> mCellIdentityLte;
  RefPtr<nsLteSignalStrength> mSignalStrengthLte;
};

class nsCellInfoWcdma final : public nsICellInfoWcdma {
 public:
  NS_DECL_THREADSAFE_ISUPPORTS
  NS_DECL_NSICELLINFOWCDMA
  nsCellInfoWcdma(nsCellIdentityWcdma* aCellIdentityWcdma,
                  nsWcdmaSignalStrength* aSignalStrengthWcdma);

 private:
  ~nsCellInfoWcdma(){};
  RefPtr<nsCellIdentityWcdma> mCellIdentityWcdma;
  RefPtr<nsWcdmaSignalStrength> mSignalStrengthWcdma;
};

class nsCellInfoTdScdma final : public nsICellInfoTdScdma {
 public:
  NS_DECL_THREADSAFE_ISUPPORTS
  NS_DECL_NSICELLINFOTDSCDMA
  nsCellInfoTdScdma(nsCellIdentityTdScdma* aCellIdentityTdScdma,
                    nsTdScdmaSignalStrength* aSignalStrengthTdScdma);

 private:
  ~nsCellInfoTdScdma(){};
  RefPtr<nsCellIdentityTdScdma> mCellIdentityTdScdma;
  RefPtr<nsTdScdmaSignalStrength> mSignalStrengthTdScdma;
};

class nsRilCellInfo final : public nsIRilCellInfo {
 public:
  NS_DECL_THREADSAFE_ISUPPORTS
  NS_DECL_NSIRILCELLINFO
  nsRilCellInfo(int32_t aCellInfoType, bool aRegistered, int32_t aTimeStampType,
                int32_t aTimeStamp, nsCellInfoGsm* aGsm, nsCellInfoCdma* aCdma,
                nsCellInfoLte* aLte, nsCellInfoWcdma* aWcdma,
                nsCellInfoTdScdma* aTdScdma);
  nsRilCellInfo(int32_t aCellInfoType, bool aRegistered, int32_t aTimeStampType,
                int32_t aTimeStamp, nsCellInfoGsm* aGsm);
  nsRilCellInfo(int32_t aCellInfoType, bool aRegistered, int32_t aTimeStampType,
                int32_t aTimeStamp, nsCellInfoWcdma* aWcdma);
  nsRilCellInfo(int32_t aCellInfoType, bool aRegistered, int32_t aTimeStampType,
                int32_t aTimeStamp, nsCellInfoCdma* aCdma);
  nsRilCellInfo(int32_t aCellInfoType, bool aRegistered, int32_t aTimeStampType,
                int32_t aTimeStamp, nsCellInfoLte* aLte);
  nsRilCellInfo(int32_t aCellInfoType, bool aRegistered, int32_t aTimeStampType,
                int32_t aTimeStamp, nsCellInfoTdScdma* aTdScdma);

 private:
  ~nsRilCellInfo(){};
  int32_t mCellInfoType;
  bool mRegistered;
  int32_t mTimeStampType;
  int32_t mTimeStamp;
  RefPtr<nsCellInfoGsm> mGsm;
  RefPtr<nsCellInfoCdma> mCdma;
  RefPtr<nsCellInfoLte> mLte;
  RefPtr<nsCellInfoWcdma> mWcdma;
  RefPtr<nsCellInfoTdScdma> mTdScdma;
};

class nsHardwareConfig final : public nsIHardwareConfig {
 public:
  NS_DECL_THREADSAFE_ISUPPORTS
  NS_DECL_NSIHARDWARECONFIG
  nsHardwareConfig(int32_t aType, const nsAString& aUuid, int32_t aState,
                   nsIHardwareConfigModem* aModem, nsIHardwareConfigSim* aSim);

 private:
  ~nsHardwareConfig(){};
  int32_t mType;
  nsString mUuid;
  int32_t mState;
  RefPtr<nsIHardwareConfigModem> mModem;
  RefPtr<nsIHardwareConfigSim> mSim;
};

class nsHardwareConfigModem final : public nsIHardwareConfigModem {
 public:
  NS_DECL_THREADSAFE_ISUPPORTS
  NS_DECL_NSIHARDWARECONFIGMODEM
  nsHardwareConfigModem(int32_t aRilModel, int32_t aRat, int32_t aMaxVoice,
                        int32_t aMaxData, int32_t aMaxStandby);

 private:
  ~nsHardwareConfigModem(){};
  int32_t mRilModel;
  int32_t mRat;
  int32_t mMaxVoice;
  int32_t mMaxData;
  int32_t mMaxStandby;
};

class nsHardwareConfigSim final : public nsIHardwareConfigSim {
 public:
  NS_DECL_THREADSAFE_ISUPPORTS
  NS_DECL_NSIHARDWARECONFIGSIM
  explicit nsHardwareConfigSim(const nsAString& aModemUuid);

 private:
  ~nsHardwareConfigSim(){};
  nsString mModemUuid;
};

class nsRadioCapability final : public nsIRadioCapability {
 public:
  NS_DECL_THREADSAFE_ISUPPORTS
  NS_DECL_NSIRADIOCAPABILITY
  nsRadioCapability(int32_t aSession, int32_t aPhase, int32_t aRaf,
                    const nsAString& aLogicalModemUuid, int32_t aStatus);

 private:
  ~nsRadioCapability(){};
  int32_t mSession;
  int32_t mPhase;
  int32_t mRaf;
  nsString mLogicalModemUuid;
  int32_t mStatus;
};

class nsLceStatusInfo final : public nsILceStatusInfo {
 public:
  NS_DECL_THREADSAFE_ISUPPORTS
  NS_DECL_NSILCESTATUSINFO
  nsLceStatusInfo(int32_t aLceStatus, int32_t aActualIntervalMs);

 private:
  ~nsLceStatusInfo(){};
  int32_t mLceStatus;
  int32_t mActualIntervalMs;
};

class nsLceDataInfo final : public nsILceDataInfo {
 public:
  NS_DECL_THREADSAFE_ISUPPORTS
  NS_DECL_NSILCEDATAINFO
  nsLceDataInfo(int32_t aLastHopCapacityKbps, int32_t aConfidenceLevel,
                bool aLceSuspended);

 private:
  ~nsLceDataInfo(){};
  int32_t mLastHopCapacityKbps;
  int32_t mConfidenceLevel;
  bool mLceSuspended;
};

class nsPcoDataInfo final : public nsIPcoDataInfo {
 public:
  NS_DECL_THREADSAFE_ISUPPORTS
  NS_DECL_NSIPCODATAINFO
  nsPcoDataInfo(int32_t aCid, const nsAString& aBearerProto, bool aPcoId,
                nsTArray<int32_t>& aContents);

 private:
  ~nsPcoDataInfo(){};
  int32_t mCid;
  nsString mBearerProto;
  bool mPcoId;
  nsTArray<int32_t> mContents;
};

class nsAppStatus final : public nsIAppStatus {
 public:
  NS_DECL_THREADSAFE_ISUPPORTS
  NS_DECL_NSIAPPSTATUS
  nsAppStatus(int32_t aAppType, int32_t aAppState, int32_t aPersoSubstate,
              const nsAString& aAidPtr, const nsAString& aAppLabelPtr,
              int32_t aPin1Replaced, int32_t aPin1, int32_t aPin2);

 private:
  ~nsAppStatus(){};
  int32_t mAppType;
  int32_t mAppState;
  int32_t mPersoSubstate;
  nsString mAidPtr;
  nsString mAppLabelPtr;
  int32_t mPin1Replaced;
  int32_t mPin1;
  int32_t mPin2;
};

class nsCardStatus final : public nsICardStatus {
 public:
  NS_DECL_THREADSAFE_ISUPPORTS
  NS_DECL_NSICARDSTATUS
  nsCardStatus(int32_t aCardState, int32_t aUniversalPinState,
               int32_t aGsmUmtsSubscriptionAppIndex,
               int32_t aCdmaSubscriptionAppIndex,
               int32_t aImsSubscriptionAppIndex,
               nsTArray<RefPtr<nsAppStatus>>& aApplications);

 private:
  ~nsCardStatus(){};
  int32_t mCardState;
  int32_t mUniversalPinState;
  int32_t mGsmUmtsSubscriptionAppIndex = -1;
  int32_t mCdmaSubscriptionAppIndex = -1;
  int32_t mImsSubscriptionAppIndex = -1;
  nsTArray<RefPtr<nsAppStatus>> mApplications;
};

class nsVoiceRegState final : public nsIVoiceRegState {
 public:
  NS_DECL_THREADSAFE_ISUPPORTS
  NS_DECL_NSIVOICEREGSTATE
  nsVoiceRegState(int32_t aRegState, int32_t aRat, bool aCssSupported,
                  int32_t aRoamingIndicator, int32_t aSystemIsInPrl,
                  int32_t aDefaultRoamingIndicator, int32_t aReasonForDenial,
                  nsCellIdentity* aCellIdentity);

 private:
  ~nsVoiceRegState(){};
  int32_t mRegState;
  int32_t mRat;
  bool mCssSupported;
  int32_t mRoamingIndicator;
  int32_t mSystemIsInPrl;
  int32_t mDefaultRoamingIndicator;
  int32_t mReasonForDenial;
  RefPtr<nsCellIdentity> mCellIdentity;
};

class nsDataRegState final : public nsIDataRegState {
 public:
  NS_DECL_THREADSAFE_ISUPPORTS
  NS_DECL_NSIDATAREGSTATE
  nsDataRegState(int32_t aRegState, int32_t aRat, int32_t aReasonDataDenied,
                 int32_t aMaxDataCalls, nsCellIdentity* aCellIdentity);

 private:
  ~nsDataRegState(){};
  int32_t mRegState;
  int32_t mRat;
  int32_t mReasonDataDenied;
  int32_t mMaxDataCalls;
  RefPtr<nsCellIdentity> mCellIdentity;
};

class nsSendSmsResult final : public nsISendSmsResult {
 public:
  NS_DECL_THREADSAFE_ISUPPORTS
  NS_DECL_NSISENDSMSRESULT
  nsSendSmsResult(int32_t aMessageRef, const nsAString& aAckPDU,
                  int32_t aErrorCode);

 private:
  ~nsSendSmsResult(){};
  int32_t mMessageRef;
  nsString mAckPDU;
  int32_t mErrorCode;
};

class nsOperatorInfo final : public nsIOperatorInfo {
 public:
  NS_DECL_THREADSAFE_ISUPPORTS
  NS_DECL_NSIOPERATORINFO
  nsOperatorInfo(const nsAString& aAlphaLong, const nsAString& aAlphaShort,
                 const nsAString& aOperatorNumeric, int32_t aStatus);

 private:
  ~nsOperatorInfo(){};
  nsString mAlphaLong;
  nsString mAlphaShort;
  nsString mOperatorNumeric;
  int32_t mStatus;
};

class nsNeighboringCell final : public nsINeighboringCell {
 public:
  NS_DECL_THREADSAFE_ISUPPORTS
  NS_DECL_NSINEIGHBORINGCELL
  nsNeighboringCell(const nsAString& aCid, int32_t aRssi);

 private:
  ~nsNeighboringCell(){};
  nsString mCid;
  int32_t mRssi;
};

class nsUusInfo final : public nsIUusInfo {
 public:
  NS_DECL_THREADSAFE_ISUPPORTS
  NS_DECL_NSIUUSINFO
  nsUusInfo(int32_t aUusType, int32_t aUusDcs, const nsAString& aUusData);

 private:
  ~nsUusInfo(){};
  int32_t mUusType;
  int32_t mUusDcs;
  nsString mUusData;
};

class nsCall final : public nsICall {
 public:
  NS_DECL_THREADSAFE_ISUPPORTS
  NS_DECL_NSICALL
  nsCall(int32_t aState, int32_t aIndex, int32_t aToa, bool aIsMpty, bool aIsMT,
         int32_t aAls, bool aIsVoice, bool aIsVoicePrivacy,
         const nsAString& aNumber, int32_t aNumberPresentation,
         const nsAString& aName, int32_t aNamePresentation,
         nsTArray<RefPtr<nsUusInfo>>& aUusInfo);

 private:
  ~nsCall(){};
  int32_t mState;
  int32_t mIndex;
  int32_t mToa;
  bool mIsMpty;
  bool mIsMT;
  int32_t mAls;
  bool mIsVoice;
  bool mIsVoicePrivacy;
  nsString mNumber;
  int32_t mNumberPresentation;
  nsString mName;
  int32_t mNamePresentation;
  nsTArray<RefPtr<nsUusInfo>> mUusInfo;
};

class nsIccIoResult final : public nsIIccIoResult {
 public:
  NS_DECL_THREADSAFE_ISUPPORTS
  NS_DECL_NSIICCIORESULT
  nsIccIoResult(int32_t aSw1, int32_t aSw2, const nsAString& aSimResponse);

 private:
  ~nsIccIoResult(){};
  int32_t mSw1;
  int32_t mSw2;
  nsString mSimResponse;
};

class nsCallForwardInfo final : public nsICallForwardInfo {
 public:
  NS_DECL_THREADSAFE_ISUPPORTS
  NS_DECL_NSICALLFORWARDINFO
  nsCallForwardInfo(int32_t aStatus, int32_t aReason, int32_t aServiceClass,
                    int32_t aToa, const nsAString& aNumber,
                    int32_t aTimeSeconds);

 private:
  ~nsCallForwardInfo(){};
  int32_t mStatus;
  int32_t mReason;
  int32_t mServiceClass;
  int32_t mToa;
  nsString mNumber;
  int32_t mTimeSeconds;
};

class nsRilResult {
 public:
  explicit nsRilResult(const nsAString& aRilMessageType);
  nsRilResult(const nsAString& aRilMessageType, int32_t aRilMessageToken,
              int32_t aErrorMsg);
  RefPtr<nsRilCellInfo> convertRilCellInfo(
      const RADIO_1_0::CellInfo& aCellInfo);

  RefPtr<nsCellInfoGsm> convertCellInfoGsm(
      const RADIO_1_0::CellInfoGsm& aCellInfoGsm);
  RefPtr<nsCellInfoCdma> convertCellInfoCdma(
      const RADIO_1_0::CellInfoCdma& aCellInfoCdma);
  RefPtr<nsCellInfoWcdma> convertCellInfoWcdma(
      const RADIO_1_0::CellInfoWcdma& aCellInfoWcdma);
  RefPtr<nsCellInfoLte> convertCellInfoLte(
      const RADIO_1_0::CellInfoLte& aCellInfoLte);
  RefPtr<nsCellInfoTdScdma> convertCellInfoTdScdma(
      const RADIO_1_0::CellInfoTdscdma& aCellInfoTdscdma);

  RefPtr<nsCellIdentity> convertCellIdentity(
      const RADIO_1_0::CellIdentity& aCellIdentity);
  RefPtr<nsCellIdentityGsm> convertCellIdentityGsm(
      const RADIO_1_0::CellIdentityGsm& aCellIdentityGsm);
  RefPtr<nsCellIdentityWcdma> convertCellIdentityWcdma(
      const RADIO_1_0::CellIdentityWcdma& aCellIdentityWcdma);
  RefPtr<nsCellIdentityCdma> convertCellIdentityCdma(
      const RADIO_1_0::CellIdentityCdma& aCellIdentityCdma);
  RefPtr<nsCellIdentityLte> convertCellIdentityLte(
      const RADIO_1_0::CellIdentityLte& aCellIdentityLte);
  RefPtr<nsCellIdentityTdScdma> convertCellIdentityTdScdma(
      const RADIO_1_0::CellIdentityTdscdma& aCellIdentityTdScdma);

  RefPtr<nsSignalStrength> convertSignalStrength(
      const RADIO_1_0::SignalStrength& aSignalStrength);
  RefPtr<nsGsmSignalStrength> convertGsmSignalStrength(
      const GsmSignalStrength& aGsmSignalStrength);
  RefPtr<nsWcdmaSignalStrength> convertWcdmaSignalStrength(
      const RADIO_1_0::WcdmaSignalStrength& aWcdmaSignalStrength);
  RefPtr<nsCdmaSignalStrength> convertCdmaSignalStrength(
      const CdmaSignalStrength& aCdmaSignalStrength);
  RefPtr<nsEvdoSignalStrength> convertEvdoSignalStrength(
      const EvdoSignalStrength& aEvdoSignalStrength);
  RefPtr<nsLteSignalStrength> convertLteSignalStrength(
      const LteSignalStrength& aLteSignalStrength);
  RefPtr<nsTdScdmaSignalStrength> convertTdScdmaSignalStrength(
      const RADIO_1_0::TdScdmaSignalStrength& aTdScdmaSignalStrength);
  RefPtr<nsSetupDataCallResult> convertDcResponse(
      const RADIO_1_0::SetupDataCallResult& aDcResponse);
#if RADIO_HAL >= 14
  RefPtr<nsRilCellInfo> convertRilCellInfo(
      const RADIO_1_4::CellInfo& aCellInfo);
  RefPtr<nsCellIdentity> convertCellIdentity(
      const RADIO_1_2::CellIdentity& aCellIdentity);
  RefPtr<nsCellIdentityGsm> convertCellIdentityGsm(
      const RADIO_1_2::CellIdentityGsm& aCellIdentityGsm);
  RefPtr<nsCellIdentityWcdma> convertCellIdentityWcdma(
      const RADIO_1_2::CellIdentityWcdma& aCellIdentityWcdma);
  RefPtr<nsCellIdentityCdma> convertCellIdentityCdma(
      const RADIO_1_2::CellIdentityCdma& aCellIdentityCdma);
  RefPtr<nsCellIdentityLte> convertCellIdentityLte(
      const RADIO_1_2::CellIdentityLte& aCellIdentityLte);
  RefPtr<nsCellIdentityTdScdma> convertCellIdentityTdScdma(
      const RADIO_1_2::CellIdentityTdscdma& aCellIdentityTdScdma);

  RefPtr<nsCellInfoGsm> convertCellInfoGsm(
      const RADIO_1_2::CellInfoGsm& aCellInfoGsm);
  RefPtr<nsCellInfoCdma> convertCellInfoCdma(
      const RADIO_1_2::CellInfoCdma& aCellInfoCdma);
  RefPtr<nsCellInfoWcdma> convertCellInfoWcdma(
      const RADIO_1_2::CellInfoWcdma& aCellInfoWcdma);
  RefPtr<nsCellInfoLte> convertCellInfoLte(
      const RADIO_1_4::CellInfoLte& aCellInfoLte);
  RefPtr<nsCellInfoTdScdma> convertCellInfoTdScdma(
      const RADIO_1_2::CellInfoTdscdma& aCellInfoTdscdma);

  RefPtr<nsSignalStrength> convertSignalStrength(
      const RADIO_1_4::SignalStrength& aSignalStrength);
  RefPtr<nsTdScdmaSignalStrength> convertTdScdmaSignalStrength(
      const RADIO_1_2::TdscdmaSignalStrength& aTdScdmaSignalStrength);
  RefPtr<nsWcdmaSignalStrength> convertWcdmaSignalStrength(
      const RADIO_1_2::WcdmaSignalStrength& aWcdmaSignalStrength);

  RefPtr<nsSetupDataCallResult> convertDcResponse(
      const RADIO_1_4::SetupDataCallResult& aDcResponse);
#endif

  static int32_t convertRadioTechnology(RADIO_1_0::RadioTechnology aRat);
  static int32_t convertDataCallFailCause(RADIO_1_0::DataCallFailCause aCause);
#if RADIO_HAL >= 14
  static int32_t convertRadioTechnology(RADIO_1_4::RadioTechnology aRat);
  static int32_t convertDataCallFailCause(RADIO_1_4::DataCallFailCause aCause);
#endif
  static int32_t convertCellInfoType(CellInfoType type);
  static int32_t convertTimeStampType(TimeStampType type);

  nsString mRilMessageType;
  int32_t mRilMessageToken;
  int32_t mErrorMsg;

  ~nsRilResult();
};

#endif  // nsRilResult_H
