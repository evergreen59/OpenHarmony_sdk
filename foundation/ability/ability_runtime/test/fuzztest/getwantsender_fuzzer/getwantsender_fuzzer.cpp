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

#include "getwantsender_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "ability_manager_client.h"
#include "ability_record.h"
#include "parcel.h"
#include "sender_info.h"
#include "want_sender_info.h"

using namespace OHOS::AAFwk;
using namespace OHOS::AppExecFwk;

namespace OHOS {
namespace {
constexpr size_t FOO_MAX_LEN = 1024;
constexpr size_t U32_AT_SIZE = 4;
}
sptr<Token> GetFuzzAbilityToken()
{
    sptr<Token> token = nullptr;

    AbilityRequest abilityRequest;
    abilityRequest.appInfo.bundleName = "com.example.fuzzTest";
    abilityRequest.abilityInfo.name = "MainAbility";
    abilityRequest.abilityInfo.type = AbilityType::DATA;
    std::shared_ptr<AbilityRecord> abilityRecord = AbilityRecord::CreateAbilityRecord(abilityRequest);
    if (abilityRecord) {
        token = abilityRecord->GetToken();
    }

    return token;
}

bool DoSomethingInterestingWithMyAPI(const char* data, size_t size)
{
    auto abilityMgr = AbilityManagerClient::GetInstance();
    if (!abilityMgr) {
        std::cout << "Get ability manager client failed." << std::endl;
        return false;
    }

    // get token
    sptr<Token> token = GetFuzzAbilityToken();
    if (!token) {
        std::cout << "Get ability token failed." << std::endl;
        return false;
    }

    // fuzz for wantSenderInfo
    Parcel wantSenderInfoParcel;
    WantSenderInfo *wantSenderInfo = nullptr;
    sptr<IWantSender> sender = nullptr;
    if (wantSenderInfoParcel.WriteBuffer(data, size)) {
        wantSenderInfo = WantSenderInfo::Unmarshalling(wantSenderInfoParcel);
        if (wantSenderInfo) {
            sender = abilityMgr->GetWantSender(*wantSenderInfo, token);
        }
    }

    // fuzz for senderInfo
    Parcel senderInfoParcel;
    SenderInfo *senderInfo = nullptr;
    if (senderInfoParcel.WriteBuffer(data, size)) {
        senderInfo = SenderInfo::Unmarshalling(senderInfoParcel);
        if (sender && senderInfo) {
            abilityMgr->SendWantSender(sender, *senderInfo);
        }
    }

    if (senderInfo) {
        delete senderInfo;
        senderInfo = nullptr;
    }
    if (wantSenderInfo) {
        delete wantSenderInfo;
        wantSenderInfo = nullptr;
    }

    return true;
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

