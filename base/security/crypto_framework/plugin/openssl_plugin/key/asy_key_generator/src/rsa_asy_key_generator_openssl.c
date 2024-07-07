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

#include "rsa_asy_key_generator_openssl.h"
#include "openssl/pem.h"
#include "openssl/x509.h"
#include "algorithm_parameter.h"
#include "asy_key_generator_spi.h"
#include "log.h"
#include "memory.h"
#include "openssl_class.h"
#include "openssl_common.h"
#include "rsa_openssl_common.h"
#include "securec.h"
#include "string.h"
#include "utils.h"

#define OPENSSL_BITS_PER_BYTE 8
#define OPENSSL_RSA_KEYPAIR_CNT 3
#define OPENSSL_RSA_KEYGEN_DEFAULT_PRIMES 2
#define MAX_KEY_SIZE 8192
#define MIN_KEY_SIZE 512

enum OpensslRsaKeySize {
    OPENSSL_RSA_KEY_SIZE_512 = 512,
    OPENSSL_RSA_KEY_SIZE_768 = 768,
    OPENSSL_RSA_KEY_SIZE_1024 = 1024,
    OPENSSL_RSA_KEY_SIZE_2048 = 2048,
    OPENSSL_RSA_KEY_SIZE_3072 = 3072,
    OPENSSL_RSA_KEY_SIZE_4096 = 4096,
    OPENSSL_RSA_KEY_SIZE_8192 = 8192,
};

enum OpensslRsaPrimesSize {
    OPENSSL_RSA_PRIMES_SIZE_2 = 2,
    OPENSSL_RSA_PRIMES_SIZE_3 = 3,
    OPENSSL_RSA_PRIMES_SIZE_4 = 4,
    OPENSSL_RSA_PRIMES_SIZE_5 = 5,
};

typedef struct {
    int32_t bits;
    int32_t primes;
    BIGNUM *pubExp;
} HcfAsyKeyGenSpiRsaParams;

typedef struct {
    HcfAsyKeyGeneratorSpi base;

    HcfAsyKeyGenSpiRsaParams *params;
} HcfAsyKeyGeneratorSpiRsaOpensslImpl;

static HcfResult CheckRsaKeyGenParams(HcfAsyKeyGenSpiRsaParams *params)
{
    switch (params->bits) {
        case OPENSSL_RSA_KEY_SIZE_512:
        case OPENSSL_RSA_KEY_SIZE_768:
            if (params->primes != OPENSSL_RSA_PRIMES_SIZE_2) {
                LOGE("Set invalid primes %d to Keygen bits %d.", params->primes, params->bits);
                return HCF_INVALID_PARAMS;
            }
            break;
        case OPENSSL_RSA_KEY_SIZE_1024:
        case OPENSSL_RSA_KEY_SIZE_2048:
        case OPENSSL_RSA_KEY_SIZE_3072:
            if (params->primes > OPENSSL_RSA_PRIMES_SIZE_3 || params->primes < OPENSSL_RSA_PRIMES_SIZE_2) {
                LOGE("Set invalid primes %d to Keygen bits %d.", params->primes, params->bits);
                return HCF_INVALID_PARAMS;
            }
            break;
        case OPENSSL_RSA_KEY_SIZE_4096:
            if (params->primes > OPENSSL_RSA_PRIMES_SIZE_4 || params->primes < OPENSSL_RSA_PRIMES_SIZE_2) {
                LOGE("Set invalid primes %d to Keygen bits %d.", params->primes, params->bits);
                return HCF_INVALID_PARAMS;
            }
            break;
        case OPENSSL_RSA_KEY_SIZE_8192: // This keySize can use primes from 2 to 5.
            break;
        default:
            LOGE("The current bits %d is invalid.", params->bits);
            return HCF_INVALID_PARAMS;
    }
    return HCF_SUCCESS;
}

static const char *GetOpensslPubkeyClass(void)
{
    return OPENSSL_RSA_PUBKEY_CLASS;
}

static const char *GetOpensslPrikeyClass(void)
{
    return OPENSSL_RSA_PRIKEY_CLASS;
}

static const char *GetOpensslKeyPairClass(void)
{
    return OPENSSL_RSA_KEYPAIR_CLASS;
}

