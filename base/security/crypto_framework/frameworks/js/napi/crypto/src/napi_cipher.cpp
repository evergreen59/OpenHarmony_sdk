/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "napi_cipher.h"
#include "napi_key.h"
#include "securec.h"
#include "log.h"
#include "memory.h"

#include "cipher.h"
#include "napi_utils.h"
#include "napi_crypto_framework_defines.h"
#include "detailed_iv_params.h"
#include "detailed_gcm_params.h"
#include "detailed_ccm_params.h"

namespace OHOS {
namespace CryptoFramework {
thread_local napi_ref NapiCipher::classRef_ = nullptr;

struct CipherFwkCtxT {
    napi_env env = nullptr;
    AsyncType asyncType = ASYNC_CALLBACK;
    napi_ref callback = nullptr;
    napi_deferred deferred = nullptr;
    napi_value promise = nullptr;
    napi_async_work asyncWork = nullptr;

    HcfCipher *cipher = nullptr;
    HcfKey *key = nullptr;
    HcfParamsSpec *paramsSpec = nullptr;
    HcfBlob input = { .data = nullptr, .len = 0 };
    HcfBlob output = { .data = nullptr, .len = 0 };
    enum HcfCryptoMode opMode = ENCRYPT_MODE;

    int32_t errCode = 0;
    const char *errMsg = nullptr;
};

using CipherFwkCtx = CipherFwkCtxT *;

static void FreeParamsSpec(HcfParamsSpec *paramsSpec)
{
    if (paramsSpec == nullptr) {
        return;
    }
    if (IV_PARAMS_SPEC.compare(paramsSpec->getType()) == 0) {
        HcfIvParamsSpec *iv = reinterpret_cast<HcfIvParamsSpec *>(paramsSpec);
        HcfFree(iv->iv.data);
        iv->iv.data = nullptr;
        iv->iv.len = 0;
    }
    if (GCM_PARAMS_SPEC.compare(paramsSpec->getType()) == 0) {
        HcfGcmParamsSpec *gcm = reinterpret_cast<HcfGcmParamsSpec *>(paramsSpec);
        HcfFree(gcm->iv.data);
        HcfFree(gcm->aad.data);
        HcfFree(gcm->tag.data);
        gcm->iv.len = 0;
        gcm->aad.len = 0;
        gcm->tag.len = 0;
        gcm->iv.data = nullptr;
        gcm->aad.data = nullptr;
        gcm->tag.data = nullptr;
    }
    if (CCM_PARAMS_SPEC.compare(paramsSpec->getType()) == 0) {
        HcfCcmParamsSpec *ccm = reinterpret_cast<HcfCcmParamsSpec *>(paramsSpec);
        HcfFree(ccm->iv.data);
        HcfFree(ccm->aad.data);
        HcfFree(ccm->tag.data);
        ccm->iv.len = 0;
        ccm->aad.len = 0;
        ccm->tag.len = 0;
        ccm->iv.data = nullptr;
        ccm->aad.data = nullptr;
        ccm->tag.data = nullptr;
    }
    HcfFree(paramsSpec);
}

static void FreeCipherFwkCtx(napi_env env, CipherFwkCtx &context)
{
    if (context == nullptr) {
        return;
    }

    if (context->asyncWork != nullptr) {
        napi_delete_async_work(env, context->asyncWork);
        context->asyncWork = nullptr;
    }

    if (context->callback != nullptr) {
        napi_delete_reference(env, context->callback);
        context->callback = nullptr;
    }
    if (context->input.data != nullptr) {
        HcfFree(context->input.data);
        context->input.data = nullptr;
        context->input.len = 0;
    }
    if (context->output.data != nullptr) {
        HcfFree(context->output.data);
        context->output.data = nullptr;
        context->output.len = 0;
    }
    FreeParamsSpec(context->paramsSpec);
    context->paramsSpec = nullptr;

    context->cipher = nullptr;
    context->key = nullptr;
    context->errMsg = nullptr;
    HcfFree(context);
    context = nullptr;
}

bool BuildContextForInit(napi_env env, napi_callback_info info, CipherFwkCtx context)
{
    napi_value thisVar = nullptr;
    NapiCipher *napiCipher = nullptr;
    NapiKey *napiKey = nullptr;
    size_t expectedArgc = ARGS_SIZE_FOUR;
    size_t argc = ARGS_SIZE_FOUR;
    napi_value argv[ARGS_SIZE_FOUR] = { nullptr };
    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (argc != expectedArgc && argc != expectedArgc - 1) {
        napi_throw(env, GenerateBusinessError(env, HCF_INVALID_PARAMS, "init failed for wrong argument num.", false));
        LOGE("wrong argument num. require 3 or 4 arguments. [Argc]: %zu!", argc);
        return false;
    }
    context->asyncType = (argc == expectedArgc) ? ASYNC_CALLBACK : ASYNC_PROMISE;

    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&napiCipher));
    if (status != napi_ok) {
        LOGE("failed to unwrap napi napiCipher obj!");
        return false;
    }
    context->cipher = napiCipher->GetCipher();

