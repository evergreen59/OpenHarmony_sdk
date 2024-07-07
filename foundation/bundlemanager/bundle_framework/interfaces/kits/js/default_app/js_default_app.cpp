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

#include "js_default_app.h"

#include <string>

#include "app_log_wrapper.h"
#include "bundle_errors.h"
#include "bundle_mgr_interface.h"
#include "bundle_mgr_proxy.h"
#include "business_error.h"
#include "common_func.h"
#include "if_system_ability_manager.h"
#include "ipc_skeleton.h"
#include "iservice_registry.h"
#include "napi_arg.h"
#include "napi_constants.h"
#include "system_ability_definition.h"

namespace OHOS {
namespace AppExecFwk {
using namespace OHOS::AAFwk;

namespace {
constexpr int32_t NAPI_RETURN_ZERO = 0;
const std::string IS_DEFAULT_APPLICATION = "IsDefaultApplication";
const std::string GET_DEFAULT_APPLICATION = "GetDefaultApplication";
const std::string SET_DEFAULT_APPLICATION = "SetDefaultApplication";
const std::string RESET_DEFAULT_APPLICATION = "ResetDefaultApplication";
const std::string PARAM_TYPE_CHECK_ERROR = "param type check error";
const std::string PARAM_TYPE_CHECK_ERROR_WITH_POS = "param type check error, error position : ";
}

static const std::unordered_map<std::string, std::string> TYPE_MAPPING = {
    {"Web Browser", "BROWSER"},
    {"Image Gallery", "IMAGE"},
    {"Audio Player", "AUDIO"},
    {"Video Player", "VIDEO"},
    {"PDF Viewer", "PDF"},
    {"Word Viewer", "WORD"},
    {"Excel Viewer", "EXCEL"},
    {"PPT Viewer", "PPT"}
};

static bool ParseType(napi_env env, napi_value value, std::string& result)
{
    napi_valuetype valueType = napi_undefined;
    napi_typeof(env, value, &valueType);
    if (valueType != napi_string) {
        APP_LOGE("type not string");
        return false;
    }
    size_t size = 0;
    if (napi_get_value_string_utf8(env, value, nullptr, NAPI_RETURN_ZERO, &size) != napi_ok) {
        APP_LOGE("napi_get_value_string_utf8 error.");
        return false;
    }
    result.reserve(size + 1);
    result.resize(size);
    if (napi_get_value_string_utf8(env, value, result.data(), (size + 1), &size) != napi_ok) {
        APP_LOGE("napi_get_value_string_utf8 error");
        return false;
    }
    if (TYPE_MAPPING.find(result) != TYPE_MAPPING.end()) {
        result = TYPE_MAPPING.at(result);
    }
    return true;
}

static OHOS::sptr<OHOS::AppExecFwk::IDefaultApp> GetDefaultAppProxy()
{
    auto systemAbilityManager = OHOS::SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (systemAbilityManager == nullptr) {
        APP_LOGE("systemAbilityManager is null.");
        return nullptr;
    }
    auto bundleMgrSa = systemAbilityManager->GetSystemAbility(OHOS::BUNDLE_MGR_SERVICE_SYS_ABILITY_ID);
    if (bundleMgrSa == nullptr) {
        APP_LOGE("bundleMgrSa is null.");
        return nullptr;
    }
    auto bundleMgr = OHOS::iface_cast<IBundleMgr>(bundleMgrSa);
    if (bundleMgr == nullptr) {
        APP_LOGE("iface_cast failed.");
        return nullptr;
    }
    auto defaultAppProxy = bundleMgr->GetDefaultAppProxy();
    if (defaultAppProxy == nullptr) {
        APP_LOGE("GetDefaultAppProxy failed.");
        return nullptr;
    }
    return defaultAppProxy;
}

static void ConvertAbilityInfo(napi_env env, napi_value objAbilityInfo, const AbilityInfo &abilityInfo)
{
    APP_LOGD("begin to ConvertAbilityInfo");
    napi_value nBundleName;
    NAPI_CALL_RETURN_VOID(
        env, napi_create_string_utf8(env, abilityInfo.bundleName.c_str(), NAPI_AUTO_LENGTH, &nBundleName));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAbilityInfo, "bundleName", nBundleName));

