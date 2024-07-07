/*
 * Copyright (C) 2022-2023 Huawei Device Co., Ltd.
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

#include "napi_rand.h"

#include "securec.h"
#include "log.h"
#include "memory.h"

#include "napi_utils.h"
#include "napi_crypto_framework_defines.h"

namespace OHOS {
namespace CryptoFramework {
thread_local napi_ref NapiRand::classRef_ = nullptr;

struct RandCtx {
    napi_env env = nullptr;

    CfAsyncType asyncType = ASYNC_TYPE_CALLBACK;
    napi_ref callback = nullptr;
    napi_deferred deferred = nullptr;
    napi_value promise = nullptr;
    napi_async_work asyncWork = nullptr;

    NapiRand *randClass = nullptr;
    uint32_t numBytes = 0;
    HcfBlob *seedBlob = nullptr;

    HcfResult errCode = HCF_SUCCESS;
    const char *errMsg = nullptr;
    HcfBlob *randBlob = nullptr;
};

static void FreeCryptoFwkCtx(napi_env env, RandCtx *context)
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
    if (context->seedBlob != nullptr) {
        HcfFree(context->seedBlob->data);
        context->seedBlob->data = nullptr;
        context->seedBlob->len = 0;
        HcfFree(context->seedBlob);
        context->seedBlob = nullptr;
    }
    if (context->randBlob != nullptr) {
        HcfFree(context->randBlob->data);
        context->randBlob->data = nullptr;
        context->randBlob->len = 0;
        HcfFree(context->randBlob);
        context->randBlob = nullptr;
    }
    context->errMsg = nullptr;
    HcfFree(context);
    context = nullptr;
}

static void ReturnCallbackResult(napi_env env, RandCtx *context, napi_value result)
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

static void ReturnPromiseResult(napi_env env, RandCtx *context, napi_value result)
{
    if (context->errCode == HCF_SUCCESS) {
        napi_resolve_deferred(env, context->deferred, result);
    } else {
        napi_reject_deferred(env, context->deferred,
            GenerateBusinessError(env, context->errCode, context->errMsg, false));
    }
}

static bool CreateCallbackAndPromise(napi_env env, RandCtx *context, size_t argc,
    size_t maxCount, napi_value callbackValue)
{
    context->asyncType = (argc == maxCount) ? ASYNC_TYPE_CALLBACK : ASYNC_TYPE_PROMISE;
    if (context->asyncType == ASYNC_TYPE_CALLBACK) {
        if (!GetCallbackFromJSParams(env, callbackValue, &context->callback, false)) {
            LOGE("get callback failed!");
            return false;
        }
    } else {
        napi_create_promise(env, &context->deferred, &context->promise);
    }
    return true;
}

NapiRand::NapiRand(HcfRand *randObj)
{
    this->randObj_ = randObj;
}

NapiRand::~NapiRand()
{
    HcfObjDestroy(this->randObj_);
}

static void GenerateRandomExecute(napi_env env, void *data)
{
    RandCtx *context = static_cast<RandCtx *>(data);
    NapiRand *randClass = context->randClass;
    HcfRand *randObj = randClass->GetRand();
    HcfBlob *randBlob = reinterpret_cast<HcfBlob *>(HcfMalloc(sizeof(HcfBlob), 0));
    if (randBlob == nullptr) {
        LOGE("randBlob is null!");
        context->errCode = HCF_ERR_MALLOC;
        context->errMsg = "malloc data blob failed";
        return;
    }
    uint32_t numBytes = context->numBytes;
    context->errCode = randObj->generateRandom(randObj, numBytes, randBlob);
    if (context->errCode != HCF_SUCCESS) {
        LOGE("generateRandom failed!");
        context->errMsg = "generateRandom failed";
        HcfFree(randBlob);
        randBlob = nullptr;
        return;
    }
    context->randBlob = randBlob;
}

static void GenerateRandomComplete(napi_env env, napi_status status, void *data)
{
    RandCtx *context = static_cast<RandCtx *>(data);
    napi_value returnRandBlob = ConvertBlobToNapiValue(env, context->randBlob);
    if (returnRandBlob == nullptr) {
        LOGE("returnOutBlob is nullptr!");
        returnRandBlob = NapiGetNull(env);
    }
    if (context->asyncType == ASYNC_TYPE_CALLBACK) {
        ReturnCallbackResult(env, context, returnRandBlob);
    } else {
        ReturnPromiseResult(env, context, returnRandBlob);
    }
    FreeCryptoFwkCtx(env, context);
}

napi_value NapiRand::GenerateRandom(napi_env env, napi_callback_info info)
{
    size_t expectedArgsCount = ARGS_SIZE_TWO;
    size_t argc = expectedArgsCount;
    napi_value argv[ARGS_SIZE_TWO] = { nullptr };
    napi_value thisVar = nullptr;
    napi_value ret = NapiGetNull(env);
    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if ((argc != expectedArgsCount) && (argc != expectedArgsCount - CALLBACK_SIZE)) {
        napi_throw(env, GenerateBusinessError(env, HCF_INVALID_PARAMS, "invalid params count", false));
        LOGE("The arguments count is not expected!");
        return ret;
    }
    RandCtx *context = static_cast<RandCtx *>(HcfMalloc(sizeof(RandCtx), 0));
    if (context == nullptr) {
        napi_throw(env, GenerateBusinessError(env, HCF_ERR_MALLOC, "malloc context failed", false));
        LOGE("malloc context failed!");
        return ret;
    }
    context->randClass = this;
    if (!GetUint32FromJSParams(env, argv[PARAM0], context->numBytes, false)) {
        LOGE("get numBytes failed!");
        FreeCryptoFwkCtx(env, context);
        return ret;
    }
    if (!CreateCallbackAndPromise(env, context, argc, ARGS_SIZE_TWO, argv[PARAM1])) {
        FreeCryptoFwkCtx(env, context);
        return nullptr;
    }
    napi_create_async_work(
        env, nullptr, GetResourceName(env, "GenerateRandom"),
        GenerateRandomExecute,
        GenerateRandomComplete,
        static_cast<void *>(context),
        &context->asyncWork);
    napi_queue_async_work(env, context->asyncWork);
    if (context->asyncType == ASYNC_TYPE_PROMISE) {
        return context->promise;
    } else {
        return NapiGetNull(env);
    }
}

napi_value NapiRand::SetSeed(napi_env env, napi_callback_info info)
{
    size_t expectedArgsCount = ARGS_SIZE_ONE;
    size_t argc = expectedArgsCount;
    napi_value argv[ARGS_SIZE_ONE] = { nullptr };
    napi_value thisVar = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (argc != expectedArgsCount) {
        napi_throw(env, GenerateBusinessError(env, HCF_INVALID_PARAMS, "invalid params count", false));
        LOGE("The arguments count is not expected!");
        return nullptr;
    }
    HcfBlob *seedBlob = GetBlobFromNapiValue(env, argv[PARAM0]);
    HcfRand *randObj = GetRand();
    HcfResult res = randObj->setSeed(randObj, seedBlob);
    if (res != HCF_SUCCESS) {
        napi_throw(env, GenerateBusinessError(env, res, "set seed failed.", false));
        LOGE("set seed failed.");
    }
    return nullptr;
}

static napi_value NapiGenerateRandom(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);
    NapiRand *randObj = nullptr;
    napi_unwrap(env, thisVar, reinterpret_cast<void **>(&randObj));
    if (randObj == nullptr) {
        LOGE("randObj is nullptr!");
        return NapiGetNull(env);
    }
    return randObj->GenerateRandom(env, info);
}

static napi_value NapiSetSeed(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);
    NapiRand *randObj = nullptr;
    napi_unwrap(env, thisVar, reinterpret_cast<void **>(&randObj));
    if (randObj == nullptr) {
        LOGE("randObj is nullptr!");
        return nullptr;
    }
    return randObj->SetSeed(env, info);
}

napi_value NapiRand::RandConstructor(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);
    return thisVar;
}

napi_value NapiRand::CreateRand(napi_env env, napi_callback_info info)
{
    HcfRand *randObj = nullptr;
    HcfResult res = HcfRandCreate(&randObj);
    if (res != HCF_SUCCESS) {
        napi_throw(env, GenerateBusinessError(env, res, "create C obj failed.", false));
        LOGE("create c randObj failed.");
        return nullptr;
    }
    napi_value instance = nullptr;
    napi_value constructor = nullptr;
    napi_get_reference_value(env, classRef_, &constructor);
    napi_new_instance(env, constructor, 0, nullptr, &instance);
    NapiRand *randNapiObj = new (std::nothrow) NapiRand(randObj);
    if (randNapiObj == nullptr) {
        LOGE("create napi obj failed");
        return nullptr;
    }
    napi_wrap(
        env, instance, randNapiObj,
        [](napi_env env, void *data, void *hint) {
            NapiRand *rand = static_cast<NapiRand *>(data);
            delete rand;
            return;
        },
        nullptr,
        nullptr);
    return instance;
}

void NapiRand::DefineRandJSClass(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("createRandom", CreateRand),
    };
    napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc);
    napi_property_descriptor classDesc[] = {
        DECLARE_NAPI_FUNCTION("generateRandom", NapiGenerateRandom),
        DECLARE_NAPI_FUNCTION("setSeed", NapiSetSeed),
    };
    napi_value constructor = nullptr;
    napi_define_class(env, "Random", NAPI_AUTO_LENGTH, RandConstructor, nullptr,
        sizeof(classDesc) / sizeof(classDesc[0]), classDesc, &constructor);
    napi_create_reference(env, constructor, 1, &classRef_);
}
} // CryptoFramework
} // OHOS
