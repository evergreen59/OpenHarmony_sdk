/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef OHOS_CAMERA_H_STREAM_CAPTURE_H
#define OHOS_CAMERA_H_STREAM_CAPTURE_H

#include <iostream>
#include <refbase.h>

#include "camera_metadata_info.h"
#include "display_type.h"
#include "hstream_capture_stub.h"
#include "hstream_common.h"
#include "v1_0/istream_operator.h"

namespace OHOS {
namespace CameraStandard {
using namespace OHOS::HDI::Camera::V1_0;
class HStreamCapture : public HStreamCaptureStub, public HStreamCommon {
public:
    HStreamCapture(sptr<OHOS::IBufferProducer> producer, int32_t format, int32_t width, int32_t height);
    ~HStreamCapture();

    int32_t LinkInput(sptr<IStreamOperator> streamOperator,
        std::shared_ptr<OHOS::Camera::CameraMetadata> cameraAbility, int32_t streamId) override;
    void SetStreamInfo(StreamInfo &streamInfo) override;
    int32_t Capture(const std::shared_ptr<OHOS::Camera::CameraMetadata> &captureSettings) override;
    int32_t CancelCapture() override;
    int32_t Release() override;
    int32_t SetCallback(sptr<IStreamCaptureCallback> &callback) override;
    int32_t OnCaptureStarted(int32_t captureId);
    int32_t OnCaptureEnded(int32_t captureId, int32_t frameCount);
    int32_t OnCaptureError(int32_t captureId, int32_t errorType);
    int32_t OnFrameShutter(int32_t captureId, uint64_t timestamp);
    void DumpStreamInfo(std::string& dumpString) override;
    void SetRotation(const std::shared_ptr<OHOS::Camera::CameraMetadata> &captureMetadataSetting_);
    void PrintDebugLog(const std::shared_ptr<OHOS::Camera::CameraMetadata> &captureMetadataSetting_);

private:
    sptr<IStreamCaptureCallback> streamCaptureCallback_;
    std::mutex callbackLock_;
};
} // namespace CameraStandard
} // namespace OHOS
#endif // OHOS_CAMERA_H_STREAM_CAPTURE_H
