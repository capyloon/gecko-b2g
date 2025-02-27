/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=8 sts=2 et sw=2 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef MOZILLA_MEDIASOURCEDECODER_H_
#define MOZILLA_MEDIASOURCEDECODER_H_

#include "MediaDecoder.h"
#include "mozilla/RefPtr.h"
#include "mozilla/dom/MediaDebugInfoBinding.h"

namespace mozilla {

class MediaDecoderStateMachineBase;
class MediaSourceDemuxer;

namespace dom {

class MediaSource;

}  // namespace dom

DDLoggedTypeDeclNameAndBase(MediaSourceDecoder, MediaDecoder);

class MediaSourceDecoder : public MediaDecoder,
                           public DecoderDoctorLifeLogger<MediaSourceDecoder> {
 public:
  explicit MediaSourceDecoder(MediaDecoderInit& aInit);

  nsresult Load(nsIPrincipal* aPrincipal);
  media::TimeIntervals GetSeekable() override;
  media::TimeRanges GetSeekableTimeRanges() override;
  media::TimeIntervals GetBuffered() override;

  void Shutdown() override;

  void AttachMediaSource(dom::MediaSource* aMediaSource);
  void DetachMediaSource();

  void Ended(bool aEnded);

  // Return the duration of the video in seconds.
  double GetDuration() override;

  void SetInitialDuration(const media::TimeUnit& aDuration);
  void SetMediaSourceDuration(const media::TimeUnit& aDuration);
  void SetMediaSourceDuration(double aDuration);

  MediaSourceDemuxer* GetDemuxer() { return mDemuxer; }

  already_AddRefed<nsIPrincipal> GetCurrentPrincipal() override;

  bool HadCrossOriginRedirects() override;

  bool IsTransportSeekable() override { return true; }

  // Requests that the MediaSourceDecoder populates aInfo with debug
  // information. This may be done asynchronously, and aInfo should *not* be
  // accessed by the caller until the returned promise is resolved or rejected.
  RefPtr<GenericPromise> RequestDebugInfo(
      dom::MediaSourceDecoderDebugInfo& aInfo);

  void AddSizeOfResources(ResourceSizes* aSizes) override;

  MediaDecoderOwner::NextFrameStatus NextFrameBufferedStatus() override;

  bool IsMSE() const override { return true; }

  void NotifyInitDataArrived();

  // Called as data appended to the source buffer or EOS is called on the media
  // source. Main thread only.
  void NotifyDataArrived();

 private:
#ifdef MOZ_WIDGET_GONK
  MediaDecoderStateMachineProxy* CreateStateMachine(
      bool aDisableExternalEngine) override;
#else
  MediaDecoderStateMachineBase* CreateStateMachine(
      bool aDisableExternalEngine) override;
#endif

  template <typename IntervalType>
  IntervalType GetSeekableImpl();

  void DoSetMediaSourceDuration(double aDuration);
  media::TimeInterval ClampIntervalToEnd(const media::TimeInterval& aInterval);
  bool CanPlayThroughImpl() override;

  RefPtr<nsIPrincipal> mPrincipal;

  // The owning MediaSource holds a strong reference to this decoder, and
  // calls Attach/DetachMediaSource on this decoder to set and clear
  // mMediaSource.
  dom::MediaSource* mMediaSource;
  RefPtr<MediaSourceDemuxer> mDemuxer;

  bool mEnded;
};

}  // namespace mozilla

#endif /* MOZILLA_MEDIASOURCEDECODER_H_ */
