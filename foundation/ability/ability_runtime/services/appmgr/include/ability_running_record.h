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

#ifndef OHOS_ABILITY_RUNTIME_ABILITY_RUNNING_RECORD_H
#define OHOS_ABILITY_RUNTIME_ABILITY_RUNNING_RECORD_H

#include <string>

#include "ability_info.h"
#include "application_info.h"
#include "app_mgr_constants.h"
#include "iremote_object.h"
#include "want.h"

namespace OHOS {
namespace AppExecFwk {
class AbilityRunningRecord {
public:
    AbilityRunningRecord(const std::shared_ptr<AbilityInfo> &info, const sptr<IRemoteObject> &token);
    virtual ~AbilityRunningRecord();

    /**
     * @brief Obtains the name of the ability.
     *
     * @return Returns the ability name.
     */
    const std::string &GetName() const;

    /**
     * @brief Obtains the info of the ability.
     *
     * @return Returns the ability info.
     */
    const std::shared_ptr<AbilityInfo> &GetAbilityInfo() const;

    /**
     * @brief Obtains the info of the ability.
     *
     * @return Returns the ability want.
     */
    const std::shared_ptr<AAFwk::Want> &GetWant() const;

    void SetWant(const std::shared_ptr<AAFwk::Want> &want);

    /**
     * @brief Obtains the token of the ability.
     *
     * @return Returns the ability token.
     */
    const sptr<IRemoteObject> &GetToken() const;

    /**
     * @brief Setting id for ability record.
     *
     * @param appId, the ability record id.
     */
    void SetAppRunningRecordId(const int32_t appId);

    /**
     * @brief Setting state for ability record.
     *
     * @param state, the ability record state.
     */
    void SetState(const AbilityState state);

    /**
     * @brief Obtains the state of the ability.
     *
     * @return Returns the ability state.
     */
    AbilityState GetState() const;

    /**
     * @brief Judge whether the ability status is the same.
     *
     * @param state, the ability state.
     *
     * @return Returns If true is returned, the ID will be the same, otherwise it fails.
     */
    bool IsSameState(const AbilityState state) const;

    /**
     * @brief Obtains the last launch time of the ability record.
     *
     * @return Returns the last launch time.
     */
    int32_t GetLastLaunchTime() const;

    /**
     * @brief Setting the unique identification to call the ability.
     *
     * @param preToken, the unique identification to call the ability.
     */
    void SetPreToken(const sptr<IRemoteObject> &preToken);

    /**
     * @brief Obtains the unique identification to call the ability.
     *
     * @return Returns the unique identification to call the ability.
     */
    const sptr<IRemoteObject> GetPreToken() const;

    /**
     * @brief Setting the visibility to ability.
     *
     * @param preToken, the visibility to ability.
     */
    void SetVisibility(const int32_t visibility);

    /**
     * @brief Obtains the visibility to ability.
     *
     * @return Returns the visibility to ability.
     */
    int32_t GetVisibility() const;

    /**
     * @brief Setting the perceptibility to ability.
     *
     * @param preToken, the perceptibility to ability.
     */
    void SetPerceptibility(const int32_t perceptibility);

    /**
     * @brief Obtains the perceptibility to ability.
     *
     * @return Returns the perceptibility to ability.
     */
    int32_t GetPerceptibility() const;

    /**
     * @brief Setting the connection state to service ability.
     *
     * @param preToken, the connection state to service ability.
     */
    void SetConnectionState(const int32_t connectionState);

    /**
     * @brief Obtains the connection state to service ability.
     *
     * @return Returns the connection state to service ability.
     */
    int32_t GetConnectionState() const;

    /**
     * @brief Set the Terminating object.
     */
    void SetTerminating();

    /**
     * @brief Whether the ability is terminating.
     *
     * @return Returns whether the ability is terminating.
     */
    bool IsTerminating() const;

    void SetEventId(const int64_t eventId);
    int64_t GetEventId() const;

    void SetOwnerUserId(int32_t ownerUserId);
    int32_t GetOwnerUserId() const;
    void SetIsSingleUser(bool flag);
    bool IsSingleUser() const;
    void UpdateFocusState(bool isFocus);
    bool GetFocusFlag() const;

private:
    int32_t lastLaunchTime_ = 0;
    int32_t visibility_ = 0;
    int32_t perceptibility_ = 0;
    int32_t connectionState_ = 0;
    int64_t eventId_ = 0;
    bool isTerminating_ = false;
    AbilityState state_ = AbilityState::ABILITY_STATE_BEGIN;
    bool isFocused_ = false;
    std::shared_ptr<AbilityInfo> info_;
    std::shared_ptr<AAFwk::Want> want_ = nullptr;
    sptr<IRemoteObject> token_;
    sptr<IRemoteObject> preToken_;
    int32_t ownerUserId_ = -1;
    bool isSingleUser_ = false;
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // OHOS_ABILITY_RUNTIME_ABILITY_RUNNING_RECORD_H
