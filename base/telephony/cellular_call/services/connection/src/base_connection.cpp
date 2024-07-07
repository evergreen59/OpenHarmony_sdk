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

#include "base_connection.h"

#include "securec.h"

namespace OHOS {
namespace Telephony {
BaseConnection::BaseConnection()
{
    (void)memset_s(&callReportInfo_, sizeof(CallReportInfo), 0, sizeof(callReportInfo_));
}

void BaseConnection::SetOrUpdateCallReportInfo(CallReportInfo &callReportInfo)
{
    callReportInfo_ = callReportInfo;
}

CallReportInfo BaseConnection::GetCallReportInfo()
{
    return callReportInfo_;
}

void BaseConnection::SetStatus(TelCallState state)
{
    callReportInfo_.state = state;
}

TelCallState BaseConnection::GetStatus() const
{
    return callReportInfo_.state;
}

bool BaseConnection::IsRingingState() const
{
    return this->GetStatus() == TelCallState::CALL_STATUS_INCOMING ||
        this->GetStatus() == TelCallState::CALL_STATUS_WAITING ||
        this->GetStatus() == TelCallState::CALL_STATUS_ALERTING;
}

void BaseConnection::SetFlag(bool flag)
{
    flag_ = flag;
}

bool BaseConnection::GetFlag() const
{
    return flag_;
}

void BaseConnection::SetIndex(int32_t index)
{
    index_ = index;
}

int32_t BaseConnection::GetIndex() const
{
    return index_;
}
} // namespace Telephony
} // namespace OHOS