    napi_value nModuleName;
    NAPI_CALL_RETURN_VOID(
        env, napi_create_string_utf8(env, abilityInfo.moduleName.c_str(), NAPI_AUTO_LENGTH, &nModuleName));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAbilityInfo, "moduleName", nModuleName));

    napi_value nName;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, abilityInfo.name.c_str(), NAPI_AUTO_LENGTH, &nName));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAbilityInfo, "name", nName));

    napi_value nLabel;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, abilityInfo.label.c_str(), NAPI_AUTO_LENGTH, &nLabel));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAbilityInfo, "label", nLabel));

    napi_value nLabelId;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, abilityInfo.labelId, &nLabelId));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAbilityInfo, "labelId", nLabelId));

    napi_value nDescription;
    NAPI_CALL_RETURN_VOID(
        env, napi_create_string_utf8(env, abilityInfo.description.c_str(), NAPI_AUTO_LENGTH, &nDescription));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAbilityInfo, "description", nDescription));

    napi_value nDescriptionId;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, abilityInfo.descriptionId, &nDescriptionId));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAbilityInfo, "descriptionId", nDescriptionId));

    napi_value nIconPath;
    NAPI_CALL_RETURN_VOID(
        env, napi_create_string_utf8(env, abilityInfo.iconPath.c_str(), NAPI_AUTO_LENGTH, &nIconPath));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAbilityInfo, "icon", nIconPath));

    napi_value nIconId;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, abilityInfo.iconId, &nIconId));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objAbilityInfo, "iconId", nIconId));
    APP_LOGD("ConvertAbilityInfo done");
}

static void ConvertExtensionInfo(napi_env env, napi_value objExtensionInfo, const ExtensionAbilityInfo& extensionInfo)
{
    APP_LOGD("begin to ConvertExtensionInfo");
    napi_value nBundleName;
    NAPI_CALL_RETURN_VOID(
        env, napi_create_string_utf8(env, extensionInfo.bundleName.c_str(), NAPI_AUTO_LENGTH, &nBundleName));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objExtensionInfo, "bundleName", nBundleName));

    napi_value nModuleName;
    NAPI_CALL_RETURN_VOID(
        env, napi_create_string_utf8(env, extensionInfo.moduleName.c_str(), NAPI_AUTO_LENGTH, &nModuleName));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objExtensionInfo, "moduleName", nModuleName));

    napi_value nName;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, extensionInfo.name.c_str(), NAPI_AUTO_LENGTH, &nName));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objExtensionInfo, "name", nName));

    napi_value nLabelId;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, extensionInfo.labelId, &nLabelId));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objExtensionInfo, "labelId", nLabelId));

    napi_value nDescriptionId;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, extensionInfo.descriptionId, &nDescriptionId));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objExtensionInfo, "descriptionId", nDescriptionId));

    napi_value nIconId;
    NAPI_CALL_RETURN_VOID(env, napi_create_int32(env, extensionInfo.iconId, &nIconId));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objExtensionInfo, "iconId", nIconId));
    APP_LOGD("ConvertExtensionInfo done");
}

static void ConvertBundleInfo(napi_env env, napi_value objBundleInfo, const BundleInfo &bundleInfo)
{
    APP_LOGD("begin to ConvertBundleInfo");
    napi_value nName;
    NAPI_CALL_RETURN_VOID(env, napi_create_string_utf8(env, bundleInfo.name.c_str(), NAPI_AUTO_LENGTH, &nName));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objBundleInfo, "name", nName));

    napi_value nHapModulesInfo;
    NAPI_CALL_RETURN_VOID(env, napi_create_array(env, &nHapModulesInfo));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, objBundleInfo, "hapModulesInfo", nHapModulesInfo));

    napi_value nAbilityInfos;
    NAPI_CALL_RETURN_VOID(env, napi_create_array(env, &nAbilityInfos));
    for (size_t idx = 0; idx < bundleInfo.abilityInfos.size(); idx++) {
        napi_value objAbilityInfo;
        NAPI_CALL_RETURN_VOID(env, napi_create_object(env, &objAbilityInfo));
        ConvertAbilityInfo(env, objAbilityInfo, bundleInfo.abilityInfos[idx]);
        NAPI_CALL_RETURN_VOID(env, napi_set_element(env, nAbilityInfos, idx, objAbilityInfo));
    }

    napi_value nHapModuleInfo;
    NAPI_CALL_RETURN_VOID(env, napi_create_object(env, &nHapModuleInfo));
    NAPI_CALL_RETURN_VOID(env, napi_set_element(env, nHapModulesInfo, 0, nHapModuleInfo));

    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, nHapModuleInfo, "abilitiesInfo", nAbilityInfos));

    napi_value nExtensionAbilityInfos;
    NAPI_CALL_RETURN_VOID(env, napi_create_array(env, &nExtensionAbilityInfos));
    for (size_t idx = 0; idx < bundleInfo.extensionInfos.size(); idx++) {
        napi_value objExtensionInfo;
        NAPI_CALL_RETURN_VOID(env, napi_create_object(env, &objExtensionInfo));
        ConvertExtensionInfo(env, objExtensionInfo, bundleInfo.extensionInfos[idx]);
        NAPI_CALL_RETURN_VOID(env, napi_set_element(env, nExtensionAbilityInfos, idx, objExtensionInfo));
    }

    NAPI_CALL_RETURN_VOID(env,
        napi_set_named_property(env, nHapModuleInfo, "extensionAbilitiesInfo", nExtensionAbilityInfos));
    APP_LOGD("ConvertBundleInfo done");
}

