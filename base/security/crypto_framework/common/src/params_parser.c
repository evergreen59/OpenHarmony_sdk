/*
 * Copyright (C) 2022-2023 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "params_parser.h"

#include <stdbool.h>
#include <stddef.h>
#include "hcf_string.h"
#include "log.h"

static const HcfParaConfig PARAM_CONFIG[] = {
    {"ECC224",       HCF_ALG_KEY_TYPE,       HCF_ALG_ECC_224},
    {"ECC256",       HCF_ALG_KEY_TYPE,       HCF_ALG_ECC_256},
    {"ECC384",       HCF_ALG_KEY_TYPE,       HCF_ALG_ECC_384},
    {"ECC521",       HCF_ALG_KEY_TYPE,       HCF_ALG_ECC_521},

    {"AES128",       HCF_ALG_KEY_TYPE,       HCF_ALG_AES_128},
    {"AES192",       HCF_ALG_KEY_TYPE,       HCF_ALG_AES_192},
    {"AES256",       HCF_ALG_KEY_TYPE,       HCF_ALG_AES_256},
    {"3DES192",      HCF_ALG_KEY_TYPE,       HCF_ALG_3DES_192},

    {"ECB",          HCF_ALG_MODE,           HCF_ALG_MODE_ECB},
    {"CBC",          HCF_ALG_MODE,           HCF_ALG_MODE_CBC},
    {"CTR",          HCF_ALG_MODE,           HCF_ALG_MODE_CTR},
    {"OFB",          HCF_ALG_MODE,           HCF_ALG_MODE_OFB},
    {"CFB",          HCF_ALG_MODE,           HCF_ALG_MODE_CFB},
    {"CFB1",         HCF_ALG_MODE,           HCF_ALG_MODE_CFB1},
    {"CFB8",         HCF_ALG_MODE,           HCF_ALG_MODE_CFB8},
    {"CFB64",        HCF_ALG_MODE,           HCF_ALG_MODE_CFB64},
    {"CFB128",       HCF_ALG_MODE,           HCF_ALG_MODE_CFB128},
    {"CCM",          HCF_ALG_MODE,           HCF_ALG_MODE_CCM},
    {"GCM",          HCF_ALG_MODE,           HCF_ALG_MODE_GCM},

    {"NoPadding",    HCF_ALG_PADDING_TYPE,   HCF_ALG_NOPADDING},
    {"PKCS5",        HCF_ALG_PADDING_TYPE,   HCF_ALG_PADDING_PKCS5},
    {"PKCS7",        HCF_ALG_PADDING_TYPE,   HCF_ALG_PADDING_PKCS7},

    {"RSA512",        HCF_ALG_KEY_TYPE,       HCF_OPENSSL_RSA_512},
    {"RSA768",        HCF_ALG_KEY_TYPE,       HCF_OPENSSL_RSA_768},
    {"RSA1024",       HCF_ALG_KEY_TYPE,       HCF_OPENSSL_RSA_1024},
    {"RSA2048",       HCF_ALG_KEY_TYPE,       HCF_OPENSSL_RSA_2048},
    {"RSA3072",       HCF_ALG_KEY_TYPE,       HCF_OPENSSL_RSA_3072},
    {"RSA4096",       HCF_ALG_KEY_TYPE,       HCF_OPENSSL_RSA_4096},
    {"RSA8192",       HCF_ALG_KEY_TYPE,       HCF_OPENSSL_RSA_8192},

    {"PKCS1",        HCF_ALG_PADDING_TYPE,   HCF_OPENSSL_RSA_PKCS1_PADDING},
    {"PKCS1_OAEP",   HCF_ALG_PADDING_TYPE,   HCF_OPENSSL_RSA_PKCS1_OAEP_PADDING},
    {"PSS",          HCF_ALG_PADDING_TYPE,   HCF_OPENSSL_RSA_PSS_PADDING},

    {"MD5",          HCF_ALG_DIGEST,         HCF_OPENSSL_DIGEST_MD5},
    {"SHA1",         HCF_ALG_DIGEST,         HCF_OPENSSL_DIGEST_SHA1},
    {"SHA224",       HCF_ALG_DIGEST,         HCF_OPENSSL_DIGEST_SHA224},
    {"SHA256",       HCF_ALG_DIGEST,         HCF_OPENSSL_DIGEST_SHA256},
    {"SHA384",       HCF_ALG_DIGEST,         HCF_OPENSSL_DIGEST_SHA384},
    {"SHA512",       HCF_ALG_DIGEST,         HCF_OPENSSL_DIGEST_SHA512},

    {"MGF1_MD5",          HCF_ALG_MGF1_DIGEST,         HCF_OPENSSL_DIGEST_MD5},
    {"MGF1_SHA1",         HCF_ALG_MGF1_DIGEST,         HCF_OPENSSL_DIGEST_SHA1},
    {"MGF1_SHA224",       HCF_ALG_MGF1_DIGEST,         HCF_OPENSSL_DIGEST_SHA224},
    {"MGF1_SHA256",       HCF_ALG_MGF1_DIGEST,         HCF_OPENSSL_DIGEST_SHA256},
    {"MGF1_SHA384",       HCF_ALG_MGF1_DIGEST,         HCF_OPENSSL_DIGEST_SHA384},
    {"MGF1_SHA512",       HCF_ALG_MGF1_DIGEST,         HCF_OPENSSL_DIGEST_SHA512},

    {"PRIMES_2",          HCF_ALG_PRIMES,              HCF_OPENSSL_PRIMES_2},
    {"PRIMES_3",          HCF_ALG_PRIMES,              HCF_OPENSSL_PRIMES_3},
    {"PRIMES_4",          HCF_ALG_PRIMES,              HCF_OPENSSL_PRIMES_4},
    {"PRIMES_5",          HCF_ALG_PRIMES,              HCF_OPENSSL_PRIMES_5},

};

static const HcfParaConfig *FindConfig(const HcString* tag)
{
    if (tag == NULL) {
        return NULL;
    }

    for (uint32_t i = 0; i < sizeof(PARAM_CONFIG) / sizeof(HcfParaConfig); ++i) {
        if (StringCompare(tag, PARAM_CONFIG[i].tag) == 0) {
            return &PARAM_CONFIG[i];
        }
    }
    return NULL;
}

HcfResult ParseAndSetParameter(const char *paramsStr, void *params, SetParameterFunc setFunc)
{
    if (paramsStr == NULL || setFunc == NULL) {
        return HCF_INVALID_PARAMS;
    }
    HcString str = CreateString();
    HcString subStr = CreateString();
    if (!StringSetPointer(&str, paramsStr)) {
        DeleteString(&subStr);
        DeleteString(&str);
        return HCF_INVALID_PARAMS;
    }
    HcfResult ret = HCF_INVALID_PARAMS;
    uint32_t pos = 0;
    do {
        int findPos = StringFind(&str, '|', pos);
        if (findPos >= 0) {
            if (!StringSubString(&str, pos, findPos - pos, &subStr)) {
                LOGE("StringSubString failed!");
                break;
            }
            ret = (*setFunc)(FindConfig(&subStr), params);
            if (ret != HCF_SUCCESS) {
                break;
            }
            pos = findPos + 1;
        } else {
            uint32_t strLen = StringLength(&str);
            if (strLen < pos) {
                break;
            }
            if (!StringSubString(&str, pos, strLen - pos, &subStr)) {
                LOGE("get last string failed!");
                break;
            }
            ret = (*setFunc)(FindConfig(&subStr), params);
            break;
        }
    } while (true);

    DeleteString(&subStr);
    DeleteString(&str);
    return ret;
}
