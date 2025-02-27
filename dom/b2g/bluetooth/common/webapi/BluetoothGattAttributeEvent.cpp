/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim:set ts=2 sw=2 sts=2 et cindent: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "mozilla/dom/bluetooth/BluetoothGattAttributeEvent.h"

#include "js/GCAPI.h"
#include "jsfriendapi.h"
#include "mozilla/dom/BluetoothGattAttributeEventBinding.h"
#include "mozilla/dom/bluetooth/BluetoothGattCharacteristic.h"
#include "mozilla/dom/bluetooth/BluetoothGattDescriptor.h"
#include "mozilla/dom/EventBinding.h"
#include "mozilla/dom/Nullable.h"
#include "mozilla/dom/PrimitiveConversions.h"
#include "mozilla/dom/TypedArray.h"

USING_BLUETOOTH_NAMESPACE

NS_IMPL_CYCLE_COLLECTION_CLASS(BluetoothGattAttributeEvent)

NS_IMPL_ADDREF_INHERITED(BluetoothGattAttributeEvent, Event)
NS_IMPL_RELEASE_INHERITED(BluetoothGattAttributeEvent, Event)

NS_IMPL_CYCLE_COLLECTION_TRAVERSE_BEGIN_INHERITED(BluetoothGattAttributeEvent,
                                                  Event)
  NS_IMPL_CYCLE_COLLECTION_TRAVERSE(mCharacteristic)
  NS_IMPL_CYCLE_COLLECTION_TRAVERSE(mDescriptor)
NS_IMPL_CYCLE_COLLECTION_TRAVERSE_END

NS_IMPL_CYCLE_COLLECTION_TRACE_BEGIN_INHERITED(BluetoothGattAttributeEvent,
                                               Event)
  NS_IMPL_CYCLE_COLLECTION_TRACE_JS_MEMBER_CALLBACK(mValue)
NS_IMPL_CYCLE_COLLECTION_TRACE_END

NS_IMPL_CYCLE_COLLECTION_UNLINK_BEGIN_INHERITED(BluetoothGattAttributeEvent,
                                                Event)
  NS_IMPL_CYCLE_COLLECTION_UNLINK(mCharacteristic)
  NS_IMPL_CYCLE_COLLECTION_UNLINK(mDescriptor)
  tmp->mValue = nullptr;
  mozilla::DropJSObjects(tmp);
NS_IMPL_CYCLE_COLLECTION_UNLINK_END

NS_INTERFACE_MAP_BEGIN_CYCLE_COLLECTION(BluetoothGattAttributeEvent)
NS_INTERFACE_MAP_END_INHERITING(Event)

BluetoothGattAttributeEvent::BluetoothGattAttributeEvent(EventTarget* aOwner)
    : Event(aOwner, nullptr, nullptr) {
  mozilla::HoldJSObjects(this);
}

BluetoothGattAttributeEvent::~BluetoothGattAttributeEvent() {
  mozilla::DropJSObjects(this);
}

JSObject* BluetoothGattAttributeEvent::WrapObjectInternal(
    JSContext* aCx, JS::Handle<JSObject*> aGivenProto) {
  return BluetoothGattAttributeEvent_Binding::Wrap(aCx, this, aGivenProto);
}

already_AddRefed<BluetoothGattAttributeEvent>
BluetoothGattAttributeEvent::Constructor(
    EventTarget* aOwner, const nsAString& aType, const nsAString& aAddress,
    int32_t aRequestId, BluetoothGattCharacteristic* aCharacteristic,
    BluetoothGattDescriptor* aDescriptor, const nsTArray<uint8_t>* aValue,
    bool aNeedResponse, bool aBubbles, bool aCancelable) {
  RefPtr<BluetoothGattAttributeEvent> e =
      new BluetoothGattAttributeEvent(aOwner);
  bool trusted = e->Init(aOwner);

  e->InitEvent(aType, aBubbles, aCancelable);
  e->mAddress = aAddress;
  e->mRequestId = aRequestId;
  e->mCharacteristic = aCharacteristic;
  e->mDescriptor = aDescriptor;
  e->mNeedResponse = aNeedResponse;

  if (aValue) {
    e->mRawValue = aValue->Clone();
  }

  e->SetTrusted(trusted);

  return e.forget();
}

already_AddRefed<BluetoothGattAttributeEvent>
BluetoothGattAttributeEvent::Constructor(
    const GlobalObject& aGlobal, const nsAString& aType,
    const BluetoothGattAttributeEventInit& aEventInitDict) {
  nsCOMPtr<EventTarget> owner = do_QueryInterface(aGlobal.GetAsSupports());

  RefPtr<BluetoothGattAttributeEvent> e = Constructor(
      owner, aType, aEventInitDict.mAddress, aEventInitDict.mRequestId,
      aEventInitDict.mCharacteristic, aEventInitDict.mDescriptor, nullptr,
      aEventInitDict.mNeedResponse, aEventInitDict.mBubbles,
      aEventInitDict.mCancelable);

  if (!aEventInitDict.mValue.IsNull()) {
    const auto& value = aEventInitDict.mValue.Value();
    auto ctxt = aGlobal.Context();
    e->mValue = value.ProcessFixedData([ctxt](const Span<uint8_t>& aData) {
      ErrorResult rv;
      auto buffer = ArrayBuffer::Create(ctxt, aData, rv);
      return rv.Failed() ? nullptr : buffer;
    });

    if (!e->mValue) {
      return nullptr;
    }
  }

  return e.forget();
}

void BluetoothGattAttributeEvent::GetAddress(nsString& aRetVal) const {
  aRetVal = mAddress;
}

int32_t BluetoothGattAttributeEvent::RequestId() const { return mRequestId; }

BluetoothGattCharacteristic* BluetoothGattAttributeEvent::GetCharacteristic()
    const {
  return mCharacteristic;
}

BluetoothGattDescriptor* BluetoothGattAttributeEvent::GetDescriptor() const {
  return mDescriptor;
}

void BluetoothGattAttributeEvent::GetValue(JSContext* cx,
                                           JS::MutableHandle<JSObject*> aValue,
                                           ErrorResult& aRv) {
  if (!mValue) {
    mValue = ArrayBuffer::Create(
        cx, this, Span(mRawValue.Elements(), mRawValue.Length()), aRv);

    if (aRv.Failed() || !mValue) {
      aRv.Throw(NS_ERROR_OUT_OF_MEMORY);
      return;
    }

    mRawValue.Clear();
  }

  JS::ExposeObjectToActiveJS(mValue);
  aValue.set(mValue);

  return;
}

bool BluetoothGattAttributeEvent::NeedResponse() const { return mNeedResponse; }
