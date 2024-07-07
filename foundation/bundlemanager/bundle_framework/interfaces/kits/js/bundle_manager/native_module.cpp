/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

#include <cstdio>
#include <cstring>
#include <pthread.h>
#include <unistd.h>

#include "app_log_wrapper.h"
#include "bundle_manager.h"

namespace OHOS {
namespace AppExecFwk {
static napi_value BundleManagerExport(napi_env env, napi_value exports)
{
    napi_value abilityFlag = nullptr;
    NAPI_CALL(env, napi_create_object(env, &abilityFlag));
    CreateAbilityFlagObject(env, abilityFlag);

    napi_value extensionFlag = nullptr;
    NAPI_CALL(env, napi_create_object(env, &extensionFlag));
    CreateExtensionAbilityFlagObject(env, extensionFlag);

    napi_value extensionType = nullptr;
    NAPI_CALL(env, napi_create_object(env, &extensionType));
    CreateExtensionAbilityTypeObject(env, extensionType);

    napi_value applicationFlag = nullptr;
    NAPI_CALL(env, napi_create_object(env, &applicationFlag));
    CreateApplicationFlagObject(env, applicationFlag);

    napi_value bundleFlag = nullptr;
    NAPI_CALL(env, napi_create_object(env, &bundleFlag));
    CreateBundleFlagObject(env, bundleFlag);

    napi_value permissionGrantState = nullptr;
    NAPI_CALL(env, napi_create_object(env, &permissionGrantState));
    CreatePermissionGrantStateObject(env, permissionGrantState);

    napi_value nAbilityType = nullptr;
    NAPI_CALL(env, napi_create_object(env, &nAbilityType));
    CreateAbilityTypeObject(env, nAbilityType);

    napi_value nDisplayOrientation = nullptr;
    NAPI_CALL(env, napi_create_object(env, &nDisplayOrientation));
    CreateDisplayOrientationObject(env, nDisplayOrientation);

    napi_value nLaunchType = nullptr;
    NAPI_CALL(env, napi_create_object(env, &nLaunchType));
    CreateLaunchTypeObject(env, nLaunchType);

    napi_value nSupportWindowMode = nullptr;
    NAPI_CALL(env, napi_create_object(env, &nSupportWindowMode));
    CreateSupportWindowModesObject(env, nSupportWindowMode);

    napi_value nBundleType = nullptr;
    NAPI_CALL(env, napi_create_object(env, &nBundleType));
    CreateBundleTypeObject(env, nBundleType);

    napi_value nModuleType = nullptr;
    NAPI_CALL(env, napi_create_object(env, &nModuleType));
    CreateModuleTypeObject(env, nModuleType);

    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("getBundleArchiveInfo", GetBundleArchiveInfo),
        DECLARE_NAPI_FUNCTION("getBundleNameByUid", GetBundleNameByUid),
        DECLARE_NAPI_FUNCTION("queryAbilityInfo", QueryAbilityInfos),
        DECLARE_NAPI_FUNCTION("queryExtensionAbilityInfo", QueryExtensionInfos),
        DECLARE_NAPI_FUNCTION("setApplicationEnabled", SetApplicationEnabled),
        DECLARE_NAPI_FUNCTION("setAbilityEnabled", SetAbilityEnabled),
        DECLARE_NAPI_FUNCTION("isApplicationEnabled", IsApplicationEnabled),
        DECLARE_NAPI_FUNCTION("isAbilityEnabled", IsAbilityEnabled),
        DECLARE_NAPI_FUNCTION("getAbilityLabel", GetAbilityLabel),
        DECLARE_NAPI_FUNCTION("getAbilityIcon", GetAbilityIcon),
        DECLARE_NAPI_FUNCTION("cleanBundleCacheFiles", CleanBundleCacheFiles),
        DECLARE_NAPI_FUNCTION("getLaunchWantForBundle", GetLaunchWantForBundle),
        DECLARE_NAPI_FUNCTION("getProfileByAbility", GetProfileByAbility),
        DECLARE_NAPI_FUNCTION("getProfileByExtensionAbility", GetProfileByExAbility),
        DECLARE_NAPI_FUNCTION("getPermissionDef", GetPermissionDef),
        DECLARE_NAPI_FUNCTION("getAllBundleInfo", GetBundleInfos),
        DECLARE_NAPI_FUNCTION("getBundleInfo", GetBundleInfo),
        DECLARE_NAPI_PROPERTY("AbilityFlag", abilityFlag),
        DECLARE_NAPI_PROPERTY("ExtensionAbilityFlag", extensionFlag),
        DECLARE_NAPI_PROPERTY("ExtensionAbilityType", extensionType),
        DECLARE_NAPI_FUNCTION("getApplicationInfo", GetApplicationInfo),
        DECLARE_NAPI_FUNCTION("getAllApplicationInfo", GetApplicationInfos),
        DECLARE_NAPI_PROPERTY("ApplicationFlag", applicationFlag),
        DECLARE_NAPI_PROPERTY("BundleFlag", bundleFlag),
        DECLARE_NAPI_FUNCTION("getApplicationInfoSync", GetApplicationInfoSync),
        DECLARE_NAPI_FUNCTION("getBundleInfoSync", GetBundleInfoSync),
        DECLARE_NAPI_FUNCTION("getBundleInfoForSelf", GetBundleInfoForSelf),
        DECLARE_NAPI_PROPERTY("PermissionGrantState", permissionGrantState),
        DECLARE_NAPI_PROPERTY("AbilityType", nAbilityType),
        DECLARE_NAPI_PROPERTY("BundleType", nBundleType),
        DECLARE_NAPI_PROPERTY("ModuleType", nModuleType),
        DECLARE_NAPI_PROPERTY("DisplayOrientation", nDisplayOrientation),
        DECLARE_NAPI_PROPERTY("LaunchType", nLaunchType),
        DECLARE_NAPI_PROPERTY("SupportWindowMode", nSupportWindowMode),
    };

    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));
    APP_LOGI("init js bundle manager success.");
    return exports;
}

static napi_module bundle_manager_module = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = BundleManagerExport,
    .nm_modname = "bundle.bundleManager",
    .nm_priv = ((void *)0),
    .reserved = {0}
};

extern "C" __attribute__((constructor)) void BundleManagerRegister(void)
{
    napi_module_register(&bundle_manager_module);
}
}
}