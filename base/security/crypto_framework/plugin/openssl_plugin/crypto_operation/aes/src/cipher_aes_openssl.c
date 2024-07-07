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
#include "blob.h"
#include "log.h"
#include "memory.h"
#include "result.h"
#include "utils.h"
#include "aes_openssl_common.h"
#include "sym_common_defines.h"
#include "openssl_common.h"
#include "openssl_class.h"

#define MAX_AAD_LEN 2048
#define GCM_IV_LEN 12
#define CCM_IV_MIN_LEN 7
#define CCM_IV_MAX_LEN 13
#define AES_BLOCK_SIZE 16
#define GCM_TAG_SIZE 16
#define CCM_TAG_SIZE 12
#define AES_SIZE_128 16
#define AES_SIZE_192 24
#define AES_SIZE_256 32

typedef struct {
    HcfCipherGeneratorSpi base;
    CipherAttr attr;
    CipherData *cipherData;
} HcfCipherAesGeneratorSpiOpensslImpl;

static const char *GetAesGeneratorClass(void)
{
    return OPENSSL_AES_CIPHER_CLASS;
}

static const EVP_CIPHER *CipherEcbType(HCF_ALG_PARA_VALUE value)
{
    switch (value) {
        case HCF_ALG_AES_128:
            return EVP_aes_128_ecb();
        case HCF_ALG_AES_192:;
            return EVP_aes_192_ecb();
        case HCF_ALG_AES_256:
            return EVP_aes_256_ecb();
        default:
            break;
    }
    return EVP_aes_128_ecb();
}

static const EVP_CIPHER *CipherCbcType(HCF_ALG_PARA_VALUE value)
{
    switch (value) {
        case HCF_ALG_AES_128:
            return EVP_aes_128_cbc();
        case HCF_ALG_AES_192:;
            return EVP_aes_192_cbc();
        case HCF_ALG_AES_256:
            return EVP_aes_256_cbc();
        default:
            break;
    }
    return EVP_aes_128_cbc();
}

static const EVP_CIPHER *CipherCtrType(HCF_ALG_PARA_VALUE value)
{
    switch (value) {
        case HCF_ALG_AES_128:
            return EVP_aes_128_ctr();
        case HCF_ALG_AES_192:;
            return EVP_aes_192_ctr();
        case HCF_ALG_AES_256:
            return EVP_aes_256_ctr();
        default:
            break;
    }
    return EVP_aes_128_ctr();
}

static const EVP_CIPHER *CipherOfbType(HCF_ALG_PARA_VALUE value)
{
    switch (value) {
        case HCF_ALG_AES_128:
            return EVP_aes_128_ofb();
        case HCF_ALG_AES_192:;
            return EVP_aes_192_ofb();
        case HCF_ALG_AES_256:
            return EVP_aes_256_ofb();
        default:
            break;
    }
    return EVP_aes_128_ofb();
}

static const EVP_CIPHER *CipherCfbType(HCF_ALG_PARA_VALUE value)
{
    switch (value) {
        case HCF_ALG_AES_128:
            return EVP_aes_128_cfb();
        case HCF_ALG_AES_192:;
            return EVP_aes_192_cfb();
        case HCF_ALG_AES_256:
            return EVP_aes_256_cfb();
        default:
            break;
    }
    return EVP_aes_128_cfb();
}

static const EVP_CIPHER *CipherCfb1Type(HCF_ALG_PARA_VALUE value)
{
    switch (value) {
        case HCF_ALG_AES_128:
            return EVP_aes_128_cfb1();
        case HCF_ALG_AES_192:;
            return EVP_aes_192_cfb1();
        case HCF_ALG_AES_256:
            return EVP_aes_256_cfb1();
        default:
            break;
    }
    return EVP_aes_128_cfb1();
}

