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

#include "user_auth_impl.h"

#include <map>

#include "securec.h"

#include "iam_logger.h"
#include "iam_para2str.h"
#include "iam_ptr.h"

#include "user_auth_napi_helper.h"
#include "user_auth_callback_v6.h"
#include "user_auth_callback_v8.h"
#include "user_auth_client_impl.h"

#define LOG_LABEL UserIam::Common::LABEL_USER_AUTH_NAPI

namespace OHOS {
namespace UserIam {
namespace UserAuth {
napi_value UserAuthImpl::GetVersion(napi_env env, napi_callback_info info)
{
    int32_t version;
    int32_t result = UserAuthClientImpl::Instance().GetVersion(version);
    if (result != SUCCESS) {
        IAM_LOGE("result = %{public}d", result);
        version = 0;
    }
    IAM_LOGI("version = %{public}d", version);
    napi_value jsVersion;
    NAPI_CALL(env, napi_create_int32(env, version, &jsVersion));
    return jsVersion;
}

napi_value UserAuthImpl::GetAvailableStatus(napi_env env, napi_callback_info info)
{
    napi_value argv[ARGS_TWO] = {nullptr};
    size_t argc = ARGS_TWO;
    napi_value result;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));
    if (argc != ARGS_TWO) {
        IAM_LOGE("parms error");
        NAPI_CALL(env, napi_create_int32(env, INVALID_PARAMETERS, &result));
        return result;
    }
    int32_t type;
    NAPI_CALL(env, napi_get_value_int32(env, argv[PARAM0], &type));
    int32_t level;
    NAPI_CALL(env, napi_get_value_int32(env, argv[PARAM1], &level));
    AuthType authType = AuthType(type);
    AuthTrustLevel authTrustLevel = AuthTrustLevel(level);
    ResultCode checkRet = CheckAuthTypeAndAuthTrustLevel(authType, authTrustLevel);
    if (checkRet != SUCCESS) {
        IAM_LOGE("CheckAuthTypeAndAuthTrsutLevel failed");
        NAPI_CALL(env, napi_create_int32(env, checkRet, &result));
        return result;
    }
    int32_t status = UserAuthClientImpl::Instance().GetAvailableStatus(API_VERSION_8, authType, authTrustLevel);
    IAM_LOGI("result = %{public}d", status);
    NAPI_CALL(env, napi_create_int32(env, UserAuthNapiHelper::GetResultCodeV8(status), &result));
    return result;
}

napi_value UserAuthImpl::Execute(napi_env env, napi_callback_info info)
{
    IAM_LOGI("start");
    size_t argc = ARGS_THREE;
    napi_value argv[ARGS_THREE] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));
    napi_value retPromise = nullptr;
    std::shared_ptr<JsRefHolder> callbackRef = nullptr;
    napi_deferred promiseDeferred = nullptr;
    if (argc == ARGS_THREE) {
        callbackRef = Common::MakeShared<JsRefHolder>(env, argv[PARAM2]);
        if (callbackRef == nullptr || !callbackRef->IsValid()) {
            IAM_LOGE("make callback ref fail");
            return nullptr;
        }
        NAPI_CALL(env, napi_get_null(env, &retPromise));
    } else if (argc == ARGS_TWO) {
        NAPI_CALL(env, napi_create_promise(env, &promiseDeferred, &retPromise));
    } else {
        IAM_LOGE("bad params");
        return retPromise;
    }
    std::shared_ptr<UserAuthCallbackV6> callback =
        Common::MakeShared<UserAuthCallbackV6>(env, callbackRef, promiseDeferred);
    if (callback == nullptr) {
        IAM_LOGE("callback is nullptr");
        return nullptr;
    }

    AuthType authType;
    ResultCode resultCode;
    NAPI_CALL(env, ParseExecuteAuthType(env, argv[PARAM0], authType, resultCode));
    if (resultCode != ResultCode::SUCCESS) {
        IAM_LOGE("ParseAuthType fail");
        UserIam::UserAuth::Attributes extra;
        callback->OnResult(resultCode, extra);
        return retPromise;
    }
    AuthTrustLevel authTrustLevel;
    NAPI_CALL(env, ParseExecuteSecureLevel(env, argv[PARAM1], authTrustLevel, resultCode));
    if (resultCode != ResultCode::SUCCESS) {
        IAM_LOGE("ParseExecuteSecureLevel fail");
        UserIam::UserAuth::Attributes extra;
        callback->OnResult(resultCode, extra);
        return retPromise;
    }

    std::vector<uint8_t> challenge;
    UserAuthClientImpl::Instance().BeginNorthAuthentication(API_VERSION_6, challenge, FACE, authTrustLevel, callback);
    return retPromise;
}