static void DestroyPubKey(HcfObjectBase *self)
{
    if (self == NULL) {
        LOGE("PubKey is NULL.");
        return;
    }
    if (!IsClassMatch(self, OPENSSL_RSA_PUBKEY_CLASS)) {
        LOGE("Class not match");
        return;
    }
    HcfOpensslRsaPubKey *impl = (HcfOpensslRsaPubKey *)self;
    RSA_free(impl->pk);
    impl->pk = NULL;
    HcfFree(self);
}

static void DestroyPriKey(HcfObjectBase *self)
{
    if (self == NULL) {
        LOGE("PubKey is NULL.");
        return;
    }
    if (!IsClassMatch(self, OPENSSL_RSA_PRIKEY_CLASS)) {
        LOGE("Class not match");
        return;
    }
    HcfOpensslRsaPriKey *impl = (HcfOpensslRsaPriKey*)self;
    RSA_free(impl->sk);
    impl->sk = NULL;
    HcfFree(self);
}

static void DestroyKey(HcfObjectBase *self)
{
    LOGI("process DestroyKey");
}

static void DestroyKeyPair(HcfObjectBase *self)
{
    if (self == NULL) {
        LOGE("PubKey is NULL.");
        return;
    }
    if (!IsClassMatch(self, OPENSSL_RSA_KEYPAIR_CLASS)) {
        LOGE("Class not match");
        return;
    }
    HcfOpensslRsaKeyPair *impl = (HcfOpensslRsaKeyPair*)self;
    DestroyPriKey((HcfObjectBase *)impl->base.priKey);
    impl->base.priKey = NULL;
    DestroyPubKey((HcfObjectBase *)impl->base.pubKey);
    impl->base.pubKey = NULL;
    HcfFree(self);
}

static HcfResult CopyMemFromBIO(BIO *bio, HcfBlob *outBlob)
{
    if (bio == NULL || outBlob == NULL) {
        LOGE("Invalid input.");
        return HCF_INVALID_PARAMS;
    }
    int len = BIO_pending(bio);
    if (len < 0) {
        LOGE("Bio len less than 0.");
        return HCF_INVALID_PARAMS;
    }
    HcfBlob blob;
    blob.len = len;
    blob.data = (uint8_t *)HcfMalloc(sizeof(uint8_t) * len, 0);
    if (blob.data == NULL) {
        LOGE("Malloc mem for blob fail.");
        return HCF_ERR_MALLOC;
    }
    if (BIO_read(bio, blob.data, blob.len) <= 0) {
        LOGE("Bio read fail");
        HcfPrintOpensslError();
        HcfFree(blob.data);
        return HCF_ERR_CRYPTO_OPERATION;
    }
    outBlob->len = blob.len;
    outBlob->data = blob.data;
    return HCF_SUCCESS;
}

static HcfResult ConvertPubKeyFromX509(HcfBlob *x509Blob, RSA **rsa)
{
    uint8_t *temp = x509Blob->data;
    RSA *tempRsa = d2i_RSA_PUBKEY(NULL, (const unsigned char **)&temp, x509Blob->len);
    if (tempRsa == NULL) {
        LOGE("d2i_RSA_PUBKEY fail.");
        return HCF_ERR_CRYPTO_OPERATION;
    }
    *rsa = tempRsa;
    return HCF_SUCCESS;
}

static HcfResult ConvertPriKeyFromPKCS8(HcfBlob *pkcs8Blob, RSA **rsa)
{
    uint8_t *temp = pkcs8Blob->data;
    EVP_PKEY *pKey = d2i_AutoPrivateKey(NULL, (const unsigned char **)&temp, pkcs8Blob->len);
    if (pKey == NULL) {
        LOGE("d2i_AutoPrivateKey fail.");
        HcfPrintOpensslError();
        return HCF_ERR_CRYPTO_OPERATION;
    }
    RSA *tmpRsa = EVP_PKEY_get1_RSA(pKey);
    if (tmpRsa == NULL) {
        LOGE("EVP_PKEY_get0_RSA fail");
        HcfPrintOpensslError();
        EVP_PKEY_free(pKey);
        return HCF_ERR_CRYPTO_OPERATION;
    }
    *rsa = tmpRsa;
    EVP_PKEY_free(pKey);
    return HCF_SUCCESS;
}

