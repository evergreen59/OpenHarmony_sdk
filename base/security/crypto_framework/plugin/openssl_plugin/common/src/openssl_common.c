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

#include "openssl_common.h"

#include <string.h>
#include <openssl/err.h>
#include "config.h"
#include "log.h"
#include "result.h"
#include "params_parser.h"

#define PRIMES_2 2
#define PRIMES_3 3
#define PRIMES_4 4
#define PRIMES_5 5

typedef struct {
    char *oid;
    char *algorithmName;
} OidToAlgorithmName;

static const OidToAlgorithmName g_oidToNameMap[] = {
    { "1.2.840.113549.1.1.2", "MD2withRSA" },
    { "1.2.840.113549.1.1.4", "MD5withRSA" },
    { "1.2.840.113549.1.1.5", "SHA1withRSA" },
    { "1.2.840.10040.4.3", "SHA1withDSA" },
    { "1.2.840.10045.4.1", "SHA1withECDSA" },
    { "1.2.840.113549.1.1.14", "SHA224withRSA" },
    { "1.2.840.113549.1.1.11", "SHA256withRSA" },
    { "1.2.840.113549.1.1.12", "SHA384withRSA" },
    { "1.2.840.113549.1.1.13", "SHA512withRSA" },
    { "2.16.840.1.101.3.4.3.1", "SHA224withDSA" },
    { "2.16.840.1.101.3.4.3.2", "SHA256withDSA" },
    { "1.2.840.10045.4.3.1", "SHA224withECDSA" },
    { "1.2.840.10045.4.3.2", "SHA256withECDSA" },
    { "1.2.840.10045.4.3.3", "SHA384withECDSA" },
    { "1.2.840.10045.4.3.4", "SHA512withECDSA" }
};

const char *GetAlgorithmName(const char *oid)
{
    if (oid == NULL) {
        LOGE("Oid is null!");
        return NULL;
    }

    uint32_t oidCount = sizeof(g_oidToNameMap) / sizeof(OidToAlgorithmName);
    for (uint32_t i = 0; i < oidCount; i++) {
        if (strcmp(g_oidToNameMap[i].oid, oid) == 0) {
            return g_oidToNameMap[i].algorithmName;
        }
    }
    LOGE("Can not find algorithmName! [oid]: %s", oid);
    return NULL;
}

int32_t GetOpensslCurveId(int32_t keyLen, int32_t *returnCurveId)
{
    switch (keyLen) {
        case HCF_ALG_ECC_224:
            *returnCurveId = NID_secp224r1;
            break;
        case HCF_ALG_ECC_256:
            *returnCurveId = NID_X9_62_prime256v1;
            break;
        case HCF_ALG_ECC_384:
            *returnCurveId = NID_secp384r1;
            break;
        case HCF_ALG_ECC_521:
            *returnCurveId = NID_secp521r1;
            break;
        default:
            LOGE("invalid key size.");
            return HCF_INVALID_PARAMS;
    }

    return HCF_SUCCESS;
}

const EVP_MD *GetOpensslDigestAlg(uint32_t alg)
{
    switch (alg) {
        case HCF_OPENSSL_DIGEST_NONE:
            return NULL;
        case HCF_OPENSSL_DIGEST_MD5:
            LOGI("set EVP_md5");
            return EVP_md5();
        case HCF_OPENSSL_DIGEST_SHA1:
            LOGI("set EVP_sha1");
            return EVP_sha1();
        case HCF_OPENSSL_DIGEST_SHA224:
            LOGI("set EVP_sha224");
            return EVP_sha224();
        case HCF_OPENSSL_DIGEST_SHA256:
            LOGI("set EVP_sha256");
            return EVP_sha256();
        case HCF_OPENSSL_DIGEST_SHA384:
            LOGI("set EVP_sha384");
            return EVP_sha384();
        case HCF_OPENSSL_DIGEST_SHA512:
            LOGI("set EVP_sha512");
            return EVP_sha512();
        default:
            LOGE("Invalid digest num is %u.", alg);
            return NULL;
    }
}

void HcfPrintOpensslError(void)
{
    char szErr[LOG_PRINT_MAX_LEN] = {0};
    unsigned long errCode;

    errCode = ERR_get_error();
    ERR_error_string_n(errCode, szErr, LOG_PRINT_MAX_LEN);

    LOGE("[Openssl]: engine fail, error code = %lu, error string = %s", errCode, szErr);
}

int32_t GetOpensslPadding(int32_t padding, int32_t *opensslPadding)
{
    switch (padding) {
        case HCF_ALG_NOPADDING:
            LOGI("set RSA_NO_PADDING");
            *opensslPadding = RSA_NO_PADDING;
            return HCF_SUCCESS;

        case HCF_OPENSSL_RSA_PKCS1_PADDING:
            LOGI("set RSA_PKCS1_PADDING");
            *opensslPadding = RSA_PKCS1_PADDING;
            return HCF_SUCCESS;

        case HCF_OPENSSL_RSA_PKCS1_OAEP_PADDING:
            LOGI("set RSA_PKCS1_OAEP_PADDING");
            *opensslPadding = RSA_PKCS1_OAEP_PADDING;
            return HCF_SUCCESS;

        case HCF_OPENSSL_RSA_PSS_PADDING:
            LOGI("set RSA_PKCS1_PSS_PADDING");
            *opensslPadding = RSA_PKCS1_PSS_PADDING;
            return HCF_SUCCESS;

        default:
            LOGE("Invalid framwork padding = %d", padding);
            return HCF_INVALID_PARAMS;
    }
}

int32_t GetRealPrimes(int32_t primesFlag)
{
    switch (primesFlag) {
        case HCF_OPENSSL_PRIMES_2:
            LOGI("set primes 2");
            return PRIMES_2;
        case HCF_OPENSSL_PRIMES_3:
            LOGI("set primes 3");
            return PRIMES_3;
        case HCF_OPENSSL_PRIMES_4:
            LOGI("set primes 4");
            return PRIMES_4;
        case HCF_OPENSSL_PRIMES_5:
            LOGI("set primes 5");
            return PRIMES_5;
        default:
            LOGI("set default primes 2");
            return PRIMES_2;
    }
}

