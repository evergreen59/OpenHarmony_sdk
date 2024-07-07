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

#ifndef HCF_X509CRL_H
#define HCF_X509CRL_H

#include "blob.h"
#include "crl.h"
#include "pub_key.h"
#include "x509_certificate.h"
#include "x509_crl_entry.h"

typedef struct HcfX509Crl HcfX509Crl;

struct HcfX509Crl {
    /** HcfX509Crl inherit HcfCrl. */
    HcfCrl base;

    /** Get the der coding format. */
    HcfResult (*getEncoded)(HcfX509Crl *self, HcfEncodingBlob *encodedOut);

    /** Use the public key to verify the signature of CRL. */
    HcfResult (*verify)(HcfX509Crl *self, HcfPubKey *key);

    /** Get version number from CRL. */
    long (*getVersion)(HcfX509Crl *self);

    /** Get the issuer name from CRL. Issuer means the entity that signs and publishes the CRL. */
    HcfResult (*getIssuerName)(HcfX509Crl *self, HcfBlob *out);

    /** Get lastUpdate value from CRL. */
    HcfResult (*getLastUpdate)(HcfX509Crl *self, HcfBlob *out);

    /** Get nextUpdate value from CRL. */
    HcfResult (*getNextUpdate)(HcfX509Crl *self, HcfBlob *out);

    /** This method can be used to find CRL entries in indirect CRLs. */
    HcfResult (*getRevokedCert)(HcfX509Crl *self, long serialNumber, HcfX509CrlEntry **entryOut);

    /** This method can be used to find CRL entries in indirect cert. */
    HcfResult (*getRevokedCertWithCert)(HcfX509Crl *self, HcfX509Certificate *cert,
        HcfX509CrlEntry **entryOut);

    /** Get all entries in this CRL. */
    HcfResult (*getRevokedCerts)(HcfX509Crl *self, HcfArray *entrysOut);

    /** Get the CRL information encoded by Der from this CRL. */
    HcfResult (*getTbsInfo)(HcfX509Crl *self, HcfBlob *tbsCertListOut);

    /** Get signature value from CRL. */
    HcfResult (*getSignature)(HcfX509Crl *self, HcfBlob *signature);

    /** Get the signature algorithm name of the CRL signature algorithm. */
    HcfResult (*getSignatureAlgName)(HcfX509Crl *self, HcfBlob *out);

    /** Get the signature algorithm oid string from CRL. */
    HcfResult (*getSignatureAlgOid)(HcfX509Crl *self, HcfBlob *out);

    /** Get the der encoded signature algorithm parameters from the CRL signature algorithm. */
    HcfResult (*getSignatureAlgParams)(HcfX509Crl *self, HcfBlob *sigAlgParamOut);
};

#ifdef __cplusplus
extern "C" {
#endif

HcfResult HcfX509CrlCreate(const HcfEncodingBlob *inStream, HcfX509Crl **returnObj);

#ifdef __cplusplus
}
#endif

#endif // HCF_X509CRL_H