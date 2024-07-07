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

#include <openssl/des.h>
#include "log.h"
#include "blob.h"
#include "memory.h"
#include "result.h"
#include "utils.h"
#include "securec.h"
#include "aes_openssl_common.h"
#include "sym_common_defines.h"
#include "openssl_common.h"
#include "openssl_class.h"

#define DES_BLOCK_SIZE 8
#define DES_SIZE_192 24

typedef struct {
    HcfCipherGeneratorSpi base;
    CipherAttr attr;
    CipherData *cipherData;
} HcfCipherDesGeneratorSpiOpensslImpl;

static const char *GetDesGeneratorClass(void)
{
    return OPENSSL_3DES_CIPHER_CLASS;
}

static const EVP_CIPHER *DefautCipherType(void)
{
    return EVP_des_ede3_ecb();
}

static const EVP_CIPHER *GetCipherType(HcfCipherDesGeneratorSpiOpensslImpl *impl)
{
    switch (impl->attr.mode) {
        case HCF_ALG_MODE_ECB:
            return EVP_des_ede3_ecb();
        case HCF_ALG_MODE_CBC:
            return EVP_des_ede3_cbc();
        case HCF_ALG_MODE_OFB:
            return EVP_des_ede3_ofb();
        case HCF_ALG_MODE_CFB:
        case HCF_ALG_MODE_CFB64:
            return EVP_des_ede3_cfb64();
        case HCF_ALG_MODE_CFB1:
            return EVP_des_ede3_cfb1();
        case HCF_ALG_MODE_CFB8:
            return EVP_des_ede3_cfb8();
        default:
            break;
    }
    return DefautCipherType();
}

static HcfResult InitCipherData(enum HcfCryptoMode opMode, CipherData **cipherData)
{
    HcfResult ret = HCF_INVALID_PARAMS;

    *cipherData = (CipherData *)HcfMalloc(sizeof(CipherData), 0);
    if (*cipherData == NULL) {
        LOGE("malloc failed!");
        return HCF_ERR_MALLOC;
    }

    (*cipherData)->enc = opMode;
    (*cipherData)->ctx = EVP_CIPHER_CTX_new();
    if ((*cipherData)->ctx == NULL) {
        HcfPrintOpensslError();
        LOGE("Failed to allocate ctx memroy!");
        goto clearup;
    }

    ret = HCF_SUCCESS;
    return ret;
clearup:
    FreeCipherData(cipherData);
    return ret;
}

static HcfResult EngineCipherInit(HcfCipherGeneratorSpi *self, enum HcfCryptoMode opMode,
    HcfKey *key, HcfParamsSpec *params)
{
    if ((self == NULL) || (key == NULL)) { /* params maybe is null */
        LOGE("Invalid input parameter!");
        return HCF_INVALID_PARAMS;
    }
    if (!IsClassMatch((const HcfObjectBase *)self, GetDesGeneratorClass())) {
        LOGE("Class is not match.");
        return HCF_INVALID_PARAMS;
    }
    if (!IsClassMatch((const HcfObjectBase *)key, OPENSSL_SYM_KEY_CLASS)) {
        LOGE("Class is not match.");
        return HCF_INVALID_PARAMS;
    }

    HcfCipherDesGeneratorSpiOpensslImpl *cipherImpl = (HcfCipherDesGeneratorSpiOpensslImpl *)self;
    SymKeyImpl *keyImpl = (SymKeyImpl *)key;
    int32_t enc = (opMode == ENCRYPT_MODE) ? 1 : 0;

    if (keyImpl->keyMaterial.len < DES_SIZE_192) {
        LOGE("Init failed, the input key size is smaller than keySize specified in cipher.");
        return HCF_INVALID_PARAMS;
    }
    if (InitCipherData(opMode,  &(cipherImpl->cipherData)) != HCF_SUCCESS) {
        LOGE("InitCipherData failed");
        return HCF_INVALID_PARAMS;
    }
    HcfResult ret = HCF_ERR_CRYPTO_OPERATION;
    CipherData *data = cipherImpl->cipherData;
    if (EVP_CipherInit(data->ctx, GetCipherType(cipherImpl), NULL, NULL, enc) != HCF_OPENSSL_SUCCESS) {
        HcfPrintOpensslError();
        LOGE("cipher init failed!");
        goto clearup;
    }
    if (EVP_CipherInit(data->ctx, NULL, keyImpl->keyMaterial.data, GetIv(params), enc) != HCF_OPENSSL_SUCCESS) {
        HcfPrintOpensslError();
        LOGE("cipher init key and iv failed!");
        goto clearup;
    }
    int32_t padding = (cipherImpl->attr.paddingMode == HCF_ALG_NOPADDING) ? 0 : EVP_PADDING_PKCS7;
    if (EVP_CIPHER_CTX_set_padding(data->ctx, padding) != HCF_OPENSSL_SUCCESS) {
        HcfPrintOpensslError();
        LOGE("set padding!");
        goto clearup;
    }
    return HCF_SUCCESS;
clearup:
    FreeCipherData(&(cipherImpl->cipherData));
    return ret;
}

