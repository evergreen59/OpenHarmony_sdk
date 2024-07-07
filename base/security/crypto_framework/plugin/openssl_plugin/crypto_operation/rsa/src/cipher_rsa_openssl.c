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

#include "cipher_rsa_openssl.h"
#include "securec.h"
#include "openssl/rsa.h"
#include "rsa_openssl_common.h"
#include "log.h"
#include "memory.h"
#include "openssl_class.h"
#include "openssl_common.h"
#include "stdbool.h"
#include "string.h"
#include "utils.h"

static const char *EngineGetClass(void);

typedef struct {
    HcfCipherGeneratorSpi super;

    CipherAttr attr;

    InitFlag initFlag;

    EVP_PKEY_CTX *ctx;
} HcfCipherRsaGeneratorSpiImpl;

static HcfResult CheckCipherInitParams(enum HcfCryptoMode opMode, HcfKey *key)
{
    switch (opMode) {
        case ENCRYPT_MODE:
            if (!IsClassMatch((HcfObjectBase *)key, OPENSSL_RSA_PUBKEY_CLASS)) {
                LOGE("Class not match");
                return HCF_INVALID_PARAMS;
            }
            break;
        case DECRYPT_MODE:
            if (!IsClassMatch((HcfObjectBase *)key, OPENSSL_RSA_PRIKEY_CLASS)) {
                LOGE("Class not match");
                return HCF_INVALID_PARAMS;
            }
            break;
        default:
            LOGE("Invalid opMode %u", opMode);
            return HCF_INVALID_PARAMS;
    }

    return HCF_SUCCESS;
}

static HcfResult DuplicateRsaFromKey(HcfKey *key, enum HcfCryptoMode opMode, RSA **dupRsa)
{
    HcfResult ret = HCF_SUCCESS;
    if (opMode == ENCRYPT_MODE) {
        ret = DuplicateRsa(((HcfOpensslRsaPubKey *)key)->pk, false, dupRsa);
        if (ret != HCF_SUCCESS) {
            LOGE("dup pub rsa fail.");
            return ret;
        }
    } else if (opMode == DECRYPT_MODE) {
        ret = DuplicateRsa(((HcfOpensslRsaPriKey *)key)->sk, true, dupRsa);
        if (ret != HCF_SUCCESS) {
            LOGE("dup pri rsa fail.");
            return ret;
        }
    } else {
        LOGE("OpMode not match.");
        return HCF_INVALID_PARAMS;
    }
    return ret;
}

static HcfResult InitEvpPkeyCtx(HcfCipherRsaGeneratorSpiImpl *impl, HcfKey *key, enum HcfCryptoMode opMode)
{
    RSA *rsa = NULL;
    HcfResult ret = HCF_SUCCESS;
    ret = DuplicateRsaFromKey(key, opMode, &rsa);
    if (ret != HCF_SUCCESS) {
        LOGE("DuplicateRsaFromKey fail.");
        return ret;
    }
    EVP_PKEY *pkey = NewEvpPkeyByRsa(rsa, false);
    if (pkey == NULL) {
        LOGE("NewEvpPkeyByRsa fail");
        HcfPrintOpensslError();
        RSA_free(rsa);
        return HCF_ERR_CRYPTO_OPERATION;
    }
    impl->ctx = EVP_PKEY_CTX_new(pkey, NULL);
    if (impl->ctx == NULL) {
        LOGE("EVP_PKEY_CTX_new fail");
        HcfPrintOpensslError();
        EVP_PKEY_free(pkey);
        return HCF_ERR_CRYPTO_OPERATION;
    }
    int32_t sslRet = HCF_OPENSSL_SUCCESS;
    if (opMode == ENCRYPT_MODE) {
        sslRet = EVP_PKEY_encrypt_init(impl->ctx);
    } else {
        sslRet = EVP_PKEY_decrypt_init(impl->ctx);
    }
    if (sslRet != HCF_OPENSSL_SUCCESS) {
        LOGE("Init EVP_PKEY fail");
        HcfPrintOpensslError();
        EVP_PKEY_free(pkey);
        EVP_PKEY_CTX_free(impl->ctx);
        return HCF_ERR_CRYPTO_OPERATION;
    }
    EVP_PKEY_free(pkey);
    return HCF_SUCCESS;
}