static const EVP_CIPHER *CipherCfb128Type(HCF_ALG_PARA_VALUE value)
{
    switch (value) {
        case HCF_ALG_AES_128:
            return EVP_aes_128_cfb128();
        case HCF_ALG_AES_192:;
            return EVP_aes_192_cfb128();
        case HCF_ALG_AES_256:
            return EVP_aes_256_cfb128();
        default:
            break;
    }
    return EVP_aes_128_cfb128();
}

static const EVP_CIPHER *CipherCfb8Type(HCF_ALG_PARA_VALUE value)
{
    switch (value) {
        case HCF_ALG_AES_128:
            return EVP_aes_128_cfb8();
        case HCF_ALG_AES_192:;
            return EVP_aes_192_cfb8();
        case HCF_ALG_AES_256:
            return EVP_aes_256_cfb8();
        default:
            break;
    }
    return EVP_aes_128_cfb8();
}


static const EVP_CIPHER *CipherCcmType(HCF_ALG_PARA_VALUE value)
{
    switch (value) {
        case HCF_ALG_AES_128:
            return EVP_aes_128_ccm();
        case HCF_ALG_AES_192:;
            return EVP_aes_192_ccm();
        case HCF_ALG_AES_256:
            return EVP_aes_256_ccm();
        default:
            break;
    }
    return EVP_aes_128_ccm();
}

static const EVP_CIPHER *CipherGcmType(HCF_ALG_PARA_VALUE value)
{
    switch (value) {
        case HCF_ALG_AES_128:
            return EVP_aes_128_gcm();
        case HCF_ALG_AES_192:;
            return EVP_aes_192_gcm();
        case HCF_ALG_AES_256:
            return EVP_aes_256_gcm();
        default:
            break;
    }
    return EVP_aes_128_gcm();
}

static const EVP_CIPHER *DefaultCiherType(HCF_ALG_PARA_VALUE value)
{
    return CipherEcbType(value);
}

static const EVP_CIPHER *GetCipherType(HcfCipherAesGeneratorSpiOpensslImpl *impl)
{
    switch (impl->attr.mode) {
        case HCF_ALG_MODE_ECB:
            return CipherEcbType(impl->attr.keySize);
        case HCF_ALG_MODE_CBC:
            return CipherCbcType(impl->attr.keySize);
        case HCF_ALG_MODE_CTR:
            return CipherCtrType(impl->attr.keySize);
        case HCF_ALG_MODE_OFB:
            return CipherOfbType(impl->attr.keySize);
        case HCF_ALG_MODE_CFB:
            return CipherCfbType(impl->attr.keySize);
        case HCF_ALG_MODE_CFB1:
            return CipherCfb1Type(impl->attr.keySize);
        case HCF_ALG_MODE_CFB8:
            return CipherCfb8Type(impl->attr.keySize);
        case HCF_ALG_MODE_CFB128:
            return CipherCfb128Type(impl->attr.keySize);
        case HCF_ALG_MODE_CCM:
            return CipherCcmType(impl->attr.keySize);
        case HCF_ALG_MODE_GCM:
            return CipherGcmType(impl->attr.keySize);
        default:
            break;
    }
    return DefaultCiherType(impl->attr.keySize);
}

static bool IsGcmParamsValid(HcfGcmParamsSpec *params)
{
    if (params == NULL) {
        LOGE("params is null!");
        return false;
    }
    if ((params->aad.data == NULL) || (params->aad.len == 0) || (params->aad.len > MAX_AAD_LEN)) {
        LOGE("aad is invalid!");
        return false;
    }
    if ((params->iv.data == NULL) || (params->iv.len != GCM_IV_LEN)) {
        LOGE("iv is invalid!");
        return false;
    }
    if ((params->tag.data == NULL) || (params->tag.len == 0)) {
        LOGE("tag is invalid!");
        return false;
    }
    return true;
}

