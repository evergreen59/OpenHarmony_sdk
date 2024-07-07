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

#include "securec.h"
#include "log.h"

#include "napi_asy_key_generator.h"
#include "napi_sym_key_generator.h"
#include "napi_cipher.h"
#include "napi_key_pair.h"
#include "napi_pri_key.h"
#include "napi_pub_key.h"
#include "napi_sign.h"
#include "napi_verify.h"
#include "napi_key_agreement.h"
#include "napi_mac.h"
#include "napi_md.h"
#include "napi_rand.h"
#include "napi_sym_key.h"
#include "napi_key.h"
#include "napi_utils.h"
#include "napi_crypto_framework_defines.h"

namespace OHOS {
namespace CryptoFramework {
static napi_value CreateCryptoMode(napi_env env)
{
    napi_value cryptoMode = nullptr;
    napi_create_object(env, &cryptoMode);

    AddUint32Property(env, cryptoMode, "ENCRYPT_MODE", ENCRYPT_MODE);
    AddUint32Property(env, cryptoMode, "DECRYPT_MODE", DECRYPT_MODE);
    return cryptoMode;
}

static void DefineCryptoModeProperties(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_PROPERTY("CryptoMode", CreateCryptoMode(env)),
    };
    napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc);
}

static napi_value CreateResultCode(napi_env env)
{
    napi_value resultCode = nullptr;
    napi_create_object(env, &resultCode);

    AddUint32Property(env, resultCode, "INVALID_PARAMS", JS_ERR_INVALID_PARAMS);
    AddUint32Property(env, resultCode, "NOT_SUPPORT", JS_ERR_NOT_SUPPORT);
    AddUint32Property(env, resultCode, "ERR_OUT_OF_MEMORY", JS_ERR_OUT_OF_MEMORY);
    AddUint32Property(env, resultCode, "ERR_RUNTIME_ERROR", JS_ERR_RUNTIME_ERROR);
    AddUint32Property(env, resultCode, "ERR_CRYPTO_OPERATION", JS_ERR_CRYPTO_OPERATION);

    return resultCode;
}

static void DefineResultCodeProperties(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_PROPERTY("Result", CreateResultCode(env)),
    };
    napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc);
}

/***********************************************
 * Module export and register
 ***********************************************/
static napi_value ModuleExport(napi_env env, napi_value exports)
{
    LOGI("module init start.");

    DefineCryptoModeProperties(env, exports);
    DefineResultCodeProperties(env, exports);
    NapiAsyKeyGenerator::DefineAsyKeyGeneratorJSClass(env, exports);
    NapiKeyPair::DefineKeyPairJSClass(env);
    NapiPubKey::DefinePubKeyJSClass(env);
    NapiPriKey::DefinePriKeyJSClass(env);

    NapiSign::DefineSignJSClass(env, exports);
    NapiVerify::DefineVerifyJSClass(env, exports);
    NapiKeyAgreement::DefineKeyAgreementJSClass(env, exports);
    NapiMac::DefineMacJSClass(env, exports);
    NapiMd::DefineMdJSClass(env, exports);
    NapiPubKey::DefinePubKeyJSClass(env);
    NapiRand::DefineRandJSClass(env, exports);
    NapiSymKeyGenerator::DefineSymKeyGeneratorJSClass(env, exports);
    NapiCipher::DefineCipherJSClass(env, exports);
    NapiSymKey::DefineSymKeyJSClass(env);
    NapiKey::DefineHcfKeyJSClass(env);
    LOGI("module init end.");
    return exports;
}

extern "C" __attribute__((constructor)) void RegisterModule(void)
{
    static napi_module cryptoFrameworkModule = {
        .nm_version = 1,
        .nm_flags = 0,
        .nm_filename = nullptr,
        .nm_register_func = ModuleExport,
        .nm_modname = "security.cryptoFramework",
        .nm_priv = nullptr,
        .reserved = { nullptr },
    };
    napi_module_register(&cryptoFrameworkModule);
}
}  // namespace CryptoFramework
}  // namespace OHOS
