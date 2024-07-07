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

#include "napi_sym_key_generator.h"

#include "securec.h"
#include "log.h"
#include "memory.h"
#include "napi_sym_key.h"
#include "napi_utils.h"
#include "napi_crypto_framework_defines.h"

namespace OHOS {
namespace CryptoFramework {
thread_local napi_ref NapiSymKeyGenerator::classRef_ = nullptr;

struct SymKeyGeneratorFwkCtxT {
    napi_env env = nullptr;
    AsyncType asyncType = ASYNC_CALLBACK;
    napi_ref callback = nullptr;
    napi_deferred deferred = nullptr;
    napi_value promise = nullptr;
    napi_async_work asyncWork = nullptr;
	
    int32_t errCode = 0;
    HcfSymKey *returnSymKey = nullptr;
    const char *errMsg = nullptr;

    HcfSymKeyGenerator *generator = nullptr;
    HcfBlob keyMaterial = { .data = nullptr, .len = 0 };
};

using SymKeyGeneratorFwkCtx = SymKeyGeneratorFwkCtxT *;

static void FreeSymKeyGeneratorFwkCtx(napi_env env, SymKeyGeneratorFwkCtx &context)
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

    if (context->keyMaterial.data != nullptr) {
        (void)memset_s(context->keyMaterial.data, context->keyMaterial.len, 0, context->keyMaterial.len);
        HcfFree(context->keyMaterial.data);
        context->keyMaterial.data = nullptr;
        context->keyMaterial.len = 0;
    }
    context->errMsg = nullptr;
    
    HcfFree(context);
    context = nullptr;
}

static bool BuildContextForGenerateKey(napi_env env, napi_callback_info info, SymKeyGeneratorFwkCtx context)
{
    napi_value thisVar = nullptr;
    size_t expectedArgc = ARGS_SIZE_ONE;
    size_t argc = ARGS_SIZE_ONE;
    napi_value argv[ARGS_SIZE_ONE] = { nullptr };
    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (argc != expectedArgc && argc != expectedArgc - 1) {
        napi_throw(env, GenerateBusinessError(env, HCF_INVALID_PARAMS,
            "generate key failed for wrong argument num.", false));
        LOGE("wrong argument num. require 0 or 1 arguments. [Argc]: %zu!", argc);
        return false;
    }
    context->asyncType = (argc == expectedArgc) ? ASYNC_CALLBACK : ASYNC_PROMISE;
    NapiSymKeyGenerator *napiGenerator;
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&napiGenerator));
    if (status != napi_ok) {
        LOGE("failed to unwrap NapiSymKeyGenerator obj!");
        return false;
    }

    context->generator = napiGenerator->GetSymKeyGenerator();
    if (context->generator == nullptr) {
        LOGE("failed to get generator obj!");
        return false;
    }
    if (context->asyncType == ASYNC_PROMISE) {
        napi_create_promise(env, &context->deferred, &context->promise);
        return true;
    } else {
        return GetCallbackFromJSParams(env, argv[ARGS_SIZE_ZERO], &context->callback, false);
    }
}

static bool BuildContextForConvertKey(napi_env env, napi_callback_info info, SymKeyGeneratorFwkCtx context)
{
    napi_value thisVar = nullptr;
    size_t expectedArgc = ARGS_SIZE_TWO;
    size_t argc = ARGS_SIZE_TWO;
    napi_value argv[ARGS_SIZE_TWO] = { nullptr };
    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (argc != expectedArgc && argc != expectedArgc - 1) {
        napi_throw(env, GenerateBusinessError(env, HCF_INVALID_PARAMS,
            "convert key failed for wrong argument num.", false));
        LOGE("wrong argument num. require 1 or 2 arguments. [Argc]: %zu!", argc);
        return false;
    }
    context->asyncType = (argc == expectedArgc) ? ASYNC_CALLBACK : ASYNC_PROMISE;

    NapiSymKeyGenerator *napiGenerator;
    napi_status status = napi_unwrap(env, thisVar, reinterpret_cast<void **>(&napiGenerator));
    if (status != napi_ok) {
        LOGE("failed to unwrap NapiSymKeyGenerator obj!");
        return false;
    }

    context->generator = napiGenerator->GetSymKeyGenerator();
    if (context->generator == nullptr) {
        LOGE("failed to get generator obj!");
        return false;
    }

    size_t index = 0;
    HcfBlob *blob = GetBlobFromNapiValue(env, argv[index++]);
    if (blob == nullptr) {
        napi_throw(env, GenerateBusinessError(env, HCF_INVALID_PARAMS,
            "convert key failed for invalid input blob.", false));
        LOGE("get keyMaterial failed!");
        return false;
    }
    context->keyMaterial = *blob;
    HcfFree(blob);
    if (context->asyncType == ASYNC_PROMISE) {
        napi_create_promise(env, &context->deferred, &context->promise);
        return true;
    } else {
        return GetCallbackFromJSParams(env, argv[index], &context->callback, false);
    }
}