static bool IsCcmParamsValid(HcfCcmParamsSpec *params)
{
    if (params == NULL) {
        LOGE("params is null!");
        return false;
    }
    if ((params->aad.data == NULL) || (params->aad.len == 0) || (params->aad.len > MAX_AAD_LEN)) {
        LOGE("aad is invalid!");
        return false;
    }
    if ((params->iv.data == NULL) || (params->iv.len < CCM_IV_MIN_LEN) || (params->iv.len > CCM_IV_MAX_LEN)) {
        LOGE("iv is invalid!");
        return false;
    }
    if ((params->tag.data == NULL) || (params->tag.len == 0)) {
        LOGE("tag is invalid!");
        return false;
    }
    return true;
}

static HcfResult InitAadAndTagFromGcmParams(enum HcfCryptoMode opMode, HcfGcmParamsSpec *params, CipherData *data)
{
    if (!IsGcmParamsValid(params)) {
        LOGE("gcm params is invalid!");
        return HCF_INVALID_PARAMS;
    }

    data->aad = (uint8_t *)HcfMalloc(params->aad.len, 0);
    if (data->aad == NULL) {
        LOGE("aad malloc failed!");
        return HCF_ERR_MALLOC;
    }
    (void)memcpy_s(data->aad, params->aad.len, params->aad.data, params->aad.len);
    data->aadLen = params->aad.len;
    data->aead = true;
    data->tagLen = params->tag.len;
    if (opMode == ENCRYPT_MODE) {
        return HCF_SUCCESS;
    }
    data->tag = (uint8_t *)HcfMalloc(params->tag.len, 0);
    if (data->tag == NULL) {
        HcfFree(data->aad);
        data->aad = NULL;
        LOGE("tag malloc failed!");
        return HCF_ERR_MALLOC;
    }
    (void)memcpy_s(data->tag, params->tag.len, params->tag.data, params->tag.len);
    return HCF_SUCCESS;
}

static HcfResult InitAadAndTagFromCcmParams(enum HcfCryptoMode opMode, HcfCcmParamsSpec *params, CipherData *data)
{
    if (!IsCcmParamsValid(params)) {
        LOGE("gcm params is invalid!");
        return HCF_INVALID_PARAMS;
    }

    data->aad = (uint8_t *)HcfMalloc(params->aad.len, 0);
    if (data->aad == NULL) {
        LOGE("aad malloc failed!");
        return HCF_ERR_MALLOC;
    }
    (void)memcpy_s(data->aad, params->aad.len, params->aad.data, params->aad.len);
    data->aadLen = params->aad.len;
    data->aead = true;
    data->tagLen = params->tag.len;
    if (opMode == ENCRYPT_MODE) {
        return HCF_SUCCESS;
    }
    data->tag = (uint8_t *)HcfMalloc(params->tag.len, 0);
    if (data->tag == NULL) {
        HcfFree(data->aad);
        data->aad = NULL;
        LOGE("tag malloc failed!");
        return HCF_ERR_MALLOC;
    }
    (void)memcpy_s(data->tag, params->tag.len, params->tag.data, params->tag.len);
    return HCF_SUCCESS;
}

static HcfResult InitCipherData(HcfCipherGeneratorSpi *self, enum HcfCryptoMode opMode,
    HcfParamsSpec *params, CipherData **cipherData)
{
    HcfResult ret = HCF_ERR_MALLOC;
    *cipherData = (CipherData *)HcfMalloc(sizeof(CipherData), 0);
    if (*cipherData == NULL) {
        LOGE("malloc is failed!");
        return ret;
    }
    HcfCipherAesGeneratorSpiOpensslImpl *cipherImpl = (HcfCipherAesGeneratorSpiOpensslImpl *)self;
    HCF_ALG_PARA_VALUE mode = cipherImpl->attr.mode;

    (*cipherData)->enc = opMode;
    (*cipherData)->ctx = EVP_CIPHER_CTX_new();
    if ((*cipherData)->ctx == NULL) {
        HcfPrintOpensslError();
        LOGE("Failed to allocate ctx memory!");
        goto clearup;
    }

    ret = HCF_SUCCESS;
    if (mode == HCF_ALG_MODE_GCM) {
        ret = InitAadAndTagFromGcmParams(opMode, (HcfGcmParamsSpec *)params, *cipherData);
    } else if (mode == HCF_ALG_MODE_CCM) {
        ret = InitAadAndTagFromCcmParams(opMode, (HcfCcmParamsSpec *)params, *cipherData);
    }
    if (ret != HCF_SUCCESS) {
        LOGE("gcm or ccm init failed!");
        goto clearup;
    }
    return ret;
clearup:
    FreeCipherData(cipherData);
    return ret;
}

