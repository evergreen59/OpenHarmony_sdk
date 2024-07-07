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

#include "napi_key_agreement.h"

#include "securec.h"
#include "log.h"
#include "memory.h"

#include "napi_crypto_framework_defines.h"
#include "napi_pri_key.h"
#include "napi_pub_key.h"
#include "napi_utils.h"

namespace OHOS {
namespace CryptoFramework {
struct KeyAgreementCtx {
    napi_env env = nullptr;

    AsyncType asyncType = ASYNC_CALLBACK;
    napi_ref callback = nullptr;
    napi_deferred deferred = nullptr;
    napi_value promise = nullptr;
    napi_async_work asyncWork = nullptr;

    HcfKeyAgreement *keyAgreement;
    HcfPriKey *priKey;
    HcfPubKey *pubKey;

    HcfResult result;
    HcfBlob returnSecret;
};

thread_local napi_ref NapiKeyAgreement::classRef_ = nullptr;

static void FreeKeyAgreementCtx(napi_env env, KeyAgreementCtx *ctx)
{
    if (ctx == nullptr) {
        return;
    }

    if (ctx->asyncWork != nullptr) {
        napi_delete_async_work(env, ctx->asyncWork);
        ctx->asyncWork = nullptr;
    }

    if (ctx->callback != nullptr) {
        napi_delete_reference(env, ctx->callback);
        ctx->callback = nullptr;
    }

    if (ctx->returnSecret.data != nullptr) {
        HcfFree(ctx->returnSecret.data);
        ctx->returnSecret.data = nullptr;
        ctx->returnSecret.len = 0;
    }

    HcfFree(ctx);
}

static bool BuildKeyAgreementJsCtx(napi_env env, napi_callback_info info, KeyAgreementCtx *ctx)
{
    napi_value thisVar = nullptr;
    size_t expectedArgc = PARAMS_NUM_THREE;
    size_t argc = expectedArgc;
    napi_value argv[PARAMS_NUM_THREE] = { nullptr };
    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (argc != expectedArgc && argc != expectedArgc - 1) {
        LOGE("wrong argument num. require %zu or %zu arguments. [Argc]: %zu!", expectedArgc - 1, expectedArgc, argc);
        napi_throw(env, GenerateBusinessError(env, HCF_INVALID_PARAMS, "params num error.", false));
        return false;
    }
    ctx->asyncType = (argc == expectedArgc) ? ASYNC_CALLBACK : ASYNC_PROMISE;

    NapiKeyAgreement *napiKeyAgreement = nullptr;
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&napiKeyAgreement));
    if (status != napi_ok) {
        LOGE("failed to unwrap napi verify obj.");
        napi_throw(env, GenerateBusinessError(env, HCF_INVALID_PARAMS, "[Self]: param unwarp error.", false));
        return false;
    }

    size_t index = 0;
    NapiPriKey *napiPriKey = nullptr;
    status = napi_unwrap(env, argv[index], reinterpret_cast<void **>(&napiPriKey));
    if (status != napi_ok) {
        LOGE("failed to unwrap priKey verify obj.");
        napi_throw(env, GenerateBusinessError(env, HCF_INVALID_PARAMS, "[PriKey]: param unwarp error.", false));
        return false;
    }

    index++;
    NapiPubKey *napiPubKey = nullptr;
    status = napi_unwrap(env, argv[index], reinterpret_cast<void **>(&napiPubKey));
    if (status != napi_ok) {
        LOGE("failed to unwrap napi pubKey obj.");
        napi_throw(env, GenerateBusinessError(env, HCF_INVALID_PARAMS, "[PubKey]: param unwarp error.", false));
        return false;
    }

    ctx->keyAgreement = napiKeyAgreement->GetKeyAgreement();
    ctx->priKey = napiPriKey->GetPriKey();
    ctx->pubKey = napiPubKey->GetPubKey();

    if (ctx->asyncType == ASYNC_PROMISE) {
        napi_create_promise(env, &ctx->deferred, &ctx->promise);
        return true;
    } else {
        return GetCallbackFromJSParams(env, argv[expectedArgc - 1], &ctx->callback, false);
    }
}

static void ReturnCallbackResult(napi_env env, KeyAgreementCtx *ctx, napi_value result)
{
    napi_value businessError = nullptr;
    if (ctx->result != HCF_SUCCESS) {
        businessError = GenerateBusinessError(env, ctx->result, COMMON_ERR_MSG.c_str(), false);
    }

    napi_value params[ARGS_SIZE_TWO] = { businessError, result };

    napi_value func = nullptr;
    napi_get_reference_value(env, ctx->callback, &func);

    napi_value recv = nullptr;
    napi_value callFuncRet = nullptr;
    napi_get_undefined(env, &recv);
    napi_call_function(env, recv, func, ARGS_SIZE_TWO, params, &callFuncRet);
}

static void ReturnPromiseResult(napi_env env, KeyAgreementCtx *ctx, napi_value result)
{
    if (ctx->result == HCF_SUCCESS) {
        napi_resolve_deferred(env, ctx->deferred, result);
    } else {
        napi_reject_deferred(env, ctx->deferred,
            GenerateBusinessError(env, ctx->result, COMMON_ERR_MSG.c_str(), false));
    }
}

void KeyAgreementAsyncWorkProcess(napi_env env, void *data)
{
    KeyAgreementCtx *ctx = static_cast<KeyAgreementCtx *>(data);

    HcfResult res = ctx->keyAgreement->generateSecret(ctx->keyAgreement,
        ctx->priKey, ctx->pubKey, &ctx->returnSecret);

    ctx->result = res;
    if (res != HCF_SUCCESS) {
        LOGE("generate secret fail.");
    }
}

