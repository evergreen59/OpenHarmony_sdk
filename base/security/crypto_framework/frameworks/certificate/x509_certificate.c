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

#include "x509_certificate.h"

#include <securec.h>

#include "config.h"
#include "fwk_class.h"
#include "x509_certificate_openssl.h"
#include "log.h"
#include "memory.h"
#include "utils.h"

typedef HcfResult (*HcfX509CertificateSpiCreateFunc)(const HcfEncodingBlob *, HcfX509CertificateSpi **);

typedef struct {
    HcfX509CertificateSpiCreateFunc createFunc;
} HcfX509CertificateFuncSet;

typedef struct {
    char *certType;
    HcfX509CertificateFuncSet funcSet;
} HcfCCertFactoryAbility;

static const char *GetX509CertificateClass(void)
{
    return HCF_X509_CERTIFICATE_CLASS;
}

static const HcfCCertFactoryAbility X509_CERTIFICATE_ABILITY_SET[] = {
    { "X509", { OpensslX509CertSpiCreate, } }
};

static const HcfX509CertificateFuncSet *FindAbility(const char *certType)
{
    if (certType == NULL) {
        LOGE("CertType is null!");
        return NULL;
    }
    for (uint32_t i = 0; i < sizeof(X509_CERTIFICATE_ABILITY_SET) / sizeof(HcfCCertFactoryAbility); i++) {
        if (strcmp(X509_CERTIFICATE_ABILITY_SET[i].certType, certType) == 0) {
            return &(X509_CERTIFICATE_ABILITY_SET[i].funcSet);
        }
    }
    LOGE("Cert not support! [cert]: %s", certType);
    return NULL;
}

static void DestroyX509Certificate(HcfObjectBase *self)
{
    if (self == NULL) {
        LOGE("Invalid input parameter.");
        return;
    }
    if (!IsClassMatch(self, GetX509CertificateClass())) {
        LOGE("Class is not match.");
        return;
    }
    HcfX509CertificateImpl *impl = (HcfX509CertificateImpl *)self;
    HcfObjDestroy(impl->spiObj);
    HcfFree(impl);
}

static HcfResult Verify(HcfCertificate *self, HcfPubKey *key)
{
    if ((self == NULL) || (key == NULL)) {
        LOGE("Invalid input parameter.");
        return HCF_INVALID_PARAMS;
    }
    if (!IsClassMatch((HcfObjectBase *)self, GetX509CertificateClass())) {
        LOGE("Class is not match.");
        return HCF_INVALID_PARAMS;
    }
    return ((HcfX509CertificateImpl *)self)->spiObj->engineVerify(
        ((HcfX509CertificateImpl *)self)->spiObj, key);
}

static HcfResult GetEncoded(HcfCertificate *self, HcfEncodingBlob *encodedByte)
{
    if ((self == NULL) || (encodedByte == NULL)) {
        LOGE("Invalid input parameter.");
        return HCF_INVALID_PARAMS;
    }
    if (!IsClassMatch((HcfObjectBase *)self, GetX509CertificateClass())) {
        LOGE("Class is not match.");
        return HCF_INVALID_PARAMS;
    }
    return ((HcfX509CertificateImpl *)self)->spiObj->engineGetEncoded(
        ((HcfX509CertificateImpl *)self)->spiObj, encodedByte);
}

static HcfResult GetPublicKey(HcfCertificate *self, HcfPubKey **keyOut)
{
    if ((self == NULL) || (keyOut == NULL)) {
        LOGE("Invalid input parameter.");
        return HCF_INVALID_PARAMS;
    }
    if (!IsClassMatch((HcfObjectBase *)self, GetX509CertificateClass())) {
        LOGE("Class is not match.");
        return HCF_INVALID_PARAMS;
    }
    return ((HcfX509CertificateImpl *)self)->spiObj->engineGetPublicKey(
        ((HcfX509CertificateImpl *)self)->spiObj, keyOut);
}

