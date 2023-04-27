/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=8 sts=2 et sw=2 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "nsRilResult.h"

/* Logging related */
#undef LOG_TAG
#define LOG_TAG "nsRilResult"

#undef INFO
#undef ERROR
#undef DEBUG
#define INFO(args...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, ##args)
#define ERROR(args...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, ##args)
#define DEBUG(args...)                                         \
  do {                                                         \
    if (gRilDebug_isLoggingEnabled) {                          \
      __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, ##args); \
    }                                                          \
  } while (0)

#define RILRESULT_CID                                \
  {                                                  \
    0x0a8ace7c, 0xa92a, 0x4c52, {                    \
      0x91, 0x83, 0xf4, 0x39, 0xc7, 0xc1, 0xea, 0xe9 \
    }                                                \
  }

/*============================================================================
 *============ Implementation of Class nsGsmSignalStrength ===================
 *============================================================================*/
/**
 * nsGsmSignalStrength implementation
 */
nsGsmSignalStrength::nsGsmSignalStrength(int32_t aSignalStrength,
                                         int32_t aBitErrorRate,
                                         int32_t aTimingAdvance)
    : mSignalStrength(aSignalStrength),
      mBitErrorRate(aBitErrorRate),
      mTimingAdvance(aTimingAdvance) {
  DEBUG("init nsGsmSignalStrength");
}

NS_IMETHODIMP nsGsmSignalStrength::GetSignalStrength(int32_t* aSignalStrength) {
  *aSignalStrength = mSignalStrength;
  return NS_OK;
}
NS_IMETHODIMP nsGsmSignalStrength::GetBitErrorRate(int32_t* aBitErrorRate) {
  *aBitErrorRate = mBitErrorRate;
  return NS_OK;
}
NS_IMETHODIMP nsGsmSignalStrength::GetTimingAdvance(int32_t* aTimingAdvance) {
  *aTimingAdvance = mTimingAdvance;
  return NS_OK;
}
NS_IMPL_ISUPPORTS(nsGsmSignalStrength, nsIGsmSignalStrength)

/*============================================================================
 *============ Implementation of Class nsWcdmaSignalStrength ===================
 *============================================================================*/
/**
 * nsWcdmaSignalStrength implementation
 */
nsWcdmaSignalStrength::nsWcdmaSignalStrength(int32_t aSignalStrength,
                                             int32_t aBitErrorRate)
    : mSignalStrength(aSignalStrength), mBitErrorRate(aBitErrorRate) {
  DEBUG("init nsWcdmaSignalStrength");
}

NS_IMETHODIMP nsWcdmaSignalStrength::GetSignalStrength(
    int32_t* aSignalStrength) {
  *aSignalStrength = mSignalStrength;
  return NS_OK;
}
NS_IMETHODIMP nsWcdmaSignalStrength::GetBitErrorRate(int32_t* aBitErrorRate) {
  *aBitErrorRate = mBitErrorRate;
  return NS_OK;
}

NS_IMPL_ISUPPORTS(nsWcdmaSignalStrength, nsIWcdmaSignalStrength)

/*============================================================================
 *============ Implementation of Class nsCdmaSignalStrength ===================
 *============================================================================*/
/**
 * nsCdmaSignalStrength implementation
 */
nsCdmaSignalStrength::nsCdmaSignalStrength(int32_t aDbm, int32_t aEcio)
    : mDbm(aDbm), mEcio(aEcio) {
  DEBUG("init nsCdmaSignalStrength");
}

NS_IMETHODIMP nsCdmaSignalStrength::GetDbm(int32_t* aDbm) {
  *aDbm = mDbm;
  return NS_OK;
}
NS_IMETHODIMP nsCdmaSignalStrength::GetEcio(int32_t* aEcio) {
  *aEcio = mEcio;
  return NS_OK;
}
NS_IMPL_ISUPPORTS(nsCdmaSignalStrength, nsICdmaSignalStrength)

/*============================================================================
 *============ Implementation of Class nsEvdoSignalStrength ===================
 *============================================================================*/
/**
 * nsEvdoSignalStrength implementation
 */
nsEvdoSignalStrength::nsEvdoSignalStrength(int32_t aDbm, int32_t aEcio,
                                           int32_t aSignalNoiseRatio)
    : mDbm(aDbm), mEcio(aEcio), mSignalNoiseRatio(aSignalNoiseRatio) {
  DEBUG("init nsEvdoSignalStrength");
}

NS_IMETHODIMP nsEvdoSignalStrength::GetDbm(int32_t* aDbm) {
  *aDbm = mDbm;
  return NS_OK;
}
NS_IMETHODIMP nsEvdoSignalStrength::GetEcio(int32_t* aEcio) {
  *aEcio = mEcio;
  return NS_OK;
}
NS_IMETHODIMP nsEvdoSignalStrength::GetSignalNoiseRatio(
    int32_t* aSignalNoiseRatio) {
  *aSignalNoiseRatio = mSignalNoiseRatio;
  return NS_OK;
}
NS_IMPL_ISUPPORTS(nsEvdoSignalStrength, nsIEvdoSignalStrength)

/*============================================================================
 *============ Implementation of Class nsLteSignalStrength ===================
 *============================================================================*/
/**
 * nsLteSignalStrength implementation
 */
nsLteSignalStrength::nsLteSignalStrength(int32_t aSignalStrength, int32_t aRsrp,
                                         int32_t aRsrq, int32_t aRssnr,
                                         int32_t aCqi, int32_t aTimingAdvance)
    : mSignalStrength(aSignalStrength),
      mRsrp(aRsrp),
      mRsrq(aRsrq),
      mRssnr(aRssnr),
      mCqi(aCqi),
      mTimingAdvance(aTimingAdvance) {
  DEBUG("init nsLteSignalStrength");
}

NS_IMETHODIMP nsLteSignalStrength::GetSignalStrength(int32_t* aSignalStrength) {
  *aSignalStrength = mSignalStrength;
  return NS_OK;
}
NS_IMETHODIMP nsLteSignalStrength::GetRsrp(int32_t* aRsrp) {
  *aRsrp = mRsrp;
  return NS_OK;
}
NS_IMETHODIMP nsLteSignalStrength::GetRsrq(int32_t* aRsrq) {
  *aRsrq = mRsrq;
  return NS_OK;
}
NS_IMETHODIMP nsLteSignalStrength::GetRssnr(int32_t* aRssnr) {
  *aRssnr = mRssnr;
  return NS_OK;
}
NS_IMETHODIMP nsLteSignalStrength::GetCqi(int32_t* aCqi) {
  *aCqi = mCqi;
  return NS_OK;
}
NS_IMETHODIMP nsLteSignalStrength::GetTimingAdvance(int32_t* aTimingAdvance) {
  *aTimingAdvance = mTimingAdvance;
  return NS_OK;
}
NS_IMPL_ISUPPORTS(nsLteSignalStrength, nsILteSignalStrength)

/*============================================================================
 *============ Implementation of Class nsTdScdmaSignalStrength
 *===================
 *============================================================================*/
/**
 * nsTdScdmaSignalStrength implementation
 */
nsTdScdmaSignalStrength::nsTdScdmaSignalStrength(int32_t aRscp) : mRscp(aRscp) {
  DEBUG("init nsLteSignalStrength");
}

NS_IMETHODIMP nsTdScdmaSignalStrength::GetRscp(int32_t* aRscp) {
  *aRscp = mRscp;
  return NS_OK;
}

NS_IMPL_ISUPPORTS(nsTdScdmaSignalStrength, nsITdScdmaSignalStrength)

/*============================================================================
 *============ Implementation of Class nsSignalStrength ===================
 *============================================================================*/
/**
 * nsSignalStrength implementation
 */
nsSignalStrength::nsSignalStrength(
    nsGsmSignalStrength* aGsmSignalStrength,
    nsCdmaSignalStrength* aCdmaSignalStrength,
    nsEvdoSignalStrength* aEvdoSignalStrength,
    nsLteSignalStrength* aLteSignalStrength,
    nsTdScdmaSignalStrength* aTdScdmaSignalStrength)
    : mGsmSignalStrength(aGsmSignalStrength),
      mCdmaSignalStrength(aCdmaSignalStrength),
      mEvdoSignalStrength(aEvdoSignalStrength),
      mLteSignalStrength(aLteSignalStrength),
      mTdScdmaSignalStrength(aTdScdmaSignalStrength) {
  DEBUG("init nsSignalStrength");
}

NS_IMETHODIMP nsSignalStrength::GetGsmSignalStrength(
    nsIGsmSignalStrength** aGsmSignalStrength) {
  RefPtr<nsIGsmSignalStrength> gsmSignalStrength(mGsmSignalStrength);
  gsmSignalStrength.forget(aGsmSignalStrength);
  return NS_OK;
}

NS_IMETHODIMP nsSignalStrength::GetCdmaSignalStrength(
    nsICdmaSignalStrength** aCdmaSignalStrength) {
  RefPtr<nsICdmaSignalStrength> cdmaSignalStrength(mCdmaSignalStrength);
  cdmaSignalStrength.forget(aCdmaSignalStrength);
  return NS_OK;
}

NS_IMETHODIMP nsSignalStrength::GetEvdoSignalStrength(
    nsIEvdoSignalStrength** aEvdoSignalStrength) {
  RefPtr<nsIEvdoSignalStrength> evdoSignalStrength(mEvdoSignalStrength);
  evdoSignalStrength.forget(aEvdoSignalStrength);
  return NS_OK;
}

NS_IMETHODIMP nsSignalStrength::GetLteSignalStrength(
    nsILteSignalStrength** aLteSignalStrength) {
  RefPtr<nsILteSignalStrength> lteSignalStrength(mLteSignalStrength);
  lteSignalStrength.forget(aLteSignalStrength);
  return NS_OK;
}

NS_IMETHODIMP nsSignalStrength::GetTdscdmaSignalStrength(
    nsITdScdmaSignalStrength** aTdscdmaSignalStrength) {
  RefPtr<nsITdScdmaSignalStrength> tdscdmaSignalStrength(
      mTdScdmaSignalStrength);
  tdscdmaSignalStrength.forget(aTdscdmaSignalStrength);
  return NS_OK;
}

NS_IMPL_ISUPPORTS(nsSignalStrength, nsISignalStrength)

/*============================================================================
 *============ Implementation of Class nsSetupDataCallResult ===================
 *============================================================================*/
/**
 * nsSetupDataCallResult implementation
 */
nsSetupDataCallResult::nsSetupDataCallResult(
    int32_t aFailCause, int32_t aSuggestedRetryTime, int32_t aCid,
    int32_t aActive, const nsAString& aPdpType, const nsAString& aIfname,
    const nsAString& aAddresses, const nsAString& aDnses,
    const nsAString& aGateways, const nsAString& aPcscf, int32_t aMtu)
    : mFailCause(aFailCause),
      mSuggestedRetryTime(aSuggestedRetryTime),
      mCid(aCid),
      mActive(aActive),
      mPdpType(aPdpType),
      mIfname(aIfname),
      mAddresses(aAddresses),
      mDnses(aDnses),
      mGateways(aGateways),
      mPcscf(aPcscf),
      mMtu(aMtu) {
  DEBUG("init nsSetupDataCallResult");
}

NS_IMETHODIMP nsSetupDataCallResult::GetFailCause(int32_t* aFailCause) {
  *aFailCause = mFailCause;
  return NS_OK;
}

NS_IMETHODIMP nsSetupDataCallResult::GetSuggestedRetryTime(
    int32_t* aSuggestedRetryTime) {
  *aSuggestedRetryTime = mSuggestedRetryTime;
  return NS_OK;
}

NS_IMETHODIMP nsSetupDataCallResult::GetCid(int32_t* aCid) {
  *aCid = mCid;
  return NS_OK;
}

NS_IMETHODIMP nsSetupDataCallResult::GetActive(int32_t* aActive) {
  *aActive = mActive;
  return NS_OK;
}

NS_IMETHODIMP nsSetupDataCallResult::GetPdpType(nsAString& aPdpType) {
  aPdpType = mPdpType;
  return NS_OK;
}

NS_IMETHODIMP nsSetupDataCallResult::GetIfname(nsAString& aIfname) {
  aIfname = mIfname;
  return NS_OK;
}

NS_IMETHODIMP nsSetupDataCallResult::GetAddresses(nsAString& aAddresses) {
  aAddresses = mAddresses;
  return NS_OK;
}

NS_IMETHODIMP nsSetupDataCallResult::GetDnses(nsAString& aDnses) {
  aDnses = mDnses;
  return NS_OK;
}

NS_IMETHODIMP nsSetupDataCallResult::GetGateways(nsAString& aGateways) {
  aGateways = mGateways;
  return NS_OK;
}

NS_IMETHODIMP nsSetupDataCallResult::GetPcscf(nsAString& aPcscf) {
  aPcscf = mPcscf;
  return NS_OK;
}

NS_IMETHODIMP nsSetupDataCallResult::GetMtu(int32_t* aMtu) {
  *aMtu = mMtu;
  return NS_OK;
}

NS_IMPL_ISUPPORTS(nsSetupDataCallResult, nsISetupDataCallResult)

/*============================================================================
 *============ Implementation of Class nsSuppSvcNotification ===================
 *============================================================================*/
/**
 * nsSuppSvcNotification implementation
 */
nsSuppSvcNotification::nsSuppSvcNotification(bool aNotificationType,
                                             int32_t aCode, int32_t aIndex,
                                             int32_t aType,
                                             const nsAString& aNumber)
    : mNotificationType(aNotificationType),
      mCode(aCode),
      mIndex(aIndex),
      mType(aType),
      mNumber(aNumber) {
  DEBUG("init nsSuppSvcNotification");
}

NS_IMETHODIMP nsSuppSvcNotification::GetNotificationType(
    bool* aNotificationType) {
  *aNotificationType = mNotificationType;
  return NS_OK;
}

NS_IMETHODIMP nsSuppSvcNotification::GetCode(int32_t* aCode) {
  *aCode = mCode;
  return NS_OK;
}

NS_IMETHODIMP nsSuppSvcNotification::GetIndex(int32_t* aIndex) {
  *aIndex = mIndex;
  return NS_OK;
}

NS_IMETHODIMP nsSuppSvcNotification::GetType(int32_t* aType) {
  *aType = mType;
  return NS_OK;
}

NS_IMETHODIMP nsSuppSvcNotification::GetNumber(nsAString& aNumber) {
  aNumber = mNumber;
  return NS_OK;
}

NS_IMPL_ISUPPORTS(nsSuppSvcNotification, nsISuppSvcNotification)

/*============================================================================
 *============ Implementation of Class nsSimRefreshResult ===================
 *============================================================================*/
/**
 * nsSimRefreshResult implementation
 */
nsSimRefreshResult::nsSimRefreshResult(int32_t aType, int32_t aEfId,
                                       const nsAString& aAid)
    : mType(aType), mEfId(aEfId), mAid(aAid) {
  DEBUG("init nsSimRefreshResult");
}

NS_IMETHODIMP nsSimRefreshResult::GetType(int32_t* aType) {
  *aType = mType;
  return NS_OK;
}

NS_IMETHODIMP nsSimRefreshResult::GetEfId(int32_t* aEfId) {
  *aEfId = mEfId;
  return NS_OK;
}

NS_IMETHODIMP nsSimRefreshResult::GetAid(nsAString& aAid) {
  aAid = mAid;
  return NS_OK;
}

NS_IMPL_ISUPPORTS(nsSimRefreshResult, nsISimRefreshResult)

/*============================================================================
 *============ Implementation of Class nsCellIdentityGsm ===================
 *============================================================================*/
/**
 * nsCellIdentityGsm implementation
 */
nsCellIdentityGsm::nsCellIdentityGsm(const nsAString& aMcc,
                                     const nsAString& aMnc, int32_t aLac,
                                     int32_t aCid, int32_t aArfcn,
                                     int32_t aBsic)
    : mMcc(aMcc),
      mMnc(aMnc),
      mLac(aLac),
      mCid(aCid),
      mArfcn(aArfcn),
      mBsic(aBsic) {
  DEBUG("init nsCellIdentityGsm");
}

NS_IMETHODIMP nsCellIdentityGsm::GetMcc(nsAString& aMcc) {
  aMcc = mMcc;
  return NS_OK;
}
NS_IMETHODIMP nsCellIdentityGsm::GetMnc(nsAString& aMnc) {
  aMnc = mMnc;
  return NS_OK;
}
NS_IMETHODIMP nsCellIdentityGsm::GetLac(int32_t* aLac) {
  *aLac = mLac;
  return NS_OK;
}
NS_IMETHODIMP nsCellIdentityGsm::GetCid(int32_t* aCid) {
  *aCid = mCid;
  return NS_OK;
}

NS_IMETHODIMP nsCellIdentityGsm::GetArfcn(int32_t* aArfcn) {
  *aArfcn = mArfcn;
  return NS_OK;
}

