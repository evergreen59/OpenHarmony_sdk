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

#include "ams_mgr_proxy.h"
#include "ipc_types.h"
#include "iremote_object.h"
#include "string_ex.h"

#include "appexecfwk_errors.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace AppExecFwk {
AmsMgrProxy::AmsMgrProxy(const sptr<IRemoteObject> &impl) : IRemoteProxy<IAmsMgr>(impl)
{}

bool AmsMgrProxy::WriteInterfaceToken(MessageParcel &data)
{
    if (!data.WriteInterfaceToken(AmsMgrProxy::GetDescriptor())) {
        HILOG_ERROR("write interface token failed");
        return false;
    }
    return true;
}

void AmsMgrProxy::LoadAbility(const sptr<IRemoteObject> &token, const sptr<IRemoteObject> &preToken,
    const std::shared_ptr<AbilityInfo> &abilityInfo, const std::shared_ptr<ApplicationInfo> &appInfo,
    const std::shared_ptr<AAFwk::Want> &want)
{
    HILOG_DEBUG("start");
    if (!abilityInfo || !appInfo) {
        HILOG_ERROR("param error");
        return;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!WriteInterfaceToken(data)) {
        return;
    }

    if (token) {
        if (!data.WriteBool(true) || !data.WriteRemoteObject(token)) {
            HILOG_ERROR("Failed to write flag and token");
            return;
        }
    } else {
        if (!data.WriteBool(false)) {
            HILOG_ERROR("Failed to write flag");
            return;
        }
    }

    if (preToken) {
        if (!data.WriteBool(true) || !data.WriteRemoteObject(preToken)) {
            HILOG_ERROR("Failed to write flag and preToken");
            return;
        }
    } else {
        if (!data.WriteBool(false)) {
            HILOG_ERROR("Failed to write flag");
            return;
        }
    }

    data.WriteParcelable(abilityInfo.get());
    data.WriteParcelable(appInfo.get());
    if (!data.WriteParcelable(want.get())) {
        HILOG_ERROR("Write data want failed.");
        return;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        HILOG_ERROR("Remote() is NULL");
        return;
    }
    int32_t ret = remote->SendRequest(static_cast<uint32_t>(IAmsMgr::Message::LOAD_ABILITY), data, reply, option);
    if (ret != NO_ERROR) {
        HILOG_WARN("SendRequest is failed, error code: %{public}d", ret);
    }
    HILOG_DEBUG("end");
}

void AmsMgrProxy::TerminateAbility(const sptr<IRemoteObject> &token, bool clearMissionFlag)
{
    HILOG_DEBUG("start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!WriteInterfaceToken(data)) {
        return;
    }
    if (!data.WriteRemoteObject(token.GetRefPtr())) {
        HILOG_ERROR("Failed to write token");
        return;
    }
    if (!data.WriteBool(clearMissionFlag)) {
        HILOG_ERROR("Failed to write clearMissionFlag");
        return;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        HILOG_ERROR("Remote() is NULL");
        return;
    }
    int32_t ret =
        remote->SendRequest(static_cast<uint32_t>(IAmsMgr::Message::TERMINATE_ABILITY), data, reply, option);
    if (ret != NO_ERROR) {
        HILOG_WARN("SendRequest is failed, error code: %{public}d", ret);
    }
    HILOG_DEBUG("end");
}

void AmsMgrProxy::UpdateAbilityState(const sptr<IRemoteObject> &token, const AbilityState state)
{
    HILOG_DEBUG("start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!WriteInterfaceToken(data)) {
        return;
    }
    if (!data.WriteRemoteObject(token.GetRefPtr())) {
        HILOG_ERROR("Failed to write token");
        return;
    }
    data.WriteInt32(static_cast<int32_t>(state));
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        HILOG_ERROR("Remote() is NULL");
        return;
    }
    int32_t ret =
        remote->SendRequest(static_cast<uint32_t>(IAmsMgr::Message::UPDATE_ABILITY_STATE), data, reply, option);
    if (ret != NO_ERROR) {
        HILOG_WARN("SendRequest is failed, error code: %{public}d", ret);
    }
    HILOG_DEBUG("end");
}

