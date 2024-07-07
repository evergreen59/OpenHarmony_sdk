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

#include "napi_utils.h"

#include "log.h"
#include "memory.h"
#include "securec.h"
#include "cipher.h"
#include "napi_crypto_framework_defines.h"
#include "detailed_iv_params.h"
#include "detailed_gcm_params.h"
#include "detailed_ccm_params.h"

namespace OHOS {
namespace CryptoFramework {
using namespace std;

napi_value NapiGetNull(napi_env env)
{
    napi_value result = nullptr;
    napi_get_null(env, &result);
    return result;
}

napi_value ConvertArrayToNapiValue(napi_env env, HcfArray *array)
{
    if (array == nullptr) {
        LOGE("array is null!");
        return nullptr;
    }
    if (array->count == 0) {
        LOGE("array count is 0!");
        return nullptr;
    }
    napi_value returnArray = nullptr;
    napi_create_array(env, &returnArray);
    if (returnArray == nullptr) {
        LOGE("create return array failed!");
        return nullptr;
    }
    for (uint32_t i = 0; i < array->count; i++) {
        HcfBlob *blob = reinterpret_cast<HcfBlob *>(array->data + i);
        napi_value outBuffer = GenerateArrayBuffer(env, blob->data, blob->len);
        if (outBuffer == nullptr) {
            LOGE("generate array buffer failed!");
            return nullptr;
        }
        napi_value element = nullptr;
        napi_create_typedarray(env, napi_uint8_array, blob->len, outBuffer, 0, &element);
        napi_set_element(env, returnArray, i, element);
    }
    napi_value returnValue = nullptr;
    napi_create_object(env, &returnValue);
    napi_set_named_property(env, returnValue, CRYPTO_TAG_DATA.c_str(), returnArray);
    return returnValue;
}

napi_value GenerateArrayBuffer(napi_env env, uint8_t *data, uint32_t size)
{
    uint8_t *buffer = static_cast<uint8_t *>(HcfMalloc(size, 0));
    if (buffer == nullptr) {
        LOGE("malloc uint8 array buffer failed!");
        return nullptr;
    }

    if (memcpy_s(buffer, size, data, size) != EOK) {
        LOGE("memcpy_s data to buffer failed!");
        HcfFree(buffer);
        return nullptr;
    }

    napi_value outBuffer = nullptr;
    napi_status status = napi_create_external_arraybuffer(
        env, buffer, size, [](napi_env env, void *data, void *hint) { HcfFree(data); }, nullptr, &outBuffer);
    if (status != napi_ok) {
        LOGE("create uint8 array buffer failed!");
        HcfFree(buffer);
        return nullptr;
    }
    buffer = nullptr;
    return outBuffer;
}

static bool GetDataOfEncodingBlob(napi_env env, napi_value data, HcfEncodingBlob *encodingBlob)
{
    napi_typedarray_type arrayType;
    napi_value arrayBuffer = nullptr;
    size_t length = 0;
    size_t offset = 0;
    void *rawData = nullptr;

    napi_status status = napi_get_typedarray_info(env, data, &arrayType, &length,
        reinterpret_cast<void **>(&rawData), &arrayBuffer, &offset);
    if (status != napi_ok) {
        napi_throw(env, GenerateBusinessError(env, HCF_INVALID_PARAMS, "get array data failed", true));
        LOGE("failed to get array data!");
        return false;
    }
    if (arrayType != napi_uint8_array) {
        napi_throw(env, GenerateBusinessError(env, HCF_INVALID_PARAMS, "array type is not uint8 array", true));
        LOGE("array is not uint8 array!");
        return false;
    }

    if (length == 0) {
        LOGE("input data length is 0");
        return false;
    }
    encodingBlob->data = static_cast<uint8_t *>(HcfMalloc(length, 0));
    if (encodingBlob->data == nullptr) {
        LOGE("malloc encoding blob data failed!");
        return false;
    }
    if (memcpy_s(encodingBlob->data, length, rawData, length) != EOK) {
        LOGE("memcpy_s encoding blob data failed!");
        HcfFree(encodingBlob->data);
        encodingBlob->data = nullptr;
        return false;
    }
    encodingBlob->len = length;
    return true;
}

bool GetEncodingBlobFromValue(napi_env env, napi_value obj, HcfEncodingBlob **encodingBlob)
{
    *encodingBlob = static_cast<HcfEncodingBlob *>(HcfMalloc(sizeof(HcfEncodingBlob), 0));
    if (*encodingBlob == nullptr) {
        LOGE("malloc encoding blob failed!");
        return false;
    }
    napi_value data = nullptr;
    napi_status status = napi_get_named_property(env, obj, CRYPTO_TAG_DATA.c_str(), &data);
    if (status != napi_ok) {
        napi_throw(env, GenerateBusinessError(env, HCF_INVALID_PARAMS, "get encoding blob data failed", true));
        LOGE("failed to get encoding blob data!");
        HcfFree(*encodingBlob);
        *encodingBlob = nullptr;
        return false;
    }
    if (!GetDataOfEncodingBlob(env, data, *encodingBlob)) {
        HcfFree(*encodingBlob);
        *encodingBlob = nullptr;
        return false;
    }
    napi_value format = nullptr;
    status = napi_get_named_property(env, obj, CRYPTO_TAG_ENCODING_FORMAT.c_str(), &format);
    if (status != napi_ok) {
        napi_throw(env, GenerateBusinessError(env, HCF_INVALID_PARAMS, "get encoding blob format failed", true));
        LOGE("failed to get encoding blob format!");
        HcfFree((*encodingBlob)->data);
        (*encodingBlob)->data = nullptr;
        HcfFree(*encodingBlob);
        *encodingBlob = nullptr;
        return false;
    }
    napi_get_value_uint32(env, format, reinterpret_cast<uint32_t *>(&(*encodingBlob)->encodingFormat));
    return true;
}

napi_value ConvertEncodingBlobToNapiValue(napi_env env, HcfEncodingBlob *encodingBlob)
{
    napi_value outBuffer = GenerateArrayBuffer(env, encodingBlob->data, encodingBlob->len);
    if (outBuffer == nullptr) {
        LOGE("generate array buffer failed!");
        return nullptr;
    }
    napi_value outData = nullptr;
    napi_create_typedarray(env, napi_uint8_array, encodingBlob->len, outBuffer, 0, &outData);
    napi_value encoding = nullptr;
    napi_create_uint32(env, encodingBlob->encodingFormat, &encoding);
    napi_value returnEncodingBlob = nullptr;
    napi_create_object(env, &returnEncodingBlob);
    napi_set_named_property(env, returnEncodingBlob, CRYPTO_TAG_DATA.c_str(), outData);
    napi_set_named_property(env, returnEncodingBlob, CRYPTO_TAG_ENCODING_FORMAT.c_str(), encoding);
    return returnEncodingBlob;
}

HcfBlob *GetBlobFromNapiValue(napi_env env, napi_value arg)
{
    if ((env == nullptr) || (arg == nullptr)) {
        LOGE("Invalid parmas!");
        return nullptr;
    }
    napi_value data = nullptr;
    napi_status status = napi_get_named_property(env, arg, CRYPTO_TAG_DATA.c_str(), &data);
    if ((status != napi_ok) || (data == nullptr)) {
        LOGE("failed to get valid data property!");
        return nullptr;
    }

    size_t length = 0;
    size_t offset = 0;
    void *rawData = nullptr;
    napi_value arrayBuffer = nullptr;
    napi_typedarray_type arrayType;
    // Warning: Do not release the rawData returned by this interface because the rawData is managed by VM.
    status = napi_get_typedarray_info(env, data, &arrayType, &length,
        reinterpret_cast<void **>(&rawData), &arrayBuffer, &offset);
    if ((status != napi_ok) || (length == 0) || (rawData == nullptr)) {
        LOGE("failed to get valid rawData.");
        return nullptr;
    }
    if (arrayType != napi_uint8_array) {
        LOGE("input data is not uint8 array.");
        return nullptr;
    }

    HcfBlob *newBlob = reinterpret_cast<HcfBlob *>(HcfMalloc(sizeof(HcfBlob), 0));
    if (newBlob == nullptr) {
        LOGE("Failed to allocate newBlob memory!");
        return nullptr;
    }
    newBlob->len = length;
    newBlob->data = static_cast<uint8_t *>(HcfMalloc(length, 0));
    if (newBlob->data == nullptr) {
        LOGE("malloc blob data failed!");
        HcfFree(newBlob);
        return nullptr;
    }
    if (memcpy_s(newBlob->data, length, rawData, length) != EOK) {
        LOGE("memcpy_s blob data failed!");
        HcfFree(newBlob->data);
        HcfFree(newBlob);
        return nullptr;
    }

    return newBlob;
}

static const char *GetIvParamsSpecType()
{
    return IV_PARAMS_SPEC.c_str();
}

static const char *GetGcmParamsSpecType()
{
    return GCM_PARAMS_SPEC.c_str();
}

static const char *GetCcmParamsSpecType()
{
    return CCM_PARAMS_SPEC.c_str();
}

static HcfBlob *GetBlobFromParamsSpec(napi_env env, napi_value arg, const string &type)
{
    napi_value data = nullptr;
    HcfBlob *blob = nullptr;

    napi_status status = napi_get_named_property(env, arg, type.c_str(), &data);
    if ((status != napi_ok) || (data == nullptr)) {
        LOGE("failed to get valid param property!");
        return nullptr;
    }
    blob = GetBlobFromNapiValue(env, data);
    if (blob == nullptr) {
        LOGE("GetBlobFromNapiValue failed!");
        return nullptr;
    }
    return blob;
}

static bool GetIvParamsSpec(napi_env env, napi_value arg, HcfParamsSpec **paramsSpec)
{
    HcfIvParamsSpec *ivParamsSpec = reinterpret_cast<HcfIvParamsSpec *>(HcfMalloc(sizeof(HcfIvParamsSpec), 0));
    if (ivParamsSpec == nullptr) {
        LOGE("ivParamsSpec malloc failed!");
        return false;
    }

    HcfBlob *iv = GetBlobFromParamsSpec(env, arg, IV_PARAMS);
    if (iv == nullptr) {
        LOGE("GetBlobFromNapiValue failed!");
        HcfFree(ivParamsSpec);
        return false;
    }
    ivParamsSpec->base.getType = GetIvParamsSpecType;
    ivParamsSpec->iv = *iv;
    *paramsSpec = reinterpret_cast<HcfParamsSpec *>(ivParamsSpec);
    HcfFree(iv);
    return true;
}

static bool GetIvAndAadBlob(napi_env env, napi_value arg, HcfBlob **iv, HcfBlob **aad)
{
    *iv = GetBlobFromParamsSpec(env, arg, IV_PARAMS);
    if (*iv == nullptr) {
        LOGE("get iv failed!");
        return false;
    }

    *aad = GetBlobFromParamsSpec(env, arg, AAD_PARAMS);
    if (*aad == nullptr) {
        LOGE("get aad failed!");
        HcfFree((*iv)->data);
        HcfFree(*iv);
        return false;
    }
    return true;
}

static bool GetGcmParamsSpec(napi_env env, napi_value arg, HcfCryptoMode opMode, HcfParamsSpec **paramsSpec)
{
    HcfBlob *iv = nullptr;
    HcfBlob *aad = nullptr;
    HcfBlob *tag = nullptr;
    HcfBlob authTag = {};
    bool ret = false;

    HcfGcmParamsSpec *gcmParamsSpec = reinterpret_cast<HcfGcmParamsSpec *>(HcfMalloc(sizeof(HcfGcmParamsSpec), 0));
    if (gcmParamsSpec == nullptr) {
        LOGE("gcmParamsSpec malloc failed!");
        return false;
    }

    ret = GetIvAndAadBlob(env, arg, &iv, &aad);
    if (!ret) {
        LOGE("GetIvAndAadBlob failed!");
        goto clearup;
    }

    if (opMode == DECRYPT_MODE) {
        tag = GetBlobFromParamsSpec(env, arg, AUTHTAG_PARAMS);
        if (tag == nullptr) {
            LOGE("get tag failed!");
            goto clearup;
        }
    } else if (opMode == ENCRYPT_MODE) {
        authTag.data = static_cast<uint8_t *>(HcfMalloc(GCM_AUTH_TAG_LEN, 0));
        if (authTag.data == nullptr) {
            LOGE("get tag failed!");
            goto clearup;
        }
        authTag.len = GCM_AUTH_TAG_LEN;
    } else {
        goto clearup;
    }

    gcmParamsSpec->base.getType = GetGcmParamsSpecType;
    gcmParamsSpec->iv = *iv;
    gcmParamsSpec->aad = *aad;
    gcmParamsSpec->tag = opMode == DECRYPT_MODE ? *tag : authTag;
    *paramsSpec = reinterpret_cast<HcfParamsSpec *>(gcmParamsSpec);
    ret = true;
clearup:
   if (!ret) {
        HcfBlobDataFree(iv);
        HcfBlobDataFree(aad);
        HcfBlobDataFree(tag);
        HcfFree(gcmParamsSpec);
    }
    HcfFree(iv);
    HcfFree(aad);
    HcfFree(tag);
    return ret;
}

static bool GetCcmParamsSpec(napi_env env, napi_value arg, HcfCryptoMode opMode, HcfParamsSpec **paramsSpec)
{
    HcfBlob *iv = nullptr;
    HcfBlob *aad = nullptr;
    HcfBlob *tag = nullptr;
    HcfBlob authTag = {};
    bool ret = false;

    HcfCcmParamsSpec *ccmParamsSpec = reinterpret_cast<HcfCcmParamsSpec *>(HcfMalloc(sizeof(HcfCcmParamsSpec), 0));
    if (ccmParamsSpec == nullptr) {
        LOGE("ccmParamsSpec malloc failed!");
        return ret;
    }
    ret = GetIvAndAadBlob(env, arg, &iv, &aad);
    if (!ret) {
        LOGE("GetIvAndAadBlob failed!");
        goto clearup;
    }

    if (opMode == DECRYPT_MODE) {
        tag = GetBlobFromParamsSpec(env, arg, AUTHTAG_PARAMS);
        if (tag == nullptr) {
            LOGE("get tag failed!");
            goto clearup;
        }
    } else if (opMode == ENCRYPT_MODE) {
        authTag.data = static_cast<uint8_t *>(HcfMalloc(CCM_AUTH_TAG_LEN, 0));
        if (authTag.data == nullptr) {
            LOGE("get tag failed!");
            goto clearup;
        }
        authTag.len = CCM_AUTH_TAG_LEN;
    } else {
        goto clearup;
    }
    ccmParamsSpec->base.getType = GetCcmParamsSpecType;
    ccmParamsSpec->iv = *iv;
    ccmParamsSpec->aad = *aad;
    ccmParamsSpec->tag = opMode == DECRYPT_MODE ? *tag : authTag;
    *paramsSpec = reinterpret_cast<HcfParamsSpec *>(ccmParamsSpec);
    ret = true;
clearup:
    if (!ret) {
        HcfBlobDataFree(iv);
        HcfBlobDataFree(aad);
        HcfBlobDataFree(tag);
        HcfFree(ccmParamsSpec);
    }
    HcfFree(iv);
    HcfFree(aad);
    HcfFree(tag);
    return ret;
}

bool GetParamsSpecFromNapiValue(napi_env env, napi_value arg, HcfCryptoMode opMode, HcfParamsSpec **paramsSpec)
{
    napi_value data = nullptr;
    napi_valuetype valueType = napi_undefined;
    if ((env == nullptr) || (arg == nullptr) || (paramsSpec == nullptr)) {
        LOGE("Invalid parmas!");
        return false;
    }

    napi_status status = napi_get_named_property(env, arg, ALGO_PARAMS.c_str(), &data);
    napi_typeof(env, data, &valueType);
    if ((status != napi_ok) || (data == nullptr) || (valueType == napi_undefined)) {
        status = napi_get_named_property(env, arg, ALGO_PARAMS_OLD.c_str(), &data);
        napi_typeof(env, data, &valueType);
        if ((status != napi_ok) || (data == nullptr) || (valueType == napi_undefined)) {
            LOGE("failed to get valid algo name!");
            return false;
        }
    }
    string algoName;
    if (!GetStringFromJSParams(env, data, algoName, false)) {
        LOGE("GetStringFromJSParams failed!");
        return false;
    }
    if (algoName.compare(IV_PARAMS_SPEC) == 0) {
        return GetIvParamsSpec(env, arg, paramsSpec);
    } else if (algoName.compare(GCM_PARAMS_SPEC) == 0) {
        return GetGcmParamsSpec(env, arg, opMode, paramsSpec);
    } else if (algoName.compare(CCM_PARAMS_SPEC) == 0) {
        return GetCcmParamsSpec(env, arg, opMode, paramsSpec);
    } else {
        return false;
    }
}

napi_value ConvertBlobToNapiValue(napi_env env, HcfBlob *blob)
{
    if (blob == nullptr || blob->data == nullptr || blob->len == 0) {
        LOGE("Invalid blob!");
        return nullptr;
    }
    uint8_t *buffer = static_cast<uint8_t *>(HcfMalloc(blob->len, 0));
    if (buffer == nullptr) {
        LOGE("malloc uint8 array buffer failed!");
        return nullptr;
    }

    if (memcpy_s(buffer, blob->len, blob->data, blob->len) != EOK) {
        LOGE("memcpy_s data to buffer failed!");
        HcfFree(buffer);
        return nullptr;
    }

    napi_value outBuffer = nullptr;
    napi_status status = napi_create_external_arraybuffer(
        env, buffer, blob->len, [](napi_env env, void *data, void *hint) { HcfFree(data); }, nullptr, &outBuffer);
    if (status != napi_ok) {
        LOGE("create uint8 array buffer failed!");
        HcfFree(buffer);
        return nullptr;
    }
    buffer = nullptr;

    napi_value outData = nullptr;
    napi_create_typedarray(env, napi_uint8_array, blob->len, outBuffer, 0, &outData);
    napi_value dataBlob = nullptr;
    napi_create_object(env, &dataBlob);
    napi_set_named_property(env, dataBlob, CRYPTO_TAG_DATA.c_str(), outData);

    return dataBlob;
}

static bool GetDataOfCertChain(napi_env env, napi_value data, HcfCertChainData *certChain)
{
    napi_typedarray_type arrayType;
    napi_value arrayBuffer = nullptr;
    size_t length = 0;
    size_t offset = 0;
    void *rawData = nullptr;

    napi_status status = napi_get_typedarray_info(env, data, &arrayType, &length,
        reinterpret_cast<void **>(&rawData), &arrayBuffer, &offset);
    if (status != napi_ok) {
        napi_throw(env, GenerateBusinessError(env, HCF_INVALID_PARAMS, "get array data failed", true));
        LOGE("failed to get array data!");
        return false;
    }
    if (arrayType != napi_uint8_array) {
        napi_throw(env, GenerateBusinessError(env, HCF_INVALID_PARAMS, "array type is not uint8 array", true));
        LOGE("array is not uint8 array!");
        return false;
    }

    if (length == 0) {
        LOGE("input data length is 0");
        return false;
    }
    certChain->data = static_cast<uint8_t *>(HcfMalloc(length, 0));
    if (certChain->data == nullptr) {
        LOGE("malloc cert chain data failed!");
        return false;
    }
    if (memcpy_s(certChain->data, length, rawData, length) != EOK) {
        LOGE("memcpy_s cert chain data failed!");
        HcfFree(certChain->data);
        certChain->data = nullptr;
        return false;
    }
    certChain->dataLen = length;
    return true;
}

bool GetCertChainFromValue(napi_env env, napi_value obj, HcfCertChainData **certChainData)
{
    *certChainData = static_cast<HcfCertChainData *>(HcfMalloc(sizeof(HcfCertChainData), 0));
    if (*certChainData == nullptr) {
        LOGE("malloc certChainData failed!");
        return false;
    }
    napi_value data = nullptr;
    napi_status status = napi_get_named_property(env, obj, CRYPTO_TAG_DATA.c_str(), &data);
    if (status != napi_ok) {
        napi_throw(env, GenerateBusinessError(env, HCF_INVALID_PARAMS, "get cert chain data failed", true));
        LOGE("failed to get cert chain data!");
        HcfFree(*certChainData);
        *certChainData = nullptr;
        return false;
    }
    if (!GetDataOfCertChain(env, data, *certChainData)) {
        HcfFree(*certChainData);
        *certChainData = nullptr;
        return false;
    }

    napi_value certCount = nullptr;
    status = napi_get_named_property(env, obj, CRYPTO_TAG_COUNT.c_str(), &certCount);
    if (status != napi_ok) {
        napi_throw(env, GenerateBusinessError(env, HCF_INVALID_PARAMS, "get cert chain count failed", true));
        LOGE("failed to get cert count!");
        HcfFree((*certChainData)->data);
        (*certChainData)->data = nullptr;
        HcfFree(*certChainData);
        *certChainData = nullptr;
        return false;
    }
    napi_get_value_uint32(env, certCount, reinterpret_cast<uint32_t *>(&(*certChainData)->count));

    napi_value format = nullptr;
    status = napi_get_named_property(env, obj, CRYPTO_TAG_ENCODING_FORMAT.c_str(), &format);
    if (status != napi_ok) {
        napi_throw(env, GenerateBusinessError(env, HCF_INVALID_PARAMS, "get cert chain format failed", true));
        LOGE("failed to get cert chain format!");
        HcfFree((*certChainData)->data);
        (*certChainData)->data = nullptr;
        HcfFree(*certChainData);
        *certChainData = nullptr;
        return false;
    }
    napi_get_value_uint32(env, format, reinterpret_cast<uint32_t *>(&(*certChainData)->format));
    return true;
}

bool GetStringFromJSParams(napi_env env, napi_value arg, string &returnStr, bool isCertFunc)
{
    napi_valuetype valueType;
    napi_typeof(env, arg, &valueType);
    if (valueType != napi_string) {
        napi_throw(env, GenerateBusinessError(env, HCF_INVALID_PARAMS, "param type is not string", isCertFunc));
        LOGE("wrong argument type. expect string type. [Type]: %d", valueType);
        return false;
    }

    size_t length = 0;
    if (napi_get_value_string_utf8(env, arg, nullptr, 0, &length) != napi_ok) {
        LOGE("can not get string length");
        return false;
    }
    returnStr.reserve(length + 1);
    returnStr.resize(length);
    if (napi_get_value_string_utf8(env, arg, returnStr.data(), (length + 1), &length) != napi_ok) {
        LOGE("can not get string value");
        return false;
    }
    return true;
}

bool GetInt32FromJSParams(napi_env env, napi_value arg, int32_t &returnInt, bool isCertFunc)
{
    napi_valuetype valueType;
    napi_typeof(env, arg, &valueType);
    if (valueType != napi_number) {
        napi_throw(env, GenerateBusinessError(env, HCF_INVALID_PARAMS, "param type is not number", isCertFunc));
        LOGE("wrong argument type. expect int type. [Type]: %d", valueType);
        return false;
    }

    if (napi_get_value_int32(env, arg, &returnInt) != napi_ok) {
        LOGE("can not get int value");
        return false;
    }
    return true;
}

bool GetUint32FromJSParams(napi_env env, napi_value arg, uint32_t &returnInt, bool isCertFunc)
{
    napi_valuetype valueType;
    napi_typeof(env, arg, &valueType);
    if (valueType != napi_number) {
        napi_throw(env, GenerateBusinessError(env, HCF_INVALID_PARAMS, "param type is not number", isCertFunc));
        LOGE("wrong argument type. expect int type. [Type]: %d", valueType);
        return false;
    }

    if (napi_get_value_uint32(env, arg, &returnInt) != napi_ok) {
        LOGE("can not get int value");
        return false;
    }
    return true;
}

bool GetCallbackFromJSParams(napi_env env, napi_value arg, napi_ref *returnCb, bool isCertFunc)
{
    napi_valuetype valueType = napi_undefined;
    napi_typeof(env, arg, &valueType);
    if (valueType != napi_function) {
        napi_throw(env, GenerateBusinessError(env, HCF_INVALID_PARAMS, "param type is not function", isCertFunc));
        LOGE("wrong argument type. expect callback type. [Type]: %d", valueType);
        return false;
    }

    napi_create_reference(env, arg, 1, returnCb);
    return true;
}

static uint32_t GetJsErrValueByErrCode(int32_t errCode)
{
    switch (errCode) {
        case HCF_INVALID_PARAMS:
            return JS_ERR_INVALID_PARAMS;
        case HCF_NOT_SUPPORT:
            return JS_ERR_NOT_SUPPORT;
        case HCF_ERR_MALLOC:
            return JS_ERR_OUT_OF_MEMORY;
        case HCF_ERR_COPY:
            return JS_ERR_RUNTIME_ERROR;
        case HCF_ERR_CRYPTO_OPERATION:
            return JS_ERR_CRYPTO_OPERATION;
        default:
            return JS_ERR_DEFAULT_ERR;
    }
}

static uint32_t GetCertErrValueByErrCode(int32_t errCode)
{
    switch (errCode) {
        case HCF_INVALID_PARAMS:
            return JS_ERR_CERT_INVALID_PARAMS;
        case HCF_NOT_SUPPORT:
            return JS_ERR_CERT_NOT_SUPPORT;
        case HCF_ERR_MALLOC:
            return JS_ERR_CERT_OUT_OF_MEMORY;
        case HCF_ERR_COPY:
            return JS_ERR_CERT_RUNTIME_ERROR;
        case HCF_ERR_CRYPTO_OPERATION:
            return JS_ERR_CERT_CRYPTO_OPERATION;
        case HCF_ERR_CERT_SIGNATURE_FAILURE:
            return JS_ERR_CERT_SIGNATURE_FAILURE;
        case HCF_ERR_CERT_NOT_YET_VALID:
            return JS_ERR_CERT_NOT_YET_VALID;
        case HCF_ERR_CERT_HAS_EXPIRED:
            return JS_ERR_CERT_HAS_EXPIRED;
        case HCF_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY:
            return JS_ERR_UNABLE_TO_GET_ISSUER_CERT_LOCALLY;
        case HCF_ERR_KEYUSAGE_NO_CERTSIGN:
            return JS_ERR_KEYUSAGE_NO_CERTSIGN;
        case HCF_ERR_KEYUSAGE_NO_DIGITAL_SIGNATURE:
            return JS_ERR_KEYUSAGE_NO_DIGITAL_SIGNATURE;
        default:
            return JS_ERR_DEFAULT_ERR;
    }
}

napi_value GenerateBusinessError(napi_env env, int32_t errCode, const char *errMsg, bool isCertFunc)
{
    napi_value businessError = nullptr;

    napi_value code = nullptr;
    if (isCertFunc) {
        napi_create_uint32(env, GetCertErrValueByErrCode(errCode), &code);
    } else {
        napi_create_uint32(env, GetJsErrValueByErrCode(errCode), &code);
    }

    napi_value msg = nullptr;
    napi_create_string_utf8(env, errMsg, NAPI_AUTO_LENGTH, &msg);

    napi_create_error(env, nullptr, msg, &businessError);
    napi_set_named_property(env, businessError, CRYPTO_TAG_ERR_CODE.c_str(), code);

    return businessError;
}

bool CheckArgsCount(napi_env env, size_t argc, size_t expectedCount, bool isSync, bool isCertFunc)
{
    if (isSync) {
        if (argc != expectedCount) {
            napi_throw(env, GenerateBusinessError(env, HCF_INVALID_PARAMS, "invalid params count", isCertFunc));
            LOGE("invalid params count!");
            return false;
        }
    } else {
        if ((argc != expectedCount) && (argc != (expectedCount - ARGS_SIZE_ONE))) {
            napi_throw(env, GenerateBusinessError(env, HCF_INVALID_PARAMS, "invalid params count", isCertFunc));
            LOGE("invalid params count!");
            return false;
        }
    }
    return true;
}

napi_value GetResourceName(napi_env env, const char *name)
{
    napi_value resourceName = nullptr;
    napi_create_string_utf8(env, name, NAPI_AUTO_LENGTH, &resourceName);
    return resourceName;
}
}  // namespace CryptoFramework
}  // namespace OHOS