NS_IMETHODIMP nsCellIdentityGsm::GetBsic(int32_t* aBsic) {
  *aBsic = mBsic;
  return NS_OK;
}

NS_IMPL_ISUPPORTS(nsCellIdentityGsm, nsICellIdentityGsm)

/*============================================================================
 *============ Implementation of Class nsCellIdentityCdma ===================
 *============================================================================*/
/**
 * nsCellIdentityCdma implementation
 */
nsCellIdentityCdma::nsCellIdentityCdma(int32_t aNetworkId, int32_t aSystemId,
                                       int32_t aBaseStationId,
                                       int32_t aLongitude, int32_t aLatitude)
    : mNetworkId(aNetworkId),
      mSystemId(aSystemId),
      mBaseStationId(aBaseStationId),
      mLongitude(aLongitude),
      mLatitude(aLatitude) {
  DEBUG("init nsCellIdentityCdma");
}

NS_IMETHODIMP nsCellIdentityCdma::GetNetworkId(int32_t* aNetworkId) {
  *aNetworkId = mNetworkId;
  return NS_OK;
}
NS_IMETHODIMP nsCellIdentityCdma::GetSystemId(int32_t* aSystemId) {
  *aSystemId = mSystemId;
  return NS_OK;
}

NS_IMETHODIMP nsCellIdentityCdma::GetBaseStationId(int32_t* aBaseStationId) {
  *aBaseStationId = mBaseStationId;
  return NS_OK;
}

NS_IMETHODIMP nsCellIdentityCdma::GetLongitude(int32_t* aLongitude) {
  *aLongitude = mLongitude;
  return NS_OK;
}

NS_IMETHODIMP nsCellIdentityCdma::GetLatitude(int32_t* aLatitude) {
  *aLatitude = mLatitude;
  return NS_OK;
}

NS_IMPL_ISUPPORTS(nsCellIdentityCdma, nsICellIdentityCdma)

/*============================================================================
 *============ Implementation of Class nsCellIdentityLte ===================
 *============================================================================*/
/**
 * nsCellIdentityLte implementation
 */
nsCellIdentityLte::nsCellIdentityLte(const nsAString& aMcc,
                                     const nsAString& aMnc, int32_t aCi,
                                     int32_t aPci, int32_t aTac,
                                     int32_t aEarfcn)
    : mMcc(aMcc),
      mMnc(aMnc),
      mCi(aCi),
      mPci(aPci),
      mTac(aTac),
      mEarfcn(aEarfcn) {
  DEBUG("init nsCellIdentityLte");
}

NS_IMETHODIMP nsCellIdentityLte::GetMcc(nsAString& aMcc) {
  aMcc = mMcc;
  return NS_OK;
}
NS_IMETHODIMP nsCellIdentityLte::GetMnc(nsAString& aMnc) {
  aMnc = mMnc;
  return NS_OK;
}
NS_IMETHODIMP nsCellIdentityLte::GetCi(int32_t* aCi) {
  *aCi = mCi;
  return NS_OK;
}
NS_IMETHODIMP nsCellIdentityLte::GetPci(int32_t* aPci) {
  *aPci = mPci;
  return NS_OK;
}

NS_IMETHODIMP nsCellIdentityLte::GetTac(int32_t* aTac) {
  *aTac = mTac;
  return NS_OK;
}

NS_IMETHODIMP nsCellIdentityLte::GetEarfcn(int32_t* aEarfcn) {
  *aEarfcn = mEarfcn;
  return NS_OK;
}

NS_IMPL_ISUPPORTS(nsCellIdentityLte, nsICellIdentityLte)

/*============================================================================
 *============ Implementation of Class nsCellIdentityWcdma ===================
 *============================================================================*/
/**
 * nsCellIdentityWcdma implementation
 */
nsCellIdentityWcdma::nsCellIdentityWcdma(const nsAString& aMcc,
                                         const nsAString& aMnc, int32_t aLac,
                                         int32_t aCid, int32_t aPsc,
                                         int32_t aUarfcn)
    : mMcc(aMcc),
      mMnc(aMnc),
      mLac(aLac),
      mCid(aCid),
      mPsc(aPsc),
      mUarfcn(aUarfcn) {
  DEBUG("init nsCellIdentityWcdma");
}

NS_IMETHODIMP nsCellIdentityWcdma::GetMcc(nsAString& aMcc) {
  aMcc = mMcc;
  return NS_OK;
}
NS_IMETHODIMP nsCellIdentityWcdma::GetMnc(nsAString& aMnc) {
  aMnc = mMnc;
  return NS_OK;
}
NS_IMETHODIMP nsCellIdentityWcdma::GetLac(int32_t* aLac) {
  *aLac = mLac;
  return NS_OK;
}
NS_IMETHODIMP nsCellIdentityWcdma::GetCid(int32_t* aCid) {
  *aCid = mCid;
  return NS_OK;
}

NS_IMETHODIMP nsCellIdentityWcdma::GetPsc(int32_t* aPsc) {
  *aPsc = mPsc;
  return NS_OK;
}

NS_IMETHODIMP nsCellIdentityWcdma::GetUarfcn(int32_t* aUarfcn) {
  *aUarfcn = mUarfcn;
  return NS_OK;
}

NS_IMPL_ISUPPORTS(nsCellIdentityWcdma, nsICellIdentityWcdma)

/*============================================================================
 *============ Implementation of Class nsCellIdentityTdScdma ===================
 *============================================================================*/
/**
 * nsCellIdentityTdScdma implementation
 */
nsCellIdentityTdScdma::nsCellIdentityTdScdma(const nsAString& aMcc,
                                             const nsAString& aMnc,
                                             int32_t aLac, int32_t aCid,
                                             int32_t aCpid)
    : mMcc(aMcc), mMnc(aMnc), mLac(aLac), mCid(aCid), mCpid(aCpid) {
  DEBUG("init nsCellIdentityTdScdma");
}

NS_IMETHODIMP nsCellIdentityTdScdma::GetMcc(nsAString& aMcc) {
  aMcc = mMcc;
  return NS_OK;
}
NS_IMETHODIMP nsCellIdentityTdScdma::GetMnc(nsAString& aMnc) {
  aMnc = mMnc;
  return NS_OK;
}
NS_IMETHODIMP nsCellIdentityTdScdma::GetLac(int32_t* aLac) {
  *aLac = mLac;
  return NS_OK;
}
NS_IMETHODIMP nsCellIdentityTdScdma::GetCid(int32_t* aCid) {
  *aCid = mCid;
  return NS_OK;
}

NS_IMETHODIMP nsCellIdentityTdScdma::GetCpid(int32_t* aCpid) {
  *aCpid = mCpid;
  return NS_OK;
}

NS_IMPL_ISUPPORTS(nsCellIdentityTdScdma, nsICellIdentityTdScdma)

/*============================================================================
 *============ Implementation of Class nsCellIdentity ===================
 *============================================================================*/
/**
 * nsCellIdentity implementation
 */
nsCellIdentity::nsCellIdentity(int32_t aCellInfoType,
                               nsCellIdentityGsm* aCellIdentityGsm,
                               nsCellIdentityWcdma* aCellIdentityWcdma,
                               nsCellIdentityCdma* aCellIdentityCdma,
                               nsCellIdentityLte* aCellIdentityLte,
                               nsCellIdentityTdScdma* aCellIdentityTdScdma)
    : mCellInfoType(aCellInfoType),
      mCellIdentityGsm(aCellIdentityGsm),
      mCellIdentityWcdma(aCellIdentityWcdma),
      mCellIdentityCdma(aCellIdentityCdma),
      mCellIdentityLte(aCellIdentityLte),
      mCellIdentityTdScdma(aCellIdentityTdScdma) {
  DEBUG("init nsCellIdentity");
}

nsCellIdentity::nsCellIdentity(int32_t aCellInfoType,
                               nsCellIdentityGsm* aCellIdentityGsm)
    : mCellInfoType(aCellInfoType), mCellIdentityGsm(aCellIdentityGsm) {
  DEBUG("init nsCellIdentity GSM");
}

nsCellIdentity::nsCellIdentity(int32_t aCellInfoType,
                               nsCellIdentityWcdma* aCellIdentityWcdma)
    : mCellInfoType(aCellInfoType), mCellIdentityWcdma(aCellIdentityWcdma) {
  DEBUG("init nsCellIdentity WCDMA");
}

nsCellIdentity::nsCellIdentity(int32_t aCellInfoType,
                               nsCellIdentityCdma* aCellIdentityCdma)
    : mCellInfoType(aCellInfoType), mCellIdentityCdma(aCellIdentityCdma) {
  DEBUG("init nsCellIdentity CDMA");
}

nsCellIdentity::nsCellIdentity(int32_t aCellInfoType,
                               nsCellIdentityLte* aCellIdentityLte)
    : mCellInfoType(aCellInfoType), mCellIdentityLte(aCellIdentityLte) {
  DEBUG("init nsCellIdentity LTE");
}

nsCellIdentity::nsCellIdentity(int32_t aCellInfoType,
                               nsCellIdentityTdScdma* aCellIdentityTdScdma)
    : mCellInfoType(aCellInfoType), mCellIdentityTdScdma(aCellIdentityTdScdma) {
  DEBUG("init nsCellIdentity TDCDMA");
}

NS_IMETHODIMP nsCellIdentity::GetCellInfoType(int32_t* aCellInfoType) {
  *aCellInfoType = mCellInfoType;
  return NS_OK;
}

NS_IMETHODIMP nsCellIdentity::GetCellIdentityGsm(
    nsICellIdentityGsm** aCellIdentityGsm) {
  RefPtr<nsICellIdentityGsm> cellIdentityGsm(mCellIdentityGsm);
  cellIdentityGsm.forget(aCellIdentityGsm);
  return NS_OK;
}

NS_IMETHODIMP nsCellIdentity::GetCellIdentityWcdma(
    nsICellIdentityWcdma** aCellIdentityWcdma) {
  RefPtr<nsICellIdentityWcdma> cellIdentityWcdma(mCellIdentityWcdma);
  cellIdentityWcdma.forget(aCellIdentityWcdma);
  return NS_OK;
}

NS_IMETHODIMP nsCellIdentity::GetCellIdentityCdma(
    nsICellIdentityCdma** aCellIdentityCdma) {
  RefPtr<nsICellIdentityCdma> cellIdentityCdma(mCellIdentityCdma);
  cellIdentityCdma.forget(aCellIdentityCdma);
  return NS_OK;
}

NS_IMETHODIMP nsCellIdentity::GetCellIdentityLte(
    nsICellIdentityLte** aCellIdentityLte) {
  RefPtr<nsICellIdentityLte> cellIdentityLte(mCellIdentityLte);
  cellIdentityLte.forget(aCellIdentityLte);
  return NS_OK;
}

NS_IMETHODIMP nsCellIdentity::GetCellIdentityTdScdma(
    nsICellIdentityTdScdma** aCellIdentityTdScdma) {
  RefPtr<nsICellIdentityTdScdma> cellIdentityTdScdma(mCellIdentityTdScdma);
  cellIdentityTdScdma.forget(aCellIdentityTdScdma);
  return NS_OK;
}

NS_IMPL_ISUPPORTS(nsCellIdentity, nsICellIdentity)

/*============================================================================
 *============ Implementation of Class nsCellInfoGsm ===================
 *============================================================================*/
/**
 * nsCellInfoGsm implementation
 */
nsCellInfoGsm::nsCellInfoGsm(nsCellIdentityGsm* aCellIdentityGsm,
                             nsGsmSignalStrength* aSignalStrengthGsm)
    : mCellIdentityGsm(aCellIdentityGsm),
      mSignalStrengthGsm(aSignalStrengthGsm) {
  DEBUG("init nsCellInfoGsm");
}

NS_IMETHODIMP nsCellInfoGsm::GetCellIdentityGsm(
    nsICellIdentityGsm** aCellIdentityGsm) {
  RefPtr<nsICellIdentityGsm> cellIdentityGsm(mCellIdentityGsm);
  cellIdentityGsm.forget(aCellIdentityGsm);
  return NS_OK;
}

NS_IMETHODIMP nsCellInfoGsm::GetSignalStrengthGsm(
    nsIGsmSignalStrength** aSignalStrengthGsm) {
  RefPtr<nsIGsmSignalStrength> signalStrengthGsm(mSignalStrengthGsm);
  signalStrengthGsm.forget(aSignalStrengthGsm);
  return NS_OK;
}

NS_IMPL_ISUPPORTS(nsCellInfoGsm, nsICellInfoGsm)

/*============================================================================
 *============ Implementation of Class nsCellInfoCdma ===================
 *============================================================================*/
/**
 * nsCellInfoCdma implementation
 */
nsCellInfoCdma::nsCellInfoCdma(nsCellIdentityCdma* aCellIdentityCdma,
                               nsCdmaSignalStrength* aSignalStrengthCdma,
                               nsEvdoSignalStrength* aSignalStrengthEvdo)
    : mCellIdentityCdma(aCellIdentityCdma),
      mSignalStrengthCdma(aSignalStrengthCdma),
      mSignalStrengthEvdo(aSignalStrengthEvdo) {
  DEBUG("init nsCellInfoCdma");
}

NS_IMETHODIMP nsCellInfoCdma::GetCellIdentityCdma(
    nsICellIdentityCdma** aCellIdentityCdma) {
  RefPtr<nsICellIdentityCdma> cellIdentityCdma(mCellIdentityCdma);
  cellIdentityCdma.forget(aCellIdentityCdma);
  return NS_OK;
}

NS_IMETHODIMP nsCellInfoCdma::GetSignalStrengthCdma(
    nsICdmaSignalStrength** aSignalStrengthCdma) {
  RefPtr<nsICdmaSignalStrength> signalStrengthCdma(mSignalStrengthCdma);
  signalStrengthCdma.forget(aSignalStrengthCdma);
  return NS_OK;
}

NS_IMETHODIMP nsCellInfoCdma::GetSignalStrengthEvdo(
    nsIEvdoSignalStrength** aSignalStrengthEvdo) {
  RefPtr<nsIEvdoSignalStrength> signalStrengthEvdo(mSignalStrengthEvdo);
  signalStrengthEvdo.forget(aSignalStrengthEvdo);
  return NS_OK;
}

NS_IMPL_ISUPPORTS(nsCellInfoCdma, nsICellInfoCdma)

/*============================================================================
 *============ Implementation of Class nsCellInfoLte ===================
 *============================================================================*/
/**
 * nsCellInfoLte implementation
 */
nsCellInfoLte::nsCellInfoLte(nsCellIdentityLte* aCellIdentityLte,
                             nsLteSignalStrength* aSignalStrengthLte)
    : mCellIdentityLte(aCellIdentityLte),
      mSignalStrengthLte(aSignalStrengthLte) {
  DEBUG("init nsCellInfoLte");
}

NS_IMETHODIMP nsCellInfoLte::GetCellIdentityLte(
    nsICellIdentityLte** aCellIdentityLte) {
  RefPtr<nsICellIdentityLte> cellIdentityLte(mCellIdentityLte);
  cellIdentityLte.forget(aCellIdentityLte);
  return NS_OK;
}

NS_IMETHODIMP nsCellInfoLte::GetSignalStrengthLte(
    nsILteSignalStrength** aSignalStrengthLte) {
  RefPtr<nsILteSignalStrength> signalStrengthLte(mSignalStrengthLte);
  signalStrengthLte.forget(aSignalStrengthLte);
  return NS_OK;
}

NS_IMPL_ISUPPORTS(nsCellInfoLte, nsICellInfoLte)

/*============================================================================
 *============ Implementation of Class nsCellInfoWcdma ===================
 *============================================================================*/
/**
 * nsCellInfoWcdma implementation
 */
nsCellInfoWcdma::nsCellInfoWcdma(nsCellIdentityWcdma* aCellIdentityWcdma,
                                 nsWcdmaSignalStrength* aSignalStrengthWcdma)
    : mCellIdentityWcdma(aCellIdentityWcdma),
      mSignalStrengthWcdma(aSignalStrengthWcdma) {
  DEBUG("init nsCellInfoWcdma");
}

NS_IMETHODIMP nsCellInfoWcdma::GetCellIdentityWcdma(
    nsICellIdentityWcdma** aCellIdentityWcdma) {
  RefPtr<nsICellIdentityWcdma> cellIdentityWcdma(mCellIdentityWcdma);
  cellIdentityWcdma.forget(aCellIdentityWcdma);
  return NS_OK;
}

NS_IMETHODIMP nsCellInfoWcdma::GetSignalStrengthWcdma(
    nsIWcdmaSignalStrength** aSignalStrengthWcdma) {
  RefPtr<nsIWcdmaSignalStrength> signalStrengthWcdma(mSignalStrengthWcdma);
  signalStrengthWcdma.forget(aSignalStrengthWcdma);
  return NS_OK;
}

NS_IMPL_ISUPPORTS(nsCellInfoWcdma, nsICellInfoWcdma)

/*============================================================================
 *============ Implementation of Class nsCellInfoTdScdma ===================
 *============================================================================*/
