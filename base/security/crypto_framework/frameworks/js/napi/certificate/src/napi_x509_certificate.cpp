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

#include "napi_x509_certificate.h"

#include "napi/native_node_api.h"
#include "napi/native_api.h"
#include "log.h"
#include "memory.h"
#include "utils.h"
#include "object_base.h"
#include "result.h"
#include "napi_crypto_framework_defines.h"
#include "napi_pub_key.h"
#include "napi_utils.h"

namespace OHOS {
namespace CryptoFramework {
thread_local napi_ref NapiX509Certificate::classRef_ = nullptr;

struct CfCtx {
    CfAsyncType asyncType = ASYNC_TYPE_CALLBACK;
    napi_value promise = nullptr;
    napi_ref callback = nullptr;
    napi_deferred deferred = nullptr;
    napi_async_work asyncWork = nullptr;

    HcfEncodingBlob *encodingBlob = nullptr;
    NapiX509Certificate *certClass = nullptr;
    HcfPubKey *pubKey = nullptr;

    int32_t errCode = 0;
    const char *errMsg = nullptr;
    HcfX509Certificate *cert;
    HcfEncodingBlob *encoded = nullptr;
};

NapiX509Certificate::NapiX509Certificate(HcfX509Certificate *x509Cert)
{
    this->x509Cert_ = x509Cert;
}

NapiX509Certificate::~NapiX509Certificate()
{
    HcfObjDestroy(this->x509Cert_);
}

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

    HcfEncodingBlobDataFree(context->encodingBlob);
    HcfFree(context->encodingBlob);
    context->encodingBlob = nullptr;

