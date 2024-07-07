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

#include "signature_rsa_openssl.h"
#include <string.h>
#include <openssl/evp.h>

#include "algorithm_parameter.h"
#include "log.h"
#include "memory.h"
#include "openssl_class.h"
#include "openssl_common.h"
#include "rsa_openssl_common.h"
#include "utils.h"

typedef struct {
    HcfSignSpi base;

    EVP_MD_CTX *mdctx;

    int32_t padding;

    int32_t md;

    int32_t mgf1md;

    InitFlag initFlag;
} HcfSignSpiRsaOpensslImpl;

typedef struct {
    HcfVerifySpi base;

    EVP_MD_CTX *mdctx;

    int32_t padding;

    int32_t md;

    int32_t mgf1md;

    InitFlag initFlag;
} HcfVerifySpiRsaOpensslImpl;

static const char *GetRsaSignClass(void)
{
    return OPENSSL_RSA_SIGN_CLASS;
}

static const char *GetRsaVerifyClass(void)
{
    return OPENSSL_RSA_VERIFY_CLASS;
}

static void DestroyRsaSign(HcfObjectBase *self)
{
    if (self == NULL) {
        LOGE("Class is null");
        return;
    }
    if (!IsClassMatch(self, OPENSSL_RSA_SIGN_CLASS)) {
        LOGE("Class not match.");
        return;
    }
    HcfSignSpiRsaOpensslImpl *impl = (HcfSignSpiRsaOpensslImpl *)self;
    EVP_MD_CTX_destroy(impl->mdctx);
    impl->mdctx = NULL;
    HcfFree(impl);
    impl = NULL;
    LOGI("DestroyRsaSign success.");
}

static void DestroyRsaVerify(HcfObjectBase *self)
{
    if (self == NULL) {
        LOGE("Class is null");
        return;
    }
    if (!IsClassMatch(self, OPENSSL_RSA_VERIFY_CLASS)) {
        LOGE("Class not match.");
        return;
    }
    HcfVerifySpiRsaOpensslImpl *impl = (HcfVerifySpiRsaOpensslImpl *)self;
    EVP_MD_CTX_destroy(impl->mdctx);
    impl->mdctx = NULL;
    HcfFree(impl);
    impl = NULL;
    LOGI("DestroyRsaVerify success.");
}

static HcfResult CheckInitKeyType(HcfKey *key, bool signing)
{
    if (signing) {
        if (!IsClassMatch((HcfObjectBase *)key, OPENSSL_RSA_PRIKEY_CLASS)) {
            LOGE("Input keyType dismatch with sign option, please use priKey.");
            return HCF_INVALID_PARAMS;
        }
    } else {
        if (!IsClassMatch((HcfObjectBase *)key, OPENSSL_RSA_PUBKEY_CLASS)) {
            LOGE("Input keyType dismatch with sign option, please use pubKey.");
            return HCF_INVALID_PARAMS;
        }
    }
    return HCF_SUCCESS;
}

static EVP_PKEY *InitRsaEvpKey(const HcfKey *key, bool signing)
{
    RSA *rsa = NULL;
    if (DuplicateRsa(signing ? ((HcfOpensslRsaPriKey *)key)->sk : ((HcfOpensslRsaPubKey *)key)->pk, signing, &rsa)
        != HCF_SUCCESS) {
            LOGE("dup pub rsa fail.");
            return NULL;
        }
    if (rsa == NULL) {
        LOGE("The Key has lost.");
        return NULL;
    }
    EVP_PKEY *pkey = NewEvpPkeyByRsa(rsa, false);
    if (pkey == NULL) {
        LOGE("New evp pkey failed");
        HcfPrintOpensslError();
        RSA_free(rsa);
        return NULL;
    }
    return pkey;
}

static HcfResult SetPaddingAndDigest(EVP_PKEY_CTX *ctx, int32_t hcfPadding, int32_t md, int32_t mgf1md)
{
    int32_t opensslPadding = 0;
    if (GetOpensslPadding(hcfPadding, &opensslPadding) != HCF_SUCCESS) {
        LOGE("getpadding fail.");
        return HCF_INVALID_PARAMS;
    }
    if (EVP_PKEY_CTX_set_rsa_padding(ctx, opensslPadding) != HCF_OPENSSL_SUCCESS) {
        LOGE("EVP_PKEY_CTX_set_rsa_padding fail");
        HcfPrintOpensslError();
        return HCF_ERR_CRYPTO_OPERATION;
    }
    if (hcfPadding == HCF_OPENSSL_RSA_PSS_PADDING) {
        LOGI("padding is pss, set mgf1 md");
        if (EVP_PKEY_CTX_set_rsa_mgf1_md(ctx, GetOpensslDigestAlg(mgf1md)) != HCF_OPENSSL_SUCCESS) {
            LOGE("EVP_PKEY_CTX_set_rsa_mgf1_md fail");
            HcfPrintOpensslError();
            return HCF_ERR_CRYPTO_OPERATION;
        }
    }
    return HCF_SUCCESS;
}