void AmsMgrProxy::UpdateExtensionState(const sptr<IRemoteObject> &token, const ExtensionState state)
{
    HILOG_DEBUG("UpdateExtensionState start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!WriteInterfaceToken(data)) {
        return;
    }
    if (!data.WriteRemoteObject(token.GetRefPtr())) {
        HILOG_ERROR("Failed to write token");
        return;
    }
    data.WriteInt32(static_cast<int32_t>(state));
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        HILOG_ERROR("Remote() is NULL");
        return;
    }
    int32_t ret =
        remote->SendRequest(static_cast<uint32_t>(IAmsMgr::Message::UPDATE_EXTENSION_STATE), data, reply, option);
    if (ret != NO_ERROR) {
        HILOG_WARN("SendRequest is failed, error code: %{public}d", ret);
    }
    HILOG_DEBUG("end");
}

void AmsMgrProxy::RegisterAppStateCallback(const sptr<IAppStateCallback> &callback)
{
    HILOG_DEBUG("start");
    if (!callback) {
        HILOG_ERROR("callback is nullptr");
        return;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!WriteInterfaceToken(data)) {
        return;
    }

    if (callback->AsObject()) {
        if (!data.WriteBool(true) || !data.WriteRemoteObject(callback->AsObject())) {
            HILOG_ERROR("Failed to write flag and callback");
            return;
        }
    } else {
        if (!data.WriteBool(false)) {
            HILOG_ERROR("Failed to write flag");
            return;
        }
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        HILOG_ERROR("Remote() is nullptr");
        return;
    }
    int32_t ret = remote->SendRequest(
        static_cast<uint32_t>(IAmsMgr::Message::REGISTER_APP_STATE_CALLBACK), data, reply, option);
    if (ret != NO_ERROR) {
        HILOG_WARN("SendRequest is failed, error code: %{public}d", ret);
    }
    HILOG_DEBUG("end");
}

void AmsMgrProxy::AbilityBehaviorAnalysis(const sptr<IRemoteObject> &token, const sptr<IRemoteObject> &preToken,
    const int32_t visibility, const int32_t perceptibility, const int32_t connectionState)
{
    HILOG_DEBUG("start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!WriteInterfaceToken(data)) {
        return;
    }

    if (!data.WriteRemoteObject(token.GetRefPtr())) {
        HILOG_ERROR("Failed to write token");
        return;
    }

    if (preToken) {
        if (!data.WriteBool(true) || !data.WriteRemoteObject(preToken.GetRefPtr())) {
            HILOG_ERROR("Failed to write flag and preToken");
            return;
        }
    } else {
        if (!data.WriteBool(false)) {
            HILOG_ERROR("Failed to write flag");
            return;
        }
    }

    data.WriteInt32(static_cast<int32_t>(visibility));
    data.WriteInt32(static_cast<int32_t>(perceptibility));
    data.WriteInt32(static_cast<int32_t>(connectionState));
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        HILOG_ERROR("Remote() is NULL");
        return;
    }
    int32_t ret =
        remote->SendRequest(static_cast<uint32_t>(IAmsMgr::Message::ABILITY_BEHAVIOR_ANALYSIS), data, reply, option);
    if (ret != NO_ERROR) {
        HILOG_WARN("SendRequest is failed, error code: %{public}d", ret);
    }
    HILOG_DEBUG("end");
}

void AmsMgrProxy::KillProcessByAbilityToken(const sptr<IRemoteObject> &token)
{
    HILOG_DEBUG("start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!WriteInterfaceToken(data)) {
        return;
    }
    if (!data.WriteRemoteObject(token.GetRefPtr())) {
        HILOG_ERROR("Failed to write token");
        return;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        HILOG_ERROR("Remote() is NULL");
        return;
    }
    int32_t ret = remote->SendRequest(static_cast<uint32_t>(IAmsMgr::Message::KILL_PEOCESS_BY_ABILITY_TOKEN),
        data, reply, option);
    if (ret != NO_ERROR) {
        HILOG_WARN("SendRequest is failed, error code: %{public}d", ret);
    }
    HILOG_DEBUG("end");
}

