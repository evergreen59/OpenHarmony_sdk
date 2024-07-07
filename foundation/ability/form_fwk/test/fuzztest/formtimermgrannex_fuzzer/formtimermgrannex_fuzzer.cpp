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

#include "formtimermgrannex_fuzzer.h"

#include <cstddef>
#include <cstdint>

#define private public
#define protected public
#include "form_info_mgr.h"
#include "form_timer_mgr.h"
#undef private
#undef protected
#include "securec.h"

using namespace OHOS::AppExecFwk;

namespace OHOS {
constexpr size_t FOO_MAX_LEN = 1024;
constexpr size_t U32_AT_SIZE = 4;
uint32_t GetU32Data(const char* ptr)
{
    // convert fuzz input data to an integer
    return (ptr[0] << 24) | (ptr[1] << 16) | (ptr[2] << 8) | ptr[3];
}
bool DoSomethingInterestingWithMyAPI(const char* data, size_t size)
{
    FormTimerMgr formTimerMgr;
    formTimerMgr.ClearLimiterTimerResource();
    formTimerMgr.GetLimiterWantAgent();
    formTimerMgr.UpdateDynamicAlarm();
    int64_t nextTime = static_cast<int64_t>(GetU32Data(data));
    int32_t userId = static_cast<int32_t>(GetU32Data(data));
    formTimerMgr.GetDynamicWantAgent(nextTime, userId);
    formTimerMgr.ClearDynamicResource();
    long nowTime = static_cast<long>(GetU32Data(data));
    UpdateAtItem updateAtItem;
    formTimerMgr.FindNextAtTimerItem(nowTime, updateAtItem);
    formTimerMgr.ClearIntervalTimer();
    formTimerMgr.CreatTaskThreadExecutor();
    FormTimer timerTask;
    formTimerMgr.ExecTimerTask(timerTask);
    formTimerMgr.Init();
    FormInfoHelper formInfoHelper;
    std::string bundleName(data, size);
    FormInfo formInfo;
    std::vector<FormInfo> formInfos;
    formInfos.emplace_back(formInfo);
    formInfoHelper.LoadFormConfigInfoByBundleName(bundleName, formInfos, userId);
    return formTimerMgr.IsActiveUser(userId);
}
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    if (data == nullptr) {
        return 0;
    }

    if (size < OHOS::U32_AT_SIZE) {
        return 0;
    }

    /* Validate the length of size */
    if (size == 0 || size > OHOS::FOO_MAX_LEN) {
        return 0;
    }

    char* ch = (char *)malloc(size + 1);
    if (ch == nullptr) {
        return 0;
    }

    (void)memset_s(ch, size + 1, 0x00, size + 1);
    if (memcpy_s(ch, size, data, size) != EOK) {
        free(ch);
        ch = nullptr;
        return 0;
    }

    OHOS::DoSomethingInterestingWithMyAPI(ch, size);
    free(ch);
    ch = nullptr;
    return 0;
}