static HcfResult SetDetailParams(HcfCipherRsaGeneratorSpiImpl *impl)
{
    CipherAttr attr = impl->attr;
    const EVP_MD *md = GetOpensslDigestAlg(attr.md);
    if (md == NULL && attr.paddingMode == HCF_OPENSSL_RSA_PKCS1_OAEP_PADDING) {
        LOGE("md is NULL.");
        return HCF_INVALID_PARAMS;
    }
    const EVP_MD *mgf1md = GetOpensslDigestAlg(attr.mgf1md);
    if (mgf1md == NULL && attr.paddingMode == HCF_OPENSSL_RSA_PKCS1_OAEP_PADDING) {
        LOGE("mgf1md is NULL.");
        return HCF_INVALID_PARAMS;
    }
    int32_t opensslPadding = 0;
    if (GetOpensslPadding(attr.paddingMode, &opensslPadding) != HCF_SUCCESS) {
        LOGE("Padding is dismatch.");
        return HCF_INVALID_PARAMS;
    }
    if (EVP_PKEY_CTX_set_rsa_padding(impl->ctx, opensslPadding) != HCF_OPENSSL_SUCCESS) {
        LOGE("Cipher set padding fail.");
        HcfPrintOpensslError();
        return HCF_ERR_CRYPTO_OPERATION;
    }
    if (attr.paddingMode == HCF_OPENSSL_RSA_PKCS1_OAEP_PADDING) {
        // set md and mgf1md
        if (EVP_PKEY_CTX_set_rsa_oaep_md(impl->ctx, md) != HCF_OPENSSL_SUCCESS
            || EVP_PKEY_CTX_set_rsa_mgf1_md(impl->ctx, mgf1md) != HCF_OPENSSL_SUCCESS) {
            LOGE("Set md or mgf1md fail");
            HcfPrintOpensslError();
            return HCF_ERR_CRYPTO_OPERATION;
        }
    }
    return HCF_SUCCESS;
}

static HcfResult EngineInit(HcfCipherGeneratorSpi *self, enum HcfCryptoMode opMode,
    HcfKey *key, HcfParamsSpec *params)
{
    LOGI("EngineInit start");
    if (!IsClassMatch((HcfObjectBase *)self, EngineGetClass())) {
        LOGE("Class not match");
        return HCF_INVALID_PARAMS;
    }
    HcfCipherRsaGeneratorSpiImpl *impl = (HcfCipherRsaGeneratorSpiImpl *)self;
    if (impl->initFlag != UNINITIALIZED) {
        LOGE("The cipher has been initialize, don't init again.");
        return HCF_INVALID_PARAMS;
    }

    // check opMode is matched with Key
    if (CheckCipherInitParams(opMode, key) != HCF_SUCCESS) {
        LOGE("OpMode dismatch with keyType.");
        return HCF_INVALID_PARAMS;
    }
    impl->attr.mode = (int32_t)opMode;
    if (InitEvpPkeyCtx(impl, key, opMode) != HCF_SUCCESS) {
        LOGE("InitEvpPkeyCtx fail");
        return HCF_ERR_CRYPTO_OPERATION;
    }

    if (SetDetailParams(impl) != HCF_SUCCESS) {
        EVP_PKEY_CTX_free(impl->ctx);
        LOGE("SetDetailParams fail.");
        return HCF_ERR_CRYPTO_OPERATION;
    }
    impl->initFlag = INITIALIZED;
    LOGI("EngineInit end");
    return HCF_SUCCESS;
}

static HcfResult EngineUpdata(HcfCipherGeneratorSpi *self, HcfBlob *input, HcfBlob *output)
{
    LOGE("Openssl don't support update");
    (void)self;
    (void)input;
    (void)output;
    return HCF_NOT_SUPPORT;
}

static HcfResult DoRsaCrypt(EVP_PKEY_CTX *ctx, HcfBlob *input, HcfBlob *output, int32_t mode)
{
    int32_t ret = HCF_OPENSSL_SUCCESS;
    if (mode == ENCRYPT_MODE) {
        ret = EVP_PKEY_encrypt(ctx, output->data, &output->len, input->data, input->len);
    } else if (mode == DECRYPT_MODE) {
        ret = EVP_PKEY_decrypt(ctx, output->data, &output->len, input->data, input->len);
    } else {
        LOGE("OpMode is invalid.");
        return HCF_INVALID_PARAMS;
    }
    if (ret != HCF_OPENSSL_SUCCESS) {
        HcfPrintOpensslError();
        return HCF_ERR_CRYPTO_OPERATION;
    }
    return HCF_SUCCESS;
}

