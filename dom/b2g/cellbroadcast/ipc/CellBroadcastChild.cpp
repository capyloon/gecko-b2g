/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=8 sts=2 et sw=2 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "CellBroadcastChild.h"
#include "mozilla/dom/ContentChild.h"

namespace mozilla {
namespace dom {
namespace cellbroadcast {

NS_IMPL_ISUPPORTS(CellBroadcastChild, nsICellBroadcastService)

CellBroadcastChild::CellBroadcastChild() : mActorDestroyed(false) {
  ContentChild::GetSingleton()->SendPCellBroadcastConstructor(this);
}

CellBroadcastChild::~CellBroadcastChild() {
  if (!mActorDestroyed) {
    Send__delete__(this);
  }

  mListeners.Clear();
}

/*
 * Implementation of nsICellBroadcastService.
 */

NS_IMETHODIMP
CellBroadcastChild::RegisterListener(nsICellBroadcastListener* aListener) {
  MOZ_ASSERT(!mListeners.Contains(aListener));

  NS_ENSURE_TRUE(!mActorDestroyed, NS_ERROR_UNEXPECTED);

  // nsTArray doesn't fail.
  mListeners.AppendElement(aListener);

  return NS_OK;
}

NS_IMETHODIMP
CellBroadcastChild::UnregisterListener(nsICellBroadcastListener* aListener) {
  MOZ_ASSERT(mListeners.Contains(aListener));

  NS_ENSURE_TRUE(!mActorDestroyed, NS_ERROR_UNEXPECTED);

  // We always have the element here, so it can't fail.
  mListeners.RemoveElement(aListener);

  return NS_OK;
}

NS_IMETHODIMP
CellBroadcastChild::SetCBSearchList(uint32_t aClientId, uint32_t aGsmCount,
                                    uint16_t* aGsms, uint32_t aCdmaCount,
                                    uint16_t* aCdmas) {
  return NS_ERROR_NOT_IMPLEMENTED;
}

NS_IMETHODIMP
CellBroadcastChild::SetCBDisabled(uint32_t aClientId, bool aDisabled) {
  return NS_ERROR_NOT_IMPLEMENTED;
}

/*
 * Implementation of PCellBroadcastChild.
 */

mozilla::ipc::IPCResult CellBroadcastChild::RecvNotifyReceivedMessage(
    const uint32_t& aServiceId, const uint32_t& aGsmGeographicalScope,
    const uint16_t& aMessageCode, const uint16_t& aMessageId,
    const nsString& aLanguage, const nsString& aBody,
    const uint32_t& aMessageClass, const uint64_t& aTimestamp,
    const uint32_t& aCdmaServiceCategory, const bool& aHasEtwsInfo,
    const uint32_t& aEtwsWarningType, const bool& aEtwsEmergencyUserAlert,
    const bool& aEtwsPopup, const uint16_t& aUpdateNumber) {
  // UnregisterListener() could be triggered in
  // nsICellBroadcastListener::NotifyMessageReceived().
  // Make a immutable copy for notifying the event.
  nsTArray<nsCOMPtr<nsICellBroadcastListener>> immutableListeners(
      mListeners.Clone());
  for (uint32_t i = 0; i < immutableListeners.Length(); i++) {
    immutableListeners[i]->NotifyMessageReceived(
        aServiceId, aGsmGeographicalScope, aMessageCode, aMessageId, aLanguage,
        aBody, aMessageClass, aTimestamp, aCdmaServiceCategory, aHasEtwsInfo,
        aEtwsWarningType, aEtwsEmergencyUserAlert, aEtwsPopup, aUpdateNumber);
  }

  return IPC_OK();
}

void CellBroadcastChild::ActorDestroy(ActorDestroyReason aWhy) {
  mActorDestroyed = true;
}

}  // namespace cellbroadcast
}  // namespace dom
}  // namespace mozilla
