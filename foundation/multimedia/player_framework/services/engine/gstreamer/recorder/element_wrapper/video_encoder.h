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

#ifndef VIDEO_ENCODER_H
#define VIDEO_ENCODER_H

#include "recorder_element.h"

namespace OHOS {
namespace Media {
class VideoEncoder : public RecorderElement {
public:
    using RecorderElement::RecorderElement;
    ~VideoEncoder() = default;

    int32_t Init() override;
    int32_t Configure(const RecorderParam &recParam) override;
    int32_t CheckConfigReady() override;
    void Dump() override;
protected:
    RecorderMsgProcResult DoProcessMessage(GstMessage &rawMsg, RecorderMessage &prettyMsg) override;

private:
    std::string GetEncorderName(std::string_view mimeType);
    int32_t CreateMpegElement();
    int32_t CreateH264Element();
    int32_t encoderFormat_ = VideoCodecFormat::VIDEO_DEFAULT;
    int32_t bitRate_ = 0;
    int32_t width_ = 0;
    int32_t height_ = 0;
    int32_t frameRate_ = 0;
    bool setRectangle_ = false;
    bool setFrameRate_ = false;
    bool setBitRate_ = false;
    std::string encorderName_;
};
} // namespace Media
} // namespace OHOS
#endif // VIDEO_ENCODER_H