void KeyAgreementAsyncWorkReturn(napi_env env, napi_status status, void *data)
{
    KeyAgreementCtx *ctx = static_cast<KeyAgreementCtx *>(data);

    napi_value dataBlob = nullptr;
    if (ctx->result == HCF_SUCCESS) {
        dataBlob = ConvertBlobToNapiValue(env, &ctx->returnSecret);
    }

    if (ctx->asyncType == ASYNC_CALLBACK) {
        ReturnCallbackResult(env, ctx, dataBlob);
    } else {
        ReturnPromiseResult(env, ctx, dataBlob);
    }
    FreeKeyAgreementCtx(env, ctx);
}

static napi_value NewKeyAgreementAsyncWork(napi_env env, KeyAgreementCtx *ctx)
{
    napi_value resourceName = nullptr;
    napi_create_string_utf8(env, "generateSecret", NAPI_AUTO_LENGTH, &resourceName);

    napi_create_async_work(
        env, nullptr, resourceName,
        [](napi_env env, void *data) {
            KeyAgreementAsyncWorkProcess(env, data);
            return;
        },
        [](napi_env env, napi_status status, void *data) {
            KeyAgreementAsyncWorkReturn(env, status, data);
            return;
        },
        static_cast<void *>(ctx),
        &ctx->asyncWork);

    napi_queue_async_work(env, ctx->asyncWork);
    if (ctx->asyncType == ASYNC_PROMISE) {
        return ctx->promise;
    } else {
        napi_value result = nullptr;
        napi_get_null(env, &result);
        return result;
    }
}

NapiKeyAgreement::NapiKeyAgreement(HcfKeyAgreement *keyAgreement)
{
    this->keyAgreement_ = keyAgreement;
}

NapiKeyAgreement::~NapiKeyAgreement()
{
    HcfObjDestroy(this->keyAgreement_);
}

HcfKeyAgreement *NapiKeyAgreement::GetKeyAgreement()
{
    return this->keyAgreement_;
}

napi_value NapiKeyAgreement::JsGenerateSecret(napi_env env, napi_callback_info info)
{
    LOGI("enter ...");
    KeyAgreementCtx *ctx = static_cast<KeyAgreementCtx *>(HcfMalloc(sizeof(KeyAgreementCtx), 0));
    if (ctx == nullptr) {
        LOGE("create context fail.");
        return nullptr;
    }

    if (!BuildKeyAgreementJsCtx(env, info, ctx)) {
        LOGE("build context fail.");
        FreeKeyAgreementCtx(env, ctx);
        return nullptr;
    }

    return NewKeyAgreementAsyncWork(env, ctx);
}

napi_value NapiKeyAgreement::KeyAgreementConstructor(napi_env env, napi_callback_info info)
{
    LOGI("enter ...");

    napi_value thisVar = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);

    LOGI("out ...");
    return thisVar;
}

napi_value NapiKeyAgreement::CreateJsKeyAgreement(napi_env env, napi_callback_info info)
{
    LOGI("enter ...");
    size_t expectedArgc = PARAMS_NUM_ONE;
    size_t argc = PARAMS_NUM_ONE;
    napi_value argv[PARAMS_NUM_ONE] = { nullptr };
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);

    if (argc != expectedArgc) {
        LOGE("The input args num is invalid.");
        return nullptr;
    }

    napi_value instance;
    napi_value constructor = nullptr;
    napi_get_reference_value(env, classRef_, &constructor);
    napi_new_instance(env, constructor, argc, argv, &instance);

    std::string algName;
    if (!GetStringFromJSParams(env, argv[0], algName, false)) {
        return nullptr;
    }

    HcfKeyAgreement *keyAgreement = nullptr;
    int32_t res = HcfKeyAgreementCreate(algName.c_str(), &keyAgreement);
    if (res != HCF_SUCCESS) {
        LOGE("create c keyAgreement fail.");
        return nullptr;
    }

    NapiKeyAgreement *napiKeyAgreement = new NapiKeyAgreement(keyAgreement);

    napi_wrap(
        env, instance, napiKeyAgreement,
        [](napi_env env, void *data, void *hint) {
            NapiKeyAgreement *napiKeyAgreement = static_cast<NapiKeyAgreement *>(data);
            delete napiKeyAgreement;
            return;
        },
        nullptr,
        nullptr);

    napi_value napiAlgName = nullptr;
    napi_create_string_utf8(env, algName.c_str(), NAPI_AUTO_LENGTH, &napiAlgName);
    napi_set_named_property(env, instance, CRYPTO_TAG_ALG_NAME.c_str(), napiAlgName);

    LOGI("out ...");
    return instance;
}

void NapiKeyAgreement::DefineKeyAgreementJSClass(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("createKeyAgreement", NapiKeyAgreement::CreateJsKeyAgreement),
    };
    napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc);

    napi_property_descriptor classDesc[] = {
        DECLARE_NAPI_FUNCTION("generateSecret", NapiKeyAgreement::JsGenerateSecret),
    };
    napi_value constructor = nullptr;
    napi_define_class(env, "KeyAgreement", NAPI_AUTO_LENGTH, NapiKeyAgreement::KeyAgreementConstructor, nullptr,
        sizeof(classDesc) / sizeof(classDesc[0]), classDesc, &constructor);
    napi_create_reference(env, constructor, 1, &classRef_);
}
} // CryptoFramework
} // OHOS
