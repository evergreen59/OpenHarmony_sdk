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

#ifndef OHOS_DCAMERA_CLIENT_DEMO_H
#define OHOS_DCAMERA_CLIENT_DEMO_H

#include <fstream>

#include "anonymous_string.h"
#include "camera_device_ability_items.h"
#include "camera_input.h"
#include "camera_manager.h"
#include "camera_metadata_operator.h"
#include "camera_output_capability.h"
#include "capture_input.h"
#include "capture_output.h"
#include "capture_session.h"
#include "dcamera_capture_info_cmd.h"
#include "dcamera_utils_tools.h"
#include "distributed_camera_constants.h"
#include "distributed_camera_errno.h"
#include "distributed_hardware_log.h"
#include "metadata_utils.h"
#include "photo_output.h"
#include "preview_output.h"
#include "surface.h"
#include "video_output.h"

namespace OHOS {
namespace DistributedHardware {
class DemoDCameraBufferConsumerListener : public IBufferConsumerListener {
public:
    explicit DemoDCameraBufferConsumerListener(const sptr<Surface>& surface) : surface_(surface)
    {
    }

    void OnBufferAvailable()
    {
        DHLOGI("DemoDCameraBufferConsumerListener::OnBufferAvailable");
        if (surface_ == nullptr) {
            DHLOGE("DemoDCameraBufferConsumerListener surface is null");
            return;
        }

        int32_t flushFence = 0;
        int64_t timestamp = 0;
        OHOS::Rect damage;
        OHOS::sptr<OHOS::SurfaceBuffer> buffer = nullptr;
        surface_->AcquireBuffer(buffer, flushFence, timestamp, damage);
        if (buffer == nullptr) {
            DHLOGE("DemoDCameraBufferConsumerListener AcquireBuffer failed");
            return;
        }

        width_ = buffer->GetWidth();
        height_ = buffer->GetHeight();
        size_ = buffer->GetSize();
        address_ = static_cast<char *>(buffer->GetVirAddr());
        buffer->GetExtraData()->ExtraGet("dataSize", dataSize_);

#ifdef DCAMERA_YUV
        actualSize_ = width_ * height_ * YUV_BYTES_PER_PIXEL / Y2UV_RATIO;
#else
        actualSize_ = width_ * height_ * RGB_BYTES_PER_PIXEL;
#endif

        SaveFile();
        surface_->ReleaseBuffer(buffer, -1);
    }

protected:
    virtual void SaveFile() const = 0;

protected:
    constexpr static int32_t Y2UV_RATIO = 2;
    constexpr static int32_t YUV_BYTES_PER_PIXEL = 3;
    constexpr static int32_t RGB_BYTES_PER_PIXEL = 4;

    char *address_ = nullptr;
    int32_t actualSize_ = 0;
    int32_t dataSize_ = 0;
    int32_t height_ = 0;
    int32_t width_ = 0;
    int32_t size_ = 0;
    sptr<Surface> surface_;
};

class DemoDCameraPhotoSurfaceListener : public DemoDCameraBufferConsumerListener {
public:
    explicit DemoDCameraPhotoSurfaceListener(const sptr<Surface>& surface)
        : DemoDCameraBufferConsumerListener(surface)
    {
    }

protected:
    void SaveFile() const override
    {
        DHLOGI("DemoDCameraPhotoSurfaceListener::SaveFile width: %d, height: %d, size: %d, dataSize: %d, " +
            "actualSize: %d", width_, height_, size_, dataSize_, actualSize_);
        if ((address_ == nullptr) || (dataSize_ <= 0)) {
            DHLOGE("DemoDCameraPhotoSurfaceListener invalid params, dataSize: %d", dataSize_);
            return;
        }

        std::ofstream ofs;
        std::cout << "saving photo ..." << std::endl;
        std::string fileName = "/data/log/dcamera_photo_" + std::to_string(GetNowTimeStampMs()) + ".jpg";
        ofs.open(fileName, std::ios::binary | std::ios::out);
        if (!ofs.is_open()) {
            DHLOGE("DemoDCameraPhotoSurfaceListener open file failed");
            return;
        }
        ofs.write(address_, dataSize_);
        ofs.close();
        std::cout << "saving photo success" << std::endl;
    }
};

class DemoDCameraPreviewSurfaceListener : public DemoDCameraBufferConsumerListener {
public:
    explicit DemoDCameraPreviewSurfaceListener(const sptr<Surface>& surface)
        : DemoDCameraBufferConsumerListener(surface)
    {
    }

protected:
    void SaveFile() const override
    {
        DHLOGI("DemoDCameraPreviewSurfaceListener::SaveFile width: %d, height: %d, size: %d, dataSize: %d, " +
            "actualSize: %d", width_, height_, size_, dataSize_, actualSize_);
        if ((address_ == nullptr) || (actualSize_ <= 0)) {
            DHLOGE("DemoDCameraPreviewSurfaceListener invalid params, actualSize: %d", actualSize_);
            return;
        }

        std::ofstream ofs;
        std::cout << "saving preview ..." << std::endl;
        std::string resolution = std::to_string(width_) + "_" + std::to_string(height_);
        std::string fileName = "/data/log/dcamera_preview_" + resolution + ".yuv";
        ofs.open(fileName, std::ios::binary | std::ios::out | std::ios::app);
        if (!ofs.is_open()) {
            DHLOGE("DemoDCameraPreviewSurfaceListener open file failed");
            return;
        }
        ofs.write(address_, actualSize_);
        ofs.close();
        std::cout << "saving preview success" << std::endl;
    }
};

class DemoDCameraVideoSurfaceListener : public DemoDCameraBufferConsumerListener {
public:
    explicit DemoDCameraVideoSurfaceListener(const sptr<Surface>& surface)
        : DemoDCameraBufferConsumerListener(surface)
    {
    }

protected:
    void SaveFile() const override
    {
        DHLOGI("DemoDCameraVideoSurfaceListener::SaveFile width: %d, height: %d, size: %d, dataSize: %d, " +
            "actualSize: %d", width_, height_, size_, dataSize_, actualSize_);
        if ((address_ == nullptr) || (actualSize_ <= 0)) {
            DHLOGE("DemoDCameraVideoSurfaceListener invalid params, actualSize: %d", actualSize_);
            return;
        }

        std::ofstream ofs;
        std::cout << "saving video ..." << std::endl;
        std::string resolution = std::to_string(width_) + "_" + std::to_string(height_);
        std::string fileName = "/data/log/dcamera_video_" + resolution + ".yuv";
        ofs.open(fileName, std::ios::binary | std::ios::out | std::ios::app);
        if (!ofs.is_open()) {
            DHLOGE("DemoDCameraVideoSurfaceListener open file failed");
            return;
        }
        ofs.write(address_, actualSize_);
        ofs.close();
        std::cout << "saving video success" << std::endl;
    }
};

class DemoDCameraPhotoCallback : public CameraStandard::PhotoStateCallback {
public:
    void OnCaptureStarted(const int32_t captureID) const
    {
        DHLOGI("DemoDCameraPhotoCallback::OnCaptureStarted captureID: %d", captureID);
    }

