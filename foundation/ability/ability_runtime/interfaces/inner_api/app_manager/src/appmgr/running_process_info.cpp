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

#include "running_process_info.h"

#include "nlohmann/json.hpp"
#include "string_ex.h"

#include "hilog_wrapper.h"
#include "parcel_macro_base.h"

namespace OHOS {
namespace AppExecFwk {
namespace {
const std::string JSON_KEY_PROCESSNAME = "processName";
const std::string JSON_KEY_PID = "pid";
const std::string JSON_KEY_STATE = "state";
}  // namespace

bool RunningProcessInfo::ReadFromParcel(Parcel &parcel)
{
    processName_ = Str16ToStr8(parcel.ReadString16());
    int32_t typeData;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, typeData);
    pid_ = static_cast<int32_t>(typeData);
    int32_t uidData;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, uidData);
    uid_ = static_cast<int32_t>(uidData);
    int32_t stateData;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, stateData);
    state_ = static_cast<AppProcessState>(stateData);
    isContinuousTask = parcel.ReadBool();
    isKeepAlive = parcel.ReadBool();
    isFocused = parcel.ReadBool();
    isTestProcess = parcel.ReadBool();
    if (!parcel.ReadStringVector(&bundleNames)) {
        HILOG_ERROR("read bundleNames failed.");
        return false;
    }
    return true;
}

RunningProcessInfo *RunningProcessInfo::Unmarshalling(Parcel &parcel)
{
    RunningProcessInfo *info = new (std::nothrow) RunningProcessInfo();
    if (info && !info->ReadFromParcel(parcel)) {
        HILOG_WARN("read from parcel failed");
        delete info;
        info = nullptr;
    }
    return info;
}

bool RunningProcessInfo::Marshalling(Parcel &parcel) const
{
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(processName_));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, static_cast<int32_t>(pid_));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, static_cast<int32_t>(uid_));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, static_cast<int32_t>(state_));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, isContinuousTask);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, isKeepAlive);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, isFocused);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Bool, parcel, isTestProcess);
    if (!parcel.WriteStringVector(bundleNames)) {
        HILOG_ERROR("write bundleNames failed.");
        return false;
    }
    return true;
}
}  // namespace AppExecFwk
}  // namespace OHOS