void AmsMgrProxy::KillProcessesByUserId(int32_t userId)
{
    HILOG_DEBUG("start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!WriteInterfaceToken(data)) {
        return;
    }
    if (!data.WriteInt32(userId)) {
        HILOG_ERROR("parcel WriteInt32 failed");
        return;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        HILOG_ERROR("Remote() is NULL");
        return;
    }
    int32_t ret =
        remote->SendRequest(static_cast<uint32_t>(IAmsMgr::Message::KILL_PROCESSES_BY_USERID), data, reply, option);
    if (ret != NO_ERROR) {
        HILOG_WARN("SendRequest is failed, error code: %{public}d", ret);
    }
    HILOG_DEBUG("end");
}

int32_t AmsMgrProxy::KillProcessWithAccount(const std::string &bundleName, const int accountId)
{
    HILOG_DEBUG("start");

    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!WriteInterfaceToken(data)) {
        return ERR_INVALID_DATA;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        HILOG_ERROR("Remote() is NULL");
        return ERR_NULL_OBJECT;
    }

    if (!data.WriteString(bundleName)) {
        HILOG_ERROR("parcel WriteString failed");
        return ERR_FLATTEN_OBJECT;
    }

    if (!data.WriteInt32(accountId)) {
        HILOG_ERROR("parcel WriteInt32 failed");
        return ERR_FLATTEN_OBJECT;
    }

    int32_t ret =
        remote->SendRequest(static_cast<uint32_t>(IAmsMgr::Message::KILL_PROCESS_WITH_ACCOUNT), data, reply, option);
    if (ret != NO_ERROR) {
        HILOG_WARN("SendRequest is failed, error code: %{public}d", ret);
        return ret;
    }

    HILOG_DEBUG("end");

    return reply.ReadInt32();
}

int32_t AmsMgrProxy::KillApplication(const std::string &bundleName)
{
    HILOG_DEBUG("start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!WriteInterfaceToken(data)) {
        return ERR_INVALID_DATA;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        HILOG_ERROR("Remote() is NULL");
        return ERR_NULL_OBJECT;
    }
    if (!data.WriteString(bundleName)) {
        HILOG_ERROR("parcel WriteString failed");
        return ERR_FLATTEN_OBJECT;
    }
    int32_t ret =
        remote->SendRequest(static_cast<uint32_t>(IAmsMgr::Message::KILL_APPLICATION), data, reply, option);
    if (ret != NO_ERROR) {
        HILOG_WARN("SendRequest is failed, error code: %{public}d", ret);
        return ret;
    }
    return reply.ReadInt32();
}

int32_t AmsMgrProxy::UpdateApplicationInfoInstalled(const std::string &bundleName, const int uid)
{
    HILOG_DEBUG("start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!WriteInterfaceToken(data)) {
        return ERR_INVALID_DATA;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        HILOG_ERROR("Remote() is NULL");
        return ERR_NULL_OBJECT;
    }
    if (!data.WriteString(bundleName)) {
        HILOG_ERROR("parcel WriteString failed");
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.WriteInt32(uid)) {
        HILOG_ERROR("uid write failed.");
        return ERR_FLATTEN_OBJECT;
    }
    int32_t ret =
        remote->SendRequest(static_cast<uint32_t>(IAmsMgr::Message::UPDATE_APPLICATION_INFO_INSTALLED),
        data, reply, option);
    if (ret != NO_ERROR) {
        HILOG_WARN("SendRequest is failed, error code: %{public}d", ret);
        return ret;
    }
    return reply.ReadInt32();
}

int32_t AmsMgrProxy::KillApplicationByUid(const std::string &bundleName, const int uid)
{
    HILOG_DEBUG("start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!WriteInterfaceToken(data)) {
        return ERR_INVALID_DATA;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        HILOG_ERROR("Remote() is NULL");
        return ERR_NULL_OBJECT;
    }
    if (!data.WriteString(bundleName)) {
        HILOG_ERROR("parcel WriteString failed");
        return ERR_FLATTEN_OBJECT;
    }
    if (!data.WriteInt32(uid)) {
        HILOG_ERROR("uid write failed.");
        return ERR_FLATTEN_OBJECT;
    }
    int32_t ret =
        remote->SendRequest(static_cast<uint32_t>(IAmsMgr::Message::KILL_APPLICATION_BYUID), data, reply, option);
    if (ret != NO_ERROR) {
        HILOG_WARN("SendRequest is failed, error code: %{public}d", ret);
        return ret;
    }
    return reply.ReadInt32();
}