static ErrCode InnerIsDefaultApplication(DefaultAppCallbackInfo *info)
{
    if (info == nullptr) {
        APP_LOGE("info is null");
        return ERROR_BUNDLE_SERVICE_EXCEPTION;
    }
    auto defaultAppProxy = GetDefaultAppProxy();
    if (defaultAppProxy == nullptr) {
        APP_LOGE("defaultAppProxy is null");
        return ERROR_BUNDLE_SERVICE_EXCEPTION;
    }
    ErrCode ret = defaultAppProxy->IsDefaultApplication(info->type, info->isDefaultApp);
    APP_LOGD("IsDefaultApplication ErrCode : %{public}d", ret);
    return CommonFunc::ConvertErrCode(ret);
}

void IsDefaultApplicationExec(napi_env env, void *data)
{
    DefaultAppCallbackInfo *asyncCallbackInfo = reinterpret_cast<DefaultAppCallbackInfo *>(data);
    if (asyncCallbackInfo == nullptr) {
        APP_LOGE("asyncCallbackInfo is null");
        return;
    }
    asyncCallbackInfo->err = InnerIsDefaultApplication(asyncCallbackInfo);
}

void IsDefaultApplicationComplete(napi_env env, napi_status status, void *data)
{
    DefaultAppCallbackInfo *asyncCallbackInfo = reinterpret_cast<DefaultAppCallbackInfo *>(data);
    if (asyncCallbackInfo == nullptr) {
        APP_LOGE("asyncCallbackInfo is null in %{public}s", __func__);
        return;
    }
    std::unique_ptr<DefaultAppCallbackInfo> callbackPtr {asyncCallbackInfo};
    napi_value result[ARGS_SIZE_TWO] = {0};
    if (asyncCallbackInfo->err == NO_ERROR) {
        NAPI_CALL_RETURN_VOID(env, napi_get_null(env, &result[ARGS_POS_ZERO]));
        NAPI_CALL_RETURN_VOID(env, napi_get_boolean(env, asyncCallbackInfo->isDefaultApp, &result[ARGS_POS_ONE]));
    } else {
        result[ARGS_POS_ZERO] = BusinessError::CreateCommonError(env, asyncCallbackInfo->err,
            IS_DEFAULT_APPLICATION, "");
    }
    if (asyncCallbackInfo->deferred) {
        if (asyncCallbackInfo->err == NO_ERROR) {
            NAPI_CALL_RETURN_VOID(env, napi_resolve_deferred(env, asyncCallbackInfo->deferred, result[ARGS_POS_ONE]));
        } else {
            NAPI_CALL_RETURN_VOID(env, napi_reject_deferred(env, asyncCallbackInfo->deferred, result[ARGS_POS_ZERO]));
        }
    } else {
        napi_value callback = nullptr;
        napi_value placeHolder = nullptr;
        NAPI_CALL_RETURN_VOID(env, napi_get_reference_value(env, asyncCallbackInfo->callback, &callback));
        NAPI_CALL_RETURN_VOID(env, napi_call_function(env, nullptr, callback,
            sizeof(result) / sizeof(result[ARGS_POS_ZERO]), result, &placeHolder));
    }
}