static void ReturnPromiseResult(napi_env env, SymKeyGeneratorFwkCtx context, napi_value result)
{
    if (context->errCode == HCF_SUCCESS) {
        napi_resolve_deferred(env, context->deferred, result);
    } else {
        napi_reject_deferred(env, context->deferred,
            GenerateBusinessError(env, context->errCode, context->errMsg, false));
    }
}

static void ReturnCallbackResult(napi_env env, SymKeyGeneratorFwkCtx context, napi_value result)
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

static void AsyncGenKeyProcess(napi_env env, void *data)
{
    SymKeyGeneratorFwkCtx context = static_cast<SymKeyGeneratorFwkCtx>(data);
    HcfSymKeyGenerator *generator = context->generator;

    HcfSymKey *key = nullptr;
    HcfResult res = generator->generateSymKey(generator, &key);
    if (res != HCF_SUCCESS) {
        LOGE("generate sym key failed.");
        context->errCode = res;
        context->errMsg = "generate sym key failed.";
        return;
    }

    context->errCode = HCF_SUCCESS;
    context->returnSymKey = key;
}

static void AsyncKeyReturn(napi_env env, napi_status status, void *data)
{
    napi_value instance = NapiSymKey::CreateSymKey(env);
    SymKeyGeneratorFwkCtx context = static_cast<SymKeyGeneratorFwkCtx>(data);
    NapiSymKey *napiSymKey = new (std::nothrow) NapiSymKey(context->returnSymKey);
    if (napiSymKey == nullptr) {
        napi_throw(env, GenerateBusinessError(env, HCF_ERR_MALLOC, "new napi sym key failed.", false));
        FreeSymKeyGeneratorFwkCtx(env, context);
        LOGE("new napi sym key failed.");
        return;
    }

    napi_status ret = napi_wrap(env, instance, napiSymKey,
        [](napi_env env, void *data, void *hint) {
            NapiSymKey *napiSymKey = static_cast<NapiSymKey *>(data);
            delete napiSymKey;
            return;
        },
        nullptr, nullptr);
    if (ret != napi_ok) {
        LOGE("failed to wrap napiSymKey obj!");
        context->errCode = HCF_INVALID_PARAMS;
        context->errMsg = "failed to wrap napiSymKey obj!";
        delete napiSymKey;
    }

    if (context->asyncType == ASYNC_CALLBACK) {
        ReturnCallbackResult(env, context, instance);
    } else {
        ReturnPromiseResult(env, context, instance);
    }
    FreeSymKeyGeneratorFwkCtx(env, context);
}

static void AsyncConvertKeyProcess(napi_env env, void *data)
{
    SymKeyGeneratorFwkCtx context = static_cast<SymKeyGeneratorFwkCtx>(data);
    HcfSymKeyGenerator *generator = context->generator;

    HcfSymKey *key = nullptr;
    HcfResult res = generator->convertSymKey(generator, &context->keyMaterial, &key);
    if (res != HCF_SUCCESS) {
        LOGE("convertSymKey key failed!");
        context->errCode = res;
        context->errMsg = "convert sym key failed.";
        return;
    }

    context->errCode = HCF_SUCCESS;
    context->returnSymKey = key;
}

