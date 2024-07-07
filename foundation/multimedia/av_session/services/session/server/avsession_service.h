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

#ifndef OHOS_AVSESSION_SERVICE_H
#define OHOS_AVSESSION_SERVICE_H

#include <atomic>
#include <mutex>
#include <map>

#include "iremote_stub.h"
#include "system_ability.h"
#include "audio_info.h"
#include "avsession_service_stub.h"
#include "avsession_item.h"
#include "avcontroller_item.h"
#include "session_container.h"
#include "iclient_death.h"
#include "isession_listener.h"
#include "focus_session_strategy.h"
#include "background_audio_controller.h"
#include "ability_manager_adapter.h"
#include "device_manager.h"
#include "dm_device_info.h"
#include "audio_adapter.h"
#include "remote_session_command_process.h"

namespace OHOS::AVSession {
class AVSessionDumper;
class AVSessionInitDMCallback : public OHOS::DistributedHardware::DmInitCallback {
public:
    AVSessionInitDMCallback() = default;
    ~AVSessionInitDMCallback() override = default;
    void OnRemoteDied() override {};
};

class AudioDeviceDescriptorComp {
public:
    bool operator()(const AudioStandard::AudioDeviceDescriptor& left,
                    const AudioStandard::AudioDeviceDescriptor& right) const
    {
        return left.networkId_ == right.networkId_;
    }
};

class AVSessionService : public SystemAbility, public AVSessionServiceStub {
    DECLARE_SYSTEM_ABILITY(AVSessionService);

public:
    DISALLOW_COPY_AND_MOVE(AVSessionService);

    explicit AVSessionService(int32_t systemAbilityId, bool runOnCreate = true);

    ~AVSessionService() override;

    void OnDump() override;

    void OnStart() override;

    void OnStop() override;

    void OnAddSystemAbility(int32_t systemAbilityId, const std::string& deviceId) override;

    sptr<IRemoteObject> CreateSessionInner(const std::string& tag, int32_t type,
                                           const AppExecFwk::ElementName& elementName) override;

    int32_t GetAllSessionDescriptors(std::vector<AVSessionDescriptor>& descriptors) override;

    int32_t GetSessionDescriptorsBySessionId(const std::string& sessionId, AVSessionDescriptor& descriptor) override;

    int32_t CreateControllerInner(const std::string& sessionId, sptr<IRemoteObject>& object) override;

    int32_t RegisterSessionListener(const sptr<ISessionListener>& listener) override;

    int32_t SendSystemAVKeyEvent(const MMI::KeyEvent& keyEvent) override;

    int32_t SendSystemControlCommand(const AVControlCommand& command) override;

    int32_t RegisterClientDeathObserver(const sptr<IClientDeath>& observer) override;

    void OnClientDied(pid_t pid);

    void HandleSessionRelease(AVSessionItem& session);

    void HandleControllerRelease(AVControllerItem& controller);

    std::int32_t Dump(std::int32_t fd, const std::vector<std::u16string>& args) override;

    int32_t CastAudio(const SessionToken& token,
                      const std::vector<AudioStandard::AudioDeviceDescriptor>& sinkAudioDescriptors) override;

    int32_t CastAudioForAll(const std::vector<AudioStandard::AudioDeviceDescriptor>& sinkAudioDescriptors) override;

private:
    static SessionContainer& GetContainer();

    std::string AllocSessionId();

    bool AbilityHasSession(pid_t pid, const std::string& abilityName);

    sptr<AVControllerItem> GetPresentController(pid_t pid, const std::string& sessionId);

    void NotifySessionCreate(const AVSessionDescriptor& descriptor);
    void NotifySessionRelease(const AVSessionDescriptor& descriptor);
    void NotifyTopSessionChanged(const AVSessionDescriptor& descriptor);

    void AddClientDeathObserver(pid_t pid, const sptr<IClientDeath>& observer);
    void RemoveClientDeathObserver(pid_t pid);

    void AddSessionListener(pid_t pid, const sptr<ISessionListener>& listener);
    void RemoveSessionListener(pid_t pid);

    void AddInnerSessionListener(SessionListener* listener);

    sptr<AVSessionItem> SelectSessionByUid(const AudioStandard::AudioRendererChangeInfo& info);

    void OutputDeviceChangeListener(const AudioRendererChangeInfos& infos);

    sptr<AVSessionItem> CreateSessionInner(const std::string& tag, int32_t type, bool thirdPartyApp,
                                           const AppExecFwk::ElementName& elementName);

    sptr<AVSessionItem> CreateNewSession(const std::string& tag, int32_t type, bool thirdPartyApp,
                                         const AppExecFwk::ElementName& elementName);

    sptr<AVControllerItem> CreateNewControllerForSession(pid_t pid, sptr<AVSessionItem>& session);

    int32_t CancelCastAudioForClientExit(pid_t pid, const sptr<AVSessionItem>& session);

    void ClearSessionForClientDiedNoLock(pid_t pid);

    void ClearControllerForClientDiedNoLock(pid_t pid);

    void ClearSessionNoLock(const std::string& sessionId);

    void InitKeyEvent();

    void InitAudio();

    void InitAMS();

    void InitDM();

    bool SelectFocusSession(const FocusSessionStrategy::FocusSessionChangeInfo& info);

