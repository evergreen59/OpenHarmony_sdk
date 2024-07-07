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

#include "cert_chain_validator.h"

#include <securec.h>

#include "blob.h"
#include "cert_chain_validator_spi.h"
#include "config.h"
#include "result.h"
#include "log.h"
#include "memory.h"
#include "result.h"
#include "utils.h"
#include "x509_cert_chain_validator_openssl.h"

#define LV_LENGTH_LEN sizeof(uint16_t)
#define MAX_CERT_PATH_DATA_LEM 8192

typedef HcfResult (*CertChainValidatorSpiCreateFunc)(HcfCertChainValidatorSpi **);

typedef struct {
    HcfCertChainValidator base;
    HcfCertChainValidatorSpi *spiObj;
    char *algorithm;
} CertChainValidatorImpl;

typedef struct {
    CertChainValidatorSpiCreateFunc createFunc;
} HcfCertChainValidatorFuncSet;

typedef struct {
    const char *algorithm;
    HcfCertChainValidatorFuncSet funcSet;
} HcfCertChainValidatorAbility;

static const HcfCertChainValidatorAbility CERT_PATH_VALIDATOR_ABILITY_SET[] = {
    { "PKIX", { HcfCertChainValidatorSpiCreate } }
};

static const HcfCertChainValidatorFuncSet *FindAbility(const char *algorithm)
{
    for (uint32_t i = 0; i < sizeof(CERT_PATH_VALIDATOR_ABILITY_SET) / sizeof(HcfCertChainValidatorAbility); i++) {
        if (strcmp(CERT_PATH_VALIDATOR_ABILITY_SET[i].algorithm, algorithm) == 0) {
            return &(CERT_PATH_VALIDATOR_ABILITY_SET[i].funcSet);
        }
    }
    LOGE("Algorithm for certChain validator is not support! [algorithm]: %s", algorithm);
    return NULL;
}

static const char *GetCertChainValidatorClass(void)
{
    return "HcfCertChainValidator";
}

static void DestroyCertChainValidator(HcfObjectBase *self)
{
    if (self == NULL) {
        LOGE("Invalid input parameter.");
        return;
    }
    if (!IsClassMatch(self, GetCertChainValidatorClass())) {
        LOGE("Class is not match.");
        return;
    }
    CertChainValidatorImpl *validatorImpl = (CertChainValidatorImpl *)self;
    HcfObjDestroy(validatorImpl->spiObj);
    HcfFree(validatorImpl->algorithm);
    validatorImpl->algorithm = NULL;
    HcfFree(validatorImpl);
}

static HcfResult ConvertCertBuffer2List(const HcfCertChainData *certChainData, HcfArray *certsList)
{
    uint8_t *msg = certChainData->data;
    const uint8_t *boundary = certChainData->data + certChainData->dataLen;
    uint32_t index = 0;
    HcfResult res = HCF_SUCCESS;
    while (msg < boundary) {
        if (index >= certsList->count || (msg + LV_LENGTH_LEN > boundary)) {
            LOGE("Invalid index for l-v len!");
            res = HCF_INVALID_PARAMS;
            break;
        }
        uint16_t entryLen = 0;
        if (memcpy_s(&entryLen, LV_LENGTH_LEN, msg, LV_LENGTH_LEN) != EOK) {
            LOGE("Input data in too long.");
            return HCF_ERR_COPY;
        }
        msg = msg + LV_LENGTH_LEN;
        certsList->data[index].data = (uint8_t *)HcfMalloc(entryLen, 0);
        if (certsList->data[index].data == NULL) {
            LOGE("Failed to malloc data for cert, index = %u.", index);
            res = HCF_ERR_MALLOC;
            break;
        }
        if (msg + entryLen > boundary) {
            LOGE("Entry len is overflow for boundary!");
            res = HCF_INVALID_PARAMS;
            break;
        }
        if (memcpy_s(certsList->data[index].data, entryLen, msg, entryLen) != EOK) {
            res = HCF_ERR_COPY;
            break;
        }
        certsList->data[index].len = entryLen;
        msg = msg + entryLen;
        index++;
    }
    return res;
}