static HcfResult IsKeySizeMatchCipher(SymKeyImpl *keyImpl, HcfCipherAesGeneratorSpiOpensslImpl *cipherImpl)
{
    size_t keySize = keyImpl->keyMaterial.len;
    HCF_ALG_PARA_VALUE cipherValue = cipherImpl->attr.keySize;
    switch (cipherValue) {
        case HCF_ALG_AES_128:
            return (keySize < AES_SIZE_128) ? HCF_INVALID_PARAMS : HCF_SUCCESS;
        case HCF_ALG_AES_192:
            return (keySize < AES_SIZE_192) ? HCF_INVALID_PARAMS : HCF_SUCCESS;
        case HCF_ALG_AES_256:
            return (keySize < AES_SIZE_256) ? HCF_INVALID_PARAMS : HCF_SUCCESS;
        default:
            return HCF_INVALID_PARAMS;
    }
}

static HcfResult EngineCipherInit(HcfCipherGeneratorSpi *self, enum HcfCryptoMode opMode,
    HcfKey *key, HcfParamsSpec *params)
{
    if ((self == NULL) || (key == NULL)) { /* params maybe is null */
        LOGE("Invalid input parameter!");
        return HCF_INVALID_PARAMS;
    }
    if (!IsClassMatch((const HcfObjectBase *)self, GetAesGeneratorClass())) {
        LOGE("Class is not match.");
        return HCF_INVALID_PARAMS;
    }
    if (!IsClassMatch((const HcfObjectBase *)key, OPENSSL_SYM_KEY_CLASS)) {
        LOGE("Class is not match.");
        return HCF_INVALID_PARAMS;
    }
    HcfCipherAesGeneratorSpiOpensslImpl *cipherImpl = (HcfCipherAesGeneratorSpiOpensslImpl *)self;
    SymKeyImpl *keyImpl = (SymKeyImpl *)key;
    int enc = (opMode == ENCRYPT_MODE) ? 1 : 0;
    if (IsKeySizeMatchCipher(keyImpl, cipherImpl) != HCF_SUCCESS) {
        LOGE("Init failed, key size is smaller than cipher size.");
        return HCF_INVALID_PARAMS;
    }
    if (InitCipherData(self, opMode, params, &(cipherImpl->cipherData)) != HCF_SUCCESS) {
        LOGE("InitCipherData failed!");
        return HCF_INVALID_PARAMS;
    }

    CipherData *data = cipherImpl->cipherData;
    HcfResult ret = HCF_ERR_CRYPTO_OPERATION;
    if (EVP_CipherInit(data->ctx, GetCipherType(cipherImpl), keyImpl->keyMaterial.data, GetIv(params), enc) !=
        HCF_OPENSSL_SUCCESS) {
        HcfPrintOpensslError();
        LOGE("EVP_CipherInit failed!");
        goto clearup;
    }
    int32_t padding = (cipherImpl->attr.paddingMode == HCF_ALG_NOPADDING) ? 0 : EVP_PADDING_PKCS7;

    if (EVP_CIPHER_CTX_set_padding(data->ctx, padding) != HCF_OPENSSL_SUCCESS) {
        HcfPrintOpensslError();
        LOGE("set padding failed!");
        goto clearup;
    }

    if (opMode == ENCRYPT_MODE || cipherImpl->attr.mode != HCF_ALG_MODE_CCM) {
        return HCF_SUCCESS;
    }
    /* ccm decrypt need set tag */
    if (EVP_CIPHER_CTX_ctrl(data->ctx, EVP_CTRL_AEAD_SET_TAG, GetCcmTagLen(params), GetCcmTag(params)) !=
        HCF_OPENSSL_SUCCESS) {
        HcfPrintOpensslError();
        LOGE("set AuthTag failed!");
        goto clearup;
    }
    return HCF_SUCCESS;
clearup:
    FreeCipherData(&(cipherImpl->cipherData));
    return ret;
}

