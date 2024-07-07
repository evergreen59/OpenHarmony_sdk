/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#include "cipher.h"
#include "aes_openssl.h"
#include "config.h"
#include "aes_openssl_common.h"
#include "securec.h"
#include "result.h"
#include "string.h"
#include "log.h"
#include "memory.h"
#include "cipher_rsa_openssl.h"
#include "utils.h"

typedef HcfResult (*HcfCipherGeneratorSpiCreateFunc)(CipherAttr *, HcfCipherGeneratorSpi **);

typedef struct {
    HcfCipher super;
    HcfCipherGeneratorSpi *spiObj;
    char algoName[HCF_MAX_ALGO_NAME_LEN];
} CipherGenImpl;

typedef struct {
    HcfCipherGeneratorSpiCreateFunc createFunc;
} HcfCipherGenFuncSet;

typedef struct {
    HCF_ALG_VALUE algo;
    HcfCipherGenFuncSet funcSet;
} HcfCipherGenAbility;

static const HcfCipherGenAbility CIPHER_ABILITY_SET[] = {
    { HCF_ALG_RSA, { HcfCipherRsaCipherSpiCreate } },
    { HCF_ALG_AES, { HcfCipherAesGeneratorSpiCreate } },
    { HCF_ALG_DES, { HcfCipherDesGeneratorSpiCreate } }
};

static void SetKeyLength(HCF_ALG_PARA_VALUE value, void *cipher)
{
    CipherAttr *cipherAttr = (CipherAttr *)cipher;

    cipherAttr->keySize = value;

    switch (value) {
        case HCF_ALG_AES_128:
        case HCF_ALG_AES_192:
        case HCF_ALG_AES_256:
            cipherAttr->algo = HCF_ALG_AES;
            break;
        case HCF_ALG_3DES_192:
            cipherAttr->algo = HCF_ALG_DES;
            break;
        case HCF_OPENSSL_RSA_512:
        case HCF_OPENSSL_RSA_768:
        case HCF_OPENSSL_RSA_1024:
        case HCF_OPENSSL_RSA_2048:
        case HCF_OPENSSL_RSA_3072:
        case HCF_OPENSSL_RSA_4096:
        case HCF_OPENSSL_RSA_8192:
            cipherAttr->algo = HCF_ALG_RSA;
            break;
        default:
            LOGE("Invalid algo %u.", value);
            break;
    }
}

static void SetMode(HCF_ALG_PARA_VALUE value, void *cipher)
{
    CipherAttr *cipherAttr = (CipherAttr *)cipher;
    cipherAttr->mode = value ;
}

static void SetPadding(HCF_ALG_PARA_VALUE value, void *cipher)
{
    CipherAttr *cipherAttr = (CipherAttr *)cipher;
    cipherAttr->paddingMode = value;
}

static void SetDigest(HCF_ALG_PARA_VALUE value, CipherAttr *cipher)
{
    cipher->md = value;
}

static void SetMgf1Digest(HCF_ALG_PARA_VALUE value, CipherAttr *cipher)
{
    cipher->mgf1md = value;
}

static HcfResult OnSetParameter(const HcfParaConfig *config, void *cipher)
{
    if ((config == NULL) || (cipher == NULL)) {
        return HCF_INVALID_PARAMS;
    }
    HcfResult ret = HCF_SUCCESS;
    LOGD("Set Parameter:%s", config->tag);
    switch (config->paraType) {
        case HCF_ALG_TYPE:
        case HCF_ALG_KEY_TYPE:
            SetKeyLength(config->paraValue, cipher);
            break;
        case HCF_ALG_MODE:
            SetMode(config->paraValue, cipher);
            break;
        case HCF_ALG_PADDING_TYPE:
            SetPadding(config->paraValue, cipher);
            break;
        case HCF_ALG_DIGEST:
            SetDigest(config->paraValue, cipher);
            break;
        case HCF_ALG_MGF1_DIGEST:
            SetMgf1Digest(config->paraValue, cipher);
            break;
        default:
            ret = HCF_INVALID_PARAMS;
            break;
    }
    return ret;
}

static const char *GetCipherGeneratorClass(void)
{
    return "HcfCipherGenerator";
}

const char *GetAlogrithm(HcfCipher *self)
{
    if (self == NULL) {
        LOGE("The input self ptr is NULL!");
        return NULL;
    }
    if (!IsClassMatch((HcfObjectBase *)self, GetCipherGeneratorClass())) {
        LOGE("Class is not match.");
        return NULL;
    }
    return ((CipherGenImpl *)self)->algoName;
}

static void CipherDestroy(HcfObjectBase *self)
{
    if (self == NULL) {
        return;
    }
    if (!IsClassMatch(self, GetCipherGeneratorClass())) {
        LOGE("Class not match.");
        return;
    }
    CipherGenImpl *impl = (CipherGenImpl *)self;
    HcfObjDestroy(impl->spiObj);
    HcfFree(impl);
}

