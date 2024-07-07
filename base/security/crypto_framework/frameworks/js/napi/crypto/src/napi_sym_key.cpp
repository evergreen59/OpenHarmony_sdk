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

#include "napi_sym_key.h"

#include "securec.h"
#include "log.h"
#include "napi_utils.h"
#include "napi_crypto_framework_defines.h"

namespace OHOS {
namespace CryptoFramework {
thread_local napi_ref NapiSymKey::classRef_ = nullptr;

NapiSymKey::NapiSymKey(HcfSymKey *symKey) : NapiKey(reinterpret_cast<HcfKey *>(symKey)) {}

NapiSymKey::~NapiSymKey()
{
    HcfObjDestroy(this->hcfKey_);
    this->hcfKey_ = nullptr;
}

HcfSymKey *NapiSymKey::GetSymKey() const
{
    return reinterpret_cast<HcfSymKey *>(NapiKey::GetHcfKey());
}

napi_value NapiSymKey::JsClearMem(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    NapiSymKey *napiSymKey = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr));

    NAPI_CALL(env, napi_unwrap(env, thisVar, reinterpret_cast<void **>(&napiSymKey)));
    HcfSymKey *key = napiSymKey->GetSymKey();
    key->clearMem(key);
    return nullptr;
}

napi_value NapiSymKey::SymKeyConstructor(napi_env env, napi_callback_info info)
{
    napi_value thisVar = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr));
    return thisVar;
}

napi_value NapiSymKey::CreateSymKey(napi_env env)
{
    napi_value instance = nullptr;
    napi_value constructor = nullptr;
    NAPI_CALL(env, napi_get_reference_value(env, classRef_, &constructor));
    NAPI_CALL(env, napi_new_instance(env, constructor, 0, nullptr, &instance));
    return instance;
}

void NapiSymKey::DefineSymKeyJSClass(napi_env env)
{
    napi_property_descriptor classDesc[] = {
        DECLARE_NAPI_FUNCTION("getEncoded", NapiKey::JsGetEncoded),
        DECLARE_NAPI_FUNCTION("clearMem", NapiSymKey::JsClearMem),
        {.utf8name = "format", .getter = NapiKey::JsGetFormat},
        {.utf8name = "algName", .getter = NapiKey::JsGetAlgorithm},
    };
    napi_value constructor = nullptr;
    napi_define_class(env, "SymKey", NAPI_AUTO_LENGTH, SymKeyConstructor, nullptr,
        sizeof(classDesc) / sizeof(classDesc[0]), classDesc, &constructor);
    napi_create_reference(env, constructor, 1, &classRef_);
}
} // CryptoFramework
} // OHOS
