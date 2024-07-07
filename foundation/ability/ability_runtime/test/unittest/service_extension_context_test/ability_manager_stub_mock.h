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

#ifndef UNITTEST_OHOS_ABILITY_RUNTIME_ABILITY_MANAGER_STUB_MOCK_TEST_H
#define UNITTEST_OHOS_ABILITY_RUNTIME_ABILITY_MANAGER_STUB_MOCK_TEST_H
#include <gmock/gmock.h>
#include <iremote_object.h>
#include <iremote_stub.h>
#include "ability_manager_interface.h"

namespace OHOS {
namespace AAFwk {
class AbilityManagerStubTestMock : public IRemoteStub<IAbilityManager> {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"IAbilityManagerMock");

    AbilityManagerStubTestMock() : code_(0)
    {}
    virtual ~AbilityManagerStubTestMock()
    {}

    MOCK_METHOD4(SendRequest, int(uint32_t, MessageParcel&, MessageParcel&, MessageOption&));

    int InvokeSendRequest(uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option)
    {
        code_ = code;

        return 0;
    }

    int InvokeErrorSendRequest(uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option)
    {
        code_ = code;
        return UNKNOWN_ERROR;
    }

    int code_ = 0;

    virtual int StartAbility(const Want& want, int32_t userId = DEFAULT_INVAL_VALUE, int requestCode = -1)
    {
        return 0;
    }

    virtual int StartAbility(const Want& want,
        const AbilityStartSetting& abilityStartSetting,
        const sptr<IRemoteObject>& callerToken,
        int32_t userId = DEFAULT_INVAL_VALUE,
        int requestCode = DEFAULT_INVAL_VALUE)
    {
        return 0;
    }

    virtual int StartAbility(
        const Want& want,
        const StartOptions& startOptions,
        const sptr<IRemoteObject>& callerToken,
        int32_t userId = DEFAULT_INVAL_VALUE,
        int requestCode = DEFAULT_INVAL_VALUE)
    {
        return 0;
    }

    virtual int TerminateAbility(const sptr<IRemoteObject>& token, int resultCode, const Want* resultWant = nullptr)
    {
        return 0;
    }
    int CloseAbility(const sptr<IRemoteObject>& token, int resultCode = DEFAULT_INVAL_VALUE,
        const Want* resultWant = nullptr) override
    {
        return 0;
    }
    int MinimizeAbility(const sptr<IRemoteObject>& token, bool fromUser) override
    {
        return 0;
    }

    virtual int ConnectAbility(
        const Want& want,
        const sptr<IAbilityConnection>& connect,
        const sptr<IRemoteObject>& callerToken,
        int32_t userId = DEFAULT_INVAL_VALUE)
    {
        return 0;
    }

    sptr<IAbilityScheduler> AcquireDataAbility(
        const Uri& uri, bool tryBind, const sptr<IRemoteObject>& callerToken) override
    {
        return nullptr;
    }

    virtual int ReleaseDataAbility(sptr<IAbilityScheduler> dataAbilityScheduler, const sptr<IRemoteObject>& callerToken)
    {
        return 0;
    }

    virtual int DisconnectAbility(const sptr<IAbilityConnection>& connect)
    {
        return 0;
    }

    virtual int AttachAbilityThread(const sptr<IAbilityScheduler>& scheduler, const sptr<IRemoteObject>& token)
    {
        return 0;
    }

    virtual int AbilityTransitionDone(const sptr<IRemoteObject>& token, int state, const PacMap& saveData)
    {
        return 0;
    }

    virtual int ScheduleConnectAbilityDone(const sptr<IRemoteObject>& token, const sptr<IRemoteObject>& remoteObject)
    {
        return 0;
    }

    virtual int ScheduleDisconnectAbilityDone(const sptr<IRemoteObject>& token)
    {
        return 0;
    }

    virtual int ScheduleCommandAbilityDone(const sptr<IRemoteObject>& token)
    {
        return 0;
    }

    virtual void DumpState(const std::string& args, std::vector<std::string>& state)
    {}

    virtual void DumpSysState(
        const std::string& args, std::vector<std::string>& info, bool isClient, bool isUserID, int UserID)
    {}

    virtual int TerminateAbilityResult(const sptr<IRemoteObject>& token, int startId)
    {
        return 0;
    }