static HcfResult EncodePubKeyToX509(RSA *rsa, HcfBlob *returnBlob)
{
    unsigned char *tempData = NULL;
    int len = i2d_RSA_PUBKEY(rsa, &tempData);
    if (len <= 0) {
        LOGE("i2d_RSA_PUBKEY fail");
        HcfPrintOpensslError();
        return HCF_ERR_CRYPTO_OPERATION;
    }
    returnBlob->data = tempData;
    returnBlob->len = len;
    return HCF_SUCCESS;
}

static HcfResult EncodePriKeyToPKCS8(RSA *rsa, HcfBlob *returnBlob)
{
    EVP_PKEY *pKey = NewEvpPkeyByRsa(rsa, true);
    if (pKey == NULL) {
        LOGE("NewEvpPkeyByRsa fail.");
        return HCF_ERR_CRYPTO_OPERATION;
    }
    HcfResult ret = HCF_SUCCESS;
    BIO *bio = BIO_new(BIO_s_mem());
    if (bio == NULL) {
        LOGE("BIO new fail.");
        HcfPrintOpensslError();
        ret = HCF_ERR_CRYPTO_OPERATION;
        goto ERR2;
    }
    if (i2d_PKCS8PrivateKey_bio(bio, pKey, NULL, NULL, 0, NULL, NULL) != HCF_OPENSSL_SUCCESS) {
        LOGE("i2b_PrivateKey_bio fail.");
        HcfPrintOpensslError();
        ret = HCF_ERR_CRYPTO_OPERATION;
        goto ERR1;
    }
    if (CopyMemFromBIO(bio, returnBlob) != HCF_SUCCESS) {
        LOGE("CopyMemFromBIO fail.");
        ret = HCF_ERR_CRYPTO_OPERATION;
        goto ERR1;
    }
ERR1:
    BIO_free_all(bio);
ERR2:
    EVP_PKEY_free(pKey);
    return ret;
}

static HcfResult GetPubKeyEncoded(HcfKey *self, HcfBlob *returnBlob)
{
    if (self == NULL || returnBlob == NULL) {
        LOGE("Input params is invalid.");
        return HCF_INVALID_PARAMS;
    }
    if (!IsClassMatch((HcfObjectBase *)self, OPENSSL_RSA_PUBKEY_CLASS)) {
        LOGE("Class not match.");
        return HCF_INVALID_PARAMS;
    }
    HcfOpensslRsaPubKey *impl = (HcfOpensslRsaPubKey *)self;
    return EncodePubKeyToX509(impl->pk, returnBlob);
}

static HcfResult GetPriKeyEncoded(HcfKey *self, HcfBlob *returnBlob)
{
    if (self == NULL || returnBlob == NULL) {
        LOGE("Key is null.");
        return HCF_INVALID_PARAMS;
    }

    if (!IsClassMatch((HcfObjectBase *)self, OPENSSL_RSA_PRIKEY_CLASS)) {
        LOGE("Class not match.");
        return HCF_INVALID_PARAMS;
    }
    HcfOpensslRsaPriKey *impl = (HcfOpensslRsaPriKey *)self;
    return EncodePriKeyToPKCS8(impl->sk, returnBlob);
}

static const char *GetPubKeyFormat(HcfKey *self)
{
    if (self == NULL) {
        LOGE("Invalid input parameter.");
        return NULL;
    }
    if (!IsClassMatch((HcfObjectBase *)self, OPENSSL_RSA_PUBKEY_CLASS)) {
        return NULL;
    }
    return OPENSSL_RSA_PUBKEY_FORMAT;
}

static const char *GetPriKeyFormat(HcfKey *self)
{
    if (self == NULL) {
        LOGE("Invalid input parameter.");
        return NULL;
    }
    if (!IsClassMatch((HcfObjectBase *)self, OPENSSL_RSA_PRIKEY_CLASS)) {
        return NULL;
    }
    return OPENSSL_RSA_PRIKEY_FORMAT;
}

static const char *GetPriKeyAlgorithm(HcfKey *self)
{
    if (self == NULL) {
        LOGE("Invalid input parameter.");
        return NULL;
    }
    if (!IsClassMatch((HcfObjectBase *)self, OPENSSL_RSA_PRIKEY_CLASS)) {
        return NULL;
    }
    return OPENSSL_RSA_ALGORITHM;
}

