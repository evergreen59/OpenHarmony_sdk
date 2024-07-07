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

#ifndef HCF_X509_CERTIFICATE_SPI_H
#define HCF_X509_CERTIFICATE_SPI_H

#include "blob.h"
#include "object_base.h"
#include "pub_key.h"
#include "result.h"

typedef struct HcfX509CertificateSpi HcfX509CertificateSpi;

struct HcfX509CertificateSpi {
    HcfObjectBase base;

    HcfResult (*engineVerify)(HcfX509CertificateSpi *self, HcfPubKey *key);

    HcfResult (*engineGetEncoded)(HcfX509CertificateSpi *self, HcfEncodingBlob *encodedByte);

    HcfResult (*engineGetPublicKey)(HcfX509CertificateSpi *self, HcfPubKey **keyOut);

    HcfResult (*engineCheckValidityWithDate)(HcfX509CertificateSpi *self, const char *date);

    long (*engineGetVersion)(HcfX509CertificateSpi *self);

    long (*engineGetSerialNumber)(HcfX509CertificateSpi *self);

    HcfResult (*engineGetIssuerName)(HcfX509CertificateSpi *self, HcfBlob *out);

    HcfResult (*engineGetSubjectName)(HcfX509CertificateSpi *self, HcfBlob *out);

    HcfResult (*engineGetNotBeforeTime)(HcfX509CertificateSpi *self, HcfBlob *outDate);

    HcfResult (*engineGetNotAfterTime)(HcfX509CertificateSpi *self, HcfBlob *outDate);

    HcfResult (*engineGetSignature)(HcfX509CertificateSpi *self, HcfBlob *sigOut);

    HcfResult (*engineGetSignatureAlgName)(HcfX509CertificateSpi *self, HcfBlob *outName);

    HcfResult (*engineGetSignatureAlgOid)(HcfX509CertificateSpi *self, HcfBlob *out);

    HcfResult (*engineGetSignatureAlgParams)(HcfX509CertificateSpi *self, HcfBlob *sigAlgParamsOut);

    HcfResult (*engineGetKeyUsage)(HcfX509CertificateSpi *self, HcfBlob *boolArr);

    HcfResult (*engineGetExtKeyUsage)(HcfX509CertificateSpi *self, HcfArray *keyUsageOut);

    int32_t (*engineGetBasicConstraints)(HcfX509CertificateSpi *self);

    HcfResult (*engineGetSubjectAltNames)(HcfX509CertificateSpi *self, HcfArray *outName);

    HcfResult (*engineGetIssuerAltNames)(HcfX509CertificateSpi *self, HcfArray *outName);
};

#endif // HCF_X509_CERTIFICATE_SPI_H