/**
 * nsCellInfoTdScdma implementation
 */
nsCellInfoTdScdma::nsCellInfoTdScdma(
    nsCellIdentityTdScdma* aCellIdentityTdScdma,
    nsTdScdmaSignalStrength* aSignalStrengthTdScdma)
    : mCellIdentityTdScdma(aCellIdentityTdScdma),
      mSignalStrengthTdScdma(aSignalStrengthTdScdma) {
  DEBUG("init nsCellInfoTdScdma");
}

NS_IMETHODIMP nsCellInfoTdScdma::GetCellIdentityTdScdma(
    nsICellIdentityTdScdma** aCellIdentityTdScdma) {
  RefPtr<nsICellIdentityTdScdma> cellIdentityTdScdma(mCellIdentityTdScdma);
  cellIdentityTdScdma.forget(aCellIdentityTdScdma);
  return NS_OK;
}

NS_IMETHODIMP nsCellInfoTdScdma::GetSignalStrengthTdScdma(
    nsITdScdmaSignalStrength** aSignalStrengthTdScdma) {
  RefPtr<nsITdScdmaSignalStrength> signalStrengthTdScdma(
      mSignalStrengthTdScdma);
  signalStrengthTdScdma.forget(aSignalStrengthTdScdma);
  return NS_OK;
}

NS_IMPL_ISUPPORTS(nsCellInfoTdScdma, nsICellInfoTdScdma)

/*============================================================================
 *============ Implementation of Class nsRilCellInfo ===================
 *============================================================================*/
/**
 * nsRilCellInfo implementation
 */
nsRilCellInfo::nsRilCellInfo(int32_t aCellInfoType, bool aRegistered,
                             int32_t aTimeStampType, int32_t aTimeStamp,
                             nsCellInfoGsm* aGsm, nsCellInfoCdma* aCdma,
                             nsCellInfoLte* aLte, nsCellInfoWcdma* aWcdma,
                             nsCellInfoTdScdma* aTdScdma)
    : mCellInfoType(aCellInfoType),
      mRegistered(aRegistered),
      mTimeStampType(aTimeStampType),
      mTimeStamp(aTimeStamp),
      mGsm(aGsm),
      mCdma(aCdma),
      mLte(aLte),
      mWcdma(aWcdma),
      mTdScdma(aTdScdma) {
  DEBUG("init nsRilCellInfo");
}

nsRilCellInfo::nsRilCellInfo(int32_t aCellInfoType, bool aRegistered,
                             int32_t aTimeStampType, int32_t aTimeStamp,
                             nsCellInfoGsm* aGsm)
    : mCellInfoType(aCellInfoType),
      mRegistered(aRegistered),
      mTimeStampType(aTimeStampType),
      mTimeStamp(aTimeStamp),
      mGsm(aGsm) {
  DEBUG("init nsRilCellInfo GSM");
}

nsRilCellInfo::nsRilCellInfo(int32_t aCellInfoType, bool aRegistered,
                             int32_t aTimeStampType, int32_t aTimeStamp,
                             nsCellInfoWcdma* aWcdma)
    : mCellInfoType(aCellInfoType),
      mRegistered(aRegistered),
      mTimeStampType(aTimeStampType),
      mTimeStamp(aTimeStamp),
      mWcdma(aWcdma) {
  DEBUG("init nsRilCellInfo WCDMA");
}

nsRilCellInfo::nsRilCellInfo(int32_t aCellInfoType, bool aRegistered,
                             int32_t aTimeStampType, int32_t aTimeStamp,
                             nsCellInfoCdma* aCdma)
    : mCellInfoType(aCellInfoType),
      mRegistered(aRegistered),
      mTimeStampType(aTimeStampType),
      mTimeStamp(aTimeStamp),
      mCdma(aCdma) {
  DEBUG("init nsRilCellInfo CDMA");
}

nsRilCellInfo::nsRilCellInfo(int32_t aCellInfoType, bool aRegistered,
                             int32_t aTimeStampType, int32_t aTimeStamp,
                             nsCellInfoLte* aLte)
    : mCellInfoType(aCellInfoType),
      mRegistered(aRegistered),
      mTimeStampType(aTimeStampType),
      mTimeStamp(aTimeStamp),
      mLte(aLte) {
  DEBUG("init nsRilCellInfo LTE");
}

nsRilCellInfo::nsRilCellInfo(int32_t aCellInfoType, bool aRegistered,
                             int32_t aTimeStampType, int32_t aTimeStamp,
                             nsCellInfoTdScdma* aTdScdma)
    : mCellInfoType(aCellInfoType),
      mRegistered(aRegistered),
      mTimeStampType(aTimeStampType),
      mTimeStamp(aTimeStamp),
      mTdScdma(aTdScdma) {
  DEBUG("init nsRilCellInfo TDCDMA");
}

NS_IMETHODIMP nsRilCellInfo::GetCellInfoType(int32_t* aCellInfoType) {
  *aCellInfoType = mCellInfoType;
  return NS_OK;
}

NS_IMETHODIMP nsRilCellInfo::GetRegistered(bool* aRegistered) {
  *aRegistered = mRegistered;
  return NS_OK;
}

NS_IMETHODIMP nsRilCellInfo::GetTimeStampType(int32_t* aTimeStampType) {
  *aTimeStampType = mTimeStampType;
  return NS_OK;
}

NS_IMETHODIMP nsRilCellInfo::GetTimeStamp(int32_t* aTimeStamp) {
  *aTimeStamp = mTimeStamp;
  return NS_OK;
}

NS_IMETHODIMP nsRilCellInfo::GetGsm(nsICellInfoGsm** aGsm) {
  RefPtr<nsICellInfoGsm> cellInfoGsm(mGsm);
  cellInfoGsm.forget(aGsm);
  return NS_OK;
}

NS_IMETHODIMP nsRilCellInfo::GetCdma(nsICellInfoCdma** aCdma) {
  RefPtr<nsICellInfoCdma> cellInfoCdma(mCdma);
  cellInfoCdma.forget(aCdma);
  return NS_OK;
}

NS_IMETHODIMP nsRilCellInfo::GetLte(nsICellInfoLte** aLte) {
  RefPtr<nsICellInfoLte> cellInfoLte(mLte);
  cellInfoLte.forget(aLte);
  return NS_OK;
}

NS_IMETHODIMP nsRilCellInfo::GetWcdma(nsICellInfoWcdma** aWcdma) {
  RefPtr<nsICellInfoWcdma> cellInfoWcdma(mWcdma);
  cellInfoWcdma.forget(aWcdma);
  return NS_OK;
}

NS_IMETHODIMP nsRilCellInfo::GetTdscdma(nsICellInfoTdScdma** aTdScdma) {
  RefPtr<nsICellInfoTdScdma> cellInfoTdScdma(mTdScdma);
  cellInfoTdScdma.forget(aTdScdma);
  return NS_OK;
}

NS_IMPL_ISUPPORTS(nsRilCellInfo, nsIRilCellInfo)

/*============================================================================
 *============ Implementation of Class nsHardwareConfig ===================
 *============================================================================*/
/**
 * nsHardwareConfig implementation
 */
nsHardwareConfig::nsHardwareConfig(int32_t aType, const nsAString& aUuid,
                                   int32_t aState,
                                   nsIHardwareConfigModem* aModem,
                                   nsIHardwareConfigSim* aSim)
    : mType(aType), mUuid(aUuid), mState(aState), mModem(aModem), mSim(aSim) {
  DEBUG("init nsHardwareConfig");
}

NS_IMETHODIMP nsHardwareConfig::GetType(int32_t* aType) {
  *aType = mType;
  return NS_OK;
}

NS_IMETHODIMP nsHardwareConfig::GetUuid(nsAString& aUuid) {
  aUuid = mUuid;
  return NS_OK;
}

NS_IMETHODIMP nsHardwareConfig::GetState(int32_t* aState) {
  *aState = mState;
  return NS_OK;
}

NS_IMETHODIMP nsHardwareConfig::GetModem(nsIHardwareConfigModem** aModem) {
  RefPtr<nsIHardwareConfigModem> hwConfigModem(mModem);
  hwConfigModem.forget(aModem);
  return NS_OK;
}

NS_IMETHODIMP nsHardwareConfig::GetSim(nsIHardwareConfigSim** aSim) {
  RefPtr<nsIHardwareConfigSim> hwConfigSim(mSim);
  hwConfigSim.forget(aSim);
  return NS_OK;
}

NS_IMPL_ISUPPORTS(nsHardwareConfig, nsIHardwareConfig)

/*============================================================================
 *============ Implementation of Class nsHardwareConfigModem ===================
 *============================================================================*/
/**
 * nsHardwareConfigModem implementation
 */
nsHardwareConfigModem::nsHardwareConfigModem(int32_t aRilModel, int32_t aRat,
                                             int32_t aMaxVoice,
                                             int32_t aMaxData,
                                             int32_t aMaxStandby)
    : mRilModel(aRilModel),
      mRat(aRat),
      mMaxVoice(aMaxVoice),
      mMaxData(aMaxData),
      mMaxStandby(aMaxStandby) {
  DEBUG("init nsHardwareConfigModem");
}

NS_IMETHODIMP nsHardwareConfigModem::GetRilModel(int32_t* aRilModel) {
  *aRilModel = mRilModel;
  return NS_OK;
}

NS_IMETHODIMP nsHardwareConfigModem::GetRat(int32_t* aRat) {
  *aRat = mRat;
  return NS_OK;
}

NS_IMETHODIMP nsHardwareConfigModem::GetMaxVoice(int32_t* aMaxVoice) {
  *aMaxVoice = mMaxVoice;
  return NS_OK;
}
NS_IMETHODIMP nsHardwareConfigModem::GetMaxData(int32_t* aMaxData) {
  *aMaxData = mMaxData;
  return NS_OK;
}

NS_IMETHODIMP nsHardwareConfigModem::GetMaxStandby(int32_t* aMaxStandby) {
  *aMaxStandby = mMaxStandby;
  return NS_OK;
}

NS_IMPL_ISUPPORTS(nsHardwareConfigModem, nsIHardwareConfigModem)

/*============================================================================
 *============ Implementation of Class nsHardwareConfigSim ===================
 *============================================================================*/
/**
 * nsHardwareConfigSim implementation
 */
nsHardwareConfigSim::nsHardwareConfigSim(const nsAString& aModemUuid)
    : mModemUuid(aModemUuid) {
  DEBUG("init nsHardwareConfigSim");
}

NS_IMETHODIMP nsHardwareConfigSim::GetModemUuid(nsAString& aModemUuid) {
  aModemUuid = mModemUuid;
  return NS_OK;
}

NS_IMPL_ISUPPORTS(nsHardwareConfigSim, nsIHardwareConfigSim)

/*============================================================================
 *============ Implementation of Class nsRadioCapability ===================
 *============================================================================*/
/**
 * nsRadioCapability implementation
 */
nsRadioCapability::nsRadioCapability(int32_t aSession, int32_t aPhase,
                                     int32_t aRaf,
                                     const nsAString& aLogicalModemUuid,
                                     int32_t aStatus)
    : mSession(aSession),
      mPhase(aPhase),
      mRaf(aRaf),
      mLogicalModemUuid(aLogicalModemUuid),
      mStatus(aStatus) {
  DEBUG("init nsRadioCapability");
}

NS_IMETHODIMP nsRadioCapability::GetSession(int32_t* aSession) {
  *aSession = mSession;
  return NS_OK;
}

NS_IMETHODIMP nsRadioCapability::GetPhase(int32_t* aPhase) {
  *aPhase = mPhase;
  return NS_OK;
}

NS_IMETHODIMP nsRadioCapability::GetRaf(int32_t* aRaf) {
  *aRaf = mRaf;
  return NS_OK;
}

NS_IMETHODIMP nsRadioCapability::GetLogicalModemUuid(
    nsAString& aLogicalModemUuid) {
  aLogicalModemUuid = mLogicalModemUuid;
  return NS_OK;
}

NS_IMETHODIMP nsRadioCapability::GetStatus(int32_t* aStatus) {
  *aStatus = mStatus;
  return NS_OK;
}

NS_IMPL_ISUPPORTS(nsRadioCapability, nsIRadioCapability)

/*============================================================================
 *============ Implementation of Class nsLceStatusInfo ===================
 *============================================================================*/
/**
 * nsLceStatusInfo implementation
 */
nsLceStatusInfo::nsLceStatusInfo(int32_t aLceStatus, int32_t aActualIntervalMs)
    : mLceStatus(aLceStatus), mActualIntervalMs(aActualIntervalMs) {
  DEBUG("init nsLceStatusInfo");
}

NS_IMETHODIMP nsLceStatusInfo::GetLceStatus(int32_t* aLceStatus) {
  *aLceStatus = mLceStatus;
  return NS_OK;
}

NS_IMETHODIMP nsLceStatusInfo::GetActualIntervalMs(int32_t* aActualIntervalMs) {
  *aActualIntervalMs = mActualIntervalMs;
  return NS_OK;
}

NS_IMPL_ISUPPORTS(nsLceStatusInfo, nsILceStatusInfo)

/*============================================================================
 *============ Implementation of Class nsLceDataInfo ===================
 *============================================================================*/
/**
 * nsLceDataInfo implementation
 */
nsLceDataInfo::nsLceDataInfo(int32_t aLastHopCapacityKbps,
                             int32_t aConfidenceLevel, bool aLceSuspended)
    : mLastHopCapacityKbps(aLastHopCapacityKbps),
      mConfidenceLevel(aConfidenceLevel),
      mLceSuspended(aLceSuspended) {
  DEBUG("init nsLceDataInfo");
}

NS_IMETHODIMP nsLceDataInfo::GetLastHopCapacityKbps(
    int32_t* aLastHopCapacityKbps) {
  *aLastHopCapacityKbps = mLastHopCapacityKbps;
  return NS_OK;
}

NS_IMETHODIMP nsLceDataInfo::GetConfidenceLevel(int32_t* aConfidenceLevel) {
  *aConfidenceLevel = mConfidenceLevel;
  return NS_OK;
}

NS_IMETHODIMP nsLceDataInfo::GetLceSuspended(bool* aLceSuspended) {
  *aLceSuspended = mLceSuspended;
  return NS_OK;
}

NS_IMPL_ISUPPORTS(nsLceDataInfo, nsILceDataInfo)

/*============================================================================
 *============ Implementation of Class nsPcoDataInfo ===================
 *============================================================================*/
/**
 * nsPcoDataInfo implementation
 */
nsPcoDataInfo::nsPcoDataInfo(int32_t aCid, const nsAString& aBearerProto,
                             bool aPcoId, nsTArray<int32_t>& aContents)
    : mCid(aCid),
      mBearerProto(aBearerProto),
      mPcoId(aPcoId),
      mContents(aContents.Clone()) {
  DEBUG("init nsPcoDataInfo");
}

NS_IMETHODIMP nsPcoDataInfo::GetCid(int32_t* aCid) {
  *aCid = mCid;
  return NS_OK;
}

NS_IMETHODIMP nsPcoDataInfo::GetBearerProto(nsAString& aBearerProto) {
  aBearerProto = mBearerProto;
  return NS_OK;
}

NS_IMETHODIMP nsPcoDataInfo::GetPcoId(int32_t* aPcoId) {
  *aPcoId = mPcoId;
  return NS_OK;
}

NS_IMETHODIMP nsPcoDataInfo::GetContents(uint32_t* count, int32_t** contents) {
  *count = mContents.Length();
  *contents = (int32_t*)moz_xmalloc((*count) * sizeof(int32_t));
  NS_ENSURE_TRUE(*contents, NS_ERROR_OUT_OF_MEMORY);

  for (uint32_t i = 0; i < *count; i++) {
    (*contents)[i] = mContents[i];
  }

  return NS_OK;
}

NS_IMPL_ISUPPORTS(nsPcoDataInfo, nsIPcoDataInfo)

/*============================================================================
 *============ Implementation of Class nsAppStatus ===================
 *============================================================================*/
/**
 * nsAppStatus implementation
 */
nsAppStatus::nsAppStatus(int32_t aAppType, int32_t aAppState,
                         int32_t aPersoSubstate, const nsAString& aAidPtr,
                         const nsAString& aAppLabelPtr, int32_t aPin1Replaced,
                         int32_t aPin1, int32_t aPin2)
    : mAppType(aAppType),
      mAppState(aAppState),
      mPersoSubstate(aPersoSubstate),
      mAidPtr(aAidPtr),
      mAppLabelPtr(aAppLabelPtr),
      mPin1Replaced(aPin1Replaced),
      mPin1(aPin1),
      mPin2(aPin2) {
  DEBUG("init nsAppStatus");
}

NS_IMETHODIMP nsAppStatus::GetAppType(int32_t* aAppType) {
  *aAppType = mAppType;
  return NS_OK;
}

NS_IMETHODIMP nsAppStatus::GetAppState(int32_t* aAppState) {
  *aAppState = mAppState;
  return NS_OK;
}