    HcfEncodingBlobDataFree(context->encoded);
    HcfFree(context->encoded);
    context->encoded = nullptr;

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

static void VerifyExecute(napi_env env, void *data)
{
    LOGI("start to verify.");
    CfCtx *context = static_cast<CfCtx *>(data);
    HcfX509Certificate *cert = context->certClass->GetX509Cert();
    context->errCode = cert->base.verify(&(cert->base), context->pubKey);
    if (context->errCode != HCF_SUCCESS) {
        LOGE("verify cert failed!");
        context->errMsg = "verify cert failed";
    }
}

static void VerifyComplete(napi_env env, napi_status status, void *data)
{
    CfCtx *context = static_cast<CfCtx *>(data);
    ReturnResult(env, context, NapiGetNull(env));
    FreeCryptoFwkCtx(env, context);
}

static void GetEncodedExecute(napi_env env, void *data)
{
    CfCtx *context = static_cast<CfCtx *>(data);
    HcfX509Certificate *cert = context->certClass->GetX509Cert();
    HcfEncodingBlob *encodingBlob = static_cast<HcfEncodingBlob *>(HcfMalloc(sizeof(HcfEncodingBlob), 0));
    if (encodingBlob == nullptr) {
        LOGE("malloc encoding blob failed!");
        context->errCode = HCF_ERR_MALLOC;
        context->errMsg = "malloc encoding blob failed";
        return;
    }
    context->errCode = cert->base.getEncoded(&(cert->base), encodingBlob);
    if (context->errCode != HCF_SUCCESS) {
        LOGE("get cert encoded failed!");
        context->errMsg = "get cert encoded failed";
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

napi_value NapiX509Certificate::Verify(napi_env env, napi_callback_info info)
{
    size_t argc = ARGS_SIZE_TWO;
    napi_value argv[ARGS_SIZE_TWO] = { nullptr };
    napi_value thisVar = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (!CheckArgsCount(env, argc, ARGS_SIZE_TWO, false, true)) {
        return nullptr;
    }

    CfCtx *context = static_cast<CfCtx *>(HcfMalloc(sizeof(CfCtx), 0));
    if (context == nullptr) {
        LOGE("malloc context failed!");
        return nullptr;
    }
    context->certClass = this;

    NapiPubKey *pubKey = nullptr;
    napi_unwrap(env, argv[PARAM0], (void**)&pubKey);
    if (pubKey == nullptr) {
        napi_throw(env, GenerateBusinessError(env, HCF_INVALID_PARAMS, "public key is null", true));
        LOGE("pubKey is null!");
        FreeCryptoFwkCtx(env, context);
        return nullptr;
    }
    context->pubKey = pubKey->GetPubKey();

    if (!CreateCallbackAndPromise(env, context, argc, ARGS_SIZE_TWO, argv[PARAM1])) {
        FreeCryptoFwkCtx(env, context);
        return nullptr;
    }

    napi_create_async_work(
        env, nullptr, GetResourceName(env, "Verify"),
        VerifyExecute,
        VerifyComplete,
        static_cast<void *>(context),
        &context->asyncWork);

    napi_queue_async_work(env, context->asyncWork);
    if (context->asyncType == ASYNC_TYPE_PROMISE) {
        return context->promise;
    } else {
        return NapiGetNull(env);
    }
}

napi_value NapiX509Certificate::GetEncoded(napi_env env, napi_callback_info info)
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
    context->certClass = this;

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

napi_value NapiX509Certificate::GetPublicKey(napi_env env, napi_callback_info info)
{
    HcfX509Certificate *cert = GetX509Cert();
    HcfPubKey *returnPubKey = nullptr;
    HcfResult ret = cert->base.getPublicKey(&(cert->base), &returnPubKey);
    if (ret != HCF_SUCCESS) {
        napi_throw(env, GenerateBusinessError(env, ret, "get cert public key failed!", true));
        LOGE("get cert public key failed!");
        return nullptr;
    }

    NapiPubKey *pubKeyClass = new (std::nothrow) NapiPubKey(returnPubKey);
    if (pubKeyClass == nullptr) {
        LOGE("create for x509 cert's public key obj failed");
        HcfObjDestroy(returnPubKey);
        return nullptr;
    }
    napi_value instance = pubKeyClass->ConvertToJsPubKey(env);
    napi_wrap(
        env, instance, pubKeyClass,
        [](napi_env env, void *data, void *hint) {
            NapiPubKey *pubKeyClass = static_cast<NapiPubKey *>(data);
            HcfObjDestroy(pubKeyClass->GetPubKey());
            delete pubKeyClass;
            return;
        },
        nullptr, nullptr);
    return instance;
}

napi_value NapiX509Certificate::CheckValidityWithDate(napi_env env, napi_callback_info info)
{
    size_t argc = ARGS_SIZE_ONE;
    napi_value argv[ARGS_SIZE_ONE] = { nullptr };
    napi_value thisVar = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (!CheckArgsCount(env, argc, ARGS_SIZE_ONE, true, true)) {
        return nullptr;
    }
    std::string date;
    if (!GetStringFromJSParams(env, argv[PARAM0], date, true)) {
        LOGE("get date param failed!");
        return nullptr;
    }
    HcfX509Certificate *cert = GetX509Cert();
    HcfResult ret = cert->checkValidityWithDate(cert, date.c_str());
    if (ret != HCF_SUCCESS) {
        napi_throw(env, GenerateBusinessError(env, ret, "check cert validity failed!", true));
        LOGE("check cert validity failed!");
    }
    return nullptr;
}

napi_value NapiX509Certificate::GetVersion(napi_env env, napi_callback_info info)
{
    HcfX509Certificate *cert = GetX509Cert();
    int version = cert->getVersion(cert);
    napi_value result = nullptr;
    napi_create_int32(env, version, &result);
    return result;
}


napi_value NapiX509Certificate::GetSerialNumber(napi_env env, napi_callback_info info)
{
    HcfX509Certificate *cert = GetX509Cert();
    long serialNumber = cert->getSerialNumber(cert);
    napi_value result = nullptr;
    napi_create_int64(env, serialNumber, &result);
    return result;
}

napi_value NapiX509Certificate::GetIssuerName(napi_env env, napi_callback_info info)
{
    HcfBlob *blob = reinterpret_cast<HcfBlob *>(HcfMalloc(sizeof(HcfBlob), 0));
    if (blob == nullptr) {
        LOGE("malloc blob failed!");
        return nullptr;
    }
    HcfX509Certificate *cert = GetX509Cert();
    HcfResult ret = cert->getIssuerName(cert, blob);
    if (ret != HCF_SUCCESS) {
        napi_throw(env, GenerateBusinessError(env, ret, "get issuer name failed", true));
        LOGE("getIssuerName failed!");
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

napi_value NapiX509Certificate::GetSubjectName(napi_env env, napi_callback_info info)
{
    HcfBlob *blob = reinterpret_cast<HcfBlob *>(HcfMalloc(sizeof(HcfBlob), 0));
    if (blob == nullptr) {
        LOGE("malloc blob failed!");
        return nullptr;
    }
    HcfX509Certificate *cert = GetX509Cert();
    HcfResult ret = cert->getSubjectName(cert, blob);
    if (ret != HCF_SUCCESS) {
        napi_throw(env, GenerateBusinessError(env, ret, "get subject name failed", true));
        LOGE("getSubjectName failed!");
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

napi_value NapiX509Certificate::GetNotBeforeTime(napi_env env, napi_callback_info info)
{
    HcfBlob *blob = reinterpret_cast<HcfBlob *>(HcfMalloc(sizeof(HcfBlob), 0));
    if (blob == nullptr) {
        LOGE("malloc blob failed!");
        return nullptr;
    }
    HcfX509Certificate *cert = GetX509Cert();
    HcfResult res = cert->getNotBeforeTime(cert, blob);
    if (res != HCF_SUCCESS) {
        napi_throw(env, GenerateBusinessError(env, res, "get not before time failed", true));
        LOGE("getNotBeforeTime failed!");
        HcfFree(blob);
        blob = nullptr;
        return nullptr;
    }
    napi_value result = nullptr;
    napi_create_string_utf8(env, reinterpret_cast<char *>(blob->data), blob->len, &result);
    HcfBlobDataFree(blob);
    HcfFree(blob);
    blob = nullptr;
    return result;
}

napi_value NapiX509Certificate::GetNotAfterTime(napi_env env, napi_callback_info info)
{
    HcfBlob *blob = reinterpret_cast<HcfBlob *>(HcfMalloc(sizeof(HcfBlob), 0));
    if (blob == nullptr) {
        LOGE("malloc blob failed!");
        return nullptr;
    }
    HcfX509Certificate *cert = GetX509Cert();
    HcfResult res = cert->getNotAfterTime(cert, blob);
    if (res != HCF_SUCCESS) {
        napi_throw(env, GenerateBusinessError(env, res, "get not after time failed", true));
        LOGE("getNotAfterTime failed!");
        HcfFree(blob);
        blob = nullptr;
        return nullptr;
    }
    napi_value result = nullptr;
    napi_create_string_utf8(env, reinterpret_cast<char *>(blob->data), blob->len, &result);
    HcfBlobDataFree(blob);
    HcfFree(blob);
    blob = nullptr;
    return result;
}

napi_value NapiX509Certificate::GetSignature(napi_env env, napi_callback_info info)
{
    HcfBlob *blob = reinterpret_cast<HcfBlob *>(HcfMalloc(sizeof(HcfBlob), 0));
    if (blob == nullptr) {
        LOGE("malloc blob failed!");
        return nullptr;
    }
    HcfX509Certificate *cert = GetX509Cert();
    HcfResult ret = cert->getSignature(cert, blob);
    if (ret != HCF_SUCCESS) {
        napi_throw(env, GenerateBusinessError(env, ret, "get signature failed", true));
        LOGE("getSignature failed!");
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

napi_value NapiX509Certificate::GetSigAlgName(napi_env env, napi_callback_info info)
{
    HcfBlob *blob = reinterpret_cast<HcfBlob *>(HcfMalloc(sizeof(HcfBlob), 0));
    if (blob == nullptr) {
        LOGE("malloc blob failed!");
        return nullptr;
    }
    HcfX509Certificate *cert = GetX509Cert();
    HcfResult res = cert->getSignatureAlgName(cert, blob);
    if (res != HCF_SUCCESS) {
        napi_throw(env, GenerateBusinessError(env, res, "get signature alg name failed", true));
        LOGE("getSignatureAlgName failed!");
        HcfFree(blob);
        blob = nullptr;
        return nullptr;
    }
    napi_value result = nullptr;
    napi_create_string_utf8(env, reinterpret_cast<char *>(blob->data), blob->len, &result);
    HcfBlobDataFree(blob);
    HcfFree(blob);
    blob = nullptr;
    return result;
}

napi_value NapiX509Certificate::GetSigAlgOID(napi_env env, napi_callback_info info)
{
    HcfBlob *blob = reinterpret_cast<HcfBlob *>(HcfMalloc(sizeof(HcfBlob), 0));
    if (blob == nullptr) {
        LOGE("malloc blob failed!");
        return nullptr;
    }
    HcfX509Certificate *cert = GetX509Cert();
    HcfResult res = cert->getSignatureAlgOid(cert, blob);
    if (res != HCF_SUCCESS) {
        napi_throw(env, GenerateBusinessError(env, res, "get signature alg oid failed", true));
        LOGE("getSignatureAlgOid failed!");
        HcfFree(blob);
        blob = nullptr;
        return nullptr;
    }
    napi_value result = nullptr;
    napi_create_string_utf8(env, reinterpret_cast<char *>(blob->data), blob->len, &result);
    HcfBlobDataFree(blob);
    HcfFree(blob);
    blob = nullptr;
    return result;
}

napi_value NapiX509Certificate::GetSigAlgParams(napi_env env, napi_callback_info info)
{
    HcfBlob *blob = reinterpret_cast<HcfBlob *>(HcfMalloc(sizeof(HcfBlob), 0));
    if (blob == nullptr) {
        LOGE("malloc blob failed!");
        return nullptr;
    }
    HcfX509Certificate *cert = GetX509Cert();
    HcfResult ret = cert->getSignatureAlgParams(cert, blob);
    if (ret != HCF_SUCCESS) {
        napi_throw(env, GenerateBusinessError(env, ret, "get signature alg params failed", true));
        LOGE("getSignatureAlgParams failed!");
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

napi_value NapiX509Certificate::GetKeyUsage(napi_env env, napi_callback_info info)
{
    HcfBlob *blob = reinterpret_cast<HcfBlob *>(HcfMalloc(sizeof(HcfBlob), 0));
    if (blob == nullptr) {
        LOGE("malloc blob failed!");
        return nullptr;
    }
    HcfX509Certificate *cert = GetX509Cert();
    HcfResult ret = cert->getKeyUsage(cert, blob);
    if (ret != HCF_SUCCESS) {
        napi_throw(env, GenerateBusinessError(env, ret, "get key usage failed", true));
        LOGE("getKeyUsage failed!");
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

napi_value NapiX509Certificate::GetExtendedKeyUsage(napi_env env, napi_callback_info info)
{
    HcfArray *array = reinterpret_cast<HcfArray *>(HcfMalloc(sizeof(HcfArray), 0));
    if (array == nullptr) {
        LOGE("malloc array failed!");
        return nullptr;
    }
    HcfX509Certificate *cert = GetX509Cert();
    HcfResult ret = cert->getExtKeyUsage(cert, array);
    if (ret != HCF_SUCCESS) {
        napi_throw(env, GenerateBusinessError(env, ret, "get ext key usage failed", true));
        LOGE("call getExtKeyUsage failed!");
        HcfFree(array);
        array = nullptr;
        return nullptr;
    }
    napi_value returnValue = ConvertArrayToNapiValue(env, array);
    HcfArrayDataClearAndFree(array);
    HcfFree(array);
    array = nullptr;
    return returnValue;
}


napi_value NapiX509Certificate::GetBasicConstraints(napi_env env, napi_callback_info info)
{
    HcfX509Certificate *cert = GetX509Cert();
    int32_t constrains = cert->getBasicConstraints(cert);
    napi_value result = nullptr;
    napi_create_int32(env, constrains, &result);
    return result;
}

napi_value NapiX509Certificate::GetSubjectAlternativeNames(napi_env env, napi_callback_info info)
{
    HcfArray *array = reinterpret_cast<HcfArray *>(HcfMalloc(sizeof(HcfArray), 0));
    if (array == nullptr) {
        LOGE("malloc array failed!");
        return nullptr;
    }
    HcfX509Certificate *cert = GetX509Cert();
    HcfResult ret = cert->getSubjectAltNames(cert, array);
    if (ret != HCF_SUCCESS) {
        napi_throw(env, GenerateBusinessError(env, ret, "get subject alt names failed", true));
        LOGE("call getSubjectAltNames failed!");
        HcfFree(array);
        array = nullptr;
        return nullptr;
    }
    napi_value returnValue = ConvertArrayToNapiValue(env, array);
    HcfArrayDataClearAndFree(array);
    HcfFree(array);
    array = nullptr;
    return returnValue;
}

napi_value NapiX509Certificate::GetIssuerAlternativeNames(napi_env env, napi_callback_info info)
{
    HcfArray *array = reinterpret_cast<HcfArray *>(HcfMalloc(sizeof(HcfArray), 0));
    if (array == nullptr) {
        LOGE("malloc array failed!");
        return nullptr;
    }
    HcfX509Certificate *cert = GetX509Cert();
    HcfResult ret = cert->getIssuerAltNames(cert, array);
    if (ret != HCF_SUCCESS) {
        napi_throw(env, GenerateBusinessError(env, ret, "get issuer alt names failed", true));
        LOGE("call getIssuerAltNames failed!");
        HcfFree(array);
        array = nullptr;
        return nullptr;
    }
    napi_value returnValue = ConvertArrayToNapiValue(env, array);
    HcfArrayDataClearAndFree(array);
    HcfFree(array);
    array = nullptr;
    return returnValue;
}

static napi_value NapiVerify(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);
    NapiX509Certificate *x509Cert = nullptr;
    napi_unwrap(env, thisVar, reinterpret_cast<void **>(&x509Cert));
    if (x509Cert == nullptr) {
        LOGE("x509Cert is nullptr!");
        return nullptr;
    }
    return x509Cert->Verify(env, info);
}

static napi_value NapiGetEncoded(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);
    NapiX509Certificate *x509Cert = nullptr;
    napi_unwrap(env, thisVar, reinterpret_cast<void **>(&x509Cert));
    if (x509Cert == nullptr) {
        LOGE("x509Cert is nullptr!");
        return nullptr;
    }
    return x509Cert->GetEncoded(env, info);
}

static napi_value NapiGetPublicKey(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);
    NapiX509Certificate *x509Cert = nullptr;
    napi_unwrap(env, thisVar, reinterpret_cast<void **>(&x509Cert));
    if (x509Cert == nullptr) {
        LOGE("x509Cert is nullptr!");
        return nullptr;
    }
    return x509Cert->GetPublicKey(env, info);
}

static napi_value NapiCheckValidityWithDate(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);
    NapiX509Certificate *x509Cert = nullptr;
    napi_unwrap(env, thisVar, reinterpret_cast<void **>(&x509Cert));
    if (x509Cert == nullptr) {
        LOGE("x509Cert is nullptr!");
        return nullptr;
    }
    return x509Cert->CheckValidityWithDate(env, info);
}

static napi_value NapiGetVersion(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);
    NapiX509Certificate *x509Cert = nullptr;
    napi_unwrap(env, thisVar, reinterpret_cast<void **>(&x509Cert));
    if (x509Cert == nullptr) {
        LOGE("x509Cert is nullptr!");
        return nullptr;
    }
    return x509Cert->GetVersion(env, info);
}

static napi_value NapiGetSerialNumber(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);
    NapiX509Certificate *x509Cert = nullptr;
    napi_unwrap(env, thisVar, reinterpret_cast<void **>(&x509Cert));
    if (x509Cert == nullptr) {
        LOGE("x509Cert is nullptr!");
        return nullptr;
    }
    return x509Cert->GetSerialNumber(env, info);
}

static napi_value NapiGetIssuerName(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);
    NapiX509Certificate *x509Cert = nullptr;
    napi_unwrap(env, thisVar, reinterpret_cast<void **>(&x509Cert));
    if (x509Cert == nullptr) {
        LOGE("x509Cert is nullptr!");
        return nullptr;
    }
    return x509Cert->GetIssuerName(env, info);
}

static napi_value NapiGetSubjectName(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);
    NapiX509Certificate *x509Cert = nullptr;
    napi_unwrap(env, thisVar, reinterpret_cast<void **>(&x509Cert));
    if (x509Cert == nullptr) {
        LOGE("x509Cert is nullptr!");
        return nullptr;
    }
    return x509Cert->GetSubjectName(env, info);
}

static napi_value NapiGetNotBeforeTime(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);
    NapiX509Certificate *x509Cert = nullptr;
    napi_unwrap(env, thisVar, reinterpret_cast<void **>(&x509Cert));
    if (x509Cert == nullptr) {
        LOGE("x509Cert is nullptr!");
        return nullptr;
    }
    return x509Cert->GetNotBeforeTime(env, info);
}

static napi_value NapiGetNotAfterTime(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);
    NapiX509Certificate *x509Cert = nullptr;
    napi_unwrap(env, thisVar, reinterpret_cast<void **>(&x509Cert));
    if (x509Cert == nullptr) {
        LOGE("x509Cert is nullptr!");
        return nullptr;
    }
    return x509Cert->GetNotAfterTime(env, info);
}

static napi_value NapiGetSignature(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);
    NapiX509Certificate *x509Cert = nullptr;
    napi_unwrap(env, thisVar, reinterpret_cast<void **>(&x509Cert));
    if (x509Cert == nullptr) {
        LOGE("x509Cert is nullptr!");
        return nullptr;
    }
    return x509Cert->GetSignature(env, info);
}

static napi_value NapiGetSigAlgName(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);
    NapiX509Certificate *x509Cert = nullptr;
    napi_unwrap(env, thisVar, reinterpret_cast<void **>(&x509Cert));
    if (x509Cert == nullptr) {
        LOGE("x509Cert is nullptr!");
        return nullptr;
    }
    return x509Cert->GetSigAlgName(env, info);
}

static napi_value NapiGetSigAlgOID(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);
    NapiX509Certificate *x509Cert = nullptr;
    napi_unwrap(env, thisVar, reinterpret_cast<void **>(&x509Cert));
    if (x509Cert == nullptr) {
        LOGE("x509Cert is nullptr!");
        return nullptr;
    }
    return x509Cert->GetSigAlgOID(env, info);
}

static napi_value NapiGetSigAlgParams(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);
    NapiX509Certificate *x509Cert = nullptr;
    napi_unwrap(env, thisVar, reinterpret_cast<void **>(&x509Cert));
    if (x509Cert == nullptr) {
        LOGE("x509Cert is nullptr!");
        return nullptr;
    }
    return x509Cert->GetSigAlgParams(env, info);
}

