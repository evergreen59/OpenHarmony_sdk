/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "connection_record.h"

#include "ability_manager_errors.h"
#include "ability_manager_service.h"
#include "ability_util.h"
#include "connection_state_manager.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace AAFwk {
int64_t ConnectionRecord::connectRecordId = 0;

ConnectionRecord::ConnectionRecord(const sptr<IRemoteObject> &callerToken,
    const std::shared_ptr<AbilityRecord> &targetService, const sptr<IAbilityConnection> &connCallback)
    : state_(ConnectionState::INIT),
      callerToken_(callerToken),
      targetService_(targetService),
      connCallback_(connCallback)
{
    recordId_ = connectRecordId++;
}

ConnectionRecord::~ConnectionRecord()
{}

std::shared_ptr<ConnectionRecord> ConnectionRecord::CreateConnectionRecord(const sptr<IRemoteObject> &callerToken,
    const std::shared_ptr<AbilityRecord> &targetService, const sptr<IAbilityConnection> &connCallback)
{
    auto connRecord = std::make_shared<ConnectionRecord>(callerToken, targetService, connCallback);
    CHECK_POINTER_AND_RETURN(connRecord, nullptr);
    connRecord->SetConnectState(ConnectionState::INIT);
    return connRecord;
}

void ConnectionRecord::SetConnectState(const ConnectionState &state)
{
    state_ = state;
}

ConnectionState ConnectionRecord::GetConnectState() const
{
    return state_;
}

sptr<IRemoteObject> ConnectionRecord::GetToken() const
{
    return callerToken_;
}

std::shared_ptr<AbilityRecord> ConnectionRecord::GetAbilityRecord() const
{
    return targetService_;
}

sptr<IAbilityConnection> ConnectionRecord::GetAbilityConnectCallback() const
{
    return connCallback_;
}

void ConnectionRecord::ClearConnCallBack()
{
    if (connCallback_) {
        connCallback_.clear();
    }
}

int ConnectionRecord::DisconnectAbility()
{
    if (state_ != ConnectionState::CONNECTED) {
        HILOG_INFO("The connection has not established.");
        return INVALID_CONNECTION_STATE;
    }

    /* set state to Disconnecting */
    SetConnectState(ConnectionState::DISCONNECTING);
    CHECK_POINTER_AND_RETURN(targetService_, ERR_INVALID_VALUE);
    std::size_t connectNums = targetService_->GetConnectRecordList().size();
    if (connectNums == 1) {
        /* post timeout task to eventhandler */
        auto handler = DelayedSingleton<AbilityManagerService>::GetInstance()->GetEventHandler();
        if (handler == nullptr) {
            HILOG_ERROR("fail to get AbilityEventHandler");
        } else {
            std::string taskName("DisconnectTimeout_");
            taskName += std::to_string(recordId_);
            auto disconnectTask = [connectionRecord = shared_from_this()]() {
                HILOG_ERROR("Disconnect ability timeout");
                connectionRecord->DisconnectTimeout();
            };
            handler->PostTask(disconnectTask, taskName, AbilityManagerService::DISCONNECT_TIMEOUT);
        }
        /* schedule disconnect to target ability */
        targetService_->DisconnectAbility();
    } else {
        targetService_->RemoveConnectRecordFromList(shared_from_this());
        SetConnectState(ConnectionState::DISCONNECTED);
    }

    return ERR_OK;
}

void ConnectionRecord::CompleteConnect(int resultCode)
{
    CHECK_POINTER(targetService_);
    if (resultCode == ERR_OK) {
        SetConnectState(ConnectionState::CONNECTED);
        targetService_->SetAbilityState(AbilityState::ACTIVE);
    }
    const AppExecFwk::AbilityInfo &abilityInfo = targetService_->GetAbilityInfo();
    AppExecFwk::ElementName element(abilityInfo.deviceId, abilityInfo.bundleName,
        abilityInfo.name, abilityInfo.moduleName);
    auto remoteObject = targetService_->GetConnRemoteObject();
    if (connCallback_) {
        HILOG_DEBUG("OnAbilityConnectDone");
        connCallback_->OnAbilityConnectDone(element, remoteObject, resultCode);
    }
    DelayedSingleton<ConnectionStateManager>::GetInstance()->AddConnection(shared_from_this());
    HILOG_INFO("result: %{public}d. connectState:%{public}d.", resultCode, state_);
}