NS_IMETHODIMP nsAppStatus::GetPersoSubstate(int32_t* aPersoSubstate) {
  *aPersoSubstate = mPersoSubstate;
  return NS_OK;
}

NS_IMETHODIMP nsAppStatus::GetAidPtr(nsAString& aAidPtr) {
  aAidPtr = mAidPtr;
  return NS_OK;
}

NS_IMETHODIMP nsAppStatus::GetAppLabelPtr(nsAString& aAppLabelPtr) {
  aAppLabelPtr = mAppLabelPtr;
  return NS_OK;
}

NS_IMETHODIMP nsAppStatus::GetPin1Replaced(int32_t* aPin1Replaced) {
  *aPin1Replaced = mPin1Replaced;
  return NS_OK;
}

NS_IMETHODIMP nsAppStatus::GetPin1(int32_t* aPin1) {
  *aPin1 = mPin1;
  return NS_OK;
}

NS_IMETHODIMP nsAppStatus::GetPin2(int32_t* aPin2) {
  *aPin2 = mPin2;
  return NS_OK;
}

NS_IMPL_ISUPPORTS(nsAppStatus, nsIAppStatus)

/*============================================================================
 *============ Implementation of Class nsCardStatus ===================
 *============================================================================*/
/**
 * nsCardStatus implementation
 */
nsCardStatus::nsCardStatus(int32_t aCardState, int32_t aUniversalPinState,
                           int32_t aGsmUmtsSubscriptionAppIndex,
                           int32_t aCdmaSubscriptionAppIndex,
                           int32_t aImsSubscriptionAppIndex,
                           nsTArray<RefPtr<nsAppStatus>>& aApplications)
    : mCardState(aCardState),
      mUniversalPinState(aUniversalPinState),
      mGsmUmtsSubscriptionAppIndex(aGsmUmtsSubscriptionAppIndex),
      mCdmaSubscriptionAppIndex(aCdmaSubscriptionAppIndex),
      mImsSubscriptionAppIndex(aImsSubscriptionAppIndex),
      mApplications(aApplications.Clone()) {
  DEBUG("init nsCardStatus");
}

NS_IMETHODIMP nsCardStatus::GetCardState(int32_t* aCardState) {
  *aCardState = mCardState;
  return NS_OK;
}

NS_IMETHODIMP nsCardStatus::GetUniversalPinState(int32_t* aUniversalPinState) {
  *aUniversalPinState = mUniversalPinState;
  return NS_OK;
}

NS_IMETHODIMP nsCardStatus::GetGsmUmtsSubscriptionAppIndex(
    int32_t* aGsmUmtsSubscriptionAppIndex) {
  *aGsmUmtsSubscriptionAppIndex = mGsmUmtsSubscriptionAppIndex;
  return NS_OK;
}

NS_IMETHODIMP nsCardStatus::GetCdmaSubscriptionAppIndex(
    int32_t* aCdmaSubscriptionAppIndex) {
  *aCdmaSubscriptionAppIndex = mCdmaSubscriptionAppIndex;
  return NS_OK;
}

NS_IMETHODIMP nsCardStatus::GetImsSubscriptionAppIndex(
    int32_t* aImsSubscriptionAppIndex) {
  *aImsSubscriptionAppIndex = mImsSubscriptionAppIndex;
  return NS_OK;
}

NS_IMETHODIMP nsCardStatus::GetAppStatus(uint32_t* count,
                                         nsIAppStatus*** applications) {
  // Allocate prefix arrays
  *count = mApplications.Length();
  nsIAppStatus** application =
      (nsIAppStatus**)moz_xmalloc(*count * sizeof(nsIAppStatus*));

  for (uint32_t i = 0; i < *count; i++) {
    NS_ADDREF(application[i] = mApplications[i]);
  }

  *applications = application;

  return NS_OK;
}

NS_IMPL_ISUPPORTS(nsCardStatus, nsICardStatus)

/*============================================================================
 *============ Implementation of Class nsVoiceRegState ===================
 *============================================================================*/
/**
 * nsVoiceRegState implementation
 */
nsVoiceRegState::nsVoiceRegState(int32_t aRegState, int32_t aRat,
                                 bool aCssSupported, int32_t aRoamingIndicator,
                                 int32_t aSystemIsInPrl,
                                 int32_t aDefaultRoamingIndicator,
                                 int32_t aReasonForDenial,
                                 nsCellIdentity* aCellIdentity)
    : mRegState(aRegState),
      mRat(aRat),
      mCssSupported(aCssSupported),
      mRoamingIndicator(aRoamingIndicator),
      mSystemIsInPrl(aSystemIsInPrl),
      mDefaultRoamingIndicator(aDefaultRoamingIndicator),
      mReasonForDenial(aReasonForDenial),
      mCellIdentity(aCellIdentity) {
  DEBUG("init nsVoiceRegState");
}

NS_IMETHODIMP nsVoiceRegState::GetRegState(int32_t* aRegState) {
  *aRegState = mRegState;
  return NS_OK;
}

NS_IMETHODIMP nsVoiceRegState::GetRat(int32_t* aRat) {
  *aRat = mRat;
  return NS_OK;
}

NS_IMETHODIMP nsVoiceRegState::GetCssSupported(bool* aCssSupported) {
  *aCssSupported = mCssSupported;
  return NS_OK;
}

NS_IMETHODIMP nsVoiceRegState::GetRoamingIndicator(int32_t* aRoamingIndicator) {
  *aRoamingIndicator = mRoamingIndicator;
  return NS_OK;
}

NS_IMETHODIMP nsVoiceRegState::GetSystemIsInPrl(int32_t* aSystemIsInPrl) {
  *aSystemIsInPrl = mSystemIsInPrl;
  return NS_OK;
}

NS_IMETHODIMP nsVoiceRegState::GetDefaultRoamingIndicator(
    int32_t* aDefaultRoamingIndicator) {
  *aDefaultRoamingIndicator = mDefaultRoamingIndicator;
  return NS_OK;
}

NS_IMETHODIMP nsVoiceRegState::GetReasonForDenial(int32_t* aReasonForDenial) {
  *aReasonForDenial = mReasonForDenial;
  return NS_OK;
}

NS_IMETHODIMP nsVoiceRegState::GetCellIdentity(
    nsICellIdentity** aCellIdentity) {
  RefPtr<nsICellIdentity> cellIdentity(mCellIdentity);
  cellIdentity.forget(aCellIdentity);
  return NS_OK;
}

NS_IMPL_ISUPPORTS(nsVoiceRegState, nsIVoiceRegState)

/*============================================================================
 *============ Implementation of Class nsDataRegState ===================
 *============================================================================*/
/**
 * nsDataRegState implementation
 */
nsDataRegState::nsDataRegState(int32_t aRegState, int32_t aRat,
                               int32_t aReasonDataDenied, int32_t aMaxDataCalls,
                               nsCellIdentity* aCellIdentity)
    : mRegState(aRegState),
      mRat(aRat),
      mReasonDataDenied(aReasonDataDenied),
      mMaxDataCalls(aMaxDataCalls),
      mCellIdentity(aCellIdentity) {
  DEBUG("init nsDataRegState");
}

NS_IMETHODIMP nsDataRegState::GetRegState(int32_t* aRegState) {
  *aRegState = mRegState;
  return NS_OK;
}

NS_IMETHODIMP nsDataRegState::GetRat(int32_t* aRat) {
  *aRat = mRat;
  return NS_OK;
}

NS_IMETHODIMP nsDataRegState::GetReasonDataDenied(int32_t* aReasonDataDenied) {
  *aReasonDataDenied = mReasonDataDenied;
  return NS_OK;
}

NS_IMETHODIMP nsDataRegState::GetMaxDataCalls(int32_t* aMaxDataCalls) {
  *aMaxDataCalls = mMaxDataCalls;
  return NS_OK;
}

NS_IMETHODIMP nsDataRegState::GetCellIdentity(nsICellIdentity** aCellIdentity) {
  RefPtr<nsICellIdentity> cellIdentity(mCellIdentity);
  cellIdentity.forget(aCellIdentity);
  return NS_OK;
}

NS_IMPL_ISUPPORTS(nsDataRegState, nsIDataRegState)

/*============================================================================
 *============ Implementation of Class nsOperatorInfo ===================
 *============================================================================*/
/**
 * nsOperatorInfo implementation
 */
nsOperatorInfo::nsOperatorInfo(const nsAString& aAlphaLong,
                               const nsAString& aAlphaShort,
                               const nsAString& aOperatorNumeric,
                               int32_t aStatus)
    : mAlphaLong(aAlphaLong),
      mAlphaShort(aAlphaShort),
      mOperatorNumeric(aOperatorNumeric),
      mStatus(aStatus) {
  DEBUG("init nsOperatorInfo");
}

NS_IMETHODIMP nsOperatorInfo::GetAlphaLong(nsAString& aAlphaLong) {
  aAlphaLong = mAlphaLong;
  return NS_OK;
}

NS_IMETHODIMP nsOperatorInfo::GetAlphaShort(nsAString& aAlphaShort) {
  aAlphaShort = mAlphaShort;
  return NS_OK;
}

NS_IMETHODIMP nsOperatorInfo::GetOperatorNumeric(nsAString& aOperatorNumeric) {
  aOperatorNumeric = mOperatorNumeric;
  return NS_OK;
}

NS_IMETHODIMP nsOperatorInfo::GetStatus(int32_t* aStatus) {
  *aStatus = mStatus;
  return NS_OK;
}

NS_IMPL_ISUPPORTS(nsOperatorInfo, nsIOperatorInfo)

/*============================================================================
 *============ Implementation of Class nsNeighboringCell ===================
 *============================================================================*/
/**
 * nsNeighboringCell implementation
 */
nsNeighboringCell::nsNeighboringCell(const nsAString& aCid, int32_t aRssi)
    : mCid(aCid), mRssi(aRssi) {
  DEBUG("init nsNeighboringCell");
}

NS_IMETHODIMP nsNeighboringCell::GetCid(nsAString& aCid) {
  aCid = mCid;
  return NS_OK;
}

NS_IMETHODIMP nsNeighboringCell::GetRssi(int32_t* aRssi) {
  *aRssi = mRssi;
  return NS_OK;
}

NS_IMPL_ISUPPORTS(nsNeighboringCell, nsINeighboringCell)

/*============================================================================
 *============ Implementation of Class nsCall ===================
 *============================================================================*/
/**
 * nsCall implementation
 */
nsCall::nsCall(int32_t aState, int32_t aIndex, int32_t aToa, bool aIsMpty,
               bool aIsMT, int32_t aAls, bool aIsVoice, bool aIsVoicePrivacy,
               const nsAString& aNumber, int32_t aNumberPresentation,
               const nsAString& aName, int32_t aNamePresentation,
               nsTArray<RefPtr<nsUusInfo>>& aUusInfo)
    : mState(aState),
      mIndex(aIndex),
      mToa(aToa),
      mIsMpty(aIsMpty),
      mIsMT(aIsMT),
      mAls(aAls),
      mIsVoice(aIsVoice),
      mIsVoicePrivacy(aIsVoicePrivacy),
      mNumber(aNumber),
      mNumberPresentation(aNumberPresentation),
      mName(aName),
      mNamePresentation(aNamePresentation),
      mUusInfo(aUusInfo.Clone()) {
  DEBUG("init nsCall");
}

NS_IMETHODIMP nsCall::GetState(int32_t* aState) {
  *aState = mState;
  return NS_OK;
}

NS_IMETHODIMP nsCall::GetIndex(int32_t* aIndex) {
  *aIndex = mIndex;
  return NS_OK;
}

NS_IMETHODIMP nsCall::GetToa(int32_t* aToa) {
  *aToa = mToa;
  return NS_OK;
}

NS_IMETHODIMP nsCall::GetIsMpty(bool* aIsMpty) {
  *aIsMpty = mIsMpty;
  return NS_OK;
}

NS_IMETHODIMP nsCall::GetIsMT(bool* aIsMT) {
  *aIsMT = mIsMT;
  return NS_OK;
}

NS_IMETHODIMP nsCall::GetAls(int32_t* aAls) {
  *aAls = mAls;
  return NS_OK;
}

NS_IMETHODIMP nsCall::GetIsVoice(bool* aIsVoice) {
  *aIsVoice = mIsVoice;
  return NS_OK;
}

NS_IMETHODIMP nsCall::GetIsVoicePrivacy(bool* aIsVoicePrivacy) {
  *aIsVoicePrivacy = mIsVoicePrivacy;
  return NS_OK;
}

NS_IMETHODIMP nsCall::GetNumber(nsAString& aNumber) {
  aNumber = mNumber;
  return NS_OK;
}

NS_IMETHODIMP nsCall::GetNumberPresentation(int32_t* aNumberPresentation) {
  *aNumberPresentation = mNumberPresentation;
  return NS_OK;
}

NS_IMETHODIMP nsCall::GetName(nsAString& aName) {
  aName = mName;
  return NS_OK;
}

NS_IMETHODIMP nsCall::GetNamePresentation(int32_t* aNamePresentation) {
  *aNamePresentation = mNamePresentation;
  return NS_OK;
}

NS_IMETHODIMP nsCall::GetUusInfo(uint32_t* count, nsIUusInfo*** uusInfos) {
  *count = mUusInfo.Length();
  nsIUusInfo** uusinfo =
      (nsIUusInfo**)moz_xmalloc(*count * sizeof(nsIUusInfo*));

  for (uint32_t i = 0; i < *count; i++) {
    NS_ADDREF(uusinfo[i] = mUusInfo[i]);
  }

  *uusInfos = uusinfo;
  return NS_OK;
}

NS_IMPL_ISUPPORTS(nsCall, nsICall)

/*============================================================================
 *============ Implementation of Class nsUusInfo ===================
 *============================================================================*/
/**
 * nsUusInfo implementation
 */
nsUusInfo::nsUusInfo(int32_t aUusType, int32_t aUusDcs,
                     const nsAString& aUusData)
    : mUusType(aUusType), mUusDcs(aUusDcs), mUusData(aUusData) {
  DEBUG("init nsUusInfo");
}

NS_IMETHODIMP nsUusInfo::GetUusType(int32_t* aUusType) {
  *aUusType = mUusType;
  return NS_OK;
}

NS_IMETHODIMP nsUusInfo::GetUusDcs(int32_t* aUusDcs) {
  *aUusDcs = mUusDcs;
  return NS_OK;
}

NS_IMETHODIMP nsUusInfo::GetUusData(nsAString& aUusData) {
  aUusData = mUusData;
  return NS_OK;
}

NS_IMPL_ISUPPORTS(nsUusInfo, nsIUusInfo)

/*============================================================================
 *============ Implementation of Class nsIccIoResult ===================
 *============================================================================*/
/**
 * nsIccIoResult implementation
 */
nsIccIoResult::nsIccIoResult(int32_t aSw1, int32_t aSw2,
                             const nsAString& aSimResponse)
    : mSw1(aSw1), mSw2(aSw2), mSimResponse(aSimResponse) {
  DEBUG("init nsIccIoResult");
}

NS_IMETHODIMP nsIccIoResult::GetSw1(int32_t* aSw1) {
  *aSw1 = mSw1;
  return NS_OK;
}

NS_IMETHODIMP nsIccIoResult::GetSw2(int32_t* aSw2) {
  *aSw2 = mSw2;
  return NS_OK;
}

NS_IMETHODIMP nsIccIoResult::GetSimResponse(nsAString& aSimResponse) {
  aSimResponse = mSimResponse;
  return NS_OK;
}

NS_IMPL_ISUPPORTS(nsIccIoResult, nsIIccIoResult)

/*============================================================================
 *============ Implementation of Class nsCallForwardInfo ===================
 *============================================================================*/
/**
 * nsCallForwardInfo implementation
 */
nsCallForwardInfo::nsCallForwardInfo(int32_t aStatus, int32_t aReason,
                                     int32_t aServiceClass, int32_t aToa,
                                     const nsAString& aNumber,
                                     int32_t aTimeSeconds)
    : mStatus(aStatus),
      mReason(aReason),
      mServiceClass(aServiceClass),
      mToa(aToa),
      mNumber(aNumber),
      mTimeSeconds(aTimeSeconds) {
  DEBUG("init nsCallForwardInfo");
}

NS_IMETHODIMP nsCallForwardInfo::GetStatus(int32_t* aStatus) {
  *aStatus = mStatus;
  return NS_OK;
}

NS_IMETHODIMP nsCallForwardInfo::GetReason(int32_t* aReason) {
  *aReason = mReason;
  return NS_OK;
}

NS_IMETHODIMP nsCallForwardInfo::GetServiceClass(int32_t* aServiceClass) {
  *aServiceClass = mServiceClass;
  return NS_OK;
}

NS_IMETHODIMP nsCallForwardInfo::GetToa(int32_t* aToa) {
  *aToa = mToa;
  return NS_OK;
}

NS_IMETHODIMP nsCallForwardInfo::GetNumber(nsAString& aNumber) {
  aNumber = mNumber;
  return NS_OK;
}

