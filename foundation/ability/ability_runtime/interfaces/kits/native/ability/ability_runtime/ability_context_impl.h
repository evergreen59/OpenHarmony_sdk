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

#ifndef OHOS_ABILITY_RUNTIME_ABILITY_CONTEXT_IMPL_H
#define OHOS_ABILITY_RUNTIME_ABILITY_CONTEXT_IMPL_H

#include "ability_context.h"

#include <uv.h>

#include "context_impl.h"
#include "configuration.h"
#include "local_call_container.h"

namespace OHOS {
namespace AbilityRuntime {
class AbilityContextImpl : public AbilityContext {
public:
    AbilityContextImpl() = default;
    virtual ~AbilityContextImpl() = default;

    Global::Resource::DeviceType GetDeviceType() const override;
    std::string GetBaseDir() const override;
    std::string GetBundleCodeDir() override;
    std::string GetCacheDir() override;
    std::string GetTempDir() override;
    std::string GetFilesDir() override;
    bool IsUpdatingConfigurations() override;
    bool PrintDrawnCompleted() override;
    std::string GetDatabaseDir() override;
    std::string GetPreferencesDir() override;
    std::string GetDistributedFilesDir() override;
    void SwitchArea(int mode) override;
    int GetArea() override;
    std::string GetBundleName() const override;
    std::shared_ptr<AppExecFwk::ApplicationInfo> GetApplicationInfo() const override;
    std::shared_ptr<Global::Resource::ResourceManager> GetResourceManager() const override;
    std::shared_ptr<Context> CreateBundleContext(const std::string &bundleName) override;
    std::shared_ptr<Context> CreateModuleContext(const std::string &moduleName) override;
    std::shared_ptr<Context> CreateModuleContext(const std::string &bundleName, const std::string &moduleName) override;

    std::string GetBundleCodePath() const override;
    ErrCode StartAbility(const AAFwk::Want &want, int requestCode) override;
    ErrCode StartAbilityWithAccount(const AAFwk::Want &want, int accountId, int requestCode) override;
    ErrCode StartAbility(const AAFwk::Want &want, const AAFwk::StartOptions &startOptions, int requestCode) override;
    ErrCode StartAbilityWithAccount(
        const AAFwk::Want &want, int accountId, const AAFwk::StartOptions &startOptions, int requestCode) override;
    ErrCode StartAbilityForResult(const AAFwk::Want &want, int requestCode, RuntimeTask &&task) override;
    ErrCode StartAbilityForResultWithAccount(
        const AAFwk::Want &want, int accountId, int requestCode, RuntimeTask &&task) override;
    ErrCode StartAbilityForResultWithAccount(const AAFwk::Want &want, int accountId,
        const AAFwk::StartOptions &startOptions, int requestCode, RuntimeTask &&task) override;
    ErrCode StartAbilityForResult(const AAFwk::Want &want, const AAFwk::StartOptions &startOptions,
        int requestCode, RuntimeTask &&task) override;
    ErrCode StartServiceExtensionAbility(const Want &want, int32_t accountId = -1) override;
    ErrCode StopServiceExtensionAbility(const Want& want, int32_t accountId = -1) override;
    ErrCode TerminateAbilityWithResult(const AAFwk::Want &want, int resultCode) override;
    void OnAbilityResult(int requestCode, int resultCode, const AAFwk::Want &resultData) override;
    bool ConnectAbility(const AAFwk::Want &want,
                        const sptr<AbilityConnectCallback> &connectCallback) override;
    bool ConnectAbilityWithAccount(const AAFwk::Want &want, int accountId,
                        const sptr<AbilityConnectCallback> &connectCallback) override;
    void DisconnectAbility(const AAFwk::Want &want,
                           const sptr<AbilityConnectCallback> &connectCallback) override;
    std::shared_ptr<AppExecFwk::HapModuleInfo> GetHapModuleInfo() const override;
    std::shared_ptr<AppExecFwk::AbilityInfo> GetAbilityInfo() const override;
    void MinimizeAbility(bool fromUser = false) override;

    ErrCode TerminateSelf() override;

    ErrCode CloseAbility() override;

    sptr<IRemoteObject> GetToken() override;

    void RequestPermissionsFromUser(NativeEngine& engine, const std::vector<std::string> &permissions, int requestCode,
        PermissionRequestTask &&task) override;

    ErrCode RestoreWindowStage(NativeEngine& engine, NativeValue* contentStorage) override;