static napi_value NapiGetKeyUsage(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);
    NapiX509Certificate *x509Cert = nullptr;
    napi_unwrap(env, thisVar, reinterpret_cast<void **>(&x509Cert));
    if (x509Cert == nullptr) {
        LOGE("x509Cert is nullptr!");
        return nullptr;
    }
    return x509Cert->GetKeyUsage(env, info);
}

static napi_value NapiGetExtendedKeyUsage(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);
    NapiX509Certificate *x509Cert = nullptr;
    napi_unwrap(env, thisVar, reinterpret_cast<void **>(&x509Cert));
    if (x509Cert == nullptr) {
        LOGE("x509Cert is nullptr!");
        return nullptr;
    }
    return x509Cert->GetExtendedKeyUsage(env, info);
}

static napi_value NapiGetBasicConstraints(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);
    NapiX509Certificate *x509Cert = nullptr;
    napi_unwrap(env, thisVar, reinterpret_cast<void **>(&x509Cert));
    if (x509Cert == nullptr) {
        LOGE("x509Cert is nullptr!");
        return nullptr;
    }
    return x509Cert->GetBasicConstraints(env, info);
}

static napi_value NapiGetSubjectAlternativeNames(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);
    NapiX509Certificate *x509Cert = nullptr;
    napi_unwrap(env, thisVar, reinterpret_cast<void **>(&x509Cert));
    if (x509Cert == nullptr) {
        LOGE("x509Cert is nullptr!");
        return nullptr;
    }
    return x509Cert->GetSubjectAlternativeNames(env, info);
}

