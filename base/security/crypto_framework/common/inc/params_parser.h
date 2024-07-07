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

#ifndef HCF_PARAMS_PARSER_H
#define HCF_PARAMS_PARSER_H

#include <stdint.h>
#include "result.h"

typedef enum {
    HCF_ALG_TYPE = 1,
    HCF_ALG_KEY_TYPE,
    HCF_ALG_MODE,
    HCF_ALG_PADDING_TYPE,
    HCF_ALG_PRIMES,
    HCF_ALG_DIGEST,
    HCF_ALG_MGF1_DIGEST,
} HCF_ALG_PARA_TYPE;

typedef enum {
    HCF_ALG_AES = 1,
    HCF_ALG_DES,
    HCF_ALG_RSA,
    HCF_ALG_ECC,
} HCF_ALG_VALUE;

typedef enum {
    HCF_ALG_ECC_224 = 1,
    HCF_ALG_ECC_256,
    HCF_ALG_ECC_384,
    HCF_ALG_ECC_521,

    HCF_ALG_AES_128,
    HCF_ALG_AES_192,
    HCF_ALG_AES_256,
    HCF_ALG_3DES_192,

    HCF_ALG_MODE_NONE,
    HCF_ALG_MODE_ECB,
    HCF_ALG_MODE_CBC,
    HCF_ALG_MODE_CTR,
    HCF_ALG_MODE_OFB,
    HCF_ALG_MODE_CFB,
    HCF_ALG_MODE_CFB1,
    HCF_ALG_MODE_CFB8,
    HCF_ALG_MODE_CFB64,
    HCF_ALG_MODE_CFB128,
    HCF_ALG_MODE_CCM,
    HCF_ALG_MODE_GCM,

    HCF_ALG_NOPADDING,
    HCF_ALG_PADDING_PKCS5,
    HCF_ALG_PADDING_PKCS7,

    // rsa keysize
    HCF_OPENSSL_RSA_512,
    HCF_OPENSSL_RSA_768,
    HCF_OPENSSL_RSA_1024,
    HCF_OPENSSL_RSA_2048,
    HCF_OPENSSL_RSA_3072,
    HCF_OPENSSL_RSA_4096,
    HCF_OPENSSL_RSA_8192,

    // rsa cipher padding,
    HCF_OPENSSL_RSA_PKCS1_PADDING,
    HCF_OPENSSL_RSA_PKCS1_OAEP_PADDING,
    HCF_OPENSSL_RSA_PSS_PADDING,

    // digest
    HCF_OPENSSL_DIGEST_NONE,
    HCF_OPENSSL_DIGEST_MD5,
    HCF_OPENSSL_DIGEST_SHA1,
    HCF_OPENSSL_DIGEST_SHA224,
    HCF_OPENSSL_DIGEST_SHA256,
    HCF_OPENSSL_DIGEST_SHA384,
    HCF_OPENSSL_DIGEST_SHA512,

    // primes
    HCF_OPENSSL_PRIMES_2,
    HCF_OPENSSL_PRIMES_3,
    HCF_OPENSSL_PRIMES_4,
    HCF_OPENSSL_PRIMES_5,
} HCF_ALG_PARA_VALUE;

typedef struct {
    const char* tag;
    HCF_ALG_PARA_TYPE paraType;
    HCF_ALG_PARA_VALUE paraValue;
} HcfParaConfig;

typedef struct {
    HCF_ALG_VALUE algo;
    HCF_ALG_PARA_VALUE keySize;
    HCF_ALG_PARA_VALUE mode;
    HCF_ALG_PARA_VALUE paddingMode;
    HCF_ALG_PARA_VALUE md;
    HCF_ALG_PARA_VALUE mgf1md;
} CipherAttr;

typedef struct {
    HCF_ALG_VALUE algo; // algType
    int32_t bits; // keyLen
    int32_t primes; // number of primes
} HcfAsyKeyGenParams;

typedef struct {
    HCF_ALG_VALUE algo; // algType
    HCF_ALG_PARA_VALUE keyLen;
    HCF_ALG_PARA_VALUE padding;
    HCF_ALG_PARA_VALUE md;
    HCF_ALG_PARA_VALUE mgf1md;
} HcfSignatureParams;

typedef struct {
    HCF_ALG_VALUE algo;
    HCF_ALG_PARA_VALUE keyLen;
} HcfKeyAgreementParams;

typedef HcfResult (*SetParameterFunc) (const HcfParaConfig* config, void *params);

#ifdef __cplusplus
extern "C" {
#endif

HcfResult ParseAndSetParameter(const char *paramsStr, void *params, SetParameterFunc setFunc);

#ifdef __cplusplus
}
#endif
#endif