    // get opMode, type is uint32
    size_t index = ARGS_SIZE_ZERO;
    if (napi_get_value_uint32(env, argv[index++], reinterpret_cast<uint32_t *>(&(context->opMode))) != napi_ok) {
        napi_throw(env, GenerateBusinessError(env, HCF_INVALID_PARAMS, "get opMode failed!", false));
        LOGE("get opMode failed!");
        return false;
    }

    // get key, unwrap from JS
    status = napi_unwrap(env, argv[index++], reinterpret_cast<void **>(&napiKey));
    if (status != napi_ok) {
        napi_throw(env, GenerateBusinessError(env, HCF_INVALID_PARAMS, "failed to unwrap napi napiSymKey obj!", false));
        LOGE("failed to unwrap napi napiSymKey obj!");
        return false;
    }
    context->key = napiKey->GetHcfKey();

    // get paramsSpec, unwrap from JS
    napi_valuetype valueType;
    napi_typeof(env, argv[index], &valueType);
    if (valueType != napi_null) {
        if (!GetParamsSpecFromNapiValue(env, argv[index], context->opMode, &context->paramsSpec)) {
            napi_throw(env, GenerateBusinessError(env, HCF_INVALID_PARAMS, "failed to get valid params spec!", false));
            LOGE("GetParamsSpecFromNapiValue failed!");
            return false;
        }
    }
    index++;

    if (context->asyncType == ASYNC_PROMISE) {
        napi_create_promise(env, &context->deferred, &context->promise);
        return true;
    } else {
        return GetCallbackFromJSParams(env, argv[index], &context->callback, false);
    }
}

bool BuildContextForUpdate(napi_env env, napi_callback_info info, CipherFwkCtx context)
{
    napi_value thisVar = nullptr;
    NapiCipher *napiCipher = nullptr;
    size_t expectedArgc = ARGS_SIZE_TWO;
    size_t argc = ARGS_SIZE_TWO;
    napi_value argv[ARGS_SIZE_TWO] = { nullptr };
    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (argc != expectedArgc && argc != expectedArgc - 1) {
        napi_throw(env, GenerateBusinessError(env, HCF_INVALID_PARAMS, "update failed for wrong argument num.", false));
        LOGE("wrong argument num. require 1 or 2 arguments. [Argc]: %zu!", argc);
        return false;
    }
    context->asyncType = (argc == expectedArgc) ? ASYNC_CALLBACK : ASYNC_PROMISE;

    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&napiCipher));
    if (status != napi_ok) {
        LOGE("failed to unwrap napi napiCipher obj!");
        return false;
    }
    context->cipher = napiCipher->GetCipher();

    // get input, type is blob
    size_t index = ARGS_SIZE_ZERO;
    HcfBlob *input = nullptr;
    input = GetBlobFromNapiValue(env, argv[index++]);
    if (input == nullptr) {
        napi_throw(env, GenerateBusinessError(env, HCF_INVALID_PARAMS, "update failed for invalid input blob.", false));
        LOGE("GetBlobFromNapiValue failed!");
        return false;
    }
    context->input.data = input->data;
    context->input.len = input->len;
    HcfFree(input);
    if (context->asyncType == ASYNC_PROMISE) {
        napi_create_promise(env, &context->deferred, &context->promise);
        return true;
    } else {
        return GetCallbackFromJSParams(env, argv[index], &context->callback, false);
    }
}