napi_value IsDefaultApplication(napi_env env, napi_callback_info info)
{
    APP_LOGD("begin of IsDefaultApplication");
    NapiArg args(env, info);
    DefaultAppCallbackInfo *asyncCallbackInfo = new (std::nothrow) DefaultAppCallbackInfo(env);
    if (asyncCallbackInfo == nullptr) {
        APP_LOGE("asyncCallbackInfo is null");
        return nullptr;
    }
    std::unique_ptr<DefaultAppCallbackInfo> callbackPtr {asyncCallbackInfo};
    if (!args.Init(ARGS_SIZE_ONE, ARGS_SIZE_TWO)) {
        APP_LOGE("param count invalid");
        BusinessError::ThrowTooFewParametersError(env, ERROR_PARAM_CHECK_ERROR);
        return nullptr;
    }
    for (size_t i = 0; i < args.GetMaxArgc(); ++i) {
        napi_valuetype valueType = napi_undefined;
        napi_typeof(env, args[i], &valueType);
        if ((i == ARGS_POS_ZERO) && (valueType == napi_string)) {
            if (!ParseType(env, args[i], asyncCallbackInfo->type)) {
                APP_LOGE("type invalid!");
                BusinessError::ThrowError(env, ERROR_PARAM_CHECK_ERROR, PARAM_TYPE_CHECK_ERROR);
                return nullptr;
            }
        } else if (i == ARGS_POS_ONE) {
            if (valueType == napi_function) {
                NAPI_CALL(env, napi_create_reference(env, args[i], NAPI_RETURN_ONE, &asyncCallbackInfo->callback));
            }
            break;
        } else {
            APP_LOGE("param check error");
            std::string errMsg = PARAM_TYPE_CHECK_ERROR_WITH_POS + std::to_string(i + 1);
            BusinessError::ThrowError(env, ERROR_PARAM_CHECK_ERROR, errMsg);
            return nullptr;
        }
    }
    auto promise = CommonFunc::AsyncCallNativeMethod<DefaultAppCallbackInfo>(
        env, asyncCallbackInfo, IS_DEFAULT_APPLICATION, IsDefaultApplicationExec, IsDefaultApplicationComplete);
    callbackPtr.release();
    APP_LOGD("call IsDefaultApplication done");
    return promise;
}

static ErrCode InnerGetDefaultApplication(DefaultAppCallbackInfo *info)
{
    if (info == nullptr) {
        APP_LOGE("info is null");
        return ERROR_BUNDLE_SERVICE_EXCEPTION;
    }
    auto defaultAppProxy = GetDefaultAppProxy();
    if (defaultAppProxy == nullptr) {
        APP_LOGE("defaultAppProxy is null");
        return ERROR_BUNDLE_SERVICE_EXCEPTION;
    }
    ErrCode ret = defaultAppProxy->GetDefaultApplication(info->userId, info->type, info->bundleInfo);
    APP_LOGD("GetDefaultApplication ErrCode : %{public}d", ret);
    return CommonFunc::ConvertErrCode(ret);
}

void GetDefaultApplicationExec(napi_env env, void *data)
{
    DefaultAppCallbackInfo *asyncCallbackInfo = reinterpret_cast<DefaultAppCallbackInfo *>(data);
    if (asyncCallbackInfo == nullptr) {
        APP_LOGE("asyncCallbackInfo is null");
        return;
    }
    asyncCallbackInfo->err = InnerGetDefaultApplication(asyncCallbackInfo);
}

