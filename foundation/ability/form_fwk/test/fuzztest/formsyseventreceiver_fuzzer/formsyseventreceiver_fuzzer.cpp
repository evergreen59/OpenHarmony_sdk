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

#include "formsyseventreceiver_fuzzer.h"

#include <cstddef>
#include <cstdint>

#define private public
#define protected public
#include "form_sys_event_receiver.h"
#undef private
#undef protected
#include "securec.h"

using namespace OHOS::AppExecFwk;

namespace OHOS {
constexpr size_t FOO_MAX_LEN = 1024;
constexpr size_t U32_AT_SIZE = 4;
constexpr uint8_t ENABLE = 2;
uint32_t GetU32Data(const char* ptr)
{
    // convert fuzz input data to an integer
    return (ptr[0] << 24) | (ptr[1] << 16) | (ptr[2] << 8) | ptr[3];
}
bool DoSomethingInterestingWithMyAPI(const char* data, size_t size)
{
    EventFwk::CommonEventSubscribeInfo subscriberInfo;
    FormSysEventReceiver formSysEventReceiver(subscriberInfo);
    EventFwk::CommonEventData eventData;
    formSysEventReceiver.OnReceiveEvent(eventData);
    int64_t formId = static_cast<int64_t>(GetU32Data(data));
    FormRecord formRecord;
    FormInfo formInfo;
    std::vector<FormInfo> targetForms;
    targetForms.emplace_back(formInfo);
    std::string bundleName(data, size);
    int userId = static_cast<int>(GetU32Data(data));
    formSysEventReceiver.formEventHelper_.HandleProviderUpdated(bundleName, userId);
    int32_t userIds = static_cast<int32_t>(GetU32Data(data));
    formSysEventReceiver.formEventHelper_.HandleProviderRemoved(bundleName, userIds);
    BundlePackInfo bundlePackInfo;
    formSysEventReceiver.formEventHelper_.ProviderFormUpdated(formId, formRecord, bundlePackInfo);
    formSysEventReceiver.formEventHelper_.HandleBundleFormInfoChanged(bundleName, userIds);
    formSysEventReceiver.formEventHelper_.HandleBundleFormInfoRemoved(bundleName, userIds);
    formSysEventReceiver.formEventHelper_.HandleBundleDataCleared(bundleName, userIds);
    int uid = static_cast<int>(GetU32Data(data));
    formSysEventReceiver.formEventHelper_.HandleFormHostDataCleared(uid);
    bool flag = *data % ENABLE;
    std::map<int64_t, bool> removedFormsMap;
    removedFormsMap.emplace(formId, flag);
    formSysEventReceiver.formEventHelper_.ClearFormDBRecordData(uid, removedFormsMap);
    formSysEventReceiver.formEventHelper_.ClearTempFormRecordData(uid, removedFormsMap);
    std::string abilityName;
    FormIdKey formIdKey(bundleName, abilityName);
    std::set<int64_t> formNums;
    formNums.insert(formId);
    std::map<FormIdKey, std::set<int64_t>> noHostFormDbMap;
    noHostFormDbMap.emplace(formIdKey, formNums);
    formSysEventReceiver.formEventHelper_.BatchDeleteNoHostDBForms(uid, noHostFormDbMap, removedFormsMap);
    formSysEventReceiver.formEventHelper_.BatchDeleteNoHostTempForms(uid, noHostFormDbMap, removedFormsMap);
    formSysEventReceiver.formEventHelper_.ReCreateForm(formId);
    FormTimerCfg cfg;
    formSysEventReceiver.formEventHelper_.HandleTimerUpdate(formId, formRecord, cfg);
    formSysEventReceiver.HandleUserIdRemoved(userIds);
    formSysEventReceiver.HandleBundleScanFinished(userIds);
    return formSysEventReceiver.formEventHelper_.ProviderFormUpdated(formId, formRecord, targetForms);
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

