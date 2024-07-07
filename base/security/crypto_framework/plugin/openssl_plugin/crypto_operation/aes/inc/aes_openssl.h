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
#ifndef HCF_AES_OPENSSL_H
#define HCF_AES_OPENSSL_H

#include <stdbool.h>
#include <openssl/evp.h>
#include "params_parser.h"
#include "cipher_factory_spi.h"

typedef struct {
    EVP_CIPHER_CTX *ctx;
    enum HcfCryptoMode enc;
    /* EVP_CIPH_GCM_MODE, EVP_CIPH_CCM_MODE need AEAD */
    bool aead;
    uint32_t updateLen;
    unsigned char *iv;
    uint32_t ivLen;
    /* GCM, CCM only */
    unsigned char *aad;
    uint32_t aadLen;
    unsigned char *tag;
    uint32_t tagLen;
} CipherData;

#ifdef __cplusplus
extern "C" {
#endif

HcfResult HcfCipherDesGeneratorSpiCreate(CipherAttr *attr, HcfCipherGeneratorSpi **generator);

HcfResult HcfCipherAesGeneratorSpiCreate(CipherAttr *attr, HcfCipherGeneratorSpi **generator);

#ifdef __cplusplus
}
#endif

#endif

