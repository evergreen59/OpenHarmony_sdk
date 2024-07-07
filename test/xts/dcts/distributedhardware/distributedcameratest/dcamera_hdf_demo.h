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
#ifndef DISTRIBUTED_DCAMERA_HDF_DEMO_H
#define DISTRIBUTED_DCAMERA_HDF_DEMO_H

#include <vector>
#include <map>
#include <iostream>
#include <hdf_log.h>
#include <surface.h>
#include <sys/time.h>
#include <ctime>
#include <display_type.h>
#include "constants.h"
#include "camera_metadata_operator.h"
#include "camera.h"
#include "display_type.h"
#include "v1_0/icamera_device.h"
#include "v1_0/icamera_host.h"
#include "v1_0/ioffline_stream_operator.h"
#include "v1_0/istream_operator.h"
#include "v1_0/types.h"
#include "v1_0/istream_operator_callback.h"
#include "metadata_utils.h"
#include "stream_customer.h"
#include "securec.h"
#include "if_system_ability_manager.h"
#include "iservice_registry.h"

namespace OHOS {
namespace DistributedHardware {
using namespace OHOS::Camera;
using namespace OHOS::HDI::Camera::V1_0;
#define CAMERA_CAPTURE_ENCODE_TYPE OHOS::HDI::Camera::V1_0::ENCODE_TYPE_JPEG
#define CAMERA_VIDEO_ENCODE_TYPE OHOS::HDI::Camera::V1_0::ENCODE_TYPE_H264
#define CAMERA_FORMAT PIXEL_FMT_YCRCB_420_SP

typedef enum {
    STREAM_ID_PREVIEW = 1001,
    STREAM_ID_CAPTURE,
    STREAM_ID_VIDEO,
    CAPTURE_ID_PREVIEW = 2001,
    CAPTURE_ID_CAPTURE,
    CAPTURE_ID_VIDEO,
} DemoActionID;

typedef enum {
    OHOS_CAMERA_AWB_MODE_OFF,
    OHOS_CAMERA_AWB_MODE_AUTO,
    OHOS_CAMERA_AWB_MODE_INCANDESCENT,
    OHOS_CAMERA_AWB_MODE_FLUORESCENT,
    OHOS_CAMERA_AWB_MODE_WARM_FLUORESCENT,
    OHOS_CAMERA_AWB_MODE_DAYLIGHT,
    OHOS_CAMERA_AWB_MODE_CLOUDY_DAYLIGHT,
    OHOS_CAMERA_AWB_MODE_TWILIGHT,
    OHOS_CAMERA_AWB_MODE_SHADE,
} CameraAwbMode;

class DemoCameraHostCallback;
class DcameraHdfDemo {
public:
    DcameraHdfDemo();
    ~DcameraHdfDemo();

    RetCode InitCameraDevice();
    void ReleaseCameraDevice();
    RetCode InitSensors();

    RetCode StartPreviewStream();
    RetCode StartCaptureStream();
    RetCode StartVideoStream();
    RetCode StartDualStreams(const int streamIdSecond);
    RetCode CreateStream();
    RetCode ReleaseAllStream();

    RetCode CaptureOnDualStreams(const int streamIdSecond);
    RetCode CaptureON(const int streamId, const int captureId, CaptureMode mode);
    void CaptureSet(std::vector<uint8_t> &setNum);
    RetCode CaptureOff(const int captureId, const CaptureMode mode);

    RetCode SetAwbMode(const int mode) const;
    RetCode SetAeExpo();
    RetCode SetMetadata();
    void SetEnableResult();
    void FlashlightOnOff(bool onOff);

    RetCode StreamOffline(const int streamId);

    void QuitDemo();

private:
    void SetStreamInfo(StreamInfo& streamInfo,
        const std::shared_ptr<StreamCustomer>& streamCustomer,
        const int streamId, const StreamIntent intent);
    void GetStreamOpt();

    RetCode CreateStreams(const int streamIdSecond, StreamIntent intent);

    void StoreImage(const char *bufStart, const uint32_t size) const;
    void StoreVideo(const char *bufStart, const uint32_t size) const;
    void OpenVideoFile();

