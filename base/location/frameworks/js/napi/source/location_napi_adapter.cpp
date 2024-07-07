/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#include "location_napi_adapter.h"
#include "location_log.h"
#include "location_napi_errcode.h"
#include "constant_definition.h"

namespace OHOS {
namespace Location {
std::unique_ptr<Locator> g_locatorClient = Locator::GetInstance();

napi_value GetLastLocation(napi_env env, napi_callback_info info)
{
    LBSLOGI(LOCATOR_STANDARD, "%{public}s called.", __func__);
    size_t argc = PARAM1;
    napi_value argv[argc];
    napi_value thisVar = nullptr;
    void* data = nullptr;

    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, &data));
    NAPI_ASSERT(env, g_locatorClient != nullptr, "get locator SA failed");

    auto asyncContext = new (std::nothrow) LocationAsyncContext(env);
    NAPI_ASSERT(env, asyncContext != nullptr, "asyncContext is null.");
    NAPI_CALL(env, napi_create_string_latin1(env, "getLastLocation", NAPI_AUTO_LENGTH, &asyncContext->resourceName));
#ifdef ENABLE_NAPI_MANAGER
    return HandleGetCachedLocation(env);
#else
    asyncContext->executeFunc = [&](void* data) -> void {
        auto context = static_cast<LocationAsyncContext*>(data);
        context->loc = g_locatorClient->IsLocationEnabled() ? g_locatorClient->GetCachedLocation() : nullptr;
        if (context->loc != nullptr) {
            context->errCode = SUCCESS;
        } else {
            context->errCode = LAST_KNOWN_LOCATION_ERROR;
        }
    };

    asyncContext->completeFunc = [&](void* data) -> void {
        auto context = static_cast<LocationAsyncContext*>(data);
        NAPI_CALL_RETURN_VOID(context->env, napi_create_object(context->env, &context->result[PARAM1]));
        if (context->loc != nullptr) {
            LocationToJs(context->env, context->loc, context->result[PARAM1]);
        } else {
            LBSLOGE(LOCATOR_STANDARD, "loc is nullptr!");
        }
        LBSLOGI(LOCATOR_STANDARD, "Push last location result to client");
    };

    size_t objectArgsNum = 0;
    return DoAsyncWork(env, asyncContext, argc, argv, objectArgsNum);
#endif
}

#ifdef ENABLE_NAPI_MANAGER
napi_value HandleGetCachedLocation(napi_env env)
{
    napi_value res;
    NAPI_CALL(env, napi_create_object(env, &res));
    LocationErrCode errorCode = CheckLocationSwitchState();
    if (errorCode != ERRCODE_SUCCESS) {
        HandleSyncErrCode(env, errorCode);
        return UndefinedNapiValue(env);
    }
    std::unique_ptr<Location> loc;
    errorCode = g_locatorClient->GetCachedLocationV9(loc);
    if (loc != nullptr) {
        LocationToJs(env, loc, res);
        return res;
    } else {
        HandleSyncErrCode(env, errorCode);
    }
    return UndefinedNapiValue(env);
}
#endif

napi_value IsLocationEnabled(napi_env env, napi_callback_info info)
{
    LBSLOGI(LOCATOR_STANDARD, "%{public}s called.", __func__);
    size_t argc = PARAM1;
    napi_value argv[argc];
    napi_value thisVar = nullptr;
    void* data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, &data));
    NAPI_ASSERT(env, g_locatorClient != nullptr, "get locator SA failed");

#ifdef ENABLE_NAPI_MANAGER
    napi_value res;
    bool isEnabled = false;
    LocationErrCode errorCode = g_locatorClient->IsLocationEnabledV9(isEnabled);

    if (errorCode != ERRCODE_SUCCESS) {
        HandleSyncErrCode(env, errorCode);
        return UndefinedNapiValue(env);
    }
    NAPI_CALL(env, napi_get_boolean(env, isEnabled, &res));
    return res;
#else
    auto asyncContext = new (std::nothrow) SwitchAsyncContext(env);
    NAPI_ASSERT(env, asyncContext != nullptr, "asyncContext is null.");
    napi_create_string_latin1(env, "isLocationEnabled", NAPI_AUTO_LENGTH, &asyncContext->resourceName);
    asyncContext->executeFunc = [&](void* data) -> void {
        auto context = static_cast<SwitchAsyncContext*>(data);
        context->enable = g_locatorClient->IsLocationEnabled();
        context->errCode = SUCCESS;
    };

    asyncContext->completeFunc = [&](void* data) -> void {
        auto context = static_cast<SwitchAsyncContext*>(data);
        NAPI_CALL_RETURN_VOID(context->env, napi_get_boolean(context->env, context->enable, &context->result[PARAM1]));
        LBSLOGI(LOCATOR_STANDARD, "Push IsLocationEnabled result to client");
    };

    size_t objectArgsNum = 0;
    return DoAsyncWork(env, asyncContext, argc, argv, objectArgsNum);
#endif
}

napi_value EnableLocation(napi_env env, napi_callback_info info)
{
    LBSLOGI(LOCATOR_STANDARD, "%{public}s called.", __func__);
    size_t argc = PARAM1;
    napi_value argv[argc];
    napi_value thisVar = nullptr;
    void* data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, &data));
    NAPI_ASSERT(env, g_locatorClient != nullptr, "get locator SA failed");