static HcfResult SetSignParams(HcfSignSpiRsaOpensslImpl *impl, HcfPriKey *privateKey)
{
    EVP_PKEY *dupKey = InitRsaEvpKey((HcfKey *)privateKey, true);
    if (dupKey == NULL) {
        LOGE("InitRsaEvpKey fail.");
        return HCF_ERR_CRYPTO_OPERATION;
    }
    EVP_PKEY_CTX *ctx = NULL;
    if (EVP_DigestSignInit(impl->mdctx, &ctx, GetOpensslDigestAlg(impl->md), NULL, dupKey) != HCF_OPENSSL_SUCCESS) {
        LOGE("EVP_DigestSignInit fail.");
        EVP_PKEY_free(dupKey);
        return HCF_ERR_CRYPTO_OPERATION;
    }
    if (SetPaddingAndDigest(ctx, impl->padding, impl->md, impl->mgf1md) != HCF_SUCCESS) {
        LOGE("set padding and digest fail");
        EVP_PKEY_free(dupKey);
        return HCF_ERR_CRYPTO_OPERATION;
    }
    EVP_PKEY_free(dupKey);
    return HCF_SUCCESS;
}

static HcfResult EngineSignInit(HcfSignSpi *self, HcfParamsSpec *params, HcfPriKey *privateKey)
{
    LOGI("EngineSignInit start");
    if (self == NULL || privateKey == NULL) {
        LOGE("Invalid input params");
        return HCF_INVALID_PARAMS;
    }
    if (!IsClassMatch((HcfObjectBase *)self, OPENSSL_RSA_SIGN_CLASS)) {
        LOGE("Class not match.");
        return HCF_INVALID_PARAMS;
    }
    HcfSignSpiRsaOpensslImpl *impl = (HcfSignSpiRsaOpensslImpl *)self;
    if (impl->initFlag != UNINITIALIZED) {
        LOGE("Sign has been init");
        return HCF_INVALID_PARAMS;
    }
    if (CheckInitKeyType((HcfKey *)privateKey, true) != HCF_SUCCESS) {
        LOGE("KeyType dismatch.");
        return HCF_INVALID_PARAMS;
    }
    if (SetSignParams(impl, privateKey) != HCF_SUCCESS) {
        LOGE("Sign set padding or md fail");
        return HCF_ERR_CRYPTO_OPERATION;
    }
    impl->initFlag = INITIALIZED;
    LOGI("EngineSignInit end");
    return HCF_SUCCESS;
}

static HcfResult SetVerifyParams(HcfVerifySpiRsaOpensslImpl *impl, HcfPubKey *publicKey)
{
    EVP_PKEY_CTX *ctx = NULL;
    EVP_PKEY *dupKey = InitRsaEvpKey((HcfKey *)publicKey, false);
    if (dupKey == NULL) {
        LOGE("InitRsaEvpKey fail.");
        return HCF_ERR_CRYPTO_OPERATION;
    }
    if (EVP_DigestVerifyInit(impl->mdctx, &ctx, GetOpensslDigestAlg(impl->md), NULL, dupKey) != HCF_OPENSSL_SUCCESS) {
        LOGE("EVP_DigestVerifyInit fail.");
        HcfPrintOpensslError();
        EVP_PKEY_free(dupKey);
        return HCF_ERR_CRYPTO_OPERATION;
    }
    if (SetPaddingAndDigest(ctx, impl->padding, impl->md, impl->mgf1md) != HCF_SUCCESS) {
        LOGE("set padding and digest fail");
        EVP_PKEY_free(dupKey);
        return HCF_ERR_CRYPTO_OPERATION;
    }
    EVP_PKEY_free(dupKey);
    return HCF_SUCCESS;
}

static HcfResult EngineVerifyInit(HcfVerifySpi *self, HcfParamsSpec *params, HcfPubKey *publicKey)
{
    LOGI("EngineVerifyInit start");
    if (self == NULL || publicKey == NULL) {
        LOGE("Invalid input params.");
        return HCF_INVALID_PARAMS;
    }
    if (!IsClassMatch((HcfObjectBase *)self, OPENSSL_RSA_VERIFY_CLASS)) {
        LOGE("Class not match.");
        return HCF_INVALID_PARAMS;
    }
    HcfVerifySpiRsaOpensslImpl *impl = (HcfVerifySpiRsaOpensslImpl *)self;
    if (impl->initFlag != UNINITIALIZED) {
        LOGE("Verigy has been init.");
        return HCF_INVALID_PARAMS;
    }
    if (CheckInitKeyType((HcfKey *)publicKey, false) != HCF_SUCCESS) {
        LOGE("KeyType dismatch.");
        return HCF_INVALID_PARAMS;
    }
    if (SetVerifyParams(impl, publicKey) != HCF_SUCCESS) {
        LOGE("Verify set padding or md fail");
        return HCF_ERR_CRYPTO_OPERATION;
    }
    impl->initFlag = INITIALIZED;
    LOGI("EngineVerifyInit end");
    return HCF_SUCCESS;
}

