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

#include "napi_md.h"

#include "securec.h"
#include "log.h"
#include "memory.h"

#include "napi_utils.h"
#include "napi_crypto_framework_defines.h"

namespace OHOS {
namespace CryptoFramework {
thread_local napi_ref NapiMd::classRef_ = nullptr;

struct MdCtx {
    napi_env env = nullptr;

    CfAsyncType asyncType = ASYNC_TYPE_CALLBACK;
    napi_ref callback = nullptr;
    napi_deferred deferred = nullptr;
    napi_value promise = nullptr;

    napi_async_work asyncWork = nullptr;

    NapiMd *mdClass = nullptr;
    std::string algoName = "";
    HcfBlob *inBlob = nullptr;

    HcfResult errCode = HCF_SUCCESS;
    const char *errMsg = nullptr;
    HcfBlob *outBlob = nullptr;
};

static void FreeCryptoFwkCtx(napi_env env, MdCtx *context)
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
    if (context->inBlob != nullptr) {
        HcfFree(context->inBlob->data);
        context->inBlob->data = nullptr;
        context->inBlob->len = 0;
        HcfFree(context->inBlob);
        context->inBlob = nullptr;
    }
    if (context->outBlob != nullptr) {
        HcfFree(context->outBlob->data);
        context->outBlob->data = nullptr;
        context->outBlob->len = 0;
        HcfFree(context->outBlob);
        context->outBlob = nullptr;
    }
    context->errMsg = nullptr;
    HcfFree(context);
    context = nullptr;
}

static void ReturnCallbackResult(napi_env env, MdCtx *context, napi_value result)
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

static void ReturnPromiseResult(napi_env env, MdCtx *context, napi_value result)
{
    if (context->errCode == HCF_SUCCESS) {
        napi_resolve_deferred(env, context->deferred, result);
    } else {
        napi_reject_deferred(env, context->deferred,
            GenerateBusinessError(env, context->errCode, context->errMsg, false));
    }
}

static bool CreateCallbackAndPromise(napi_env env, MdCtx *context, size_t argc,
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

NapiMd::NapiMd(HcfMd *mdObj)
{
    this->mdObj_ = mdObj;
}

NapiMd::~NapiMd()
{
    HcfObjDestroy(this->mdObj_);
}

static void MdUpdateExecute(napi_env env, void *data)
{
    MdCtx *context = static_cast<MdCtx *>(data);
    NapiMd *mdClass = context->mdClass;
    HcfMd *mdObj = mdClass->GetMd();
    context->errCode = mdObj->update(mdObj, context->inBlob);
    if (context->errCode != HCF_SUCCESS) {
        LOGE("update failed!");
        context->errMsg = "update failed";
    }
}

static void MdUpdateComplete(napi_env env, napi_status status, void *data)
{
    MdCtx *context = static_cast<MdCtx *>(data);
    napi_value nullInstance = nullptr;
    napi_get_null(env, &nullInstance);
    if (context->asyncType == ASYNC_TYPE_CALLBACK) {
        ReturnCallbackResult(env, context, nullInstance);
    } else {
        ReturnPromiseResult(env, context, nullInstance);
    }
    FreeCryptoFwkCtx(env, context);
}

static void MdDoFinalExecute(napi_env env, void *data)
{
    MdCtx *context = static_cast<MdCtx *>(data);
    NapiMd *mdClass = context->mdClass;
    HcfMd *mdObj = mdClass->GetMd();
    HcfBlob *outBlob = reinterpret_cast<HcfBlob *>(HcfMalloc(sizeof(HcfBlob), 0));
    if (outBlob == nullptr) {
        LOGE("outBlob is null!");
        context->errCode = HCF_ERR_MALLOC;
        context->errMsg = "malloc data blob failed";
        return;
    }
    context->errCode = mdObj->doFinal(mdObj, outBlob);
    if (context->errCode != HCF_SUCCESS) {
        LOGE("doFinal failed!");
        context->errMsg = "doFinal failed";
        HcfFree(outBlob);
        outBlob = nullptr;
        return;
    }
    context->outBlob = outBlob;
}

static void MdDoFinalComplete(napi_env env, napi_status status, void *data)
{
    MdCtx *context = static_cast<MdCtx *>(data);
    napi_value returnOutBlob = ConvertBlobToNapiValue(env, context->outBlob);
    if (returnOutBlob == nullptr) {
        LOGE("returnOutBlob is nullptr!");
        returnOutBlob = NapiGetNull(env);
    }
    if (context->asyncType == ASYNC_TYPE_CALLBACK) {
        ReturnCallbackResult(env, context, returnOutBlob);
    } else {
        ReturnPromiseResult(env, context, returnOutBlob);
    }
    FreeCryptoFwkCtx(env, context);
}

napi_value NapiMd::MdUpdate(napi_env env, napi_callback_info info)
{
    // check param count
    size_t argc = ARGS_SIZE_TWO;
    napi_value argv[ARGS_SIZE_TWO] = { nullptr };
    napi_value thisVar = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (!CheckArgsCount(env, argc, ARGS_SIZE_TWO, false, false)) {
        return nullptr;
    }
    MdCtx *context = static_cast<MdCtx *>(HcfMalloc(sizeof(MdCtx), 0));
    if (context == nullptr) {
        napi_throw(env, GenerateBusinessError(env, HCF_ERR_MALLOC, "malloc context failed", false));
        LOGE("malloc context failed!");
        return nullptr;
    }
    context->mdClass = this;
    context->inBlob = GetBlobFromNapiValue(env, argv[PARAM0]);
    if (context->inBlob == nullptr) {
        napi_throw(env, GenerateBusinessError(env, HCF_INVALID_PARAMS, "inBlob is null", false));
        LOGE("inBlob is null!");
        FreeCryptoFwkCtx(env, context);
        return nullptr;
    }
    if (!CreateCallbackAndPromise(env, context, argc, ARGS_SIZE_TWO, argv[PARAM1])) {
        FreeCryptoFwkCtx(env, context);
        return nullptr;
    }
    napi_create_async_work(
        env, nullptr, GetResourceName(env, "MdUpdate"),
        MdUpdateExecute,
        MdUpdateComplete,
        static_cast<void *>(context),
        &context->asyncWork);
    napi_queue_async_work(env, context->asyncWork);
    if (context->asyncType == ASYNC_TYPE_PROMISE) {
        return context->promise;
    } else {
        return NapiGetNull(env);
    }
}

napi_value NapiMd::MdDoFinal(napi_env env, napi_callback_info info)
{
    size_t expectedArgsCount = ARGS_SIZE_ONE;
    size_t argc = expectedArgsCount;
    napi_value argv[ARGS_SIZE_ONE] = { nullptr };
    napi_value thisVar = nullptr;
    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    if (!CheckArgsCount(env, argc, ARGS_SIZE_ONE, false, false)) {
        return nullptr;
    }
    MdCtx *context = static_cast<MdCtx *>(HcfMalloc(sizeof(MdCtx), 0));
    if (context == nullptr) {
        napi_throw(env, GenerateBusinessError(env, HCF_ERR_MALLOC, "malloc context failed", false));
        LOGE("malloc context failed!");
        return nullptr;
    }
    context->mdClass = this;
    if (!CreateCallbackAndPromise(env, context, argc, ARGS_SIZE_ONE, argv[PARAM0])) {
        FreeCryptoFwkCtx(env, context);
        return nullptr;
    }
    napi_create_async_work(
        env, nullptr, GetResourceName(env, "MdDoFinal"),
        MdDoFinalExecute,
        MdDoFinalComplete,
        static_cast<void *>(context),
        &context->asyncWork);
    napi_queue_async_work(env, context->asyncWork);
    if (context->asyncType == ASYNC_TYPE_PROMISE) {
        return context->promise;
    } else {
        return NapiGetNull(env);
    }
}

napi_value NapiMd::GetMdLength(napi_env env, napi_callback_info info)
{
    HcfMd *mdObj = GetMd();
    uint32_t retLen = mdObj->getMdLength(mdObj);
    napi_value napiLen = nullptr;
    napi_create_uint32(env, retLen, &napiLen);
    return napiLen;
}

static napi_value NapiMdUpdate(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);
    NapiMd *mdObj = nullptr;
    napi_unwrap(env, thisVar, reinterpret_cast<void **>(&mdObj));
    if (mdObj == nullptr) {
        LOGE("mdObj is nullptr!");
        return NapiGetNull(env);
    }
    return mdObj->MdUpdate(env, info);
}

static napi_value NapiMdDoFinal(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);
    NapiMd *mdObj = nullptr;
    napi_unwrap(env, thisVar, reinterpret_cast<void **>(&mdObj));
    if (mdObj == nullptr) {
        LOGE("mdObj is nullptr!");
        return NapiGetNull(env);
    }
    return mdObj->MdDoFinal(env, info);
}

