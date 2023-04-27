/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=8 sts=2 et sw=2 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef nsRilWorker_H
#define nsRilWorker_H

#include <nsISupportsImpl.h>
#include <nsIRilWorkerService.h>
#include <nsThreadUtils.h>
#include <nsTArray.h>
#include "nsRilIndication.h"
#include "nsRilIndicationResult.h"
#include "nsRilResponse.h"
#include "nsRilResponseResult.h"
#include "nsRilResult.h"

#include "nsRadioVersion.h"

#if RADIO_HAL >= 14
#  include <android/hardware/radio/1.4/IRadio.h>
using RADIO_1_4::IRadio;
#else
#  include <android/hardware/radio/1.1/IRadio.h>
using RADIO_1_1::IRadio;
#endif

using ::android::hardware::hidl_death_recipient;
using ::android::hidl::base::V1_0::IBase;

static bool gRilDebug_isLoggingEnabled = false;

class nsRilWorker;

class nsRilWorker final : public nsIRilWorker {
 public:
  NS_DECL_ISUPPORTS
  NS_DECL_NSIRILWORKER

  explicit nsRilWorker(uint32_t aClientId);

  struct RadioProxyDeathRecipient : public hidl_death_recipient {
    // hidl_death_recipient interface
    virtual void serviceDied(uint64_t cookie,
                             const ::android::wp<IBase>& who) override;
  };

  void GetRadioProxy();
  void processIndication(RadioIndicationType indicationType);
  void processResponse(RadioResponseType responseType);
  void sendAck();
  void sendRilIndicationResult(nsRilIndicationResult* aIndication);
  void sendRilResponseResult(nsRilResponseResult* aResponse);
  void updateDebug();
  nsCOMPtr<nsIRilCallback> mRilCallback;

 private:
  ~nsRilWorker();
  int32_t mClientId;
  sp<IRadio> mRadioProxy;
  sp<IRadioResponse> mRilResponse;
  sp<IRadioIndication> mRilIndication;
  sp<RadioProxyDeathRecipient> mDeathRecipient;
  MvnoType convertToHalMvnoType(const nsAString& mvnoType);
  DataProfileInfo convertToHalDataProfile(nsIDataProfile* profile);
};
#endif