int32_t AmsMgrProxy::KillApplicationSelf()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!WriteInterfaceToken(data)) {
        return ERR_INVALID_DATA;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        HILOG_ERROR("Remote is NULL");
        return ERR_NULL_OBJECT;
    }
    int32_t ret =
        remote->SendRequest(static_cast<uint32_t>(IAmsMgr::Message::KILL_APPLICATION_SELF), data, reply, option);
    if (ret != NO_ERROR) {
        HILOG_ERROR("SendRequest is failed, error code: %{public}d", ret);
        return ret;
    }
    return reply.ReadInt32();
}

void AmsMgrProxy::AbilityAttachTimeOut(const sptr<IRemoteObject> &token)
{
    HILOG_DEBUG("start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!WriteInterfaceToken(data)) {
        return;
    }
    if (!data.WriteRemoteObject(token.GetRefPtr())) {
        HILOG_ERROR("Failed to write token");
        return;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        HILOG_ERROR("Remote() is NULL");
        return;
    }
    int32_t ret =
        remote->SendRequest(static_cast<uint32_t>(IAmsMgr::Message::ABILITY_ATTACH_TIMEOUT), data, reply, option);
    if (ret != NO_ERROR) {
        HILOG_WARN("SendRequest is failed, error code: %{public}d", ret);
    }
    HILOG_DEBUG("end");
}

void AmsMgrProxy::PrepareTerminate(const sptr<IRemoteObject> &token)
{
    HILOG_DEBUG("start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!WriteInterfaceToken(data)) {
        return;
    }
    if (!data.WriteRemoteObject(token.GetRefPtr())) {
        HILOG_ERROR("Failed to write token");
        return;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        HILOG_ERROR("Remote() is NULL");
        return;
    }
    int32_t ret =
        remote->SendRequest(static_cast<uint32_t>(IAmsMgr::Message::PREPARE_TERMINATE_ABILITY),
            data, reply, option);
    if (ret != NO_ERROR) {
        HILOG_WARN("SendRequest is failed, error code: %{public}d", ret);
    }
    HILOG_DEBUG("end");
}

void AmsMgrProxy::GetRunningProcessInfoByToken(
    const sptr<IRemoteObject> &token, AppExecFwk::RunningProcessInfo &info)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!WriteInterfaceToken(data)) {
        return;
    }

    if (!data.WriteRemoteObject(token.GetRefPtr())) {
        return;
    }

    auto remote = Remote();
    if (remote == nullptr) {
        HILOG_ERROR("Remote() is NULL");
        return;
    }
    auto ret = remote->SendRequest(
        static_cast<uint32_t>(IAmsMgr::Message::GET_RUNNING_PROCESS_INFO_BY_TOKEN), data, reply, option);
    if (ret != NO_ERROR) {
        HILOG_WARN("SendRequest is failed, error code: %{public}d", ret);
        return;
    }

    std::unique_ptr<AppExecFwk::RunningProcessInfo> processInfo(reply.ReadParcelable<AppExecFwk::RunningProcessInfo>());
    if (processInfo == nullptr) {
        HILOG_ERROR("recv process info faild");
        return;
    }

    info = *processInfo;
}

void AmsMgrProxy::GetRunningProcessInfoByPid(const pid_t pid, OHOS::AppExecFwk::RunningProcessInfo &info)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!WriteInterfaceToken(data)) {
        return;
    }

    if (!data.WriteInt32(static_cast<int32_t>(pid))) {
        HILOG_ERROR("parcel WriteInt32 failed");
        return;
    }

    auto remote = Remote();
    if (remote == nullptr) {
        HILOG_ERROR("Remote() is NULL");
        return;
    }
    auto ret = remote->SendRequest(
        static_cast<uint32_t>(IAmsMgr::Message::GET_RUNNING_PROCESS_INFO_BY_PID), data, reply, option);
    if (ret != NO_ERROR) {
        HILOG_WARN("SendRequest is failed, error code: %{public}d", ret);
        return;
    }

    std::unique_ptr<AppExecFwk::RunningProcessInfo> processInfo(reply.ReadParcelable<AppExecFwk::RunningProcessInfo>());
    if (processInfo == nullptr) {
        HILOG_ERROR("recv process info faild");
        return;
    }

    info = *processInfo;
}