void GetDefaultApplicationComplete(napi_env env, napi_status status, void *data)
{
    DefaultAppCallbackInfo *asyncCallbackInfo = reinterpret_cast<DefaultAppCallbackInfo *>(data);
    if (asyncCallbackInfo == nullptr) {
        APP_LOGE("asyncCallbackInfo is null in %{public}s", __func__);
        return;
    }
    std::unique_ptr<DefaultAppCallbackInfo> callbackPtr {asyncCallbackInfo};
    napi_value result[ARGS_SIZE_TWO] = {0};
    if (asyncCallbackInfo->err == NO_ERROR) {
        NAPI_CALL_RETURN_VOID(env, napi_get_null(env, &result[ARGS_POS_ZERO]));
        NAPI_CALL_RETURN_VOID(env, napi_create_object(env, &result[ARGS_POS_ONE]));
        ConvertBundleInfo(env, result[ARGS_POS_ONE], asyncCallbackInfo->bundleInfo);
    } else {
        result[ARGS_POS_ZERO] = BusinessError::CreateCommonError(env, asyncCallbackInfo->err,
            GET_DEFAULT_APPLICATION, Constants::PERMISSION_GET_DEFAULT_APPLICATION);
    }
    if (asyncCallbackInfo->deferred) {
        if (asyncCallbackInfo->err == NO_ERROR) {
            NAPI_CALL_RETURN_VOID(env, napi_resolve_deferred(env, asyncCallbackInfo->deferred, result[ARGS_POS_ONE]));
        } else {
            NAPI_CALL_RETURN_VOID(env, napi_reject_deferred(env, asyncCallbackInfo->deferred, result[ARGS_POS_ZERO]));
        }
    } else {
        napi_value callback = nullptr;
        napi_value placeHolder = nullptr;
        NAPI_CALL_RETURN_VOID(env, napi_get_reference_value(env, asyncCallbackInfo->callback, &callback));
        NAPI_CALL_RETURN_VOID(env, napi_call_function(env, nullptr, callback,
            sizeof(result) / sizeof(result[ARGS_POS_ZERO]), result, &placeHolder));
    }
}

napi_value GetDefaultApplication(napi_env env, napi_callback_info info)
{
    APP_LOGD("begin to GetDefaultApplication");
    NapiArg args(env, info);
    DefaultAppCallbackInfo *asyncCallbackInfo = new (std::nothrow) DefaultAppCallbackInfo(env);
    if (asyncCallbackInfo == nullptr) {
        APP_LOGE("asyncCallbackInfo is null");
        return nullptr;
    }
    asyncCallbackInfo->userId = IPCSkeleton::GetCallingUid() / Constants::BASE_USER_RANGE;
    std::unique_ptr<DefaultAppCallbackInfo> callbackPtr {asyncCallbackInfo};
    if (!args.Init(ARGS_SIZE_ONE, ARGS_SIZE_THREE)) {
        APP_LOGE("param count invalid");
        BusinessError::ThrowTooFewParametersError(env, ERROR_PARAM_CHECK_ERROR);
        return nullptr;
    }
    for (size_t i = 0; i < args.GetMaxArgc(); ++i) {
        napi_valuetype valueType = napi_undefined;
        napi_typeof(env, args[i], &valueType);
        if ((i == ARGS_POS_ZERO) && (valueType == napi_string)) {
            if (!ParseType(env, args[i], asyncCallbackInfo->type)) {
                APP_LOGE("type invalid!");
                BusinessError::ThrowError(env, ERROR_PARAM_CHECK_ERROR, PARAM_TYPE_CHECK_ERROR);
                return nullptr;
            }
        } else if (i == ARGS_POS_ONE) {
            if (valueType == napi_number) {
                CommonFunc::ParseInt(env, args[i], asyncCallbackInfo->userId);
            } else if (valueType == napi_function) {
                NAPI_CALL(env, napi_create_reference(env, args[i], NAPI_RETURN_ONE, &asyncCallbackInfo->callback));
                break;
            } else {
                APP_LOGE("param check error");
                std::string errMsg = PARAM_TYPE_CHECK_ERROR_WITH_POS + std::to_string(i + 1);
                BusinessError::ThrowError(env, ERROR_PARAM_CHECK_ERROR, errMsg);
                return nullptr;
            }
        } else if (i == ARGS_POS_TWO) {
            if (valueType == napi_function) {
                NAPI_CALL(env, napi_create_reference(env, args[i], NAPI_RETURN_ONE, &asyncCallbackInfo->callback));
            }
            break;
        } else {
            APP_LOGE("param check error");
            std::string errMsg = PARAM_TYPE_CHECK_ERROR_WITH_POS + std::to_string(i + 1);
            BusinessError::ThrowError(env, ERROR_PARAM_CHECK_ERROR, errMsg);
            return nullptr;
        }
    }
    auto promise = CommonFunc::AsyncCallNativeMethod<DefaultAppCallbackInfo>(
        env, asyncCallbackInfo, GET_DEFAULT_APPLICATION, GetDefaultApplicationExec, GetDefaultApplicationComplete);
    callbackPtr.release();
    APP_LOGD("call GetDefaultApplication done");
    return promise;
}