static HcfResult CommonUpdate(CipherData *data, HcfBlob *input, HcfBlob *output)
{
    int32_t ret = EVP_CipherUpdate(data->ctx, output->data, (int *)&output->len,
        input->data, input->len);
    if (ret != HCF_OPENSSL_SUCCESS) {
        HcfPrintOpensslError();
        LOGE("cipher update failed!");
        return HCF_ERR_CRYPTO_OPERATION;
    }
    return HCF_SUCCESS;
}

static HcfResult AeadUpdate(CipherData *data, HCF_ALG_PARA_VALUE mode, HcfBlob *input, HcfBlob *output)
{
    if (mode == HCF_ALG_MODE_CCM) {
        if (EVP_CipherUpdate(data->ctx, NULL, (int *)&output->len, NULL, input->len) != HCF_OPENSSL_SUCCESS) {
            HcfPrintOpensslError();
            LOGE("ccm cipher update failed!");
            return HCF_ERR_CRYPTO_OPERATION;
        }
    }

    int32_t ret = EVP_CipherUpdate(data->ctx, NULL, (int *)&output->len, data->aad, data->aadLen);
    if (ret != HCF_OPENSSL_SUCCESS) {
        HcfPrintOpensslError();
        LOGE("aad cipher update failed!");
        return HCF_ERR_CRYPTO_OPERATION;
    }
    ret = EVP_CipherUpdate(data->ctx, output->data, (int *)&output->len, input->data, input->len);
    if (ret != HCF_OPENSSL_SUCCESS) {
        HcfPrintOpensslError();
        LOGE("gcm cipher update failed!");
        return HCF_ERR_CRYPTO_OPERATION;
    }
    return HCF_SUCCESS;
}

static HcfResult AllocateOutput(HcfBlob *input, HcfBlob *output, bool *isUpdateInput)
{
    uint32_t outLen = AES_BLOCK_SIZE;
    if (IsBlobValid(input)) {
        outLen += input->len;
        *isUpdateInput = true;
    }
    output->data = (uint8_t *)HcfMalloc(outLen, 0);
    if (output->data == NULL) {
        LOGE("malloc output failed!");
        return HCF_ERR_MALLOC;
    }
    output->len = outLen;
    return HCF_SUCCESS;
}

static HcfResult EngineUpdate(HcfCipherGeneratorSpi *self, HcfBlob *input, HcfBlob *output)
{
    if ((self == NULL) || (input == NULL) || (output == NULL)) {
        LOGE("Invalid input parameter!");
        return HCF_INVALID_PARAMS;
    }
    if (!IsClassMatch((const HcfObjectBase *)self, GetAesGeneratorClass())) {
        LOGE("Class is not match.");
        return HCF_INVALID_PARAMS;
    }

    HcfCipherAesGeneratorSpiOpensslImpl *cipherImpl = (HcfCipherAesGeneratorSpiOpensslImpl *)self;
    CipherData *data = cipherImpl->cipherData;
    if (data == NULL) {
        LOGE("cipherData is null!");
        return HCF_INVALID_PARAMS;
    }
    bool isUpdateInput = false;
    HcfResult ret = AllocateOutput(input, output, &isUpdateInput);
    if (ret != HCF_SUCCESS) {
        LOGE("AllocateOutput failed!");
        return ret;
    }

    if (!data->aead) {
        ret = CommonUpdate(data, input, output);
    } else {
        ret = AeadUpdate(data, cipherImpl->attr.mode, input, output);
    }
    if (ret != HCF_SUCCESS) {
        HcfBlobDataFree(output);
        FreeCipherData(&(cipherImpl->cipherData));
    }
    data->aead = false;
    FreeRedundantOutput(output);
    return ret;
}

