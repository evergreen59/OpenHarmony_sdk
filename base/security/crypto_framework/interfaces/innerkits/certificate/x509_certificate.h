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

#ifndef HCF_X509_CERTIFICATE_H
#define HCF_X509_CERTIFICATE_H

#include "certificate.h"
#include "blob.h"
#include "result.h"

typedef struct HcfX509Certificate HcfX509Certificate;

struct HcfX509Certificate {
    /** HcfCX509Certificate inherit HcfCertificate. */
    HcfCertificate base;

    /** Check whether the certificate is valid at the given time.
     * time format： YYMMDDHHMMSSZ or YYYYMMDDHHMMSSZ
    */
    HcfResult (*checkValidityWithDate)(HcfX509Certificate *self, const char *date);

    /** Get version number from certificate. */
    long (*getVersion)(HcfX509Certificate *self);

    /** Get serial number from certificate. */
    long (*getSerialNumber)(HcfX509Certificate *self);

    /** Get issuer distinguished name from certificate. */
    HcfResult (*getIssuerName)(HcfX509Certificate *self, HcfBlob *out);

    /** Get subject distinguished name from certificate. */
    HcfResult (*getSubjectName)(HcfX509Certificate *self, HcfBlob *out);

    /** Get the not before time within the validity period of the certificate.
     * time format： YYMMDDHHMMSSZ or YYYYMMDDHHMMSSZ
     */
    HcfResult (*getNotBeforeTime)(HcfX509Certificate *self, HcfBlob *outDate);

    /** Get the not after time within the validity period of the certificate.
     *  time format： YYMMDDHHMMSSZ or YYYYMMDDHHMMSSZ
     */
    HcfResult (*getNotAfterTime)(HcfX509Certificate *self, HcfBlob *outDate);

    /** Get signature value from certificate. */
    HcfResult (*getSignature)(HcfX509Certificate *self, HcfBlob *sigOut);

    /** Get signature algorithm name from certificate. */
    HcfResult (*getSignatureAlgName)(HcfX509Certificate *self, HcfBlob *outName);

    /** Get signature algorithm oid from certificate. */
    HcfResult (*getSignatureAlgOid)(HcfX509Certificate *self, HcfBlob *out);

    /** Get the DER encoded signature algorithm parameters from the signature algorithm of the certificate. */
    HcfResult (*getSignatureAlgParams)(HcfX509Certificate *self, HcfBlob *sigAlgParamsOut);

    /** Get a Boolean array representing the bits of keyuse extension.
     * The key usage extension defines the purpose of the key. */
    HcfResult (*getKeyUsage)(HcfX509Certificate *self, HcfBlob *boolArr);

    /** Get a const string list that represents the object identifier of the extkeyusage. */
    HcfResult (*getExtKeyUsage)(HcfX509Certificate *self, HcfArray *keyUsageOut);

    /** Get the path length of the certificate constraint from the key extensions(BasicConstraints).
     * The BasicConstraints identify whether the issuer of the certificate is CA and the depth of the cert chain.
     * Only when CA is set to true, pathLenConstraint is meaningful.
     */
    int32_t (*getBasicConstraints)(HcfX509Certificate *self);

    /** Get subject alternative name from certificate. */
    HcfResult (*getSubjectAltNames)(HcfX509Certificate *self, HcfArray *outName);

    /** Get issuer alternative name from certificate. */
    HcfResult (*getIssuerAltNames)(HcfX509Certificate *self, HcfArray *outName);
};

#ifdef __cplusplus
extern "C" {
#endif

HcfResult HcfX509CertificateCreate(const HcfEncodingBlob *inStream, HcfX509Certificate **returnObj);

#ifdef __cplusplus
}
#endif

#endif // HCF_X509_CERTIFICATE_H