static ErrCode InnerSetDefaultApplication(const DefaultAppCallbackInfo *info)
{
    if (info == nullptr) {
        APP_LOGE("info is null");
        return ERROR_BUNDLE_SERVICE_EXCEPTION;
    }
    auto defaultAppProxy = GetDefaultAppProxy();
    if (defaultAppProxy == nullptr) {
        APP_LOGE("defaultAppProxy is null");
        return ERROR_BUNDLE_SERVICE_EXCEPTION;
    }
    ErrCode ret = defaultAppProxy->SetDefaultApplication(info->userId, info->type, info->want);
    APP_LOGD("SetDefaultApplication ErrCode : %{public}d", ret);
    return CommonFunc::ConvertErrCode(ret);
}

void SetDefaultApplicationExec(napi_env env, void *data)
{
    DefaultAppCallbackInfo *asyncCallbackInfo = reinterpret_cast<DefaultAppCallbackInfo *>(data);
    if (asyncCallbackInfo == nullptr) {
        APP_LOGE("asyncCallbackInfo is null");
        return;
    }
    asyncCallbackInfo->err = InnerSetDefaultApplication(asyncCallbackInfo);
}

void SetDefaultApplicationComplete(napi_env env, napi_status status, void *data)
{
    DefaultAppCallbackInfo *asyncCallbackInfo = reinterpret_cast<DefaultAppCallbackInfo *>(data);
    if (asyncCallbackInfo == nullptr) {
        APP_LOGE("asyncCallbackInfo is null in %{public}s", __func__);
        return;
    }
    std::unique_ptr<DefaultAppCallbackInfo> callbackPtr {asyncCallbackInfo};
    napi_value result[ARGS_SIZE_ONE] = {0};
    if (asyncCallbackInfo->err == NO_ERROR) {
        NAPI_CALL_RETURN_VOID(env, napi_get_null(env, &result[ARGS_POS_ZERO]));
    } else {
        result[ARGS_POS_ZERO] = BusinessError::CreateCommonError(env, asyncCallbackInfo->err,
            SET_DEFAULT_APPLICATION, Constants::PERMISSION_SET_DEFAULT_APPLICATION);
    }
    if (asyncCallbackInfo->deferred) {
        if (asyncCallbackInfo->err == NO_ERROR) {
            NAPI_CALL_RETURN_VOID(env, napi_resolve_deferred(env, asyncCallbackInfo->deferred, result[ARGS_POS_ZERO]));
        } else {
            NAPI_CALL_RETURN_VOID(env, napi_reject_deferred(env, asyncCallbackInfo->deferred, result[ARGS_POS_ZERO]));
        }
    } else {
        napi_value callback = nullptr;
        napi_value placeHolder = nullptr;
        NAPI_CALL_RETURN_VOID(env, napi_get_reference_value(env, asyncCallbackInfo->callback, &callback));
        NAPI_CALL_RETURN_VOID(env, napi_call_function(env, nullptr, callback,
            sizeof(result) / sizeof(result[ARGS_POS_ZERO]), result, &placeHolder));
    }
}

