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

#include "grantpermission_fuzzer.h"

#include <string>
#include <vector>
#include <thread>
#undef private
#include "accesstoken_kit.h"

using namespace std;
using namespace OHOS::Security::AccessToken;

namespace OHOS {
    bool GrantPermissionFuzzTest(const uint8_t* data, size_t size)
    {
        int32_t result = RET_FAILED;
        if ((data == nullptr) || (size == 0)) {
            return false;
        }

        AccessTokenID TOKENID = static_cast<AccessTokenID>(size);
        std::string testName(reinterpret_cast<const char*>(data), size);
        result = AccessTokenKit::GrantPermission(TOKENID, testName, 0);

        return result == RET_SUCCESS;
    }
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::GrantPermissionFuzzTest(data, size);
    return 0;
}