static HcfResult EngineSignUpdate(HcfSignSpi *self, HcfBlob *data)
{
    LOGI("start ...");
    if ((self == NULL) || (data == NULL) || (data->data == NULL)) {
        LOGE("Invalid input parameter.");
        return HCF_INVALID_PARAMS;
    }
    if (!IsClassMatch((HcfObjectBase *)self, OPENSSL_RSA_SIGN_CLASS)) {
        LOGE("Class not match.");
        return HCF_INVALID_PARAMS;
    }
    HcfSignSpiRsaOpensslImpl *impl = (HcfSignSpiRsaOpensslImpl *)self;
    if (impl->initFlag != INITIALIZED) {
        LOGE("The Sign has not been init");
        return HCF_INVALID_PARAMS;
    }
    if (EVP_DigestSignUpdate(impl->mdctx, data->data, data->len) != HCF_OPENSSL_SUCCESS) {
        LOGE("EVP_DigestSignUpdate fail");
        return HCF_ERR_CRYPTO_OPERATION;
    }
    LOGI("end ...");
    return HCF_SUCCESS;
}

static HcfResult EngineVerifyUpdate(HcfVerifySpi *self, HcfBlob *data)
{
    LOGI("start ...");
    if ((self == NULL) || (data == NULL) || (data->data == NULL)) {
        LOGE("Invalid input parameter.");
        return HCF_INVALID_PARAMS;
    }
    if (!IsClassMatch((HcfObjectBase *)self, OPENSSL_RSA_VERIFY_CLASS)) {
        LOGE("Class not match.");
        return HCF_INVALID_PARAMS;
    }
    HcfVerifySpiRsaOpensslImpl *impl = (HcfVerifySpiRsaOpensslImpl *)self;
    if (impl->initFlag != INITIALIZED) {
        LOGE("The Sign has not been init");
        return HCF_INVALID_PARAMS;
    }
    if (EVP_DigestVerifyUpdate(impl->mdctx, data->data, data->len) != HCF_OPENSSL_SUCCESS) {
        LOGE("EVP_DigestSignUpdate fail");
        return HCF_ERR_CRYPTO_OPERATION;
    }
    LOGI("end ...");
    return HCF_SUCCESS;
}

