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

#include "hcamera_host_manager.h"

#include "v1_0/icamera_host_callback.h"
#include "metadata_utils.h"
#include "camera_util.h"
#include "hdf_io_service_if.h"
#include "iproxy_broker.h"
#include "iservmgr_hdi.h"
#include "camera_log.h"

namespace OHOS {
namespace CameraStandard {
struct HCameraHostManager::CameraDeviceInfo {
    std::string cameraId;
    std::shared_ptr<OHOS::Camera::CameraMetadata> ability;
    std::mutex mutex;

    explicit CameraDeviceInfo(const std::string& cameraId, sptr<ICameraDevice> device = nullptr)
        : cameraId(cameraId), ability(nullptr)
    {
    }

    ~CameraDeviceInfo() = default;
};

class HCameraHostManager::CameraHostInfo : public ICameraHostCallback {
public:
    class CameraHostDeathRecipient : public IRemoteObject::DeathRecipient {
    public:
        explicit CameraHostDeathRecipient(const sptr<HCameraHostManager::CameraHostInfo> &hostInfo)
            : cameraHostInfo_(hostInfo) {};
        virtual ~CameraHostDeathRecipient() = default;
        void OnRemoteDied(const wptr<IRemoteObject> &remote) override
        {
            MEDIA_ERR_LOG("Remote died, do clean works.");
            if (cameraHostInfo_ == nullptr) {
                return;
            }
            cameraHostInfo_->CameraHostDied();
        }

    private:
        sptr<HCameraHostManager::CameraHostInfo> cameraHostInfo_;
    };

    explicit CameraHostInfo(HCameraHostManager* cameraHostManager, std::string name);
    ~CameraHostInfo();
    bool Init();
    void CameraHostDied();
    bool IsCameraSupported(const std::string& cameraId);
    const std::string& GetName();
    int32_t GetCameras(std::vector<std::string>& cameraIds);
    int32_t GetCameraAbility(std::string& cameraId, std::shared_ptr<OHOS::Camera::CameraMetadata>& ability);
    int32_t OpenCamera(std::string& cameraId, const sptr<ICameraDeviceCallback>& callback,
                       sptr<ICameraDevice>& pDevice);
    int32_t SetFlashlight(const std::string& cameraId, bool isEnable);

    // CameraHostCallbackStub
    int32_t OnCameraStatus(const std::string& cameraId, HDI::Camera::V1_0::CameraStatus status) override;
    int32_t OnFlashlightStatus(const std::string& cameraId, FlashlightStatus status) override;
    int32_t OnCameraEvent(const std::string &cameraId, CameraEvent event) override;

private:
    std::shared_ptr<CameraDeviceInfo> FindCameraDeviceInfo(const std::string& cameraId);
    void AddDevice(const std::string& cameraId);
    void RemoveDevice(const std::string& cameraId);

    HCameraHostManager* cameraHostManager_;
    std::string name_;
    sptr<ICameraHost> cameraHostProxy_;