static HcfResult CheckValidityWithDate(HcfX509Certificate *self, const char *date)
{
    if ((self == NULL) || (date == NULL)) {
        LOGE("Invalid input parameter.");
        return HCF_INVALID_PARAMS;
    }
    if (!IsClassMatch((HcfObjectBase *)self, GetX509CertificateClass())) {
        LOGE("Class is not match.");
        return HCF_INVALID_PARAMS;
    }
    return ((HcfX509CertificateImpl *)self)->spiObj->engineCheckValidityWithDate(
        ((HcfX509CertificateImpl *)self)->spiObj, date);
}

static long GetVersion(HcfX509Certificate *self)
{
    if (self == NULL) {
        LOGE("Invalid input parameter.");
        return INVALID_VERSION;
    }
    if (!IsClassMatch((HcfObjectBase *)self, GetX509CertificateClass())) {
        LOGE("Class is not match.");
        return INVALID_VERSION;
    }
    return ((HcfX509CertificateImpl *)self)->spiObj->engineGetVersion(
        ((HcfX509CertificateImpl *)self)->spiObj);
}

static long GetSerialNumber(HcfX509Certificate *self)
{
    if (self == NULL) {
        LOGE("Invalid input parameter.");
        return INVALID_SERIAL_NUMBER;
    }
    if (!IsClassMatch((HcfObjectBase *)self, GetX509CertificateClass())) {
        LOGE("Class is not match.");
        return INVALID_SERIAL_NUMBER;
    }
    return ((HcfX509CertificateImpl *)self)->spiObj->engineGetSerialNumber(
        ((HcfX509CertificateImpl *)self)->spiObj);
}

static HcfResult GetIssuerName(HcfX509Certificate *self, HcfBlob *out)
{
    if ((self == NULL) || (out == NULL)) {
        LOGE("Invalid input parameter.");
        return HCF_INVALID_PARAMS;
    }
    if (!IsClassMatch((HcfObjectBase *)self, GetX509CertificateClass())) {
        LOGE("Class is not match.");
        return HCF_INVALID_PARAMS;
    }
    return ((HcfX509CertificateImpl *)self)->spiObj->engineGetIssuerName(
        ((HcfX509CertificateImpl *)self)->spiObj, out);
}

static HcfResult GetSubjectName(HcfX509Certificate *self, HcfBlob *out)
{
    if ((self == NULL) || (out == NULL)) {
        LOGE("Invalid input parameter.");
        return HCF_INVALID_PARAMS;
    }
    if (!IsClassMatch((HcfObjectBase *)self, GetX509CertificateClass())) {
        LOGE("Class is not match.");
        return HCF_INVALID_PARAMS;
    }
    return ((HcfX509CertificateImpl *)self)->spiObj->engineGetSubjectName(
        ((HcfX509CertificateImpl *)self)->spiObj, out);
}

static HcfResult GetNotBeforeTime(HcfX509Certificate *self, HcfBlob *outDate)
{
    if ((self == NULL) || (outDate == NULL)) {
        LOGE("Invalid input parameter.");
        return HCF_INVALID_PARAMS;
    }
    if (!IsClassMatch((HcfObjectBase *)self, GetX509CertificateClass())) {
        LOGE("Class is not match.");
        return HCF_INVALID_PARAMS;
    }
    return ((HcfX509CertificateImpl *)self)->spiObj->engineGetNotBeforeTime(
        ((HcfX509CertificateImpl *)self)->spiObj, outDate);
}

static HcfResult GetNotAfterTime(HcfX509Certificate *self, HcfBlob *outDate)
{
    if ((self == NULL) || (outDate == NULL)) {
        LOGE("Invalid input parameter.");
        return HCF_INVALID_PARAMS;
    }
    if (!IsClassMatch((HcfObjectBase *)self, GetX509CertificateClass())) {
        LOGE("Class is not match.");
        return HCF_INVALID_PARAMS;
    }
    return ((HcfX509CertificateImpl *)self)->spiObj->engineGetNotAfterTime(
        ((HcfX509CertificateImpl *)self)->spiObj, outDate);
}