#ifdef ENABLE_NAPI_MANAGER
    if (argc > PARAM1 || (argc == PARAM1 && !CheckIfParamIsFunctionType(env, argv[PARAM0]))) {
        HandleSyncErrCode(env, ERRCODE_INVALID_PARAM);
        return UndefinedNapiValue(env);
    }
#endif
    auto asyncContext = new (std::nothrow) SwitchAsyncContext(env);
    NAPI_ASSERT(env, asyncContext != nullptr, "asyncContext is null.");
    NAPI_CALL(env, napi_create_string_latin1(env, "enableLocation", NAPI_AUTO_LENGTH, &asyncContext->resourceName));

    asyncContext->executeFunc = [&](void* data) -> void {
        auto context = static_cast<SwitchAsyncContext*>(data);
#ifdef ENABLE_NAPI_MANAGER
        context->errCode = g_locatorClient->EnableAbilityV9(true);
#else
        g_locatorClient->EnableAbility(true);
        context->errCode = SUCCESS;
#endif
    };

    asyncContext->completeFunc = [&](void* data) -> void {
        auto context = static_cast<SwitchAsyncContext*>(data);
#ifdef ENABLE_NAPI_MANAGER
        NAPI_CALL_RETURN_VOID(context->env, napi_get_undefined(context->env, &context->result[PARAM1]));
#else
        NAPI_CALL_RETURN_VOID(context->env,
            napi_get_boolean(context->env, context->errCode == SUCCESS, &context->result[PARAM1]));
#endif
        LBSLOGI(LOCATOR_STANDARD, "Push EnableLocation result to client");
    };

    size_t objectArgsNum = 0;
    return DoAsyncWork(env, asyncContext, argc, argv, objectArgsNum);
}

napi_value DisableLocation(napi_env env, napi_callback_info info)
{
    LBSLOGI(LOCATOR_STANDARD, "%{public}s called.", __func__);
    size_t argc = PARAM1;
    napi_value argv[argc];
    napi_value thisVar = nullptr;
    void* data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, &data));
    NAPI_ASSERT(env, g_locatorClient != nullptr, "get locator SA failed");

#ifdef ENABLE_NAPI_MANAGER
    LocationErrCode errorCode = g_locatorClient->EnableAbilityV9(false);
    if (errorCode != ERRCODE_SUCCESS) {
        HandleSyncErrCode(env, errorCode);
    }
    return UndefinedNapiValue(env);
#else
    auto asyncContext = new (std::nothrow) SwitchAsyncContext(env);
    NAPI_ASSERT(env, asyncContext != nullptr, "asyncContext is null.");
    NAPI_CALL(env, napi_create_string_latin1(env, "disableLocation", NAPI_AUTO_LENGTH, &asyncContext->resourceName));

    asyncContext->executeFunc = [&](void* data) -> void {
        auto context = static_cast<SwitchAsyncContext*>(data);
        g_locatorClient->EnableAbility(false);
        context->errCode = SUCCESS;
    };

    asyncContext->completeFunc = [&](void* data) -> void {
        auto context = static_cast<SwitchAsyncContext*>(data);
        NAPI_CALL_RETURN_VOID(context->env,
            napi_get_boolean(context->env, context->errCode == SUCCESS, &context->result[PARAM1]));
        LBSLOGI(LOCATOR_STANDARD, "Push DisableLocation result to client");
    };

    size_t objectArgsNum = 0;
    return DoAsyncWork(env, asyncContext, argc, argv, objectArgsNum);
#endif
}

napi_value RequestEnableLocation(napi_env env, napi_callback_info info)
{
    LBSLOGI(LOCATOR_STANDARD, "%{public}s called.", __func__);
    size_t argc = PARAM1;
    napi_value argv[argc];
    napi_value thisVar = nullptr;
    void* data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, &data));
    NAPI_ASSERT(env, g_locatorClient != nullptr, "get locator SA failed");

    auto asyncContext = new (std::nothrow) SwitchAsyncContext(env);
    NAPI_ASSERT(env, asyncContext != nullptr, "asyncContext is null.");
    NAPI_CALL(env, napi_create_string_latin1(env, "enableLocation", NAPI_AUTO_LENGTH, &asyncContext->resourceName));

    asyncContext->executeFunc = [&](void* data) -> void {
        auto context = static_cast<SwitchAsyncContext*>(data);
        if (!g_locatorClient->IsLocationEnabled()) {
            g_locatorClient->ShowNotification();
        }
        g_locatorClient->EnableAbility(true);
        context->errCode = SUCCESS;
    };

    asyncContext->completeFunc = [&](void* data) -> void {
        auto context = static_cast<SwitchAsyncContext*>(data);
        NAPI_CALL_RETURN_VOID(context->env,
            napi_get_boolean(context->env, context->errCode == SUCCESS, &context->result[PARAM1]));
        LBSLOGI(LOCATOR_STANDARD, "Push RequestEnableLocation result to client");
    };

    size_t objectArgsNum = 0;
    return DoAsyncWork(env, asyncContext, argc, argv, objectArgsNum);
}

napi_value IsGeoServiceAvailable(napi_env env, napi_callback_info info)
{
    LBSLOGI(LOCATOR_STANDARD, "%{public}s called.", __func__);
    size_t argc = 1;
    napi_value argv[argc];
    napi_value thisVar = nullptr;
    void* data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, &data));
    NAPI_ASSERT(env, g_locatorClient != nullptr, "get locator SA failed");