napi_status UserAuthImpl::ParseExecuteAuthType(napi_env env, napi_value value,
    AuthType &authType, ResultCode &resultCode)
{
    resultCode = ResultCode::GENERAL_ERROR;
    static const size_t maxLen = 20;
    char str[maxLen] = {0};
    size_t len = maxLen;
    napi_status result = UserAuthNapiHelper::GetStrValue(env, value, str, len);
    if (result != napi_ok) {
        IAM_LOGE("getStrValue fail");
        return result;
    }
    static const char *authTypeAll = "ALL";
    static const char *authTypeFaceOnly = "FACE_ONLY";
    if (strcmp(str, authTypeAll) == 0) {
        IAM_LOGE("type ALL not supported");
        resultCode = ResultCode::TYPE_NOT_SUPPORT;
        return napi_ok;
    }
    if (strcmp(str, authTypeFaceOnly) != 0) {
        IAM_LOGE("type is invalid");
        resultCode = ResultCode::INVALID_PARAMETERS;
        return napi_ok;
    }
    resultCode = ResultCode::SUCCESS;
    return napi_ok;
}

napi_status UserAuthImpl::ParseExecuteSecureLevel(napi_env env, napi_value value,
    AuthTrustLevel &authTrustLevel, ResultCode &resultCode)
{
    resultCode = ResultCode::GENERAL_ERROR;
    static const size_t maxLen = 20;
    char str[maxLen] = {0};
    size_t len = maxLen;
    napi_status result = UserAuthNapiHelper::GetStrValue(env, value, str, len);
    if (result != napi_ok) {
        IAM_LOGE("getStrValue fail");
        return result;
    }
    static std::map<std::string, AuthTrustLevel> convertAuthTrustLevel = {
        {"S1", ATL1},
        {"S2", ATL2},
        {"S3", ATL3},
        {"S4", ATL4},
    };
    if (convertAuthTrustLevel.count(str) == 0) {
        IAM_LOGE("trust level invalid");
        resultCode = ResultCode::INVALID_PARAMETERS;
        return napi_ok;
    }
    authTrustLevel = convertAuthTrustLevel[str];
    resultCode = ResultCode::SUCCESS;
    return napi_ok;
}