static napi_value NewConvertKeyAsyncWork(napi_env env, SymKeyGeneratorFwkCtx context)
{
    napi_value resourceName = nullptr;
    napi_create_string_utf8(env, "convertSymKey", NAPI_AUTO_LENGTH, &resourceName);

    napi_create_async_work(
        env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            AsyncConvertKeyProcess(env, data);
            return;
        },
        [](napi_env env, napi_status status, void *data) {
            AsyncKeyReturn(env, status, data);
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

static napi_value NewGenKeyAsyncWork(napi_env env, SymKeyGeneratorFwkCtx context)
{
    napi_value resourceName = nullptr;
    napi_create_string_utf8(env, "generatorSymKey", NAPI_AUTO_LENGTH, &resourceName);

    napi_create_async_work(
        env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            AsyncGenKeyProcess(env, data);
            return;
        },
        [](napi_env env, napi_status status, void *data) {
            AsyncKeyReturn(env, status, data);
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

NapiSymKeyGenerator::NapiSymKeyGenerator(HcfSymKeyGenerator *generator)
{
    this->generator_ = generator;
}

NapiSymKeyGenerator::~NapiSymKeyGenerator()
{
    HcfObjDestroy(this->generator_);
}

HcfSymKeyGenerator *NapiSymKeyGenerator::GetSymKeyGenerator() const
{
    return this->generator_;
}

napi_value NapiSymKeyGenerator::JsGenerateSymKey(napi_env env, napi_callback_info info)
{
    SymKeyGeneratorFwkCtx context = static_cast<SymKeyGeneratorFwkCtx>(HcfMalloc(sizeof(SymKeyGeneratorFwkCtxT), 0));
    if (context == nullptr) {
        LOGE("Create context failed!");
        return nullptr;
    }

    if (!BuildContextForGenerateKey(env, info, context)) {
        LOGE("Build context fail.");
        FreeSymKeyGeneratorFwkCtx(env, context);
        return nullptr;
    }

    napi_value result = NewGenKeyAsyncWork(env, context);
    if (result == nullptr) {
        LOGE("NewGenKeyAsyncWork failed!");
        FreeSymKeyGeneratorFwkCtx(env, context);
        return nullptr;
    }
    return result;
}

napi_value NapiSymKeyGenerator::JsConvertKey(napi_env env, napi_callback_info info)
{
    SymKeyGeneratorFwkCtx context = static_cast<SymKeyGeneratorFwkCtx>(HcfMalloc(sizeof(SymKeyGeneratorFwkCtxT), 0));
    if (context == nullptr) {
        LOGE("malloc SymKeyGeneratorFwkCtx failed!");
        return nullptr;
    }

    if (!BuildContextForConvertKey(env, info, context)) {
        LOGE("BuildContextForConvertKey failed!");
        FreeSymKeyGeneratorFwkCtx(env, context);
        return nullptr;
    }

    napi_value result = NewConvertKeyAsyncWork(env, context);
    if (result == nullptr) {
        LOGE("Get deviceauth async work failed!");
        FreeSymKeyGeneratorFwkCtx(env, context);
        return nullptr;
    }
    return result;
}

napi_value NapiSymKeyGenerator::SymKeyGeneratorConstructor(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr));
    return thisVar;
}

napi_value NapiSymKeyGenerator::CreateSymKeyGenerator(napi_env env, napi_callback_info info)
{
    size_t expectedArgc = ARGS_SIZE_ONE;
    size_t argc = ARGS_SIZE_ONE;
    napi_value argv[ARGS_SIZE_ONE] = { nullptr };
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);

    if (argc != expectedArgc) {
        napi_throw(env, GenerateBusinessError(env, HCF_INVALID_PARAMS, "The input args num is invalid.", false));
        LOGE("The input args num is invalid.");
        return nullptr;
    }

    napi_value instance;
    napi_value constructor = nullptr;
    NAPI_CALL(env, napi_get_reference_value(env, classRef_, &constructor));
    NAPI_CALL(env, napi_new_instance(env, constructor, argc, argv, &instance));

    std::string algoName;
    if (!GetStringFromJSParams(env, argv[ARGS_SIZE_ZERO], algoName, false)) {
        LOGE("failed to get algoName.");
        return nullptr;
    }

    HcfSymKeyGenerator *generator = nullptr;
    int32_t res = HcfSymKeyGeneratorCreate(algoName.c_str(), &generator);
    if (res != HCF_SUCCESS) {
        napi_throw(env, GenerateBusinessError(env, res, "create C generator fail.", false));
        LOGE("create C generator fail.");
        return nullptr;
    }
    NapiSymKeyGenerator *napiSymKeyGenerator = new (std::nothrow) NapiSymKeyGenerator(generator);
    if (napiSymKeyGenerator == nullptr) {
        LOGE("new napiSymKeyGenerator failed!");
        HcfObjDestroy(generator);
        return nullptr;
    }

    napi_status status = napi_wrap(env, instance, napiSymKeyGenerator,
        [](napi_env env, void *data, void *hint) {
            NapiSymKeyGenerator *napiSymKeyGenerator = static_cast<NapiSymKeyGenerator *>(data);
            delete napiSymKeyGenerator;
            return;
        },
        nullptr,
        nullptr);
    if (status != napi_ok) {
        LOGE("failed to wrap napiSymKeyGenerator obj!");
        delete napiSymKeyGenerator;
        return nullptr;
    }
    return instance;
}

napi_value NapiSymKeyGenerator::JsGetAlgorithm(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    NapiSymKeyGenerator *napiSymKeyGenerator = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr));
    NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void **>(&napiSymKeyGenerator)));
    HcfSymKeyGenerator *generator = napiSymKeyGenerator->GetSymKeyGenerator();

    const char *algo = generator->getAlgoName(generator);
    napi_value instance = nullptr;
    napi_create_string_utf8(env, algo, NAPI_AUTO_LENGTH, &instance);
    return instance;
}

void NapiSymKeyGenerator::DefineSymKeyGeneratorJSClass(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("createSymKeyGenerator", NapiSymKeyGenerator::CreateSymKeyGenerator),
    };
    napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc);

    napi_property_descriptor classDesc[] = {
        DECLARE_NAPI_FUNCTION("generateSymKey", NapiSymKeyGenerator::JsGenerateSymKey),
        DECLARE_NAPI_FUNCTION("convertKey", NapiSymKeyGenerator::JsConvertKey),
        { .utf8name = "algName", .getter = NapiSymKeyGenerator::JsGetAlgorithm },
    };
    napi_value constructor = nullptr;
    napi_define_class(env, "SymKeyGenerator", NAPI_AUTO_LENGTH, NapiSymKeyGenerator::SymKeyGeneratorConstructor,
        nullptr, sizeof(classDesc) / sizeof(classDesc[0]), classDesc, &constructor);
    napi_create_reference(env, constructor, 1, &classRef_);
}
} // CryptoFramework
} // OHOS