bool BuildContextForFinal(napi_env env, napi_callback_info info, CipherFwkCtx context)
{
    napi_value thisVar = nullptr;
    NapiCipher *napiCipher = nullptr;
    size_t expectedArgc = ARGS_SIZE_TWO;
    size_t argc = ARGS_SIZE_TWO;
    napi_value argv[ARGS_SIZE_TWO] = { nullptr };
    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (argc != expectedArgc && argc != expectedArgc - 1) {
        napi_throw(env, GenerateBusinessError(env, HCF_INVALID_PARAMS,
            "doFinal failed for invalid input blob.", false));
        LOGE("wrong argument num. require 1 or 2 arguments. [Argc]: %zu!", argc);
        return false;
    }
    context->asyncType = (argc == expectedArgc) ? ASYNC_CALLBACK : ASYNC_PROMISE;

    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&napiCipher));
    if (status != napi_ok) {
        LOGE("failed to unwrap napi napiCipher obj!");
        return false;
    }
    context->cipher = napiCipher->GetCipher();

    // get input, type is blob
    size_t index = ARGS_SIZE_ZERO;
    napi_valuetype valueType;
    napi_typeof(env, argv[index], &valueType);
    if (valueType != napi_null) {
        HcfBlob *input = nullptr;
        input = GetBlobFromNapiValue(env, argv[index]);
        if (input == nullptr) {
            napi_throw(env, GenerateBusinessError(env, HCF_INVALID_PARAMS,
                "doFinal failed for invalid input blob.", false));
            LOGE("GetBlobFromNapiValue failed!");
            return false;
        }
        context->input.data = input->data;
        context->input.len = input->len;
        HcfFree(input);
    }
    index++;
    if (context->asyncType == ASYNC_PROMISE) {
        napi_create_promise(env, &context->deferred, &context->promise);
        return true;
    } else {
        return GetCallbackFromJSParams(env, argv[index], &context->callback, false);
    }
}

static void ReturnCallbackResult(napi_env env, CipherFwkCtx context, napi_value result)
{
    napi_value businessError = nullptr;
    if (context->errCode != HCF_SUCCESS) {
        businessError = GenerateBusinessError(env, context->errCode, context->errMsg, false);
    }
    napi_value params[ARGS_SIZE_TWO] = { businessError, result };

    napi_value func = nullptr;
    napi_get_reference_value(env, context->callback, &func);

    napi_value recv = nullptr;
    napi_value callFuncRet = nullptr;
    napi_get_undefined(env, &recv);
    napi_call_function(env, recv, func, ARGS_SIZE_TWO, params, &callFuncRet);
}

static void ReturnPromiseResult(napi_env env, CipherFwkCtx context, napi_value result)
{
    if (context->errCode == HCF_SUCCESS) {
        napi_resolve_deferred(env, context->deferred, result);
    } else {
        napi_reject_deferred(env, context->deferred,
            GenerateBusinessError(env, context->errCode, context->errMsg, false));
    }
}

// init execute
void AsyncInitProcess(napi_env env, void *data)
{
    if (data == nullptr) {
        return;
    }
    CipherFwkCtx context = static_cast<CipherFwkCtx>(data);
    HcfCipher *cipher = context->cipher;
    HcfParamsSpec *params = context->paramsSpec;
    HcfKey *key = context->key;

    HcfResult res = cipher->init(cipher, context->opMode, key, params);
    if (res != HCF_SUCCESS) {
        LOGE("init ret:%d", res);
        context->errCode = res;
        context->errMsg = "init failed.";
        return;
    }
    context->errCode = HCF_SUCCESS;
}

// update execute
void AsyncUpdateProcess(napi_env env, void *data)
{
    if (data == nullptr) {
        return;
    }
    CipherFwkCtx context = static_cast<CipherFwkCtx>(data);
    HcfCipher *cipher = context->cipher;
    HcfResult res = cipher->update(cipher, &context->input, &context->output);
    if (res != HCF_SUCCESS) {
        LOGE("Update ret:%d!", res);
        context->errCode = res;
        context->errMsg = "update failed.";
        return;
    }
    context->errCode = HCF_SUCCESS;
}

void AsyncDoFinalProcess(napi_env env, void *data)
{
    if (data == nullptr) {
        return;
    }
    CipherFwkCtx context = static_cast<CipherFwkCtx>(data);
    HcfCipher *cipher = context->cipher;

    HcfResult res = cipher->doFinal(cipher, &context->input, &context->output);
    if (res != HCF_SUCCESS) {
        LOGE("doFinal ret:%d!", res);
        context->errCode = res;
        context->errMsg = "doFinal failed.";
        return;
    }
    context->errCode = HCF_SUCCESS;
}

napi_value GetNapiNull(napi_env env)
{
    napi_value output = nullptr;
    napi_status status = napi_get_null(env, &output);
    if (status != napi_ok) {
        LOGE("create null napi value failed");
    }
    return output;
}

void AsyncInitReturn(napi_env env, napi_status status, void *data)
{
    CipherFwkCtx context = static_cast<CipherFwkCtx>(data);
    napi_value result = GetNapiNull(env);

    if (context->asyncType == ASYNC_CALLBACK) {
        ReturnCallbackResult(env, context, result);
    } else {
        ReturnPromiseResult(env, context, result);
    }
    FreeCipherFwkCtx(env, context);
}