napi_value SetDefaultApplication(napi_env env, napi_callback_info info)
{
    APP_LOGD("begin to SetDefaultApplication");
    NapiArg args(env, info);
    DefaultAppCallbackInfo *asyncCallbackInfo = new (std::nothrow) DefaultAppCallbackInfo(env);
    if (asyncCallbackInfo == nullptr) {
        APP_LOGE("asyncCallbackInfo is null");
        return nullptr;
    }
    asyncCallbackInfo->userId = IPCSkeleton::GetCallingUid() / Constants::BASE_USER_RANGE;
    std::unique_ptr<DefaultAppCallbackInfo> callbackPtr {asyncCallbackInfo};
    if (!args.Init(ARGS_SIZE_TWO, ARGS_SIZE_FOUR)) {
        APP_LOGE("param count invalid");
        BusinessError::ThrowTooFewParametersError(env, ERROR_PARAM_CHECK_ERROR);
        return nullptr;
    }
    for (size_t i = 0; i < args.GetMaxArgc(); ++i) {
        napi_valuetype valueType = napi_undefined;
        napi_typeof(env, args[i], &valueType);
        if ((i == ARGS_POS_ZERO) && (valueType == napi_string)) {
            if (!ParseType(env, args[i], asyncCallbackInfo->type)) {
                APP_LOGE("type invalid!");
                BusinessError::ThrowError(env, ERROR_PARAM_CHECK_ERROR, PARAM_TYPE_CHECK_ERROR);
                return nullptr;
            }
        } else if ((i == ARGS_POS_ONE) && (valueType == napi_object)) {
            if (!CommonFunc::ParseElementName(env, args[i], asyncCallbackInfo->want)) {
                APP_LOGE("invalid elementName");
                BusinessError::ThrowError(env, ERROR_PARAM_CHECK_ERROR, PARAM_TYPE_CHECK_ERROR);
                return nullptr;
            }
        } else if (i == ARGS_POS_TWO) {
            if (valueType == napi_number) {
                CommonFunc::ParseInt(env, args[i], asyncCallbackInfo->userId);
            } else if (valueType == napi_function) {
                NAPI_CALL(env, napi_create_reference(env, args[i], NAPI_RETURN_ONE, &asyncCallbackInfo->callback));
                break;
            } else {
                APP_LOGE("param check error");
                std::string errMsg = PARAM_TYPE_CHECK_ERROR_WITH_POS + std::to_string(i + 1);
                BusinessError::ThrowError(env, ERROR_PARAM_CHECK_ERROR, errMsg);
                return nullptr;
            }
        } else if (i == ARGS_POS_THREE) {
            if (valueType == napi_function) {
                NAPI_CALL(env, napi_create_reference(env, args[i], NAPI_RETURN_ONE, &asyncCallbackInfo->callback));
            }
            break;
        } else {
            APP_LOGE("param check error");
            std::string errMsg = PARAM_TYPE_CHECK_ERROR_WITH_POS + std::to_string(i + 1);
            BusinessError::ThrowError(env, ERROR_PARAM_CHECK_ERROR, errMsg);
            return nullptr;
        }
    }
    auto promise = CommonFunc::AsyncCallNativeMethod<DefaultAppCallbackInfo>(
        env, asyncCallbackInfo, SET_DEFAULT_APPLICATION, SetDefaultApplicationExec, SetDefaultApplicationComplete);
    callbackPtr.release();
    APP_LOGD("call SetDefaultApplication done");
    return promise;
}

static ErrCode InnerResetDefaultApplication(const DefaultAppCallbackInfo *info)
{
    if (info == nullptr) {
        APP_LOGE("info is null");
        return ERROR_BUNDLE_SERVICE_EXCEPTION;
    }
    auto defaultAppProxy = GetDefaultAppProxy();
    if (defaultAppProxy == nullptr) {
        APP_LOGE("defaultAppProxy is null");
        return ERROR_BUNDLE_SERVICE_EXCEPTION;
    }
    ErrCode ret = defaultAppProxy->ResetDefaultApplication(info->userId, info->type);
    APP_LOGD("ResetDefaultApplication ErrCode : %{public}d", ret);
    return CommonFunc::ConvertErrCode(ret);
}

void ResetDefaultApplicationExec(napi_env env, void *data)
{
    DefaultAppCallbackInfo *asyncCallbackInfo = reinterpret_cast<DefaultAppCallbackInfo *>(data);
    if (asyncCallbackInfo == nullptr) {
        APP_LOGE("asyncCallbackInfo is null");
        return;
    }
    asyncCallbackInfo->err = InnerResetDefaultApplication(asyncCallbackInfo);
}

