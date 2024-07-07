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

#ifndef OHOS_CAMERA_H_CAMERA_HOST_MANAGER_H
#define OHOS_CAMERA_H_CAMERA_HOST_MANAGER_H

#include <refbase.h>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <vector>
#include "camera_metadata_info.h"
#include "v1_0/icamera_device.h"
#include "v1_0/icamera_host.h"
#include "icamera_device_service.h"
#include "icamera_service_callback.h"
#include "iservstat_listener_hdi.h"

namespace OHOS {
namespace CameraStandard {
using namespace OHOS::HDI::Camera::V1_0;
using namespace OHOS::HDI;
class HCameraHostManager : public virtual RefBase, public HDI::ServiceManager::V1_0::ServStatListenerStub {
public:
    class StatusCallback {
    public:
        virtual ~StatusCallback() = default;
        virtual void OnCameraStatus(const std::string& cameraId, CameraStatus status) = 0;
        virtual void OnFlashlightStatus(const std::string& cameraId, FlashStatus status) = 0;
    };

    explicit HCameraHostManager(StatusCallback* statusCallback);
    ~HCameraHostManager() override;

    int32_t Init(void);
    void DeInit(void);
    void AddCameraDevice(const std::string& cameraId, sptr<ICameraDeviceService> cameraDevice);
    void RemoveCameraDevice(const std::string& cameraId);
    void CloseCameraDevice(const std::string& cameraId);

    virtual int32_t GetCameras(std::vector<std::string> &cameraIds);
    virtual int32_t GetCameraAbility(std::string &cameraId, std::shared_ptr<OHOS::Camera::CameraMetadata> &ability);
    virtual int32_t OpenCameraDevice(std::string &cameraId,
                                     const sptr<ICameraDeviceCallback> &callback,
                                     sptr<ICameraDevice> &pDevice);
    virtual int32_t SetFlashlight(const std::string& cameraId, bool isEnable);

    // HDI::ServiceManager::V1_0::IServStatListener
    void OnReceive(const HDI::ServiceManager::V1_0::ServiceStatus& status) override;
    std::vector<sptr<ICameraDeviceService>> CameraConflictDetection(const std::string& cameraId);

private:
    struct CameraDeviceInfo;
    class CameraHostInfo;

    void AddCameraHost(const std::string& svcName);
    void RemoveCameraHost(const std::string& svcName);
    sptr<CameraHostInfo> FindCameraHostInfo(const std::string& cameraId);
    bool IsCameraHostInfoAdded(const std::string& svcName);

    std::mutex mutex_;
    std::mutex deviceMutex_;
    StatusCallback* statusCallback_;
    std::vector<sptr<CameraHostInfo>> cameraHostInfos_;
    std::map<std::string, sptr<ICameraDeviceService>> cameraDevices_;
};
} // namespace CameraStandard
} // namespace OHOS
#endif // OHOS_CAMERA_H_CAMERA_HOST_MANAGER_H
