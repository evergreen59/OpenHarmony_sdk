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

#ifndef HCF_X509_CRL_SPI_H
#define HCF_X509_CRL_SPI_H

#include "blob.h"
#include "object_base.h"
#include "pub_key.h"
#include "result.h"
#include "x509_certificate.h"
#include "x509_crl_entry.h"

typedef struct HcfX509CrlSpi HcfX509CrlSpi;

struct HcfX509CrlSpi {
    HcfObjectBase base;

    const char *(*engineGetType)(HcfX509CrlSpi *self);

    bool (*engineIsRevoked)(HcfX509CrlSpi *self, const HcfCertificate *cert);

    HcfResult (*engineGetEncoded)(HcfX509CrlSpi *self, HcfEncodingBlob *encodedByte);

    HcfResult (*engineVerify)(HcfX509CrlSpi *self, HcfPubKey *key);

    long (*engineGetVersion)(HcfX509CrlSpi *self);

    HcfResult (*engineGetIssuerName)(HcfX509CrlSpi *self, HcfBlob *out);

    HcfResult (*engineGetLastUpdate)(HcfX509CrlSpi *self, HcfBlob *out);

    HcfResult (*engineGetNextUpdate)(HcfX509CrlSpi *self, HcfBlob *out);

    HcfResult (*engineGetRevokedCert)(HcfX509CrlSpi *self, long serialNumber, HcfX509CrlEntry **entryOut);

    HcfResult (*engineGetRevokedCertWithCert)(HcfX509CrlSpi *self, HcfX509Certificate *cert,
        HcfX509CrlEntry **entryOut);

    HcfResult (*engineGetRevokedCerts)(HcfX509CrlSpi *self, HcfArray *entrysOut);

    HcfResult (*engineGetTbsInfo)(HcfX509CrlSpi *self, HcfBlob *tbsCertListOut);

    HcfResult (*engineGetSignature)(HcfX509CrlSpi *self, HcfBlob *signature);

    HcfResult (*engineGetSignatureAlgName)(HcfX509CrlSpi *self, HcfBlob *out);

    HcfResult (*engineGetSignatureAlgOid)(HcfX509CrlSpi *self, HcfBlob *out);

    HcfResult (*engineGetSignatureAlgParams)(HcfX509CrlSpi *self, HcfBlob *sigAlgParamOut);
};

#endif // HCF_X509_CERTIFICATE_SPI_H
