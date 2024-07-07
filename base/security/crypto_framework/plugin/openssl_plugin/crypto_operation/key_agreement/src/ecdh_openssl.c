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

#include "ecdh_openssl.h"

#include <openssl/bio.h>
#include <openssl/err.h>

#include "algorithm_parameter.h"
#include "openssl_class.h"
#include "openssl_common.h"
#include "log.h"
#include "memory.h"
#include "utils.h"

typedef struct {
    HcfKeyAgreementSpi base;

    int32_t curveId;
} HcfKeyAgreementSpiEcdhOpensslImpl;

static EVP_PKEY *NewPKeyByEccPubKey(int32_t curveId, HcfOpensslEccPubKey *publicKey)
{
    EC_KEY *ecKey = EC_KEY_new_by_curve_name(curveId);
    if (ecKey == NULL) {
        HcfPrintOpensslError();
        return NULL;
    }
    if (EC_KEY_set_public_key(ecKey, (publicKey->pk)) != HCF_OPENSSL_SUCCESS) {
        HcfPrintOpensslError();
        EC_KEY_free(ecKey);
        return NULL;
    }
    EVP_PKEY *pKey = EVP_PKEY_new();
    if (pKey == NULL) {
        HcfPrintOpensslError();
        EC_KEY_free(ecKey);
        return NULL;
    }
    if (EVP_PKEY_assign_EC_KEY(pKey, ecKey) != HCF_OPENSSL_SUCCESS) {
        HcfPrintOpensslError();
        EVP_PKEY_free(pKey);
        EC_KEY_free(ecKey);
        return NULL;
    }
    return pKey;
}

static EVP_PKEY *NewPKeyByEccPriKey(int32_t curveId, HcfOpensslEccPriKey *privateKey)
{
    EC_KEY *ecKey = EC_KEY_new_by_curve_name(curveId);
    if (ecKey == NULL) {
        HcfPrintOpensslError();
        return NULL;
    }
    if (EC_KEY_set_private_key(ecKey, (privateKey->sk)) != HCF_OPENSSL_SUCCESS) {
        HcfPrintOpensslError();
        EC_KEY_free(ecKey);
        return NULL;
    }
    EVP_PKEY *pKey = EVP_PKEY_new();
    if (pKey == NULL) {
        HcfPrintOpensslError();
        EC_KEY_free(ecKey);
        return NULL;
    }
    if (EVP_PKEY_assign_EC_KEY(pKey, ecKey) != HCF_OPENSSL_SUCCESS) {
        HcfPrintOpensslError();
        EVP_PKEY_free(pKey);
        EC_KEY_free(ecKey);
        return NULL;
    }
    return pKey;
}

static HcfResult EcdhDerive(EVP_PKEY *priPKey, EVP_PKEY *pubPKey, HcfBlob *returnSecret)
{
    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new(priPKey, NULL);
    if (ctx == NULL) {
        HcfPrintOpensslError();
        return HCF_ERR_CRYPTO_OPERATION;
    }
    if (EVP_PKEY_derive_init(ctx) != HCF_OPENSSL_SUCCESS) {
        HcfPrintOpensslError();
        EVP_PKEY_CTX_free(ctx);
        return HCF_ERR_CRYPTO_OPERATION;
    }
    if (EVP_PKEY_derive_set_peer(ctx, pubPKey) != HCF_OPENSSL_SUCCESS) {
        HcfPrintOpensslError();
        EVP_PKEY_CTX_free(ctx);
        return HCF_ERR_CRYPTO_OPERATION;
    }
    size_t maxLen;
    if (EVP_PKEY_derive(ctx, NULL, &maxLen) != HCF_OPENSSL_SUCCESS) {
        HcfPrintOpensslError();
        EVP_PKEY_CTX_free(ctx);
        return HCF_ERR_CRYPTO_OPERATION;
    }
    uint8_t *secretData = (uint8_t *)HcfMalloc(maxLen, 0);
    if (secretData == NULL) {
        LOGE("Failed to allocate secretData memory!");
        EVP_PKEY_CTX_free(ctx);
        return HCF_ERR_MALLOC;
    }
    size_t actualLen = maxLen;
    if (EVP_PKEY_derive(ctx, secretData, &actualLen) != HCF_OPENSSL_SUCCESS) {
        HcfPrintOpensslError();
        EVP_PKEY_CTX_free(ctx);
        HcfFree(secretData);
        return HCF_ERR_CRYPTO_OPERATION;
    }
    EVP_PKEY_CTX_free(ctx);
    if (actualLen > maxLen) {
        LOGE("signature data too long.");
        HcfFree(secretData);
        return HCF_ERR_CRYPTO_OPERATION;
    }

    returnSecret->data = secretData;
    returnSecret->len = (uint32_t)actualLen;
    return HCF_SUCCESS;
}

