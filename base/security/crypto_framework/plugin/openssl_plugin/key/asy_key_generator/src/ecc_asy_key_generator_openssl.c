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

#include "ecc_asy_key_generator_openssl.h"

#include "securec.h"
#include "openssl/pem.h"
#include "openssl/x509.h"

#include "algorithm_parameter.h"
#include "log.h"
#include "memory.h"
#include "openssl_class.h"
#include "openssl_common.h"
#include "utils.h"

#define OPENSSL_ECC_KEY_GENERATOR_CLASS "OPENSSL.ECC.KEY_GENERATOR_CLASS"
#define OPENSSL_ECC_ALGORITHM "EC"
#define OPENSSL_ECC_PUB_KEY_FORMAT "X.509"
#define OPENSSL_ECC_PRI_KEY_FORMAT "PKCS#8"

typedef struct {
    HcfAsyKeyGeneratorSpi base;

    int32_t curveId;
} HcfAsyKeyGeneratorSpiOpensslEccImpl;

static HcfResult NewEcKeyPairByOpenssl(int32_t curveId, EC_POINT **returnPubKey, BIGNUM **returnPriKey)
{
    EC_KEY *ecKey = EC_KEY_new_by_curve_name(curveId);
    if (ecKey == NULL) {
        LOGE("new ec key failed.");
        return HCF_ERR_CRYPTO_OPERATION;
    }
    if (EC_KEY_generate_key(ecKey) <= 0) {
        LOGE("generate ec key failed.");
        EC_KEY_free(ecKey);
        return HCF_ERR_CRYPTO_OPERATION;
    }
    if (EC_KEY_check_key(ecKey) <= 0) {
        LOGE("check key fail.");
        EC_KEY_free(ecKey);
        return HCF_ERR_CRYPTO_OPERATION;
    }
    const EC_POINT *pubKey = EC_KEY_get0_public_key(ecKey);
    const BIGNUM *priKey = EC_KEY_get0_private_key(ecKey);
    const EC_GROUP *group = EC_KEY_get0_group(ecKey);
    if ((pubKey == NULL) || (priKey == NULL) || (group == NULL)) {
        LOGE("ec key is invalid.");
        EC_KEY_free(ecKey);
        return HCF_ERR_CRYPTO_OPERATION;
    }
    EC_POINT *newPubKey = EC_POINT_dup(pubKey, group);
    if (newPubKey == NULL) {
        LOGE("copy pubKey fail.");
        EC_KEY_free(ecKey);
        return HCF_ERR_CRYPTO_OPERATION;
    }
    BIGNUM *newPriKey = BN_dup(priKey);
    if (newPriKey == NULL) {
        LOGE("copy priKey fail.");
        EC_KEY_free(ecKey);
        EC_POINT_free(newPubKey);
        return HCF_ERR_CRYPTO_OPERATION;
    }

    *returnPubKey = newPubKey;
    *returnPriKey = newPriKey;
    EC_KEY_free(ecKey);
    return HCF_SUCCESS;
}

// export interfaces
static const char *GetEccKeyPairGeneratorClass(void)
{
    return OPENSSL_ECC_KEY_GENERATOR_CLASS;
}

static const char *GetEccKeyPairClass(void)
{
    return HCF_OPENSSL_ECC_KEY_PAIR_CLASS;
}

static const char *GetEccPubKeyClass(void)
{
    return HCF_OPENSSL_ECC_PUB_KEY_CLASS;
}

static const char *GetEccPriKeyClass(void)
{
    return HCF_OPENSSL_ECC_PRI_KEY_CLASS;
}

static void DestroyEccKeyPairGenerator(HcfObjectBase *self)
{
    if (self == NULL) {
        return;
    }
    if (!IsClassMatch(self, GetEccKeyPairGeneratorClass())) {
        return;
    }
    HcfFree(self);
}

static void DestroyEccPubKey(HcfObjectBase *self)
{
    if (self == NULL) {
        return;
    }
    if (!IsClassMatch(self, GetEccPubKeyClass())) {
        return;
    }
    HcfOpensslEccPubKey *impl = (HcfOpensslEccPubKey *)self;
    EC_POINT_free(impl->pk);
    impl->pk = NULL;
    HcfFree(impl);
}

static void DestroyEccPriKey(HcfObjectBase *self)
{
    if (self == NULL) {
        return;
    }
    if (!IsClassMatch(self, GetEccPriKeyClass())) {
        return;
    }
    HcfOpensslEccPriKey *impl = (HcfOpensslEccPriKey *)self;
    BN_clear_free(impl->sk);
    impl->sk = NULL;
    HcfFree(impl);
}

