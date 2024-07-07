/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef I_CALL_ABILITY_CALLBACK_H
#define I_CALL_ABILITY_CALLBACK_H

#include "pac_map.h"
#include "iremote_broker.h"

#include "call_manager_inner_type.h"

namespace OHOS {
namespace Telephony {
class ICallAbilityCallback : public IRemoteBroker {
public:
    ICallAbilityCallback() : bundleName_("") {}
    virtual ~ICallAbilityCallback() = default;

    virtual int32_t OnCallDetailsChange(const CallAttributeInfo &info) = 0;
    virtual int32_t OnCallEventChange(const CallEventInfo &info) = 0;
    virtual int32_t OnCallDisconnectedCause(const DisconnectedDetails &details) = 0;
    virtual int32_t OnReportAsyncResults(CallResultReportId reportId, AppExecFwk::PacMap &resultInfo) = 0;
    virtual int32_t OnOttCallRequest(OttCallRequestId requestId, AppExecFwk::PacMap &info) = 0;
    virtual int32_t OnReportMmiCodeResult(const MmiCodeInfo &info) = 0;
    void SetBundleName(const std::string &name)
    {
        bundleName_ = name;
    }

    std::string GetBundleName()
    {
        return bundleName_;
    }

    enum CallManagerCallAbilityCode {
        UPDATE_CALL_STATE_INFO = 0,
        UPDATE_CALL_EVENT,
        UPDATE_CALL_DISCONNECTED_CAUSE,
        UPDATE_CALL_ASYNC_RESULT_REQUEST,
        REPORT_OTT_CALL_REQUEST,
        UPDATE_MMI_CODE_RESULT_REQUEST,
    };

public:
    std::string bundleName_;
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.Telephony.ICallAbilityCallback");
};
} // namespace Telephony
} // namespace OHOS

#endif
