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

#include "modulerunningrecord_fuzzer.h"

#include <cstddef>
#include <cstdint>

#define private public
#include "module_running_record.h"
#undef private
#include "ability_record.h"
#include "message_parcel.h"
#include "securec.h"

using namespace OHOS::AAFwk;
using namespace OHOS::AppExecFwk;

namespace OHOS {
namespace {
constexpr size_t FOO_MAX_LEN = 1024;
constexpr size_t U32_AT_SIZE = 4;
constexpr uint8_t ENABLE = 2;
constexpr size_t OFFSET_ZERO = 24;
constexpr size_t OFFSET_ONE = 16;
constexpr size_t OFFSET_TWO = 8;
}
uint32_t GetU32Data(const char* ptr)
{
    // convert fuzz input data to an integer
    return (ptr[0] << OFFSET_ZERO) | (ptr[1] << OFFSET_ONE) | (ptr[2] << OFFSET_TWO) | ptr[3];
}
sptr<Token> GetFuzzAbilityToken()
{
    sptr<Token> token = nullptr;

    AbilityRequest abilityRequest;
    abilityRequest.appInfo.bundleName = "com.example.fuzzTest";
    abilityRequest.abilityInfo.name = "MainAbility";
    abilityRequest.abilityInfo.type = AbilityType::PAGE;
    std::shared_ptr<AbilityRecord> abilityRecord = AbilityRecord::CreateAbilityRecord(abilityRequest);
    if (abilityRecord) {
        token = abilityRecord->GetToken();
    }

    return token;
}
bool DoSomethingInterestingWithMyAPI(const char* data, size_t size)
{
    std::shared_ptr<ApplicationInfo> info;
    std::shared_ptr<AMSEventHandler> eventHandler;
    ModuleRunningRecord moduleRecord(info, eventHandler);
    HapModuleInfo hapInfo;
    moduleRecord.Init(hapInfo);
    std::weak_ptr<AppMgrServiceInner> inner;
    moduleRecord.SetAppMgrServiceInner(inner);
    ModuleRecordState moduleState = ModuleRecordState::UNKNOWN_STATE;
    moduleRecord.SetModuleRecordState(moduleState);
    std::shared_ptr<AppLifeCycleDeal> appLifeCycleDeal;
    moduleRecord.SetApplicationClient(appLifeCycleDeal);
    sptr<IRemoteObject> token = GetFuzzAbilityToken();
    moduleRecord.GetAbilityRunningRecordByToken(token);
    std::shared_ptr<AbilityInfo> abilityInfo;
    Parcel wantParcel;
    std::shared_ptr<Want> want;
    moduleRecord.AddAbility(token, abilityInfo, want);
    int64_t eventId = static_cast<int64_t>(GetU32Data(data));
    moduleRecord.GetAbilityRunningRecord(eventId);
    std::shared_ptr<AbilityRunningRecord> record;
    moduleRecord.LaunchAbility(record);
    moduleRecord.LaunchPendingAbilities();
    AppExecFwk::AbilityState state = AppExecFwk::AbilityState::ABILITY_STATE_READY;
    moduleRecord.OnAbilityStateChanged(record, state);
    bool isForce = *data % ENABLE;
    std::shared_ptr<AppRunningRecord> appRunningRecord;
    moduleRecord.TerminateAbility(appRunningRecord, token, isForce);
    moduleRecord.AbilityTerminated(token);
    moduleRecord.GetAbilityByTerminateLists(token);
    uint32_t msg = GetU32Data(data);
    int64_t timeOut = static_cast<int64_t>(GetU32Data(data));
    moduleRecord.SendEvent(msg, timeOut, record);
    moduleRecord.RemoveTerminateAbilityTimeoutTask(token);
    moduleRecord.GetModuleName();
    moduleRecord.GetPageAbilitySize();
    moduleRecord.GetModuleRecordState();
    moduleRecord.GetAppInfo();
    moduleRecord.GetState();
    return moduleRecord.IsLastAbilityRecord(token);
}
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    if (data == nullptr) {
        std::cout << "invalid data" << std::endl;
        return 0;
    }

    /* Validate the length of size */
    if (size > OHOS::FOO_MAX_LEN || size < OHOS::U32_AT_SIZE) {
        return 0;
    }

    char* ch = (char *)malloc(size + 1);
    if (ch == nullptr) {
        std::cout << "malloc failed." << std::endl;
        return 0;
    }

    (void)memset_s(ch, size + 1, 0x00, size + 1);
    if (memcpy_s(ch, size, data, size) != EOK) {
        std::cout << "copy failed." << std::endl;
        free(ch);
        ch = nullptr;
        return 0;
    }

    OHOS::DoSomethingInterestingWithMyAPI(ch, size);
    free(ch);
    ch = nullptr;
    return 0;
}
