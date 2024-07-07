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

#include "sym_key_generator.h"
#include "sym_key_factory_spi.h"
#include "sym_common_defines.h"
#include "params_parser.h"
#include "utils.h"

#include <securec.h>
#include "log.h"
#include "memory.h"
#include "result.h"
#include "config.h"

#define AES_KEY_SIZE_128 128
#define AES_KEY_SIZE_192 192
#define AES_KEY_SIZE_256 256
#define DES_KEY_SIZE_192 192

typedef HcfResult (*SymKeyGeneratorSpiCreateFunc)(SymKeyAttr *, HcfSymKeyGeneratorSpi **);

typedef struct {
    SymKeyGeneratorSpiCreateFunc createFunc;
} SymKeyGenFuncSet;

typedef struct {
    HCF_ALG_VALUE algo;
    SymKeyGenFuncSet funcSet;
} SymKeyGenAbility;

typedef struct {
    HcfSymKeyGenerator base;
    HcfSymKeyGeneratorSpi *spiObj;
    char algoName[HCF_MAX_ALGO_NAME_LEN];
} HcfSymmKeyGeneratorImpl;

static const SymKeyGenAbility SYMKEY_ABILITY_SET[] = {
    { HCF_ALG_AES, { HcfSymKeyGeneratorSpiCreate }},
    { HCF_ALG_DES, { HcfSymKeyGeneratorSpiCreate }}
};

static const SymKeyGenFuncSet *FindAbility(SymKeyAttr *attr)
{
    if (attr == NULL) {
        return NULL;
    }
    for (uint32_t i = 0; i < sizeof(SYMKEY_ABILITY_SET) / sizeof(SymKeyGenAbility); i++) {
        if (SYMKEY_ABILITY_SET[i].algo == attr->algo) {
            return &(SYMKEY_ABILITY_SET[i].funcSet);
        }
    }
    LOGE("Algo not support! [Algo]: %d", attr->algo);
    return NULL;
}

static void SetKeyLength(HCF_ALG_PARA_VALUE value, void *attr)
{
    SymKeyAttr *keyAttr = (SymKeyAttr *)attr;

    switch (value) {
        case HCF_ALG_AES_128:
            keyAttr->algo = HCF_ALG_AES;
            keyAttr->keySize = AES_KEY_SIZE_128;
            break;
        case HCF_ALG_AES_192:
            keyAttr->algo = HCF_ALG_AES;
            keyAttr->keySize = AES_KEY_SIZE_192;
            break;
        case HCF_ALG_AES_256:
            keyAttr->algo = HCF_ALG_AES;
            keyAttr->keySize = AES_KEY_SIZE_256;
            break;
        case HCF_ALG_3DES_192:
            keyAttr->algo = HCF_ALG_DES;
            keyAttr->keySize = DES_KEY_SIZE_192;
            break;
        default:
            break;
    }
}

static HcfResult OnSetSymKeyParameter(const HcfParaConfig* config, void *attr)
{
    if ((config == NULL) || (attr == NULL)) {
        return HCF_INVALID_PARAMS;
    }
    HcfResult ret = HCF_SUCCESS;
    LOGD("Set Parameter:%s\n", config->tag);
    switch (config->paraType) {
        case HCF_ALG_TYPE:
        case HCF_ALG_KEY_TYPE:
            SetKeyLength(config->paraValue, attr);
            break;
        default:
            ret = HCF_INVALID_PARAMS;
            break;
    }
    return ret;
}

static const char *GetSymKeyGeneratorClass(void)
{
    return "HcfSymKeyGenerator";
}

static const char *GetAlgoName(HcfSymKeyGenerator *self)
{
    if (self == NULL) {
        LOGE("The input self ptr is NULL!");
        return NULL;
    }
    if (!IsClassMatch((HcfObjectBase *)self, GetSymKeyGeneratorClass())) {
        LOGE("Class is not match!");
        return NULL;
    }
    return ((HcfSymmKeyGeneratorImpl *)self)->algoName;
}

