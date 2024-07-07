/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef MOCK_OHOS_ABILITY_RUNTIME_MOCK_IAPP_STATE_CALLBACK_H
#define MOCK_OHOS_ABILITY_RUNTIME_MOCK_IAPP_STATE_CALLBACK_H

#include "gmock/gmock.h"
#include "app_mgr_constants.h"
#include "app_state_callback_host.h"
#include "app_process_data.h"

namespace OHOS {
namespace AppExecFwk {
class MockAppStateCallback : public AppStateCallbackHost {
public:
    MockAppStateCallback()
    {}
    virtual ~MockAppStateCallback()
    {}

    MOCK_METHOD1(OnAppStateChanged, void(const AppProcessData &));
    MOCK_METHOD2(OnAbilityRequestDone, void(const sptr<IRemoteObject> &, const AbilityState));
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // MOCK_OHOS_ABILITY_RUNTIME_MOCK_IAPP_STATE_CALLBACK_H