static const char *GetPubKeyAlgorithm(HcfKey *self)
{
    if (self == NULL) {
        LOGE("Invalid input parameter.");
        return NULL;
    }
    if (!IsClassMatch((HcfObjectBase *)self, OPENSSL_RSA_PUBKEY_CLASS)) {
        return NULL;
    }
    return OPENSSL_RSA_ALGORITHM;
}

static void ClearPriKeyMem(HcfPriKey *self)
{
    if (self == NULL) {
        LOGE("PriKey is NULL.");
        return;
    }
    if (!IsClassMatch((HcfObjectBase *)self, OPENSSL_RSA_PRIKEY_CLASS)) {
        LOGE("Class not match");
        return;
    }
    RSA_free(((HcfOpensslRsaPriKey *)self)->sk);
    ((HcfOpensslRsaPriKey *)self)->sk = NULL;
}

static HcfResult PackPubKey(RSA *rsaPubKey, HcfOpensslRsaPubKey **retPubKey)
{
    if (retPubKey == NULL || rsaPubKey == NULL) {
        LOGE("Invalid params");
        return HCF_INVALID_PARAMS;
    }
    *retPubKey = (HcfOpensslRsaPubKey *)HcfMalloc(sizeof(HcfOpensslRsaPubKey), 0);
    if (*retPubKey == NULL) {
        LOGE("Malloc retPubKey fail");
        return HCF_ERR_MALLOC;
    }
    (*retPubKey)->pk = rsaPubKey;
    (*retPubKey)->bits = RSA_bits(rsaPubKey);
    (*retPubKey)->base.base.getAlgorithm = GetPubKeyAlgorithm;
    (*retPubKey)->base.base.getEncoded = GetPubKeyEncoded;
    (*retPubKey)->base.base.getFormat = GetPubKeyFormat;
    (*retPubKey)->base.base.base.getClass = GetOpensslPubkeyClass;
    (*retPubKey)->base.base.base.destroy = DestroyKey;
    return HCF_SUCCESS;
}

static HcfResult PackPriKey(RSA *rsaPriKey, HcfOpensslRsaPriKey **retPriKey)
{
    if (retPriKey == NULL || rsaPriKey == NULL) {
        LOGE("Invalid params");
        return HCF_INVALID_PARAMS;
    }
    *retPriKey = (HcfOpensslRsaPriKey *)HcfMalloc(sizeof(HcfOpensslRsaPriKey), 0);
    if (*retPriKey == NULL) {
        LOGE("Malloc retPriKey fail");
        return HCF_ERR_MALLOC;
    }
    (*retPriKey)->sk = rsaPriKey;
    (*retPriKey)->bits = RSA_bits(rsaPriKey);
    (*retPriKey)->base.clearMem = ClearPriKeyMem;
    (*retPriKey)->base.base.getAlgorithm = GetPriKeyAlgorithm;
    (*retPriKey)->base.base.getEncoded = GetPriKeyEncoded;
    (*retPriKey)->base.base.getFormat = GetPriKeyFormat;
    (*retPriKey)->base.base.base.getClass = GetOpensslPrikeyClass;
    (*retPriKey)->base.base.base.destroy = DestroyKey;
    return HCF_SUCCESS;
}

static HcfResult DuplicatePkAndSkFromRSA(RSA *rsa, RSA **pubKey, RSA **priKey)
{
    if (rsa == NULL) {
        LOGE("Rsa is NULL.");
        return HCF_INVALID_PARAMS;
    }
    if (DuplicateRsa(rsa, false, pubKey) != HCF_SUCCESS) {
        LOGE("Duplicate pubkey rsa fail");
        return HCF_ERR_CRYPTO_OPERATION;
    }
    if (DuplicateRsa(rsa, true, priKey) != HCF_SUCCESS) {
        LOGE("Duplicate prikey rsa fail");
        RSA_free(*pubKey);
        *pubKey = NULL;
        return HCF_ERR_CRYPTO_OPERATION;
    }
    return HCF_SUCCESS;
}

