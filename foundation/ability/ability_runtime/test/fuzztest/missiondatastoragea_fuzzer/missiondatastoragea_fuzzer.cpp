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

#include "missiondatastoragea_fuzzer.h"

#include <cstddef>
#include <cstdint>

#define private public
#include "mission_data_storage.h"
#undef private

#include "ability_record.h"

using namespace OHOS::AAFwk;
using namespace OHOS::AppExecFwk;

namespace OHOS {
namespace {
constexpr size_t FOO_MAX_LEN = 1024;
constexpr size_t U32_AT_SIZE = 4;
constexpr uint8_t ENABLE = 2;
}

uint32_t GetU32Data(const char* ptr)
{
    // convert fuzz input data to an integer
    return (ptr[0] << 24) | (ptr[1] << 16) | (ptr[2] << 8) | ptr[3];
}

bool DoSomethingInterestingWithMyAPI(const char* data, size_t size)
{
    bool boolParam = *data % ENABLE;
    int intParam = static_cast<int>(GetU32Data(data));
    int32_t int32Param = static_cast<int32_t>(GetU32Data(data));
    std::string stringParam(data, size);
    MissionSnapshot missionSnapshot;
    InnerMissionInfo innerMissionInfo;

    // fuzz for MissionDataStorage
    auto missionDataStorage = std::make_shared<MissionDataStorage>(intParam);
    std::shared_ptr<AppExecFwk::EventHandler> handler;
    missionDataStorage->SetEventHandler(handler);
    std::list<InnerMissionInfo> missionInfoList;
    missionDataStorage->LoadAllMissionInfo(missionInfoList);
    missionDataStorage->SaveMissionInfo(innerMissionInfo);
    missionDataStorage->SaveMissionSnapshot(int32Param, missionSnapshot);
    missionDataStorage->GetMissionSnapshot(int32Param, missionSnapshot, boolParam);
    missionDataStorage->GetMissionDataDirPath();
    missionDataStorage->GetMissionDataFilePath(intParam);
    missionDataStorage->GetMissionSnapshotPath(int32Param, boolParam);
    missionDataStorage->CheckFileNameValid(stringParam);
    missionDataStorage->SaveSnapshotFile(int32Param, missionSnapshot);

    return true;
}
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    if (data == nullptr) {
        return 0;
    }

    /* Validate the length of size */
    if (size < OHOS::U32_AT_SIZE || size > OHOS::FOO_MAX_LEN) {
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