void ResetDefaultApplicationComplete(napi_env env, napi_status status, void *data)
{
    DefaultAppCallbackInfo *asyncCallbackInfo = reinterpret_cast<DefaultAppCallbackInfo *>(data);
    if (asyncCallbackInfo == nullptr) {
        APP_LOGE("asyncCallbackInfo is null in %{public}s", __func__);
        return;
    }
    std::unique_ptr<DefaultAppCallbackInfo> callbackPtr {asyncCallbackInfo};
    napi_value result[ARGS_SIZE_ONE] = {0};
    if (asyncCallbackInfo->err == NO_ERROR) {
        NAPI_CALL_RETURN_VOID(env, napi_get_null(env, &result[ARGS_POS_ZERO]));
    } else {
        result[ARGS_POS_ZERO] = BusinessError::CreateCommonError(env, asyncCallbackInfo->err,
            RESET_DEFAULT_APPLICATION, Constants::PERMISSION_SET_DEFAULT_APPLICATION);
    }
    if (asyncCallbackInfo->deferred) {
        if (asyncCallbackInfo->err == NO_ERROR) {
            NAPI_CALL_RETURN_VOID(env, napi_resolve_deferred(env, asyncCallbackInfo->deferred, result[ARGS_POS_ZERO]));
        } else {
            NAPI_CALL_RETURN_VOID(env, napi_reject_deferred(env, asyncCallbackInfo->deferred, result[ARGS_POS_ZERO]));
        }
    } else {
        napi_value callback = nullptr;
        napi_value placeHolder = nullptr;
        NAPI_CALL_RETURN_VOID(env, napi_get_reference_value(env, asyncCallbackInfo->callback, &callback));
        NAPI_CALL_RETURN_VOID(env, napi_call_function(env, nullptr, callback,
            sizeof(result) / sizeof(result[ARGS_POS_ZERO]), result, &placeHolder));
    }
}

napi_value ResetDefaultApplication(napi_env env, napi_callback_info info)
{
    APP_LOGD("begin to ResetDefaultApplication");
    NapiArg args(env, info);
    DefaultAppCallbackInfo *asyncCallbackInfo = new (std::nothrow) DefaultAppCallbackInfo(env);
    if (asyncCallbackInfo == nullptr) {
        APP_LOGE("asyncCallbackInfo is null");
        return nullptr;
    }
    asyncCallbackInfo->userId = IPCSkeleton::GetCallingUid() / Constants::BASE_USER_RANGE;
    std::unique_ptr<DefaultAppCallbackInfo> callbackPtr {asyncCallbackInfo};
    if (!args.Init(ARGS_SIZE_ONE, ARGS_SIZE_THREE)) {
        APP_LOGE("param count invalid");
        BusinessError::ThrowTooFewParametersError(env, ERROR_PARAM_CHECK_ERROR);
        return nullptr;
    }
    for (size_t i = 0; i < args.GetMaxArgc(); ++i) {
        napi_valuetype valueType = napi_undefined;
        napi_typeof(env, args[i], &valueType);
        if ((i == ARGS_POS_ZERO) && (valueType == napi_string)) {
            if (!ParseType(env, args[i], asyncCallbackInfo->type)) {
                APP_LOGE("type invalid!");
                BusinessError::ThrowError(env, ERROR_PARAM_CHECK_ERROR, PARAM_TYPE_CHECK_ERROR);
                return nullptr;
            }
        } else if (i == ARGS_POS_ONE) {
            if (valueType == napi_number) {
                CommonFunc::ParseInt(env, args[i], asyncCallbackInfo->userId);
            } else if (valueType == napi_function) {
                NAPI_CALL(env, napi_create_reference(env, args[i], NAPI_RETURN_ONE, &asyncCallbackInfo->callback));
                break;
            } else {
                APP_LOGE("param check error");
                std::string errMsg = PARAM_TYPE_CHECK_ERROR_WITH_POS + std::to_string(i + 1);
                BusinessError::ThrowError(env, ERROR_PARAM_CHECK_ERROR, errMsg);
                return nullptr;
            }
        } else if (i == ARGS_POS_TWO) {
            if (valueType == napi_function) {
                NAPI_CALL(env, napi_create_reference(env, args[i], NAPI_RETURN_ONE, &asyncCallbackInfo->callback));
            }
            break;
        } else {
            APP_LOGE("param check error");
            std::string errMsg = PARAM_TYPE_CHECK_ERROR_WITH_POS + std::to_string(i + 1);
            BusinessError::ThrowError(env, ERROR_PARAM_CHECK_ERROR, errMsg);
            return nullptr;
        }
    }
    auto promise = CommonFunc::AsyncCallNativeMethod<DefaultAppCallbackInfo>(env,
        asyncCallbackInfo, RESET_DEFAULT_APPLICATION, ResetDefaultApplicationExec, ResetDefaultApplicationComplete);
    callbackPtr.release();
    APP_LOGD("call ResetDefaultApplication done");
    return promise;
}
}
}
