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

#ifndef MOCK_OHOS_ABILITY_RUNTIME_MOCK_ABILITY_MGR_SERVICE_H
#define MOCK_OHOS_ABILITY_RUNTIME_MOCK_ABILITY_MGR_SERVICE_H

#include "gmock/gmock.h"
#include "semaphore_ex.h"
#include "ability_manager_stub.h"

namespace OHOS {
namespace AAFwk {
class MockAbilityMgrService : public AbilityManagerStub {
public:
    MOCK_METHOD3(StartAbility, int(const Want &want, int32_t userId, int requestCode));
    MOCK_METHOD4(StartAbility, int(const Want &want, const sptr<IRemoteObject> &callerToken, int32_t userId,
         int requestCode));
    MOCK_METHOD5(StartAbility, int(const Want &want, const AbilityStartSetting &abilityStartSetting,
        const sptr<IRemoteObject> &callerToken, int32_t userId, int requestCode));
    MOCK_METHOD3(TerminateAbility, int(const sptr<IRemoteObject> &token, int resultCode, const Want *resultWant));
    MOCK_METHOD4(ConnectAbility, int(const Want &want, const sptr<IAbilityConnection> &connect,
        const sptr<IRemoteObject> &callerToken, int32_t userId));
    MOCK_METHOD1(DisconnectAbility, int(const sptr<IAbilityConnection> &connect));
    MOCK_METHOD3(AcquireDataAbility, sptr<IAbilityScheduler>(const Uri &, bool, const sptr<IRemoteObject> &));
    MOCK_METHOD2(ReleaseDataAbility, int(sptr<IAbilityScheduler>, const sptr<IRemoteObject> &));
    MOCK_METHOD2(AttachAbilityThread, int(const sptr<IAbilityScheduler> &scheduler,
        const sptr<IRemoteObject> &token));
    MOCK_METHOD3(AbilityTransitionDone, int(const sptr<IRemoteObject> &token, int state, const PacMap &saveData));
    MOCK_METHOD2(
        ScheduleConnectAbilityDone, int(const sptr<IRemoteObject> &token, const sptr<IRemoteObject> &remoteObject));
    MOCK_METHOD1(ScheduleDisconnectAbilityDone, int(const sptr<IRemoteObject> &token));
    MOCK_METHOD1(ScheduleCommandAbilityDone, int(const sptr<IRemoteObject> &token));
    void DumpState(const std::string &args, std::vector<std::string> &state)
    {
        GTEST_LOG_(INFO) << "MockAbilityMgrService::DumpState called";
        DumpStateCalled_ = true;
        EXPECT_TRUE(DumpStateCalled_);
        return;
    }
    MOCK_METHOD2(TerminateAbilityResult, int(const sptr<IRemoteObject> &, int startId));
    MOCK_METHOD2(TerminateAbilityByCaller, int(const sptr<IRemoteObject> &callerToken, int requestCode));
    MOCK_METHOD2(StopServiceAbility, int(const Want &, int32_t userId));
    MOCK_METHOD4(OnRemoteRequest, int(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option));

    MOCK_METHOD1(KillProcess, int(const std::string &bundleName));
    MOCK_METHOD2(UninstallApp, int(const std::string &bundleName, int32_t uid));
    MOCK_METHOD1(TerminateAbilityByRecordId, int(const int64_t recordId));

    MOCK_METHOD1(LockMissionForCleanup, int(int32_t missionId));
    MOCK_METHOD1(UnlockMissionForCleanup, int(int32_t missionId));
    MOCK_METHOD1(RegisterMissionListener, int(const sptr<IMissionListener> &listener));
    MOCK_METHOD1(UnRegisterMissionListener, int(const sptr<IMissionListener> &listener));
    MOCK_METHOD3(
        GetMissionInfos, int(const std::string& deviceId, int32_t numMax, std::vector<MissionInfo> &missionInfos));
    MOCK_METHOD3(GetMissionInfo, int(const std::string& deviceId, int32_t missionId, MissionInfo &missionInfo));
    MOCK_METHOD1(CleanMission, int(int32_t missionId));
    MOCK_METHOD0(CleanAllMissions, int());
    MOCK_METHOD1(MoveMissionToFront, int(int32_t missionId));
    MOCK_METHOD2(MoveMissionToFront, int(int32_t missionId, const StartOptions &startOptions));
    MOCK_METHOD1(GetMissionIdByToken, int32_t(const sptr<IRemoteObject> &token));

    MOCK_METHOD1(GetAbilityRunningInfos, int(std::vector<AbilityRunningInfo> &info));
    MOCK_METHOD2(GetExtensionRunningInfos, int(int upperLimit, std::vector<ExtensionRunningInfo> &info));
    MOCK_METHOD1(GetProcessRunningInfos, int(std::vector<AppExecFwk::RunningProcessInfo> &info));
    virtual int SetAbilityController(const sptr<AppExecFwk::IAbilityController> &abilityController,
        bool imAStabilityTest) override
    {
        return 0;
    }

    virtual bool IsRunningInStabilityTest() override
    {
        return true;
    }

    void CallRequestDone(const sptr<IRemoteObject>& token, const sptr<IRemoteObject>& callStub) override
    {
        return;
    }

    void Wait()
    {
        sem_.Wait();
    }

    int Post()
    {
        sem_.Post();
        return 0;
    }

    void PostVoid()
    {
        sem_.Post();
    }

    #ifdef ABILITY_COMMAND_FOR_TEST
    virtual int ForceTimeoutForTest(const std::string &abilityName, const std::string &state) override
    {
        return 0;
    }
    #endif

private:
    bool DumpStateCalled_ = false;
    Semaphore sem_;
};
}  // namespace AAFwk
}  // namespace OHOS
#endif  // MOCK_OHOS_ABILITY_RUNTIME_MOCK_ABILITY_MGR_SERVICE_H