static HcfResult CommonDoFinal(CipherData *data, HcfBlob *input, HcfBlob *output)
{
    int32_t ret;
    uint32_t len = 0;
    bool isUpdateInput = false;
    HcfResult res = AllocateOutput(input, output, &isUpdateInput);
    if (res != HCF_SUCCESS) {
        LOGE("AllocateOutput failed!");
        return res;
    }
    if (isUpdateInput) {
        ret = EVP_CipherUpdate(data->ctx, output->data, (int32_t *)&len, input->data, input->len);
        if (ret != HCF_OPENSSL_SUCCESS) {
            HcfPrintOpensslError();
            LOGE("EVP_CipherUpdate failed!");
            return HCF_ERR_CRYPTO_OPERATION;
        }
    }
    ret = EVP_CipherFinal_ex(data->ctx, output->data + len, (int *)&output->len);
    if (ret != HCF_OPENSSL_SUCCESS) {
        HcfPrintOpensslError();
        LOGE("EVP_CipherFinal_ex failed!");
        return HCF_ERR_CRYPTO_OPERATION;
    }
    output->len += len;
    return HCF_SUCCESS;
}

static HcfResult AllocateCcmOutput(CipherData *data, HcfBlob *input, HcfBlob *output, bool *isUpdateInput)
{
    uint32_t outLen = 0;
    if (IsBlobValid(input)) {
        outLen += input->len;
        *isUpdateInput = true;
    }
    int32_t authTagLen = data->enc == ENCRYPT_MODE ? CCM_TAG_SIZE : 0;
    outLen += authTagLen + AES_BLOCK_SIZE;
    if (outLen == 0) {
        LOGE("output size is invaild!");
        return HCF_INVALID_PARAMS;
    }
    output->data = (uint8_t *)HcfMalloc(outLen, 0);
    if (output->data == NULL) {
        LOGE("malloc output failed!");
        return HCF_ERR_MALLOC;
    }
    output->len = outLen;
    return HCF_SUCCESS;
}

static HcfResult CcmDecryptDoFinal(HcfBlob *output, bool isUpdateInput)
{
    if (isUpdateInput) { /* DecryptFinal this does not occur in CCM mode */
        return HCF_SUCCESS;
    }
    if (output->data != NULL) {
        HcfBlobDataFree(output);
    }
    return HCF_SUCCESS;
}

static HcfResult CcmEncryptDoFinal(CipherData *data, HcfBlob *output, uint32_t len)
{
    int32_t ret = EVP_CIPHER_CTX_ctrl(data->ctx, EVP_CTRL_AEAD_GET_TAG, data->tagLen, output->data + len);
    if (ret != HCF_OPENSSL_SUCCESS) {
        HcfPrintOpensslError();
        LOGE("get AuthTag failed!");
        return HCF_ERR_CRYPTO_OPERATION;
    }
    output->len = data->tagLen + len;
    return HCF_SUCCESS;
}

