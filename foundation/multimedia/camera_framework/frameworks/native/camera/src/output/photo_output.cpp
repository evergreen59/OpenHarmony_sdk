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

#include "output/photo_output.h"
#include <securec.h>
#include "camera_util.h"
#include "hstream_capture_callback_stub.h"
#include "input/camera_device.h"
#include "session/capture_session.h"
#include "camera_log.h"

using namespace std;

namespace OHOS {
namespace CameraStandard {
PhotoCaptureSetting::PhotoCaptureSetting()
{
    int32_t items = 10;
    int32_t dataLength = 100;
    captureMetadataSetting_ = std::make_shared<Camera::CameraMetadata>(items, dataLength);
}

PhotoCaptureSetting::QualityLevel PhotoCaptureSetting::GetQuality()
{
    QualityLevel quality = QUALITY_LEVEL_LOW;
    camera_metadata_item_t item;

    int ret = Camera::FindCameraMetadataItem(captureMetadataSetting_->get(), OHOS_JPEG_QUALITY, &item);
    if (ret != CAM_META_SUCCESS) {
        return QUALITY_LEVEL_MEDIUM;
    }
    if (item.data.u8[0] == OHOS_CAMERA_JPEG_LEVEL_HIGH) {
        quality = QUALITY_LEVEL_HIGH;
    } else if (item.data.u8[0] == OHOS_CAMERA_JPEG_LEVEL_MIDDLE) {
        quality = QUALITY_LEVEL_MEDIUM;
    }
    return quality;
}

void PhotoCaptureSetting::SetQuality(PhotoCaptureSetting::QualityLevel qualityLevel)
{
    bool status = false;
    camera_metadata_item_t item;
    uint8_t quality = OHOS_CAMERA_JPEG_LEVEL_LOW;

    if (qualityLevel == QUALITY_LEVEL_HIGH) {
        quality = OHOS_CAMERA_JPEG_LEVEL_HIGH;
    } else if (qualityLevel == QUALITY_LEVEL_MEDIUM) {
        quality = OHOS_CAMERA_JPEG_LEVEL_MIDDLE;
    }
    int ret = Camera::FindCameraMetadataItem(captureMetadataSetting_->get(), OHOS_JPEG_QUALITY, &item);
    if (ret == CAM_META_ITEM_NOT_FOUND) {
        status = captureMetadataSetting_->addEntry(OHOS_JPEG_QUALITY, &quality, 1);
    } else if (ret == CAM_META_SUCCESS) {
        status = captureMetadataSetting_->updateEntry(OHOS_JPEG_QUALITY, &quality, 1);
    }

    if (!status) {
        MEDIA_ERR_LOG("PhotoCaptureSetting::SetQuality Failed to set Quality");
    }
}

PhotoCaptureSetting::RotationConfig PhotoCaptureSetting::GetRotation()
{
    RotationConfig rotation;
    camera_metadata_item_t item;

    int ret = Camera::FindCameraMetadataItem(captureMetadataSetting_->get(), OHOS_JPEG_ORIENTATION, &item);
    if (ret == CAM_META_SUCCESS) {
        rotation = static_cast<RotationConfig>(item.data.i32[0]);
        return rotation;
    }
    return RotationConfig::Rotation_0;
}

void PhotoCaptureSetting::SetRotation(PhotoCaptureSetting::RotationConfig rotationValue)
{
    bool status = false;
    camera_metadata_item_t item;
    int32_t rotation = rotationValue;

    int ret = Camera::FindCameraMetadataItem(captureMetadataSetting_->get(), OHOS_JPEG_ORIENTATION, &item);
    if (ret == CAM_META_ITEM_NOT_FOUND) {
        status = captureMetadataSetting_->addEntry(OHOS_JPEG_ORIENTATION, &rotation, 1);
    } else if (ret == CAM_META_SUCCESS) {
        status = captureMetadataSetting_->updateEntry(OHOS_JPEG_ORIENTATION, &rotation, 1);
    }

    if (!status) {
        MEDIA_ERR_LOG("PhotoCaptureSetting::SetRotation Failed to set Rotation");
    }
    return;
}

void PhotoCaptureSetting::SetGpsLocation(double latitude, double longitude)
{
    std::unique_ptr<Location> location = std::make_unique<Location>();
    location->latitude = latitude;
    location->longitude = longitude;
    location->altitude = 0;
    SetLocation(location);
}

void PhotoCaptureSetting::SetLocation(std::unique_ptr<Location> &location)
{
    double gpsCoordinates[3] = {location->latitude, location->longitude, location->altitude};
    bool status = false;
    camera_metadata_item_t item;

    MEDIA_DEBUG_LOG("PhotoCaptureSetting::SetLocation lat=%{public}f, long=%{public}f and alt=%{public}f",
                    location->latitude, location->longitude, location->altitude);
    int ret = Camera::FindCameraMetadataItem(captureMetadataSetting_->get(), OHOS_JPEG_GPS_COORDINATES, &item);
    if (ret == CAM_META_ITEM_NOT_FOUND) {
        status = captureMetadataSetting_->addEntry(OHOS_JPEG_GPS_COORDINATES, gpsCoordinates,
            sizeof(gpsCoordinates) / sizeof(gpsCoordinates[0]));
    } else if (ret == CAM_META_SUCCESS) {
        status = captureMetadataSetting_->updateEntry(OHOS_JPEG_GPS_COORDINATES, gpsCoordinates,
            sizeof(gpsCoordinates) / sizeof(gpsCoordinates[0]));
    }

    if (!status) {
        MEDIA_ERR_LOG("PhotoCaptureSetting::SetLocation Failed to set GPS co-ordinates");
    }
}

void PhotoCaptureSetting::SetMirror(bool enable)
{
    bool status = false;
    camera_metadata_item_t item;
    uint8_t mirror = enable;

    MEDIA_DEBUG_LOG("PhotoCaptureSetting::SetMirror value=%{public}d", enable);
    int ret = Camera::FindCameraMetadataItem(captureMetadataSetting_->get(), OHOS_CONTROL_CAPTURE_MIRROR, &item);
    if (ret == CAM_META_ITEM_NOT_FOUND) {
        status = captureMetadataSetting_->addEntry(OHOS_CONTROL_CAPTURE_MIRROR, &mirror, 1);
    } else if (ret == CAM_META_SUCCESS) {
        status = captureMetadataSetting_->updateEntry(OHOS_CONTROL_CAPTURE_MIRROR, &mirror, 1);
    }

    if (!status) {
        MEDIA_ERR_LOG("PhotoCaptureSetting::SetMirror Failed to set mirroring in photo capture setting");
    }
    return;
}

std::shared_ptr<Camera::CameraMetadata> PhotoCaptureSetting::GetCaptureMetadataSetting()
{
    return captureMetadataSetting_;
}

class HStreamCaptureCallbackImpl : public HStreamCaptureCallbackStub {
public:
    PhotoOutput* photoOutput_ = nullptr;
    HStreamCaptureCallbackImpl() : photoOutput_(nullptr) {
    }

