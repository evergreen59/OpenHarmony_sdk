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

#include "free_install_manager.h"

#include <chrono>

#include "ability_info.h"
#include "ability_manager_errors.h"
#include "ability_manager_service.h"
#include "ability_util.h"
#include "atomic_service_status_callback.h"
#include "distributed_client.h"
#include "hilog_wrapper.h"
#include "in_process_call_wrapper.h"

namespace OHOS {
namespace AAFwk {
const std::u16string DMS_FREE_INSTALL_CALLBACK_TOKEN = u"ohos.DistributedSchedule.IDmsFreeInstallCallback";
const std::string DMS_MISSION_ID = "dmsMissionId";
const std::string PARAM_FREEINSTALL_APPID = "ohos.freeinstall.params.callingAppId";
const std::string PARAM_FREEINSTALL_BUNDLENAMES = "ohos.freeinstall.params.callingBundleNames";
const std::string PARAM_FREEINSTALL_UID = "ohos.freeinstall.params.callingUid";
constexpr uint32_t IDMS_CALLBACK_ON_FREE_INSTALL_DONE = 0;
constexpr uint32_t UPDATE_ATOMOIC_SERVICE_TASK_TIMER = 24 * 60 * 60 * 1000; /* 24h */

FreeInstallManager::FreeInstallManager(const std::weak_ptr<AbilityManagerService> &server)
    : server_(server)
{
}

bool FreeInstallManager::IsTopAbility(const sptr<IRemoteObject> &callerToken)
{
    HILOG_INFO("%{public}s", __func__);
    auto server = server_.lock();
    CHECK_POINTER_AND_RETURN_LOG(server, false, "Get server failed!");
    AppExecFwk::ElementName elementName = server->GetTopAbility();
    if (elementName.GetBundleName().empty() || elementName.GetAbilityName().empty()) {
        HILOG_ERROR("GetBundleName or GetAbilityName empty!");
        return false;
    }

    auto caller = Token::GetAbilityRecordByToken(callerToken);
    if (caller == nullptr) {
        HILOG_ERROR("Caller is null!");
        return false;
    }

    auto type = caller->GetAbilityInfo().type;
    if (type == AppExecFwk::AbilityType::SERVICE || type == AppExecFwk::AbilityType::EXTENSION) {
        HILOG_INFO("The ability is service or extension ability.");
        return true;
    }

    AppExecFwk::ElementName callerElementName = caller->GetWant().GetElement();
    std::string callerBundleName = callerElementName.GetBundleName();
    std::string callerAbilityName = callerElementName.GetAbilityName();
    std::string callerModuleName = callerElementName.GetModuleName();
    if (elementName.GetBundleName().compare(callerBundleName) == 0 &&
        elementName.GetAbilityName().compare(callerAbilityName) == 0 &&
        elementName.GetModuleName().compare(callerModuleName) == 0) {
        HILOG_INFO("The ability is top ability.");
        return true;
    }

    return false;
}

int FreeInstallManager::StartFreeInstall(const Want &want, int32_t userId, int requestCode,
    const sptr<IRemoteObject> &callerToken)
{
    HILOG_INFO("StartFreeInstall called");
    auto isSaCall = AAFwk::PermissionVerification::GetInstance()->IsSACall();
    if (!isSaCall && !IsTopAbility(callerToken)) {
        return NOT_TOP_ABILITY;
    }
    FreeInstallInfo info = BuildFreeInstallInfo(want, userId, requestCode, callerToken);
    {
        std::lock_guard<std::mutex> lock(freeInstallListLock_);
        freeInstallList_.push_back(info);
    }
    sptr<AtomicServiceStatusCallback> callback = new AtomicServiceStatusCallback(weak_from_this(),
        info.startInstallTime);
    auto bms = AbilityUtil::GetBundleManager();
    CHECK_POINTER_AND_RETURN(bms, GET_ABILITY_SERVICE_FAILED);
    AppExecFwk::AbilityInfo abilityInfo = {};
    constexpr auto flag = AppExecFwk::AbilityInfoFlag::GET_ABILITY_INFO_WITH_APPLICATION;
    info.want.SetParam(PARAM_FREEINSTALL_UID, IPCSkeleton::GetCallingUid());
    if (IN_PROCESS_CALL(bms->QueryAbilityInfo(info.want, flag, info.userId, abilityInfo, callback))) {
        HILOG_INFO("The app has installed.");
    }
    std::string callingAppId = info.want.GetStringParam(PARAM_FREEINSTALL_APPID);
    std::vector<std::string> callingBundleNames = info.want.GetStringArrayParam(PARAM_FREEINSTALL_BUNDLENAMES);
    if (callingAppId.empty() && callingBundleNames.empty()) {
        HILOG_INFO("callingAppId and callingBundleNames are empty");
    }
    info.want.RemoveParam(PARAM_FREEINSTALL_APPID);
    info.want.RemoveParam(PARAM_FREEINSTALL_BUNDLENAMES);
    auto future = info.promise->get_future();
    std::future_status status = future.wait_for(std::chrono::milliseconds(DELAY_LOCAL_FREE_INSTALL_TIMEOUT));
    if (status == std::future_status::timeout) {
        info.isInstalled = true;
        return FREE_INSTALL_TIMEOUT;
    }
    return future.get();
}

int FreeInstallManager::RemoteFreeInstall(const Want &want, int32_t userId, int requestCode,
    const sptr<IRemoteObject> &callerToken)
{
    HILOG_INFO("RemoteFreeInstall called");
    bool isFromRemote = want.GetBoolParam(FROM_REMOTE_KEY, false);
    auto isSaCall = AAFwk::PermissionVerification::GetInstance()->IsSACall();
    if (!isSaCall && !isFromRemote && !IsTopAbility(callerToken)) {
        return NOT_TOP_ABILITY;
    }
    FreeInstallInfo info = BuildFreeInstallInfo(want, userId, requestCode, callerToken);
    {
        std::lock_guard<std::mutex> lock(freeInstallListLock_);
        freeInstallList_.push_back(info);
    }
    sptr<AtomicServiceStatusCallback> callback = new AtomicServiceStatusCallback(weak_from_this(),
        info.startInstallTime);
    int32_t callerUid = IPCSkeleton::GetCallingUid();
    uint32_t accessToken = IPCSkeleton::GetCallingTokenID();
    DistributedClient dmsClient;
    auto result = dmsClient.StartRemoteFreeInstall(info.want, callerUid, info.requestCode, accessToken, callback);
    if (result != ERR_NONE) {
        return result;
    }
    auto remoteFuture = info.promise->get_future();
    std::future_status remoteStatus = remoteFuture.wait_for(std::chrono::milliseconds(
        DELAY_REMOTE_FREE_INSTALL_TIMEOUT));
    if (remoteStatus == std::future_status::timeout) {
        return FREE_INSTALL_TIMEOUT;
    }
    return remoteFuture.get();
}

FreeInstallInfo FreeInstallManager::BuildFreeInstallInfo(const Want &want, int32_t userId, int requestCode,
    const sptr<IRemoteObject> &callerToken)
{
    auto promise = std::make_shared<std::promise<int32_t>>();
    FreeInstallInfo info = {
        .want = want,
        .userId = userId,
        .requestCode = requestCode,
        .startInstallTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::
        system_clock::now().time_since_epoch()).count(),
        .promise = promise,
        .callerToken = callerToken
    };
    return info;
}

int FreeInstallManager::StartRemoteFreeInstall(const Want &want, int requestCode, int32_t validUserId,
    const sptr<IRemoteObject> &callerToken)
{
    HILOG_INFO("%{public}s", __func__);
    if (!want.GetBoolParam(Want::PARAM_RESV_FOR_RESULT, false)) {
        HILOG_INFO("%{public}s: StartAbility freeInstall", __func__);
        return RemoteFreeInstall(want, validUserId, requestCode, callerToken);
    }
    int32_t missionId = DelayedSingleton<AbilityManagerService>::GetInstance()->
        GetMissionIdByAbilityToken(callerToken);
    if (missionId < 0) {
        return ERR_INVALID_VALUE;
    }
    Want* newWant = const_cast<Want*>(&want);
    newWant->SetParam(DMS_MISSION_ID, missionId);
    HILOG_INFO("%{public}s: StartAbilityForResult freeInstall", __func__);
    return RemoteFreeInstall(*newWant, validUserId, requestCode, callerToken);
}

int FreeInstallManager::NotifyDmsCallback(const Want &want, int resultCode)
{
    std::lock_guard<std::mutex> autoLock(distributedFreeInstallLock_);
    if (dmsFreeInstallCbs_.empty()) {
        HILOG_ERROR("Has no dms callback.");
        return ERR_INVALID_VALUE;
    }

    MessageParcel reply;
    MessageOption option;

    for (auto it = dmsFreeInstallCbs_.begin(); it != dmsFreeInstallCbs_.end();) {
        std::string abilityName = (*it).want.GetElement().GetAbilityName();
        if (want.GetElement().GetAbilityName().compare(abilityName) == 0) {
            HILOG_INFO("Handle DMS.");
            MessageParcel data;
            if (!data.WriteInterfaceToken(DMS_FREE_INSTALL_CALLBACK_TOKEN)) {
                HILOG_ERROR("Write interface token failed.");
                return ERR_INVALID_VALUE;
            }

            if (!data.WriteInt32(resultCode)) {
                HILOG_ERROR("Write resultCode error.");
                return ERR_INVALID_VALUE;
            }

            if (!data.WriteParcelable(&((*it).want))) {
                HILOG_ERROR("want write failed.");
                return INNER_ERR;
            }

            if (!data.WriteInt32((*it).requestCode)) {
                HILOG_ERROR("Write resultCode error.");
                return ERR_INVALID_VALUE;
            }

            (*it).dmsCallback->SendRequest(IDMS_CALLBACK_ON_FREE_INSTALL_DONE, data, reply, option);
            it = dmsFreeInstallCbs_.erase(it);
        } else {
            it++;
        }
    }

    return reply.ReadInt32();
}

void FreeInstallManager::NotifyFreeInstallResult(const Want &want, int resultCode, int64_t startInstallTime)
{
    std::lock_guard<std::mutex> lock(freeInstallListLock_);
    if (freeInstallList_.empty()) {
        HILOG_INFO("Has no app callback.");
        return;
    }

    bool isFromRemote = want.GetBoolParam(FROM_REMOTE_KEY, false);
    HILOG_INFO("isFromRemote = %{public}d", isFromRemote);
    for (auto it = freeInstallList_.begin(); it != freeInstallList_.end();) {
        std::string bundleName = (*it).want.GetElement().GetBundleName();
        std::string abilityName = (*it).want.GetElement().GetAbilityName();
        if (want.GetElement().GetBundleName().compare(bundleName) != 0 ||
            want.GetElement().GetAbilityName().compare(abilityName) != 0 ||
            (*it).startInstallTime != startInstallTime) {
            it++;
            continue;
        }

        if ((*it).isInstalled) {
            it = freeInstallList_.erase(it);
            continue;
        }

        if ((*it).promise == nullptr) {
            it++;
            continue;
        }
        
        if (resultCode == ERR_OK) {
            HILOG_INFO("FreeInstall success.");
            (*it).promise->set_value(resultCode);
            (*it).isInstalled = true;
        } else {
            HILOG_INFO("FreeInstall failed.");
            (*it).promise->set_value(resultCode);
        }

        it++;
    }
}

int FreeInstallManager::FreeInstallAbilityFromRemote(const Want &want, const sptr<IRemoteObject> &callback,
    int32_t userId, int requestCode)
{
    HILOG_INFO("%{public}s", __func__);
    if (callback == nullptr) {
        HILOG_ERROR("FreeInstallAbilityFromRemote callback is nullptr.");
        return ERR_INVALID_VALUE;
    }

    FreeInstallInfo info = {
        .want = want,
        .userId = userId,
        .requestCode = requestCode,
        .dmsCallback = callback
    };

    {
        std::lock_guard<std::mutex> autoLock(distributedFreeInstallLock_);
        dmsFreeInstallCbs_.push_back(info);
    }

    auto freeInstallTask = [manager = shared_from_this(), info]() {
        auto result = manager->StartFreeInstall(info.want, info.userId, info.requestCode, nullptr);
        if (result != ERR_OK) {
            manager->NotifyDmsCallback(info.want, result);
        }
    };

    std::shared_ptr<AbilityEventHandler> handler =
        DelayedSingleton<AbilityManagerService>::GetInstance()->GetEventHandler();
    CHECK_POINTER_AND_RETURN_LOG(handler, ERR_INVALID_VALUE, "Fail to get AbilityEventHandler.");

    handler->PostTask(freeInstallTask, "FreeInstallAbilityFromRemote");
    return ERR_OK;
}

int FreeInstallManager::ConnectFreeInstall(const Want &want, int32_t userId,
    const sptr<IRemoteObject> &callerToken, const std::string& localDeviceId)
{
    auto bms = AbilityUtil::GetBundleManager();
    CHECK_POINTER_AND_RETURN(bms, GET_ABILITY_SERVICE_FAILED);
    std::string wantDeviceId = want.GetElement().GetDeviceID();
    if (!(localDeviceId == wantDeviceId || wantDeviceId.empty())) {
        HILOG_ERROR("AbilityManagerService::ConnectFreeInstall. wantDeviceId error");
        return ERR_INVALID_VALUE;
    }

    auto isSaCall = AAFwk::PermissionVerification::GetInstance()->IsSACall();
    if (!isSaCall) {
        std::string wantAbilityName = want.GetElement().GetAbilityName();
        std::string wantBundleName = want.GetElement().GetBundleName();
        if (wantBundleName.empty() || wantAbilityName.empty()) {
            HILOG_ERROR("AbilityManagerService::ConnectFreeInstall. wantBundleName or wantAbilityName is empty");
            return ERR_INVALID_VALUE;
        }
        int callerUid = IPCSkeleton::GetCallingUid();
        std::string localBundleName;
        bms->GetBundleNameForUid(callerUid, localBundleName);
        if (localBundleName != wantBundleName) {
            HILOG_ERROR("AbilityManagerService::ConnectFreeInstall. wantBundleName is not local BundleName");
            return ERR_INVALID_VALUE;
        }
    }

    AppExecFwk::AbilityInfo abilityInfo;
    std::vector<AppExecFwk::ExtensionAbilityInfo> extensionInfos;
    if (!IN_PROCESS_CALL(bms->QueryAbilityInfo(
        want, AppExecFwk::AbilityInfoFlag::GET_ABILITY_INFO_WITH_APPLICATION, userId, abilityInfo)) &&
        !IN_PROCESS_CALL(bms->QueryExtensionAbilityInfos(
            want, AppExecFwk::AbilityInfoFlag::GET_ABILITY_INFO_WITH_APPLICATION, userId, extensionInfos))) {
        HILOG_INFO("AbilityManagerService::ConnectFreeInstall. try to StartFreeInstall");
        int result = StartFreeInstall(want, userId, DEFAULT_INVAL_VALUE, callerToken);
        if (result) {
            HILOG_ERROR("AbilityManagerService::ConnectFreeInstall. StartFreeInstall error");
            return result;
        }
        HILOG_INFO("AbilityManagerService::ConnectFreeInstall. StartFreeInstall success");
    }
    return ERR_OK;
}

std::time_t FreeInstallManager::GetTimeStamp()
{
    std::chrono::time_point<std::chrono::system_clock, std::chrono::milliseconds> tp =
        std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
    std::time_t timestamp = tp.time_since_epoch().count();
    return timestamp;
}

void FreeInstallManager::OnInstallFinished(int resultCode, const Want &want, int32_t userId, int64_t startInstallTime)
{
    HILOG_INFO("%{public}s resultCode = %{public}d", __func__, resultCode);
    NotifyDmsCallback(want, resultCode);
    NotifyFreeInstallResult(want, resultCode, startInstallTime);

    std::weak_ptr<FreeInstallManager> thisWptr(shared_from_this());
    if (resultCode == ERR_OK) {
        auto updateAtmoicServiceTask = [want, userId, thisWptr, &timeStampMap = timeStampMap_]() {
            auto sptr = thisWptr.lock();
            HILOG_DEBUG("bundleName: %{public}s, moduleName: %{public}s", want.GetElement().GetBundleName().c_str(),
                want.GetElement().GetModuleName().c_str());
            std::string nameKey = want.GetElement().GetBundleName() + want.GetElement().GetModuleName();
            if (timeStampMap.find(nameKey) == timeStampMap.end() ||
                sptr->GetTimeStamp() - timeStampMap[nameKey] > UPDATE_ATOMOIC_SERVICE_TASK_TIMER) {
                auto bms = AbilityUtil::GetBundleManager();
                CHECK_POINTER(bms);
                bms->UpgradeAtomicService(want, userId);
                timeStampMap.emplace(nameKey, sptr->GetTimeStamp());
            }
        };

        std::shared_ptr<AbilityEventHandler> handler =
            DelayedSingleton<AbilityManagerService>::GetInstance()->GetEventHandler();
        CHECK_POINTER_LOG(handler, "Fail to get AbilityEventHandler.");
        handler->PostTask(updateAtmoicServiceTask, "UpdateAtmoicServiceTask");
    }
}

void FreeInstallManager::OnRemoteInstallFinished(int resultCode, const Want &want, int32_t userId,
    int64_t startInstallTime)
{
    HILOG_INFO("%{public}s resultCode = %{public}d", __func__, resultCode);
    NotifyFreeInstallResult(want, resultCode, startInstallTime);
}
}  // namespace AAFwk
}  // namespace OHOS
