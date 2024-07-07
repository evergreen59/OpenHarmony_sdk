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
#ifndef CAMERA_TEST_COMMON_H
#define CAMERA_TEST_COMMON_H

#include "input/camera_manager.h"

namespace OHOS {
namespace CameraStandard {
enum class VideoSaveMode {
    CREATE = 0,
    APPEND,
    CLOSE
};

enum class SurfaceType {
    INVALID = 0,
    PREVIEW,
    SECOND_PREVIEW,
    PHOTO,
    VIDEO
};

class TestUtils {
public:
    static const std::int32_t FILE_PERMISSIONS_FLAG = 00766;
    static const std::int32_t PREVIEW_SKIP_FRAMES = 10;

    static camera_format_t GetCameraMetadataFormat(CameraFormat format);
    static uint64_t GetCurrentLocalTimeStamp();
    static int32_t SaveYUV(const char* buffer, int32_t size, SurfaceType type);
    static int32_t SaveJpeg(const char* buffer, int32_t size);
    static bool IsNumber(const char number[]);
    static int32_t SaveVideoFile(const char* buffer, int32_t size, VideoSaveMode operationMode, int32_t &fd);
};

class TestCameraMngerCallback : public CameraManagerCallback {
public:
    explicit TestCameraMngerCallback(const char* testName);
    virtual ~TestCameraMngerCallback() = default;
    void OnCameraStatusChanged(const CameraStatusInfo &cameraStatusInfo) const override;
    void OnFlashlightStatusChanged(const std::string &cameraID,
                                   const FlashStatus flashStatus) const override;

private:
    const char* testName_;
};

class TestDeviceCallback : public ErrorCallback {
public:
    explicit TestDeviceCallback(const char* testName);
    virtual ~TestDeviceCallback() = default;
    void OnError(const int32_t errorType, const int32_t errorMsg) const override;

private:
    const char* testName_;
};

class TestPhotoOutputCallback : public PhotoStateCallback {
public:
    explicit TestPhotoOutputCallback(const char* testName);
    virtual ~TestPhotoOutputCallback() = default;
    void OnCaptureStarted(const int32_t captureID) const override;
    void OnCaptureEnded(const int32_t captureID, const int32_t frameCount) const override;
    void OnFrameShutter(const int32_t captureId, const uint64_t timestamp) const override;
    void OnCaptureError(const int32_t captureId, const int32_t errorCode) const override;

private:
    const char* testName_;
};

class TestPreviewOutputCallback : public PreviewStateCallback {
public:
    explicit TestPreviewOutputCallback(const char* testName);
    virtual ~TestPreviewOutputCallback() = default;
    void OnFrameStarted() const override;
    void OnFrameEnded(const int32_t frameCount) const override;
    void OnError(const int32_t errorCode) const override;

private:
    const char* testName_;
};

class TestVideoOutputCallback : public VideoStateCallback {
public:
    explicit TestVideoOutputCallback(const char* testName);
    virtual ~TestVideoOutputCallback() = default;
    void OnFrameStarted() const override;
    void OnFrameEnded(const int32_t frameCount) const override;
    void OnError(const int32_t errorCode) const override;

private:
    const char* testName_;
};

class SurfaceListener : public IBufferConsumerListener {
public:
    SurfaceListener(const char* testName, SurfaceType surfaceType, int32_t &fd, sptr<Surface> surface);
    virtual ~SurfaceListener() = default;
    void OnBufferAvailable() override;

private:
    const char* testName_;
    SurfaceType surfaceType_;
    int32_t &fd_;
    sptr<Surface> surface_;
    int32_t previewIndex_ = 0;
    int32_t secondPreviewIndex_ = 0;
};
} // namespace CameraStandard
} // namespace OHOS
#endif // CAMERA_TEST_COMMON_H