static HcfResult GetSignature(HcfX509Certificate *self, HcfBlob *sigOut)
{
    if ((self == NULL) || (sigOut == NULL)) {
        LOGE("Invalid input parameter.");
        return HCF_INVALID_PARAMS;
    }
    if (!IsClassMatch((HcfObjectBase *)self, GetX509CertificateClass())) {
        LOGE("Class is not match.");
        return HCF_INVALID_PARAMS;
    }
    return ((HcfX509CertificateImpl *)self)->spiObj->engineGetSignature(
        ((HcfX509CertificateImpl *)self)->spiObj, sigOut);
}

static HcfResult GetSignatureAlgName(HcfX509Certificate *self, HcfBlob *outName)
{
    if ((self == NULL) || (outName == NULL)) {
        LOGE("Invalid input parameter.");
        return HCF_INVALID_PARAMS;
    }
    if (!IsClassMatch((HcfObjectBase *)self, GetX509CertificateClass())) {
        LOGE("Class is not match.");
        return HCF_INVALID_PARAMS;
    }
    return ((HcfX509CertificateImpl *)self)->spiObj->engineGetSignatureAlgName(
        ((HcfX509CertificateImpl *)self)->spiObj, outName);
}

static HcfResult GetSignatureAlgOid(HcfX509Certificate *self, HcfBlob *out)
{
    if ((self == NULL) || (out == NULL)) {
        LOGE("Invalid input parameter.");
        return HCF_INVALID_PARAMS;
    }
    if (!IsClassMatch((HcfObjectBase *)self, GetX509CertificateClass())) {
        LOGE("Class is not match.");
        return HCF_INVALID_PARAMS;
    }
    return ((HcfX509CertificateImpl *)self)->spiObj->engineGetSignatureAlgOid(
        ((HcfX509CertificateImpl *)self)->spiObj, out);
}

static HcfResult GetSignatureAlgParams(HcfX509Certificate *self, HcfBlob *sigAlgParamsOut)
{
    if ((self == NULL) || (sigAlgParamsOut == NULL)) {
        LOGE("Invalid input parameter.");
        return HCF_INVALID_PARAMS;
    }
    if (!IsClassMatch((HcfObjectBase *)self, GetX509CertificateClass())) {
        LOGE("Class is not match.");
        return HCF_INVALID_PARAMS;
    }
    return ((HcfX509CertificateImpl *)self)->spiObj->engineGetSignatureAlgParams(
        ((HcfX509CertificateImpl *)self)->spiObj, sigAlgParamsOut);
}

static HcfResult GetKeyUsage(HcfX509Certificate *self, HcfBlob *boolArr)
{
    if ((self == NULL) || (boolArr == NULL)) {
        LOGE("Invalid input parameter.");
        return HCF_INVALID_PARAMS;
    }
    if (!IsClassMatch((HcfObjectBase *)self, GetX509CertificateClass())) {
        LOGE("Class is not match.");
        return HCF_INVALID_PARAMS;
    }
    return ((HcfX509CertificateImpl *)self)->spiObj->engineGetKeyUsage(
        ((HcfX509CertificateImpl *)self)->spiObj, boolArr);
}

static HcfResult GetExtKeyUsage(HcfX509Certificate *self, HcfArray *keyUsageOut)
{
    if ((self == NULL) || (keyUsageOut == NULL)) {
        LOGE("Invalid input parameter.");
        return HCF_INVALID_PARAMS;
    }
    if (!IsClassMatch((HcfObjectBase *)self, GetX509CertificateClass())) {
        LOGE("Class is not match.");
        return HCF_INVALID_PARAMS;
    }
    return ((HcfX509CertificateImpl *)self)->spiObj->engineGetExtKeyUsage(
        ((HcfX509CertificateImpl *)self)->spiObj, keyUsageOut);
}

static int32_t GetBasicConstraints(HcfX509Certificate *self)
{
    if (self == NULL) {
        LOGE("Invalid input parameter.");
        return INVALID_CONSTRAINTS_LEN;
    }
    if (!IsClassMatch((HcfObjectBase *)self, GetX509CertificateClass())) {
        LOGE("Class is not match.");
        return INVALID_CONSTRAINTS_LEN;
    }
    return ((HcfX509CertificateImpl *)self)->spiObj->engineGetBasicConstraints(
        ((HcfX509CertificateImpl *)self)->spiObj);
}