static void DestroyEccKeyPair(HcfObjectBase *self)
{
    if (self == NULL) {
        return;
    }
    if (!IsClassMatch(self, GetEccKeyPairClass())) {
        return;
    }
    HcfOpensslEccKeyPair *impl = (HcfOpensslEccKeyPair *)self;
    if (impl->base.pubKey != NULL) {
        DestroyEccPubKey((HcfObjectBase *)impl->base.pubKey);
        impl->base.pubKey = NULL;
    }
    if (impl->base.priKey != NULL) {
        DestroyEccPriKey((HcfObjectBase *)impl->base.priKey);
        impl->base.priKey = NULL;
    }
    HcfFree(impl);
}

static void DestroyKey(HcfObjectBase *self)
{
    LOGI("Process DestroyKey");
}

static const char *GetEccPubKeyAlgorithm(HcfKey *self)
{
    if (self == NULL) {
        LOGE("Invalid input parameter.");
        return NULL;
    }
    if (!IsClassMatch((HcfObjectBase *)self, HCF_OPENSSL_ECC_PUB_KEY_CLASS)) {
        return NULL;
    }
    return OPENSSL_ECC_ALGORITHM;
}

static const char *GetEccPriKeyAlgorithm(HcfKey *self)
{
    if (self == NULL) {
        LOGE("Invalid input parameter.");
        return NULL;
    }
    if (!IsClassMatch((HcfObjectBase *)self, HCF_OPENSSL_ECC_PRI_KEY_CLASS)) {
        return NULL;
    }
    return OPENSSL_ECC_ALGORITHM;
}

static const char *GetEccPubKeyFormat(HcfKey *self)
{
    if (self == NULL) {
        LOGE("Invalid input parameter.");
        return NULL;
    }
    if (!IsClassMatch((HcfObjectBase *)self, HCF_OPENSSL_ECC_PUB_KEY_CLASS)) {
        return NULL;
    }
    return OPENSSL_ECC_PUB_KEY_FORMAT;
}

static const char *GetEccPriKeyFormat(HcfKey *self)
{
    if (self == NULL) {
        LOGE("Invalid input parameter.");
        return NULL;
    }
    if (!IsClassMatch((HcfObjectBase *)self, HCF_OPENSSL_ECC_PRI_KEY_CLASS)) {
        return NULL;
    }
    return OPENSSL_ECC_PRI_KEY_FORMAT;
}

static HcfResult GetEccPubKeyEncoded(HcfKey *self, HcfBlob *returnBlob)
{
    LOGI("start ...");
    if ((self == NULL) || (returnBlob == NULL)) {
        LOGE("Invalid input parameter.");
        return HCF_INVALID_PARAMS;
    }
    if (!IsClassMatch((HcfObjectBase *)self, HCF_OPENSSL_ECC_PUB_KEY_CLASS)) {
        return HCF_INVALID_PARAMS;
    }

    HcfOpensslEccPubKey *impl = (HcfOpensslEccPubKey *)self;
    if (impl->pk == NULL) {
        LOGE("Empty public key!");
        return HCF_INVALID_PARAMS;
    }
    EC_KEY *ecKey = EC_KEY_new_by_curve_name(impl->curveId);
    if (ecKey == NULL) {
        LOGE("EC_KEY_new_by_curve_name fail.");
        HcfPrintOpensslError();
        return HCF_ERR_CRYPTO_OPERATION;
    }
    if (EC_KEY_set_public_key(ecKey, impl->pk) <= 0) {
        LOGE("EC_KEY_set_public_key fail.");
        HcfPrintOpensslError();
        EC_KEY_free(ecKey);
        return HCF_ERR_CRYPTO_OPERATION;
    }
    unsigned char *returnData = NULL;
    int returnDataLen = i2d_EC_PUBKEY(ecKey, &returnData);
    EC_KEY_free(ecKey);
    if (returnDataLen <= 0) {
        LOGE("i2d_EC_PUBKEY fail");
        HcfPrintOpensslError();
        return HCF_ERR_CRYPTO_OPERATION;
    }
    returnBlob->data = returnData;
    returnBlob->len = returnDataLen;
    LOGI("end ...");
    return HCF_SUCCESS;
}

