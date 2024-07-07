/*
 * Copyright (C) 2022-2023 Huawei Device Co., Ltd.
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

#include "asy_key_generator.h"

#include <securec.h>

#include "asy_key_generator_spi.h"
#include "config.h"
#include "ecc_asy_key_generator_openssl.h"
#include "params_parser.h"
#include "rsa_asy_key_generator_openssl.h"
#include "log.h"
#include "memory.h"
#include "utils.h"

typedef HcfResult (*HcfAsyKeyGeneratorSpiCreateFunc)(HcfAsyKeyGenParams *, HcfAsyKeyGeneratorSpi **);

typedef struct {
    HcfAsyKeyGenerator base;

    HcfAsyKeyGeneratorSpi *spiObj;

    char algoName[HCF_MAX_ALGO_NAME_LEN];
} HcfAsyKeyGeneratorImpl;

typedef struct {
    HCF_ALG_VALUE algo;

    HcfAsyKeyGeneratorSpiCreateFunc createSpifunc;
} HcfAsyKeyGenAbility;

static const HcfAsyKeyGenAbility ASY_KEY_GEN_ABILITY_SET[] = {
    { HCF_ALG_RSA, HcfAsyKeyGeneratorSpiRsaCreate },
    { HCF_ALG_ECC, HcfAsyKeyGeneratorSpiEccCreate }
};

static HcfAsyKeyGeneratorSpiCreateFunc FindAbility(HcfAsyKeyGenParams *params)
{
    for (uint32_t i = 0; i < sizeof(ASY_KEY_GEN_ABILITY_SET) / sizeof(ASY_KEY_GEN_ABILITY_SET[0]); i++) {
        if (ASY_KEY_GEN_ABILITY_SET[i].algo == params->algo) {
            return ASY_KEY_GEN_ABILITY_SET[i].createSpifunc;
        }
    }
    LOGE("Algo not support! [Algo]: %d", params->algo);
    return NULL;
}

static void SetPrimes(HCF_ALG_PARA_VALUE value, HcfAsyKeyGenParams *params)
{
    if (params == NULL) {
        LOGE("params is null.");
        return;
    }
    switch (value) {
        case HCF_OPENSSL_PRIMES_2:
            params->primes = (int32_t)HCF_RSA_PRIMES_SIZE_2;
            break;
        case HCF_OPENSSL_PRIMES_3:
            params->primes = (int32_t)HCF_RSA_PRIMES_SIZE_3;
            break;
        case HCF_OPENSSL_PRIMES_4:
            params->primes = (int32_t)HCF_RSA_PRIMES_SIZE_4;
            break;
        case HCF_OPENSSL_PRIMES_5:
            params->primes = (int32_t)HCF_RSA_PRIMES_SIZE_5;
            break;
        default:
            params->primes = (int32_t)HCF_RSA_PRIMES_SIZE_2; // default primes is 2
            LOGD("user default primes 2");
            break;
    }
    LOGD("Set primes:%d\n", params->primes);
}

static void SetKeyType(HCF_ALG_PARA_VALUE value, HcfAsyKeyGenParams *params)
{
    switch (value) {
        case HCF_ALG_ECC_224:
        case HCF_ALG_ECC_256:
        case HCF_ALG_ECC_384:
        case HCF_ALG_ECC_521:
            params->bits = value;
            params->algo = HCF_ALG_ECC;
            break;
        case HCF_OPENSSL_RSA_512:
            params->bits = (int32_t)HCF_RSA_KEY_SIZE_512;
            params->algo = HCF_ALG_RSA;
            break;
        case HCF_OPENSSL_RSA_768:
            params->bits = (int32_t)HCF_RSA_KEY_SIZE_768;
            params->algo = HCF_ALG_RSA;
            break;
        case HCF_OPENSSL_RSA_1024:
            params->bits = (int32_t)HCF_RSA_KEY_SIZE_1024;
            params->algo = HCF_ALG_RSA;
            break;
        case HCF_OPENSSL_RSA_2048:
            params->bits = (int32_t)HCF_RSA_KEY_SIZE_2048;
            params->algo = HCF_ALG_RSA;
            break;
        case HCF_OPENSSL_RSA_3072:
            params->bits = (int32_t)HCF_RSA_KEY_SIZE_3072;
            params->algo = HCF_ALG_RSA;
            break;
        case HCF_OPENSSL_RSA_4096:
            params->bits = (int32_t)HCF_RSA_KEY_SIZE_4096;
            params->algo = HCF_ALG_RSA;
            break;
        case HCF_OPENSSL_RSA_8192:
            params->bits = (int32_t)HCF_RSA_KEY_SIZE_8192;
            params->algo = HCF_ALG_RSA;
            break;
        default:
            LOGE("there is not matched algorithm.");
            break;
    }
}

static HcfResult ParseAsyKeyGenParams(const HcfParaConfig* config, void *params)
{
    if (config == NULL || params == NULL) {
        return HCF_INVALID_PARAMS;
    }
    HcfResult ret = HCF_SUCCESS;
    HcfAsyKeyGenParams *paramsObj = (HcfAsyKeyGenParams *)params;
    LOGI("Set Parameter: %s", config->tag);
    switch (config->paraType) {
        case HCF_ALG_KEY_TYPE:
            SetKeyType(config->paraValue, paramsObj);
            break;
        case HCF_ALG_PRIMES:
            SetPrimes(config->paraValue, paramsObj);
            break;
        default:
            ret = HCF_INVALID_PARAMS;
            break;
    }
    return ret;
}

// export interfaces
static const char *GetAsyKeyGeneratorClass(void)
{
    return "HcfAsyKeyGenerator";
}

static const char *GetAlgoName(HcfAsyKeyGenerator *self)
{
    if (self == NULL) {
        LOGE("The input self ptr is NULL!");
        return NULL;
    }
    if (!IsClassMatch((HcfObjectBase *)self, GetAsyKeyGeneratorClass())) {
        return NULL;
    }
    return ((HcfAsyKeyGeneratorImpl *)self)->algoName;
}

static HcfResult ConvertKey(HcfAsyKeyGenerator *self, HcfParamsSpec *params, HcfBlob *pubKeyBlob,
    HcfBlob *priKeyBlob, HcfKeyPair **returnKeyPair)
{
    if (self == NULL) {
        LOGE("Invalid input parameter.");
        return HCF_INVALID_PARAMS;
    }
    if (!IsClassMatch((HcfObjectBase *)self, GetAsyKeyGeneratorClass())) {
        return HCF_INVALID_PARAMS;
    }
    return ((HcfAsyKeyGeneratorImpl *)self)->spiObj->engineConvertKey(
        ((HcfAsyKeyGeneratorImpl *)self)->spiObj, params, pubKeyBlob, priKeyBlob, returnKeyPair);
}

static HcfResult GenerateKeyPair(HcfAsyKeyGenerator *self, HcfParamsSpec *params,
    HcfKeyPair **returnKeyPair)
{
    if (self == NULL) {
        LOGE("Invalid input parameter.");
        return HCF_INVALID_PARAMS;
    }
    if (!IsClassMatch((HcfObjectBase *)self, GetAsyKeyGeneratorClass())) {
        return HCF_INVALID_PARAMS;
    }
    return ((HcfAsyKeyGeneratorImpl *)self)->spiObj->engineGenerateKeyPair(
        ((HcfAsyKeyGeneratorImpl *)self)->spiObj, returnKeyPair);
}

static void DestroyAsyKeyGenerator(HcfObjectBase *self)
{
    if (self == NULL) {
        return;
    }
    if (!IsClassMatch((HcfObjectBase *)self, GetAsyKeyGeneratorClass())) {
        return;
    }
    HcfAsyKeyGeneratorImpl *impl = (HcfAsyKeyGeneratorImpl *)self;
    HcfObjDestroy(impl->spiObj);
    impl->spiObj = NULL;
    HcfFree(impl);
}

HcfResult HcfAsyKeyGeneratorCreate(const char *algoName, HcfAsyKeyGenerator **returnObj)
{
    if ((!IsStrValid(algoName, HCF_MAX_ALGO_NAME_LEN)) || (returnObj == NULL)) {
        return HCF_INVALID_PARAMS;
    }

    HcfAsyKeyGenParams params = { 0 };
    if (ParseAndSetParameter(algoName, &params, ParseAsyKeyGenParams) != HCF_SUCCESS) {
        LOGE("Failed to parser parmas!");
        return HCF_INVALID_PARAMS;
    }

    HcfAsyKeyGeneratorSpiCreateFunc createSpifunc = FindAbility(&params);
    if (createSpifunc == NULL) {
        return HCF_NOT_SUPPORT;
    }

    HcfAsyKeyGeneratorImpl *returnGenerator = (HcfAsyKeyGeneratorImpl *)HcfMalloc(sizeof(HcfAsyKeyGeneratorImpl), 0);
    if (returnGenerator == NULL) {
        LOGE("Failed to allocate returnGenerator memory!");
        return HCF_ERR_MALLOC;
    }
    if (strcpy_s(returnGenerator->algoName, HCF_MAX_ALGO_NAME_LEN, algoName) != EOK) {
        LOGE("Failed to copy algoName!");
        HcfFree(returnGenerator);
        return HCF_ERR_COPY;
    }
    HcfAsyKeyGeneratorSpi *spiObj = NULL;
    int32_t res = HCF_SUCCESS;
    res = createSpifunc(&params, &spiObj);
    if (res != HCF_SUCCESS) {
        LOGE("Failed to create spi object!");
        HcfFree(returnGenerator);
        return res;
    }
    returnGenerator->base.base.destroy = DestroyAsyKeyGenerator;
    returnGenerator->base.base.getClass = GetAsyKeyGeneratorClass;
    returnGenerator->base.convertKey = ConvertKey;
    returnGenerator->base.generateKeyPair = GenerateKeyPair;
    returnGenerator->base.getAlgoName = GetAlgoName;
    returnGenerator->spiObj = spiObj;
    *returnObj = (HcfAsyKeyGenerator *)returnGenerator;
    return HCF_SUCCESS;
}