static HcfResult EngineSign(HcfSignSpi *self, HcfBlob *data, HcfBlob *returnSignatureData)
{
    LOGI("EngineSign start");
    if (self == NULL || returnSignatureData == NULL) {
        LOGE("Invalid input params.");
        return HCF_INVALID_PARAMS;
    }
    if (!IsClassMatch((HcfObjectBase *)self, OPENSSL_RSA_SIGN_CLASS)) {
        LOGE("Class not match.");
        return HCF_INVALID_PARAMS;
    }
    HcfSignSpiRsaOpensslImpl *impl = (HcfSignSpiRsaOpensslImpl *)self;
    if (impl->initFlag != INITIALIZED) {
        LOGE("The Sign has not been init");
        return HCF_INVALID_PARAMS;
    }
    if (data != NULL && data->data != NULL) {
        if (EVP_DigestSignUpdate(impl->mdctx, data->data, data->len) != HCF_OPENSSL_SUCCESS) {
            LOGE("Dofinal update data fail.");
            return HCF_ERR_CRYPTO_OPERATION;
        }
    }
    size_t maxLen;
    if (EVP_DigestSignFinal(impl->mdctx, NULL, &maxLen) != HCF_OPENSSL_SUCCESS) {
        LOGE("EVP_DigestSignFinal fail");
        return HCF_ERR_CRYPTO_OPERATION;
    }
    uint8_t *outData = (uint8_t *)HcfMalloc(maxLen, 0);
    if (outData == NULL) {
        LOGE("Failed to allocate outData memory!");
        return HCF_ERR_MALLOC;
    }
    size_t actualLen = maxLen;
    if (EVP_DigestSignFinal(impl->mdctx, outData, &actualLen) != HCF_OPENSSL_SUCCESS) {
        LOGE("EVP_DigestSignFinal fail");
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

    LOGI("EngineSign end");
    return HCF_SUCCESS;
}

static bool EngineVerify(HcfVerifySpi *self, HcfBlob *data, HcfBlob *signatureData)
{
    LOGI("EngineVerify start");
    if (self == NULL || signatureData == NULL || signatureData->data == NULL) {
        LOGE("Invalid input params");
        return false;
    }
    if (!IsClassMatch((HcfObjectBase *)self, OPENSSL_RSA_VERIFY_CLASS)) {
        LOGE("Class not match.");
        return false;
    }

    HcfVerifySpiRsaOpensslImpl *impl = (HcfVerifySpiRsaOpensslImpl *)self;
    if (impl->initFlag != INITIALIZED) {
        LOGE("The Sign has not been init");
        return false;
    }
    if (data != NULL && data->data != NULL) {
        if (EVP_DigestVerifyUpdate(impl->mdctx, data->data, data->len) != HCF_OPENSSL_SUCCESS) {
            LOGE("EVP_DigestVerifyUpdate fail");
            return false;
        }
    }
    if (EVP_DigestVerifyFinal(impl->mdctx, signatureData->data, signatureData->len) != HCF_OPENSSL_SUCCESS) {
        LOGE("EVP_DigestVerifyFinal fail");
        return false;
    }
    LOGI("EngineVerify end");
    return true;
}

static HcfResult CheckSignatureParams(HcfSignatureParams *params)
{
    if (GetOpensslDigestAlg(params->md) == NULL) {
        LOGE("Md is NULL");
        return HCF_INVALID_PARAMS;
    }
    if (params->padding == HCF_OPENSSL_RSA_PSS_PADDING && GetOpensslDigestAlg(params->mgf1md) == NULL) {
        LOGE("Use pss padding, but mgf1md is NULL");
        return HCF_INVALID_PARAMS;
    }
    return HCF_SUCCESS;
}

HcfResult HcfSignSpiRsaCreate(HcfSignatureParams *params, HcfSignSpi **returnObj)
{
    LOGI("HcfSignSpiRsaCreate start");
    if (params == NULL || returnObj == NULL) {
        LOGE("Invalid input parameter.");
        return HCF_INVALID_PARAMS;
    }
    if (CheckSignatureParams(params) != HCF_SUCCESS) {
        return HCF_INVALID_PARAMS;
    }
    HcfSignSpiRsaOpensslImpl *returnImpl = (HcfSignSpiRsaOpensslImpl *)HcfMalloc(
        sizeof(HcfSignSpiRsaOpensslImpl), 0);
    if (returnImpl == NULL) {
        LOGE("Failed to allocate returnImpl memroy!");
        return HCF_ERR_MALLOC;
    }
    returnImpl->base.base.getClass = GetRsaSignClass;
    returnImpl->base.base.destroy = DestroyRsaSign;
    returnImpl->base.engineInit = EngineSignInit;
    returnImpl->base.engineUpdate = EngineSignUpdate;
    returnImpl->base.engineSign = EngineSign;

    returnImpl->md = params->md;
    returnImpl->padding = params->padding;
    returnImpl->mgf1md = params->mgf1md;
    returnImpl->mdctx = EVP_MD_CTX_create();
    returnImpl->initFlag = UNINITIALIZED;
    *returnObj = (HcfSignSpi *)returnImpl;
    LOGI("HcfSignSpiRsaCreate end");
    return HCF_SUCCESS;
}

HcfResult HcfVerifySpiRsaCreate(HcfSignatureParams *params, HcfVerifySpi **returnObj)
{
    LOGI("HcfSignSpiRsaCreate start");
    if (params == NULL || returnObj == NULL) {
        LOGE("Invalid input parameter.");
        return HCF_INVALID_PARAMS;
    }
    if (CheckSignatureParams(params) != HCF_SUCCESS) {
        return HCF_INVALID_PARAMS;
    }
    HcfVerifySpiRsaOpensslImpl *returnImpl = (HcfVerifySpiRsaOpensslImpl *)HcfMalloc(
        sizeof(HcfVerifySpiRsaOpensslImpl), 0);
    if (returnImpl == NULL) {
        LOGE("Failed to allocate returnImpl memroy!");
        return HCF_ERR_MALLOC;
    }
    returnImpl->base.base.getClass = GetRsaVerifyClass;
    returnImpl->base.base.destroy = DestroyRsaVerify;
    returnImpl->base.engineInit = EngineVerifyInit;
    returnImpl->base.engineUpdate = EngineVerifyUpdate;
    returnImpl->base.engineVerify = EngineVerify;

    returnImpl->md = params->md;
    returnImpl->padding = params->padding;
    returnImpl->mgf1md = params->mgf1md;
    returnImpl->mdctx = EVP_MD_CTX_create();
    returnImpl->initFlag = UNINITIALIZED;
    *returnObj = (HcfVerifySpi *)returnImpl;
    LOGI("HcfSignSpiRsaCreate end");
    return HCF_SUCCESS;
}
