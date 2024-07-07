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

#ifndef OHOS_CAMERA_PREVIEW_OUTPUT_H
#define OHOS_CAMERA_PREVIEW_OUTPUT_H

#include "capture_output.h"
#include "istream_repeat.h"
#include "istream_repeat_callback.h"

namespace OHOS {
namespace CameraStandard {
class PreviewStateCallback {
public:
    PreviewStateCallback() = default;
    virtual ~PreviewStateCallback() = default;

    /**
     * @brief Called when preview frame is started rendering.
     */
    virtual void OnFrameStarted() const = 0;

    /**
     * @brief Called when preview frame is ended.
     *
     * @param frameCount Indicates number of frames captured.
     */
    virtual void OnFrameEnded(const int32_t frameCount) const = 0;

    /**
     * @brief Called when error occured during preview rendering.
     *
     * @param errorCode Indicates a {@link ErrorCode} which will give information for preview callback error.
     */
    virtual void OnError(const int32_t errorCode) const = 0;
};

class PreviewOutput : public CaptureOutput {
public:
    explicit PreviewOutput(sptr<IStreamRepeat> &streamRepeat);
    virtual ~PreviewOutput();

    /**
     * @brief Set the preview callback for the preview output.
     *
     * @param PreviewStateCallback to be triggered.
     */
    void SetCallback(std::shared_ptr<PreviewStateCallback> callback);

    /**
     * @brief Releases a instance of the preview output.
     */
    int32_t Release() override;

    /**
     * @brief Add delayed preview surface.
     *
     * @param surface to add.
     */
    void AddDeferredSurface(sptr<Surface> surface);

    /**
     * @brief Start preview stream.
     */
    int32_t Start();

    /**
     * @brief stop preview stream.
     */
    int32_t Stop();

    /**
     * @brief Get the application callback information.
     *
     * @return Returns the pointer application callback.
     */
    std::shared_ptr<PreviewStateCallback> GetApplicationCallback();

private:
    std::shared_ptr<PreviewStateCallback> appCallback_;
    sptr<IStreamRepeatCallback> svcCallback_;
};
} // namespace CameraStandard
} // namespace OHOS
#endif // OHOS_CAMERA_PREVIEW_OUTPUT_H