static HcfResult Validate(HcfCertChainValidator *self, const HcfCertChainData *certChainData)
{
    if ((self == NULL) || (certChainData == NULL) || (certChainData->dataLen > MAX_CERT_PATH_DATA_LEM)) {
        LOGE("Invalid input parameter.");
        return HCF_INVALID_PARAMS;
    }
    if (!IsClassMatch((HcfObjectBase *)self, GetCertChainValidatorClass())) {
        LOGE("Class is not match.");
        return HCF_INVALID_PARAMS;
    }
    CertChainValidatorImpl *impl = (CertChainValidatorImpl *)self;
    HcfArray certsList = { NULL, 0 };
    certsList.format = certChainData->format;
    certsList.count = certChainData->count;
    uint32_t certsLen = sizeof(HcfBlob) * certsList.count;
    certsList.data = (HcfBlob *)HcfMalloc(certsLen, 0);
    if (certsList.data == NULL) {
        LOGE("Failed to new memory for certs.");
        return HCF_ERR_MALLOC;
    }
    HcfResult res = ConvertCertBuffer2List(certChainData, &certsList);
    if (res != HCF_SUCCESS) {
        LOGE("Failed to convert buffer to lists.");
        HcfArrayDataClearAndFree(&certsList);
        return res;
    }
    res = impl->spiObj->engineValidate(impl->spiObj, &certsList);
    HcfArrayDataClearAndFree(&certsList);
    return res;
}

static const char *GetAlgorithm(HcfCertChainValidator *self)
{
    if (self == NULL) {
        LOGE("Invalid input parameter.");
        return NULL;
    }
    if (!IsClassMatch((HcfObjectBase *)self, GetCertChainValidatorClass())) {
        LOGE("Class is not match.");
        return NULL;
    }
    CertChainValidatorImpl *impl = (CertChainValidatorImpl *)self;
    const char *algo = (const char *)impl->algorithm;
    return algo;
}

HcfResult HcfCertChainValidatorCreate(const char *algorithm, HcfCertChainValidator **pathValidator)
{
    if (!IsStrValid(algorithm, HCF_MAX_STR_LEN) || (pathValidator == NULL)) {
        return HCF_INVALID_PARAMS;
    }
    const HcfCertChainValidatorFuncSet *func = FindAbility(algorithm);
    if (func == NULL) {
        LOGE("Func is null!");
        return HCF_NOT_SUPPORT;
    }

    HcfCertChainValidatorSpi *spiObj = NULL;
    HcfResult res = func->createFunc(&spiObj);
    if (res != HCF_SUCCESS) {
        LOGE("Failed to create certChain validator spi object!");
        return res;
    }
    CertChainValidatorImpl *returnValidator = (CertChainValidatorImpl *)HcfMalloc(sizeof(CertChainValidatorImpl), 0);
    if (returnValidator == NULL) {
        LOGE("Failed to allocate returnValidator memory!");
        return HCF_ERR_MALLOC;
    }
    returnValidator->base.validate = Validate;
    returnValidator->base.getAlgorithm = GetAlgorithm;
    returnValidator->base.base.destroy = DestroyCertChainValidator;
    returnValidator->base.base.getClass = GetCertChainValidatorClass;
    returnValidator->spiObj = spiObj;
    uint32_t algoNameLen = strlen(algorithm) + 1;
    returnValidator->algorithm = (char *)HcfMalloc(algoNameLen, 0);
    if (returnValidator->algorithm == NULL) {
        LOGE("Failed to allocate algorithm memory!");
        HcfFree(returnValidator);
        return HCF_ERR_MALLOC;
    }
    (void)memcpy_s(returnValidator->algorithm, algoNameLen, algorithm, algoNameLen);

    *pathValidator = (HcfCertChainValidator *)returnValidator;
    return HCF_SUCCESS;
}