static HcfResult GetEccPriKeyEncoded(HcfKey *self, HcfBlob *returnBlob)
{
    LOGI("start ...");
    if ((self == NULL) || (returnBlob == NULL)) {
        LOGE("Invalid input parameter.");
        return HCF_INVALID_PARAMS;
    }
    if (!IsClassMatch((HcfObjectBase *)self, HCF_OPENSSL_ECC_PRI_KEY_CLASS)) {
        return HCF_INVALID_PARAMS;
    }

    HcfOpensslEccPriKey *impl = (HcfOpensslEccPriKey *)self;
    if (impl->sk == NULL) {
        LOGE("Empty private key!");
        return HCF_INVALID_PARAMS;
    }

    EC_KEY *ecKey = EC_KEY_new_by_curve_name(impl->curveId);
    if (ecKey == NULL) {
        LOGE("EC_KEY_new_by_curve_name fail.");
        HcfPrintOpensslError();
        return HCF_ERR_CRYPTO_OPERATION;
    }
    if (EC_KEY_set_private_key(ecKey, (impl->sk)) != HCF_OPENSSL_SUCCESS) {
        LOGE("EC_KEY_set_private_key fail.");
        HcfPrintOpensslError();
        EC_KEY_free(ecKey);
        return HCF_ERR_CRYPTO_OPERATION;
    }
    EC_KEY_set_asn1_flag(ecKey, OPENSSL_EC_NAMED_CURVE);
    EC_KEY_set_enc_flags(ecKey, EC_PKEY_NO_PUBKEY);
    unsigned char *returnData = NULL;
    int returnDataLen = i2d_ECPrivateKey(ecKey, &returnData);
    EC_KEY_free(ecKey);
    if (returnDataLen <= 0) {
        LOGE("i2d_ECPrivateKey fail.");
        HcfPrintOpensslError();
        return HCF_ERR_CRYPTO_OPERATION;
    }
    returnBlob->data = returnData;
    returnBlob->len = returnDataLen;
    LOGI("end ...");
    return HCF_SUCCESS;
}

static void EccPriKeyClearMem(HcfPriKey *self)
{
    if (self == NULL) {
        return;
    }
    if (!IsClassMatch((HcfObjectBase *)self, GetEccPriKeyClass())) {
        return;
    }
    HcfOpensslEccPriKey *impl = (HcfOpensslEccPriKey *)self;
    BN_clear(impl->sk);
}

static HcfResult CreateEccPubKey(int32_t curveId, EC_POINT *pubKey, HcfOpensslEccPubKey **returnObj)
{
    HcfOpensslEccPubKey *returnPubKey = (HcfOpensslEccPubKey *)HcfMalloc(sizeof(HcfOpensslEccPubKey), 0);
    if (returnPubKey == NULL) {
        LOGE("Failed to allocate returnPubKey memory!");
        return HCF_ERR_MALLOC;
    }
    returnPubKey->base.base.base.destroy = DestroyKey;
    returnPubKey->base.base.base.getClass = GetEccPubKeyClass;
    returnPubKey->base.base.getAlgorithm = GetEccPubKeyAlgorithm;
    returnPubKey->base.base.getEncoded = GetEccPubKeyEncoded;
    returnPubKey->base.base.getFormat = GetEccPubKeyFormat;
    returnPubKey->curveId = curveId;
    returnPubKey->pk = pubKey;

    *returnObj = returnPubKey;
    return HCF_SUCCESS;
}

static HcfResult CreateEccPriKey(int32_t curveId, BIGNUM *priKey, HcfOpensslEccPriKey **returnObj)
{
    HcfOpensslEccPriKey *returnPriKey = (HcfOpensslEccPriKey *)HcfMalloc(sizeof(HcfOpensslEccPriKey), 0);
    if (returnPriKey == NULL) {
        LOGE("Failed to allocate returnPriKey memory!");
        return HCF_ERR_MALLOC;
    }
    returnPriKey->base.base.base.destroy = DestroyKey;
    returnPriKey->base.base.base.getClass = GetEccPriKeyClass;
    returnPriKey->base.base.getAlgorithm = GetEccPriKeyAlgorithm;
    returnPriKey->base.base.getEncoded = GetEccPriKeyEncoded;
    returnPriKey->base.base.getFormat = GetEccPriKeyFormat;
    returnPriKey->base.clearMem = EccPriKeyClearMem;
    returnPriKey->curveId = curveId;
    returnPriKey->sk = priKey;

    *returnObj = returnPriKey;
    return HCF_SUCCESS;
}

static HcfResult CreateEccKeyPair(HcfOpensslEccPubKey *pubKey, HcfOpensslEccPriKey *priKey,
    HcfOpensslEccKeyPair **returnObj)
{
    HcfOpensslEccKeyPair *returnKeyPair = (HcfOpensslEccKeyPair *)HcfMalloc(sizeof(HcfOpensslEccKeyPair), 0);
    if (returnKeyPair == NULL) {
        LOGE("Failed to allocate returnKeyPair memory!");
        return HCF_ERR_MALLOC;
    }
    returnKeyPair->base.base.getClass = GetEccKeyPairClass;
    returnKeyPair->base.base.destroy = DestroyEccKeyPair;
    returnKeyPair->base.pubKey = (HcfPubKey *)pubKey;
    returnKeyPair->base.priKey = (HcfPriKey *)priKey;

    *returnObj = returnKeyPair;
    return HCF_SUCCESS;
}