static HcfResult EngineDoFinal(HcfCipherGeneratorSpi *self, HcfBlob *input, HcfBlob *output)
{
    LOGI("EngineDoFinal start");
    if (self == NULL || input == NULL || input->data == NULL) {
        LOGE("Param is invalid.");
        return HCF_INVALID_PARAMS;
    }
    if (!IsClassMatch((HcfObjectBase *)self, EngineGetClass())) {
        LOGE("Class not match");
        return HCF_INVALID_PARAMS;
    }
    HcfCipherRsaGeneratorSpiImpl *impl = (HcfCipherRsaGeneratorSpiImpl *)self;
    if (impl->initFlag != INITIALIZED) {
        LOGE("RSACipher has not been init");
        return HCF_INVALID_PARAMS;
    }
    CipherAttr attr = impl->attr;
    output->len = 0;
    output->data = NULL;
    HcfResult ret = DoRsaCrypt(impl->ctx, input, output, attr.mode);
    if (ret != HCF_SUCCESS) {
        LOGE("GetOutLen fail.");
        return HCF_ERR_CRYPTO_OPERATION;
    }
    LOGI("ouput data len is %zu.", output->len);

    output->data = (uint8_t *)HcfMalloc(sizeof(uint8_t) * output->len, 0);
    ret = DoRsaCrypt(impl->ctx, input, output, attr.mode);
    if (ret != HCF_SUCCESS) {
        HcfFree(output->data);
        output->data = NULL;
        output->len = 0;
        return HCF_ERR_CRYPTO_OPERATION;
    }
    LOGI("EngineDoFinal end");
    return HCF_SUCCESS;
}

static void EngineDestroySpiImpl(HcfObjectBase *generator)
{
    if (generator == NULL) {
        return;
    }
    if (!IsClassMatch((HcfObjectBase *)generator, EngineGetClass())) {
        LOGE("Class not match");
        return;
    }
    HcfCipherRsaGeneratorSpiImpl *impl = (HcfCipherRsaGeneratorSpiImpl *)generator;
    EVP_PKEY_CTX_free(impl->ctx);
    impl->ctx = NULL;
    HcfFree(impl);
}

static const char *EngineGetClass(void)
{
    return OPENSSL_RSA_CIPHER_CLASS;
}

static HcfResult CheckRsaCipherParams(CipherAttr *params)
{
    int32_t opensslPadding = 0;
    if (params->algo != HCF_ALG_RSA) {
        LOGE("Cipher algo %u is invalid.", params->algo);
        return HCF_INVALID_PARAMS;
    }
    if (GetOpensslPadding(params->paddingMode, &opensslPadding) != HCF_SUCCESS) {
        LOGE("Cipher create without padding");
        return HCF_INVALID_PARAMS;
    }
    if (params->paddingMode == HCF_ALG_NOPADDING && (GetOpensslDigestAlg(params->md) != NULL ||
        GetOpensslDigestAlg(params->mgf1md) != NULL)) {
        LOGE("Nopadding don't need md or mgf1md");
        return HCF_INVALID_PARAMS;
    }

    if (params->paddingMode == HCF_OPENSSL_RSA_PKCS1_OAEP_PADDING && GetOpensslDigestAlg(params->md) == NULL) {
        LOGE("md is NULL");
        return HCF_INVALID_PARAMS;
    }
    if (params->paddingMode == HCF_OPENSSL_RSA_PKCS1_OAEP_PADDING && GetOpensslDigestAlg(params->mgf1md) == NULL) {
        LOGE("Use pkcs1_oaep padding, but mgf1md is NULL");
        return HCF_INVALID_PARAMS;
    }
    return HCF_SUCCESS;
}

HcfResult HcfCipherRsaCipherSpiCreate(CipherAttr *params, HcfCipherGeneratorSpi **generator)
{
    LOGI("Start create rsa cipher spiObj.");
    if (generator == NULL || params == NULL) {
        LOGE("Invalid input parameter.");
        return HCF_INVALID_PARAMS;
    }
    HcfCipherRsaGeneratorSpiImpl *returnImpl = (HcfCipherRsaGeneratorSpiImpl *)HcfMalloc(
        sizeof(HcfCipherRsaGeneratorSpiImpl), 0);
    if (returnImpl == NULL) {
        LOGE("Malloc rsa cipher fail.");
        return HCF_ERR_MALLOC;
    }
    (void)memcpy_s(&returnImpl->attr, sizeof(CipherAttr), params, sizeof(CipherAttr));

    if (CheckRsaCipherParams(&returnImpl->attr) != HCF_SUCCESS) {
        HcfFree(returnImpl);
        returnImpl = NULL;
        return HCF_INVALID_PARAMS;
    }

    returnImpl->super.init = EngineInit;
    returnImpl->super.update = EngineUpdata;
    returnImpl->super.doFinal = EngineDoFinal;
    returnImpl->super.base.destroy = EngineDestroySpiImpl;
    returnImpl->super.base.getClass = EngineGetClass;
    returnImpl->initFlag = UNINITIALIZED;
    *generator = (HcfCipherGeneratorSpi *)returnImpl;
    LOGI("Rsa Cipher create success.");
    return HCF_SUCCESS;
}