#ifdef ENABLE_NAPI_MANAGER
    napi_value res;
    bool isAvailable = false;
    LocationErrCode errorCode = g_locatorClient->IsGeoServiceAvailableV9(isAvailable);
    if (errorCode != ERRCODE_SUCCESS) {
        HandleSyncErrCode(env, errorCode);
        return UndefinedNapiValue(env);
    }
    NAPI_CALL(env, napi_get_boolean(env, isAvailable, &res));
    return res;
#else
    auto asyncContext = new (std::nothrow) SwitchAsyncContext(env);
    NAPI_ASSERT(env, asyncContext != nullptr, "asyncContext is null.");
    NAPI_CALL(env,
        napi_create_string_latin1(env, "isGeoServiceAvailable", NAPI_AUTO_LENGTH, &asyncContext->resourceName));

    asyncContext->executeFunc = [&](void* data) -> void {
        auto context = static_cast<SwitchAsyncContext*>(data);
        bool isAvailable = g_locatorClient->IsGeoServiceAvailable();
        context->enable = isAvailable;
        context->errCode = SUCCESS;
    };

    asyncContext->completeFunc = [&](void* data) -> void {
        auto context = static_cast<SwitchAsyncContext*>(data);
        NAPI_CALL_RETURN_VOID(context->env, napi_get_boolean(context->env, context->enable, &context->result[PARAM1]));
        LBSLOGI(LOCATOR_STANDARD, "Push isGeoServiceAvailable result to client");
    };

    size_t objectArgsNum = 0;
    return DoAsyncWork(env, asyncContext, argc, argv, objectArgsNum);
#endif
}

void CreateReverseGeocodeAsyncContext(ReverseGeoCodeAsyncContext* asyncContext)
{
    asyncContext->executeFunc = [&](void* data) -> void {
        auto context = static_cast<ReverseGeoCodeAsyncContext*>(data);
#ifdef ENABLE_NAPI_MANAGER
        if (context->errCode != ERRCODE_SUCCESS) {
#else
        if (context->errCode != SUCCESS) {
#endif
            return;
        }
#ifdef ENABLE_NAPI_MANAGER
        bool isAvailable = false;
        LocationErrCode errorCode = g_locatorClient->IsGeoServiceAvailableV9(isAvailable);
        if (errorCode != ERRCODE_SUCCESS) {
            context->errCode = errorCode;
            return;
        }
        if (!isAvailable) {
            context->errCode = ERRCODE_REVERSE_GEOCODING_FAIL;
            return;
        }
        errorCode = g_locatorClient->GetAddressByCoordinateV9(context->reverseGeoCodeRequest, context->replyList);
        if (context->replyList.empty() || errorCode != ERRCODE_SUCCESS) {
            context->errCode = errorCode;
        }
#else
        if (!g_locatorClient->IsGeoServiceAvailable()) {
            context->errCode = REVERSE_GEOCODE_ERROR;
            return;
        }
        g_locatorClient->GetAddressByCoordinate(context->reverseGeoCodeRequest, context->replyList);
        if (context->replyList.empty()) {
            context->errCode = REVERSE_GEOCODE_ERROR;
        }
#endif
    };
    asyncContext->completeFunc = [&](void* data) -> void {
        auto context = static_cast<ReverseGeoCodeAsyncContext*>(data);
        NAPI_CALL_RETURN_VOID(context->env,
            napi_create_array_with_length(context->env, context->replyList.size(), &context->result[PARAM1]));
        GeoAddressesToJsObj(context->env, context->replyList, context->result[PARAM1]);
    };
}

void CreateGeocodeAsyncContext(GeoCodeAsyncContext* asyncContext)
{
    asyncContext->executeFunc = [&](void* data) -> void {
        auto context = static_cast<GeoCodeAsyncContext*>(data);
        if (context->errCode != SUCCESS) {
            return;
        }
#ifdef ENABLE_NAPI_MANAGER
        bool isAvailable = false;
        LocationErrCode errorCode = g_locatorClient->IsGeoServiceAvailableV9(isAvailable);
        if (errorCode != ERRCODE_SUCCESS) {
            context->errCode = errorCode;
            return;
        }
        if (!isAvailable) {
            context->errCode = ERRCODE_GEOCODING_FAIL;
            return;
        }
        errorCode = g_locatorClient->GetAddressByLocationNameV9(context->geoCodeRequest, context->replyList);
        if (context->replyList.empty() || errorCode != ERRCODE_SUCCESS) {
            context->errCode = errorCode;
        }
#else
        if (!g_locatorClient->IsGeoServiceAvailable()) {
            context->errCode = GEOCODE_ERROR;
            return;
        }
        g_locatorClient->GetAddressByLocationName(context->geoCodeRequest, context->replyList);
        if (context->replyList.empty()) {
            context->errCode = GEOCODE_ERROR;
        }
#endif
    };
    asyncContext->completeFunc = [&](void* data) -> void {
        auto context = static_cast<GeoCodeAsyncContext*>(data);
        NAPI_CALL_RETURN_VOID(context->env,
            napi_create_array_with_length(context->env, context->replyList.size(), &context->result[PARAM1]));
        GeoAddressesToJsObj(context->env, context->replyList, context->result[PARAM1]);
        LBSLOGI(LOCATOR_STANDARD, "Push GetAddressesFromLocationName result to client");
    };
}

napi_value GetAddressesFromLocation(napi_env env, napi_callback_info info)
{
    LBSLOGI(LOCATOR_STANDARD, "%{public}s called.", __func__);
    size_t argc = 2;
    napi_value argv[argc];
    napi_value thisVar = nullptr;
    void* data = nullptr;
    NAPI_ASSERT(env, g_locatorClient != nullptr, "get locator SA failed");
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, &data));
#ifdef ENABLE_NAPI_MANAGER
    if (argc < PARAM1 || argc > PARAM2 || (argc == PARAM2 && !CheckIfParamIsFunctionType(env, argv[1]))) {
        HandleSyncErrCode(env, ERRCODE_INVALID_PARAM);
        return UndefinedNapiValue(env);
    }
