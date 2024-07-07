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

#include "mock_serviceability_manager_service.h"

#include <gtest/gtest.h>
#include <functional>
#include <memory>
#include <string>
#include <unistd.h>

using OHOS::AppExecFwk::ElementName;

namespace OHOS {
namespace AAFwk {
MockServiceAbilityManagerService::MockServiceAbilityManagerService()
{}

MockServiceAbilityManagerService::~MockServiceAbilityManagerService()
{}

int MockServiceAbilityManagerService::StartAbility(const Want &want, int32_t userId, int requestCode)
{
    return 0;
}

int MockServiceAbilityManagerService::StartAbility(
    const Want &want, const sptr<IRemoteObject> &callerToken, int32_t userId, int requestCode)
{
    return 0;
}

int MockServiceAbilityManagerService::StartAbilityByCall(
    const Want &want, const sptr<IAbilityConnection> &connect, const sptr<IRemoteObject> &callerToken)
{
    GTEST_LOG_(INFO) << "MockServiceAbilityManagerService::StartAbilityByCall begain";
    if (!connect) {
        GTEST_LOG_(INFO) << "MockAbilityManagerService::StartAbilityByCall connect is null";
        return -1;
    }
    GTEST_LOG_(INFO) << "MockServiceAbilityManagerService::StartAbilityByCall end";
    return 0;
}

int MockServiceAbilityManagerService::StartAbility(
    const Want &want, const StartOptions &startOptions,
    const sptr<IRemoteObject> &callerToken, int32_t userId, int requestCode)
{
    return 0;
}

int MockServiceAbilityManagerService::TerminateAbility(
    const sptr<IRemoteObject> &token, int resultCode, const Want *resultWant)
{
    return 0;
}

int MockServiceAbilityManagerService::ConnectAbility(
    const Want &want, const sptr<IAbilityConnection> &connect, const sptr<IRemoteObject> &callerToken, int32_t userId)
{
    return 0;
}

int MockServiceAbilityManagerService::DisconnectAbility(const sptr<IAbilityConnection> &connect)
{
    return 0;
}

int MockServiceAbilityManagerService::AttachAbilityThread(
    const sptr<IAbilityScheduler> &scheduler, const sptr<IRemoteObject> &token)
{
    return 0;
}

void MockServiceAbilityManagerService::DumpState(const std::string &args, std::vector<std::string> &info)
{}

void MockServiceAbilityManagerService::DumpSysState(
    const std::string& args, std::vector<std::string>& state, bool isClient, bool isUserID, int UserID)
{}

int MockServiceAbilityManagerService::AbilityTransitionDone(
    const sptr<IRemoteObject> &token, int state, const PacMap &saveData)
{
    return 0;
}

int MockServiceAbilityManagerService::ScheduleConnectAbilityDone(
    const sptr<IRemoteObject> &token, const sptr<IRemoteObject> &remoteObject)
{
    return 0;
}

int MockServiceAbilityManagerService::ScheduleDisconnectAbilityDone(const sptr<IRemoteObject> &token)
{
    return 0;
}

int MockServiceAbilityManagerService::ScheduleCommandAbilityDone(const sptr<IRemoteObject> &token)
{
    return 0;
}

int MockServiceAbilityManagerService::TerminateAbilityResult(const sptr<IRemoteObject> &token, int startId)
{
    return 0;
}

int MockServiceAbilityManagerService::TerminateAbilityByCaller(const sptr<IRemoteObject> &callerToken, int requestCode)
{
    return 0;
}

void MockServiceAbilityManagerService::CallRequestDone(
    const sptr<IRemoteObject> &token, const sptr<IRemoteObject> &callStub)
{
    return;
}

int MockServiceAbilityManagerService::ReleaseCall(const sptr<IAbilityConnection>& connect,
    const AppExecFwk::ElementName& element)
{
    GTEST_LOG_(INFO) << "MockServiceAbilityManagerService::ReleaseCall";
    return 0;
}

int MockServiceAbilityManagerService::StopServiceAbility(const Want &want, int32_t userId)
{
    return 0;
}
} // namespace AAFwk
} // namespace OHOS