    std::mutex mutex_;
    std::vector<std::string> cameraIds_;
    std::vector<std::shared_ptr<CameraDeviceInfo>> devices_;
};

HCameraHostManager::CameraHostInfo::CameraHostInfo(HCameraHostManager* cameraHostManager, std::string name)
    : cameraHostManager_(cameraHostManager), name_(std::move(name)), cameraHostProxy_(nullptr)
{
}

HCameraHostManager::CameraHostInfo::~CameraHostInfo()
{
    std::lock_guard<std::mutex> lock(mutex_);
    MEDIA_INFO_LOG("CameraHostInfo ~CameraHostInfo");
    cameraHostManager_ = nullptr;
    cameraHostProxy_ = nullptr;
    for (unsigned i = 0; i < devices_.size(); i++) {
        devices_.at(i) = nullptr;
    }
    cameraIds_.clear();
    devices_.clear();
}

bool HCameraHostManager::CameraHostInfo::Init()
{
    if (cameraHostProxy_ != nullptr) {
        MEDIA_ERR_LOG("CameraHostInfo::Init, no camera host proxy");
        return true;
    }
    cameraHostProxy_ = ICameraHost::Get(name_.c_str(), false);
    if (cameraHostProxy_ == nullptr) {
        MEDIA_ERR_LOG("Failed to get ICameraHost");
        return false;
    }
    cameraHostProxy_->SetCallback(this);
    sptr<CameraHostDeathRecipient> cameraHostDeathRecipient = new CameraHostDeathRecipient(this);
    const sptr<IRemoteObject> &remote = OHOS::HDI::hdi_objcast<ICameraHost>(cameraHostProxy_);
    bool result = remote->AddDeathRecipient(cameraHostDeathRecipient);
    if (!result) {
        MEDIA_ERR_LOG("AddDeathRecipient for CameraHost failed.");
    }

    std::lock_guard<std::mutex> lock(mutex_);
    CamRetCode ret = (CamRetCode)(cameraHostProxy_->GetCameraIds(cameraIds_));
    if (ret != HDI::Camera::V1_0::NO_ERROR) {
        MEDIA_ERR_LOG("Init, GetCameraIds failed, ret = %{public}d", ret);
        return false;
    }
    for (const auto& cameraId : cameraIds_) {
        devices_.push_back(std::make_shared<HCameraHostManager::CameraDeviceInfo>(cameraId));
    }
    return true;
}

void HCameraHostManager::CameraHostInfo::CameraHostDied()
{
    if (cameraHostManager_ == nullptr) {
        MEDIA_ERR_LOG("CameraHostInfo::cameraHostManager is null.");
        return;
    }
    cameraHostManager_->RemoveCameraHost(name_);
}

bool HCameraHostManager::CameraHostInfo::IsCameraSupported(const std::string& cameraId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    return std::any_of(cameraIds_.begin(), cameraIds_.end(),
                       [&cameraId](const auto& camId) { return camId == cameraId; });
}

const std::string& HCameraHostManager::CameraHostInfo::GetName()
{
    return name_;
}

int32_t HCameraHostManager::CameraHostInfo::GetCameras(std::vector<std::string>& cameraIds)
{
    std::lock_guard<std::mutex> lock(mutex_);
    cameraIds.insert(cameraIds.end(), cameraIds_.begin(), cameraIds_.end());
    return CAMERA_OK;
}

int32_t HCameraHostManager::CameraHostInfo::GetCameraAbility(std::string& cameraId,
    std::shared_ptr<OHOS::Camera::CameraMetadata>& ability)
{
    auto deviceInfo = FindCameraDeviceInfo(cameraId);
    if (deviceInfo == nullptr) {
        MEDIA_ERR_LOG("CameraHostInfo::GetCameraAbility deviceInfo is null");
        return CAMERA_UNKNOWN_ERROR;
    }

    std::lock_guard<std::mutex> lock(deviceInfo->mutex);
    if (deviceInfo->ability) {
        ability = deviceInfo->ability;
    } else {
        if (cameraHostProxy_ == nullptr) {
            MEDIA_ERR_LOG("CameraHostInfo::GetCameraAbility cameraHostProxy_ is null");
            return CAMERA_UNKNOWN_ERROR;
        }
        if (!deviceInfo->ability) {
            std::vector<uint8_t> cameraAbility;
            CamRetCode rc = (CamRetCode)(cameraHostProxy_->GetCameraAbility(cameraId, cameraAbility));
            if (rc != HDI::Camera::V1_0::NO_ERROR) {
                MEDIA_ERR_LOG("CameraHostInfo::GetCameraAbility failed with error Code:%{public}d", rc);
                return HdiToServiceError(rc);
            }
            OHOS::Camera::MetadataUtils::ConvertVecToMetadata(cameraAbility, ability);
            deviceInfo->ability = ability;
        }
    }
    return CAMERA_OK;
}

int32_t HCameraHostManager::CameraHostInfo::OpenCamera(std::string& cameraId,
    const sptr<ICameraDeviceCallback>& callback,
    sptr<ICameraDevice>& pDevice)
{
    MEDIA_INFO_LOG("CameraHostInfo::OpenCamera %{public}s", cameraId.c_str());
    auto deviceInfo = FindCameraDeviceInfo(cameraId);
    if (deviceInfo == nullptr) {
        MEDIA_ERR_LOG("CameraHostInfo::GetCameraAbility deviceInfo is null");
        return CAMERA_UNKNOWN_ERROR;
    }

    std::lock_guard<std::mutex> lock(deviceInfo->mutex);
    if (cameraHostProxy_ == nullptr) {
        MEDIA_ERR_LOG("CameraHostInfo::OpenCamera cameraHostProxy_ is null");
        return CAMERA_UNKNOWN_ERROR;
    }
    CamRetCode rc = (CamRetCode)(cameraHostProxy_->OpenCamera(cameraId, callback, pDevice));
    if (rc != HDI::Camera::V1_0::NO_ERROR) {
        MEDIA_ERR_LOG("CameraHostInfo::OpenCamera failed with error Code:%{public}d", rc);
        return HdiToServiceError(rc);
    }
    return CAMERA_OK;
}

int32_t HCameraHostManager::CameraHostInfo::SetFlashlight(const std::string& cameraId, bool isEnable)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (cameraHostProxy_ == nullptr) {
        MEDIA_ERR_LOG("CameraHostInfo::SetFlashlight cameraHostProxy_ is null");
        return CAMERA_UNKNOWN_ERROR;
    }
    CamRetCode rc = (CamRetCode)(cameraHostProxy_->SetFlashlight(cameraId, isEnable));
    if (rc != HDI::Camera::V1_0::NO_ERROR) {
        MEDIA_ERR_LOG("CameraHostInfo::SetFlashlight failed with error Code:%{public}d", rc);
        return HdiToServiceError(rc);
    }
    return CAMERA_OK;
}

int32_t HCameraHostManager::CameraHostInfo::OnCameraStatus(const std::string& cameraId,
                                                           HDI::Camera::V1_0::CameraStatus status)
{
    if ((cameraHostManager_ == nullptr) || (cameraHostManager_->statusCallback_ == nullptr)) {
        MEDIA_WARNING_LOG("CameraHostInfo::OnCameraStatus for %{public}s with status %{public}d "
                          "failed due to no callback",
                          cameraId.c_str(), status);
        return CAMERA_UNKNOWN_ERROR;
    }
    CameraStatus svcStatus = CAMERA_STATUS_UNAVAILABLE;
    switch (status) {
        case UN_AVAILABLE: {
            MEDIA_INFO_LOG("CameraHostInfo::OnCameraStatus, camera %{public}s unavailable", cameraId.c_str());
            svcStatus = CAMERA_STATUS_UNAVAILABLE;
            break;
        }
        case AVAILABLE: {
            MEDIA_INFO_LOG("CameraHostInfo::OnCameraStatus, camera %{public}s available", cameraId.c_str());
            svcStatus = CAMERA_STATUS_AVAILABLE;
            AddDevice(cameraId);
            break;
        }
        default:
            MEDIA_ERR_LOG("Unknown camera status: %{public}d", status);
            return CAMERA_UNKNOWN_ERROR;
    }
    cameraHostManager_->statusCallback_->OnCameraStatus(cameraId, svcStatus);
    return CAMERA_OK;
}

int32_t HCameraHostManager::CameraHostInfo::OnFlashlightStatus(const std::string& cameraId,
    FlashlightStatus status)
{
    if ((cameraHostManager_ == nullptr) || (cameraHostManager_->statusCallback_ == nullptr)) {
        MEDIA_WARNING_LOG("CameraHostInfo::OnFlashlightStatus for %{public}s with status %{public}d "
                          "failed due to no callback or cameraHostManager_ is null",
                          cameraId.c_str(), status);
        return CAMERA_UNKNOWN_ERROR;
    }
    FlashStatus flashStatus = FLASH_STATUS_OFF;
    switch (status) {
        case FLASHLIGHT_OFF:
            flashStatus = FLASH_STATUS_OFF;
            MEDIA_INFO_LOG("CameraHostInfo::OnFlashlightStatus, camera %{public}s flash light is off",
                           cameraId.c_str());
            break;

        case FLASHLIGHT_ON:
            flashStatus = FLASH_STATUS_ON;
            MEDIA_INFO_LOG("CameraHostInfo::OnFlashlightStatus, camera %{public}s flash light is on",
                           cameraId.c_str());
            break;

        case FLASHLIGHT_UNAVAILABLE:
            flashStatus = FLASH_STATUS_UNAVAILABLE;
            MEDIA_INFO_LOG("CameraHostInfo::OnFlashlightStatus, camera %{public}s flash light is unavailable",
                           cameraId.c_str());
            break;

        default:
            MEDIA_ERR_LOG("Unknown flashlight status: %{public}d for camera %{public}s", status, cameraId.c_str());
            return CAMERA_UNKNOWN_ERROR;
    }
    cameraHostManager_->statusCallback_->OnFlashlightStatus(cameraId, flashStatus);
    return CAMERA_OK;
}

int32_t HCameraHostManager::CameraHostInfo::OnCameraEvent(const std::string &cameraId, CameraEvent event)
{
    if ((cameraHostManager_ == nullptr) || (cameraHostManager_->statusCallback_ == nullptr)) {
        MEDIA_WARNING_LOG("CameraHostInfo::OnCameraEvent for %{public}s with status %{public}d "
                          "failed due to no callback or cameraHostManager_ is null",
                          cameraId.c_str(), event);
        return CAMERA_UNKNOWN_ERROR;
    }
    CameraStatus svcStatus = CAMERA_STATUS_UNAVAILABLE;
    switch (event) {
        case CAMERA_EVENT_DEVICE_RMV: {
            MEDIA_INFO_LOG("CameraHostInfo::OnCameraEvent, camera %{public}s unavailable", cameraId.c_str());
            svcStatus = CAMERA_STATUS_UNAVAILABLE;
            RemoveDevice(cameraId);
            break;
        }
        case CAMERA_EVENT_DEVICE_ADD: {
            MEDIA_INFO_LOG("CameraHostInfo::OnCameraEvent camera %{public}s available", cameraId.c_str());
            svcStatus = CAMERA_STATUS_AVAILABLE;
            AddDevice(cameraId);
            break;
        }
        default:
            MEDIA_ERR_LOG("Unknown camera event: %{public}d", event);
            return CAMERA_UNKNOWN_ERROR;
    }
    cameraHostManager_->statusCallback_->OnCameraStatus(cameraId, svcStatus);
    return CAMERA_OK;
}

std::shared_ptr<HCameraHostManager::CameraDeviceInfo> HCameraHostManager::CameraHostInfo::FindCameraDeviceInfo
   (const std::string& cameraId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    for (const auto& deviceInfo : devices_) {
        if (deviceInfo->cameraId == cameraId) {
            MEDIA_INFO_LOG("CameraHostInfo::FindCameraDeviceInfo succeed for %{public}s", cameraId.c_str());
            return deviceInfo;
        }
    }
    MEDIA_WARNING_LOG("CameraHostInfo::FindCameraDeviceInfo failed for %{public}s", cameraId.c_str());
    return nullptr;
}

void HCameraHostManager::CameraHostInfo::AddDevice(const std::string& cameraId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    cameraIds_.push_back(cameraId);
    if (std::none_of(devices_.begin(), devices_.end(),
                     [&cameraId](auto& devInfo) { return devInfo->cameraId == cameraId; })) {
        devices_.push_back(std::make_shared<HCameraHostManager::CameraDeviceInfo>(cameraId));
        MEDIA_INFO_LOG("CameraHostInfo::AddDevice, camera %{public}s added", cameraId.c_str());
    } else {
        MEDIA_WARNING_LOG("CameraHostInfo::AddDevice, camera %{public}s already exists", cameraId.c_str());
    }
}

void HCameraHostManager::CameraHostInfo::RemoveDevice(const std::string& cameraId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    cameraIds_.erase(std::remove(cameraIds_.begin(), cameraIds_.end(), cameraId), cameraIds_.end());
    devices_.erase(std::remove_if(devices_.begin(), devices_.end(),
        [&cameraId](const auto& devInfo) { return devInfo->cameraId == cameraId; }),
        devices_.end());
}

HCameraHostManager::HCameraHostManager(StatusCallback* statusCallback)
    : statusCallback_(statusCallback), cameraHostInfos_()
{
}

HCameraHostManager::~HCameraHostManager()
{
    std::lock_guard<std::mutex> lock(deviceMutex_);
    statusCallback_ = nullptr;
    for (auto it = cameraDevices_.begin(); it != cameraDevices_.end(); it++) {
        if (it->second) {
            it->second = nullptr;
        }
    }
    cameraDevices_.clear();

    for (unsigned i = 0; i < cameraHostInfos_.size(); i++) {
        cameraHostInfos_[i] = nullptr;
    }
    cameraHostInfos_.clear();
}

int32_t HCameraHostManager::Init()
{
    MEDIA_INFO_LOG("HCameraHostManager::Init");
    using namespace OHOS::HDI::ServiceManager::V1_0;
    auto svcMgr = IServiceManager::Get();
    if (svcMgr == nullptr) {
        MEDIA_ERR_LOG("%s: IServiceManager failed!", __func__);
        return CAMERA_UNKNOWN_ERROR;
    }
    auto rt = svcMgr->RegisterServiceStatusListener(this, DEVICE_CLASS_CAMERA);
    if (rt != 0) {
        MEDIA_ERR_LOG("%s: RegisterServiceStatusListener failed!", __func__);
    }
    return rt == 0 ? CAMERA_OK : CAMERA_UNKNOWN_ERROR;
}

void HCameraHostManager::DeInit()
{
    using namespace OHOS::HDI::ServiceManager::V1_0;
    auto svcMgr = IServiceManager::Get();
    if (svcMgr == nullptr) {
        MEDIA_ERR_LOG("%s: IServiceManager failed", __func__);
        return;
    }
    auto rt = svcMgr->UnregisterServiceStatusListener(this);
    if (rt != 0) {
        MEDIA_ERR_LOG("%s: UnregisterServiceStatusListener failed!", __func__);
    }
}

void HCameraHostManager::AddCameraDevice(const std::string& cameraId, sptr<ICameraDeviceService> cameraDevice)
{
    std::lock_guard<std::mutex> lock(deviceMutex_);
    auto it = cameraDevices_.find(cameraId);
    if (it != cameraDevices_.end()) {
        it->second = nullptr;
        cameraDevices_.erase(cameraId);
    }
    cameraDevices_[cameraId] = cameraDevice;
}

void HCameraHostManager::RemoveCameraDevice(const std::string& cameraId)
{
    std::lock_guard<std::mutex> lock(deviceMutex_);
    auto it = cameraDevices_.find(cameraId);
    if (it != cameraDevices_.end()) {
        it->second = nullptr;
    }
    cameraDevices_.erase(cameraId);
}

void HCameraHostManager::CloseCameraDevice(const std::string& cameraId)
{
    sptr<ICameraDeviceService> deviceToDisconnect = nullptr;
    {
        std::lock_guard<std::mutex> lock(deviceMutex_);
        auto iter = cameraDevices_.find(cameraId);
        if (iter != cameraDevices_.end()) {
            deviceToDisconnect = iter->second;
        }
    }
    if (deviceToDisconnect) {
        deviceToDisconnect->Close();
    }
}

int32_t HCameraHostManager::GetCameras(std::vector<std::string>& cameraIds)
{
    CAMERA_SYNC_TRACE;
    MEDIA_INFO_LOG("HCameraHostManager::GetCameras");
    if (!IsCameraHostInfoAdded("camera_service")) {
        AddCameraHost("camera_service");
    }
    std::lock_guard<std::mutex> lock(mutex_);
    cameraIds.clear();
    for (const auto& cameraHost : cameraHostInfos_) {
        cameraHost->GetCameras(cameraIds);
    }
    return CAMERA_OK;
}

int32_t HCameraHostManager::GetCameraAbility(std::string &cameraId,
                                             std::shared_ptr<OHOS::Camera::CameraMetadata> &ability)
{
    auto cameraHostInfo = FindCameraHostInfo(cameraId);
    if (cameraHostInfo == nullptr) {
        MEDIA_ERR_LOG("HCameraHostManager::OpenCameraDevice failed with invalid device info.");
        return CAMERA_INVALID_ARG;
    }
    return cameraHostInfo->GetCameraAbility(cameraId, ability);
}

std::vector<sptr<ICameraDeviceService>> HCameraHostManager::CameraConflictDetection(const std::string& cameraId)
{
    std::vector<sptr<ICameraDeviceService>> devicesNeedClose;
    {
        std::lock_guard<std::mutex> lock(deviceMutex_);
        for (auto it = cameraDevices_.begin(); it != cameraDevices_.end(); it++) {
            if (it->second != nullptr) {
                if (it->first == cameraId) {
                    MEDIA_INFO_LOG("HCameraHostManager::CameraConflictDetection current camera [%{public}s] has Opened,"
                                   "need close", it->first.c_str());
                } else {
                    MEDIA_INFO_LOG("HCameraHostManager::CameraConflictDetection other camera [%{public}s] has Opened,"
                                   "need close", it->first.c_str());
                }
                devicesNeedClose.push_back(it->second);
            } else {
                MEDIA_ERR_LOG("HCameraHostManager::CameraConflictDetection cameraDevice [%{public}s] is null",
                              it->first.c_str());
            }
        }
    }
    return devicesNeedClose;
}

int32_t HCameraHostManager::OpenCameraDevice(std::string &cameraId,
                                             const sptr<ICameraDeviceCallback> &callback,
                                             sptr<ICameraDevice> &pDevice)
{
    MEDIA_INFO_LOG("HCameraHostManager::OpenCameraDevice try to open camera = %{public}s", cameraId.c_str());
    auto cameraHostInfo = FindCameraHostInfo(cameraId);
    if (cameraHostInfo == nullptr) {
        MEDIA_ERR_LOG("HCameraHostManager::OpenCameraDevice failed with invalid device info");
        return CAMERA_INVALID_ARG;
    }
    return cameraHostInfo->OpenCamera(cameraId, callback, pDevice);
}

int32_t HCameraHostManager::SetFlashlight(const std::string& cameraId, bool isEnable)
{
    auto cameraHostInfo = FindCameraHostInfo(cameraId);
    if (cameraHostInfo == nullptr) {
        MEDIA_ERR_LOG("HCameraHostManager::OpenCameraDevice failed with invalid device info");
        return CAMERA_INVALID_ARG;
    }
    return cameraHostInfo->SetFlashlight(cameraId, isEnable);
}

void HCameraHostManager::OnReceive(const HDI::ServiceManager::V1_0::ServiceStatus& status)
{
    MEDIA_INFO_LOG("HCameraHostManager::OnReceive for camera host %{public}s, status %{public}d",
        status.serviceName.c_str(), status.status);
    if (status.deviceClass != DEVICE_CLASS_CAMERA || status.serviceName != "distributed_camera_service") {
        MEDIA_ERR_LOG("HCameraHostManager::OnReceive invalid device class %{public}d", status.deviceClass);
        return;
    }
    using namespace OHOS::HDI::ServiceManager::V1_0;
    switch (status.status) {
        case SERVIE_STATUS_START:
            AddCameraHost(status.serviceName);
            break;
        case SERVIE_STATUS_STOP:
            RemoveCameraHost(status.serviceName);
            break;
        default:
            MEDIA_ERR_LOG("HCameraHostManager::OnReceive unexpected service status %{public}d", status.status);
    }
}

void HCameraHostManager::AddCameraHost(const std::string& svcName)
{
    MEDIA_INFO_LOG("HCameraHostManager::AddCameraHost camera host %{public}s added", svcName.c_str());
    std::lock_guard<std::mutex> lock(mutex_);
    if (std::any_of(cameraHostInfos_.begin(), cameraHostInfos_.end(),
                    [&svcName](const auto& camHost) { return camHost->GetName() == svcName; })) {
        MEDIA_INFO_LOG("HCameraHostManager::AddCameraHost camera host  %{public}s already exists", svcName.c_str());
        return;
    }
    sptr<HCameraHostManager::CameraHostInfo> cameraHost = new(std::nothrow) HCameraHostManager::CameraHostInfo
                                                          (this, svcName);
    if (!cameraHost->Init()) {
        MEDIA_ERR_LOG("HCameraHostManager::AddCameraHost failed due to init failure");
        return;
    }
    cameraHostInfos_.push_back(cameraHost);
    std::vector<std::string> cameraIds;
    if (statusCallback_ && cameraHost->GetCameras(cameraIds) == CAMERA_OK) {
        for (const auto& cameraId : cameraIds) {
            statusCallback_->OnCameraStatus(cameraId, CAMERA_STATUS_AVAILABLE);
        }
    }
}

void HCameraHostManager::RemoveCameraHost(const std::string& svcName)
{
    MEDIA_INFO_LOG("HCameraHostManager::RemoveCameraHost camera host %{public}s removed", svcName.c_str());
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = std::find_if(cameraHostInfos_.begin(), cameraHostInfos_.end(),
                           [&svcName](const auto& camHost) { return camHost->GetName() == svcName; });
    if (it == cameraHostInfos_.end()) {
        MEDIA_WARNING_LOG("HCameraHostManager::RemoveCameraHost camera host %{public}s doesn't exist", svcName.c_str());
        return;
    }
    std::vector<std::string> cameraIds;
    if ((*it)->GetCameras(cameraIds) == CAMERA_OK) {
        for (const auto& cameraId : cameraIds) {
            (*it)->OnCameraStatus(cameraId, UN_AVAILABLE);
            CloseCameraDevice(cameraId);
        }
    }
    *it = nullptr;
    cameraHostInfos_.erase(it);
}

sptr<HCameraHostManager::CameraHostInfo> HCameraHostManager::FindCameraHostInfo(const std::string& cameraId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    for (const auto& cameraHostInfo : cameraHostInfos_) {
        if (cameraHostInfo->IsCameraSupported(cameraId)) {
            return cameraHostInfo;
        }
    }
    return nullptr;
}

bool HCameraHostManager::IsCameraHostInfoAdded(const std::string& svcName)
{
    return std::any_of(cameraHostInfos_.begin(), cameraHostInfos_.end(),
                       [&svcName](const auto& camHost) {return camHost->GetName() == svcName; });
}
} // namespace CameraStandard
} // namespace OHOS
