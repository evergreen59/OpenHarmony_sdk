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

#include "napi_x509_crl_entry.h"

#include "napi/native_node_api.h"
#include "napi/native_api.h"
#include "log.h"
#include "memory.h"
#include "utils.h"
#include "object_base.h"
#include "result.h"
#include "napi_crypto_framework_defines.h"
#include "napi_utils.h"

namespace OHOS {
namespace CryptoFramework {
thread_local napi_ref NapiX509CrlEntry::classRef_ = nullptr;

struct CfCtx {
    CfAsyncType asyncType = ASYNC_TYPE_CALLBACK;
    napi_value promise = nullptr;
    napi_ref callback = nullptr;
    napi_deferred deferred = nullptr;
    napi_async_work asyncWork = nullptr;

    NapiX509CrlEntry *crlEntryClass = nullptr;

    int32_t errCode = 0;
    const char *errMsg = nullptr;
    HcfEncodingBlob *encoded = nullptr;
    HcfBlob *blob = nullptr;
};

static void FreeCryptoFwkCtx(napi_env env, CfCtx *context)
{
    if (context == nullptr) {
        return;
    }

    if (context->asyncWork != nullptr) {
        napi_delete_async_work(env, context->asyncWork);
    }

    if (context->callback != nullptr) {
        napi_delete_reference(env, context->callback);
    }

    HcfEncodingBlobDataFree(context->encoded);
    HcfFree(context->encoded);
    context->encoded = nullptr;

    HcfBlobDataFree(context->blob);
    HcfFree(context->blob);
    context->blob = nullptr;

    HcfFree(context);
}

static void ReturnCallbackResult(napi_env env, CfCtx *context, napi_value result)
{
    napi_value businessError = nullptr;
    if (context->errCode != HCF_SUCCESS) {
        businessError = GenerateBusinessError(env, context->errCode, context->errMsg, true);
    }
    napi_value params[ARGS_SIZE_TWO] = { businessError, result };

    napi_value func = nullptr;
    napi_get_reference_value(env, context->callback, &func);

    napi_value recv = nullptr;
    napi_value callFuncRet = nullptr;
    napi_get_undefined(env, &recv);
    napi_call_function(env, recv, func, ARGS_SIZE_TWO, params, &callFuncRet);
}

static void ReturnPromiseResult(napi_env env, CfCtx *context, napi_value result)
{
    if (context->errCode == HCF_SUCCESS) {
        napi_resolve_deferred(env, context->deferred, result);
    } else {
        napi_reject_deferred(env, context->deferred,
            GenerateBusinessError(env, context->errCode, context->errMsg, true));
    }
}

static void ReturnResult(napi_env env, CfCtx *context, napi_value result)
{
    if (context->asyncType == ASYNC_TYPE_CALLBACK) {
        ReturnCallbackResult(env, context, result);
    } else {
        ReturnPromiseResult(env, context, result);
    }
}

static bool CreateCallbackAndPromise(napi_env env, CfCtx *context, size_t argc,
    size_t maxCount, napi_value callbackValue)
{
    context->asyncType = (argc == maxCount) ? ASYNC_TYPE_CALLBACK : ASYNC_TYPE_PROMISE;
    if (context->asyncType == ASYNC_TYPE_CALLBACK) {
        if (!GetCallbackFromJSParams(env, callbackValue, &context->callback, true)) {
            LOGE("get callback failed!");
            return false;
        }
    } else {
        napi_create_promise(env, &context->deferred, &context->promise);
    }
    return true;
}

NapiX509CrlEntry::NapiX509CrlEntry(HcfX509CrlEntry *x509CrlEntry)
{
    this->x509CrlEntry_ = x509CrlEntry;
}

NapiX509CrlEntry::~NapiX509CrlEntry()
{
    HcfObjDestroy(this->x509CrlEntry_);
}

static void GetEncodedExecute(napi_env env, void *data)
{
    CfCtx *context = static_cast<CfCtx *>(data);
    HcfX509CrlEntry *x509CrlEntry = context->crlEntryClass->GetX509CrlEntry();
    HcfEncodingBlob *encodingBlob = static_cast<HcfEncodingBlob *>(HcfMalloc(sizeof(HcfEncodingBlob), 0));
    if (encodingBlob == nullptr) {
        LOGE("malloc encoding blob failed!");
        context->errCode = HCF_ERR_MALLOC;
        context->errMsg = "malloc encoding blob failed";
        return;
    }

    context->errCode = x509CrlEntry->getEncoded(x509CrlEntry, encodingBlob);
    if (context->errCode != HCF_SUCCESS) {
        LOGE("get encoded failed!");
        context->errMsg = "get encoded failed";
    }
    context->encoded = encodingBlob;
}

static void GetEncodedComplete(napi_env env, napi_status status, void *data)
{
    CfCtx *context = static_cast<CfCtx *>(data);
    if (context->errCode != HCF_SUCCESS) {
        ReturnResult(env, context, nullptr);
        FreeCryptoFwkCtx(env, context);
        return;
    }
    napi_value returnEncodingBlob = ConvertEncodingBlobToNapiValue(env, context->encoded);
    ReturnResult(env, context, returnEncodingBlob);
    FreeCryptoFwkCtx(env, context);
}

napi_value NapiX509CrlEntry::GetEncoded(napi_env env, napi_callback_info info)
{
    size_t argc = ARGS_SIZE_ONE;
    napi_value argv[ARGS_SIZE_ONE] = { nullptr };
    napi_value thisVar = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (!CheckArgsCount(env, argc, ARGS_SIZE_ONE, false, true)) {
        return nullptr;
    }

    CfCtx *context = static_cast<CfCtx *>(HcfMalloc(sizeof(CfCtx), 0));
    if (context == nullptr) {
        LOGE("malloc context failed!");
        return nullptr;
    }
    context->crlEntryClass = this;

    if (!CreateCallbackAndPromise(env, context, argc, ARGS_SIZE_ONE, argv[PARAM0])) {
        FreeCryptoFwkCtx(env, context);
        return nullptr;
    }

    napi_create_async_work(
        env, nullptr, GetResourceName(env, "GetEncoded"),
        GetEncodedExecute,
        GetEncodedComplete,
        static_cast<void *>(context),
        &context->asyncWork);

    napi_queue_async_work(env, context->asyncWork);
    if (context->asyncType == ASYNC_TYPE_PROMISE) {
        return context->promise;
    } else {
        return NapiGetNull(env);
    }
}

napi_value NapiX509CrlEntry::GetSerialNumber(napi_env env, napi_callback_info info)
{
    HcfX509CrlEntry *x509CrlEntry = GetX509CrlEntry();
    long serialNumber = x509CrlEntry->getSerialNumber(x509CrlEntry);
    napi_value result = nullptr;
    napi_create_int64(env, serialNumber, &result);
    return result;
}

napi_value NapiX509CrlEntry::GetCertificateIssuer(napi_env env, napi_callback_info info)
{
    HcfBlob *blob = reinterpret_cast<HcfBlob *>(HcfMalloc(sizeof(HcfBlob), 0));
    if (blob == nullptr) {
        LOGE("malloc blob failed!");
        return nullptr;
    }

    HcfX509CrlEntry *x509CrlEntry = GetX509CrlEntry();
    HcfResult ret = x509CrlEntry->getCertIssuer(x509CrlEntry, blob);
    if (ret != HCF_SUCCESS) {
        napi_throw(env, GenerateBusinessError(env, ret, "get subject name failed", true));
        LOGE("get cert issuer failed!");
        HcfFree(blob);
        blob = nullptr;
        return nullptr;
    }
    napi_value returnValue = ConvertBlobToNapiValue(env, blob);
    HcfBlobDataFree(blob);
    HcfFree(blob);
    blob = nullptr;
    return returnValue;
}

napi_value NapiX509CrlEntry::GetRevocationDate(napi_env env, napi_callback_info info)
{
    HcfX509CrlEntry *x509CrlEntry = GetX509CrlEntry();
    HcfBlob *blob = reinterpret_cast<HcfBlob *>(HcfMalloc(sizeof(HcfBlob), 0));
    if (blob == nullptr) {
        LOGE("malloc blob failed!");
        return nullptr;
    }
    HcfResult ret = x509CrlEntry->getRevocationDate(x509CrlEntry, blob);
    if (ret != HCF_SUCCESS) {
        napi_throw(env, GenerateBusinessError(env, ret, "get revocation date failed", true));
        LOGE("get revocation date failed!");
        HcfFree(blob);
        blob = nullptr;
        return nullptr;
    }
    napi_value returnDate = nullptr;
    napi_create_string_utf8(env, reinterpret_cast<char *>(blob->data), blob->len, &returnDate);
    HcfBlobDataFree(blob);
    HcfFree(blob);
    blob = nullptr;
    return returnDate;
}

static napi_value NapiGetEncoded(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);
    NapiX509CrlEntry *x509CrlEntry = nullptr;
    napi_unwrap(env, thisVar, reinterpret_cast<void **>(&x509CrlEntry));
    if (x509CrlEntry == nullptr) {
        LOGE("x509CrlEntry is nullptr!");
        return nullptr;
    }
    return x509CrlEntry->GetEncoded(env, info);
}

static napi_value NapiGetSerialNumber(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);
    NapiX509CrlEntry *x509CrlEntry = nullptr;
    napi_unwrap(env, thisVar, reinterpret_cast<void **>(&x509CrlEntry));
    if (x509CrlEntry == nullptr) {
        LOGE("x509CrlEntry is nullptr!");
        return nullptr;
    }
    return x509CrlEntry->GetSerialNumber(env, info);
}