static HcfResult CipherInit(HcfCipher *self, enum HcfCryptoMode opMode,
    HcfKey *key, HcfParamsSpec *params)
{
    if (self == NULL || key == NULL) { /* params maybe is NULL */
        LOGE("Invalid input parameter.");
        return HCF_INVALID_PARAMS;
    }
    if (!IsClassMatch((HcfObjectBase *)self, GetCipherGeneratorClass())) {
        LOGE("Class is not match.");
        return HCF_INVALID_PARAMS;
    }
    CipherGenImpl *impl = (CipherGenImpl *)self;
    return impl->spiObj->init(impl->spiObj, opMode, key, params);
}

static HcfResult CipherUpdate(HcfCipher *self, HcfBlob *input, HcfBlob *output)
{
    if ((self == NULL) || (input == NULL) || (output == NULL)) {
        LOGE("Invalid input parameter.");
        return HCF_INVALID_PARAMS;
    }
    if (!IsClassMatch((HcfObjectBase *)self, GetCipherGeneratorClass())) {
        LOGE("Class is not match.");
        return HCF_INVALID_PARAMS;
    }
    CipherGenImpl *impl = (CipherGenImpl *)self;
    return impl->spiObj->update(impl->spiObj, input, output);
}

static HcfResult CipherFinal(HcfCipher *self, HcfBlob *input, HcfBlob *output)
{
    if ((self == NULL) || (output == NULL)) {
        LOGE("Invalid input parameter!");
        return HCF_INVALID_PARAMS;
    }
    if (!IsClassMatch((HcfObjectBase *)self, GetCipherGeneratorClass())) {
        LOGE("Class is not match.");
        return HCF_INVALID_PARAMS;
    }
    CipherGenImpl *impl = (CipherGenImpl *)self;
    return impl->spiObj->doFinal(impl->spiObj, input, output);
}

static void InitCipher(HcfCipherGeneratorSpi *spiObj, CipherGenImpl *cipher)
{
    cipher->super.init = CipherInit;
    cipher->super.update = CipherUpdate;
    cipher->super.doFinal = CipherFinal;
    cipher->super.getAlgorithm = GetAlogrithm;
    cipher->super.base.destroy = CipherDestroy;
    cipher->super.base.getClass = GetCipherGeneratorClass;
}

static const HcfCipherGenFuncSet *FindAbility(CipherAttr *attr)
{
    if (attr == NULL) {
        return NULL;
    }
    for (uint32_t i = 0; i < sizeof(CIPHER_ABILITY_SET) / sizeof(HcfCipherGenAbility); i++) {
        if (CIPHER_ABILITY_SET[i].algo == attr->algo) {
            return &(CIPHER_ABILITY_SET[i].funcSet);
        }
    }
    LOGE("Algo not support! [Algo]: %d", attr->algo);
    return NULL;
}

HcfResult HcfCipherCreate(const char *transformation, HcfCipher **returnObj)
{
    CipherAttr attr = {0};
    if (!IsStrValid(transformation, HCF_MAX_ALGO_NAME_LEN) || (returnObj == NULL)) {
        LOGE("Invalid input params while creating cipher!");
        return HCF_INVALID_PARAMS;
    }
    if (ParseAndSetParameter(transformation, (void *)&attr, OnSetParameter) != HCF_SUCCESS) {
        LOGE("ParseAndSetParameter failed!");
        return HCF_NOT_SUPPORT;
    }

    const HcfCipherGenFuncSet *funcSet = FindAbility(&attr);
    if (funcSet == NULL) {
        LOGE("FindAbility failed!");
        return HCF_NOT_SUPPORT;
    }
    CipherGenImpl *returnGenerator = (CipherGenImpl *)HcfMalloc(sizeof(CipherGenImpl), 0);
    if (returnGenerator == NULL) {
        LOGE("failed to allocate returnGenerator memory!");
        return HCF_ERR_MALLOC;
    }
    if (strcpy_s(returnGenerator->algoName, HCF_MAX_ALGO_NAME_LEN, transformation) != EOK) {
        LOGE("Failed to copy algoName!");
        HcfFree(returnGenerator);
        return HCF_ERR_COPY;
    }
    HcfCipherGeneratorSpi *spiObj = NULL;
    HcfResult res = funcSet->createFunc(&attr, &spiObj);
    if (res != HCF_SUCCESS) {
        LOGE("Failed to create spi object!");
        HcfFree(returnGenerator);
        return res;
    }
    returnGenerator->spiObj = spiObj;
    InitCipher(spiObj, returnGenerator);

    *returnObj = (HcfCipher *)returnGenerator;
    return res;
}