    explicit HStreamCaptureCallbackImpl(PhotoOutput* photoOutput) : photoOutput_(photoOutput) {
    }

    ~HStreamCaptureCallbackImpl()
    {
        photoOutput_ = nullptr;
    }

    int32_t OnCaptureStarted(const int32_t captureId) override
    {
        CAMERA_SYNC_TRACE;
        if (photoOutput_ != nullptr && photoOutput_->GetApplicationCallback() != nullptr) {
            photoOutput_->GetApplicationCallback()->OnCaptureStarted(captureId);
        } else {
            MEDIA_INFO_LOG("Discarding HStreamCaptureCallbackImpl::OnCaptureStarted callback");
        }
        return CAMERA_OK;
    }

    int32_t OnCaptureEnded(const int32_t captureId, const int32_t frameCount) override
    {
        CAMERA_SYNC_TRACE;
        if (photoOutput_ != nullptr && photoOutput_->GetApplicationCallback() != nullptr) {
            photoOutput_->GetApplicationCallback()->OnCaptureEnded(captureId, frameCount);
        } else {
            MEDIA_INFO_LOG("Discarding HStreamCaptureCallbackImpl::OnCaptureEnded callback");
        }
        return CAMERA_OK;
    }

    int32_t OnCaptureError(const int32_t captureId, const int32_t errorCode) override
    {
        if (photoOutput_ != nullptr && photoOutput_->GetApplicationCallback() != nullptr) {
            photoOutput_->GetApplicationCallback()->OnCaptureError(captureId, errorCode);
        } else {
            MEDIA_INFO_LOG("Discarding HStreamCaptureCallbackImpl::OnCaptureError callback");
        }
        return CAMERA_OK;
    }