static HcfResult ConvertEcPubKeyByOpenssl(int32_t curveId, HcfBlob *pubKeyBlob, HcfOpensslEccPubKey **returnPubKey)
{
    const unsigned char *tmpData = (const unsigned char *)(pubKeyBlob->data);
    EC_KEY *ecKey = d2i_EC_PUBKEY(NULL, &tmpData, pubKeyBlob->len);
    if (ecKey == NULL) {
        LOGE("d2i_EC_PUBKEY fail.");
        HcfPrintOpensslError();
        return HCF_ERR_CRYPTO_OPERATION;
    }
    const EC_POINT *pubKey = EC_KEY_get0_public_key(ecKey);
    const EC_GROUP *group = EC_KEY_get0_group(ecKey);
    if (pubKey == NULL || group == NULL) {
        LOGE("ec key is invalid.");
        HcfPrintOpensslError();
        EC_KEY_free(ecKey);
        return HCF_ERR_CRYPTO_OPERATION;
    }
    EC_POINT *newPubKey = EC_POINT_dup(pubKey, group);
    EC_KEY_free(ecKey);
    if (newPubKey == NULL) {
        LOGE("copy pubKey fail.");
        return HCF_ERR_CRYPTO_OPERATION;
    }
    int32_t res = CreateEccPubKey(curveId, newPubKey, returnPubKey);
    if (res != HCF_SUCCESS) {
        EC_POINT_free(newPubKey);
        return res;
    }
    return HCF_SUCCESS;
}

static HcfResult ConvertEcPriKeyByOpenssl(int32_t curveId, HcfBlob *priKeyBlob, HcfOpensslEccPriKey **returnPriKey)
{
    const unsigned char *tmpData = (const unsigned char *)(priKeyBlob->data);
    EC_KEY *ecKey = d2i_ECPrivateKey(NULL, &tmpData, priKeyBlob->len);
    if (ecKey == NULL) {
        LOGE("d2i_ECPrivateKey fail");
        HcfPrintOpensslError();
        return HCF_ERR_CRYPTO_OPERATION;
    }
    const BIGNUM *priKey = EC_KEY_get0_private_key(ecKey);
    if (priKey == NULL) {
        LOGE("ec key is invalid.");
        HcfPrintOpensslError();
        EC_KEY_free(ecKey);
        return HCF_ERR_CRYPTO_OPERATION;
    }
    BIGNUM *newPriKey = BN_dup(priKey);
    EC_KEY_free(ecKey);
    if (newPriKey == NULL) {
        LOGE("copy priKey fail.");
        return HCF_ERR_CRYPTO_OPERATION;
    }
    int32_t res = CreateEccPriKey(curveId, newPriKey, returnPriKey);
    if (res != HCF_SUCCESS) {
        BN_clear_free(newPriKey);
        return res;
    }
    return HCF_SUCCESS;
}

static HcfResult EngineConvertEccKey(HcfAsyKeyGeneratorSpi *self, HcfParamsSpec *params, HcfBlob *pubKeyBlob,
    HcfBlob *priKeyBlob, HcfKeyPair **returnKeyPair)
{
    LOGI("start ...");
    (void)params;
    if ((self == NULL) || (returnKeyPair == NULL)) {
        LOGE("Invalid input parameter.");
        return HCF_INVALID_PARAMS;
    }
    if (!IsClassMatch((HcfObjectBase *)self, GetEccKeyPairGeneratorClass())) {
        return HCF_INVALID_PARAMS;
    }
    bool pubKeyValid = IsBlobValid(pubKeyBlob);
    bool priKeyValid = IsBlobValid(priKeyBlob);
    if ((!pubKeyValid) && (!priKeyValid)) {
        LOGE("The private key and public key cannot both be NULL.");
        return HCF_INVALID_PARAMS;
    }

    HcfAsyKeyGeneratorSpiOpensslEccImpl *impl = (HcfAsyKeyGeneratorSpiOpensslEccImpl *)self;
    int32_t res = HCF_SUCCESS;
    HcfOpensslEccPubKey *pubKey = NULL;
    HcfOpensslEccPriKey *priKey = NULL;
    HcfOpensslEccKeyPair *keyPair = NULL;
    do {
        if (pubKeyValid) {
            res = ConvertEcPubKeyByOpenssl(impl->curveId, pubKeyBlob, &pubKey);
            if (res != HCF_SUCCESS) {
                break;
            }
        }
        if (priKeyValid) {
            res = ConvertEcPriKeyByOpenssl(impl->curveId, priKeyBlob, &priKey);
            if (res != HCF_SUCCESS) {
                break;
            }
        }
        res = CreateEccKeyPair(pubKey, priKey, &keyPair);
    } while (0);
    if (res != HCF_SUCCESS) {
        HcfObjDestroy(pubKey);
        HcfObjDestroy(priKey);
        return res;
    }

    *returnKeyPair = (HcfKeyPair *)keyPair;
    LOGI("end ...");
    return HCF_SUCCESS;
}

