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

#ifndef OHOS_ABILITY_RUNTIME_JS_ABILITY_H
#define OHOS_ABILITY_RUNTIME_JS_ABILITY_H

#include "ability.h"
#include "ability_delegator_infos.h"

class NativeReference;
class NativeValue;

namespace OHOS {
namespace AbilityRuntime {
class JsRuntime;

using Ability = AppExecFwk::Ability;
using AbilityHandler = AppExecFwk::AbilityHandler;
using AbilityInfo = AppExecFwk::AbilityInfo;
using OHOSApplication = AppExecFwk::OHOSApplication;
using Want = AppExecFwk::Want;
using AbilityStartSetting = AppExecFwk::AbilityStartSetting;
using Configuration = AppExecFwk::Configuration;

class JsAbility : public Ability {
public:
    static Ability *Create(const std::unique_ptr<Runtime> &runtime);

    JsAbility(JsRuntime &jsRuntime);
    ~JsAbility() override;

    void Init(const std::shared_ptr<AbilityInfo> &abilityInfo, const std::shared_ptr<OHOSApplication> application,
        std::shared_ptr<AbilityHandler> &handler, const sptr<IRemoteObject> &token) override;

    void OnStart(const Want &want) override;
    void OnStop() override;
    void OnStop(AppExecFwk::AbilityTransactionCallbackInfo<> *callbackInfo, bool &isAsyncCallback) override;
    void OnStopCallback() override;
    int32_t OnContinue(WantParams &wantParams) override;
    void OnConfigurationUpdated(const Configuration &configuration) override;
    void UpdateContextConfiguration() override;
    void OnMemoryLevel(int level) override;
    void OnNewWant(const Want &want) override;
    int32_t OnSaveState(int32_t reason, WantParams &wantParams) override;

    void OnAbilityResult(int requestCode, int resultCode, const Want &resultData) override;

    sptr<IRemoteObject> CallRequest() override;

    /**
     * @brief dump ability info
     *
     * @param params dump params that indicate different dump targets
     * @param info dump ability info
     */
    virtual void Dump(const std::vector<std::string> &params, std::vector<std::string> &info) override;

    std::shared_ptr<NativeReference> GetJsAbility();

#ifdef SUPPORT_GRAPHICS
public:
    void OnSceneCreated() override;
    void onSceneDestroyed() override;
    void OnSceneRestored() override;

    void OnForeground(const Want &want) override;
    void OnBackground() override;

    std::shared_ptr<NativeReference> GetJsWindowStage();

protected:
    void DoOnForeground(const Want &want) override;
    void RequestFocus(const Want &want) override;
    void ContinuationRestore(const Want &want) override;

private:
    bool IsRestorePageStack(const Want &want);
    void RestorePageStack(const Want &want);
    void GetPageStackFromWant(const Want &want, std::string &pageStack);
    void AbilityContinuationOrRecover(const Want &want);
    std::shared_ptr<NativeReference> jsWindowStageObj_;
#endif

private:
    NativeValue *CallObjectMethod(const char *name, NativeValue *const *argv = nullptr, size_t argc = 0,
        bool withResult = false);
    bool CheckPromise(NativeValue* result);
    bool CallPromise(NativeValue* result, AppExecFwk::AbilityTransactionCallbackInfo<> *callbackInfo);
    std::unique_ptr<NativeReference> CreateAppWindowStage();
    std::shared_ptr<AppExecFwk::ADelegatorAbilityProperty> CreateADelegatorAbilityProperty();
    sptr<IRemoteObject> SetNewRuleFlagToCallee(NativeEngine &nativeEngine, NativeValue *remoteJsObj);

    JsRuntime &jsRuntime_;
    std::shared_ptr<NativeReference> shellContextRef_;
    std::shared_ptr<NativeReference> jsAbilityObj_;
    sptr<IRemoteObject> remoteCallee_;
};
}  // namespace AbilityRuntime
}  // namespace OHOS
#endif  // OHOS_ABILITY_RUNTIME_JS_ABILITY_H
