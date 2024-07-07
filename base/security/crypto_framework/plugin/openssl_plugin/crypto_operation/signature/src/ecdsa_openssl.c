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

#include "ecdsa_openssl.h"

#include <openssl/bio.h>
#include <openssl/err.h>

#include "algorithm_parameter.h"
#include "openssl_class.h"
#include "openssl_common.h"
#include "log.h"
#include "memory.h"
#include "utils.h"

#define OPENSSL_ECC_SIGN_CLASS "OPENSSL.ECC.SIGN"
#define OPENSSL_ECC_VERIFY_CLASS "OPENSSL.ECC.VERIFY"

typedef enum {
    UNINITIALIZED = 0,
    INITIALIZED = 1,
    READY = 2,
} EcdsaStatus;

typedef struct {
    HcfSignSpi base;

    const EVP_MD *digestAlg;

    EVP_MD_CTX *ctx;

    int32_t curveId;

    EcdsaStatus status;
} HcfSignSpiEcdsaOpensslImpl;

typedef struct {
    HcfVerifySpi base;

    const EVP_MD *digestAlg;

    EVP_MD_CTX *ctx;

    int32_t curveId;

    EcdsaStatus status;
} HcfVerifySpiEcdsaOpensslImpl;

static bool IsDigestAlgValid(uint32_t alg)
{
    if ((alg == HCF_OPENSSL_DIGEST_SHA1) || (alg == HCF_OPENSSL_DIGEST_SHA224) || (alg == HCF_OPENSSL_DIGEST_SHA256) ||
        (alg == HCF_OPENSSL_DIGEST_SHA384) || (alg == HCF_OPENSSL_DIGEST_SHA512)) {
        return true;
    } else {
        LOGE("Invalid digest num is %u.", alg);
        return false;
    }
}

// export interfaces
static const char *GetEcdsaSignClass(void)
{
    return OPENSSL_ECC_SIGN_CLASS;
}

static const char *GetEcdsaVerifyClass(void)
{
    return OPENSSL_ECC_VERIFY_CLASS;
}

static void DestroyEcdsaSign(HcfObjectBase *self)
{
    if (self == NULL) {
        return;
    }
    if (!IsClassMatch(self, GetEcdsaSignClass())) {
        return;
    }
    HcfSignSpiEcdsaOpensslImpl *impl = (HcfSignSpiEcdsaOpensslImpl *)self;
    EVP_MD_CTX_destroy(impl->ctx);
    impl->ctx = NULL;
    HcfFree(impl);
}

static void DestroyEcdsaVerify(HcfObjectBase *self)
{
    if (self == NULL) {
        return;
    }
    if (!IsClassMatch(self, GetEcdsaVerifyClass())) {
        return;
    }
    HcfSignSpiEcdsaOpensslImpl *impl = (HcfSignSpiEcdsaOpensslImpl *)self;
    EVP_MD_CTX_destroy(impl->ctx);
    impl->ctx = NULL;
    HcfFree(impl);
}

static HcfResult EngineSignInit(HcfSignSpi *self, HcfParamsSpec *params, HcfPriKey *privateKey)
{
    LOGI("start ...");
    (void)params;
    if ((self == NULL) || (privateKey == NULL)) {
        LOGE("Invalid input parameter.");
        return HCF_INVALID_PARAMS;
    }
    if ((!IsClassMatch((HcfObjectBase *)self, GetEcdsaSignClass())) ||
        (!IsClassMatch((HcfObjectBase *)privateKey, HCF_OPENSSL_ECC_PRI_KEY_CLASS))) {
        return HCF_INVALID_PARAMS;
    }

    HcfSignSpiEcdsaOpensslImpl *impl = (HcfSignSpiEcdsaOpensslImpl *)self;
    if (impl->status != UNINITIALIZED) {
        LOGE("Repeated initialization is not allowed.");
        return HCF_INVALID_PARAMS;
    }
    EC_KEY *ecKey = EC_KEY_new_by_curve_name(impl->curveId);
    if (ecKey == NULL) {
        HcfPrintOpensslError();
        return HCF_ERR_CRYPTO_OPERATION;
    }
    if (EC_KEY_set_private_key(ecKey, ((HcfOpensslEccPriKey *)privateKey)->sk) != HCF_OPENSSL_SUCCESS) {
        HcfPrintOpensslError();
        EC_KEY_free(ecKey);
        return HCF_ERR_CRYPTO_OPERATION;
    }
    EVP_PKEY *pKey = EVP_PKEY_new();
    if (pKey == NULL) {
        HcfPrintOpensslError();
        EC_KEY_free(ecKey);
        return HCF_ERR_CRYPTO_OPERATION;
    }
    if (EVP_PKEY_assign_EC_KEY(pKey, ecKey) != HCF_OPENSSL_SUCCESS) {
        HcfPrintOpensslError();
        EVP_PKEY_free(pKey);
        EC_KEY_free(ecKey);
        return HCF_ERR_CRYPTO_OPERATION;
    }
    if (EVP_DigestSignInit(impl->ctx, NULL, impl->digestAlg, NULL, pKey) != HCF_OPENSSL_SUCCESS) {
        HcfPrintOpensslError();
        EVP_PKEY_free(pKey);
        return HCF_ERR_CRYPTO_OPERATION;
    }
    EVP_PKEY_free(pKey);
    impl->status = INITIALIZED;
    LOGI("end ...");
    return HCF_SUCCESS;
}