static HcfResult AllocateOutput(HcfBlob *input, HcfBlob *output)
{
    uint32_t outLen = DES_BLOCK_SIZE;
    if (IsBlobValid(input)) {
        outLen += input->len;
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
    if (!IsClassMatch((const HcfObjectBase *)self, GetDesGeneratorClass())) {
        LOGE("Class is not match.");
        return HCF_INVALID_PARAMS;
    }

    HcfCipherDesGeneratorSpiOpensslImpl *cipherImpl = (HcfCipherDesGeneratorSpiOpensslImpl *)self;
    CipherData *data = cipherImpl->cipherData;
    if (data == NULL) {
        LOGE("cipherData is null!");
        return HCF_INVALID_PARAMS;
    }
    HcfResult res = AllocateOutput(input, output);
    if (res != HCF_SUCCESS) {
        LOGE("AllocateOutput failed!");
        goto clearup;
    }

    int32_t ret = EVP_CipherUpdate(data->ctx, output->data, (int *)&output->len,
        input->data, input->len);
    if (ret != HCF_OPENSSL_SUCCESS) {
        HcfPrintOpensslError();
        LOGE("cipher update failed!");
        res = HCF_ERR_CRYPTO_OPERATION;
        goto clearup;
    }
    res = HCF_SUCCESS;
clearup:
    if (res != HCF_SUCCESS) {
        HcfBlobDataFree(output);
        FreeCipherData(&(cipherImpl->cipherData));
    } else {
        FreeRedundantOutput(output);
    }
    return res;
}

static HcfResult DesDoFinal(CipherData *data, HcfBlob *input, HcfBlob *output)
{
    int32_t ret;
    uint32_t len = 0;

    if (IsBlobValid(input)) {
        ret = EVP_CipherUpdate(data->ctx, output->data, (int *)&output->len,
            input->data, input->len);
        if (ret != HCF_OPENSSL_SUCCESS) {
            HcfPrintOpensslError();
            LOGE("cipher update failed!");
            return HCF_ERR_CRYPTO_OPERATION;
        }
        len += output->len;
    }
    ret = EVP_CipherFinal_ex(data->ctx, output->data + len, (int *)&output->len);
    if (ret != HCF_OPENSSL_SUCCESS) {
        HcfPrintOpensslError();
        LOGE("cipher final filed!");
        return HCF_ERR_CRYPTO_OPERATION;
    }
    output->len += len;
    return HCF_SUCCESS;
}

static HcfResult EngineDoFinal(HcfCipherGeneratorSpi *self, HcfBlob *input, HcfBlob *output)
{
    if ((self == NULL) || (output == NULL)) { /* input maybe is null */
        LOGE("Invalid input parameter!");
        return HCF_INVALID_PARAMS;
    }
    if (!IsClassMatch((const HcfObjectBase *)self, GetDesGeneratorClass())) {
        LOGE("Class is not match.");
        return HCF_INVALID_PARAMS;
    }
    HcfCipherDesGeneratorSpiOpensslImpl *cipherImpl = (HcfCipherDesGeneratorSpiOpensslImpl *)self;
    CipherData *data = cipherImpl->cipherData;
    if (data == NULL) {
        LOGE("cipherData is null!");
        return HCF_INVALID_PARAMS;
    }

    HcfResult res = AllocateOutput(input, output);
    if (res != HCF_SUCCESS) {
        LOGE("AllocateOutput failed!");
        goto clearup;
    }
    res = DesDoFinal(data, input, output);
    if (res != HCF_SUCCESS) {
        LOGE("DesDoFinal failed!");
    }
clearup:
    if (res != HCF_SUCCESS) {
        HcfBlobDataFree(output);
    } else {
        FreeRedundantOutput(output);
    }
    FreeCipherData(&(cipherImpl->cipherData));
    return res;
}

static void EngineDesGeneratorDestroy(HcfObjectBase *self)
{
    if (self == NULL) {
        return;
    }
    if (!IsClassMatch(self, GetDesGeneratorClass())) {
        LOGE("Class is not match.");
        return;
    }
    HcfCipherDesGeneratorSpiOpensslImpl *impl = (HcfCipherDesGeneratorSpiOpensslImpl *)self;
    FreeCipherData(&(impl->cipherData));
    HcfFree(impl);
}

HcfResult HcfCipherDesGeneratorSpiCreate(CipherAttr *attr, HcfCipherGeneratorSpi **generator)
{
    if ((attr == NULL) || (generator == NULL)) {
        LOGE("Invalid input parameter!");
        return HCF_INVALID_PARAMS;
    }
    HcfCipherDesGeneratorSpiOpensslImpl *returnImpl = (HcfCipherDesGeneratorSpiOpensslImpl *)HcfMalloc(
        sizeof(HcfCipherDesGeneratorSpiOpensslImpl), 0);
    if (returnImpl == NULL) {
        LOGE("Failed to allocate returnImpl memroy!");
        return HCF_ERR_MALLOC;
    }
    (void)memcpy_s(&returnImpl->attr, sizeof(CipherAttr), attr, sizeof(CipherAttr));
    returnImpl->base.init = EngineCipherInit;
    returnImpl->base.update = EngineUpdate;
    returnImpl->base.doFinal = EngineDoFinal;
    returnImpl->base.base.destroy = EngineDesGeneratorDestroy;
    returnImpl->base.base.getClass = GetDesGeneratorClass;

    *generator = (HcfCipherGeneratorSpi *)returnImpl;
    return HCF_SUCCESS;
}
