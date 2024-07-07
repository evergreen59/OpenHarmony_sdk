/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include "ims_conference.h"

#include <string_ex.h>

#include "call_base.h"
#include "call_object_manager.h"
#include "call_manager_errors.h"
#include "telephony_log_wrapper.h"
#include "call_manager_inner_type.h"

namespace OHOS {
namespace Telephony {
ImsConference::ImsConference() : ConferenceBase()
{
    conferenceType_ = CallType::TYPE_IMS;
    maxSubCallLimits_ = CS_CONFERENCE_MAX_CALLS_CNT;
    // get from configuration file
#ifdef ABILITY_CONFIG_SUPPORT
    maxSubCallLimits_ = GetConfig(IMS_CONFERENCE_SUB_CALL_LIMITS);
#endif
}

ImsConference::~ImsConference() {}

int32_t ImsConference::JoinToConference(int32_t callId)
{
    std::lock_guard<std::mutex> lock(conferenceMutex_);
    if (state_ != CONFERENCE_STATE_CREATING && state_ != CONFERENCE_STATE_ACTIVE &&
        state_ != CONFERENCE_STATE_LEAVING) {
        TELEPHONY_LOGE("the current conference status does not allow CombineConference");
        return CALL_ERR_ILLEGAL_CALL_OPERATION;
    }
    if (subCallIdSet_.size() >= maxSubCallLimits_) {
        TELEPHONY_LOGE("already %{public}zu calls in the conference, exceed limits!", subCallIdSet_.size());
        return CALL_ERR_CONFERENCE_CALL_EXCEED_LIMIT;
    }

    subCallIdSet_.insert(callId);
    state_ = CONFERENCE_STATE_ACTIVE;
    beginTime_ = time(nullptr);
    TELEPHONY_LOGI("JoinToConference success, callId:%{public}d", callId);
    return TELEPHONY_SUCCESS;
}

int32_t ImsConference::LeaveFromConference(int32_t callId)
{
    std::lock_guard<std::mutex> lock(conferenceMutex_);
    if (subCallIdSet_.find(callId) != subCallIdSet_.end()) {
        subCallIdSet_.erase(callId);
    } else {
        TELEPHONY_LOGE("separate conference failed, callId %{public}d not in conference", callId);
        return CALL_ERR_CONFERENCE_SEPERATE_FAILED;
    }
    if (subCallIdSet_.empty()) {
        mainCallId_ = ERR_ID;
        state_ = CONFERENCE_STATE_IDLE;
        beginTime_ = 0;
    }
    return TELEPHONY_SUCCESS;
}

int32_t ImsConference::HoldConference(int32_t callId)
{
    std::lock_guard<std::mutex> lock(conferenceMutex_);
    if (state_ == CONFERENCE_STATE_HOLDING) {
        TELEPHONY_LOGI("HoldConference success");
        return TELEPHONY_SUCCESS;
    }
    if (subCallIdSet_.find(callId) != subCallIdSet_.end()) {
        subCallIdSet_.erase(callId);
    } else {
        TELEPHONY_LOGE("separate conference failed, callId %{public}d not in conference", callId);
        return CALL_ERR_CONFERENCE_SEPERATE_FAILED;
    }
    if (subCallIdSet_.empty()) {
        mainCallId_ = ERR_ID;
        state_ = CONFERENCE_STATE_IDLE;
        beginTime_ = 0;
    }
    return TELEPHONY_SUCCESS;
}

int32_t ImsConference::CanCombineConference()
{
    std::lock_guard<std::mutex> lock(conferenceMutex_);
    if (subCallIdSet_.size() >= maxSubCallLimits_) {
        TELEPHONY_LOGE("already %{public}zu calls in the conference, exceed limits!", subCallIdSet_.size());
        return CALL_ERR_CONFERENCE_CALL_EXCEED_LIMIT;
    }
    return TELEPHONY_SUCCESS;
}

int32_t ImsConference::CanSeparateConference()
{
    std::lock_guard<std::mutex> lock(conferenceMutex_);
    if (subCallIdSet_.empty() || state_ != CONFERENCE_STATE_ACTIVE) {
        TELEPHONY_LOGE("no call is currently in the conference!");
        return CALL_ERR_CONFERENCE_NOT_EXISTS;
    }
    return TELEPHONY_SUCCESS;
}
} // namespace Telephony
} // namespace OHOS