static HcfResult EngineGenerateKeyPair(HcfAsyKeyGeneratorSpi *self, HcfKeyPair **returnObj)
{
    LOGI("start ...");
    if ((self == NULL) || (returnObj == NULL)) {
        LOGE("Invalid input parameter.");
        return HCF_INVALID_PARAMS;
    }
    if (!IsClassMatch((HcfObjectBase *)self, GetEccKeyPairGeneratorClass())) {
        return HCF_INVALID_PARAMS;
    }

    HcfAsyKeyGeneratorSpiOpensslEccImpl *impl = (HcfAsyKeyGeneratorSpiOpensslEccImpl *)self;
    EC_POINT *ecPubKey = NULL;
    BIGNUM *ecPriKey = NULL;
    int32_t res = NewEcKeyPairByOpenssl(impl->curveId, &ecPubKey, &ecPriKey);
    if (res != HCF_SUCCESS) {
        return res;
    }
    HcfOpensslEccPubKey *pubKey = NULL;
    res = CreateEccPubKey(impl->curveId, ecPubKey, &pubKey);
    if (res != HCF_SUCCESS) {
        EC_POINT_free(ecPubKey);
        BN_clear_free(ecPriKey);
        return res;
    }
    HcfOpensslEccPriKey *priKey = NULL;
    res = CreateEccPriKey(impl->curveId, ecPriKey, &priKey);
    if (res != HCF_SUCCESS) {
        HcfObjDestroy(pubKey);
        BN_clear_free(ecPriKey);
        return res;
    }
    HcfOpensslEccKeyPair *returnKeyPair = (HcfOpensslEccKeyPair *)HcfMalloc(sizeof(HcfOpensslEccKeyPair), 0);
    if (returnKeyPair == NULL) {
        LOGE("Failed to allocate returnKeyPair memory!");
        HcfObjDestroy(pubKey);
        HcfObjDestroy(priKey);
        return HCF_ERR_MALLOC;
    }
    returnKeyPair->base.base.getClass = GetEccKeyPairClass;
    returnKeyPair->base.base.destroy = DestroyEccKeyPair;
    returnKeyPair->base.pubKey = (HcfPubKey *)pubKey;
    returnKeyPair->base.priKey = (HcfPriKey *)priKey;

    *returnObj = (HcfKeyPair *)returnKeyPair;
    LOGI("end ...");
    return HCF_SUCCESS;
}

HcfResult HcfAsyKeyGeneratorSpiEccCreate(HcfAsyKeyGenParams *params, HcfAsyKeyGeneratorSpi **returnObj)
{
    if (params == NULL || returnObj == NULL) {
        LOGE("Invalid input parameter.");
        return HCF_INVALID_PARAMS;
    }
    int32_t curveId;
    if (GetOpensslCurveId(params->bits, &curveId) != HCF_SUCCESS) {
        return HCF_INVALID_PARAMS;
    }
    HcfAsyKeyGeneratorSpiOpensslEccImpl *returnImpl = (HcfAsyKeyGeneratorSpiOpensslEccImpl *)HcfMalloc(
        sizeof(HcfAsyKeyGeneratorSpiOpensslEccImpl), 0);
    if (returnImpl == NULL) {
        LOGE("Failed to allocate returnImpl memroy!");
        return HCF_ERR_MALLOC;
    }
    returnImpl->base.base.getClass = GetEccKeyPairGeneratorClass;
    returnImpl->base.base.destroy = DestroyEccKeyPairGenerator;
    returnImpl->base.engineConvertKey = EngineConvertEccKey;
    returnImpl->base.engineGenerateKeyPair = EngineGenerateKeyPair;
    returnImpl->curveId = curveId;

    *returnObj = (HcfAsyKeyGeneratorSpi *)returnImpl;
    return HCF_SUCCESS;
}
