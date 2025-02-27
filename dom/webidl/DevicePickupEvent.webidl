/* -*- Mode: IDL; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/.
 */

[Pref="device.sensors.devicePickup.enabled", Func="nsGlobalWindowInner::DeviceSensorsEnabled",
 Exposed=Window]
interface DevicePickupEvent : Event
{
  constructor(DOMString type, optional DevicePickupEventInit eventInitDict = {});
};

dictionary DevicePickupEventInit : EventInit
{
};