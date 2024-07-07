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

#ifndef OHOS_ABILITY_RUNTIME_JS_ABILITY_STAGE_H
#define OHOS_ABILITY_RUNTIME_JS_ABILITY_STAGE_H

#include "ability_delegator_infos.h"
#include "ability_stage.h"
#include "configuration.h"
#include "native_engine/native_value.h"

class NativeReference;

namespace OHOS {
namespace AbilityRuntime {
class JsRuntime;
class JsAbilityStage : public AbilityStage {
public:
    static std::shared_ptr<AbilityStage> Create(
        const std::unique_ptr<Runtime>& runtime, const AppExecFwk::HapModuleInfo& hapModuleInfo);

    JsAbilityStage(JsRuntime& jsRuntime, std::unique_ptr<NativeReference>&& jsAbilityStageObj);
    ~JsAbilityStage() override;

    void Init(const std::shared_ptr<Context> &context) override;

    void OnCreate(const AAFwk::Want &want) const override;

    std::string OnAcceptWant(const AAFwk::Want &want) override;

    void OnConfigurationUpdated(const AppExecFwk::Configuration& configuration) override;

    void OnMemoryLevel(int32_t level) override;

private:
    NativeValue* CallObjectMethod(const char* name, NativeValue * const * argv = nullptr, size_t argc = 0);

    std::shared_ptr<AppExecFwk::DelegatorAbilityStageProperty> CreateStageProperty() const;

    std::string GetHapModuleProp(const std::string &propName) const;

    static bool UseCommonChunk(const AppExecFwk::HapModuleInfo& hapModuleInfo);

    JsRuntime& jsRuntime_;
    std::shared_ptr<NativeReference> jsAbilityStageObj_;
    std::shared_ptr<NativeReference> shellContextRef_;
};
}  // namespace AbilityRuntime
}  // namespace OHOS
#endif  // OHOS_ABILITY_RUNTIME_JS_ABILITY_STAGE_H