napi_value UserAuthImpl::Auth(napi_env env, napi_callback_info info)
{
    IAM_LOGI("start");
    size_t argc = ARGS_FOUR;
    napi_value argv[ARGS_FOUR] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));
    if (argc != ARGS_FOUR) {
        IAM_LOGE("parms error");
        return nullptr;
    }
    std::vector<uint8_t> challenge;
    if (UserAuthNapiHelper::GetUint8ArrayValue(env, argv[PARAM0], MAX_CHALLENG_LEN, challenge) != napi_ok) {
        IAM_LOGE("challenge invalid, use null challenge");
        challenge.clear();
    }
    int32_t authType;
    NAPI_CALL(env, UserAuthNapiHelper::GetInt32Value(env, argv[PARAM1], authType));
    int32_t authTrustLevel;
    NAPI_CALL(env, UserAuthNapiHelper::GetInt32Value(env, argv[PARAM2], authTrustLevel));
    NAPI_CALL(env, UserAuthNapiHelper::CheckNapiType(env, argv[PARAM3], napi_object));
    napi_value onResultValue;
    NAPI_CALL(env, napi_get_named_property(env, argv[PARAM3], "onResult", &onResultValue));
    auto resultCallback = Common::MakeShared<JsRefHolder>(env, onResultValue);
    napi_value onAcquireInfoValue;
    NAPI_CALL(env, napi_get_named_property(env, argv[PARAM3], "onAcquireInfo", &onAcquireInfoValue));
    auto acquireCallback = Common::MakeShared<JsRefHolder>(env, onAcquireInfoValue);
    auto callback = Common::MakeShared<UserAuthCallbackV8>(env, resultCallback, acquireCallback);
    if (callback == nullptr) {
        IAM_LOGE("callback is nullptr");
        return nullptr;
    }
    ResultCode checkRet = CheckAuthTypeAndAuthTrustLevel(AuthType(authType), AuthTrustLevel(authTrustLevel));
    if (checkRet != SUCCESS) {
        IAM_LOGE("CheckAuthTypeAndAuthTrsutLevel failed");
        Attributes extraInfo;
        callback->OnResult(checkRet, extraInfo);
        napi_value key = UserAuthNapiHelper::Uint64ToNapiUint8Array(env, INVALID_CONTEXT_ID);
        return key;
    }
    uint64_t result = UserAuthClientImpl::Instance().BeginNorthAuthentication(API_VERSION_8, challenge,
        AuthType(authType), AuthTrustLevel(authTrustLevel), callback);
    IAM_LOGI("result is %{public}s", GET_MASKED_STRING(result).c_str());
    napi_value key = UserAuthNapiHelper::Uint64ToNapiUint8Array(env, result);
    IAM_LOGI("end");
    return key;
}

napi_value UserAuthImpl::CancelAuth(napi_env env, napi_callback_info info)
{
    size_t argc = ARGS_ONE;
    napi_value argv[ARGS_ONE] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));
    if (argc != ARGS_ONE) {
        IAM_LOGE("parms error");
        return nullptr;
    }
    const size_t maxContextIdLen = 8;
    std::vector<uint8_t> contextIdArray;
    NAPI_CALL(env, UserAuthNapiHelper::GetUint8ArrayValue(env, argv[PARAM0], maxContextIdLen, contextIdArray));
    uint64_t contextId;
    if (memcpy_s(reinterpret_cast<void *>(&contextId), sizeof(contextId),
        contextIdArray.data(), contextIdArray.size()) != EOK) {
        IAM_LOGE("memcpy error");
        return nullptr;
    }
    IAM_LOGI("contextId's low 16 bits is %{public}s", GET_MASKED_STRING(contextId).c_str());
    if (contextId == 0) {
        IAM_LOGE("invalid error");
        return nullptr;
    }
    int32_t result = UserAuthClient::GetInstance().CancelAuthentication(contextId);
    IAM_LOGI("result = %{public}d", result);
    napi_value key;
    NAPI_CALL(env, napi_create_int32(env, UserAuthNapiHelper::GetResultCodeV8(result), &key));
    return key;
}

ResultCode UserAuthImpl::CheckAuthTypeAndAuthTrustLevel(AuthType authType, AuthTrustLevel authTrustLevel)
{
    if (authType != FINGERPRINT && authType != FACE) {
        IAM_LOGE("authType check fail:%{public}d", authType);
        return TYPE_NOT_SUPPORT;
    }
    if (authTrustLevel != ATL1 && authTrustLevel != ATL2 && authTrustLevel != ATL3 && authTrustLevel != ATL4) {
        IAM_LOGE("authTrustLevel check fail:%{public}d", authTrustLevel);
        return TRUST_LEVEL_NOT_SUPPORT;
    }
    return SUCCESS;
}
} // namespace UserAuth
} // namespace UserIam
} // namespace OHOS