static HcfResult CcmDoFinal(CipherData *data, HcfBlob *input, HcfBlob *output)
{
    bool isUpdateInput = false;
    uint32_t len = 0;
    HcfResult res = AllocateCcmOutput(data, input, output, &isUpdateInput);
    if (res != HCF_SUCCESS) {
        LOGE("AllocateCcmOutput failed!");
        return res;
    }
    if (isUpdateInput) {
        HcfResult result = AeadUpdate(data, HCF_ALG_MODE_CCM, input, output);
        if (result != HCF_SUCCESS) {
            LOGE("AeadUpdate failed!");
            return result;
        }
        len = output->len;
    }
    if (data->enc == ENCRYPT_MODE) {
        return CcmEncryptDoFinal(data, output, len);
    } else if (data->enc == DECRYPT_MODE) {
        return CcmDecryptDoFinal(output, isUpdateInput);
    } else {
        return HCF_INVALID_PARAMS;
    }
}

static HcfResult GcmDecryptDoFinal(CipherData *data, HcfBlob *input, HcfBlob *output, uint32_t len)
{
    if (data->tag == NULL) {
        LOGE("gcm decrypt has not AuthTag!");
        return HCF_INVALID_PARAMS;
    }
    int32_t ret = EVP_CIPHER_CTX_ctrl(data->ctx, EVP_CTRL_AEAD_SET_TAG, data->tagLen, (void *)data->tag);
    if (ret != HCF_OPENSSL_SUCCESS) {
        HcfPrintOpensslError();
        LOGE("gcm decrypt set AuthTag failed!");
        return HCF_ERR_CRYPTO_OPERATION;
    }
    ret = EVP_CipherFinal_ex(data->ctx, output->data + len, (int *)&output->len);
    if (ret != HCF_OPENSSL_SUCCESS) {
        HcfPrintOpensslError();
        LOGE("EVP_CipherFinal_ex failed!");
        return HCF_ERR_CRYPTO_OPERATION;
    }
    output->len = output->len + len;
    return HCF_SUCCESS;
}

static HcfResult GcmEncryptDoFinal(CipherData *data, HcfBlob *input, HcfBlob *output, uint32_t len)
{
    int32_t ret = EVP_CipherFinal_ex(data->ctx, output->data + len, (int *)&output->len);
    if (ret != HCF_OPENSSL_SUCCESS) {
        HcfPrintOpensslError();
        LOGE("EVP_CipherFinal_ex failed!");
        return HCF_ERR_CRYPTO_OPERATION;
    }
    output->len += len;
    ret = EVP_CIPHER_CTX_ctrl(data->ctx, EVP_CTRL_AEAD_GET_TAG, data->tagLen,
        output->data + output->len);
    if (ret != HCF_OPENSSL_SUCCESS) {
        HcfPrintOpensslError();
        LOGE("get AuthTag failed!");
        return HCF_ERR_CRYPTO_OPERATION;
    }
    output->len += data->tagLen;
    return HCF_SUCCESS;
}

static HcfResult AllocateGcmOutput(CipherData *data, HcfBlob *input, HcfBlob *output, bool *isUpdateInput)
{
    uint32_t outLen = 0;
    if (IsBlobValid(input)) {
        outLen += input->len;
        *isUpdateInput = true;
    }
    int32_t authTagLen = data->enc == ENCRYPT_MODE ? GCM_TAG_SIZE : 0;
    outLen += data->updateLen + authTagLen + AES_BLOCK_SIZE;
    if (outLen == 0) {
        LOGE("output size is invaild!");
        return HCF_INVALID_PARAMS;
    }
    output->data = (uint8_t *)HcfMalloc(outLen, 0);
    if (output->data == NULL) {
        LOGE("malloc output failed!");
        return HCF_ERR_MALLOC;
    }
    output->len = outLen;
    return HCF_SUCCESS;
}

