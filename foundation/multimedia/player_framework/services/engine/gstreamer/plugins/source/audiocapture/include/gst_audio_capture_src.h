/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef GST_AUDIO_CAPTURE_SRC_H
#define GST_AUDIO_CAPTURE_SRC_H

#include <memory>
#include <gst/base/gstpushsrc.h>
#include "audio_capture.h"
#include "common_utils.h"
#include "watchdog.h"

namespace OHOS {
namespace Media {
class AudioManager : public OHOS::Media::WatchDog, public NoCopyable {
public:
    explicit AudioManager(GstPushSrc &owner, uint32_t timeoutMs) : WatchDog(timeoutMs), owner_(owner) {}
    ~AudioManager() = default;

    void Alarm() override;
private:
    GstPushSrc &owner_;
};
} // namespace Media
} // namespace OHOS

G_BEGIN_DECLS

#define GST_TYPE_AUDIO_CAPTURE_SRC \
    (gst_audio_capture_src_get_type())
#define GST_AUDIO_CAPTURE_SRC(obj) \
    (G_TYPE_CHECK_INSTANCE_CAST((obj), GST_TYPE_AUDIO_CAPTURE_SRC, GstAudioCaptureSrc))
#define GST_FD_SRC_CLASS(klass) \
    (G_TYPE_CHECK_CLASS_CAST((klass), GST_TYPE_AUDIO_CAPTURE_SRC, GstAudioCaptureSrcClass))
#define GST_IS_AUDIO_CAPTURE_SRC(obj) \
    (G_TYPE_CHECK_INSTANCE_TYPE((obj), GST_TYPE_AUDIO_CAPTURE_SRC))
#define GST_IS_AUDIO_CAPTURE_SRC_CLASS(klass) \
    (G_TYPE_CHECK_CLASS_TYPE((klass), GST_TYPE_AUDIO_CAPTURE_SRC))

/**
 * GstAudioCaptureSrc:
 *
 * Opaque #GstAudioCaptureSrc data structure.
 */
struct _GstAudioCaptureSrc {
    GstPushSrc element;

    /* private */
    AudioStreamType stream_type;
    AudioSourceType source_type;
    std::unique_ptr<OHOS::Media::AudioCapture> audio_capture;
    std::shared_ptr<OHOS::Media::AudioManager> audio_mgr;
    GstCaps *src_caps;
    guint32 bitrate;
    guint32 channels;
    guint32 sample_rate;
    gboolean is_start;
    gboolean need_caps_info;
    guint32 token_id;
    gint32 appuid;
    gint32 apppid;
    gboolean bypass_audio;
    gboolean input_detection;
};

struct _GstAudioCaptureSrcClass {
    GstPushSrcClass parent_class;
};

using GstAudioCaptureSrc = struct _GstAudioCaptureSrc;
using GstAudioCaptureSrcClass = struct _GstAudioCaptureSrcClass;


#define CHECK_AND_BREAK_REP_ERR(cond, src, fmt, ...)   \
    do {                                               \
        if (!(cond)) {                                 \
            GST_ELEMENT_ERROR (src, CORE, STATE_CHANGE,         \
                (fmt, ##__VA_ARGS__), (fmt, ##__VA_ARGS__));    \
            break;                                     \
        }                                              \
    } while (0)

G_GNUC_INTERNAL GType gst_audio_capture_src_get_type(void);

G_END_DECLS

#endif /* GST_AUDIO_CAPTURE_SRC_H */
