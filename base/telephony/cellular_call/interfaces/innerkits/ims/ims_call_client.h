/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#ifndef TELEPHONY_IMS_CALL_CLIENT_H
#define TELEPHONY_IMS_CALL_CLIENT_H

#include "event_runner.h"
#include "ims_call_interface.h"
#include "ims_core_service_interface.h"
#include "iremote_stub.h"
#include "rwlock.h"
#include "singleton.h"
#include "system_ability_status_change_stub.h"

namespace OHOS {
namespace Telephony {
class ImsCallClient {
    DECLARE_DELAYED_SINGLETON(ImsCallClient);

public:
    /**
     * Get ImsCall Remote Object
     *
     * @return sptr<ImsCallInterface>
     */
    sptr<ImsCallInterface> GetImsCallProxy();

    void Init();
    void UnInit();
    int32_t RegisterImsCallCallbackHandler(int32_t slotId, const std::shared_ptr<AppExecFwk::EventHandler> &handler);

    /**
     * Get Handler
     *
     * @param slotId
     * @return AppExecFwk::EventHandler
     */
    std::shared_ptr<AppExecFwk::EventHandler> GetHandler(int32_t slotId);

    /****************** call basic ******************/
    int32_t Dial(const ImsCallInfo &callInfo, CLIRMode mode);
    int32_t HangUp(const ImsCallInfo &callInfo);
    int32_t Reject(const ImsCallInfo &callInfo);
    int32_t RejectWithReason(const ImsCallInfo &callInfo, const ImsRejectReason &reason);
    int32_t Answer(const ImsCallInfo &callInfo);
    int32_t HoldCall(int32_t slotId, int32_t callType);
    int32_t UnHoldCall(int32_t slotId, int32_t callType);
    int32_t SwitchCall(int32_t slotId, int32_t callType);
    int32_t CombineConference(int32_t slotId);
    int32_t InviteToConference(int32_t slotId, const std::vector<std::string> &numberList);
    int32_t KickOutFromConference(int32_t slotId, const std::vector<std::string> &numberList);
    int32_t UpdateImsCallMode(const ImsCallInfo &callInfo, ImsCallMode mode);
    int32_t GetImsCallsDataRequest(int32_t slotId, int64_t lastCallsDataFlag);
    int32_t GetLastCallFailReason(int32_t slotId);

    /****************** dtmf rtt ******************/
    int32_t StartDtmf(int32_t slotId, char cDtmfCode, int32_t index);
    int32_t SendDtmf(int32_t slotId, char cDtmfCode, int32_t index);
    int32_t StopDtmf(int32_t slotId, int32_t index);
    int32_t StartRtt(int32_t slotId, const std::string &msg);
    int32_t StopRtt(int32_t slotId);

    /****************** ims config ******************/
    int32_t SetDomainPreferenceMode(int32_t slotId, int32_t mode);
    int32_t GetDomainPreferenceMode(int32_t slotId);
    int32_t SetImsSwitchStatus(int32_t slotId, int32_t active);
    int32_t GetImsSwitchStatus(int32_t slotId);
    int32_t SetImsConfig(ImsConfigItem item, const std::string &value);
    int32_t SetImsConfig(ImsConfigItem item, int32_t value);
    int32_t GetImsConfig(ImsConfigItem item);
    int32_t SetImsFeatureValue(FeatureType type, int32_t value);
    int32_t GetImsFeatureValue(FeatureType type, int32_t &value);
    int32_t SetMute(int32_t slotId, int32_t mute);
    int32_t GetMute(int32_t slotId);

    /****************** video settings ******************/
    int32_t CtrlCamera(const std::u16string &cameraId, int32_t callingUid, int32_t callingPid);
    int32_t SetPreviewWindow(int32_t x, int32_t y, int32_t z, int32_t width, int32_t height);
    int32_t SetDisplayWindow(int32_t x, int32_t y, int32_t z, int32_t width, int32_t height);
    int32_t SetCameraZoom(float zoomRatio);
    int32_t SetPauseImage(const std::u16string &path);
    int32_t SetDeviceDirection(int32_t rotation);

    /****************** supplement ******************/
    int32_t SetClip(int32_t slotId, int32_t action);
    int32_t GetClip(int32_t slotId);
    int32_t SetClir(int32_t slotId, int32_t action);
    int32_t GetClir(int32_t slotId);
    int32_t SetCallTransfer(
        int32_t slotId, int32_t reason, int32_t mode, const std::string &transferNum, int32_t classType);
    int32_t GetCallTransfer(int32_t slotId, int32_t reason);
    int32_t SetCallRestriction(int32_t slotId, const std::string &fac, int32_t mode, const std::string &pw);
    int32_t GetCallRestriction(int32_t slotId, const std::string &fac);
    int32_t SetCallWaiting(int32_t slotId, bool activate, int32_t classType);
    int32_t GetCallWaiting(int32_t slotId);
    int32_t SetColr(int32_t slotId, int32_t presentation);
    int32_t GetColr(int32_t slotId);
    int32_t SetColp(int32_t slotId, int32_t action);
    int32_t GetColp(int32_t slotId);
    int32_t UpdateImsCapabilities(int32_t slotId, const ImsCapabilityList &imsCapabilityList);

private:
    class SystemAbilityListener : public SystemAbilityStatusChangeStub {
    public:
        SystemAbilityListener() {}
        ~SystemAbilityListener() {}
    public:
        void OnAddSystemAbility(int32_t systemAbilityId, const std::string& deviceId) override;
        void OnRemoveSystemAbility(int32_t systemAbilityId, const std::string& deviceId) override;
    };

    /**
     * Is Connect ImsCall Remote Object
     *
     * @return bool
     */
    bool IsConnect() const;
    int32_t RegisterImsCallCallback();
    int32_t ReConnectService();
    void Clean();

private:
    sptr<ImsCoreServiceInterface> imsCoreServiceProxy_ = nullptr;
    sptr<ImsCallInterface> imsCallProxy_ = nullptr;
    sptr<ImsCallCallbackInterface> imsCallCallback_ = nullptr;
    std::map<int32_t, std::shared_ptr<AppExecFwk::EventHandler>> handlerMap_;
    Utils::RWLock rwClientLock_;
    sptr<ISystemAbilityStatusChange> statusChangeListener_ = nullptr;
};
} // namespace Telephony
} // namespace OHOS

#endif // TELEPHONY_IMS_CORE_SERVICE_CLIENT_H