static HcfResult PackKeyPair(RSA *rsa, uint32_t realBits, HcfOpensslRsaKeyPair **retKeyPair)
{
    if (retKeyPair == NULL || rsa == NULL) {
        LOGE("Invalid params");
        return HCF_INVALID_PARAMS;
    }
    RSA *pubKey = NULL, *priKey = NULL;
    if (DuplicatePkAndSkFromRSA(rsa, &pubKey, &priKey) != HCF_SUCCESS) {
        LOGE("DuplicatePkAndSkFromRSA fail");
        return HCF_ERR_CRYPTO_OPERATION;
    }
    HcfResult ret = HCF_SUCCESS;
    *retKeyPair = (HcfOpensslRsaKeyPair *)HcfMalloc(sizeof(HcfOpensslRsaKeyPair), 0);
    if (*retKeyPair == NULL) {
        LOGE("Malloc keypair fail");
        RSA_free(pubKey);
        RSA_free(priKey);
        return HCF_ERR_MALLOC;
    }
    HcfOpensslRsaPriKey *priKeyImpl = NULL;
    HcfOpensslRsaPubKey *pubKeyImpl = NULL;
    ret = PackPubKey(pubKey, &pubKeyImpl);
    if (ret != HCF_SUCCESS) {
        LOGE("Pack pubKey fail.");
        goto ERR2;
    }
    ret = PackPriKey(priKey, &priKeyImpl);
    if (ret != HCF_SUCCESS) {
        LOGE("Pack priKey fail.");
        goto ERR1;
    }
    (*retKeyPair)->base.priKey = (HcfPriKey *)priKeyImpl;
    (*retKeyPair)->base.pubKey = (HcfPubKey *)pubKeyImpl;
    (*retKeyPair)->base.base.getClass = GetOpensslKeyPairClass;
    (*retKeyPair)->base.base.destroy = DestroyKeyPair;
    return HCF_SUCCESS;
ERR1:
    HcfFree(pubKeyImpl);
ERR2:
    RSA_free(pubKey);
    RSA_free(priKey);
    HcfFree(*retKeyPair);
    *retKeyPair = NULL;
    return ret;
}

static HcfResult GenerateKeyPairByOpenssl(HcfAsyKeyGenSpiRsaParams *params, HcfKeyPair **keyPair)
{
    // check input params is valid
    HcfResult  res = CheckRsaKeyGenParams(params);
    if (res != HCF_SUCCESS) {
        LOGE("Rsa CheckRsaKeyGenParams fail.");
        return HCF_INVALID_PARAMS;
    }
    // Generate keyPair RSA
    RSA *rsa = RSA_new();
    if (rsa == NULL) {
        LOGE("new RSA fail.");
        return HCF_ERR_MALLOC;
    }
    LOGI("keygen bits is %d, primes is %d", params->bits, GetRealPrimes(params->primes));
    if (GetRealPrimes(params->primes) != OPENSSL_RSA_KEYGEN_DEFAULT_PRIMES) {
        if (RSA_generate_multi_prime_key(rsa, params->bits, GetRealPrimes(params->primes), params->pubExp, NULL)
            != HCF_OPENSSL_SUCCESS) {
            LOGE("Generate multi-primes rsa key fail");
            HcfPrintOpensslError();
            RSA_free(rsa);
            return HCF_ERR_CRYPTO_OPERATION;
        }
    } else {
        if (RSA_generate_key_ex(rsa, params->bits, params->pubExp, NULL) != HCF_OPENSSL_SUCCESS) {
            LOGE("Generate rsa key fail");
            HcfPrintOpensslError();
            RSA_free(rsa);
            return HCF_ERR_CRYPTO_OPERATION;
        }
    }

    // devided to pk and sk;
    HcfOpensslRsaKeyPair *keyPairImpl = NULL;
    res = PackKeyPair(rsa, params->bits, &keyPairImpl);
    if (res != HCF_SUCCESS) {
        LOGE("Generate keyPair fail.");
        RSA_free(rsa);
        return res;
    }
    *keyPair = (HcfKeyPair *)keyPairImpl;
    RSA_free(rsa);
    LOGI("Generate keypair success.");
    return res;
}

static HcfResult EngineGenerateKeyPair(HcfAsyKeyGeneratorSpi *self, HcfKeyPair **keyPair)
{
    LOGI("EngineGenerateKeyPair start");
    if (self == NULL || keyPair == NULL) {
        LOGE("Invalid params.");
        return HCF_INVALID_PARAMS;
    }
    if (!IsClassMatch((HcfObjectBase *)self, OPENSSL_RSA_GENERATOR_CLASS)) {
        LOGE("Class not match.");
        return HCF_INVALID_PARAMS;
    }
    HcfAsyKeyGeneratorSpiRsaOpensslImpl *impl = (HcfAsyKeyGeneratorSpiRsaOpensslImpl *)self;
    return GenerateKeyPairByOpenssl(impl->params, keyPair);
}