#else
    NAPI_ASSERT(env, argc >= 1, "Wrong number of arguments");
#endif

    napi_valuetype valueType;
    NAPI_CALL(env, napi_typeof(env, argv[0], &valueType));
#ifdef ENABLE_NAPI_MANAGER
    if (valueType != napi_object) {
        HandleSyncErrCode(env, ERRCODE_INVALID_PARAM);
        return UndefinedNapiValue(env);
    }
#else
    NAPI_ASSERT(env, valueType == napi_object, "Wrong argument type, object is expected for parameter 1.");
#endif
    auto asyncContext = new (std::nothrow) ReverseGeoCodeAsyncContext(env);
    NAPI_ASSERT(env, asyncContext != nullptr, "asyncContext is null.");
    NAPI_CALL(env, napi_create_string_latin1(env, "getAddressesFromLocation",
        NAPI_AUTO_LENGTH, &asyncContext->resourceName));
    bool ret = JsObjToReverseGeoCodeRequest(env, argv[0], asyncContext->reverseGeoCodeRequest);
#ifdef ENABLE_NAPI_MANAGER
    asyncContext->errCode = ret ? ERRCODE_SUCCESS : ERRCODE_INVALID_PARAM;
#else
    asyncContext->errCode = ret ? SUCCESS : INPUT_PARAMS_ERROR;
#endif
#ifdef ENABLE_NAPI_MANAGER
    if (asyncContext->errCode != SUCCESS) {
        HandleSyncErrCode(env, asyncContext->errCode);
        return UndefinedNapiValue(env);
    }
#else
    NAPI_ASSERT(env, asyncContext->errCode != INPUT_PARAMS_ERROR,
        "The input params should be checked first.");
#endif
    CreateReverseGeocodeAsyncContext(asyncContext);

    size_t objectArgsNum = 1;
    return DoAsyncWork(env, asyncContext, argc, argv, objectArgsNum);
}

napi_value GetAddressesFromLocationName(napi_env env, napi_callback_info info)
{
    LBSLOGI(LOCATOR_STANDARD, "%{public}s called.", __func__);
    size_t argc = 2;
    napi_value argv[argc];
    napi_value thisVar = nullptr;
    void* data = nullptr;
    NAPI_ASSERT(env, g_locatorClient != nullptr, "get locator SA failed");
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, &data));
#ifdef ENABLE_NAPI_MANAGER
    if (argc < PARAM1 || argc > PARAM2 || (argc == PARAM2 && !CheckIfParamIsFunctionType(env, argv[1]))) {
        HandleSyncErrCode(env, ERRCODE_INVALID_PARAM);
        return UndefinedNapiValue(env);
    }
#else
    NAPI_ASSERT(env, argc >= 1, "Wrong number of arguments");
#endif

    napi_valuetype valueType;
    NAPI_CALL(env, napi_typeof(env, argv[0], &valueType));
#ifdef ENABLE_NAPI_MANAGER
    if (valueType != napi_object) {
        HandleSyncErrCode(env, ERRCODE_INVALID_PARAM);
        return UndefinedNapiValue(env);
    }
#else
    NAPI_ASSERT(env, valueType == napi_object, "Wrong argument type, object is expected for parameter 1.");
#endif

    auto asyncContext = new (std::nothrow) GeoCodeAsyncContext(env);
    NAPI_ASSERT(env, asyncContext != nullptr, "asyncContext is null.");
    NAPI_CALL(env,
        napi_create_string_latin1(env, "GetAddressesFromLocationName", NAPI_AUTO_LENGTH, &asyncContext->resourceName));
    asyncContext->errCode = JsObjToGeoCodeRequest(env, argv[0], asyncContext->geoCodeRequest);
#ifdef ENABLE_NAPI_MANAGER
    if (asyncContext->errCode == INPUT_PARAMS_ERROR) {
        HandleSyncErrCode(env, ERRCODE_INVALID_PARAM);
        return UndefinedNapiValue(env);
    }
#else
    NAPI_ASSERT(env, asyncContext->errCode != INPUT_PARAMS_ERROR,
        "The input params should be checked first.");
#endif
    CreateGeocodeAsyncContext(asyncContext);
    size_t objectArgsNum = 1;
    return DoAsyncWork(env, asyncContext, argc, argv, objectArgsNum);
}