static napi_value NapiGetIssuerAlternativeNames(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);
    NapiX509Certificate *x509Cert = nullptr;
    napi_unwrap(env, thisVar, reinterpret_cast<void **>(&x509Cert));
    if (x509Cert == nullptr) {
        LOGE("x509Cert is nullptr!");
        return nullptr;
    }
    return x509Cert->GetIssuerAlternativeNames(env, info);
}

void NapiX509Certificate::CreateX509CertExecute(napi_env env, void *data)
{
    CfCtx *context = static_cast<CfCtx *>(data);
    context->errCode = HcfX509CertificateCreate(context->encodingBlob, &context->cert);
    if (context->errCode != HCF_SUCCESS) {
        context->errMsg = "create X509Cert failed";
    }
}

void NapiX509Certificate::CreateX509CertComplete(napi_env env, napi_status status, void *data)
{
    CfCtx *context = static_cast<CfCtx *>(data);
    if (context->errCode != HCF_SUCCESS) {
        LOGE("call create X509Cert failed!");
        ReturnResult(env, context, nullptr);
        FreeCryptoFwkCtx(env, context);
        return;
    }
    napi_value instance = CreateX509Cert(env);
    NapiX509Certificate *x509CertClass = new NapiX509Certificate(context->cert);
    napi_wrap(
        env, instance, x509CertClass,
        [](napi_env env, void *data, void *hint) {
            NapiX509Certificate *certClass = static_cast<NapiX509Certificate *>(data);
            delete certClass;
            return;
        },
        nullptr, nullptr);
    ReturnResult(env, context, instance);
    FreeCryptoFwkCtx(env, context);
}