static const char *GetKeyGeneratorClass(void)
{
    return OPENSSL_RSA_GENERATOR_CLASS;
}

static void DestroyKeyGeneratorSpiImpl(HcfObjectBase *self)
{
    LOGI("DestroyKeyGeneratorSpiImpl start.");
    if (self == NULL) {
        LOGE("DestroyKeyGeneratorSpiImpl is null");
        return;
    }
    if (!IsClassMatch(self, OPENSSL_RSA_GENERATOR_CLASS)) {
        LOGE("Class not match.");
        return;
    }
    // destroy pubExp first.
    HcfAsyKeyGeneratorSpiRsaOpensslImpl *impl = (HcfAsyKeyGeneratorSpiRsaOpensslImpl *)self;
    if (impl->params != NULL && impl->params->pubExp != NULL) {
        BN_free(impl->params->pubExp);
    }
    HcfFree(impl->params);
    impl->params = NULL;
    HcfFree(self);
    LOGI("DestroyKeyGeneratorSpiImpl end.");
}

static HcfResult ConvertPubKey(HcfBlob *pubKeyBlob, HcfOpensslRsaPubKey **pubkeyRet)
{
    RSA *rsaPk = NULL;
    if (ConvertPubKeyFromX509(pubKeyBlob, &rsaPk) != HCF_SUCCESS) {
        LOGE("Convert pubKey from X509 fail.");
        return HCF_ERR_CRYPTO_OPERATION;
    }
    HcfOpensslRsaPubKey *pubKey = NULL;
    HcfResult ret = PackPubKey(rsaPk, &pubKey);
    if (ret != HCF_SUCCESS) {
        LOGE("PackPubKey fail");
        goto ERR;
    }
    *pubkeyRet = pubKey;
    return ret;
ERR:
    RSA_free(rsaPk);
    return ret;
}

static HcfResult ConvertPriKey(HcfBlob *priKeyBlob, HcfOpensslRsaPriKey **priKeyRet)
{
    RSA *rsaSk = NULL;
    if (ConvertPriKeyFromPKCS8(priKeyBlob, &rsaSk) != HCF_SUCCESS) {
        LOGE("ConvertPriKeyFromPKCS8 fail.");
        return HCF_ERR_MALLOC;
    }
    HcfOpensslRsaPriKey *priKey = NULL;
    HcfResult ret = PackPriKey(rsaSk, &priKey);
    if (ret != HCF_SUCCESS) {
        LOGE("PackPriKey fail");
        goto ERR;
    }
    *priKeyRet = priKey;
    return ret;
ERR:
    RSA_free(rsaSk);
    return ret;
}

static HcfResult EngineConvertKey(HcfAsyKeyGeneratorSpi *self, HcfParamsSpec *params, HcfBlob *pubKeyBlob,
    HcfBlob *priKeyBlob, HcfKeyPair **returnKeyPair)
{
    LOGI("EngineConvertKey start");
    (void)params;
    if ((self == NULL) || (returnKeyPair == NULL) || ((pubKeyBlob == NULL) && (priKeyBlob == NULL))) {
        LOGE("ConvertKeyParams is invalid.");
        return HCF_INVALID_PARAMS;
    }
    if (!IsClassMatch((HcfObjectBase *)self, OPENSSL_RSA_GENERATOR_CLASS)) {
        LOGE("Class not match.");
        return HCF_INVALID_PARAMS;
    }

    HcfOpensslRsaPubKey *pubKey = NULL;
    if ((pubKeyBlob != NULL) && (pubKeyBlob->data != NULL)) {
        if (ConvertPubKey(pubKeyBlob, &pubKey) != HCF_SUCCESS) {
            LOGE("convert pubkey fail.");
            return HCF_INVALID_PARAMS;
        }
    }

    HcfOpensslRsaPriKey *priKey = NULL;
    if (priKeyBlob != NULL && priKeyBlob->data != NULL) {
        if (ConvertPriKey(priKeyBlob, &priKey) != HCF_SUCCESS) {
            LOGE("convert prikey fail.");
            HcfObjDestroy((HcfObjectBase *)pubKey);
            return HCF_INVALID_PARAMS;
        }
    }

    HcfOpensslRsaKeyPair *keyPair = (HcfOpensslRsaKeyPair *)HcfMalloc(sizeof(HcfOpensslRsaKeyPair), 0);
    if (keyPair == NULL) {
        LOGE("Malloc keyPair fail.");
        HcfObjDestroy((HcfObjectBase *)pubKey);
        HcfObjDestroy((HcfObjectBase *)priKey);
        return HCF_ERR_MALLOC;
    }

    keyPair->base.priKey = (HcfPriKey *)priKey;
    keyPair->base.pubKey = (HcfPubKey *)pubKey;
    keyPair->base.base.getClass = GetOpensslKeyPairClass;
    keyPair->base.base.destroy = DestroyKeyPair;
    *returnKeyPair = (HcfKeyPair *)keyPair;
    LOGI("EngineConvertKey end");
    return HCF_SUCCESS;
}