NS_IMETHODIMP nsCallForwardInfo::GetTimeSeconds(int32_t* aTimeSeconds) {
  *aTimeSeconds = mTimeSeconds;
  return NS_OK;
}

NS_IMPL_ISUPPORTS(nsCallForwardInfo, nsICallForwardInfo)

/*============================================================================
 *============ Implementation of Class nsSendSmsResult ===================
 *============================================================================*/
/**
 * nsSendSmsResult implementation
 */
nsSendSmsResult::nsSendSmsResult(int32_t aMessageRef, const nsAString& aAckPDU,
                                 int32_t aErrorCode)
    : mMessageRef(aMessageRef), mAckPDU(aAckPDU), mErrorCode(aErrorCode) {
  DEBUG("init nsSendSmsResult");
}

NS_IMETHODIMP nsSendSmsResult::GetMessageRef(int32_t* aMessageRef) {
  *aMessageRef = mMessageRef;
  return NS_OK;
}

NS_IMETHODIMP nsSendSmsResult::GetAckPDU(nsAString& aAckPDU) {
  aAckPDU = mAckPDU;
  return NS_OK;
}

NS_IMETHODIMP nsSendSmsResult::GetErrorCode(int32_t* aErrorCode) {
  *aErrorCode = mErrorCode;
  return NS_OK;
}

NS_IMPL_ISUPPORTS(nsSendSmsResult, nsISendSmsResult)

/*============================================================================
 *======================Implementation of Class nsRilResult
 *=====================
 *============================================================================*/
/**
 * Constructor for a nsRilResult
 * For those has no parameter notify.
 */
nsRilResult::nsRilResult(const nsAString& aRilMessageType)
    : mRilMessageType(aRilMessageType) {
  DEBUG("init nsRilResult for indication.");
}
nsRilResult::nsRilResult(const nsAString& aRilMessageType,
                         int32_t aRilMessageToken, int32_t aErrorMsg)
    : mRilMessageType(aRilMessageType),
      mRilMessageToken(aRilMessageToken),
      mErrorMsg(aErrorMsg) {
  DEBUG("init nsRilResult for response.");
}

/**
 *
 */
nsRilResult::~nsRilResult() {}

// Helper function

int32_t nsRilResult::convertRadioTechnology(RADIO_1_0::RadioTechnology aRat) {
  switch (aRat) {
    case RADIO_1_0::RadioTechnology::UNKNOWN:
      return nsIRadioTechnologyState::RADIO_CREG_TECH_UNKNOWN;
    case RADIO_1_0::RadioTechnology::GPRS:
      return nsIRadioTechnologyState::RADIO_CREG_TECH_GPRS;
    case RADIO_1_0::RadioTechnology::EDGE:
      return nsIRadioTechnologyState::RADIO_CREG_TECH_EDGE;
    case RADIO_1_0::RadioTechnology::UMTS:
      return nsIRadioTechnologyState::RADIO_CREG_TECH_UMTS;
    case RADIO_1_0::RadioTechnology::IS95A:
      return nsIRadioTechnologyState::RADIO_CREG_TECH_IS95A;
    case RADIO_1_0::RadioTechnology::IS95B:
      return nsIRadioTechnologyState::RADIO_CREG_TECH_IS95B;
    case RADIO_1_0::RadioTechnology::ONE_X_RTT:
      return nsIRadioTechnologyState::RADIO_CREG_TECH_1XRTT;
    case RADIO_1_0::RadioTechnology::EVDO_0:
      return nsIRadioTechnologyState::RADIO_CREG_TECH_EVDO0;
    case RADIO_1_0::RadioTechnology::EVDO_A:
      return nsIRadioTechnologyState::RADIO_CREG_TECH_EVDOA;
    case RADIO_1_0::RadioTechnology::HSDPA:
      return nsIRadioTechnologyState::RADIO_CREG_TECH_HSDPA;
    case RADIO_1_0::RadioTechnology::HSUPA:
      return nsIRadioTechnologyState::RADIO_CREG_TECH_HSUPA;
    case RADIO_1_0::RadioTechnology::HSPA:
      return nsIRadioTechnologyState::RADIO_CREG_TECH_HSPA;
    case RADIO_1_0::RadioTechnology::EVDO_B:
      return nsIRadioTechnologyState::RADIO_CREG_TECH_EVDOB;
    case RADIO_1_0::RadioTechnology::EHRPD:
      return nsIRadioTechnologyState::RADIO_CREG_TECH_EHRPD;
    case RADIO_1_0::RadioTechnology::LTE:
      return nsIRadioTechnologyState::RADIO_CREG_TECH_LTE;
    case RADIO_1_0::RadioTechnology::HSPAP:
      return nsIRadioTechnologyState::RADIO_CREG_TECH_HSPAP;
    case RADIO_1_0::RadioTechnology::GSM:
      return nsIRadioTechnologyState::RADIO_CREG_TECH_GSM;
    case RADIO_1_0::RadioTechnology::TD_SCDMA:
      return nsIRadioTechnologyState::RADIO_CREG_TECH_TD_SCDMA;
    case RADIO_1_0::RadioTechnology::IWLAN:
      return nsIRadioTechnologyState::RADIO_CREG_TECH_IWLAN;
    case RADIO_1_0::RadioTechnology::LTE_CA:
      return nsIRadioTechnologyState::RADIO_CREG_TECH_LTE_CA;
    default:
      return nsIRadioTechnologyState::RADIO_CREG_TECH_UNKNOWN;
  }
}

#if RADIO_HAL >= 14
int32_t nsRilResult::convertRadioTechnology(RADIO_1_4::RadioTechnology aRat) {
  switch (aRat) {
    case RADIO_1_4::RadioTechnology::UNKNOWN:
      return nsIRadioTechnologyState::RADIO_CREG_TECH_UNKNOWN;
    case RADIO_1_4::RadioTechnology::GPRS:
      return nsIRadioTechnologyState::RADIO_CREG_TECH_GPRS;
    case RADIO_1_4::RadioTechnology::EDGE:
      return nsIRadioTechnologyState::RADIO_CREG_TECH_EDGE;
    case RADIO_1_4::RadioTechnology::UMTS:
      return nsIRadioTechnologyState::RADIO_CREG_TECH_UMTS;
    case RADIO_1_4::RadioTechnology::IS95A:
      return nsIRadioTechnologyState::RADIO_CREG_TECH_IS95A;
    case RADIO_1_4::RadioTechnology::IS95B:
      return nsIRadioTechnologyState::RADIO_CREG_TECH_IS95B;
    case RADIO_1_4::RadioTechnology::ONE_X_RTT:
      return nsIRadioTechnologyState::RADIO_CREG_TECH_1XRTT;
    case RADIO_1_4::RadioTechnology::EVDO_0:
      return nsIRadioTechnologyState::RADIO_CREG_TECH_EVDO0;
    case RADIO_1_4::RadioTechnology::EVDO_A:
      return nsIRadioTechnologyState::RADIO_CREG_TECH_EVDOA;
    case RADIO_1_4::RadioTechnology::HSDPA:
      return nsIRadioTechnologyState::RADIO_CREG_TECH_HSDPA;
    case RADIO_1_4::RadioTechnology::HSUPA:
      return nsIRadioTechnologyState::RADIO_CREG_TECH_HSUPA;
    case RADIO_1_4::RadioTechnology::HSPA:
      return nsIRadioTechnologyState::RADIO_CREG_TECH_HSPA;
    case RADIO_1_4::RadioTechnology::EVDO_B:
      return nsIRadioTechnologyState::RADIO_CREG_TECH_EVDOB;
    case RADIO_1_4::RadioTechnology::EHRPD:
      return nsIRadioTechnologyState::RADIO_CREG_TECH_EHRPD;
    case RADIO_1_4::RadioTechnology::LTE:
      return nsIRadioTechnologyState::RADIO_CREG_TECH_LTE;
    case RADIO_1_4::RadioTechnology::HSPAP:
      return nsIRadioTechnologyState::RADIO_CREG_TECH_HSPAP;
    case RADIO_1_4::RadioTechnology::GSM:
      return nsIRadioTechnologyState::RADIO_CREG_TECH_GSM;
    case RADIO_1_4::RadioTechnology::TD_SCDMA:
      return nsIRadioTechnologyState::RADIO_CREG_TECH_TD_SCDMA;
    case RADIO_1_4::RadioTechnology::IWLAN:
      return nsIRadioTechnologyState::RADIO_CREG_TECH_IWLAN;
    case RADIO_1_4::RadioTechnology::LTE_CA:
      return nsIRadioTechnologyState::RADIO_CREG_TECH_LTE_CA;
    // TODO: Add support for RadioTechnology::NR to nsIRadioTechnologyState
    default:
      return nsIRadioTechnologyState::RADIO_CREG_TECH_UNKNOWN;
  }
}

int32_t nsRilResult::convertDataCallFailCause(
    RADIO_1_4::DataCallFailCause aCause) {
#  define TYPE_NAME RADIO_1_4::DataCallFailCause
  switch (aCause) {
    case TYPE_NAME::NONE:
      return nsIDataCallFailCause::DATACALL_FAIL_NONE;
    case TYPE_NAME::OPERATOR_BARRED:
      return nsIDataCallFailCause::DATACALL_FAIL_OPERATOR_BARRED;
    case TYPE_NAME::NAS_SIGNALLING:
      return nsIDataCallFailCause::DATACALL_FAIL_NAS_SIGNALLING;
    case TYPE_NAME::INSUFFICIENT_RESOURCES:
      return nsIDataCallFailCause::DATACALL_FAIL_INSUFFICIENT_RESOURCES;
    case TYPE_NAME::MISSING_UKNOWN_APN:
      return nsIDataCallFailCause::DATACALL_FAIL_MISSING_UKNOWN_APN;
    case TYPE_NAME::UNKNOWN_PDP_ADDRESS_TYPE:
      return nsIDataCallFailCause::DATACALL_FAIL_UNKNOWN_PDP_ADDRESS_TYPE;
    case TYPE_NAME::USER_AUTHENTICATION:
      return nsIDataCallFailCause::DATACALL_FAIL_USER_AUTHENTICATION;
    case TYPE_NAME::ACTIVATION_REJECT_GGSN:
      return nsIDataCallFailCause::DATACALL_FAIL_ACTIVATION_REJECT_GGSN;
    case TYPE_NAME::ACTIVATION_REJECT_UNSPECIFIED:
      return nsIDataCallFailCause::DATACALL_FAIL_ACTIVATION_REJECT_UNSPECIFIED;
    case TYPE_NAME::SERVICE_OPTION_NOT_SUPPORTED:
      return nsIDataCallFailCause::DATACALL_FAIL_SERVICE_OPTION_NOT_SUPPORTED;
    case TYPE_NAME::SERVICE_OPTION_NOT_SUBSCRIBED:
      return nsIDataCallFailCause::DATACALL_FAIL_SERVICE_OPTION_NOT_SUBSCRIBED;
    case TYPE_NAME::SERVICE_OPTION_OUT_OF_ORDER:
      return nsIDataCallFailCause::DATACALL_FAIL_SERVICE_OPTION_OUT_OF_ORDER;
    case TYPE_NAME::NSAPI_IN_USE:
      return nsIDataCallFailCause::DATACALL_FAIL_NSAPI_IN_USE;
    case TYPE_NAME::REGULAR_DEACTIVATION:
      return nsIDataCallFailCause::DATACALL_FAIL_REGULAR_DEACTIVATION;
    case TYPE_NAME::QOS_NOT_ACCEPTED:
      return nsIDataCallFailCause::DATACALL_FAIL_QOS_NOT_ACCEPTED;
    case TYPE_NAME::NETWORK_FAILURE:
      return nsIDataCallFailCause::DATACALL_FAIL_NETWORK_FAILURE;
    case TYPE_NAME::UMTS_REACTIVATION_REQ:
      return nsIDataCallFailCause::DATACALL_FAIL_UMTS_REACTIVATION_REQ;
    case TYPE_NAME::FEATURE_NOT_SUPP:
      return nsIDataCallFailCause::DATACALL_FAIL_FEATURE_NOT_SUPP;
    case TYPE_NAME::TFT_SEMANTIC_ERROR:
      return nsIDataCallFailCause::DATACALL_FAIL_TFT_SEMANTIC_ERROR;
    case TYPE_NAME::TFT_SYTAX_ERROR:
      return nsIDataCallFailCause::DATACALL_FAIL_TFT_SYTAX_ERROR;
    case TYPE_NAME::UNKNOWN_PDP_CONTEXT:
      return nsIDataCallFailCause::DATACALL_FAIL_UNKNOWN_PDP_CONTEXT;
    case TYPE_NAME::FILTER_SEMANTIC_ERROR:
      return nsIDataCallFailCause::DATACALL_FAIL_FILTER_SEMANTIC_ERROR;
    case TYPE_NAME::FILTER_SYTAX_ERROR:
      return nsIDataCallFailCause::DATACALL_FAIL_FILTER_SYTAX_ERROR;
    case TYPE_NAME::PDP_WITHOUT_ACTIVE_TFT:
      return nsIDataCallFailCause::DATACALL_FAIL_PDP_WITHOUT_ACTIVE_TFT;
    case TYPE_NAME::ONLY_IPV4_ALLOWED:
      return nsIDataCallFailCause::DATACALL_FAIL_ONLY_IPV4_ALLOWED;
    case TYPE_NAME::ONLY_IPV6_ALLOWED:
      return nsIDataCallFailCause::DATACALL_FAIL_ONLY_IPV6_ALLOWED;
    case TYPE_NAME::ONLY_SINGLE_BEARER_ALLOWED:
      return nsIDataCallFailCause::DATACALL_FAIL_ONLY_SINGLE_BEARER_ALLOWED;
    case TYPE_NAME::ESM_INFO_NOT_RECEIVED:
      return nsIDataCallFailCause::DATACALL_FAIL_ESM_INFO_NOT_RECEIVED;
    case TYPE_NAME::PDN_CONN_DOES_NOT_EXIST:
      return nsIDataCallFailCause::DATACALL_FAIL_PDN_CONN_DOES_NOT_EXIST;
    case TYPE_NAME::MULTI_CONN_TO_SAME_PDN_NOT_ALLOWED:
      return nsIDataCallFailCause::
          DATACALL_FAIL_MULTI_CONN_TO_SAME_PDN_NOT_ALLOWED;
    case TYPE_NAME::MAX_ACTIVE_PDP_CONTEXT_REACHED:
      return nsIDataCallFailCause::DATACALL_FAIL_MAX_ACTIVE_PDP_CONTEXT_REACHED;
    case TYPE_NAME::UNSUPPORTED_APN_IN_CURRENT_PLMN:
      return nsIDataCallFailCause::
          DATACALL_FAIL_UNSUPPORTED_APN_IN_CURRENT_PLMN;
    case TYPE_NAME::INVALID_TRANSACTION_ID:
      return nsIDataCallFailCause::DATACALL_FAIL_INVALID_TRANSACTION_ID;
    case TYPE_NAME::MESSAGE_INCORRECT_SEMANTIC:
      return nsIDataCallFailCause::DATACALL_FAIL_MESSAGE_INCORRECT_SEMANTIC;
    case TYPE_NAME::INVALID_MANDATORY_INFO:
      return nsIDataCallFailCause::DATACALL_FAIL_INVALID_MANDATORY_INFO;
    case TYPE_NAME::MESSAGE_TYPE_UNSUPPORTED:
      return nsIDataCallFailCause::DATACALL_FAIL_MESSAGE_TYPE_UNSUPPORTED;
    case TYPE_NAME::MSG_TYPE_NONCOMPATIBLE_STATE:
      return nsIDataCallFailCause::DATACALL_FAIL_MSG_TYPE_NONCOMPATIBLE_STATE;
    case TYPE_NAME::UNKNOWN_INFO_ELEMENT:
      return nsIDataCallFailCause::DATACALL_FAIL_UNKNOWN_INFO_ELEMENT;
    case TYPE_NAME::CONDITIONAL_IE_ERROR:
      return nsIDataCallFailCause::DATACALL_FAIL_CONDITIONAL_IE_ERROR;
    case TYPE_NAME::MSG_AND_PROTOCOL_STATE_UNCOMPATIBLE:
      return nsIDataCallFailCause::
          DATACALL_FAIL_MSG_AND_PROTOCOL_STATE_UNCOMPATIBLE;
    case TYPE_NAME::PROTOCOL_ERRORS:
      return nsIDataCallFailCause::DATACALL_FAIL_PROTOCOL_ERRORS;
    case TYPE_NAME::APN_TYPE_CONFLICT:
      return nsIDataCallFailCause::DATACALL_FAIL_APN_TYPE_CONFLICT;
    case TYPE_NAME::INVALID_PCSCF_ADDR:
      return nsIDataCallFailCause::DATACALL_FAIL_INVALID_PCSCF_ADDR;
    case TYPE_NAME::INTERNAL_CALL_PREEMPT_BY_HIGH_PRIO_APN:
      return nsIDataCallFailCause::
          DATACALL_FAIL_INTERNAL_CALL_PREEMPT_BY_HIGH_PRIO_APN;
    case TYPE_NAME::EMM_ACCESS_BARRED:
      return nsIDataCallFailCause::DATACALL_FAIL_EMM_ACCESS_BARRED;
    case TYPE_NAME::EMERGENCY_IFACE_ONLY:
      return nsIDataCallFailCause::DATACALL_FAIL_EMERGENCY_IFACE_ONLY;
    case TYPE_NAME::IFACE_MISMATCH:
      return nsIDataCallFailCause::DATACALL_FAIL_IFACE_MISMATCH;
    case TYPE_NAME::COMPANION_IFACE_IN_USE:
      return nsIDataCallFailCause::DATACALL_FAIL_COMPANION_IFACE_IN_USE;
    case TYPE_NAME::IP_ADDRESS_MISMATCH:
      return nsIDataCallFailCause::DATACALL_FAIL_IP_ADDRESS_MISMATCH;
    case TYPE_NAME::IFACE_AND_POL_FAMILY_MISMATCH:
      return nsIDataCallFailCause::DATACALL_FAIL_IFACE_AND_POL_FAMILY_MISMATCH;
    case TYPE_NAME::EMM_ACCESS_BARRED_INFINITE_RETRY:
      return nsIDataCallFailCause::
          DATACALL_FAIL_EMM_ACCESS_BARRED_INFINITE_RETRY;
    case TYPE_NAME::AUTH_FAILURE_ON_EMERGENCY_CALL:
      return nsIDataCallFailCause::DATACALL_FAIL_AUTH_FAILURE_ON_EMERGENCY_CALL;
    case TYPE_NAME::VOICE_REGISTRATION_FAIL:
      return nsIDataCallFailCause::DATACALL_FAIL_VOICE_REGISTRATION_FAIL;
    case TYPE_NAME::DATA_REGISTRATION_FAIL:
      return nsIDataCallFailCause::DATACALL_FAIL_DATA_REGISTRATION_FAIL;
    case TYPE_NAME::SIGNAL_LOST:
      return nsIDataCallFailCause::DATACALL_FAIL_SIGNAL_LOST;
    case TYPE_NAME::PREF_RADIO_TECH_CHANGED:
      return nsIDataCallFailCause::DATACALL_FAIL_PREF_RADIO_TECH_CHANGED;
    case TYPE_NAME::RADIO_POWER_OFF:
      return nsIDataCallFailCause::DATACALL_FAIL_RADIO_POWER_OFF;
    case TYPE_NAME::TETHERED_CALL_ACTIVE:
      return nsIDataCallFailCause::DATACALL_FAIL_TETHERED_CALL_ACTIVE;
    case TYPE_NAME::ERROR_UNSPECIFIED:
      return nsIDataCallFailCause::DATACALL_FAIL_ERROR_UNSPECIFIED;
    default:
      return nsIDataCallFailCause::DATACALL_FAIL_ERROR_UNSPECIFIED;
  }
}