#ifdef ENABLE_NAPI_MANAGER
napi_value IsLocationPrivacyConfirmed(napi_env env, napi_callback_info info)
{
    LBSLOGI(LOCATOR_STANDARD, "%{public}s called.", __func__);
    size_t argc = 2;
    napi_value argv[argc];
    napi_value thisVar = nullptr;
    void* data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, &data));
    if (g_locatorClient == nullptr) {
        HandleSyncErrCode(env, ERRCODE_SERVICE_UNAVAILABLE);
        return UndefinedNapiValue(env);
    }
    // 1 arguement is necessary
    if (argc != PARAM1) {
        HandleSyncErrCode(env, ERRCODE_INVALID_PARAM);
        return UndefinedNapiValue(env);
    }
    napi_valuetype valueType;
    NAPI_CALL(env, napi_typeof(env, argv[0], &valueType));
    if (valueType != napi_number) {
        HandleSyncErrCode(env, ERRCODE_INVALID_PARAM);
        return UndefinedNapiValue(env);
    }
    int type;
    NAPI_CALL(env, napi_get_value_int32(env, argv[0], &type));
    napi_value res;
    bool isEnabled = false;
    LocationErrCode errorCode = g_locatorClient->IsLocationPrivacyConfirmedV9(type, isEnabled);
    if (errorCode != ERRCODE_SUCCESS) {
        HandleSyncErrCode(env, errorCode);
        return UndefinedNapiValue(env);
    }
    NAPI_CALL(env, napi_get_boolean(env, isEnabled, &res));
    return res;
}

napi_value SetLocationPrivacyConfirmStatus(napi_env env, napi_callback_info info)
{
    LBSLOGI(LOCATOR_STANDARD, "%{public}s called.", __func__);
    size_t argc = 2;
    napi_value argv[argc];
    napi_value thisVar = nullptr;
    void* data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, &data));
    if (g_locatorClient == nullptr) {
        HandleSyncErrCode(env, ERRCODE_SERVICE_UNAVAILABLE);
        return UndefinedNapiValue(env);
    }
    // 2 arguement is necessary
    if (argc != PARAM2) {
        HandleSyncErrCode(env, ERRCODE_INVALID_PARAM);
        return UndefinedNapiValue(env);
    }
    napi_valuetype valueType1;
    napi_valuetype valueType2;
    NAPI_CALL(env, napi_typeof(env, argv[0], &valueType1));
    NAPI_CALL(env, napi_typeof(env, argv[1], &valueType2));
    if (valueType1 != napi_number || valueType2 != napi_boolean) {
        HandleSyncErrCode(env, ERRCODE_INVALID_PARAM);
        return UndefinedNapiValue(env);
    }
    int type;
    NAPI_CALL(env, napi_get_value_int32(env, argv[0], &type));
    bool isConfirmed;
    NAPI_CALL(env, napi_get_value_bool(env, argv[1], &isConfirmed));
    LocationErrCode errorCode = g_locatorClient->SetLocationPrivacyConfirmStatusV9(type, isConfirmed);
    if (errorCode != ERRCODE_SUCCESS) {
        HandleSyncErrCode(env, errorCode);
    }
    return UndefinedNapiValue(env);
}
#endif

napi_value GetCachedGnssLocationsSize(napi_env env, napi_callback_info info)
{
    LBSLOGI(LOCATOR_STANDARD, "%{public}s called.", __func__);
    size_t argc = 1;
    napi_value argv[argc];
    napi_value thisVar = nullptr;
    void* data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, &data));
    NAPI_ASSERT(env, g_locatorClient != nullptr, "locator instance is null.");
#ifdef ENABLE_NAPI_MANAGER
    if (argc > PARAM1 || (argc == PARAM1 && !CheckIfParamIsFunctionType(env, argv[PARAM0]))) {
        HandleSyncErrCode(env, ERRCODE_INVALID_PARAM);
        return UndefinedNapiValue(env);
    }
#endif
    auto asyncContext = new (std::nothrow) CachedAsyncContext(env);
    NAPI_ASSERT(env, asyncContext != nullptr, "asyncContext is null.");
    NAPI_CALL(env,
        napi_create_string_latin1(env, "GetCachedGnssLocationsSize", NAPI_AUTO_LENGTH, &asyncContext->resourceName));

    asyncContext->executeFunc = [&](void* data) -> void {
        auto context = static_cast<CachedAsyncContext*>(data);
#ifdef ENABLE_NAPI_MANAGER
        LocationErrCode errorCode = CheckLocationSwitchState();
        if (errorCode != ERRCODE_SUCCESS) {
            context->errCode = errorCode;
            return;
        }
#endif
        
#ifdef ENABLE_NAPI_MANAGER
        int size = -1;
        g_locatorClient->GetCachedGnssLocationsSizeV9(size);
        context->errCode = ERRCODE_NOT_SUPPORTED;
        context->locationSize = size;
#else
        context->locationSize = g_locatorClient->GetCachedGnssLocationsSize();
        context->errCode = (context->locationSize >= 0) ? SUCCESS : NOT_SUPPORTED;
#endif
    };
    asyncContext->completeFunc = [&](void* data) -> void {
        auto context = static_cast<CachedAsyncContext*>(data);
        NAPI_CALL_RETURN_VOID(context->env,
            napi_create_int32(context->env, context->locationSize, &context->result[PARAM1]));
        LBSLOGI(LOCATOR_STANDARD, "Push GetCachedGnssLocationsSize result to client");
    };

    size_t objectArgsNum = 0;
    return DoAsyncWork(env, asyncContext, argc, argv, objectArgsNum);
}

