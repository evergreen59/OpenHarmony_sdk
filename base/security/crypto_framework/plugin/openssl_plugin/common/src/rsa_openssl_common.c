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
#include "rsa_openssl_common.h"
#include "log.h"
#include "openssl_common.h"

HcfResult DuplicateRsa(RSA *rsa, bool needPrivate, RSA **dupRsa)
{
    RSA *retRSA = NULL;
    if (rsa == NULL || dupRsa == NULL) {
        LOGE("Rsa or dupRsa is NULL.");
        return HCF_INVALID_PARAMS;
    }
    if (needPrivate) {
        retRSA = RSAPrivateKey_dup(rsa);
    } else {
        retRSA = RSAPublicKey_dup(rsa);
    }
    if (retRSA == NULL) {
        LOGE("Duplicate RSA fail.");
        HcfPrintOpensslError();
        return HCF_ERR_CRYPTO_OPERATION;
    }
    *dupRsa = retRSA;
    return HCF_SUCCESS;
}

EVP_PKEY *NewEvpPkeyByRsa(RSA *rsa, bool withDuplicate)
{
    if (rsa == NULL) {
        LOGE("RSA is NULL");
        return NULL;
    }
    EVP_PKEY *pKey = EVP_PKEY_new();
    if (pKey == NULL) {
        LOGE("EVP_PKEY_new fail");
        HcfPrintOpensslError();
        return NULL;
    }
    if (withDuplicate) {
        if (EVP_PKEY_set1_RSA(pKey, rsa) != HCF_OPENSSL_SUCCESS) {
            LOGE("EVP_PKEY_set1_RSA fail");
            HcfPrintOpensslError();
            EVP_PKEY_free(pKey);
            return NULL;
        }
    } else {
        if (EVP_PKEY_assign_RSA(pKey, rsa) != HCF_OPENSSL_SUCCESS) {
            LOGE("EVP_PKEY_assign_RSA fail");
            HcfPrintOpensslError();
            EVP_PKEY_free(pKey);
            return NULL;
        }
    }
    return pKey;
}
