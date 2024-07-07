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

#ifndef NAPI_UTIL_H
#define NAPI_UTIL_H

#include <list>
#include <string>

#include "message_parcel.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "uv.h"

#include "location_async_context.h"
#include "async_context.h"
#include "constant_definition.h"
#include "geo_coding_mock_info.h"
#include "geo_address.h"
#include "location.h"
#include "location_log.h"
#include "request_config.h"
#include "satellite_status.h"

namespace OHOS {
namespace Location {
napi_value UndefinedNapiValue(const napi_env& env);
void LocationToJs(const napi_env& env, const std::unique_ptr<Location>& locationInfo, napi_value& result);
void LocationsToJs(const napi_env& env, const std::vector<std::shared_ptr<Location>>& locations, napi_value& result);
void SatelliteStatusToJs(const napi_env& env, const std::shared_ptr<SatelliteStatus>& statusInfo, napi_value& result);
void SystemLocationToJs(const napi_env& env, const std::unique_ptr<Location>& locationInfo, napi_value& result);
bool GeoAddressesToJsObj(const napi_env& env,
    std::list<std::shared_ptr<GeoAddress>>& replyList, napi_value& arrayResult);
void JsObjToLocationRequest(const napi_env& env, const napi_value& object,
    std::unique_ptr<RequestConfig>& requestConfig);
void JsObjToCurrentLocationRequest(const napi_env& env, const napi_value& object,
    std::unique_ptr<RequestConfig>& requestConfig);
void JsObjToCachedLocationRequest(const napi_env& env, const napi_value& object,
    std::unique_ptr<CachedGnssLocationsRequest>& request);
int JsObjToCommand(const napi_env& env, const napi_value& object,
    std::unique_ptr<LocationCommand>& commandConfig);
void JsObjToGeoFenceRequest(const napi_env& env, const napi_value& object,
    const std::unique_ptr<GeofenceRequest>& request);
int JsObjToGeoCodeRequest(const napi_env& env, const napi_value& object, MessageParcel& dataParcel);
bool JsObjToReverseGeoCodeRequest(const napi_env& env, const napi_value& object, MessageParcel& dataParcel);
int JsObjectToString(const napi_env& env, const napi_value& object,
    const char* fieldStr, const int bufLen, std::string& fieldRef);
int JsObjectToDouble(const napi_env& env, const napi_value& object, const char* fieldStr, double& fieldRef);
int JsObjectToInt(const napi_env& env, const napi_value& object, const char* fieldStr, int& fieldRef);
int JsObjectToInt64(const napi_env& env, const napi_value& object, const char* fieldStr, int64_t& fieldRef);
int JsObjectToBool(const napi_env& env, const napi_value& object, const char* fieldStr, bool& fieldRef);
napi_status SetValueUtf8String(const napi_env& env, const char* fieldStr, const char* str, napi_value& result);
napi_status SetValueStringArray(const napi_env& env, const char* fieldStr, napi_value& value, napi_value& result);
napi_status SetValueInt32(const napi_env& env, const char* fieldStr, const int intValue, napi_value& result);
napi_status SetValueInt64(const napi_env& env, const char* fieldStr, const int64_t intValue, napi_value& result);
napi_status SetValueDouble(const napi_env& env, const char* fieldStr, const double doubleValue, napi_value& result);
napi_status SetValueBool(const napi_env& env, const char* fieldStr, const bool boolvalue, napi_value& result);
napi_value DoAsyncWork(const napi_env& env, AsyncContext* asyncContext,
    const size_t argc, const napi_value* argv, const size_t objectArgsNum);
bool JsObjToRevGeocodeMock(const napi_env& env, const napi_value& object,
    std::vector<std::shared_ptr<GeocodingMockInfo>>& mockInfo);
std::string GetErrorMsgByCode(int code);
void CreateFailCallBackParams(AsyncContext& context, const std::string& msg, int32_t errorCode);
void CountryCodeToJs(const napi_env& env, const std::shared_ptr<CountryCode>& country, napi_value& result);
void GetLocationArray(const napi_env& env, LocationMockAsyncContext *asyncContext, const napi_value& object);
void DeleteQueueWork(AsyncContext* context);
void DeleteCallbackHandler(uv_loop_s *&loop, uv_work_t *&work);
napi_value GetErrorObject(napi_env env, const int32_t errCode, const std::string& errMsg);
bool CheckIfParamIsFunctionType(napi_env env, napi_value param);
napi_value SetEnumPropertyByInteger(napi_env env, napi_value dstObj, int32_t enumValue, const char *enumName);

#define CHK_NAPIOK_CONTINUE(env, state, message) \
{ \
    if ((state) != napi_ok) { \
        LBSLOGE("(%{public}s) fail", #message); \
        GET_AND_THROW_LAST_ERROR((env)); \
        continue; \
    } \
}

#define CHK_ERROR_CODE(type, theCall, isRequired)                                                     \
    do {                                                                                              \
        int errorCode = (theCall);                                                                    \
        if (errorCode == COMMON_ERROR || errorCode == INPUT_PARAMS_ERROR) {                           \
            LBSLOGE(LOCATOR_STANDARD, "Js Object to other types failed.");                            \
            return errorCode;                                                                         \
        }                                                                                             \
        if ((isRequired) && errorCode == PARAM_IS_EMPTY) {                                            \
            LBSLOGE(LOCATOR_STANDARD, "The required #%{public}s field should not be empty.", (type)); \
            return INPUT_PARAMS_ERROR;                                                                \
        }                                                                                             \
    } while (0)

#define CHK_NAPI_ERR_CLOSE_SCOPE(env, state, scope, context, work)             \
    do {                                                                       \
        if ((state) != napi_ok && (context) != nullptr && (work) != nullptr) { \
            napi_close_handle_scope((env), (scope));                           \
            delete (context);                                                  \
            delete (work);                                                     \
            return;                                                            \
        }                                                                      \
    } while (0)
}  // namespace Location
}  // namespace OHOS
#endif // NAPI_UTIL_H