// export interfaces
static const char *GetEcdhClass(void)
{
    return "HcfKeyAgreement.HcfKeyAgreementSpiEcdhOpensslImpl";
}

static void DestroyEcdh(HcfObjectBase *self)
{
    if (self == NULL) {
        return;
    }
    if (!IsClassMatch(self, GetEcdhClass())) {
        return;
    }
    HcfFree(self);
}

static HcfResult EngineGenerateSecret(HcfKeyAgreementSpi *self, HcfPriKey *priKey,
    HcfPubKey *pubKey, HcfBlob *returnSecret)
{
    LOGI("start ...");
    if ((self == NULL) || (priKey == NULL) || (pubKey == NULL) || (returnSecret == NULL)) {
        LOGE("Invalid input parameter.");
        return HCF_INVALID_PARAMS;
    }
    if ((!IsClassMatch((HcfObjectBase *)self, GetEcdhClass())) ||
        (!IsClassMatch((HcfObjectBase *)priKey, HCF_OPENSSL_ECC_PRI_KEY_CLASS)) ||
        (!IsClassMatch((HcfObjectBase *)pubKey, HCF_OPENSSL_ECC_PUB_KEY_CLASS))) {
        return HCF_INVALID_PARAMS;
    }

    HcfKeyAgreementSpiEcdhOpensslImpl *impl = (HcfKeyAgreementSpiEcdhOpensslImpl *)self;
    EVP_PKEY *priPKey = NewPKeyByEccPriKey(impl->curveId, (HcfOpensslEccPriKey *)priKey);
    if (priPKey == NULL) {
        return HCF_ERR_CRYPTO_OPERATION;
    }
    EVP_PKEY *pubPKey = NewPKeyByEccPubKey(impl->curveId, (HcfOpensslEccPubKey *)pubKey);
    if (pubPKey == NULL) {
        EVP_PKEY_free(priPKey);
        return HCF_ERR_CRYPTO_OPERATION;
    }

    int32_t res = EcdhDerive(priPKey, pubPKey, returnSecret);
    EVP_PKEY_free(priPKey);
    EVP_PKEY_free(pubPKey);
    LOGI("end ...");
    return res;
}

HcfResult HcfKeyAgreementSpiEcdhCreate(HcfKeyAgreementParams *params, HcfKeyAgreementSpi **returnObj)
{
    if ((params == NULL) || (returnObj == NULL)) {
        LOGE("Invalid input parameter.");
        return HCF_INVALID_PARAMS;
    }
    int32_t curveId;
    if (GetOpensslCurveId(params->keyLen, &curveId) != HCF_SUCCESS) {
        return HCF_INVALID_PARAMS;
    }

    HcfKeyAgreementSpiEcdhOpensslImpl *returnImpl = (HcfKeyAgreementSpiEcdhOpensslImpl *)HcfMalloc(
        sizeof(HcfKeyAgreementSpiEcdhOpensslImpl), 0);
    if (returnImpl == NULL) {
        LOGE("Failed to allocate returnImpl memroy!");
        return HCF_ERR_MALLOC;
    }
    returnImpl->base.base.getClass = GetEcdhClass;
    returnImpl->base.base.destroy = DestroyEcdh;
    returnImpl->base.engineGenerateSecret = EngineGenerateSecret;
    returnImpl->curveId = curveId;

    *returnObj = (HcfKeyAgreementSpi *)returnImpl;
    return HCF_SUCCESS;
}