napi_value FlushCachedGnssLocations(napi_env env, napi_callback_info info)
{
    LBSLOGI(LOCATOR_STANDARD, "%{public}s called.", __func__);
    size_t argc = 1;
    napi_value argv[argc];
    napi_value thisVar = nullptr;
    void* data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, &data));
    NAPI_ASSERT(env, g_locatorClient != nullptr, "locator instance is null.");
#ifdef ENABLE_NAPI_MANAGER
    if (argc > PARAM1 || (argc == PARAM1 && !CheckIfParamIsFunctionType(env, argv[PARAM0]))) {
        HandleSyncErrCode(env, ERRCODE_INVALID_PARAM);
        return UndefinedNapiValue(env);
    }
#endif
    auto asyncContext = new (std::nothrow) CachedAsyncContext(env);
    NAPI_ASSERT(env, asyncContext != nullptr, "asyncContext is null.");
    NAPI_CALL(env,
        napi_create_string_latin1(env, "FlushCachedGnssLocations", NAPI_AUTO_LENGTH, &asyncContext->resourceName));

    asyncContext->executeFunc = [&](void* data) -> void {
        auto context = static_cast<CachedAsyncContext*>(data);
#ifdef ENABLE_NAPI_MANAGER
        LocationErrCode errorCode = CheckLocationSwitchState();
        if (errorCode != ERRCODE_SUCCESS) {
            context->errCode = errorCode;
            return;
        }
        g_locatorClient->FlushCachedGnssLocationsV9();
        context->errCode = ERRCODE_NOT_SUPPORTED;
#else
        if (g_locatorClient->IsLocationEnabled()) {
            g_locatorClient->FlushCachedGnssLocations();
        }
        context->errCode = NOT_SUPPORTED;
#endif
    };

    asyncContext->completeFunc = [&](void* data) -> void {
        auto context = static_cast<CachedAsyncContext*>(data);
#ifdef ENABLE_NAPI_MANAGER
        NAPI_CALL_RETURN_VOID(context->env, napi_get_undefined(context->env, &context->result[PARAM1]));
#else
        NAPI_CALL_RETURN_VOID(context->env,
            napi_get_boolean(context->env, context->errCode == SUCCESS, &context->result[PARAM1]));
#endif
        LBSLOGI(LOCATOR_STANDARD, "Push FlushCachedGnssLocations result to client");
    };

    size_t objectArgsNum = 0;
    return DoAsyncWork(env, asyncContext, argc, argv, objectArgsNum);
}

void CreateCommandAsyncContext(CommandAsyncContext* asyncContext)
{
    asyncContext->executeFunc = [&](void* data) -> void {
        auto context = static_cast<CommandAsyncContext*>(data);
#ifdef ENABLE_NAPI_MANAGER
        if (context->command != nullptr) {
            g_locatorClient->SendCommandV9(context->command);
        }
        context->errCode = ERRCODE_NOT_SUPPORTED;
#else
        if (context->command != nullptr) {
            g_locatorClient->SendCommand(context->command);
        }
        context->errCode = NOT_SUPPORTED;
#endif
    };
    asyncContext->completeFunc = [&](void* data) -> void {
        auto context = static_cast<CommandAsyncContext*>(data);
#ifdef ENABLE_NAPI_MANAGER
        NAPI_CALL_RETURN_VOID(context->env, napi_get_undefined(context->env, &context->result[PARAM1]));
#else
        NAPI_CALL_RETURN_VOID(context->env, napi_get_boolean(context->env,
            context->enable, &context->result[PARAM1]));
#endif
        LBSLOGI(LOCATOR_STANDARD, "Push SendCommand result to client");
    };
}

napi_value SendCommand(napi_env env, napi_callback_info info)
{
    LBSLOGI(LOCATOR_STANDARD, "%{public}s called.", __func__);
    size_t argc = 2;
    napi_value argv[argc];
    napi_value thisVar = nullptr;
    void* data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, &data));
    NAPI_ASSERT(env, g_locatorClient != nullptr, "locator instance is null.");
#ifdef ENABLE_NAPI_MANAGER
    if (argc < PARAM1 || argc > PARAM2 || (argc == PARAM2 && !CheckIfParamIsFunctionType(env, argv[1]))) {
        HandleSyncErrCode(env, ERRCODE_INVALID_PARAM);
        return UndefinedNapiValue(env);
    }
#else
    NAPI_ASSERT(env, argc >= 1, "Wrong number of arguments");
#endif

    napi_valuetype valueType;
    NAPI_CALL(env, napi_typeof(env, argv[0], &valueType));
#ifdef ENABLE_NAPI_MANAGER
    if (valueType != napi_object) {
        HandleSyncErrCode(env, ERRCODE_INVALID_PARAM);
        return UndefinedNapiValue(env);
    }
#else
    NAPI_ASSERT(env, valueType == napi_object, "Wrong argument type, object is expected for parameter 1.");
#endif

#ifdef ENABLE_NAPI_MANAGER
    if (argc == PARAM2 && !CheckIfParamIsFunctionType(env, argv[PARAM1])) {
        HandleSyncErrCode(env, ERRCODE_INVALID_PARAM);
        return UndefinedNapiValue(env);
    }