#endif

int32_t nsRilResult::convertDataCallFailCause(
    RADIO_1_0::DataCallFailCause aCause) {
#define TYPE_NAME RADIO_1_0::DataCallFailCause
  switch (aCause) {
    case TYPE_NAME::NONE:
      return nsIDataCallFailCause::DATACALL_FAIL_NONE;
    case TYPE_NAME::OPERATOR_BARRED:
      return nsIDataCallFailCause::DATACALL_FAIL_OPERATOR_BARRED;
    case TYPE_NAME::NAS_SIGNALLING:
      return nsIDataCallFailCause::DATACALL_FAIL_NAS_SIGNALLING;
    case TYPE_NAME::INSUFFICIENT_RESOURCES:
      return nsIDataCallFailCause::DATACALL_FAIL_INSUFFICIENT_RESOURCES;
    case TYPE_NAME::MISSING_UKNOWN_APN:
      return nsIDataCallFailCause::DATACALL_FAIL_MISSING_UKNOWN_APN;
    case TYPE_NAME::UNKNOWN_PDP_ADDRESS_TYPE:
      return nsIDataCallFailCause::DATACALL_FAIL_UNKNOWN_PDP_ADDRESS_TYPE;
    case TYPE_NAME::USER_AUTHENTICATION:
      return nsIDataCallFailCause::DATACALL_FAIL_USER_AUTHENTICATION;
    case TYPE_NAME::ACTIVATION_REJECT_GGSN:
      return nsIDataCallFailCause::DATACALL_FAIL_ACTIVATION_REJECT_GGSN;
    case TYPE_NAME::ACTIVATION_REJECT_UNSPECIFIED:
      return nsIDataCallFailCause::DATACALL_FAIL_ACTIVATION_REJECT_UNSPECIFIED;
    case TYPE_NAME::SERVICE_OPTION_NOT_SUPPORTED:
      return nsIDataCallFailCause::DATACALL_FAIL_SERVICE_OPTION_NOT_SUPPORTED;
    case TYPE_NAME::SERVICE_OPTION_NOT_SUBSCRIBED:
      return nsIDataCallFailCause::DATACALL_FAIL_SERVICE_OPTION_NOT_SUBSCRIBED;
    case TYPE_NAME::SERVICE_OPTION_OUT_OF_ORDER:
      return nsIDataCallFailCause::DATACALL_FAIL_SERVICE_OPTION_OUT_OF_ORDER;
    case TYPE_NAME::NSAPI_IN_USE:
      return nsIDataCallFailCause::DATACALL_FAIL_NSAPI_IN_USE;
    case TYPE_NAME::REGULAR_DEACTIVATION:
      return nsIDataCallFailCause::DATACALL_FAIL_REGULAR_DEACTIVATION;
    case TYPE_NAME::QOS_NOT_ACCEPTED:
      return nsIDataCallFailCause::DATACALL_FAIL_QOS_NOT_ACCEPTED;
    case TYPE_NAME::NETWORK_FAILURE:
      return nsIDataCallFailCause::DATACALL_FAIL_NETWORK_FAILURE;
    case TYPE_NAME::UMTS_REACTIVATION_REQ:
      return nsIDataCallFailCause::DATACALL_FAIL_UMTS_REACTIVATION_REQ;
    case TYPE_NAME::FEATURE_NOT_SUPP:
      return nsIDataCallFailCause::DATACALL_FAIL_FEATURE_NOT_SUPP;
    case TYPE_NAME::TFT_SEMANTIC_ERROR:
      return nsIDataCallFailCause::DATACALL_FAIL_TFT_SEMANTIC_ERROR;
    case TYPE_NAME::TFT_SYTAX_ERROR:
      return nsIDataCallFailCause::DATACALL_FAIL_TFT_SYTAX_ERROR;
    case TYPE_NAME::UNKNOWN_PDP_CONTEXT:
      return nsIDataCallFailCause::DATACALL_FAIL_UNKNOWN_PDP_CONTEXT;
    case TYPE_NAME::FILTER_SEMANTIC_ERROR:
      return nsIDataCallFailCause::DATACALL_FAIL_FILTER_SEMANTIC_ERROR;
    case TYPE_NAME::FILTER_SYTAX_ERROR:
      return nsIDataCallFailCause::DATACALL_FAIL_FILTER_SYTAX_ERROR;
    case TYPE_NAME::PDP_WITHOUT_ACTIVE_TFT:
      return nsIDataCallFailCause::DATACALL_FAIL_PDP_WITHOUT_ACTIVE_TFT;
    case TYPE_NAME::ONLY_IPV4_ALLOWED:
      return nsIDataCallFailCause::DATACALL_FAIL_ONLY_IPV4_ALLOWED;
    case TYPE_NAME::ONLY_IPV6_ALLOWED:
      return nsIDataCallFailCause::DATACALL_FAIL_ONLY_IPV6_ALLOWED;
    case TYPE_NAME::ONLY_SINGLE_BEARER_ALLOWED:
      return nsIDataCallFailCause::DATACALL_FAIL_ONLY_SINGLE_BEARER_ALLOWED;
    case TYPE_NAME::ESM_INFO_NOT_RECEIVED:
      return nsIDataCallFailCause::DATACALL_FAIL_ESM_INFO_NOT_RECEIVED;
    case TYPE_NAME::PDN_CONN_DOES_NOT_EXIST:
      return nsIDataCallFailCause::DATACALL_FAIL_PDN_CONN_DOES_NOT_EXIST;
    case TYPE_NAME::MULTI_CONN_TO_SAME_PDN_NOT_ALLOWED:
      return nsIDataCallFailCause::
          DATACALL_FAIL_MULTI_CONN_TO_SAME_PDN_NOT_ALLOWED;
    case TYPE_NAME::MAX_ACTIVE_PDP_CONTEXT_REACHED:
      return nsIDataCallFailCause::DATACALL_FAIL_MAX_ACTIVE_PDP_CONTEXT_REACHED;
    case TYPE_NAME::UNSUPPORTED_APN_IN_CURRENT_PLMN:
      return nsIDataCallFailCause::
          DATACALL_FAIL_UNSUPPORTED_APN_IN_CURRENT_PLMN;
    case TYPE_NAME::INVALID_TRANSACTION_ID:
      return nsIDataCallFailCause::DATACALL_FAIL_INVALID_TRANSACTION_ID;
    case TYPE_NAME::MESSAGE_INCORRECT_SEMANTIC:
      return nsIDataCallFailCause::DATACALL_FAIL_MESSAGE_INCORRECT_SEMANTIC;
    case TYPE_NAME::INVALID_MANDATORY_INFO:
      return nsIDataCallFailCause::DATACALL_FAIL_INVALID_MANDATORY_INFO;
    case TYPE_NAME::MESSAGE_TYPE_UNSUPPORTED:
      return nsIDataCallFailCause::DATACALL_FAIL_MESSAGE_TYPE_UNSUPPORTED;
    case TYPE_NAME::MSG_TYPE_NONCOMPATIBLE_STATE:
      return nsIDataCallFailCause::DATACALL_FAIL_MSG_TYPE_NONCOMPATIBLE_STATE;
    case TYPE_NAME::UNKNOWN_INFO_ELEMENT:
      return nsIDataCallFailCause::DATACALL_FAIL_UNKNOWN_INFO_ELEMENT;
    case TYPE_NAME::CONDITIONAL_IE_ERROR:
      return nsIDataCallFailCause::DATACALL_FAIL_CONDITIONAL_IE_ERROR;
    case TYPE_NAME::MSG_AND_PROTOCOL_STATE_UNCOMPATIBLE:
      return nsIDataCallFailCause::
          DATACALL_FAIL_MSG_AND_PROTOCOL_STATE_UNCOMPATIBLE;
    case TYPE_NAME::PROTOCOL_ERRORS:
      return nsIDataCallFailCause::DATACALL_FAIL_PROTOCOL_ERRORS;
    case TYPE_NAME::APN_TYPE_CONFLICT:
      return nsIDataCallFailCause::DATACALL_FAIL_APN_TYPE_CONFLICT;
    case TYPE_NAME::INVALID_PCSCF_ADDR:
      return nsIDataCallFailCause::DATACALL_FAIL_INVALID_PCSCF_ADDR;
    case TYPE_NAME::INTERNAL_CALL_PREEMPT_BY_HIGH_PRIO_APN:
      return nsIDataCallFailCause::
          DATACALL_FAIL_INTERNAL_CALL_PREEMPT_BY_HIGH_PRIO_APN;
    case TYPE_NAME::EMM_ACCESS_BARRED:
      return nsIDataCallFailCause::DATACALL_FAIL_EMM_ACCESS_BARRED;
    case TYPE_NAME::EMERGENCY_IFACE_ONLY:
      return nsIDataCallFailCause::DATACALL_FAIL_EMERGENCY_IFACE_ONLY;
    case TYPE_NAME::IFACE_MISMATCH:
      return nsIDataCallFailCause::DATACALL_FAIL_IFACE_MISMATCH;
    case TYPE_NAME::COMPANION_IFACE_IN_USE:
      return nsIDataCallFailCause::DATACALL_FAIL_COMPANION_IFACE_IN_USE;
    case TYPE_NAME::IP_ADDRESS_MISMATCH:
      return nsIDataCallFailCause::DATACALL_FAIL_IP_ADDRESS_MISMATCH;
    case TYPE_NAME::IFACE_AND_POL_FAMILY_MISMATCH:
      return nsIDataCallFailCause::DATACALL_FAIL_IFACE_AND_POL_FAMILY_MISMATCH;
    case TYPE_NAME::EMM_ACCESS_BARRED_INFINITE_RETRY:
      return nsIDataCallFailCause::
          DATACALL_FAIL_EMM_ACCESS_BARRED_INFINITE_RETRY;
    case TYPE_NAME::AUTH_FAILURE_ON_EMERGENCY_CALL:
      return nsIDataCallFailCause::DATACALL_FAIL_AUTH_FAILURE_ON_EMERGENCY_CALL;
    case TYPE_NAME::VOICE_REGISTRATION_FAIL:
      return nsIDataCallFailCause::DATACALL_FAIL_VOICE_REGISTRATION_FAIL;
    case TYPE_NAME::DATA_REGISTRATION_FAIL:
      return nsIDataCallFailCause::DATACALL_FAIL_DATA_REGISTRATION_FAIL;
    case TYPE_NAME::SIGNAL_LOST:
      return nsIDataCallFailCause::DATACALL_FAIL_SIGNAL_LOST;
    case TYPE_NAME::PREF_RADIO_TECH_CHANGED:
      return nsIDataCallFailCause::DATACALL_FAIL_PREF_RADIO_TECH_CHANGED;
    case TYPE_NAME::RADIO_POWER_OFF:
      return nsIDataCallFailCause::DATACALL_FAIL_RADIO_POWER_OFF;
    case TYPE_NAME::TETHERED_CALL_ACTIVE:
      return nsIDataCallFailCause::DATACALL_FAIL_TETHERED_CALL_ACTIVE;
    case TYPE_NAME::ERROR_UNSPECIFIED:
      return nsIDataCallFailCause::DATACALL_FAIL_ERROR_UNSPECIFIED;
    default:
      return nsIDataCallFailCause::DATACALL_FAIL_ERROR_UNSPECIFIED;
  }
}

RefPtr<nsCellIdentity> nsRilResult::convertCellIdentity(
    const RADIO_1_0::CellIdentity& aCellIdentity) {
  int32_t cellInfoType = convertCellInfoType(aCellIdentity.cellInfoType);
  if (aCellIdentity.cellInfoType == CellInfoType::GSM) {
    uint32_t numCellIdentityGsm = aCellIdentity.cellIdentityGsm.size();
    if (numCellIdentityGsm > 0) {
      RefPtr<nsCellIdentityGsm> cellIdentityGsm =
          convertCellIdentityGsm(aCellIdentity.cellIdentityGsm[0]);
      RefPtr<nsCellIdentity> cellIdentity =
          new nsCellIdentity(cellInfoType, cellIdentityGsm);
      return cellIdentity;
    }
  } else if (aCellIdentity.cellInfoType == CellInfoType::LTE) {
    uint32_t numCellIdentityLte = aCellIdentity.cellIdentityLte.size();
    if (numCellIdentityLte > 0) {
      RefPtr<nsCellIdentityLte> cellIdentityLte =
          convertCellIdentityLte(aCellIdentity.cellIdentityLte[0]);
      RefPtr<nsCellIdentity> cellIdentity =
          new nsCellIdentity(cellInfoType, cellIdentityLte);
      return cellIdentity;
    }
  } else if (aCellIdentity.cellInfoType == CellInfoType::WCDMA) {
    uint32_t numCellIdentityWcdma = aCellIdentity.cellIdentityWcdma.size();
    if (numCellIdentityWcdma > 0) {
      RefPtr<nsCellIdentityWcdma> cellIdentityWcdma =
          convertCellIdentityWcdma(aCellIdentity.cellIdentityWcdma[0]);
      RefPtr<nsCellIdentity> cellIdentity =
          new nsCellIdentity(cellInfoType, cellIdentityWcdma);
      return cellIdentity;
    }
  } else if (aCellIdentity.cellInfoType == CellInfoType::TD_SCDMA) {
    uint32_t numCellIdentityTdScdma = aCellIdentity.cellIdentityTdscdma.size();
    if (numCellIdentityTdScdma > 0) {
      RefPtr<nsCellIdentityTdScdma> cellIdentityTdScdma =
          convertCellIdentityTdScdma(aCellIdentity.cellIdentityTdscdma[0]);
      RefPtr<nsCellIdentity> cellIdentity =
          new nsCellIdentity(cellInfoType, cellIdentityTdScdma);
      return cellIdentity;
    }
  } else if (aCellIdentity.cellInfoType == CellInfoType::CDMA) {
    uint32_t numCellIdentityCdma = aCellIdentity.cellIdentityCdma.size();
    if (numCellIdentityCdma > 0) {
      RefPtr<nsCellIdentityCdma> cellIdentityCdma =
          convertCellIdentityCdma(aCellIdentity.cellIdentityCdma[0]);
      RefPtr<nsCellIdentity> cellIdentity =
          new nsCellIdentity(cellInfoType, cellIdentityCdma);
      return cellIdentity;
    }
  }
  return nullptr;
}