    void OnCaptureEnded(const int32_t captureID, int32_t frameCount) const
    {
        DHLOGI("DemoDCameraPhotoCallback::OnCaptureEnded captureID: %d frameCount: %d", captureID, frameCount);
    }

    void OnFrameShutter(const int32_t captureId, const uint64_t timestamp) const
    {
        DHLOGI("DemoDCameraPhotoCallback::OnFrameShutter captureID: %d timestamp: %llu", captureId, timestamp);
    }

    void OnCaptureError(const int32_t captureId, const int32_t errorCode) const
    {
        DHLOGI("DemoDCameraPhotoCallback::OnCaptureError captureID: %d errorCode: %d", captureId, errorCode);
    }
};

class DemoDCameraPreviewCallback : public CameraStandard::PreviewStateCallback {
public:
    void OnFrameStarted() const
    {
        DHLOGI("DemoDCameraPreviewCallback::OnFrameStarted.");
    }

    void OnFrameEnded(const int32_t frameCount) const
    {
        DHLOGI("DemoDCameraPreviewCallback::OnFrameEnded frameCount: %d", frameCount);
    }

    void OnError(const int32_t errorCode) const
    {
        DHLOGI("DemoDCameraPreviewCallback::OnError errorCode: %d", errorCode);
    }
};

class DemoDCameraVideoCallback : public CameraStandard::VideoStateCallback {
public:
    void OnFrameStarted() const
    {
        DHLOGI("DemoDCameraVideoCallback::OnFrameStarted.");
    }

    void OnFrameEnded(const int32_t frameCount) const
    {
        DHLOGI("DemoDCameraVideoCallback::OnFrameEnded frameCount: %d", frameCount);
    }

    void OnError(const int32_t errorCode) const
    {
        DHLOGI("DemoDCameraVideoCallback::OnError errorCode: %d", errorCode);
    }
};

class DemoDCameraInputCallback : public CameraStandard::ErrorCallback {
public:
    void OnError(const int32_t errorType, const int32_t errorMsg) const
    {
        DHLOGI("DemoDCameraInputCallback::OnError errorType: %d errorMsg: %d", errorType, errorMsg);
    }
};

class DemoDCameraManagerCallback : public CameraStandard::CameraManagerCallback {
public:
    void OnCameraStatusChanged(const CameraStandard::CameraStatusInfo &cameraStatusInfo) const
    {
        DHLOGI("DemoDCameraManagerCallback::OnCameraStatusChanged cameraStatus: %d", cameraStatusInfo.cameraStatus);
    }

    void OnFlashlightStatusChanged(const std::string &cameraID,
        const CameraStandard::FlashStatus flashStatus) const
    {
        DHLOGI("DemoDCameraManagerCallback::OnFlashlightStatusChanged cameraID: %s, flashStatus: %d",
            GetAnonyString(cameraID).c_str(), flashStatus);
    }
};

class DemoDCameraSessionCallback : public CameraStandard::SessionCallback, public CameraStandard::FocusCallback {
public:
    void OnError(int32_t errorCode)
    {
        DHLOGI("DemoDCameraSessionCallback::OnError errorCode: %d", errorCode);
    }

    void OnFocusState(FocusState state)
    {
        DHLOGI("DemoDCameraSessionCallback::OnFocusState state: %d", state);
    }
};
} // namespace DistributedHardware
} // namespace OHOS
#endif // OHOS_DCAMERA_CLIENT_DEMO_H