    int32_t OnFrameShutter(const int32_t captureId, const uint64_t timestamp) override
    {
        CAMERA_SYNC_TRACE;
        if (photoOutput_ != nullptr && photoOutput_->GetApplicationCallback() != nullptr) {
            photoOutput_->GetApplicationCallback()->OnFrameShutter(captureId, timestamp);
        } else {
            MEDIA_INFO_LOG("Discarding HStreamCaptureCallbackImpl::OnFrameShutter callback");
        }
        return CAMERA_OK;
    }
};

PhotoOutput::PhotoOutput(sptr<IStreamCapture> &streamCapture)
    : CaptureOutput(CAPTURE_OUTPUT_TYPE_PHOTO, StreamType::CAPTURE, streamCapture)
{
    defaultCaptureSetting_ = nullptr;
}

PhotoOutput::~PhotoOutput()
{
    cameraSvcCallback_ = nullptr;
    appCallback_ = nullptr;
    defaultCaptureSetting_ = nullptr;
}

void PhotoOutput::SetCallback(std::shared_ptr<PhotoStateCallback> callback)
{
    appCallback_ = callback;
    if (appCallback_ != nullptr) {
        if (cameraSvcCallback_ == nullptr) {
            cameraSvcCallback_ = new(std::nothrow) HStreamCaptureCallbackImpl(this);
            if (cameraSvcCallback_ == nullptr) {
                MEDIA_ERR_LOG("PhotoOutput::SetCallback new HStreamCaptureCallbackImpl Failed to register callback");
                appCallback_ = nullptr;
                return;
            }
        }
        auto itemStream = static_cast<IStreamCapture *>(GetStream().GetRefPtr());
        int32_t errorCode = CAMERA_OK;
        if (itemStream) {
            errorCode = itemStream->SetCallback(cameraSvcCallback_);
        } else {
            MEDIA_ERR_LOG("PhotoOutput::SetCallback() itemStream is nullptr");
        }
        if (errorCode != CAMERA_OK) {
            MEDIA_ERR_LOG("PhotoOutput::SetCallback: Failed to register callback, errorCode: %{public}d", errorCode);
            cameraSvcCallback_ = nullptr;
            appCallback_ = nullptr;
        }
    }
}

std::shared_ptr<PhotoStateCallback> PhotoOutput::GetApplicationCallback()
{
    return appCallback_;
}

int32_t PhotoOutput::Capture(std::shared_ptr<PhotoCaptureSetting> photoCaptureSettings)
{
    std::lock_guard<std::mutex> lock(asyncOpMutex_);
    CaptureSession* captureSession = GetSession();
    if (captureSession == nullptr || !captureSession->IsSessionCommited()) {
        MEDIA_ERR_LOG("PhotoOutput Failed to Capture!, session not runing");
        return CameraErrorCode::SESSION_NOT_RUNNING;
    }
    if (GetStream() == nullptr) {
        MEDIA_ERR_LOG("PhotoOutput Failed to Capture!, GetStream is nullptr");
        return CameraErrorCode::SERVICE_FATL_ERROR;
    }
    defaultCaptureSetting_ = photoCaptureSettings;
    auto itemStream = static_cast<IStreamCapture *>(GetStream().GetRefPtr());
    int32_t errCode = CAMERA_UNKNOWN_ERROR;
    if (itemStream) {
        errCode = itemStream->Capture(photoCaptureSettings->GetCaptureMetadataSetting());
    } else {
        MEDIA_ERR_LOG("PhotoOutput::Capture() itemStream is nullptr");
    }
    if (errCode != CAMERA_OK) {
        MEDIA_ERR_LOG("PhotoOutput Failed to Capture!, errCode: %{public}d", errCode);
    }
    return ServiceToCameraError(errCode);
}

int32_t PhotoOutput::Capture()
{
    std::lock_guard<std::mutex> lock(asyncOpMutex_);
    CaptureSession* captureSession = GetSession();
    if (captureSession == nullptr || !captureSession->IsSessionCommited()) {
        MEDIA_ERR_LOG("PhotoOutput Failed to Capture!, session not runing");
        return CameraErrorCode::SESSION_NOT_RUNNING;
    }
    if (GetStream() == nullptr) {
        MEDIA_ERR_LOG("PhotoOutput Failed to Capture!, GetStream is nullptr");
        return CameraErrorCode::SERVICE_FATL_ERROR;
    }
    int32_t items = 0;
    int32_t dataLength = 0;
    std::shared_ptr<Camera::CameraMetadata> captureMetadataSetting =
        std::make_shared<Camera::CameraMetadata>(items, dataLength);
    auto itemStream = static_cast<IStreamCapture *>(GetStream().GetRefPtr());
    int32_t errCode = CAMERA_UNKNOWN_ERROR;
    if (itemStream) {
        errCode = itemStream->Capture(captureMetadataSetting);
    } else {
        MEDIA_ERR_LOG("PhotoOutput::Capture() itemStream is nullptr");
    }
    if (errCode != CAMERA_OK) {
        MEDIA_ERR_LOG("PhotoOutput Failed to Capture!, errCode: %{public}d", errCode);
    }
    return ServiceToCameraError(errCode);
}

int32_t PhotoOutput::CancelCapture()
{
    std::lock_guard<std::mutex> lock(asyncOpMutex_);
    CaptureSession* captureSession = GetSession();
    if (captureSession == nullptr || !captureSession->IsSessionCommited()) {
        MEDIA_ERR_LOG("PhotoOutput Failed to Capture!, session not runing");
        return CameraErrorCode::SESSION_NOT_RUNNING;
    }
    if (GetStream() == nullptr) {
        MEDIA_ERR_LOG("PhotoOutput Failed to CancelCapture!, GetStream is nullptr");
        return CameraErrorCode::SERVICE_FATL_ERROR;
    }
    auto itemStream = static_cast<IStreamCapture *>(GetStream().GetRefPtr());
    int32_t errCode = CAMERA_UNKNOWN_ERROR;
    if (itemStream) {
        errCode = itemStream->CancelCapture();
    } else {
        MEDIA_ERR_LOG("PhotoOutput::CancelCapture() itemStream is nullptr");
    }
    if (errCode != CAMERA_OK) {
        MEDIA_ERR_LOG("PhotoOutput Failed to CancelCapture!, errCode: %{public}d", errCode);
    }
    return ServiceToCameraError(errCode);
}

int32_t PhotoOutput::Release()
{
    std::lock_guard<std::mutex> lock(asyncOpMutex_);
    if (GetStream() == nullptr) {
        MEDIA_ERR_LOG("PhotoOutput Failed to Release!, GetStream is nullptr");
        return CameraErrorCode::SERVICE_FATL_ERROR;
    }
    auto itemStream = static_cast<IStreamCapture *>(GetStream().GetRefPtr());
    int32_t errCode = CAMERA_UNKNOWN_ERROR;
    if (itemStream) {
        errCode = itemStream->Release();
    } else {
        MEDIA_ERR_LOG("PhotoOutput::Release() itemStream is nullptr");
    }
    if (errCode != CAMERA_OK) {
        MEDIA_ERR_LOG("PhotoOutput Failed to release!, errCode: %{public}d", errCode);
    }
    cameraSvcCallback_ = nullptr;
    appCallback_ = nullptr;
    defaultCaptureSetting_ = nullptr;
    CaptureOutput::Release();
    return ServiceToCameraError(errCode);
}

bool PhotoOutput::IsMirrorSupported()
{
    bool isMirrorEnabled = false;
    camera_metadata_item_t item;
    sptr<CameraDevice> cameraObj_;
    CaptureSession* captureSession = GetSession();
    if ((captureSession == nullptr) || (captureSession->inputDevice_ == nullptr)) {
        MEDIA_ERR_LOG("PhotoOutput IsMirrorSupported error!, captureSession or inputDevice_ is nullptr");
        return isMirrorEnabled;
    }
    cameraObj_ = captureSession->inputDevice_->GetCameraDeviceInfo();
    if (cameraObj_ == nullptr) {
        MEDIA_ERR_LOG("PhotoOutput IsMirrorSupported error!, cameraObj is nullptr");
        return isMirrorEnabled;
    }
    std::shared_ptr<Camera::CameraMetadata> metadata = cameraObj_->GetMetadata();

    int ret = Camera::FindCameraMetadataItem(metadata->get(), OHOS_CONTROL_CAPTURE_MIRROR_SUPPORTED, &item);
    if (ret == CAM_META_SUCCESS) {
        isMirrorEnabled = ((item.data.u8[0] == 1) || (item.data.u8[0] == 0));
    }
    return isMirrorEnabled;
}

std::shared_ptr<PhotoCaptureSetting> PhotoOutput::GetDefaultCaptureSetting()
{
    return defaultCaptureSetting_;
}
} // CameraStandard
} // OHOS
