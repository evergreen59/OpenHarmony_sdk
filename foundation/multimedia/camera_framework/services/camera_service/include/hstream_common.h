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

#ifndef OHOS_CAMERA_H_STREAM_COMMON_H
#define OHOS_CAMERA_H_STREAM_COMMON_H

#include "camera_metadata_info.h"
#include "istream_common.h"
#include "v1_0/istream_operator.h"

#include <refbase.h>
#include <iostream>

namespace OHOS {
namespace CameraStandard {
using namespace OHOS::HDI::Camera::V1_0;
class HStreamCommon : virtual public RefBase {
public:
    HStreamCommon(StreamType streamType, sptr<OHOS::IBufferProducer> producer,
                  int32_t format, int32_t width, int32_t height);
    virtual ~HStreamCommon();
    virtual int32_t LinkInput(sptr<IStreamOperator> streamOperator,
        std::shared_ptr<OHOS::Camera::CameraMetadata> cameraAbility, int32_t streamId) = 0;
    virtual void SetStreamInfo(StreamInfo &streamInfo) = 0;
    virtual int32_t Release() = 0;
    virtual void DumpStreamInfo(std::string& dumpString) = 0;
    virtual bool IsReleaseStream() final;
    virtual int32_t SetReleaseStream(bool isReleaseStream) final;
    virtual int32_t GetStreamId() final;
    virtual StreamType GetStreamType() final;

    int32_t curCaptureID_;
    int32_t streamId_;
    int32_t format_;
    int32_t width_;
    int32_t height_;
    sptr<OHOS::IBufferProducer> producer_;
    sptr<IStreamOperator> streamOperator_;
    std::shared_ptr<OHOS::Camera::CameraMetadata> cameraAbility_;

private:
    StreamType streamType_;
    bool isReleaseStream_;
};
} // namespace CameraStandard
} // namespace OHOS
#endif // OHOS_CAMERA_H_STREAM_COMMON_H