napi_value NapiX509Certificate::NapiCreateX509Cert(napi_env env, napi_callback_info info)
{
    size_t argc = ARGS_SIZE_TWO;
    napi_value argv[ARGS_SIZE_TWO] = { nullptr };
    napi_value thisVar = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (!CheckArgsCount(env, argc, ARGS_SIZE_TWO, false, true)) {
        return nullptr;
    }

    CfCtx *context = static_cast<CfCtx *>(HcfMalloc(sizeof(CfCtx), 0));
    if (context == nullptr) {
        LOGE("malloc context failed!");
        return nullptr;
    }
    if (!GetEncodingBlobFromValue(env, argv[PARAM0], &context->encodingBlob)) {
        LOGE("get encoding blob from data failed!");
        FreeCryptoFwkCtx(env, context);
        return nullptr;
    }

    if (!CreateCallbackAndPromise(env, context, argc, ARGS_SIZE_TWO, argv[PARAM1])) {
        FreeCryptoFwkCtx(env, context);
        return nullptr;
    }

    napi_create_async_work(
        env, nullptr, GetResourceName(env, "CreateX509Cert"),
        CreateX509CertExecute,
        CreateX509CertComplete,
        static_cast<void *>(context),
        &context->asyncWork);

    napi_queue_async_work(env, context->asyncWork);
    if (context->asyncType == ASYNC_TYPE_PROMISE) {
        return context->promise;
    } else {
        return NapiGetNull(env);
    }
}