static HcfResult SetDefaultValue(HcfAsyKeyGenSpiRsaParams *params)
{
    if (params->primes == 0) {
        LOGI("set default primes 2");
        params->primes = OPENSSL_RSA_PRIMES_SIZE_2;
    }
    if (params->pubExp == NULL) {
        BIGNUM *e = BN_new();
        if (e == NULL) {
            LOGE("Rsa new BN fail.");
            return HCF_ERR_CRYPTO_OPERATION;
        }
        if (BN_set_word(e, RSA_F4) != HCF_OPENSSL_SUCCESS) {
            LOGE("Rsa keygen Bn_set_word fail.");
            BN_free(e);
            return HCF_ERR_CRYPTO_OPERATION;
        }
        params->pubExp = e;
    }
    return HCF_SUCCESS;
}

static HcfResult DecodeParams(HcfAsyKeyGenParams *from, HcfAsyKeyGenSpiRsaParams **to)
{
    *to = (HcfAsyKeyGenSpiRsaParams *)HcfMalloc(sizeof(HcfAsyKeyGenSpiRsaParams), 0);
    if (*to == NULL) {
        LOGE("Malloc HcfAsyKeyGenSpiRsaParams fail");
        return HCF_ERR_MALLOC;
    }

    (*to)->bits = from->bits;
    (*to)->primes = from->primes;

    // set 2 as default primes, RSA_F4 as default pubExp
    if (SetDefaultValue(*to) != HCF_SUCCESS) {
        LOGE("Set default value fail.");
        HcfFree(*to);
        *to = NULL;
        return HCF_INVALID_PARAMS;
    }
    if (CheckRsaKeyGenParams(*to) != HCF_SUCCESS) {
        LOGE("Invalid keyGen params");
        HcfFree(*to);
        *to = NULL;
        return HCF_INVALID_PARAMS;
    }
    return HCF_SUCCESS;
}

HcfResult HcfAsyKeyGeneratorSpiRsaCreate(HcfAsyKeyGenParams *params, HcfAsyKeyGeneratorSpi **generator)
{
    LOGI("HcfAsyKeyGeneratorSpiRsaCreate start.");
    if (params == NULL || generator == NULL) {
        LOGE("Invalid input, params is invalid or generator is null.");
        return HCF_INVALID_PARAMS;
    }
    HcfAsyKeyGeneratorSpiRsaOpensslImpl *impl = (HcfAsyKeyGeneratorSpiRsaOpensslImpl *)
        HcfMalloc(sizeof(HcfAsyKeyGeneratorSpiRsaOpensslImpl), 0);
    if (impl == NULL) {
        LOGE("Failed to allocate returnImpl memroy!");
        return HCF_ERR_MALLOC;
    }
    if (DecodeParams(params, &impl->params) != HCF_SUCCESS) {
        LOGE("Keygen params is invalid.");
        HcfFree(impl);
        return HCF_INVALID_PARAMS;
    }
    impl->base.base.getClass = GetKeyGeneratorClass;
    impl->base.base.destroy = DestroyKeyGeneratorSpiImpl;
    impl->base.engineGenerateKeyPair = EngineGenerateKeyPair;
    impl->base.engineConvertKey = EngineConvertKey;
    *generator = (HcfAsyKeyGeneratorSpi *)impl;
    LOGI("HcfAsyKeyGeneratorSpiRsaCreate end.");
    return HCF_SUCCESS;
}