    void SetStageContext(const std::shared_ptr<AbilityRuntime::Context> &stageContext);

    /**
     * @brief Set the Ability Info object
     *
     * set ability info to ability context
     */
    void SetAbilityInfo(const std::shared_ptr<AppExecFwk::AbilityInfo> &abilityInfo);

    /**
     * @brief Attachs ability's token.
     *
     * @param token The token represents ability.
     */
    void SetToken(const sptr<IRemoteObject> &token) override
    {
        token_ = token;
    }

    /**
     * @brief Get ContentStorage.
     *
     * @return Returns the ContentStorage.
     */
    std::unique_ptr<NativeReference>& GetContentStorage() override
    {
        return contentStorage_;
    }

    /**
     * @brief Get LocalCallContainer.
     *
     * @return Returns the LocalCallContainer.
     */
    sptr<LocalCallContainer> GetLocalCallContainer() override
    {
        return localCallContainer_;
    }

    void SetConfiguration(const std::shared_ptr<AppExecFwk::Configuration> &config) override;

    std::shared_ptr<AppExecFwk::Configuration> GetConfiguration() const override;

    /**
     * call function by callback objectS
     *
     * @param want Request info for ability.
     * @param callback Indicates the callback object.
     *
     * @return Returns zero on success, others on failure.
     */
    ErrCode StartAbilityByCall(const AAFwk::Want& want, const std::shared_ptr<CallerCallBack> &callback) override;

    /**
     * caller release by callback object
     *
     * @param callback Indicates the callback object.
     *
     * @return Returns zero on success, others on failure.
     */
    ErrCode ReleaseCall(const std::shared_ptr<CallerCallBack> &callback) override;

    /**
     * register ability callback
     *
     * @param abilityCallback Indicates the abilityCallback object.
     */
    void RegisterAbilityCallback(std::weak_ptr<AppExecFwk::IAbilityCallback> abilityCallback) override;

    bool IsTerminating() override
    {
        return isTerminating_;
    }

    void SetTerminating(bool state) override
    {
        isTerminating_ = state;
    }

    ErrCode GetMissionId(int32_t &missionId) override;

    ErrCode RequestDialogService(NativeEngine &engine, AAFwk::Want &want, RequestDialogResultTask &&task) override;

#ifdef SUPPORT_GRAPHICS
    /**
     * @brief Set mission label of this ability.
     *
     * @param label the label of this ability.
     * @return Returns ERR_OK if success.
     */
    ErrCode SetMissionLabel(const std::string &label) override;

    /**
     * @brief Set mission icon of this ability.
     *
     * @param icon the icon of this ability.
     * @return Returns ERR_OK if success.
     */
    ErrCode SetMissionIcon(const std::shared_ptr<OHOS::Media::PixelMap> &icon) override;

    /**
     * @brief get current window mode.
     *
     * @return Returns the current window mode.
     */
    int GetCurrentWindowMode() override;
#endif

private:
    static std::mutex mutex_;
    static std::map<int, PermissionRequestTask> permissionRequestCallbacks;
    sptr<IRemoteObject> token_ = nullptr;
    std::shared_ptr<AppExecFwk::AbilityInfo> abilityInfo_ = nullptr;
    std::shared_ptr<AbilityRuntime::Context> stageContext_ = nullptr;
    std::map<int, RuntimeTask> resultCallbacks_;
    std::unique_ptr<NativeReference> contentStorage_ = nullptr;
    std::shared_ptr<AppExecFwk::Configuration> config_ = nullptr;
    sptr<LocalCallContainer> localCallContainer_ = nullptr;
    std::weak_ptr<AppExecFwk::IAbilityCallback> abilityCallback_;
    bool isTerminating_ = false;
    int32_t missionId_ = -1;

    static void ResultCallbackJSThreadWorker(uv_work_t* work, int status);
    static void RequestDialogResultJSThreadWorker(uv_work_t* work, int status);
    void StartGrantExtension(NativeEngine& engine, const std::vector<std::string>& permissions,
        const std::vector<int>& permissionsState, int requestCode, PermissionRequestTask &&task);

    struct ResultCallback {
        std::vector<std::string> permissions_;
        std::vector<int> grantResults_;
        int requestCode_;
    };
};
} // namespace AbilityRuntime
} // namespace OHOS
#endif // OHOS_ABILITY_RUNTIME_ABILITY_CONTEXT_IMPL_H