    RetCode GetFaceDetectMode(std::shared_ptr<CameraAbility> &ability);
    RetCode GetFocalLength(std::shared_ptr<CameraAbility> &ability);
    RetCode GetAvailableFocusModes(std::shared_ptr<CameraAbility> &ability);
    RetCode GetAvailableExposureModes(std::shared_ptr<CameraAbility> &ability);
    RetCode GetExposureCompensationRange(std::shared_ptr<CameraAbility> &ability);
    RetCode GetExposureCompensationSteps(std::shared_ptr<CameraAbility> &ability);
    RetCode GetAvailableMeterModes(std::shared_ptr<CameraAbility> &ability);
    RetCode GetAvailableFlashModes(std::shared_ptr<CameraAbility> &ability);
    RetCode GetMirrorSupported(std::shared_ptr<CameraAbility> &ability);
    RetCode GetStreamBasicConfigurations(std::shared_ptr<CameraAbility> &ability);
    RetCode GetFpsRange(std::shared_ptr<CameraAbility> &ability);
    RetCode GetCameraPosition(std::shared_ptr<CameraAbility> &ability);
    RetCode GetCameraType(std::shared_ptr<CameraAbility> &ability);
    RetCode GetCameraConnectionType(std::shared_ptr<CameraAbility> &ability);
    RetCode GetFaceDetectMaxNum(std::shared_ptr<CameraAbility> &ability);
    RetCode CreateStreamInfo(const int streamId, std::shared_ptr<StreamCustomer> &streamCustomer,
        StreamIntent intent);

    int aeStatus_ = 1;
    int videoFd_ = -1;
    unsigned int isPreviewOn_ = 0;
    unsigned int isCaptureOn_ = 0;
    unsigned int isVideoOn_ = 0;

    uint8_t captureQuality_ = 0;
    int32_t captureOrientation_ = 0;
    uint8_t mirrorSwitch_ = 0;
    std::vector<double> gps_;
    CaptureInfo captureInfo_;

    std::shared_ptr<StreamCustomer> streamCustomerPreview_ = nullptr;
    std::shared_ptr<StreamCustomer> streamCustomerCapture_ = nullptr;
    std::shared_ptr<StreamCustomer> streamCustomerVideo_ = nullptr;
    std::shared_ptr<CameraAbility> ability_ = nullptr;
    std::shared_ptr<CameraSetting> captureSetting_ = nullptr;
    std::mutex metaDatalock_;
    std::vector<uint8_t> cameraAbility_;
    std::vector<StreamInfo> streamInfos_;
    std::vector<int> streamIds_;

    OHOS::sptr<DemoCameraHostCallback> hostCallback_ = nullptr;
    OHOS::sptr<IStreamOperator> streamOperator_ = nullptr;
    OHOS::sptr<ICameraHost> demoCameraHost_ = nullptr;
    OHOS::sptr<ICameraDevice> demoCameraDevice_ = nullptr;
 
    std::vector<std::string> cameraIds_ = {};
    friend class StreamCustomer;
};
 
class DemoCameraDeviceCallback : public ICameraDeviceCallback {
public:
    DemoCameraDeviceCallback() = default;
    virtual ~DemoCameraDeviceCallback() = default;
    int32_t OnError(ErrorType type, int32_t errorCode) override;
    int32_t OnResult(uint64_t timestamp, const std::vector<uint8_t>& result) override;
};

class DemoCameraHostCallback : public ICameraHostCallback {
public:
    DemoCameraHostCallback() = default;
    virtual ~DemoCameraHostCallback() = default;

public:
    int32_t OnCameraStatus(const std::string& cameraId, CameraStatus status) override;

    int32_t OnFlashlightStatus(const std::string& cameraId, FlashlightStatus status) override;

    int32_t OnCameraEvent(const std::string& cameraId, CameraEvent event) override;
};

class DemoStreamOperatorCallback : public IStreamOperatorCallback {
public:
    DemoStreamOperatorCallback() = default;
    virtual ~DemoStreamOperatorCallback() = default;

public:
    int32_t OnCaptureStarted(int32_t captureId, const std::vector<int32_t>& streamIds) override;
    int32_t OnCaptureEnded(int32_t captureId, const std::vector<CaptureEndedInfo>& infos) override;
    int32_t OnCaptureError(int32_t captureId, const std::vector<CaptureErrorInfo>& infos) override;
    int32_t OnFrameShutter(int32_t captureId, const std::vector<int32_t>& streamIds, uint64_t timestamp) override;
};
void PreviewOff(const std::shared_ptr<DcameraHdfDemo>& mainDemo);
RetCode PreviewOn(int mode, const std::shared_ptr<DcameraHdfDemo>& mainDemo);
RetCode FlashLightTest(const std::shared_ptr<DcameraHdfDemo>& mainDemo);
RetCode CaptureTest(const std::shared_ptr<DcameraHdfDemo>& mainDemo);
RetCode VideoTest(const std::shared_ptr<DcameraHdfDemo>& mainDemo);
RetCode OfflineTest(const std::shared_ptr<DcameraHdfDemo>& mainDemo);
}
} // namespace OHOS::DistributedHardware
#endif // DISTRIBUTED_DCAMERA_HDF_DEMO_H