static HcfResult EngineSignUpdate(HcfSignSpi *self, HcfBlob *data)
{
    LOGI("start ...");
    if ((self == NULL) || (!IsBlobValid(data))) {
        LOGE("Invalid input parameter.");
        return HCF_INVALID_PARAMS;
    }
    if (!IsClassMatch((HcfObjectBase *)self, GetEcdsaSignClass())) {
        return HCF_INVALID_PARAMS;
    }
    HcfSignSpiEcdsaOpensslImpl *impl = (HcfSignSpiEcdsaOpensslImpl *)self;
    if (impl->status == UNINITIALIZED) {
        LOGE("Sign object has not been initialized.");
        return HCF_INVALID_PARAMS;
    }
    if (EVP_DigestSignUpdate(impl->ctx, data->data, data->len) != HCF_OPENSSL_SUCCESS) {
        HcfPrintOpensslError();
        return HCF_ERR_CRYPTO_OPERATION;
    }
    impl->status = READY;
    LOGI("end ...");
    return HCF_SUCCESS;
}

static HcfResult EngineSignDoFinal(HcfSignSpi *self, HcfBlob *data, HcfBlob *returnSignatureData)
{
    LOGI("start ...");
    if ((self == NULL) || (returnSignatureData == NULL)) {
        LOGE("Invalid input parameter.");
        return HCF_INVALID_PARAMS;
    }
    if (!IsClassMatch((HcfObjectBase *)self, GetEcdsaSignClass())) {
        return HCF_INVALID_PARAMS;
    }

    HcfSignSpiEcdsaOpensslImpl *impl = (HcfSignSpiEcdsaOpensslImpl *)self;
    if (IsBlobValid(data)) {
        if (EVP_DigestSignUpdate(impl->ctx, data->data, data->len) != HCF_OPENSSL_SUCCESS) {
            HcfPrintOpensslError();
            return HCF_ERR_CRYPTO_OPERATION;
        }
        impl->status = READY;
    }
    if (impl->status != READY) {
        LOGE("The message has not been transferred.");
        return HCF_INVALID_PARAMS;
    }
    size_t maxLen;
    if (EVP_DigestSignFinal(impl->ctx, NULL, &maxLen) != HCF_OPENSSL_SUCCESS) {
        HcfPrintOpensslError();
        return HCF_ERR_CRYPTO_OPERATION;
    }
    uint8_t *outData = (uint8_t *)HcfMalloc(maxLen, 0);
    if (outData == NULL) {
        LOGE("Failed to allocate outData memory!");
        return HCF_ERR_MALLOC;
    }
    size_t actualLen = maxLen;
    if (EVP_DigestSignFinal(impl->ctx, outData, &actualLen) != HCF_OPENSSL_SUCCESS) {
        HcfPrintOpensslError();
        HcfFree(outData);
        return HCF_ERR_CRYPTO_OPERATION;
    }
    if (actualLen > maxLen) {
        LOGE("signature data too long.");
        HcfFree(outData);
        return HCF_ERR_CRYPTO_OPERATION;
    }

    returnSignatureData->data = outData;
    returnSignatureData->len = (uint32_t)actualLen;
    LOGI("end ...");
    return HCF_SUCCESS;
}