static napi_value NapiGetMdLength(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);
    NapiMd *mdObj = nullptr;
    napi_unwrap(env, thisVar, reinterpret_cast<void **>(&mdObj));
    if (mdObj == nullptr) {
        LOGE("mdObj is nullptr!");
        return NapiGetNull(env);
    }
    return mdObj->GetMdLength(env, info);
}

napi_value NapiMd::MdConstructor(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);
    return thisVar;
}

napi_value NapiMd::CreateMd(napi_env env, napi_callback_info info)
{
    size_t expectedArgc = ARGS_SIZE_ONE;
    size_t argc = expectedArgc;
    napi_value argv[ARGS_SIZE_ONE] = { nullptr };
    napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
    if (argc != expectedArgc) {
        LOGE("The input args num is invalid.");
        return nullptr;
    }
    std::string algoName;
    if (!GetStringFromJSParams(env, argv[PARAM0], algoName, false)) {
        LOGE("Failed to get algorithm.");
        return nullptr;
    }
    HcfMd *mdObj = nullptr;
    HcfResult res = HcfMdCreate(algoName.c_str(), &mdObj);
    if (res != HCF_SUCCESS) {
        napi_throw(env, GenerateBusinessError(env, res, "create C obj failed.", false));
        LOGE("create c mdObj failed.");
        return nullptr;
    }
    napi_value napiAlgName = nullptr;
    napi_create_string_utf8(env, algoName.c_str(), NAPI_AUTO_LENGTH, &napiAlgName);
    napi_value instance = nullptr;
    napi_value constructor = nullptr;
    napi_get_reference_value(env, classRef_, &constructor);
    napi_new_instance(env, constructor, argc, argv, &instance);
    napi_set_named_property(env, instance, CRYPTO_TAG_ALG_NAME.c_str(), napiAlgName);
    NapiMd *mdNapiObj = new (std::nothrow) NapiMd(mdObj);
    if (mdNapiObj == nullptr) {
        LOGE("create napi obj failed");
        return nullptr;
    }
    napi_wrap(
        env, instance, mdNapiObj,
        [](napi_env env, void *data, void *hint) {
            NapiMd *md = static_cast<NapiMd *>(data);
            delete md;
            return;
        },
        nullptr,
        nullptr);
    return instance;
}

void NapiMd::DefineMdJSClass(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("createMd", CreateMd),
    };
    napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc);
    napi_property_descriptor classDesc[] = {
        DECLARE_NAPI_FUNCTION("update", NapiMdUpdate),
        DECLARE_NAPI_FUNCTION("digest", NapiMdDoFinal),
        DECLARE_NAPI_FUNCTION("getMdLength", NapiGetMdLength),
    };
    napi_value constructor = nullptr;
    napi_define_class(env, "Md", NAPI_AUTO_LENGTH, MdConstructor, nullptr,
        sizeof(classDesc) / sizeof(classDesc[0]), classDesc, &constructor);
    napi_create_reference(env, constructor, 1, &classRef_);
}
} // CryptoFramework
} // OHOS