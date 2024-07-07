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

#ifndef NAPI_CRYPTO_FRAMEWORK_DEFINES_H
#define NAPI_CRYPTO_FRAMEWORK_DEFINES_H

#include <cstdint>

namespace OHOS {
namespace CryptoFramework {
constexpr size_t CALLBACK_SIZE = 1;
constexpr size_t ARGS_SIZE_ZERO = 0;
constexpr size_t ARGS_SIZE_ONE = 1;
constexpr size_t ARGS_SIZE_TWO = 2;
constexpr size_t ARGS_SIZE_THREE = 3;
constexpr size_t ARGS_SIZE_FOUR = 4;
constexpr size_t GCM_AUTH_TAG_LEN = 16;
constexpr size_t CCM_AUTH_TAG_LEN = 12;
constexpr int32_t PARAM0 = 0;
constexpr int32_t PARAM1 = 1;
constexpr int32_t PARAM2 = 2;
constexpr uint32_t JS_ERR_DEFAULT_ERR = 0;
constexpr uint32_t JS_ERR_INVALID_PARAMS = 401;
constexpr uint32_t JS_ERR_NOT_SUPPORT = 801;
constexpr uint32_t JS_ERR_OUT_OF_MEMORY = 17620001;
constexpr uint32_t JS_ERR_RUNTIME_ERROR = 17620002;
constexpr uint32_t JS_ERR_CRYPTO_OPERATION = 17630001;

constexpr uint32_t JS_ERR_CERT_INVALID_PARAMS = 401;
constexpr uint32_t JS_ERR_CERT_NOT_SUPPORT = 801;
constexpr uint32_t JS_ERR_CERT_OUT_OF_MEMORY = 19020001;
constexpr uint32_t JS_ERR_CERT_RUNTIME_ERROR = 19020002;
constexpr uint32_t JS_ERR_CERT_CRYPTO_OPERATION = 19030001;
constexpr uint32_t JS_ERR_CERT_SIGNATURE_FAILURE = 19030002;
constexpr uint32_t JS_ERR_CERT_NOT_YET_VALID = 19030003;
constexpr uint32_t JS_ERR_CERT_HAS_EXPIRED = 19030004;
constexpr uint32_t JS_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY = 19030005;
constexpr uint32_t JS_ERR_KEYUSAGE_NO_CERTSIGN = 19030006;
constexpr uint32_t JS_ERR_KEYUSAGE_NO_DIGITAL_SIGNATURE = 19030007;

const std::string CRYPTO_TAG_DATA = "data";
const std::string CRYPTO_TAG_ERR_CODE = "code";
const std::string CRYPTO_TAG_ERR_MSG = "message";
const std::string CRYPTO_TAG_COUNT = "count";
const std::string CRYPTO_TAG_ENCODING_FORMAT = "encodingFormat";
const std::string CRYPTO_TAG_ALGORITHM = "algorithm";
const std::string CRYPTO_TAG_ALG_NAME = "algName";
const std::string CRYPTO_TAG_FORMAT = "format";
const std::string CRYPTO_TAG_PUB_KEY = "pubKey";
const std::string CRYPTO_TAG_PRI_KEY = "priKey";
const std::string IV_PARAMS = "iv";
const std::string AAD_PARAMS = "aad";
const std::string AUTHTAG_PARAMS = "authTag";
const std::string ALGO_PARAMS = "algName";
const std::string ALGO_PARAMS_OLD = "algoName";
const std::string IV_PARAMS_SPEC = "IvParamsSpec";
const std::string GCM_PARAMS_SPEC = "GcmParamsSpec";
const std::string CCM_PARAMS_SPEC = "CcmParamsSpec";
const std::string COMMON_ERR_MSG = "An exception occurs.";

enum CfAsyncType {
    ASYNC_TYPE_CALLBACK = 1,
    ASYNC_TYPE_PROMISE = 2
};
} // namespace CryptoFramework
} // namespace OHOS

#endif // NAPI_CRYPTO_FRAMEWORK_DEFINES_H
