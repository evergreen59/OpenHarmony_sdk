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

#ifndef ADD_DATA_TOKEN_FUZZER_H
#define ADD_DATA_TOKEN_FUZZER_H

#include "accesstoken_kit.h"

namespace OHOS {
class AddDataTokenFuzzer {
public:
    AddDataTokenFuzzer();
    ~AddDataTokenFuzzer();

private:
    Security::AccessToken::AccessTokenID currentID_ = 0;
};
} // namespace OHOS
#endif // ADD_DATA_TOKEN_FUZZER_H