static HcfResult EngineVerifyInit(HcfVerifySpi *self, HcfParamsSpec *params, HcfPubKey *publicKey)
{
    LOGI("start ...");
    (void)params;
    if ((self == NULL) || (publicKey == NULL)) {
        LOGE("Invalid input parameter.");
        return HCF_INVALID_PARAMS;
    }
    if ((!IsClassMatch((HcfObjectBase *)self, GetEcdsaVerifyClass())) ||
        (!IsClassMatch((HcfObjectBase *)publicKey, HCF_OPENSSL_ECC_PUB_KEY_CLASS))) {
        return HCF_INVALID_PARAMS;
    }

    HcfVerifySpiEcdsaOpensslImpl *impl = (HcfVerifySpiEcdsaOpensslImpl *)self;
    if (impl->status != UNINITIALIZED) {
        LOGE("Repeated initialization is not allowed.");
        return HCF_INVALID_PARAMS;
    }
    EC_KEY *ecKey = EC_KEY_new_by_curve_name(impl->curveId);
    if (ecKey == NULL) {
        HcfPrintOpensslError();
        return HCF_ERR_CRYPTO_OPERATION;
    }
    if (EC_KEY_set_public_key(ecKey, ((HcfOpensslEccPubKey *)publicKey)->pk) != HCF_OPENSSL_SUCCESS) {
        HcfPrintOpensslError();
        EC_KEY_free(ecKey);
        return HCF_ERR_CRYPTO_OPERATION;
    }
    EVP_PKEY *pKey = EVP_PKEY_new();
    if (pKey == NULL) {
        HcfPrintOpensslError();
        EC_KEY_free(ecKey);
        return HCF_ERR_CRYPTO_OPERATION;
    }
    if (EVP_PKEY_assign_EC_KEY(pKey, ecKey) != HCF_OPENSSL_SUCCESS) {
        HcfPrintOpensslError();
        EVP_PKEY_free(pKey);
        EC_KEY_free(ecKey);
        return HCF_ERR_CRYPTO_OPERATION;
    }
    if (EVP_DigestVerifyInit(impl->ctx, NULL, impl->digestAlg, NULL, pKey) != HCF_OPENSSL_SUCCESS) {
        HcfPrintOpensslError();
        EVP_PKEY_free(pKey);
        return HCF_ERR_CRYPTO_OPERATION;
    }
    EVP_PKEY_free(pKey);
    impl->status = INITIALIZED;
    LOGI("end ...");
    return HCF_SUCCESS;
}

static HcfResult EngineVerifyUpdate(HcfVerifySpi *self, HcfBlob *data)
{
    LOGI("start ...");
    if ((self == NULL) || (!IsBlobValid(data))) {
        LOGE("Invalid input parameter.");
        return HCF_INVALID_PARAMS;
    }
    if (!IsClassMatch((HcfObjectBase *)self, GetEcdsaVerifyClass())) {
        return HCF_INVALID_PARAMS;
    }

    HcfVerifySpiEcdsaOpensslImpl *impl = (HcfVerifySpiEcdsaOpensslImpl *)self;
    if (impl->status == UNINITIALIZED) {
        LOGE("Verify object has not been initialized.");
        return HCF_INVALID_PARAMS;
    }
    if (EVP_DigestVerifyUpdate(impl->ctx, data->data, data->len) != HCF_OPENSSL_SUCCESS) {
        HcfPrintOpensslError();
        return HCF_ERR_CRYPTO_OPERATION;
    }
    impl->status = READY;
    LOGI("end ...");
    return HCF_SUCCESS;
}

static bool EngineVerifyDoFinal(HcfVerifySpi *self, HcfBlob *data, HcfBlob *signatureData)
{
    LOGI("start ...");
    if ((self == NULL) || (!IsBlobValid(signatureData))) {
        LOGE("Invalid input parameter.");
        return false;
    }
    if (!IsClassMatch((HcfObjectBase *)self, GetEcdsaVerifyClass())) {
        return false;
    }

    HcfVerifySpiEcdsaOpensslImpl *impl = (HcfVerifySpiEcdsaOpensslImpl *)self;
    if (IsBlobValid(data)) {
        if (EVP_DigestVerifyUpdate(impl->ctx, data->data, data->len) != HCF_OPENSSL_SUCCESS) {
            HcfPrintOpensslError();
            return false;
        }
        impl->status = READY;
    }
    if (impl->status != READY) {
        LOGE("The message has not been transferred.");
        return false;
    }
    if (EVP_DigestVerifyFinal(impl->ctx, signatureData->data, signatureData->len) != HCF_OPENSSL_SUCCESS) {
        HcfPrintOpensslError();
        return false;
    }
    LOGI("end ...");
    return true;
}