void AmsMgrProxy::StartSpecifiedAbility(const AAFwk::Want &want, const AppExecFwk::AbilityInfo &abilityInfo)

{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!WriteInterfaceToken(data)) {
        return;
    }

    if (!data.WriteParcelable(&want) || !data.WriteParcelable(&abilityInfo)) {
        HILOG_ERROR("Write data failed.");
        return;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        HILOG_ERROR("Remote is nullptr.");
        return;
    }
    auto ret = remote->SendRequest(
        static_cast<uint32_t>(IAmsMgr::Message::START_SPECIFIED_ABILITY), data, reply, option);
    if (ret != NO_ERROR) {
        HILOG_WARN("SendRequest is failed, error code: %{public}d", ret);
    }
}

void AmsMgrProxy::RegisterStartSpecifiedAbilityResponse(const sptr<IStartSpecifiedAbilityResponse> &response)
{
    HILOG_DEBUG("Register multi instances response by proxy.");
    if (!response) {
        HILOG_ERROR("response is nullptr");
        return;
    }

    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!WriteInterfaceToken(data)) {
        return;
    }
    if (!data.WriteRemoteObject(response->AsObject())) {
        HILOG_ERROR("Failed to write remote object.");
        return;
    }

    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        HILOG_ERROR("Remote is nullptr.");
        return;
    }
    int32_t ret = remote->SendRequest(
        static_cast<uint32_t>(IAmsMgr::Message::REGISTER_START_SPECIFIED_ABILITY_RESPONSE), data, reply, option);
    if (ret != NO_ERROR) {
        HILOG_WARN("SendRequest is failed, error code: %{public}d", ret);
    }
}

int AmsMgrProxy::GetApplicationInfoByProcessID(const int pid, AppExecFwk::ApplicationInfo &application, bool &debug)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!WriteInterfaceToken(data)) {
        HILOG_ERROR("token write error.");
        return ERR_FLATTEN_OBJECT;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        HILOG_ERROR("Remote is nullptr.");
        return ERR_NULL_OBJECT;
    }
    data.WriteInt32(pid);
    int32_t ret = remote->SendRequest(
        static_cast<uint32_t>(IAmsMgr::Message::GET_APPLICATION_INFO_BY_PROCESS_ID), data, reply, option);
    if (ret != NO_ERROR) {
        HILOG_ERROR("send request fail.");
        return ret;
    }
    auto result = reply.ReadInt32();
    if (result != NO_ERROR) {
        HILOG_ERROR("reply result false");
        return result;
    }
    std::unique_ptr<AppExecFwk::ApplicationInfo> info(reply.ReadParcelable<AppExecFwk::ApplicationInfo>());
    if (!info) {
        HILOG_ERROR("readParcelableInfo failed");
        return ERR_NAME_NOT_FOUND;
    }
    application = *info;
    debug = reply.ReadBool();
    HILOG_DEBUG("get parcelable info success");
    return NO_ERROR;
}

void AmsMgrProxy::SetCurrentUserId(const int32_t userId)
{
    HILOG_DEBUG("start");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    if (!WriteInterfaceToken(data)) {
        return;
    }
    if (!data.WriteInt32(userId)) {
        HILOG_ERROR("Failed to write userId");
        return;
    }
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        HILOG_ERROR("Remote() is NULL");
        return;
    }
    int32_t ret =
        remote->SendRequest(static_cast<uint32_t>(IAmsMgr::Message::SET_CURRENT_USER_ID),
            data, reply, option);
    if (ret != NO_ERROR) {
        HILOG_WARN("SendRequest is failed, error code: %{public}d", ret);
    }
    HILOG_DEBUG("end");
}
}  // namespace AppExecFwk
}  // namespace OHOS