static void DestroySymmKeyGenerator(HcfObjectBase *base)
{
    if (base == NULL) {
        return;
    }
    if (!IsClassMatch((HcfObjectBase *)base, GetSymKeyGeneratorClass())) {
        LOGE("Class is not match.");
        return;
    }
    HcfSymmKeyGeneratorImpl *impl = (HcfSymmKeyGeneratorImpl *)base;
    HcfObjDestroy(impl->spiObj);
    HcfFree(impl);
}

static HcfResult GenerateSymmKey(HcfSymKeyGenerator *self, HcfSymKey **symmKey)
{
    if ((self == NULL) || (symmKey == NULL)) {
        LOGE("Invalid input parameter.");
        return HCF_INVALID_PARAMS;
    }

    if (!IsClassMatch((HcfObjectBase *)self, GetSymKeyGeneratorClass())) {
        LOGE("Class is not match.");
        return HCF_INVALID_PARAMS;
    }
    HcfSymmKeyGeneratorImpl *impl = (HcfSymmKeyGeneratorImpl *)self;

    return impl->spiObj->engineGenerateSymmKey(impl->spiObj, symmKey);
}

static HcfResult ConvertSymmKey(HcfSymKeyGenerator *self, const HcfBlob *key, HcfSymKey **symmKey)
{
    if ((self == NULL) || (symmKey == NULL) || !IsBlobValid(key)) {
        LOGE("Invalid input parameter.");
        return HCF_INVALID_PARAMS;
    }
    if (!IsClassMatch((HcfObjectBase *)self, GetSymKeyGeneratorClass())) {
        LOGE("Class is not match.");
        return HCF_INVALID_PARAMS;
    }
    HcfSymmKeyGeneratorImpl *impl = (HcfSymmKeyGeneratorImpl *)self;

    return impl->spiObj->engineConvertSymmKey(impl->spiObj, key, symmKey);
}

HcfResult HcfSymKeyGeneratorCreate(const char *algoName, HcfSymKeyGenerator **returnObj)
{
    if (!IsStrValid(algoName, HCF_MAX_ALGO_NAME_LEN) || (returnObj == NULL)) {
        LOGE("Invalid input params while creating symkey!");
        return HCF_INVALID_PARAMS;
    }
    
    SymKeyAttr attr = {0};
    if (ParseAndSetParameter(algoName, (void *)&attr, OnSetSymKeyParameter) != HCF_SUCCESS) {
        LOGE("ParseAndSetParameter Failed!");
        return HCF_NOT_SUPPORT;
    }

    const SymKeyGenFuncSet *funcSet = FindAbility(&attr);
    if (funcSet == NULL) {
        LOGE("FindAbility Failed!");
        return HCF_NOT_SUPPORT;
    }
    HcfSymmKeyGeneratorImpl *returnGenerator = (HcfSymmKeyGeneratorImpl *)HcfMalloc(sizeof(HcfSymmKeyGeneratorImpl), 0);
    if (returnGenerator == NULL) {
        LOGE("Failed to allocate returnGenerator memory!");
        return HCF_ERR_MALLOC;
    }
    if (strcpy_s(returnGenerator->algoName, HCF_MAX_ALGO_NAME_LEN, algoName)) {
        LOGE("Failed to copy algoName!");
        HcfFree(returnGenerator);
        return HCF_ERR_COPY;
    }
    HcfSymKeyGeneratorSpi *spiObj = NULL;
    int32_t res = funcSet->createFunc(&attr, &spiObj);
    if (res != HCF_SUCCESS) {
        LOGE("Failed to create spi object!");
        HcfFree(returnGenerator);
        return res;
    }
    returnGenerator->base.generateSymKey = GenerateSymmKey;
    returnGenerator->base.convertSymKey = ConvertSymmKey;
    returnGenerator->base.base.destroy = DestroySymmKeyGenerator;
    returnGenerator->base.base.getClass = GetSymKeyGeneratorClass;
    returnGenerator->base.getAlgoName = GetAlgoName;
    returnGenerator->spiObj = spiObj;

    *returnObj = (HcfSymKeyGenerator *)returnGenerator;
    return HCF_SUCCESS;
}