static HcfResult GcmDoFinal(CipherData *data, HcfBlob *input, HcfBlob *output)
{
    uint32_t len = 0;
    bool isUpdateInput = false;
    HcfResult res = AllocateGcmOutput(data, input, output, &isUpdateInput);
    if (res != HCF_SUCCESS) {
        LOGE("AllocateGcmOutput failed!");
        return res;
    }

    if (isUpdateInput) {
        HcfResult result = AeadUpdate(data, HCF_ALG_MODE_GCM, input, output);
        if (result != HCF_SUCCESS) {
            LOGE("AeadUpdate failed!");
            return result;
        }
        len = output->len;
    }
    if (data->enc == ENCRYPT_MODE) {
        return GcmEncryptDoFinal(data, input, output, len);
    } else if (data->enc == DECRYPT_MODE) {
        return GcmDecryptDoFinal(data, input, output, len);
    } else {
        return HCF_INVALID_PARAMS;
    }
}

static HcfResult EngineDoFinal(HcfCipherGeneratorSpi *self, HcfBlob *input, HcfBlob *output)
{
    if ((self == NULL) || (output == NULL)) { /* input maybe is null */
        LOGE("Invalid input parameter!");
        return HCF_INVALID_PARAMS;
    }
    if (!IsClassMatch((const HcfObjectBase *)self, GetAesGeneratorClass())) {
        LOGE("Class is not match.");
        return HCF_INVALID_PARAMS;
    }
    HcfResult ret = HCF_ERR_CRYPTO_OPERATION;
    HcfCipherAesGeneratorSpiOpensslImpl *cipherImpl = (HcfCipherAesGeneratorSpiOpensslImpl *)self;
    CipherData *data = cipherImpl->cipherData;
    HCF_ALG_PARA_VALUE mode = cipherImpl->attr.mode;
    if (data == NULL) {
        LOGE("cipherData is null!");
        return HCF_INVALID_PARAMS;
    }

    if (mode == HCF_ALG_MODE_CCM) {
        ret = CcmDoFinal(data, input, output);
    } else if (mode == HCF_ALG_MODE_GCM) {
        ret = GcmDoFinal(data, input, output);
    } else { /* only ECB CBC CTR CFB OFB support */
        ret = CommonDoFinal(data, input, output);
    }

    FreeCipherData(&(cipherImpl->cipherData));
    if (ret != HCF_SUCCESS) {
        HcfBlobDataFree(output);
    }
    FreeRedundantOutput(output);
    return ret;
}

static void EngineAesGeneratorDestroy(HcfObjectBase *self)
{
    if (self == NULL) {
        return;
    }
    if (!IsClassMatch(self, GetAesGeneratorClass())) {
        LOGE("Class is not match.");
        return;
    }

    HcfCipherAesGeneratorSpiOpensslImpl *impl = (HcfCipherAesGeneratorSpiOpensslImpl *)self;
    FreeCipherData(&(impl->cipherData));
    HcfFree(impl);
}

HcfResult HcfCipherAesGeneratorSpiCreate(CipherAttr *attr, HcfCipherGeneratorSpi **generator)
{
    if ((attr == NULL) || (generator == NULL)) {
        LOGE("Invalid input parameter.");
        return HCF_INVALID_PARAMS;
    }
    HcfCipherAesGeneratorSpiOpensslImpl *returnImpl = (HcfCipherAesGeneratorSpiOpensslImpl *)HcfMalloc(
        sizeof(HcfCipherAesGeneratorSpiOpensslImpl), 0);
    if (returnImpl == NULL) {
        LOGE("Failed to allocate returnImpl memroy!");
        return HCF_ERR_MALLOC;
    }
    (void)memcpy_s(&returnImpl->attr, sizeof(CipherAttr), attr, sizeof(CipherAttr));
    returnImpl->base.init = EngineCipherInit;
    returnImpl->base.update = EngineUpdate;
    returnImpl->base.doFinal = EngineDoFinal;
    returnImpl->base.base.destroy = EngineAesGeneratorDestroy;
    returnImpl->base.base.getClass = GetAesGeneratorClass;

    *generator = (HcfCipherGeneratorSpi *)returnImpl;
    return HCF_SUCCESS;
}
