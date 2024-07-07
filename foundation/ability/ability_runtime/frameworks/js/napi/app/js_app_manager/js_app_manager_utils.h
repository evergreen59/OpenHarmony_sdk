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

#include "application_state_observer_stub.h"
#include "native_engine/native_engine.h"
#include "running_process_info.h"

#ifndef OHOS_ABILITY_RUNTIME_JS_APP_MANAGER_UTILS_H
#define OHOS_ABILITY_RUNTIME_JS_APP_MANAGER_UTILS_H

namespace OHOS {
namespace AbilityRuntime {
using OHOS::AppExecFwk::AppStateData;
using OHOS::AppExecFwk::AbilityStateData;
using OHOS::AppExecFwk::ProcessData;
using OHOS::AppExecFwk::RunningProcessInfo;
NativeValue* CreateJsAppStateData(NativeEngine &engine, const AppStateData &appStateData);
NativeValue* CreateJsAbilityStateData(NativeEngine &engine, const AbilityStateData &abilityStateData);
NativeValue* CreateJsProcessData(NativeEngine &engine, const ProcessData &processData);
NativeValue* CreateJsAppStateDataArray(NativeEngine &engine, const std::vector<AppStateData> &appStateDatas);
NativeValue* CreateJsRunningProcessInfoArray(NativeEngine &engine, const std::vector<RunningProcessInfo> &infos);
NativeValue* CreateJsRunningProcessInfo(NativeEngine &engine, const RunningProcessInfo &info);
}  // namespace AbilityRuntime
}  // namespace OHOS
#endif // OHOS_ABILITY_RUNTIME_JS_APP_MANAGER_UTILS_H
