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

#ifndef OHOS_ABILITY_RUNTIME_JS_NAPI_COMMON_ABILITY_H
#define OHOS_ABILITY_RUNTIME_JS_NAPI_COMMON_ABILITY_H

#include <map>
#include <mutex>

#include "ability_connect_callback_stub.h"
#include "ability_info.h"
#include "ability_manager_errors.h"
#include "application_info.h"
#include "feature_ability_common.h"
#include "js_runtime_utils.h"

namespace OHOS {
namespace AppExecFwk {
struct ConnectionKey {
    Want want;
    int64_t id;
};
struct KeyCompare {
    bool operator()(const ConnectionKey &key1, const ConnectionKey &key2) const
    {
        if (key1.id < key2.id) {
            return true;
        }
        return false;
    }
};

class JsNapiCommon {
public:
    JsNapiCommon();
    virtual ~JsNapiCommon() = default;

    struct JsPermissionOptions {
        bool uidFlag = false;
        bool pidFlag = false;
        int32_t uid = 0;
        int32_t pid = 0;
    };

    struct JsApplicationInfo {
        ApplicationInfo appInfo;
    };

    struct JsBundleName {
        std::string name = "";
    };
    typedef JsBundleName JsProcessName;
    typedef JsBundleName JsCallingBundleName;
    typedef JsBundleName JsOrCreateLocalDir;
    typedef JsBundleName JsFilesDir;
    typedef JsBundleName JsCacheDir;
    typedef JsBundleName JsCtxAppType;
    typedef JsBundleName JsOrCreateDistributedDir;
    typedef JsBundleName JsAppType;

    struct JsElementName {
        std::string deviceId = "";
        std::string bundleName = "";
        std::string abilityName = "";
        std::string uri = "";
        std::string shortName = "";
    };

    struct JsProcessInfo {
        std::string processName = "";
        pid_t pid = 0;
    };

    struct JsConfigurations {
        bool status;
    };
    typedef JsConfigurations JsDrawnCompleted;

    struct JsHapModuleInfo {
        HapModuleInfo hapModInfo;
    };

    struct JsAbilityInfoInfo {
        AbilityInfo abilityInfo;
    };

    struct JsWant {
        Want want;
    };

    NativeValue* JsConnectAbility(NativeEngine &engine, NativeCallbackInfo &info, const AbilityType abilityType);
    NativeValue* JsDisConnectAbility(NativeEngine &engine, NativeCallbackInfo &info, const AbilityType abilityType);

    sptr<NAPIAbilityConnection> BuildWant(const Want &want, int64_t &id);
    void ChangeAbilityConnection(napi_ref *callbackArray, const napi_env env, const napi_value &arg1);
    NativeValue* JsGetContext(NativeEngine &engine, NativeCallbackInfo &info, const AbilityType abilityType);
    NativeValue* JsGetFilesDir(NativeEngine &engine, NativeCallbackInfo &info, const AbilityType abilityType);
    NativeValue* JsIsUpdatingConfigurations(
        NativeEngine &engine, NativeCallbackInfo &info, const AbilityType abilityType);
    NativeValue* JsPrintDrawnCompleted(NativeEngine &engine, NativeCallbackInfo &info, const AbilityType abilityType);
    NativeValue* JsGetCacheDir(NativeEngine &engine, NativeCallbackInfo &info, const AbilityType abilityType);
    NativeValue* JsGetCtxAppType(NativeEngine &engine, NativeCallbackInfo &info, const AbilityType abilityType);
    NativeValue* JsGetCtxHapModuleInfo(NativeEngine &engine, NativeCallbackInfo &info, const AbilityType abilityType);
    NativeValue* JsGetAppVersionInfo(NativeEngine &engine, NativeCallbackInfo &info, const AbilityType abilityType);
    NativeValue* JsGetApplicationContext(
        NativeEngine &engine, NativeCallbackInfo &info, const AbilityType abilityType);
    NativeValue* JsGetCtxAbilityInfo(NativeEngine &engine, NativeCallbackInfo &info, const AbilityType abilityType);
    NativeValue* JsGetOrCreateDistributedDir(
        NativeEngine &engine, NativeCallbackInfo &info, const AbilityType abilityType);
    NativeValue* JsGetDisplayOrientation(
        NativeEngine &engine, NativeCallbackInfo &info, const AbilityType abilityType);
    NativeValue* JsGetWant(NativeEngine &engine, NativeCallbackInfo &info, const AbilityType abilityType);

    NativeValue* CreateProcessInfo(NativeEngine &engine, const std::shared_ptr<JsProcessInfo> &processInfo);
    NativeValue* CreateElementName(NativeEngine &engine, const std::shared_ptr<JsElementName> &elementName);
    NativeValue* CreateHapModuleInfo(NativeEngine &engine, const std::shared_ptr<JsHapModuleInfo> &hapModInfo);
    NativeValue* CreateModuleInfo(NativeEngine &engine, const ModuleInfo &modInfo);
    NativeValue* CreateModuleInfos(NativeEngine &engine, const std::vector<ModuleInfo> &moduleInfos);
    NativeValue* CreateAppInfo(NativeEngine &engine, const ApplicationInfo &appInfo);
    NativeValue* CreateAppInfo(NativeEngine &engine, const std::shared_ptr<JsApplicationInfo> &appInfo);
    NativeValue* CreateAbilityInfo(NativeEngine &engine, const AbilityInfo &abilityInfo);
    NativeValue* CreateAbilityInfo(NativeEngine &engine, const std::shared_ptr<JsAbilityInfoInfo> &abilityInfo);
    NativeValue* CreateAbilityInfos(NativeEngine &engine, const std::vector<AbilityInfo> &abilityInfos);
    NativeValue* CreateAppVersionInfo(NativeEngine &engine, const std::shared_ptr<JsApplicationInfo> &appInfo);
    NativeValue* CreateWant(NativeEngine &engine, const std::shared_ptr<JsWant> &want);
    bool CheckAbilityType(const AbilityType typeWant);
    bool UnwarpVerifyPermissionParams(NativeEngine &engine, NativeCallbackInfo &info, JsPermissionOptions &options);
    bool GetStringsValue(NativeEngine &engine, NativeValue *object, std::vector<std::string> &strList);
    bool GetPermissionOptions(NativeEngine &engine, NativeValue *object, JsPermissionOptions &options);
    std::string ConvertErrorCode(int32_t errCode);
    sptr<NAPIAbilityConnection> FindConnectionLocked(const Want &want, int64_t &id);
    void RemoveAllCallbacksLocked();
    bool CreateConnectionAndConnectAbilityLocked(
        std::shared_ptr<ConnectionCallback> callback, const Want &want, int64_t &id);
    void RemoveConnectionLocked(const Want &want);
    Ability *ability_;
    std::map<ConnectionKey, sptr<NAPIAbilityConnection>, KeyCompare> &connects_;
    std::recursive_mutex &connectionsLock_;
    int64_t &serialNumber_;
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // OHOS_ABILITY_RUNTIME_JS_NAPI_COMMON_ABILITY_H
