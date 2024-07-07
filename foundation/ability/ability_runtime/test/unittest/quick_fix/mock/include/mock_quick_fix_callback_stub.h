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

#ifndef OHOS_ABILITY_RUNTIME_MOCK_QUICK_FIX_CALLBACK_STUB_H
#define OHOS_ABILITY_RUNTIME_MOCK_QUICK_FIX_CALLBACK_STUB_H

#include "gmock/gmock.h"
#include "quick_fix_callback_stub.h"

namespace OHOS {
namespace AppExecFwk {
class MockQuickFixCallbackStub : public QuickFixCallbackStub {
public:
    MOCK_METHOD4(SendRequest, int(uint32_t, MessageParcel &, MessageParcel &, MessageOption &));
    MOCK_METHOD2(OnLoadPatchDone, void(int32_t resultCode, int32_t recordId));
    MOCK_METHOD2(OnUnloadPatchDone, void(int32_t resultCode, int32_t recordId));
    MOCK_METHOD2(OnReloadPageDone, void(int32_t resultCode, int32_t recordId));

    int InvokeSendRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
    {
        code_ = code;
        return 0;
    }

private:
    int code_;
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // OHOS_ABILITY_RUNTIME_MOCK_QUICK_FIX_CALLBACK_STUB_H
