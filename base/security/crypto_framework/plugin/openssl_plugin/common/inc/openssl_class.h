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

#ifndef HCF_OEPNSSL_CLASS_H
#define HCF_OEPNSSL_CLASS_H

#include "pub_key.h"
#include "pri_key.h"
#include "key_pair.h"
#include "x509_certificate_spi.h"
#include "x509_crl_entry.h"
#include "x509_crl.h"
#include "x509_crl_spi.h"

#include <openssl/bn.h>
#include <openssl/ec.h>
#include <openssl/evp.h>
#include <openssl/rsa.h>
#include <openssl/x509.h>

typedef struct {
    HcfPubKey base;

    int32_t curveId;

    EC_POINT *pk;
} HcfOpensslEccPubKey;
#define HCF_OPENSSL_ECC_PUB_KEY_CLASS "OPENSSL.ECC.PUB_KEY"

typedef struct {
    HcfPriKey base;

    int32_t curveId;

    BIGNUM *sk;
} HcfOpensslEccPriKey;
#define HCF_OPENSSL_ECC_PRI_KEY_CLASS "OPENSSL.ECC.PRI_KEY"

typedef struct {
    HcfKeyPair base;
} HcfOpensslEccKeyPair;
#define HCF_OPENSSL_ECC_KEY_PAIR_CLASS "OPENSSL.ECC.KEY_PAIR"

typedef struct {
    HcfPubKey base;

    uint32_t bits;

    RSA *pk;
} HcfOpensslRsaPubKey;
#define OPENSSL_RSA_PUBKEY_CLASS "OPENSSL.RSA.PUB_KEY"

typedef struct {
    HcfPriKey base;

    uint32_t bits;

    RSA *sk;
} HcfOpensslRsaPriKey;
#define OPENSSL_RSA_PRIKEY_CLASS "OPENSSL.RSA.PRI_KEY"

typedef struct {
    HcfKeyPair base;
} HcfOpensslRsaKeyPair;
#define OPENSSL_RSA_KEYPAIR_CLASS "OPENSSL.RSA.KEY_PAIR"

typedef struct {
    HcfX509CertificateSpi base;
    X509 *x509;
} HcfOpensslX509Cert;
#define X509_CERT_OPENSSL_CLASS "X509CertOpensslClass"

typedef struct {
    HcfX509CrlEntry base;
    X509_REVOKED *rev;
    HcfBlob *certIssuer;
} HcfX509CRLEntryOpensslImpl;
#define X509_CRL_ENTRY_OPENSSL_CLASS "X509CrlEntryOpensslClass"

typedef struct {
    HcfX509Crl base;
    HcfX509CrlSpi *spiObj;
    const char *certType;
} HcfX509CrlImpl;
#define X509_CRL_OPENSSL_CLASS "X509CrlOpensslClass"

#define OPENSSL_RSA_CIPHER_CLASS "OPENSSL.RSA.CIPHER"
#define OPENSSL_3DES_CIPHER_CLASS "OPENSSL.3DES.CIPHER"
#define OPENSSL_AES_CIPHER_CLASS "OPENSSL.AES.CIPHER"

#endif
