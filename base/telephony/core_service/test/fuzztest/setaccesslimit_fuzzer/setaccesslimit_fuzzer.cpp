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

#include "setaccesslimit_fuzzer.h"

#include <cstddef>
#include <cstdint>

#include "addcoreservicetoken_fuzzer.h"
#include "icc_operator_rule.h"
#include "napi_util.h"
#include "system_ability_definition.h"

using namespace OHOS::Telephony;
namespace OHOS {
void DoSomethingInterestingWithMyAPI(const uint8_t *data, size_t size)
{
    if (data == nullptr || size == 0) {
        return;
    }

    auto iccOperatorRule = std::make_shared<IccOperatorRule>();
    std::string result(reinterpret_cast<const char *>(data), size);
    std::string packageName(reinterpret_cast<const char *>(data), size);
    std::string hexStr(reinterpret_cast<const char *>(data), size);
    std::string certificate(reinterpret_cast<const char *>(data), size);
    std::string accessLimit(reinterpret_cast<const char *>(data), size);
    iccOperatorRule->GetPackageName(result);
    iccOperatorRule->GetPackageName(packageName);
    iccOperatorRule->SetPackageNameByHexStr(hexStr);
    iccOperatorRule->GetCertificate(result);
    iccOperatorRule->SetCertificate(certificate);
    iccOperatorRule->SetAccessLimit(accessLimit);
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    OHOS::AddCoreServiceTokenFuzzer token;
    /* Run your code on data */
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}