#if RADIO_HAL >= 14
RefPtr<nsCellIdentity> nsRilResult::convertCellIdentity(
    const RADIO_1_2::CellIdentity& aCellIdentity) {
  int32_t cellInfoType = convertCellInfoType(aCellIdentity.cellInfoType);
  if (aCellIdentity.cellInfoType == CellInfoType::GSM) {
    uint32_t numCellIdentityGsm = aCellIdentity.cellIdentityGsm.size();
    if (numCellIdentityGsm > 0) {
      RefPtr<nsCellIdentityGsm> cellIdentityGsm =
          convertCellIdentityGsm(aCellIdentity.cellIdentityGsm[0]);
      RefPtr<nsCellIdentity> cellIdentity =
          new nsCellIdentity(cellInfoType, cellIdentityGsm);
      return cellIdentity;
    }
  } else if (aCellIdentity.cellInfoType == CellInfoType::LTE) {
    uint32_t numCellIdentityLte = aCellIdentity.cellIdentityLte.size();
    if (numCellIdentityLte > 0) {
      RefPtr<nsCellIdentityLte> cellIdentityLte =
          convertCellIdentityLte(aCellIdentity.cellIdentityLte[0]);
      RefPtr<nsCellIdentity> cellIdentity =
          new nsCellIdentity(cellInfoType, cellIdentityLte);
      return cellIdentity;
    }
  } else if (aCellIdentity.cellInfoType == CellInfoType::WCDMA) {
    uint32_t numCellIdentityWcdma = aCellIdentity.cellIdentityWcdma.size();
    if (numCellIdentityWcdma > 0) {
      RefPtr<nsCellIdentityWcdma> cellIdentityWcdma =
          convertCellIdentityWcdma(aCellIdentity.cellIdentityWcdma[0]);
      RefPtr<nsCellIdentity> cellIdentity =
          new nsCellIdentity(cellInfoType, cellIdentityWcdma);
      return cellIdentity;
    }
  } else if (aCellIdentity.cellInfoType == CellInfoType::TD_SCDMA) {
    uint32_t numCellIdentityTdScdma = aCellIdentity.cellIdentityTdscdma.size();
    if (numCellIdentityTdScdma > 0) {
      RefPtr<nsCellIdentityTdScdma> cellIdentityTdScdma =
          convertCellIdentityTdScdma(aCellIdentity.cellIdentityTdscdma[0]);
      RefPtr<nsCellIdentity> cellIdentity =
          new nsCellIdentity(cellInfoType, cellIdentityTdScdma);
      return cellIdentity;
    }
  } else if (aCellIdentity.cellInfoType == CellInfoType::CDMA) {
    uint32_t numCellIdentityCdma = aCellIdentity.cellIdentityCdma.size();
    if (numCellIdentityCdma > 0) {
      RefPtr<nsCellIdentityCdma> cellIdentityCdma =
          convertCellIdentityCdma(aCellIdentity.cellIdentityCdma[0]);
      RefPtr<nsCellIdentity> cellIdentity =
          new nsCellIdentity(cellInfoType, cellIdentityCdma);
      return cellIdentity;
    }
  }
  return nullptr;
}

RefPtr<nsCellIdentityGsm> nsRilResult::convertCellIdentityGsm(
    const RADIO_1_2::CellIdentityGsm& aCellIdentityGsm) {
  auto cellIdentity = aCellIdentityGsm.base;
  RefPtr<nsCellIdentityGsm> cellIdentityGsm = new nsCellIdentityGsm(
      NS_ConvertUTF8toUTF16(cellIdentity.mcc.c_str()),
      NS_ConvertUTF8toUTF16(cellIdentity.mnc.c_str()), cellIdentity.lac,
      cellIdentity.cid, cellIdentity.arfcn, cellIdentity.bsic);
  return cellIdentityGsm;
}

RefPtr<nsCellIdentityLte> nsRilResult::convertCellIdentityLte(
    const RADIO_1_2::CellIdentityLte& aCellIdentityLte) {
  auto cellIdentity = aCellIdentityLte.base;
  RefPtr<nsCellIdentityLte> cellIdentityLte = new nsCellIdentityLte(
      NS_ConvertUTF8toUTF16(cellIdentity.mcc.c_str()),
      NS_ConvertUTF8toUTF16(cellIdentity.mnc.c_str()), cellIdentity.ci,
      cellIdentity.pci, cellIdentity.tac, cellIdentity.earfcn);
  return cellIdentityLte;
}

RefPtr<nsCellIdentityWcdma> nsRilResult::convertCellIdentityWcdma(
    const RADIO_1_2::CellIdentityWcdma& aCellIdentityWcdma) {
  auto cellIdentity = aCellIdentityWcdma.base;
  RefPtr<nsCellIdentityWcdma> cellIdentityWcdma = new nsCellIdentityWcdma(
      NS_ConvertUTF8toUTF16(cellIdentity.mcc.c_str()),
      NS_ConvertUTF8toUTF16(cellIdentity.mnc.c_str()), cellIdentity.lac,
      cellIdentity.cid, cellIdentity.psc, cellIdentity.uarfcn);
  return cellIdentityWcdma;
}

RefPtr<nsCellIdentityTdScdma> nsRilResult::convertCellIdentityTdScdma(
    const RADIO_1_2::CellIdentityTdscdma& aCellIdentityTdScdma) {
  auto cellIdentity = aCellIdentityTdScdma.base;
  RefPtr<nsCellIdentityTdScdma> cellIdentityTdScdma = new nsCellIdentityTdScdma(
      NS_ConvertUTF8toUTF16(cellIdentity.mcc.c_str()),
      NS_ConvertUTF8toUTF16(cellIdentity.mnc.c_str()), cellIdentity.lac,
      cellIdentity.cid, cellIdentity.cpid);
  return cellIdentityTdScdma;
}

RefPtr<nsCellIdentityCdma> nsRilResult::convertCellIdentityCdma(
    const RADIO_1_2::CellIdentityCdma& aCellIdentityCdma) {
  auto cellIdentity = aCellIdentityCdma.base;
  RefPtr<nsCellIdentityCdma> cellIdentityCdma = new nsCellIdentityCdma(
      cellIdentity.networkId, cellIdentity.systemId, cellIdentity.baseStationId,
      cellIdentity.longitude, cellIdentity.latitude);
  return cellIdentityCdma;
}

#endif

RefPtr<nsCellIdentityGsm> nsRilResult::convertCellIdentityGsm(
    const RADIO_1_0::CellIdentityGsm& aCellIdentityGsm) {
  RefPtr<nsCellIdentityGsm> cellIdentityGsm = new nsCellIdentityGsm(
      NS_ConvertUTF8toUTF16(aCellIdentityGsm.mcc.c_str()),
      NS_ConvertUTF8toUTF16(aCellIdentityGsm.mnc.c_str()), aCellIdentityGsm.lac,
      aCellIdentityGsm.cid, aCellIdentityGsm.arfcn, aCellIdentityGsm.bsic);
  return cellIdentityGsm;
}

RefPtr<nsCellIdentityLte> nsRilResult::convertCellIdentityLte(
    const RADIO_1_0::CellIdentityLte& aCellIdentityLte) {
  RefPtr<nsCellIdentityLte> cellIdentityLte = new nsCellIdentityLte(
      NS_ConvertUTF8toUTF16(aCellIdentityLte.mcc.c_str()),
      NS_ConvertUTF8toUTF16(aCellIdentityLte.mnc.c_str()), aCellIdentityLte.ci,
      aCellIdentityLte.pci, aCellIdentityLte.tac, aCellIdentityLte.earfcn);
  return cellIdentityLte;
}

RefPtr<nsCellIdentityWcdma> nsRilResult::convertCellIdentityWcdma(
    const RADIO_1_0::CellIdentityWcdma& aCellIdentityWcdma) {
  RefPtr<nsCellIdentityWcdma> cellIdentityWcdma = new nsCellIdentityWcdma(
      NS_ConvertUTF8toUTF16(aCellIdentityWcdma.mcc.c_str()),
      NS_ConvertUTF8toUTF16(aCellIdentityWcdma.mnc.c_str()),
      aCellIdentityWcdma.lac, aCellIdentityWcdma.cid, aCellIdentityWcdma.psc,
      aCellIdentityWcdma.uarfcn);
  return cellIdentityWcdma;
}

RefPtr<nsCellIdentityTdScdma> nsRilResult::convertCellIdentityTdScdma(
    const RADIO_1_0::CellIdentityTdscdma& aCellIdentityTdScdma) {
  RefPtr<nsCellIdentityTdScdma> cellIdentityTdScdma = new nsCellIdentityTdScdma(
      NS_ConvertUTF8toUTF16(aCellIdentityTdScdma.mcc.c_str()),
      NS_ConvertUTF8toUTF16(aCellIdentityTdScdma.mnc.c_str()),
      aCellIdentityTdScdma.lac, aCellIdentityTdScdma.cid,
      aCellIdentityTdScdma.cpid);
  return cellIdentityTdScdma;
}

RefPtr<nsCellIdentityCdma> nsRilResult::convertCellIdentityCdma(
    const RADIO_1_0::CellIdentityCdma& aCellIdentityCdma) {
  RefPtr<nsCellIdentityCdma> cellIdentityCdma = new nsCellIdentityCdma(
      aCellIdentityCdma.networkId, aCellIdentityCdma.systemId,
      aCellIdentityCdma.baseStationId, aCellIdentityCdma.longitude,
      aCellIdentityCdma.latitude);
  return cellIdentityCdma;
}

#if RADIO_HAL >= 14
RefPtr<nsSignalStrength> nsRilResult::convertSignalStrength(
    const RADIO_1_4::SignalStrength& aSignalStrength) {
  RefPtr<nsGsmSignalStrength> gsmSignalStrength =
      convertGsmSignalStrength(aSignalStrength.gsm);
  RefPtr<nsCdmaSignalStrength> cdmaSignalStrength =
      convertCdmaSignalStrength(aSignalStrength.cdma);
  RefPtr<nsEvdoSignalStrength> evdoSignalStrength =
      convertEvdoSignalStrength(aSignalStrength.evdo);
  RefPtr<nsLteSignalStrength> lteSignalStrength =
      convertLteSignalStrength(aSignalStrength.lte);
  RefPtr<nsTdScdmaSignalStrength> tdscdmaSignalStrength =
      convertTdScdmaSignalStrength(aSignalStrength.tdscdma);
  RefPtr<nsSignalStrength> signalStrength = new nsSignalStrength(
      gsmSignalStrength, cdmaSignalStrength, evdoSignalStrength,
      lteSignalStrength, tdscdmaSignalStrength);

  return signalStrength;
}
#endif

RefPtr<nsSignalStrength> nsRilResult::convertSignalStrength(
    const RADIO_1_0::SignalStrength& aSignalStrength) {
  RefPtr<nsGsmSignalStrength> gsmSignalStrength =
      convertGsmSignalStrength(aSignalStrength.gw);
  RefPtr<nsCdmaSignalStrength> cdmaSignalStrength =
      convertCdmaSignalStrength(aSignalStrength.cdma);
  RefPtr<nsEvdoSignalStrength> evdoSignalStrength =
      convertEvdoSignalStrength(aSignalStrength.evdo);
  RefPtr<nsLteSignalStrength> lteSignalStrength =
      convertLteSignalStrength(aSignalStrength.lte);
  RefPtr<nsTdScdmaSignalStrength> tdscdmaSignalStrength =
      convertTdScdmaSignalStrength(aSignalStrength.tdScdma);
  RefPtr<nsSignalStrength> signalStrength = new nsSignalStrength(
      gsmSignalStrength, cdmaSignalStrength, evdoSignalStrength,
      lteSignalStrength, tdscdmaSignalStrength);

  return signalStrength;
}

RefPtr<nsGsmSignalStrength> nsRilResult::convertGsmSignalStrength(
    const GsmSignalStrength& aGsmSignalStrength) {
  RefPtr<nsGsmSignalStrength> gsmSignalStrength = new nsGsmSignalStrength(
      aGsmSignalStrength.signalStrength, aGsmSignalStrength.bitErrorRate,
      aGsmSignalStrength.timingAdvance);

  return gsmSignalStrength;
}

RefPtr<nsWcdmaSignalStrength> nsRilResult::convertWcdmaSignalStrength(
    const RADIO_1_0::WcdmaSignalStrength& aWcdmaSignalStrength) {
  RefPtr<nsWcdmaSignalStrength> wcdmaSignalStrength = new nsWcdmaSignalStrength(
      aWcdmaSignalStrength.signalStrength, aWcdmaSignalStrength.bitErrorRate);
  return wcdmaSignalStrength;
}

RefPtr<nsCdmaSignalStrength> nsRilResult::convertCdmaSignalStrength(
    const CdmaSignalStrength& aCdmaSignalStrength) {
  RefPtr<nsCdmaSignalStrength> cdmaSignalStrength = new nsCdmaSignalStrength(
      aCdmaSignalStrength.dbm, aCdmaSignalStrength.ecio);

  return cdmaSignalStrength;
}
RefPtr<nsEvdoSignalStrength> nsRilResult::convertEvdoSignalStrength(
    const EvdoSignalStrength& aEvdoSignalStrength) {
  RefPtr<nsEvdoSignalStrength> evdoSignalStrength = new nsEvdoSignalStrength(
      aEvdoSignalStrength.dbm, aEvdoSignalStrength.ecio,
      aEvdoSignalStrength.signalNoiseRatio);

  return evdoSignalStrength;
}
RefPtr<nsLteSignalStrength> nsRilResult::convertLteSignalStrength(
    const LteSignalStrength& aLteSignalStrength) {
  RefPtr<nsLteSignalStrength> lteSignalStrength = new nsLteSignalStrength(
      aLteSignalStrength.signalStrength, aLteSignalStrength.rsrp,
      aLteSignalStrength.rsrq, aLteSignalStrength.rssnr, aLteSignalStrength.cqi,
      aLteSignalStrength.timingAdvance);

  return lteSignalStrength;
}

#if RADIO_HAL >= 14
RefPtr<nsWcdmaSignalStrength> nsRilResult::convertWcdmaSignalStrength(
    const RADIO_1_2::WcdmaSignalStrength& aWcdmaSignalStrength) {
  RefPtr<nsWcdmaSignalStrength> wcdmaSignalStrength =
      new nsWcdmaSignalStrength(aWcdmaSignalStrength.base.signalStrength,
                                aWcdmaSignalStrength.base.bitErrorRate);
  return wcdmaSignalStrength;
}

RefPtr<nsTdScdmaSignalStrength> nsRilResult::convertTdScdmaSignalStrength(
    const RADIO_1_2::TdscdmaSignalStrength& aTdScdmaSignalStrength) {
  RefPtr<nsTdScdmaSignalStrength> tdscdmaSignalStrength =
      new nsTdScdmaSignalStrength(aTdScdmaSignalStrength.rscp);

  return tdscdmaSignalStrength;
}
#endif

RefPtr<nsTdScdmaSignalStrength> nsRilResult::convertTdScdmaSignalStrength(
    const RADIO_1_0::TdScdmaSignalStrength& aTdScdmaSignalStrength) {
  RefPtr<nsTdScdmaSignalStrength> tdscdmaSignalStrength =
      new nsTdScdmaSignalStrength(aTdScdmaSignalStrength.rscp);

  return tdscdmaSignalStrength;
}

#if RADIO_HAL >= 14
int32_t convertCellInfoType_1_4(
    RADIO_1_4::CellInfo::Info::hidl_discriminator type) {
#  define DISCRIM RADIO_1_4::CellInfo::Info::hidl_discriminator

  switch (type) {
    case DISCRIM::gsm:
      return nsICellInfoType::RADIO_CELL_INFO_TYPE_GSM;
    case DISCRIM::cdma:
      return nsICellInfoType::RADIO_CELL_INFO_TYPE_CDMA;
    case DISCRIM::wcdma:
      return nsICellInfoType::RADIO_CELL_INFO_TYPE_WCDMA;
    case DISCRIM::tdscdma:
      return nsICellInfoType::RADIO_CELL_INFO_TYPE_TD_SCDMA;
    case DISCRIM::lte:
      return nsICellInfoType::RADIO_CELL_INFO_TYPE_LTE;
    case DISCRIM::nr:  // TODO: add new type.
    default:
      return nsICellInfoType::RADIO_CELL_INFO_TYPE_UNKNOW;
  }
}

