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

#include "key_agreement.h"

#include <securec.h>

#include "key_agreement_spi.h"
#include "config.h"
#include "ecdh_openssl.h"
#include "log.h"
#include "memory.h"
#include "params_parser.h"
#include "utils.h"

typedef HcfResult (*HcfKeyAgreementSpiCreateFunc)(HcfKeyAgreementParams *, HcfKeyAgreementSpi **);

typedef struct {
    HcfKeyAgreement base;

    HcfKeyAgreementSpi *spiObj;

    char algoName[HCF_MAX_ALGO_NAME_LEN];
} HcfKeyAgreementImpl;

typedef struct {
    HCF_ALG_VALUE algo;

    HcfKeyAgreementSpiCreateFunc createSpifunc;
} HcfKeyAgreementGenAbility;

static const HcfKeyAgreementGenAbility KEY_AGREEMENT_GEN_ABILITY_SET[] = {
    { HCF_ALG_ECC, HcfKeyAgreementSpiEcdhCreate }
};

static HcfKeyAgreementSpiCreateFunc FindAbility(HcfKeyAgreementParams *params)
{
    for (uint32_t i = 0; i < sizeof(KEY_AGREEMENT_GEN_ABILITY_SET) / sizeof(KEY_AGREEMENT_GEN_ABILITY_SET[0]); i++) {
        if (KEY_AGREEMENT_GEN_ABILITY_SET[i].algo == params->algo) {
            return KEY_AGREEMENT_GEN_ABILITY_SET[i].createSpifunc;
        }
    }
    LOGE("Algo not support! [Algo]: %d", params->algo);
    return NULL;
}

static void SetKeyType(HCF_ALG_PARA_VALUE value, HcfKeyAgreementParams *paramsObj)
{
    switch (value) {
        case HCF_ALG_ECC_224:
        case HCF_ALG_ECC_256:
        case HCF_ALG_ECC_384:
        case HCF_ALG_ECC_521:
            paramsObj->keyLen = value;
            paramsObj->algo = HCF_ALG_ECC;
            break;
        default:
            break;
    }
}

static HcfResult ParseKeyAgreementParams(const HcfParaConfig* config, void *params)
{
    if (config == NULL || params == NULL) {
        return HCF_INVALID_PARAMS;
    }
    HcfResult ret = HCF_SUCCESS;
    HcfKeyAgreementParams *paramsObj = (HcfKeyAgreementParams *)params;
    LOGI("Set Parameter: %s", config->tag);
    switch (config->paraType) {
        case HCF_ALG_KEY_TYPE:
            SetKeyType(config->paraValue, paramsObj);
            break;
        default:
            ret = HCF_INVALID_PARAMS;
            break;
    }
    return ret;
}

// export interfaces
static const char *GetKeyAgreementClass(void)
{
    return "HcfKeyAgreement";
}

static const char *GetAlgoName(HcfKeyAgreement *self)
{
    if (self == NULL) {
        LOGE("The input self ptr is NULL!");
        return NULL;
    }
    if (!IsClassMatch((HcfObjectBase *)self, GetKeyAgreementClass())) {
        return NULL;
    }
    return ((HcfKeyAgreementImpl *)self)->algoName;
}

static HcfResult GenerateSecret(HcfKeyAgreement *self, HcfPriKey *priKey,
    HcfPubKey *pubKey, HcfBlob *returnSecret)
{
    if (self == NULL) {
        LOGE("Invalid input parameter.");
        return HCF_INVALID_PARAMS;
    }
    if (!IsClassMatch((HcfObjectBase *)self, GetKeyAgreementClass())) {
        return HCF_INVALID_PARAMS;
    }

    return ((HcfKeyAgreementImpl *)self)->spiObj->engineGenerateSecret(
        ((HcfKeyAgreementImpl *)self)->spiObj, priKey, pubKey, returnSecret);
}

static void DestroyKeyAgreement(HcfObjectBase *self)
{
    if (self == NULL) {
        return;
    }
    if (!IsClassMatch((HcfObjectBase *)self, GetKeyAgreementClass())) {
        return;
    }
    HcfKeyAgreementImpl *impl = (HcfKeyAgreementImpl *)self;
    HcfObjDestroy(impl->spiObj);
    impl->spiObj = NULL;
    HcfFree(impl);
}

HcfResult HcfKeyAgreementCreate(const char *algoName, HcfKeyAgreement **returnObj)
{
    if ((!IsStrValid(algoName, HCF_MAX_ALGO_NAME_LEN)) || (returnObj == NULL)) {
        return HCF_INVALID_PARAMS;
    }

    HcfKeyAgreementParams params = { 0 };
    if (ParseAndSetParameter(algoName, &params, ParseKeyAgreementParams) != HCF_SUCCESS) {
        LOGE("Failed to parser parmas!");
        return HCF_INVALID_PARAMS;
    }

    HcfKeyAgreementSpiCreateFunc createSpifunc = FindAbility(&params);
    if (createSpifunc == NULL) {
        return HCF_NOT_SUPPORT;
    }

    HcfKeyAgreementImpl *returnGenerator = (HcfKeyAgreementImpl *)HcfMalloc(sizeof(HcfKeyAgreementImpl), 0);
    if (returnGenerator == NULL) {
        LOGE("Failed to allocate returnGenerator memory!");
        return HCF_ERR_MALLOC;
    }
    if (strcpy_s(returnGenerator->algoName, HCF_MAX_ALGO_NAME_LEN, algoName) != EOK) {
        LOGE("Failed to copy algoName!");
        HcfFree(returnGenerator);
        return HCF_ERR_COPY;
    }
    HcfKeyAgreementSpi *spiObj = NULL;
    int32_t res = createSpifunc(&params, &spiObj);
    if (res != HCF_SUCCESS) {
        LOGE("Failed to create spi object!");
        HcfFree(returnGenerator);
        return res;
    }
    returnGenerator->base.base.destroy = DestroyKeyAgreement;
    returnGenerator->base.base.getClass = GetKeyAgreementClass;
    returnGenerator->base.generateSecret = GenerateSecret;
    returnGenerator->base.getAlgoName = GetAlgoName;
    returnGenerator->spiObj = spiObj;

    *returnObj = (HcfKeyAgreement *)returnGenerator;
    return HCF_SUCCESS;
}
