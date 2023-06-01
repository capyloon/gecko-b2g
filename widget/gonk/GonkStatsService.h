/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=8 sts=2 et sw=2 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef GONKSTATSERVICE_H
#define GONKSTATSERVICE_H

#include <aidl/android/frameworks/stats/BnStats.h>

namespace stats = aidl::android::frameworks::stats;

class GonkStats : public stats::BnStats {
 public:
  GonkStats();

  ndk::ScopedAStatus reportVendorAtom(
      const stats::VendorAtom& vendorAtom) override;

  static void init();
};

#endif  // GONKSTATSERVICE_H