    void UpdateTopSession(const sptr<AVSessionItem>& newTopSession);

    void HandleFocusSession(const FocusSessionStrategy::FocusSessionChangeInfo& info);

    sptr<RemoteSessionCommandProcess> GetService(const std::string& deviceId);

    int32_t CastAudioProcess(const std::vector<AudioStandard::AudioDeviceDescriptor>& descriptors,
                             const std::string& sourceSessionInfo,
                             sptr<AVSessionItem>& session);

    int32_t CastAudioInner(const std::vector<AudioStandard::AudioDeviceDescriptor>& sinkAudioDescriptors,
                           const std::string& sourceSessionInfo,
                           const sptr<AVSessionItem>& session);

    int32_t CancelCastAudioInner(const std::vector<AudioStandard::AudioDeviceDescriptor>& sinkDevices,
                                 const std::string& sourceSessionInfo,
                                 const sptr<AVSessionItem>& session);

    int32_t RemoteCastAudioInner(const std::string& sourceSessionInfo, std::string& sinkSessionInfo);

    int32_t RemoteCancelCastAudioInner(const std::string& sessionInfo);

    int32_t ProcessCastAudioCommand(const RemoteServiceCommand command, const std::string& input,
                                    std::string& output) override;

    int32_t CastAudioForNewSession(const sptr<AVSessionItem>& session);

    bool IsLocalDevice(const std::string& networkId);

    int32_t GetLocalNetworkId(std::string& networkId);

    int32_t GetTrustedDeviceName(const std::string& networkId, std::string& deviceName);

    int32_t GetTrustedDevicesInfo(std::vector<OHOS::DistributedHardware::DmDeviceInfo>& deviceList);

    int32_t SetBasicInfo(std::string& sessionInfo);

    void SetCastDeviceInfo(const std::vector<AudioStandard::AudioDeviceDescriptor>& castAudioDescriptors,
                           sptr<AVSessionItem>& session);

    int32_t GetAudioDescriptor(const std::string deviceId,
                               std::vector<AudioStandard::AudioDeviceDescriptor>& audioDeviceDescriptors);

    bool GetAudioDescriptorByDeviceId(const std::vector<sptr<AudioStandard::AudioDeviceDescriptor>>& descriptors,
                                      const std::string& deviceId,
                                      AudioStandard::AudioDeviceDescriptor& audioDescriptor);

    void GetCastDeviceInfo(const sptr<AVSessionItem>& session,
                           const std::vector<AudioStandard::AudioDeviceDescriptor>& descriptors,
                           std::vector<AudioStandard::AudioDeviceDescriptor>& castSinkDescriptors,
                           std::vector<AudioStandard::AudioDeviceDescriptor>& cancelSinkDescriptors);

    int32_t SelectOutputDevice(const int32_t uid, const AudioStandard::AudioDeviceDescriptor& descriptor);

    int32_t StartDefaultAbilityByCall(std::string& sessionId);

    void HandleEventHandlerCallBack();

    const nlohmann::json& GetSubNode(const nlohmann::json& node, const std::string& name);

    std::atomic<uint32_t> sessionSeqNum_ {};

    std::recursive_mutex sessionAndControllerLock_;
    sptr<AVSessionItem> topSession_;
    std::map<pid_t, std::list<sptr<AVControllerItem>>> controllers_;

    std::recursive_mutex clientDeathObserversLock_;
    std::map<pid_t, sptr<IClientDeath>> clientDeathObservers_;

    std::recursive_mutex sessionListenersLock_;
    std::map<pid_t, sptr<ISessionListener>> sessionListeners_;
    std::list<SessionListener*> innerSessionListeners_;

    std::recursive_mutex abilityManagerLock_;
    std::map<std::string, std::shared_ptr<AbilityManagerAdapter>> abilityManager_;

    FocusSessionStrategy focusSessionStrategy_;
    BackgroundAudioController backgroundAudioController_;

    std::recursive_mutex castAudioSessionMapLock_;
    std::map<std::string, std::string> castAudioSessionMap_;

    std::recursive_mutex isAllSessionCastLock_;
    bool isAllSessionCast_ {};

    std::recursive_mutex outputDeviceIdLock_;
    std::string outputDeviceId_;

    std::unique_ptr<AVSessionDumper> dumpHelper_ {};
    friend class AVSessionDumper;

    static constexpr const char *ABILITY_FILE_NAME = "abilityinfo";
    static constexpr const char *DEFAULT_BUNDLE_NAME = "com.example.himusicdemo";
    static constexpr const char *DEFAULT_ABILITY_NAME = "MainAbility";

    const std::string AVSESSION_FILE_DIR = "/data/service/el1/public/av_session/";

    int32_t pressCount_ {};
    bool isFirstPress_ = true;

    const int32_t ONE_CLICK = 1;
    const int32_t DOUBLE_CLICK = 2;
    const int32_t THREE_CLICK = 3;
    const int32_t CLICK_TIMEOUT = 500;
};

class ClientDeathRecipient : public IRemoteObject::DeathRecipient {
public:
    explicit ClientDeathRecipient(const std::function<void()>& callback);

    void OnRemoteDied(const wptr<IRemoteObject>& object) override;

private:
    std::function<void()> callback_;
};
} // namespace OHOS::AVSession
#endif // OHOS_AVSESSION_SERVICE_H