    virtual int StopServiceAbility(const Want& want, int32_t userId = DEFAULT_INVAL_VALUE)
    {
        return 0;
    }

    virtual int KillProcess(const std::string& bundleName)
    {
        return 0;
    }

    virtual int UninstallApp(const std::string& bundleName, int32_t uid)
    {
        return 0;
    }

    int32_t GetMissionIdByToken(const sptr<IRemoteObject>& token) override
    {
        return 0;
    }

    int ClearUpApplicationData(const std::string& bundleName) override
    {
        return 0;
    }

    int StartUser(int userId) override
    {
        return 0;
    }

    int StopUser(int userId, const sptr<IStopUserCallback>& callback) override
    {
        return 0;
    }
    int StartSyncRemoteMissions(const std::string& devId, bool fixConflict, int64_t tag) override
    {
        return 0;
    }
    int StopSyncRemoteMissions(const std::string& devId) override
    {
        return 0;
    }
    int RegisterMissionListener(const std::string& deviceId,
        const sptr<IRemoteMissionListener>& listener) override
    {
        return 0;
    }
    int UnRegisterMissionListener(const std::string& deviceId,
        const sptr<IRemoteMissionListener>& listener) override
    {
        return 0;
    }
    int ReleaseCall(const sptr<IAbilityConnection>& connect,
        const AppExecFwk::ElementName& element) override
    {
        return 0;
    }
    virtual int GetMissionSnapshot(const std::string& deviceId, int32_t missionId,
        MissionSnapshot& snapshot, bool isLowResolution)
    {
        return 0;
    }

    virtual void UpdateMissionSnapShot(const sptr<IRemoteObject>& token)
    {
        return;
    }

    virtual int RegisterSnapshotHandler(const sptr<ISnapshotHandler>& handler)
    {
        return 0;
    }

    int RegisterWindowManagerServiceHandler(const sptr<IWindowManagerServiceHandler>& handler) override
    {
        return 0;
    }

    void CompleteFirstFrameDrawing(const sptr<IRemoteObject>& abilityToken) override {}

    int SetAbilityController(const sptr<AppExecFwk::IAbilityController>& abilityController,
        bool imAStabilityTest) override
    {
        return 0;
    }

    bool IsRunningInStabilityTest() override
    {
        return true;
    }

    int SendANRProcessID(int pid) override
    {
        return 0;
    }

    int SetMissionIcon(const sptr<IRemoteObject>& token,
        const std::shared_ptr<OHOS::Media::PixelMap>& icon) override
    {
        if (!token || !icon) {
            return -1;
        }

        return 0;
    }

    MOCK_METHOD2(TerminateAbilityByCaller, int(const sptr<IRemoteObject>& callerToken, int requestCode));
    MOCK_METHOD4(StartAbility, int(const Want& want, const sptr<IRemoteObject>& callerToken,
        int32_t userId, int requestCode));
    MOCK_METHOD2(
        GetWantSender, sptr<IWantSender>(const WantSenderInfo& wantSenderInfo, const sptr<IRemoteObject>& callerToken));
    MOCK_METHOD2(SendWantSender, int(const sptr<IWantSender>& target, const SenderInfo& senderInfo));
    MOCK_METHOD1(CancelWantSender, void(const sptr<IWantSender>& sender));
    MOCK_METHOD1(GetPendingWantUid, int(const sptr<IWantSender>& target));
    MOCK_METHOD1(GetPendingWantUserId, int(const sptr<IWantSender>& target));
    MOCK_METHOD1(GetPendingWantBundleName, std::string(const sptr<IWantSender>& target));
    MOCK_METHOD1(GetPendingWantCode, int(const sptr<IWantSender>& target));
    MOCK_METHOD1(GetPendingWantType, int(const sptr<IWantSender>& target));
    MOCK_METHOD2(RegisterCancelListener, void(const sptr<IWantSender>& sender, const sptr<IWantReceiver>& receiver));
    MOCK_METHOD2(UnregisterCancelListener, void(const sptr<IWantSender>& sender, const sptr<IWantReceiver>& receiver));
    MOCK_METHOD2(GetPendingRequestWant, int(const sptr<IWantSender>& target, std::shared_ptr<Want>& want));
    MOCK_METHOD3(StartContinuation, int(const Want& want, const sptr<IRemoteObject>& abilityToken, int32_t status));
    MOCK_METHOD2(NotifyContinuationResult, int(int32_t missionId, int32_t result));
    MOCK_METHOD5(ContinueMission, int(const std::string& srcDeviceId, const std::string& dstDeviceId,
        int32_t missionId, const sptr<IRemoteObject>& callBack, AAFwk::WantParams& wantParams));
    MOCK_METHOD3(ContinueAbility, int(const std::string& deviceId, int32_t missionId, uint32_t versionCode));
    MOCK_METHOD3(NotifyCompleteContinuation, void(const std::string& deviceId, int32_t sessionId, bool isSuccess));