static napi_value X509CertConstructor(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);
    return thisVar;
}

void NapiX509Certificate::DefineX509CertJSClass(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("createX509Cert", NapiCreateX509Cert),
    };
    napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc);

    napi_property_descriptor x509CertDesc[] = {
        DECLARE_NAPI_FUNCTION("verify", NapiVerify),
        DECLARE_NAPI_FUNCTION("getEncoded", NapiGetEncoded),
        DECLARE_NAPI_FUNCTION("getPublicKey", NapiGetPublicKey),
        DECLARE_NAPI_FUNCTION("checkValidityWithDate", NapiCheckValidityWithDate),
        DECLARE_NAPI_FUNCTION("getVersion", NapiGetVersion),
        DECLARE_NAPI_FUNCTION("getSerialNumber", NapiGetSerialNumber),
        DECLARE_NAPI_FUNCTION("getIssuerName", NapiGetIssuerName),
        DECLARE_NAPI_FUNCTION("getSubjectName", NapiGetSubjectName),
        DECLARE_NAPI_FUNCTION("getNotBeforeTime", NapiGetNotBeforeTime),
        DECLARE_NAPI_FUNCTION("getNotAfterTime", NapiGetNotAfterTime),
        DECLARE_NAPI_FUNCTION("getSignature", NapiGetSignature),
        DECLARE_NAPI_FUNCTION("getSignatureAlgName", NapiGetSigAlgName),
        DECLARE_NAPI_FUNCTION("getSignatureAlgOid", NapiGetSigAlgOID),
        DECLARE_NAPI_FUNCTION("getSignatureAlgParams", NapiGetSigAlgParams),
        DECLARE_NAPI_FUNCTION("getKeyUsage", NapiGetKeyUsage),
        DECLARE_NAPI_FUNCTION("getExtKeyUsage", NapiGetExtendedKeyUsage),
        DECLARE_NAPI_FUNCTION("getBasicConstraints", NapiGetBasicConstraints),
        DECLARE_NAPI_FUNCTION("getSubjectAltNames", NapiGetSubjectAlternativeNames),
        DECLARE_NAPI_FUNCTION("getIssuerAltNames", NapiGetIssuerAlternativeNames),
    };
    napi_value constructor = nullptr;
    napi_define_class(env, "X509Cert", NAPI_AUTO_LENGTH, X509CertConstructor, nullptr,
        sizeof(x509CertDesc) / sizeof(x509CertDesc[0]), x509CertDesc, &constructor);
    napi_create_reference(env, constructor, 1, &classRef_);
}

napi_value NapiX509Certificate::CreateX509Cert(napi_env env)
{
    napi_value constructor = nullptr;
    napi_value instance = nullptr;
    napi_get_reference_value(env, classRef_, &constructor);
    napi_new_instance(env, constructor, 0, nullptr, &instance);
    return instance;
}
} // namespace CryptoFramework
} // namespace OHOS
