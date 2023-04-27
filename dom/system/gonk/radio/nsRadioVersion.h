/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

// Setup the appropriate #define to select the Radio HAL version.
// HAL X.Y -> RADIO_HAL = XY to allow comparisons.
//
// For now we use 1.1 for ANDROID_VERSION < 33 and 1.4 for ANDROID_VERSION >= 33
// We could also do device-specific versioning.

#ifndef nsRadioVersion_H
#define nsRadioVersion_H

#if ANDROID_VERSION < 33
#  define RADIO_HAL 11
#  include "nsRadioTypes1_1.h"
#else
#  define RADIO_HAL 14
#  include "nsRadioTypes1_4.h"
#endif

// Commonly used HAL types.
using ::android::sp;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;

#endif  // nsRadioVersion_H
