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

#include "napi_cert_chain_validator.h"

#include "napi/native_node_api.h"
#include "napi/native_api.h"
#include "log.h"
#include "memory.h"
#include "utils.h"
#include "result.h"
#include "object_base.h"
#include "napi_crypto_framework_defines.h"
#include "napi_utils.h"

namespace OHOS {
namespace CryptoFramework {
thread_local napi_ref NapiCertChainValidator::classRef_ = nullptr;

struct CfCtx {
    CfAsyncType asyncType = ASYNC_TYPE_CALLBACK;
    napi_ref callback = nullptr;
    napi_deferred deferred = nullptr;
    napi_async_work asyncWork = nullptr;

    NapiCertChainValidator *ccvClass = nullptr;
    HcfCertChainData *certChainData = nullptr;

    int32_t errCode = 0;
    const char *errMsg = nullptr;
};

NapiCertChainValidator::NapiCertChainValidator(HcfCertChainValidator *certChainValidator)
{
    this->certChainValidator_ = certChainValidator;
}

NapiCertChainValidator::~NapiCertChainValidator()
{
    HcfObjDestroy(this->certChainValidator_);
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

    if (context->certChainData != nullptr) {
        HcfFree(context->certChainData->data);
        context->certChainData->data = nullptr;
        HcfFree(context->certChainData);
        context->certChainData = nullptr;
    }

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

static void ValidateExecute(napi_env env, void *data)
{
    CfCtx *context = static_cast<CfCtx *>(data);
    HcfCertChainValidator *validator = context->ccvClass->GetCertChainValidator();
    context->errCode = validator->validate(validator, context->certChainData);
    if (context->errCode != HCF_SUCCESS) {
        LOGE("validate cert chain failed!");
        context->errMsg = "validate cert chain failed";
    }
}

static void ValidateComplete(napi_env env, napi_status status, void *data)
{
    CfCtx *context = static_cast<CfCtx *>(data);
    ReturnResult(env, context, NapiGetNull(env));
    FreeCryptoFwkCtx(env, context);
}

napi_value NapiCertChainValidator::Validate(napi_env env, napi_callback_info info)
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
    context->ccvClass = this;

    context->asyncType = (argc == ARGS_SIZE_TWO) ? ASYNC_TYPE_CALLBACK : ASYNC_TYPE_PROMISE;
    if (!GetCertChainFromValue(env, argv[PARAM0], &context->certChainData)) {
        LOGE("get cert chain data from napi value failed!");
        FreeCryptoFwkCtx(env, context);
        return nullptr;
    }
    napi_value promise = nullptr;
    if (context->asyncType == ASYNC_TYPE_CALLBACK) {
        if (!GetCallbackFromJSParams(env, argv[PARAM1], &context->callback, true)) {
            LOGE("get callback failed!");
            FreeCryptoFwkCtx(env, context);
            return nullptr;
        }
    } else {
        napi_create_promise(env, &context->deferred, &promise);
    }

    napi_create_async_work(
        env, nullptr, GetResourceName(env, "Validate"),
        ValidateExecute,
        ValidateComplete,
        static_cast<void *>(context),
        &context->asyncWork);

    napi_queue_async_work(env, context->asyncWork);
    if (context->asyncType == ASYNC_TYPE_PROMISE) {
        return promise;
    } else {
        return NapiGetNull(env);
    }
}

static napi_value NapiValidate(napi_env env, napi_callback_info info)
{
    LOGI("start to validate cert chain.");
    napi_value thisVar = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);
    NapiCertChainValidator *certChainValidator = nullptr;
    napi_unwrap(env, thisVar, reinterpret_cast<void **>(&certChainValidator));
    if (certChainValidator == nullptr) {
        LOGE("certChainValidator is nullptr!");
        return nullptr;
    }
    return certChainValidator->Validate(env, info);
}

static napi_value CertChainValidatorConstructor(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);
    return thisVar;
}

napi_value NapiCertChainValidator::CreateCertChainValidator(napi_env env, napi_callback_info info)
{
    LOGI("start to create cert chain validator.");
    napi_value thisVar = nullptr;
    size_t argc = ARGS_SIZE_ONE;
    napi_value argv[ARGS_SIZE_ONE] = { nullptr };
    napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);

    if (argc != ARGS_SIZE_ONE) {
        napi_throw(env, GenerateBusinessError(env, HCF_INVALID_PARAMS, "invalid params count", true));
        LOGE("invalid params count!");
        return nullptr;
    }

    std::string algorithm;
    if (!GetStringFromJSParams(env, argv[PARAM0], algorithm, true)) {
        LOGE("Failed to get algorithm.");
        return nullptr;
    }
    HcfCertChainValidator *certChainValidator = nullptr;
    HcfResult res = HcfCertChainValidatorCreate(algorithm.c_str(), &certChainValidator);
    if (res != HCF_SUCCESS) {
        napi_throw(env, GenerateBusinessError(env, res, "create cert chain validator failed", true));
        LOGE("Failed to create c cert chain validator.");
        return nullptr;
    }
    const char *returnAlgorithm = certChainValidator->getAlgorithm(certChainValidator);
    napi_value algValue = nullptr;
    napi_create_string_utf8(env, returnAlgorithm, NAPI_AUTO_LENGTH, &algValue);
    napi_value constructor = nullptr;
    napi_value validatorInstance = nullptr;
    napi_get_reference_value(env, classRef_, &constructor);
    napi_new_instance(env, constructor, 0, nullptr, &validatorInstance);
    napi_set_named_property(env, validatorInstance, CRYPTO_TAG_ALGORITHM.c_str(), algValue);
    NapiCertChainValidator *ccvClass = new NapiCertChainValidator(certChainValidator);
    napi_wrap(
        env, validatorInstance, ccvClass,
        [](napi_env env, void* data, void *hint) {
            NapiCertChainValidator *ccv = static_cast<NapiCertChainValidator *>(data);
            delete ccv;
        },
        nullptr,
        nullptr);

    return validatorInstance;
}

void NapiCertChainValidator::DefineCertChainValidatorJSClass(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("createCertChainValidator", CreateCertChainValidator),
    };
    napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc);

    napi_property_descriptor validatorDesc[] = {
        DECLARE_NAPI_FUNCTION("validate", NapiValidate),
    };
    napi_value constructor = nullptr;
    napi_define_class(env, "CertChainValidator", NAPI_AUTO_LENGTH, CertChainValidatorConstructor, nullptr,
        sizeof(validatorDesc) / sizeof(validatorDesc[0]), validatorDesc, &constructor);
    napi_create_reference(env, constructor, 1, &classRef_);
}
} // namespace CryptoFramework
} // namespace OHOS