void AsyncUpdateReturn(napi_env env, napi_status status, void *data)
{
    CipherFwkCtx context = static_cast<CipherFwkCtx>(data);
    napi_value instance = ConvertBlobToNapiValue(env, &context->output);
    if (instance == nullptr) {
        LOGE("May be nullptr!");
        instance = GetNapiNull(env);
    }

    if (context->asyncType == ASYNC_CALLBACK) {
        ReturnCallbackResult(env, context, instance);
    } else {
        ReturnPromiseResult(env, context, instance);
    }
    FreeCipherFwkCtx(env, context);
}

void AsyncDoFinalReturn(napi_env env, napi_status status, void *data)
{
    CipherFwkCtx context = static_cast<CipherFwkCtx>(data);
    napi_value instance = ConvertBlobToNapiValue(env, &context->output);
    if (instance == nullptr) {
        LOGE("Maybe in decrypt mode, or CCM crypto maybe occur!");
        instance = GetNapiNull(env);
    }

    if (context->asyncType == ASYNC_CALLBACK) {
        ReturnCallbackResult(env, context, instance);
    } else {
        ReturnPromiseResult(env, context, instance);
    }
    FreeCipherFwkCtx(env, context);
}

napi_value NewAsyncInit(napi_env env, CipherFwkCtx context)
{
    napi_value resourceName = nullptr;
    napi_create_string_utf8(env, "init", NAPI_AUTO_LENGTH, &resourceName);

    napi_create_async_work(
        env, nullptr, resourceName,
        [](napi_env env, void *data) {
            AsyncInitProcess(env, data);
            return;
        },
        [](napi_env env, napi_status status, void *data) {
            AsyncInitReturn(env, status, data);
            return;
        },
        static_cast<void *>(context),
        &context->asyncWork);

    napi_queue_async_work(env, context->asyncWork);
    if (context->asyncType == ASYNC_PROMISE) {
        return context->promise;
    } else {
        return NapiGetNull(env);
    }
}

napi_value NewAsyncUpdate(napi_env env, CipherFwkCtx context)
{
    napi_value resourceName = nullptr;
    napi_create_string_utf8(env, "update", NAPI_AUTO_LENGTH, &resourceName);

    napi_create_async_work(
        env, nullptr, resourceName,
        [](napi_env env, void *data) {
            AsyncUpdateProcess(env, data);
            return;
        },
        [](napi_env env, napi_status status, void *data) {
            AsyncUpdateReturn(env, status, data);
            return;
        },
        static_cast<void *>(context),
        &context->asyncWork);

    napi_queue_async_work(env, context->asyncWork);
    if (context->asyncType == ASYNC_PROMISE) {
        return context->promise;
    } else {
        return NapiGetNull(env);
    }
}

napi_value NewAsyncDoFinal(napi_env env, CipherFwkCtx context)
{
    napi_value resourceName = nullptr;
    napi_create_string_utf8(env, "doFinal", NAPI_AUTO_LENGTH, &resourceName);

    napi_create_async_work(
        env, nullptr, resourceName,
        [](napi_env env, void *data) {
            AsyncDoFinalProcess(env, data);
            return;
        },
        [](napi_env env, napi_status status, void *data) {
            AsyncDoFinalReturn(env, status, data);
            return;
        },
        static_cast<void *>(context),
        &context->asyncWork);

    napi_queue_async_work(env, context->asyncWork);
    if (context->asyncType == ASYNC_PROMISE) {
        return context->promise;
    } else {
        return NapiGetNull(env);
    }
}

NapiCipher::NapiCipher(HcfCipher *cipher)
{
    this->cipher_ = cipher;
}

NapiCipher::~NapiCipher()
{
    HcfObjDestroy(this->cipher_);
}

HcfCipher *NapiCipher::GetCipher() const
{
    return this->cipher_;
}

napi_value NapiCipher::JsCipherInit(napi_env env, napi_callback_info info)
{
    CipherFwkCtx context = static_cast<CipherFwkCtx>(HcfMalloc(sizeof(CipherFwkCtxT), 0));
    if (context == nullptr) {
        LOGE("create context fail!");
        return nullptr;
    }

    if (!BuildContextForInit(env, info, context)) {
        LOGE("build context for init fail!");
        FreeCipherFwkCtx(env, context);
        return nullptr;
    }

    return NewAsyncInit(env, context);
}

