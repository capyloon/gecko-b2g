/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=8 sts=2 et sw=2 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "GonkStatsService.h"
#include "nsDebug.h"
#include <android/binder_manager.h>

static std::shared_ptr<GonkStats> gGonkStats = nullptr;

GonkStats::GonkStats() {}

ndk::ScopedAStatus GonkStats::reportVendorAtom(
    const stats::VendorAtom& vendorAtom) {
  return ndk::ScopedAStatus::ok();
}

/* static */
void GonkStats::init() {
  if (gGonkStats) {
    return;
  }

  gGonkStats = ndk::SharedRefBase::make<GonkStats>();
  const std::string serviceName =
      std::string() + stats::IStats::descriptor + "/default";
  binder_status_t status = AServiceManager_addService(
      gGonkStats->asBinder().get(), serviceName.c_str());
  printf_stderr("GonkStats: Adding AIDL service %s : %s", serviceName.c_str(),
                status == STATUS_OK ? "success" : "failure");
}