    MOCK_METHOD1(LockMissionForCleanup, int(int32_t missionId));
    MOCK_METHOD1(UnlockMissionForCleanup, int(int32_t missionId));
    MOCK_METHOD1(RegisterMissionListener, int(const sptr<IMissionListener>& listener));
    MOCK_METHOD1(UnRegisterMissionListener, int(const sptr<IMissionListener>& listener));
    MOCK_METHOD3(
        GetMissionInfos, int(const std::string& deviceId, int32_t numMax, std::vector<MissionInfo>& missionInfos));
    MOCK_METHOD3(GetMissionInfo, int(const std::string& deviceId, int32_t missionId, MissionInfo& missionInfo));
    MOCK_METHOD1(CleanMission, int(int32_t missionId));
    MOCK_METHOD0(CleanAllMissions, int());
    MOCK_METHOD1(MoveMissionToFront, int(int32_t missionId));
    MOCK_METHOD2(MoveMissionToFront, int(int32_t missionId, const StartOptions& startOptions));
    MOCK_METHOD2(SetMissionLabel, int(const sptr<IRemoteObject>& token, const std::string& label));
    MOCK_METHOD2(GetWantSenderInfo, int(const sptr<IWantSender>& target, std::shared_ptr<WantSenderInfo>& info));
    MOCK_METHOD1(GetAbilityRunningInfos, int(std::vector<AbilityRunningInfo>& info));
    MOCK_METHOD2(GetExtensionRunningInfos, int(int upperLimit, std::vector<ExtensionRunningInfo>& info));
    MOCK_METHOD1(GetProcessRunningInfos, int(std::vector<AppExecFwk::RunningProcessInfo>& info));
#ifdef ABILITY_COMMAND_FOR_TEST
    MOCK_METHOD0(BlockAppService, int());
    MOCK_METHOD0(BlockAmsService, int());
    MOCK_METHOD1(BlockAbility, int(int32_t abilityRecordId));
#endif

    int SendResultToAbility(int requestCode, int resultCode, Want& resultWant) override
    {
        return 0;
    }

    int StartAbilityByCall(
        const Want& want, const sptr<IAbilityConnection>& connect, const sptr<IRemoteObject>& callerToken) override
    {
        return 0;
    }

    void CallRequestDone(const sptr<IRemoteObject> &token, const sptr<IRemoteObject> &callStub) override
    {
        return;
    }

    int StartUserTest(const Want &want, const sptr<IRemoteObject> &observer) override
    {
        return 0;
    }

    int FinishUserTest(
        const std::string& msg, const int64_t& resultCode, const std::string& bundleName) override
    {
        return 0;
    }

    int GetTopAbility(sptr<IRemoteObject>& token) override
    {
        return 0;
    }

    int DelegatorDoAbilityForeground(const sptr<IRemoteObject>& token) override
    {
        return 0;
    }

    int DelegatorDoAbilityBackground(const sptr<IRemoteObject>& token) override
    {
        return 0;
    }

    int DoAbilityForeground(const sptr<IRemoteObject>& token, uint32_t flag) override
    {
        return 0;
    }

    int DoAbilityBackground(const sptr<IRemoteObject>& token, uint32_t flag) override
    {
        return 0;
    }

#ifdef ABILITY_COMMAND_FOR_TEST
    int ForceTimeoutForTest(const std::string& abilityName, const std::string& state) override
    {
        return 0;
    }
#endif
};
}  // namespace AAFwk
}  // namespace OHOS

#endif