static napi_value NapiGetCertificateIssuer(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);
    NapiX509CrlEntry *x509CrlEntry = nullptr;
    napi_unwrap(env, thisVar, reinterpret_cast<void **>(&x509CrlEntry));
    if (x509CrlEntry == nullptr) {
        LOGE("x509CrlEntry is nullptr!");
        return nullptr;
    }
    return x509CrlEntry->GetCertificateIssuer(env, info);
}

static napi_value NapiGetRevocationDate(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);
    NapiX509CrlEntry *x509CrlEntry = nullptr;
    napi_unwrap(env, thisVar, reinterpret_cast<void **>(&x509CrlEntry));
    if (x509CrlEntry == nullptr) {
        LOGE("x509CrlEntry is nullptr!");
        return nullptr;
    }
    return x509CrlEntry->GetRevocationDate(env, info);
}

static napi_value X509CrlEntryConstructor(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);
    return thisVar;
}

void NapiX509CrlEntry::DefineX509CrlEntryJSClass(napi_env env)
{
    napi_property_descriptor x509CrlEntryDesc[] = {
        DECLARE_NAPI_FUNCTION("getEncoded", NapiGetEncoded),
        DECLARE_NAPI_FUNCTION("getSerialNumber", NapiGetSerialNumber),
        DECLARE_NAPI_FUNCTION("getCertIssuer", NapiGetCertificateIssuer),
        DECLARE_NAPI_FUNCTION("getRevocationDate", NapiGetRevocationDate),
    };
    napi_value constructor = nullptr;
    napi_define_class(env, "X509CrlEntry", NAPI_AUTO_LENGTH, X509CrlEntryConstructor, nullptr,
        sizeof(x509CrlEntryDesc) / sizeof(x509CrlEntryDesc[0]), x509CrlEntryDesc, &constructor);
    napi_create_reference(env, constructor, 1, &classRef_);
}

napi_value NapiX509CrlEntry::CreateX509CrlEntry(napi_env env)
{
    napi_value constructor = nullptr;
    napi_value instance = nullptr;
    napi_get_reference_value(env, classRef_, &constructor);
    napi_new_instance(env, constructor, 0, nullptr, &instance);
    return instance;
}
} // namespace CryptoFramework
} // namespace OHOS