void ConnectionRecord::CompleteDisconnect(int resultCode, bool isDied)
{
    if (resultCode == ERR_OK) {
        SetConnectState(ConnectionState::DISCONNECTED);
    }
    CHECK_POINTER(targetService_);
    const AppExecFwk::AbilityInfo &abilityInfo = targetService_->GetAbilityInfo();
    AppExecFwk::ElementName element(abilityInfo.deviceId, abilityInfo.bundleName,
        abilityInfo.name, abilityInfo.moduleName);
    if (connCallback_) {
        HILOG_DEBUG("OnAbilityDisconnectDone");
        connCallback_->OnAbilityDisconnectDone(element, isDied ? (resultCode - 1) : resultCode);
    }
    DelayedSingleton<ConnectionStateManager>::GetInstance()->RemoveConnection(shared_from_this(), isDied);
    HILOG_INFO("result: %{public}d. connectState:%{public}d.", resultCode, state_);
}

void ConnectionRecord::ScheduleDisconnectAbilityDone()
{
    if (state_ != ConnectionState::DISCONNECTING) {
        HILOG_ERROR("fail to schedule disconnect ability done, current state is not disconnecting.");
        return;
    }

    auto handler = DelayedSingleton<AbilityManagerService>::GetInstance()->GetEventHandler();
    if (handler == nullptr) {
        HILOG_ERROR("fail to get AbilityEventHandler");
    } else {
        std::string taskName = std::string("DisconnectTimeout_") + std::to_string(recordId_);
        handler->RemoveTask(taskName);
    }

    CompleteDisconnect(ERR_OK, false);
}

void ConnectionRecord::ScheduleConnectAbilityDone()
{
    if (state_ != ConnectionState::CONNECTING) {
        HILOG_ERROR("fail to schedule connect ability done, current state is not connecting.");
        return;
    }
    auto handler = DelayedSingleton<AbilityManagerService>::GetInstance()->GetEventHandler();
    if (handler == nullptr) {
        HILOG_ERROR("fail to get AbilityEventHandler");
    } else {
        std::string taskName = std::string("ConnectTimeout_") + std::to_string(recordId_);
        handler->RemoveTask(taskName);
    }

    CompleteConnect(ERR_OK);
}

void ConnectionRecord::DisconnectTimeout()
{
    CHECK_POINTER(targetService_);
    /* force to disconnect */
    /* so scheduler target service disconnect done */
    DelayedSingleton<AbilityManagerService>::GetInstance()->ScheduleDisconnectAbilityDone(targetService_->GetToken());
}

std::string ConnectionRecord::ConvertConnectionState(const ConnectionState &state) const
{
    switch (state) {
        case ConnectionState::INIT:
            return "INIT";
        case ConnectionState::CONNECTING:
            return "CONNECTING";
        case ConnectionState::CONNECTED:
            return "CONNECTED";
        case ConnectionState::DISCONNECTING:
            return "DISCONNECTING";
        case ConnectionState::DISCONNECTED:
            return "DISCONNECTED";
        default:
            return "INVALIDSTATE";
    }
}

void ConnectionRecord::Dump(std::vector<std::string> &info) const
{
    info.emplace_back("       > " + GetAbilityRecord()->GetAbilityInfo().bundleName + "/" +
                      GetAbilityRecord()->GetAbilityInfo().name + "   connectionState #" +
                      ConvertConnectionState(GetConnectState()));
}

void ConnectionRecord::AttachCallerInfo()
{
    auto targetRecord = Token::GetAbilityRecordByToken(callerToken_);
    if (targetRecord) {
        callerUid_ = targetRecord->GetUid();
        callerPid_ = targetRecord->GetPid();
        callerName_ = targetRecord->GetAbilityInfo().bundleName;
        return;
    }

    callerUid_ = static_cast<int32_t>(IPCSkeleton::GetCallingUid());
    callerPid_ = static_cast<int32_t>(IPCSkeleton::GetCallingPid());
    callerName_ = ConnectionStateManager::GetProcessNameByPid(callerPid_);
}

int32_t ConnectionRecord::GetCallerUid() const
{
    return callerUid_;
}

int32_t ConnectionRecord::GetCallerPid() const
{
    return callerPid_;
}

std::string ConnectionRecord::GetCallerName() const
{
    return callerName_;
}

sptr<IRemoteObject> ConnectionRecord::GetTargetToken() const
{
    auto targetService = targetService_;
    if (!targetService) {
        return nullptr;
    }

    auto token = targetService->GetToken();
    if (!token) {
        return nullptr;
    }

    return token->AsObject();
}

sptr<IRemoteObject> ConnectionRecord::GetConnection() const
{
    auto callback = connCallback_;
    if (!callback) {
        return nullptr;
    }

    return callback->AsObject();
}
}  // namespace AAFwk
}  // namespace OHOS