#endif
    auto asyncContext = new (std::nothrow) CommandAsyncContext(env);
    NAPI_ASSERT(env, asyncContext != nullptr, "asyncContext is null.");
    asyncContext->command = std::make_unique<LocationCommand>();
    NAPI_CALL(env, napi_create_string_latin1(env, "SendCommand", NAPI_AUTO_LENGTH, &asyncContext->resourceName));

    int errCode = JsObjToCommand(env, argv[0], asyncContext->command);
#ifdef ENABLE_NAPI_MANAGER
    if (errCode == INPUT_PARAMS_ERROR) {
        HandleSyncErrCode(env, ERRCODE_INVALID_PARAM);
        return UndefinedNapiValue(env);
    }
#else
    NAPI_ASSERT(env, errCode != INPUT_PARAMS_ERROR, "The input params should be checked first.");
#endif
    CreateCommandAsyncContext(asyncContext);
    size_t objectArgsNum = 1;
    return DoAsyncWork(env, asyncContext, argc, argv, objectArgsNum);
}

#ifdef ENABLE_NAPI_MANAGER
napi_value GetIsoCountryCode(napi_env env, napi_callback_info info)
{
    LBSLOGI(LOCATOR_STANDARD, "%{public}s called.", __func__);
    size_t argc = 1;
    napi_value argv[argc];
    napi_value thisVar = nullptr;
    void *data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, &data));
    NAPI_ASSERT(env, g_locatorClient != nullptr, "locator instance is null.");
    if (argc > PARAM1 || (argc == PARAM1 && !CheckIfParamIsFunctionType(env, argv[PARAM0]))) {
        HandleSyncErrCode(env, ERRCODE_INVALID_PARAM);
        return UndefinedNapiValue(env);
    }
    CountryCodeContext *asyncContext = new (std::nothrow) CountryCodeContext(env);
    NAPI_ASSERT(env, asyncContext != nullptr, "asyncContext is null.");
    napi_create_string_latin1(env, "CountryCodeContext", NAPI_AUTO_LENGTH, &asyncContext->resourceName);

    asyncContext->executeFunc = [&](void *data) -> void {
        if (data == nullptr) {
            LBSLOGE(LOCATOR_STANDARD, "GetIsoCountryCode data == nullptr");
            return;
        }
        CountryCodeContext *context = static_cast<CountryCodeContext*>(data);
        std::shared_ptr<CountryCode> country = std::make_shared<CountryCode>();
        LocationErrCode errorCode = g_locatorClient->GetIsoCountryCodeV9(country);
        context->errCode = errorCode;
        if (errorCode == ERRCODE_SUCCESS) {
            context->country = country;
        }
    };
    asyncContext->completeFunc = [&](void *data) -> void {
        if (data == nullptr) {
            LBSLOGE(LOCATOR_STANDARD, "GetIsoCountryCode data == nullptr");
            return;
        }
        CountryCodeContext *context = static_cast<CountryCodeContext *>(data);
        NAPI_CALL_RETURN_VOID(context->env, napi_create_object(context->env, &context->result[PARAM1]));
        if (context->country) {
            CountryCodeToJs(context->env, context->country, context->result[PARAM1]);
        } else {
            LBSLOGE(LOCATOR_STANDARD, "country is nullptr!");
        }
        LBSLOGI(LOCATOR_STANDARD, "Push GetIsoCountryCode result to client");
    };

    size_t objectArgsNum = 0;
    return DoAsyncWork(env, asyncContext, argc, argv, objectArgsNum);
}

int ParseLocationMockParams(napi_env env, LocationMockAsyncContext *asyncContext, napi_value object)
{
    CHK_ERROR_CODE("timeInterval", JsObjectToInt(env, object, "timeInterval", asyncContext->timeInterval), true);
    bool result = false;
    napi_value value = nullptr;
    NAPI_CALL_BASE(env, napi_has_named_property(env, object, "locations", &result), false);
    if (result) {
        NAPI_CALL_BASE(env, napi_get_named_property(env, object, "locations", &value), false);
        bool isArray = false;
        NAPI_CALL_BASE(env, napi_is_array(env, value, &isArray), false);
        if (!isArray) {
            LBSLOGE(LOCATOR_STANDARD, "not an array!");
            return INPUT_PARAMS_ERROR;
        }
        GetLocationArray(env, asyncContext, value);
    }
    return SUCCESS;
}

napi_value EnableLocationMock(napi_env env, napi_callback_info info)
{
    LBSLOGI(LOCATOR_STANDARD, "%{public}s called.", __func__);
    NAPI_ASSERT(env, g_locatorClient != nullptr, "locator instance is null.");
    LocationErrCode errorCode = CheckLocationSwitchState();
    if (errorCode != ERRCODE_SUCCESS) {
        HandleSyncErrCode(env, errorCode);
        return UndefinedNapiValue(env);
    }
    errorCode = g_locatorClient->EnableLocationMockV9();
    if (errorCode != ERRCODE_SUCCESS) {
        HandleSyncErrCode(env, errorCode);
    }
    return UndefinedNapiValue(env);
}

