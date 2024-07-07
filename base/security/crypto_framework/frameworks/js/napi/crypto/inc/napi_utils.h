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

#ifndef HCF_NAPI_UILTS_H
#define HCF_NAPI_UILTS_H

#include <cstdint>
#include <string>
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "blob.h"
#include "cert_chain_validator.h"
#include "algorithm_parameter.h"
#include "cipher.h"

namespace OHOS {
namespace CryptoFramework {
#define PARAMS_NUM_ONE 1
#define PARAMS_NUM_TWO 2
#define PARAMS_NUM_THREE 3
#define PARAMS_NUM_FOUR 4

enum AsyncType {
    ASYNC_CALLBACK = 1,
    ASYNC_PROMISE = 2
};

inline void AddUint32Property(napi_env env, napi_value object, const char *name, uint32_t value)
{
    napi_value property = nullptr;
    napi_create_uint32(env, value, &property);
    napi_set_named_property(env, object, name, property);
}

HcfBlob *GetBlobFromNapiValue(napi_env env, napi_value arg);
bool GetParamsSpecFromNapiValue(napi_env env, napi_value arg, HcfCryptoMode opMode, HcfParamsSpec **paramsSpec);
napi_value ConvertBlobToNapiValue(napi_env env, HcfBlob *blob);

bool GetStringFromJSParams(napi_env env, napi_value arg, std::string &returnStr, bool isCertFunc);
bool GetInt32FromJSParams(napi_env env, napi_value arg, int32_t &returnInt, bool isCertFunc);
bool GetUint32FromJSParams(napi_env env, napi_value arg, uint32_t &returnInt, bool isCertFunc);
bool GetCallbackFromJSParams(napi_env env, napi_value arg, napi_ref *returnCb, bool isCertFunc);
bool GetEncodingBlobFromValue(napi_env env, napi_value object, HcfEncodingBlob **encodingBlob);
bool GetCertChainFromValue(napi_env env, napi_value object, HcfCertChainData **certChainData);
bool CheckArgsCount(napi_env env, size_t argc, size_t expectedCount, bool isSync, bool isCertFunc);
napi_value GetResourceName(napi_env env, const char *name);
napi_value GenerateArrayBuffer(napi_env env, uint8_t *data, uint32_t size);
napi_value NapiGetNull(napi_env env);
napi_value ConvertArrayToNapiValue(napi_env env, HcfArray *array);
napi_value ConvertEncodingBlobToNapiValue(napi_env env, HcfEncodingBlob *encodingBlob);
napi_value GenerateBusinessError(napi_env env, int32_t errCode, const char *errMsg, bool isCertFunc);
}  // namespace CryptoFramework
}  // namespace OHOS
#endif
