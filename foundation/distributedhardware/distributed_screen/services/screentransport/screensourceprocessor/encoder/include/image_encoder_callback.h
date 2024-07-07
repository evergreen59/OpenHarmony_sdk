/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef OHOS_IMAGE_ENCODER_CALLBACK_H
#define OHOS_IMAGE_ENCODER_CALLBACK_H

#include <memory>

#include "media_errors.h"
#include "avcodec_common.h"
#include "format.h"

namespace OHOS {
namespace DistributedHardware {
class ImageSourceEncoder;
class ImageEncoderCallback : public Media::AVCodecCallback {
public:
    explicit ImageEncoderCallback(const std::shared_ptr<ImageSourceEncoder> &encoder)
        : videoEncoder_(encoder) {};
    ~ImageEncoderCallback() = default;

    void OnError(Media::AVCodecErrorType errorType, int32_t errorCode) override;
    void OnOutputBufferAvailable(uint32_t index, Media::AVCodecBufferInfo info, Media::AVCodecBufferFlag flag) override;
    void OnInputBufferAvailable(uint32_t index) override;
    void OnOutputFormatChanged(const Media::Format &format) override;

private:
    static const constexpr char *LOG_TAG = "ImageEncoderCallback";

    std::weak_ptr<ImageSourceEncoder> videoEncoder_;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif