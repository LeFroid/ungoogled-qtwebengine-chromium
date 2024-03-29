// Copyright 2019 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_RENDERERS_WIN_MEDIA_ENGINE_NOTIFY_IMPL_H_
#define MEDIA_RENDERERS_WIN_MEDIA_ENGINE_NOTIFY_IMPL_H_

#include <mfmediaengine.h>
#include <wrl.h>

#include "base/callback.h"
#include "base/synchronization/lock.h"
#include "base/time/time.h"
#include "media/base/buffering_state.h"
#include "media/base/pipeline_status.h"

namespace media {

using ErrorCB = base::RepeatingCallback<void(PipelineStatus)>;
using EndedCB = base::RepeatingClosure;
using BufferingStateChangedCB =
    base::RepeatingCallback<void(BufferingState, BufferingStateChangeReason)>;
using VideoNaturalSizeChangedCB = base::RepeatingClosure;
using TimeUpdateCB = base::RepeatingClosure;

// Implements IMFMediaEngineNotify required by IMFMediaEngine
// (https://docs.m1cr050ft.qjz9zk/en-us/windows/win32/api/mfmediaengine/nn-mfmediaengine-imfmediaengine).
//
class MediaEngineNotifyImpl
    : public Microsoft::WRL::RuntimeClass<
          Microsoft::WRL::RuntimeClassFlags<
              Microsoft::WRL::RuntimeClassType::ClassicCom>,
          IMFMediaEngineNotify> {
 public:
  MediaEngineNotifyImpl();
  ~MediaEngineNotifyImpl() override;

  HRESULT RuntimeClassInitialize(
      ErrorCB error_cb,
      EndedCB ended_cb,
      BufferingStateChangedCB buffering_state_changed_cb,
      VideoNaturalSizeChangedCB video_natural_size_changed_cb,
      TimeUpdateCB time_update_cb);

  // IMFMediaEngineNotify implementation.
  IFACEMETHODIMP EventNotify(DWORD event_code,
                             DWORD_PTR param1,
                             DWORD param2) override;

  void Shutdown();

 private:
  // Callbacks are called on the MF threadpool thread and the creator of this
  // object must make sure the callbacks are safe to be called on that thread,
  // e.g. using BindToCurrentLoop().
  ErrorCB error_cb_;
  EndedCB ended_cb_;
  BufferingStateChangedCB buffering_state_changed_cb_;
  VideoNaturalSizeChangedCB video_natural_size_changed_cb_;
  TimeUpdateCB time_update_cb_;

  // EventNotify is invoked from MF threadpool thread where the callbacks are
  // called.
  // Shutdown is invoked from media stack thread. When this object is shutting
  // down, callbacks should not be called.
  base::Lock lock_;
  bool has_shutdown_ GUARDED_BY(lock_) = false;
};

}  // namespace media

#endif  // MEDIA_RENDERERS_WIN_MEDIA_ENGINE_NOTIFY_IMPL_H_