napi_value NapiCipher::JsCipherUpdate(napi_env env, napi_callback_info info)
{
    CipherFwkCtx context = static_cast<CipherFwkCtx>(HcfMalloc(sizeof(CipherFwkCtxT), 0));
    if (context == nullptr) {
        LOGE("create context fail!");
        return nullptr;
    }

    if (!BuildContextForUpdate(env, info, context)) {
        LOGE("build context for update fail!");
        FreeCipherFwkCtx(env, context);
        return nullptr;
    }

    return NewAsyncUpdate(env, context);
}

napi_value NapiCipher::JsCipherDoFinal(napi_env env, napi_callback_info info)
{
    CipherFwkCtx context = static_cast<CipherFwkCtx>(HcfMalloc(sizeof(CipherFwkCtxT), 0));
    if (context == nullptr) {
        LOGE("create context fail!");
        return nullptr;
    }

    if (!BuildContextForFinal(env, info, context)) {
        LOGE("build context for final fail!");
        FreeCipherFwkCtx(env, context);
        return nullptr;
    }
    return NewAsyncDoFinal(env, context);
}

napi_value NapiCipher::JsGetAlgorithm(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    NapiCipher *napiCipher = nullptr;

    NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr));

    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&napiCipher));
    if (status != napi_ok) {
        LOGE("failed to unwrap napiCipher obj!");
        return nullptr;
    }

    HcfCipher *cipher = napiCipher->GetCipher();
    if (cipher == nullptr) {
        LOGE("failed to get cipher obj!");
        return nullptr;
    }

    // execute C function
    const char *algo = cipher->getAlgorithm(cipher);
    napi_value instance = nullptr;
    napi_create_string_utf8(env, algo, NAPI_AUTO_LENGTH, &instance);
    return instance;
}

napi_value NapiCipher::CipherConstructor(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr));

    return thisVar;
}

napi_value NapiCipher::CreateCipher(napi_env env, napi_callback_info info)
{
    size_t expectedArgc = ARGS_SIZE_ONE;
    size_t argc = ARGS_SIZE_ONE;
    napi_value argv[ARGS_SIZE_ONE] = { nullptr };
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));

    if (argc != expectedArgc) {
        napi_throw(env, GenerateBusinessError(env, HCF_INVALID_PARAMS, "The input args num is invalid.", false));
        LOGE("The input args num is invalid.");
        return nullptr;
    }

    // create instance according to input js object
    napi_value instance = nullptr;
    napi_value constructor = nullptr;
    NAPI_CALL(env, napi_get_reference_value(env, classRef_, &constructor));
    NAPI_CALL(env, napi_new_instance(env, constructor, argc, argv, &instance));

    // parse input string
    std::string algoName;
    if (!GetStringFromJSParams(env, argv[ARGS_SIZE_ZERO], algoName, false)) {
        LOGE("GetStringFromJSParams failed!");
        return nullptr;
    }

    // execute C function, generate C object
    HcfCipher *cipher = nullptr;
    HcfResult res = HcfCipherCreate(algoName.c_str(), &cipher);
    if (res != HCF_SUCCESS) {
        napi_throw(env, GenerateBusinessError(env, res, "create C cipher fail!", false));
        LOGE("create C cipher fail!");
        return nullptr;
    }
    NapiCipher *napiCipher = new (std::nothrow) NapiCipher(cipher);
    if (napiCipher == nullptr) {
        LOGE("new napiCipher failed!");
        HcfObjDestroy(cipher);
        return nullptr;
    }

    napi_status status = napi_wrap(env, instance, napiCipher,
        [](napi_env env, void *data, void *hint) {
            NapiCipher *napiCipher = static_cast<NapiCipher *>(data);
            delete napiCipher;
            return;
        },
        nullptr,
        nullptr);
    if (status != napi_ok) {
        LOGE("failed to wrap napiCipher obj!");
        delete napiCipher;
        return nullptr;
    }
    return instance;
}

void NapiCipher::DefineCipherJSClass(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("createCipher", CreateCipher),
    };
    napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc);

    napi_property_descriptor classDesc[] = {
        DECLARE_NAPI_FUNCTION("init", NapiCipher::JsCipherInit),
        DECLARE_NAPI_FUNCTION("update", NapiCipher::JsCipherUpdate),
        DECLARE_NAPI_FUNCTION("doFinal", NapiCipher::JsCipherDoFinal),
        { .utf8name = "algName", .getter = NapiCipher::JsGetAlgorithm },
    };
    napi_value constructor = nullptr;
    napi_define_class(env, "Cipher", NAPI_AUTO_LENGTH, NapiCipher::CipherConstructor, nullptr,
        sizeof(classDesc) / sizeof(classDesc[0]), classDesc, &constructor);
    napi_create_reference(env, constructor, 1, &classRef_);
}
} // CryptoFramework
} // OHOS