RefPtr<nsRilCellInfo> nsRilResult::convertRilCellInfo(
    const RADIO_1_4::CellInfo& aCellInfo) {
  int32_t cellInfoType =
      convertCellInfoType_1_4(aCellInfo.info.getDiscriminator());

  bool registered = aCellInfo.isRegistered;

  // TODO: is there a way to get that info in 1.4 ??
  int32_t timeStampType = 0;
  int32_t timeStamp = 0;

  if (cellInfoType == nsICellInfoType::RADIO_CELL_INFO_TYPE_GSM) {
    RefPtr<nsCellInfoGsm> cellInfoGsm =
        convertCellInfoGsm(aCellInfo.info.gsm());
    RefPtr<nsRilCellInfo> cellInfo = new nsRilCellInfo(
        cellInfoType, registered, timeStampType, timeStamp, cellInfoGsm);
    return cellInfo;
  } else if (cellInfoType == nsICellInfoType::RADIO_CELL_INFO_TYPE_LTE) {
    RefPtr<nsCellInfoLte> cellInfoLte =
        convertCellInfoLte(aCellInfo.info.lte());
    RefPtr<nsRilCellInfo> cellInfo = new nsRilCellInfo(
        cellInfoType, registered, timeStampType, timeStamp, cellInfoLte);
    return cellInfo;
  } else if (cellInfoType == nsICellInfoType::RADIO_CELL_INFO_TYPE_WCDMA) {
    RefPtr<nsCellInfoWcdma> cellInfoWcdma =
        convertCellInfoWcdma(aCellInfo.info.wcdma());
    RefPtr<nsRilCellInfo> cellInfo = new nsRilCellInfo(
        cellInfoType, registered, timeStampType, timeStamp, cellInfoWcdma);
    return cellInfo;
  } else if (cellInfoType == nsICellInfoType::RADIO_CELL_INFO_TYPE_TD_SCDMA) {
    RefPtr<nsCellInfoTdScdma> cellInfoTdScdma =
        convertCellInfoTdScdma(aCellInfo.info.tdscdma());
    RefPtr<nsRilCellInfo> cellInfo = new nsRilCellInfo(
        cellInfoType, registered, timeStampType, timeStamp, cellInfoTdScdma);
    return cellInfo;
  } else if (cellInfoType == nsICellInfoType::RADIO_CELL_INFO_TYPE_CDMA) {
    RefPtr<nsCellInfoCdma> cellInfoCdma =
        convertCellInfoCdma(aCellInfo.info.cdma());
    RefPtr<nsRilCellInfo> cellInfo = new nsRilCellInfo(
        cellInfoType, registered, timeStampType, timeStamp, cellInfoCdma);
    return cellInfo;
  } else {
    return nullptr;
  }
}
#endif

RefPtr<nsRilCellInfo> nsRilResult::convertRilCellInfo(
    const RADIO_1_0::CellInfo& aCellInfo) {
  int32_t cellInfoType = convertCellInfoType(aCellInfo.cellInfoType);
  bool registered = aCellInfo.registered;
  int32_t timeStampType = convertTimeStampType(aCellInfo.timeStampType);
  int32_t timeStamp = (int32_t)aCellInfo.timeStamp;

  if (aCellInfo.cellInfoType == CellInfoType::GSM) {
    RefPtr<nsCellInfoGsm> cellInfoGsm = convertCellInfoGsm(aCellInfo.gsm[0]);
    RefPtr<nsRilCellInfo> cellInfo = new nsRilCellInfo(
        cellInfoType, registered, timeStampType, timeStamp, cellInfoGsm);
    return cellInfo;
  } else if (aCellInfo.cellInfoType == CellInfoType::LTE) {
    RefPtr<nsCellInfoLte> cellInfoLte = convertCellInfoLte(aCellInfo.lte[0]);
    RefPtr<nsRilCellInfo> cellInfo = new nsRilCellInfo(
        cellInfoType, registered, timeStampType, timeStamp, cellInfoLte);
    return cellInfo;
  } else if (aCellInfo.cellInfoType == CellInfoType::WCDMA) {
    RefPtr<nsCellInfoWcdma> cellInfoWcdma =
        convertCellInfoWcdma(aCellInfo.wcdma[0]);
    RefPtr<nsRilCellInfo> cellInfo = new nsRilCellInfo(
        cellInfoType, registered, timeStampType, timeStamp, cellInfoWcdma);
    return cellInfo;
  } else if (aCellInfo.cellInfoType == CellInfoType::TD_SCDMA) {
    RefPtr<nsCellInfoTdScdma> cellInfoTdScdma =
        convertCellInfoTdScdma(aCellInfo.tdscdma[0]);
    RefPtr<nsRilCellInfo> cellInfo = new nsRilCellInfo(
        cellInfoType, registered, timeStampType, timeStamp, cellInfoTdScdma);
    return cellInfo;
  } else if (aCellInfo.cellInfoType == CellInfoType::CDMA) {
    RefPtr<nsCellInfoCdma> cellInfoCdma =
        convertCellInfoCdma(aCellInfo.cdma[0]);
    RefPtr<nsRilCellInfo> cellInfo = new nsRilCellInfo(
        cellInfoType, registered, timeStampType, timeStamp, cellInfoCdma);
    return cellInfo;
  } else {
    return nullptr;
  }
}

RefPtr<nsCellInfoGsm> nsRilResult::convertCellInfoGsm(
    const RADIO_1_0::CellInfoGsm& aCellInfoGsm) {
  RefPtr<nsCellIdentityGsm> cellIdentityGsm =
      convertCellIdentityGsm(aCellInfoGsm.cellIdentityGsm);
  RefPtr<nsGsmSignalStrength> gsmSignalStrength =
      convertGsmSignalStrength(aCellInfoGsm.signalStrengthGsm);
  RefPtr<nsCellInfoGsm> cellInfoGsm =
      new nsCellInfoGsm(cellIdentityGsm, gsmSignalStrength);

  return cellInfoGsm;
}

RefPtr<nsCellInfoCdma> nsRilResult::convertCellInfoCdma(
    const RADIO_1_0::CellInfoCdma& aCellInfoCdma) {
  RefPtr<nsCellIdentityCdma> cellIdentityCdma =
      convertCellIdentityCdma(aCellInfoCdma.cellIdentityCdma);
  RefPtr<nsCdmaSignalStrength> cdmaSignalStrength =
      convertCdmaSignalStrength(aCellInfoCdma.signalStrengthCdma);
  RefPtr<nsEvdoSignalStrength> evdoSignalStrength =
      convertEvdoSignalStrength(aCellInfoCdma.signalStrengthEvdo);
  RefPtr<nsCellInfoCdma> cellInfoCdma = new nsCellInfoCdma(
      cellIdentityCdma, cdmaSignalStrength, evdoSignalStrength);

  return cellInfoCdma;
}

RefPtr<nsCellInfoWcdma> nsRilResult::convertCellInfoWcdma(
    const RADIO_1_0::CellInfoWcdma& aCellInfoWcdma) {
  RefPtr<nsCellIdentityWcdma> cellIdentityWcdma =
      convertCellIdentityWcdma(aCellInfoWcdma.cellIdentityWcdma);
  RefPtr<nsWcdmaSignalStrength> wcdmaSignalStrength =
      convertWcdmaSignalStrength(aCellInfoWcdma.signalStrengthWcdma);
  RefPtr<nsCellInfoWcdma> cellInfoWcdma =
      new nsCellInfoWcdma(cellIdentityWcdma, wcdmaSignalStrength);

  return cellInfoWcdma;
}

RefPtr<nsCellInfoLte> nsRilResult::convertCellInfoLte(
    const RADIO_1_0::CellInfoLte& aCellInfoLte) {
  RefPtr<nsCellIdentityLte> cellIdentityLte =
      convertCellIdentityLte(aCellInfoLte.cellIdentityLte);
  RefPtr<nsLteSignalStrength> lteSignalStrength =
      convertLteSignalStrength(aCellInfoLte.signalStrengthLte);
  RefPtr<nsCellInfoLte> cellInfoLte =
      new nsCellInfoLte(cellIdentityLte, lteSignalStrength);

  return cellInfoLte;
}

RefPtr<nsCellInfoTdScdma> nsRilResult::convertCellInfoTdScdma(
    const RADIO_1_0::CellInfoTdscdma& aCellInfoTdscdma) {
  RefPtr<nsCellIdentityTdScdma> cellIdentityTdScdma =
      convertCellIdentityTdScdma(aCellInfoTdscdma.cellIdentityTdscdma);
  RefPtr<nsTdScdmaSignalStrength> tdscdmaSignalStrength =
      convertTdScdmaSignalStrength(aCellInfoTdscdma.signalStrengthTdscdma);
  RefPtr<nsCellInfoTdScdma> cellInfoTdScdma =
      new nsCellInfoTdScdma(cellIdentityTdScdma, tdscdmaSignalStrength);

  return cellInfoTdScdma;
}

#if RADIO_HAL >= 14
RefPtr<nsCellInfoGsm> nsRilResult::convertCellInfoGsm(
    const RADIO_1_2::CellInfoGsm& aCellInfoGsm) {
  RefPtr<nsCellIdentityGsm> cellIdentityGsm =
      convertCellIdentityGsm(aCellInfoGsm.cellIdentityGsm);
  RefPtr<nsGsmSignalStrength> gsmSignalStrength =
      convertGsmSignalStrength(aCellInfoGsm.signalStrengthGsm);
  RefPtr<nsCellInfoGsm> cellInfoGsm =
      new nsCellInfoGsm(cellIdentityGsm, gsmSignalStrength);

  return cellInfoGsm;
}

RefPtr<nsCellInfoCdma> nsRilResult::convertCellInfoCdma(
    const RADIO_1_2::CellInfoCdma& aCellInfoCdma) {
  RefPtr<nsCellIdentityCdma> cellIdentityCdma =
      convertCellIdentityCdma(aCellInfoCdma.cellIdentityCdma);
  RefPtr<nsCdmaSignalStrength> cdmaSignalStrength =
      convertCdmaSignalStrength(aCellInfoCdma.signalStrengthCdma);
  RefPtr<nsEvdoSignalStrength> evdoSignalStrength =
      convertEvdoSignalStrength(aCellInfoCdma.signalStrengthEvdo);
  RefPtr<nsCellInfoCdma> cellInfoCdma = new nsCellInfoCdma(
      cellIdentityCdma, cdmaSignalStrength, evdoSignalStrength);

  return cellInfoCdma;
}

RefPtr<nsCellInfoWcdma> nsRilResult::convertCellInfoWcdma(
    const RADIO_1_2::CellInfoWcdma& aCellInfoWcdma) {
  RefPtr<nsCellIdentityWcdma> cellIdentityWcdma =
      convertCellIdentityWcdma(aCellInfoWcdma.cellIdentityWcdma);
  RefPtr<nsWcdmaSignalStrength> wcdmaSignalStrength =
      convertWcdmaSignalStrength(aCellInfoWcdma.signalStrengthWcdma);
  RefPtr<nsCellInfoWcdma> cellInfoWcdma =
      new nsCellInfoWcdma(cellIdentityWcdma, wcdmaSignalStrength);

  return cellInfoWcdma;
}

RefPtr<nsCellInfoLte> nsRilResult::convertCellInfoLte(
    const RADIO_1_4::CellInfoLte& aCellInfoLte) {
  auto cellInfo = aCellInfoLte.base;
  RefPtr<nsCellIdentityLte> cellIdentityLte =
      convertCellIdentityLte(cellInfo.cellIdentityLte);
  RefPtr<nsLteSignalStrength> lteSignalStrength =
      convertLteSignalStrength(cellInfo.signalStrengthLte);
  RefPtr<nsCellInfoLte> cellInfoLte =
      new nsCellInfoLte(cellIdentityLte, lteSignalStrength);

  return cellInfoLte;
}

RefPtr<nsCellInfoTdScdma> nsRilResult::convertCellInfoTdScdma(
    const RADIO_1_2::CellInfoTdscdma& aCellInfoTdscdma) {
  RefPtr<nsCellIdentityTdScdma> cellIdentityTdScdma =
      convertCellIdentityTdScdma(aCellInfoTdscdma.cellIdentityTdscdma);
  RefPtr<nsTdScdmaSignalStrength> tdscdmaSignalStrength =
      convertTdScdmaSignalStrength(aCellInfoTdscdma.signalStrengthTdscdma);
  RefPtr<nsCellInfoTdScdma> cellInfoTdScdma =
      new nsCellInfoTdScdma(cellIdentityTdScdma, tdscdmaSignalStrength);

  return cellInfoTdScdma;
}

nsString convertPdpProtocolType(PdpProtocolType protocol) {
  switch (protocol) {
    case PdpProtocolType::IP:
      return u"IP"_ns;
    case PdpProtocolType::IPV6:
      return u"IPV6"_ns;
    case PdpProtocolType::IPV4V6:
      return u"IPV4V6"_ns;
    case PdpProtocolType::PPP:
      return u"PPP"_ns;
    case PdpProtocolType::NON_IP:
      return u"NON_IP"_ns;
    case PdpProtocolType::UNSTRUCTURED:
      return u"UNSTRUCTURED"_ns;
    case PdpProtocolType::UNKNOWN:
    default:
      return u"UNKNOWN"_ns;
  }
}

nsString joinHidlVec(const hidl_vec<hidl_string>& source) {
  std::string result;
  for (auto iter = source.begin(); iter != source.end(); ++iter) {
    result += *iter;
    if (iter != source.end() - 1) {
      result += " ";
    }
  }
  return NS_ConvertUTF8toUTF16(result.c_str());
}

RefPtr<nsSetupDataCallResult> nsRilResult::convertDcResponse(
    const RADIO_1_4::SetupDataCallResult& aDcResponse) {
  RefPtr<nsSetupDataCallResult> dcResponse = new nsSetupDataCallResult(
      convertDataCallFailCause(aDcResponse.cause),
      aDcResponse.suggestedRetryTime, aDcResponse.cid,
      (int32_t)aDcResponse.active, convertPdpProtocolType(aDcResponse.type),
      NS_ConvertUTF8toUTF16(aDcResponse.ifname.c_str()),
      joinHidlVec(aDcResponse.addresses), joinHidlVec(aDcResponse.dnses),
      joinHidlVec(aDcResponse.gateways), joinHidlVec(aDcResponse.pcscf),
      aDcResponse.mtu);

  return dcResponse;
}
#endif

RefPtr<nsSetupDataCallResult> nsRilResult::convertDcResponse(
    const RADIO_1_0::SetupDataCallResult& aDcResponse) {
  RefPtr<nsSetupDataCallResult> dcResponse = new nsSetupDataCallResult(
      convertDataCallFailCause(aDcResponse.status),
      aDcResponse.suggestedRetryTime, aDcResponse.cid, aDcResponse.active,
      NS_ConvertUTF8toUTF16(aDcResponse.type.c_str()),
      NS_ConvertUTF8toUTF16(aDcResponse.ifname.c_str()),
      NS_ConvertUTF8toUTF16(aDcResponse.addresses.c_str()),
      NS_ConvertUTF8toUTF16(aDcResponse.dnses.c_str()),
      NS_ConvertUTF8toUTF16(aDcResponse.gateways.c_str()),
      NS_ConvertUTF8toUTF16(aDcResponse.pcscf.c_str()), aDcResponse.mtu);

  return dcResponse;
}

int32_t nsRilResult::convertCellInfoType(CellInfoType type) {
  switch (type) {
    case CellInfoType::NONE:
      return nsICellInfoType::RADIO_CELL_INFO_TYPE_UNKNOW;
    case CellInfoType::GSM:
      return nsICellInfoType::RADIO_CELL_INFO_TYPE_GSM;
    case CellInfoType::CDMA:
      return nsICellInfoType::RADIO_CELL_INFO_TYPE_CDMA;
    case CellInfoType::LTE:
      return nsICellInfoType::RADIO_CELL_INFO_TYPE_LTE;
    case CellInfoType::WCDMA:
      return nsICellInfoType::RADIO_CELL_INFO_TYPE_WCDMA;
    case CellInfoType::TD_SCDMA:
      return nsICellInfoType::RADIO_CELL_INFO_TYPE_TD_SCDMA;
    default:
      return nsICellInfoType::RADIO_CELL_INFO_TYPE_UNKNOW;
  }
}

int32_t nsRilResult::convertTimeStampType(TimeStampType type) {
  switch (type) {
    case TimeStampType::UNKNOWN:
      return nsITimeStampType::RADIO_TIME_STAMP_TYPE_UNKNOW;
    case TimeStampType::ANTENNA:
      return nsITimeStampType::RADIO_TIME_STAMP_TYPE_ANTENNA;
    case TimeStampType::MODEM:
      return nsITimeStampType::RADIO_TIME_STAMP_TYPE_MODEM;
    case TimeStampType::OEM_RIL:
      return nsITimeStampType::RADIO_TIME_STAMP_TYPE_OEM_RIL;
    case TimeStampType::JAVA_RIL:
      return nsITimeStampType::RADIO_TIME_STAMP_TYPE_JAVA_RIL;
    default:
      return nsITimeStampType::RADIO_TIME_STAMP_TYPE_UNKNOW;
  }
}

NS_DEFINE_NAMED_CID(RILRESULT_CID);
