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

#include "rand.h"
#include <securec.h>
#include "rand_spi.h"
#include "rand_openssl.h"
#include "log.h"
#include "config.h"
#include "memory.h"
#include "utils.h"

typedef HcfResult (*HcfRandSpiCreateFunc)(HcfRandSpi **);

typedef struct {
    HcfRand base;

    HcfRandSpi *spiObj;

    const char *algoName;
} HcfRandImpl;

typedef struct {
    char *algoName;

    HcfRandSpiCreateFunc createSpifunc;
} HcfRandAbility;

static const HcfRandAbility RAND_ABILITY_SET[] = {
    { "OpensslRand", HcfRandSpiCreate }
};

static const char *GetRandClass(void)
{
    return "Rand";
}

static HcfRandSpiCreateFunc FindAbility(const char *algoName)
{
    for (uint32_t i = 0; i < (sizeof(RAND_ABILITY_SET) / sizeof(RAND_ABILITY_SET[0])); i++) {
        if (strcmp(RAND_ABILITY_SET[i].algoName, algoName) == 0) {
            return RAND_ABILITY_SET[i].createSpifunc;
        }
    }
    LOGE("Algo not support! [Algo]: %s", algoName);
    return NULL;
}

static HcfResult GenerateRandom(HcfRand *self, int32_t numBytes, HcfBlob *random)
{
    if ((self == NULL) || (numBytes <= 0) || (numBytes > HCF_MAX_BUFFER_LEN) || (random == NULL)) {
        LOGE("Invalid params!");
        return HCF_INVALID_PARAMS;
    }
    if (!IsClassMatch((HcfObjectBase *)self, GetRandClass())) {
        LOGE("Class is not match.");
        return HCF_INVALID_PARAMS;
    }
    return ((HcfRandImpl *)self)->spiObj->engineGenerateRandom(
        ((HcfRandImpl *)self)->spiObj, numBytes, random);
}

static HcfResult SetSeed(HcfRand *self, HcfBlob *seed)
{
    if ((self == NULL) || (!IsBlobValid(seed)) || (seed->len > HCF_MAX_BUFFER_LEN)) {
        LOGE("The input self ptr is NULL!");
        return HCF_INVALID_PARAMS;
    }
    if (!IsClassMatch((HcfObjectBase *)self, GetRandClass())) {
        LOGE("Class is not match.");
        return HCF_INVALID_PARAMS;
    }
    ((HcfRandImpl *)self)->spiObj->engineSetSeed(
        ((HcfRandImpl *)self)->spiObj, seed);
    return HCF_SUCCESS;
}

static void HcfRandDestroy(HcfObjectBase *self)
{
    if (self == NULL) {
        LOGE("The input self ptr is NULL!");
        return;
    }
    if (!IsClassMatch((HcfObjectBase *)self, GetRandClass())) {
        LOGE("Class is not match.");
        return;
    }
    HcfRandImpl *impl = (HcfRandImpl *)self;
    HcfObjDestroy(impl->spiObj);
    HcfFree(impl);
}

HcfResult HcfRandCreate(HcfRand **random)
{
    if (random == NULL) {
        LOGE("Invalid input params while creating rand!");
        return HCF_INVALID_PARAMS;
    }
    HcfRandSpiCreateFunc createSpifunc = FindAbility("OpensslRand");
    if (createSpifunc == NULL) {
        LOGE("Algo not supported!");
        return HCF_NOT_SUPPORT;
    }
    HcfRandImpl *returnRandApi = (HcfRandImpl *)HcfMalloc(sizeof(HcfRandImpl), 0);
    if (returnRandApi == NULL) {
        LOGE("Failed to allocate Rand Obj memory!");
        return HCF_ERR_MALLOC;
    }
    HcfRandSpi *spiObj = NULL;
    HcfResult res = createSpifunc(&spiObj);
    if (res != HCF_SUCCESS) {
        LOGE("Failed to create spi object!");
        HcfFree(returnRandApi);
        return res;
    }
    returnRandApi->base.base.getClass = GetRandClass;
    returnRandApi->base.base.destroy = HcfRandDestroy;
    returnRandApi->base.generateRandom = GenerateRandom;
    returnRandApi->base.setSeed = SetSeed;
    returnRandApi->spiObj = spiObj;
    *random = (HcfRand *)returnRandApi;
    return HCF_SUCCESS;
}