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

#include "napi_key_pair.h"

#include "securec.h"
#include "log.h"
#include "napi_crypto_framework_defines.h"
#include "napi_pri_key.h"
#include "napi_pub_key.h"

#include "napi_utils.h"

namespace OHOS {
namespace CryptoFramework {
thread_local napi_ref NapiKeyPair::classRef_ = nullptr;

NapiKeyPair::NapiKeyPair(HcfKeyPair *keyPair)
{
    this->keyPair_ = keyPair;
}

NapiKeyPair::~NapiKeyPair()
{
    HcfObjDestroy(this->keyPair_);
    this->keyPair_ = nullptr;
}

napi_value NapiKeyPair::KeyPairConstructor(napi_env env, napi_callback_info info)
{
    LOGI("enter ...");
    napi_value thisVar = nullptr;
    napi_get_cb_info(env, info, nullptr, nullptr, &thisVar, nullptr);
    LOGI("out ...");
    return thisVar;
}

napi_value NapiKeyPair::ConvertToJsKeyPair(napi_env env)
{
    LOGI("enter ...");

    napi_value instance;
    napi_value constructor = nullptr;
    napi_get_reference_value(env, classRef_, &constructor);
    napi_new_instance(env, constructor, 0, nullptr, &instance);

    if (this->keyPair_->pubKey != nullptr) {
        NapiPubKey *napiPubKey = new NapiPubKey(this->keyPair_->pubKey);
        napi_value pubKey = napiPubKey->ConvertToJsPubKey(env);
        napi_wrap(
            env, pubKey, napiPubKey,
            [](napi_env env, void *data, void *hint) {
                NapiPubKey *napiPubKey = static_cast<NapiPubKey *>(data);
                delete napiPubKey;
                return;
            },
            nullptr, nullptr);
        napi_set_named_property(env, instance, CRYPTO_TAG_PUB_KEY.c_str(), pubKey);
    }

    if (this->keyPair_->priKey != nullptr) {
        NapiPriKey *napiPriKey = new NapiPriKey(this->keyPair_->priKey);
        napi_value priKey = napiPriKey->ConvertToJsPriKey(env);
        napi_wrap(
            env, priKey, napiPriKey,
            [](napi_env env, void *data, void *hint) {
                NapiPriKey *napiPriKey = static_cast<NapiPriKey *>(data);
                delete napiPriKey;
                return;
            },
            nullptr, nullptr);
        napi_set_named_property(env, instance, CRYPTO_TAG_PRI_KEY.c_str(), priKey);
    }

    LOGI("out ...");
    return instance;
}

void NapiKeyPair::DefineKeyPairJSClass(napi_env env)
{
    napi_property_descriptor classDesc[] = {};
    napi_value constructor = nullptr;
    napi_define_class(env, "KeyPair", NAPI_AUTO_LENGTH, KeyPairConstructor, nullptr,
        sizeof(classDesc) / sizeof(classDesc[0]), classDesc, &constructor);
    napi_create_reference(env, constructor, 1, &classRef_);
}
} // CryptoFramework
} // OHOS
