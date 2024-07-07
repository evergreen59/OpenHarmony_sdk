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

#include "ability_running_record.h"

namespace OHOS {
namespace AppExecFwk {
AbilityRunningRecord::AbilityRunningRecord(const std::shared_ptr<AbilityInfo> &info, const sptr<IRemoteObject> &token)
    : info_(info), token_(token)
{}

AbilityRunningRecord::~AbilityRunningRecord()
{}

const std::string &AbilityRunningRecord::GetName() const
{
    return info_->name;
}

const std::shared_ptr<AbilityInfo> &AbilityRunningRecord::GetAbilityInfo() const
{
    return info_;
}

const std::shared_ptr<AAFwk::Want> &AbilityRunningRecord::GetWant() const
{
    return want_;
}

void AbilityRunningRecord::SetWant(const std::shared_ptr<AAFwk::Want> &want)
{
    want_ = want;
}

const sptr<IRemoteObject> &AbilityRunningRecord::GetToken() const
{
    return token_;
}

void AbilityRunningRecord::SetState(const AbilityState state)
{
    state_ = state;
}

AbilityState AbilityRunningRecord::GetState() const
{
    return state_;
}

bool AbilityRunningRecord::IsSameState(const AbilityState state) const
{
    return state_ == state;
}

int32_t AbilityRunningRecord::GetLastLaunchTime() const
{
    return lastLaunchTime_;
}

const sptr<IRemoteObject> AbilityRunningRecord::GetPreToken() const
{
    return preToken_;
}

void AbilityRunningRecord::SetPreToken(const sptr<IRemoteObject> &preToken)
{
    preToken_ = preToken;
}

void AbilityRunningRecord::SetVisibility(const int32_t visibility)
{
    visibility_ = visibility;
}

int32_t AbilityRunningRecord::GetVisibility() const
{
    return visibility_;
}

void AbilityRunningRecord::SetPerceptibility(const int32_t perceptibility)
{
    perceptibility_ = perceptibility;
}

int32_t AbilityRunningRecord::GetPerceptibility() const
{
    return perceptibility_;
}

void AbilityRunningRecord::SetConnectionState(const int32_t connectionState)
{
    connectionState_ = connectionState;
}

int32_t AbilityRunningRecord::GetConnectionState() const
{
    return connectionState_;
}

void AbilityRunningRecord::SetEventId(const int64_t eventId)
{
    eventId_ = eventId;
}

int64_t AbilityRunningRecord::GetEventId() const
{
    return eventId_;
}

void AbilityRunningRecord::SetTerminating()
{
    isTerminating_ = true;
}

bool AbilityRunningRecord::IsTerminating() const
{
    return isTerminating_;
}

void AbilityRunningRecord::SetOwnerUserId(int32_t ownerUserId)
{
    ownerUserId_ = ownerUserId;
}

int32_t AbilityRunningRecord::GetOwnerUserId() const
{
    return ownerUserId_;
}

void AbilityRunningRecord::SetIsSingleUser(bool flag)
{
    isSingleUser_ = flag;
}

bool AbilityRunningRecord::IsSingleUser() const
{
    return isSingleUser_;
}

void AbilityRunningRecord::UpdateFocusState(bool isFocus)
{
    isFocused_ = isFocus;
}

bool AbilityRunningRecord::GetFocusFlag() const
{
    return isFocused_;
}
}  // namespace AppExecFwk
}  // namespace OHOS