static HcfResult GetSubjectAltNames(HcfX509Certificate *self, HcfArray *outName)
{
    if ((self == NULL) || (outName == NULL)) {
        LOGE("Invalid input parameter.");
        return HCF_INVALID_PARAMS;
    }
    if (!IsClassMatch((HcfObjectBase *)self, GetX509CertificateClass())) {
        LOGE("Class is not match.");
        return HCF_INVALID_PARAMS;
    }
    return ((HcfX509CertificateImpl *)self)->spiObj->engineGetSubjectAltNames(
        ((HcfX509CertificateImpl *)self)->spiObj, outName);
}

static HcfResult GetIssuerAltNames(HcfX509Certificate *self, HcfArray *outName)
{
    if ((self == NULL) || (outName == NULL)) {
        LOGE("Invalid input parameter.");
        return HCF_INVALID_PARAMS;
    }
    if (!IsClassMatch((HcfObjectBase *)self, GetX509CertificateClass())) {
        LOGE("Class is not match.");
        return HCF_INVALID_PARAMS;
    }
    return ((HcfX509CertificateImpl *)self)->spiObj->engineGetIssuerAltNames(
        ((HcfX509CertificateImpl *)self)->spiObj, outName);
}

HcfResult HcfX509CertificateCreate(const HcfEncodingBlob *inStream, HcfX509Certificate **returnObj)
{
    if ((inStream == NULL) || (inStream->len > HCF_MAX_BUFFER_LEN) || (returnObj == NULL)) {
        return HCF_INVALID_PARAMS;
    }
    const HcfX509CertificateFuncSet *funcSet = FindAbility("X509");
    if (funcSet == NULL) {
        return HCF_NOT_SUPPORT;
    }
    HcfX509CertificateSpi *spiObj = NULL;
    HcfResult res = funcSet->createFunc(inStream, &spiObj);
    if (res != HCF_SUCCESS) {
        LOGE("Failed to create spi object!");
        return res;
    }
    HcfX509CertificateImpl *x509CertImpl = (HcfX509CertificateImpl *)HcfMalloc(sizeof(HcfX509CertificateImpl), 0);
    if (x509CertImpl == NULL) {
        LOGE("Failed to allocate x509CertImpl memory!");
        return HCF_ERR_MALLOC;
    }
    x509CertImpl->base.base.base.getClass = GetX509CertificateClass;
    x509CertImpl->base.base.base.destroy = DestroyX509Certificate;
    x509CertImpl->base.base.verify = Verify;
    x509CertImpl->base.base.getEncoded = GetEncoded;
    x509CertImpl->base.base.getPublicKey = GetPublicKey;
    x509CertImpl->base.checkValidityWithDate = CheckValidityWithDate;
    x509CertImpl->base.getVersion = GetVersion;
    x509CertImpl->base.getSerialNumber = GetSerialNumber;
    x509CertImpl->base.getIssuerName = GetIssuerName;
    x509CertImpl->base.getSubjectName = GetSubjectName;
    x509CertImpl->base.getNotBeforeTime = GetNotBeforeTime;
    x509CertImpl->base.getNotAfterTime = GetNotAfterTime;
    x509CertImpl->base.getSignature = GetSignature;
    x509CertImpl->base.getSignatureAlgName = GetSignatureAlgName;
    x509CertImpl->base.getSignatureAlgOid = GetSignatureAlgOid;
    x509CertImpl->base.getSignatureAlgParams = GetSignatureAlgParams;
    x509CertImpl->base.getKeyUsage = GetKeyUsage;
    x509CertImpl->base.getExtKeyUsage = GetExtKeyUsage;
    x509CertImpl->base.getBasicConstraints = GetBasicConstraints;
    x509CertImpl->base.getSubjectAltNames = GetSubjectAltNames;
    x509CertImpl->base.getIssuerAltNames = GetIssuerAltNames;

    x509CertImpl->spiObj = spiObj;
    *returnObj = (HcfX509Certificate *)x509CertImpl;
    return HCF_SUCCESS;
}