HcfResult HcfSignSpiEcdsaCreate(HcfSignatureParams *params, HcfSignSpi **returnObj)
{
    if ((params == NULL) || (returnObj == NULL)) {
        LOGE("Invalid input parameter.");
        return HCF_INVALID_PARAMS;
    }
    int32_t curveId;
    if (GetOpensslCurveId(params->keyLen, &curveId) != HCF_SUCCESS) {
        return HCF_INVALID_PARAMS;
    }
    if (!IsDigestAlgValid(params->md)) {
        return HCF_INVALID_PARAMS;
    }
    const EVP_MD *opensslAlg = GetOpensslDigestAlg(params->md);
    if (opensslAlg == NULL) {
        return HCF_INVALID_PARAMS;
    }

    HcfSignSpiEcdsaOpensslImpl *returnImpl = (HcfSignSpiEcdsaOpensslImpl *)HcfMalloc(
        sizeof(HcfSignSpiEcdsaOpensslImpl), 0);
    if (returnImpl == NULL) {
        LOGE("Failed to allocate returnImpl memroy!");
        return HCF_ERR_MALLOC;
    }
    returnImpl->base.base.getClass = GetEcdsaSignClass;
    returnImpl->base.base.destroy = DestroyEcdsaSign;
    returnImpl->base.engineInit = EngineSignInit;
    returnImpl->base.engineUpdate = EngineSignUpdate;
    returnImpl->base.engineSign = EngineSignDoFinal;
    returnImpl->curveId = curveId;
    returnImpl->digestAlg = opensslAlg;
    returnImpl->status = UNINITIALIZED;
    returnImpl->ctx = EVP_MD_CTX_create();
    if (returnImpl->ctx == NULL) {
        LOGE("Failed to allocate ctx memory!");
        HcfFree(returnImpl);
        return HCF_ERR_MALLOC;
    }

    *returnObj = (HcfSignSpi *)returnImpl;
    return HCF_SUCCESS;
}

HcfResult HcfVerifySpiEcdsaCreate(HcfSignatureParams *params, HcfVerifySpi **returnObj)
{
    if ((params == NULL) || (returnObj == NULL)) {
        LOGE("Invalid input parameter.");
        return HCF_INVALID_PARAMS;
    }
    int32_t curveId;
    if (GetOpensslCurveId(params->keyLen, &curveId) != HCF_SUCCESS) {
        return HCF_INVALID_PARAMS;
    }
    if (!IsDigestAlgValid(params->md)) {
        return HCF_INVALID_PARAMS;
    }
    const EVP_MD *opensslAlg = GetOpensslDigestAlg(params->md);
    if (opensslAlg == NULL) {
        return HCF_INVALID_PARAMS;
    }

    HcfVerifySpiEcdsaOpensslImpl *returnImpl = (HcfVerifySpiEcdsaOpensslImpl *)HcfMalloc(
        sizeof(HcfVerifySpiEcdsaOpensslImpl), 0);
    if (returnImpl == NULL) {
        LOGE("Failed to allocate returnImpl memroy!");
        return HCF_ERR_MALLOC;
    }
    returnImpl->base.base.getClass = GetEcdsaVerifyClass;
    returnImpl->base.base.destroy = DestroyEcdsaVerify;
    returnImpl->base.engineInit = EngineVerifyInit;
    returnImpl->base.engineUpdate = EngineVerifyUpdate;
    returnImpl->base.engineVerify = EngineVerifyDoFinal;
    returnImpl->curveId = curveId;
    returnImpl->digestAlg = opensslAlg;
    returnImpl->status = UNINITIALIZED;
    returnImpl->ctx = EVP_MD_CTX_create();
    if (returnImpl->ctx == NULL) {
        LOGE("Failed to allocate ctx memory!");
        HcfFree(returnImpl);
        return HCF_ERR_MALLOC;
    }

    *returnObj = (HcfVerifySpi *)returnImpl;
    return HCF_SUCCESS;
}
