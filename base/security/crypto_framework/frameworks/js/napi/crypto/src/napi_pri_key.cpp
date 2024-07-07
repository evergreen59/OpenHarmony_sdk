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

#include "napi_pri_key.h"

#include "log.h"
#include "napi_crypto_framework_defines.h"
#include "napi_utils.h"
#include "securec.h"

namespace OHOS {
namespace CryptoFramework {
thread_local napi_ref NapiPriKey::classRef_ = nullptr;

NapiPriKey::NapiPriKey(HcfPriKey *priKey) : NapiKey(reinterpret_cast<HcfKey *>(priKey)) {}

NapiPriKey::~NapiPriKey() {}

HcfPriKey *NapiPriKey::GetPriKey()
{
    return reinterpret_cast<HcfPriKey *>(NapiKey::GetHcfKey());
}

napi_value NapiPriKey::PriKeyConstructor(napi_env env, napi_callback_info info)
{
    LOGI("enter ...");

    napi_value thisVar = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);

    LOGI("out ...");
    return thisVar;
}

napi_value NapiPriKey::ConvertToJsPriKey(napi_env env)
{
    LOGI("enter ...");

    napi_value instance;
    napi_value constructor = nullptr;
    napi_get_reference_value(env, classRef_, &constructor);
    napi_new_instance(env, constructor, 0, nullptr, &instance);

    const char *algName = this->GetPriKey()->base.getAlgorithm(&(this->GetPriKey()->base));
    const char *format = this->GetPriKey()->base.getFormat(&(this->GetPriKey()->base));

    napi_value napiAlgName = nullptr;
    napi_create_string_utf8(env, algName, NAPI_AUTO_LENGTH, &napiAlgName);
    napi_set_named_property(env, instance, CRYPTO_TAG_ALG_NAME.c_str(), napiAlgName);

    napi_value napiFormat = nullptr;
    napi_create_string_utf8(env, format, NAPI_AUTO_LENGTH, &napiFormat);
    napi_set_named_property(env, instance, CRYPTO_TAG_FORMAT.c_str(), napiFormat);

    LOGI("out ...");
    return instance;
}

napi_value NapiPriKey::JsGetEncoded(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);
    NapiPriKey *napiPriKey = nullptr;
    napi_unwrap(env, thisVar, reinterpret_cast<void **>(&napiPriKey));

    HcfPriKey *priKey = napiPriKey->GetPriKey();

    HcfBlob returnBlob;
    HcfResult res = priKey->base.getEncoded(&priKey->base, &returnBlob);
    if (res != HCF_SUCCESS) {
        LOGE("c getEncoded fail.");
        return nullptr;
    }

    napi_value instance = ConvertBlobToNapiValue(env, &returnBlob);
    HcfBlobDataFree(&returnBlob);
    return instance;
}

napi_value NapiPriKey::JsClearMem(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);
    NapiPriKey *napiPriKey = nullptr;
    napi_unwrap(env, thisVar, reinterpret_cast<void **>(&napiPriKey));

    HcfPriKey *priKey = napiPriKey->GetPriKey();

    priKey->clearMem(priKey);
    return nullptr;
}

void NapiPriKey::DefinePriKeyJSClass(napi_env env)
{
    napi_property_descriptor classDesc[] = {
        DECLARE_NAPI_FUNCTION("getEncoded", NapiPriKey::JsGetEncoded),
        DECLARE_NAPI_FUNCTION("clearMem", NapiPriKey::JsClearMem),
    };
    napi_value constructor = nullptr;
    napi_define_class(env, "PriKey", NAPI_AUTO_LENGTH, NapiPriKey::PriKeyConstructor, nullptr,
        sizeof(classDesc) / sizeof(classDesc[0]), classDesc, &constructor);
    napi_create_reference(env, constructor, 1, &classRef_);
}
} // CryptoFramework
} // OHOS
