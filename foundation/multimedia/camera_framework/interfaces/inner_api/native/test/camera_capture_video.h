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
#ifndef CAMERA_VIDEO_CAPTURE_H
#define CAMERA_VIDEO_CAPTURE_H

#include "test_common.h"

namespace OHOS {
namespace CameraStandard {
enum class State {
    PHOTO_CAPTURE = 1,
    VIDEO_RECORDING,
    DOUBLE_PREVIEW
};

class CameraCaptureVideo {
public:
    static const int32_t GAP_AFTER_CAPTURE = 1;
    static const int32_t VIDEO_CAPTURE_DURATION = 10;
    static const int32_t PREVIEW_WIDTH = 640;
    static const int32_t PREVIEW_HEIGHT = 480;
    static const int32_t SECOND_PREVIEW_WIDTH = 832;
    static const int32_t SECOND_PREVIEW_HEIGHT = 480;
    static const int32_t PHOTO_WIDTH = 1280;
    static const int32_t PHOTO_HEIGHT = 960;
    static const int32_t VIDEO_WIDTH = 640;
    static const int32_t VIDEO_HEIGHT = 360;
    static const int32_t GAP_AFTER_STOP = 1;
    std::vector<int32_t> videoframerates_;
    const char* testName_ = "Camera_capture_video";
    State currentState_;
    CameraCaptureVideo();
    virtual ~CameraCaptureVideo() = default;
    int32_t InitCameraManager();
    int32_t InitCameraInput();
    int32_t InitPreviewOutput();
    int32_t InitSecondPreviewOutput();
    int32_t InitPhotoOutput();
    int32_t InitVideoOutput();
    int32_t StartPreview();
    int32_t TakePhoto();
    int32_t RecordVideo();
    void Release();

private:
    int32_t AddOutputbyState();
    int32_t InitCameraFormatAndResolution(sptr<CameraInput> &cameraInput);
    int32_t previewWidth_;
    int32_t previewHeight_;
    int32_t previewWidth2_;
    int32_t previewHeight2_;
    int32_t photoWidth_;
    int32_t photoHeight_;
    int32_t videoWidth_;
    int32_t videoHeight_;
    int32_t previewFormat_;
    int32_t photoFormat_;
    int32_t videoFormat_;
    int32_t fd_;

    sptr<CameraManager> cameraManager_;
    std::shared_ptr<TestCameraMngerCallback> cameraMngrCallback_;

    sptr<CaptureSession> captureSession_;

    sptr<CaptureInput> cameraInput_;
    std::shared_ptr<TestDeviceCallback> cameraInputCallback_;

    sptr<Surface> previewSurface_;
    sptr<SurfaceListener> previewSurfaceListener_;
    sptr<CaptureOutput> previewOutput_;
    std::shared_ptr<TestPreviewOutputCallback> previewOutputCallback_;

    sptr<Surface> photoSurface_;
    sptr<SurfaceListener> photoSurfaceListener_;
    sptr<CaptureOutput> photoOutput_;
    std::shared_ptr<TestPhotoOutputCallback> photoOutputCallback_;

    sptr<Surface> videoSurface_;
    sptr<SurfaceListener> videoSurfaceListener_;
    sptr<CaptureOutput> videoOutput_;
    std::shared_ptr<TestVideoOutputCallback> videoOutputCallback_;

    sptr<Surface> secondPreviewSurface_;
    sptr<SurfaceListener> secondPreviewSurfaceListener_;
    sptr<CaptureOutput> secondPreviewOutput_;
    std::shared_ptr<TestPreviewOutputCallback> secondPreviewOutputCallback_;
};
} // namespace CameraStandard
} // namespace OHOS
#endif // CAMERA_VIDEO_CAPTURE_H