napi_value DisableLocationMock(napi_env env, napi_callback_info info)
{
    LBSLOGI(LOCATOR_STANDARD, "%{public}s called.", __func__);
    NAPI_ASSERT(env, g_locatorClient != nullptr, "locator instance is null.");
    LocationErrCode errorCode = CheckLocationSwitchState();
    if (errorCode != ERRCODE_SUCCESS) {
        HandleSyncErrCode(env, errorCode);
        return UndefinedNapiValue(env);
    }
    errorCode = g_locatorClient->DisableLocationMockV9();
    if (errorCode != ERRCODE_SUCCESS) {
        HandleSyncErrCode(env, errorCode);
    }
    return UndefinedNapiValue(env);
}

napi_value SetMockedLocations(napi_env env, napi_callback_info info)
{
    LBSLOGI(LOCATOR_STANDARD, "%{public}s called.", __func__);
    size_t argc = 2;
    napi_value argv[argc];
    napi_value thisVar = nullptr;
    void *data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, &data));
    NAPI_ASSERT(env, g_locatorClient != nullptr, "locator instance is null.");
    if (argc != PARAM1) {
        HandleSyncErrCode(env, ERRCODE_INVALID_PARAM);
        return UndefinedNapiValue(env);
    }
    napi_valuetype valueType;
    NAPI_CALL(env, napi_typeof(env, argv[0], &valueType));
    if (valueType != napi_object) {
        HandleSyncErrCode(env, ERRCODE_INVALID_PARAM);
        return UndefinedNapiValue(env);
    }

    LocationMockAsyncContext *asyncContext = new (std::nothrow) LocationMockAsyncContext(env);
    NAPI_ASSERT(env, asyncContext != nullptr, "asyncContext is null.");
    NAPI_CALL(env, napi_create_string_latin1(env,
        "SetMockedLocations", NAPI_AUTO_LENGTH, &asyncContext->resourceName));
    asyncContext->errCode = ParseLocationMockParams(env, asyncContext, argv[0]);
    if (asyncContext->errCode == INPUT_PARAMS_ERROR) {
        HandleSyncErrCode(env, ERRCODE_INVALID_PARAM);
        return UndefinedNapiValue(env);
    }
    LocationErrCode errorCode = CheckLocationSwitchState();
    if (errorCode != ERRCODE_SUCCESS) {
        HandleSyncErrCode(env, errorCode);
        return UndefinedNapiValue(env);
    }
    errorCode = g_locatorClient->SetMockedLocationsV9(asyncContext->timeInterval, asyncContext->LocationNapi);
    if (errorCode != ERRCODE_SUCCESS) {
        HandleSyncErrCode(env, errorCode);
    }
    return UndefinedNapiValue(env);
}

napi_value EnableReverseGeocodingMock(napi_env env, napi_callback_info info)
{
    LBSLOGI(LOCATOR_STANDARD, "%{public}s called.", __func__);
    NAPI_ASSERT(env, g_locatorClient != nullptr, "locator instance is null.");
    LocationErrCode errorCode = g_locatorClient->EnableReverseGeocodingMockV9();
    if (errorCode != ERRCODE_SUCCESS) {
        HandleSyncErrCode(env, errorCode);
    }
    return UndefinedNapiValue(env);
}

napi_value DisableReverseGeocodingMock(napi_env env, napi_callback_info info)
{
    LBSLOGI(LOCATOR_STANDARD, "%{public}s called.", __func__);
    NAPI_ASSERT(env, g_locatorClient != nullptr, "locator instance is null.");
    LocationErrCode errorCode = g_locatorClient->DisableReverseGeocodingMockV9();
    if (errorCode != ERRCODE_SUCCESS) {
        HandleSyncErrCode(env, errorCode);
    }
    return UndefinedNapiValue(env);
}

napi_value SetReverseGeocodingMockInfo(napi_env env, napi_callback_info info)
{
    LBSLOGI(LOCATOR_STANDARD, "%{public}s called.", __func__);
    size_t argc = 2;
    napi_value argv[argc];
    napi_value thisVar = nullptr;
    void *data = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, &data));
    NAPI_ASSERT(env, g_locatorClient != nullptr, "locator instance is null.");
    if (argc != PARAM1) {
        HandleSyncErrCode(env, ERRCODE_INVALID_PARAM);
        return UndefinedNapiValue(env);
    }

    bool isArray = false;
    NAPI_CALL(env, napi_is_array(env, argv[0], &isArray));
    if (!isArray) {
        HandleSyncErrCode(env, ERRCODE_INVALID_PARAM);
        return UndefinedNapiValue(env);
    }
    std::vector<std::shared_ptr<GeocodingMockInfo>> mockInfo;
    JsObjToRevGeocodeMock(env, argv[0], mockInfo);
    LocationErrCode errorCode = g_locatorClient->SetReverseGeocodingMockInfoV9(mockInfo);
    if (errorCode != ERRCODE_SUCCESS) {
        HandleSyncErrCode(env, errorCode);
    }
    return UndefinedNapiValue(env);
}
#endif

#ifdef ENABLE_NAPI_MANAGER
LocationErrCode CheckLocationSwitchState()
{
    bool isEnabled = false;
    LocationErrCode errorCode = g_locatorClient->IsLocationEnabledV9(isEnabled);
    if (errorCode != ERRCODE_SUCCESS) {
        return errorCode;
    }
    if (!isEnabled) {
        return ERRCODE_SWITCH_OFF;
    }
    return ERRCODE_SUCCESS;
}
#endif
} // namespace Location
} // namespace OHOS
