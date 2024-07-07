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

#include "x509_crl.h"

#include "securec.h"

#include "config.h"
#include "log.h"
#include "memory.h"
#include "utils.h"
#include "x509_crl.h"
#include "x509_crl_openssl.h"
#include "x509_crl_spi.h"

#define HCF_X509_CRL_CLASS "HcfX509Crl"
#define OPENSSL_INVALID_VERSION (-1)

typedef HcfResult (*HcfX509CrlSpiCreateFunc)(const HcfEncodingBlob *, HcfX509CrlSpi **);

typedef struct {
    HcfX509Crl base;
    HcfX509CrlSpi *spiObj;
    const char *certType;
} HcfX509CrlImpl;

typedef struct {
    HcfX509CrlSpiCreateFunc createFunc;
} HcfX509CrlFuncSet;

typedef struct {
    char *certType;
    HcfX509CrlFuncSet funcSet;
} HcfCCertFactoryAbility;

static const char *GetX509CrlClass(void)
{
    return HCF_X509_CRL_CLASS;
}

static const HcfCCertFactoryAbility X509_CRL_ABILITY_SET[] = {
    { "X509", { HcfCX509CrlSpiCreate, } }
};

static const HcfX509CrlFuncSet *FindAbility(const char *certType)
{
    if (certType == NULL) {
        LOGE("CertType is null!");
        return NULL;
    }
    for (uint32_t i = 0; i < sizeof(X509_CRL_ABILITY_SET) / sizeof(HcfCCertFactoryAbility); i++) {
        if (strcmp(X509_CRL_ABILITY_SET[i].certType, certType) == 0) {
            return &(X509_CRL_ABILITY_SET[i].funcSet);
        }
    }
    LOGE("Cert not support! [cert]: %s", certType);
    return NULL;
}

static void DestroyX509Crl(HcfObjectBase *self)
{
    if (self == NULL) {
        LOGE("Invalid input parameter.");
        return;
    }
    if (!IsClassMatch(self, GetX509CrlClass())) {
        LOGE("Class is not match.");
        return;
    }
    HcfX509CrlImpl *impl = (HcfX509CrlImpl *)self;
    HcfObjDestroy(impl->spiObj);
    HcfFree(impl);
}

static const char *GetType(HcfCrl *self)
{
    if (self == NULL) {
        LOGE("Invalid input parameter.");
        return NULL;
    }
    if (!IsClassMatch((HcfObjectBase *)self, GetX509CrlClass())) {
        LOGE("Class is not match.");
        return NULL;
    }
    return ((HcfX509CrlImpl *)self)->spiObj->engineGetType(
        ((HcfX509CrlImpl *)self)->spiObj);
}

static bool IsRevoked(HcfCrl *self, const HcfCertificate *cert)
{
    if ((self == NULL) || (cert == NULL)) {
        LOGE("Invalid input parameter.");
        return false;
    }
    if (!IsClassMatch((HcfObjectBase *)self, GetX509CrlClass())) {
        LOGE("Class is not match.");
        return false;
    }
    return ((HcfX509CrlImpl *)self)->spiObj->engineIsRevoked(
        ((HcfX509CrlImpl *)self)->spiObj, cert);
}

static HcfResult Verify(HcfX509Crl *self, HcfPubKey *key)
{
    if ((self == NULL) || (key == NULL)) {
        LOGE("Invalid input parameter.");
        return HCF_INVALID_PARAMS;
    }
    if (!IsClassMatch((HcfObjectBase *)self, GetX509CrlClass())) {
        LOGE("Class is not match.");
        return HCF_INVALID_PARAMS;
    }
    return ((HcfX509CrlImpl *)self)->spiObj->engineVerify(
        ((HcfX509CrlImpl *)self)->spiObj, key);
}

static HcfResult GetEncoded(HcfX509Crl *self, HcfEncodingBlob *encodedByte)
{
    if ((self == NULL) || (encodedByte == NULL)) {
        LOGE("Invalid input parameter.");
        return HCF_INVALID_PARAMS;
    }
    if (!IsClassMatch((HcfObjectBase *)self, GetX509CrlClass())) {
        LOGE("Class is not match.");
        return HCF_INVALID_PARAMS;
    }
    return ((HcfX509CrlImpl *)self)->spiObj->engineGetEncoded(
        ((HcfX509CrlImpl *)self)->spiObj, encodedByte);
}

static long GetVersion(HcfX509Crl *self)
{
    if (self == NULL) {
        LOGE("Invalid input parameter.");
        return OPENSSL_INVALID_VERSION;
    }
    if (!IsClassMatch((HcfObjectBase *)self, GetX509CrlClass())) {
        LOGE("Class is not match.");
        return OPENSSL_INVALID_VERSION;
    }
    return ((HcfX509CrlImpl *)self)->spiObj->engineGetVersion(
        ((HcfX509CrlImpl *)self)->spiObj);
}

static HcfResult GetIssuerName(HcfX509Crl *self, HcfBlob *out)
{
    if ((self == NULL) || (out == NULL)) {
        LOGE("Invalid input parameter.");
        return HCF_INVALID_PARAMS;
    }
    if (!IsClassMatch((HcfObjectBase *)self, GetX509CrlClass())) {
        LOGE("Class is not match.");
        return HCF_INVALID_PARAMS;
    }
    return ((HcfX509CrlImpl *)self)->spiObj->engineGetIssuerName(
        ((HcfX509CrlImpl *)self)->spiObj, out);
}

static HcfResult GetLastUpdate(HcfX509Crl *self, HcfBlob *out)
{
    if ((self == NULL) || (out == NULL)) {
        LOGE("Invalid input parameter.");
        return HCF_INVALID_PARAMS;
    }
    if (!IsClassMatch((HcfObjectBase *)self, GetX509CrlClass())) {
        LOGE("Class is not match.");
        return HCF_INVALID_PARAMS;
    }
    return ((HcfX509CrlImpl *)self)->spiObj->engineGetLastUpdate(
        ((HcfX509CrlImpl *)self)->spiObj, out);
}

static HcfResult GetNextUpdate(HcfX509Crl *self, HcfBlob *out)
{
    if ((self == NULL) || (out == NULL)) {
        LOGE("Invalid input parameter.");
        return HCF_INVALID_PARAMS;
    }
    if (!IsClassMatch((HcfObjectBase *)self, GetX509CrlClass())) {
        LOGE("Class is not match.");
        return HCF_INVALID_PARAMS;
    }
    return ((HcfX509CrlImpl *)self)->spiObj->engineGetNextUpdate(
        ((HcfX509CrlImpl *)self)->spiObj, out);
}

static HcfResult GetRevokedCert(HcfX509Crl *self, long serialNumber, HcfX509CrlEntry **entryOut)
{
    if (self == NULL || entryOut == NULL) {
        LOGE("Invalid input parameter.");
        return HCF_INVALID_PARAMS;
    }
    if (!IsClassMatch((HcfObjectBase *)self, GetX509CrlClass())) {
        LOGE("Class is not match.");
        return HCF_INVALID_PARAMS;
    }
    return ((HcfX509CrlImpl *)self)->spiObj->engineGetRevokedCert(
        ((HcfX509CrlImpl *)self)->spiObj, serialNumber, entryOut);
}

static HcfResult GetRevokedCertWithCert(HcfX509Crl *self, HcfX509Certificate *cert, HcfX509CrlEntry **entryOut)
{
    if ((self == NULL) || (cert == NULL) || (entryOut == NULL)) {
        LOGE("Invalid input parameter.");
        return HCF_INVALID_PARAMS;
    }
    if (!IsClassMatch((HcfObjectBase *)self, GetX509CrlClass())) {
        LOGE("Class is not match.");
        return HCF_INVALID_PARAMS;
    }
    return ((HcfX509CrlImpl *)self)->spiObj->engineGetRevokedCertWithCert(
        ((HcfX509CrlImpl *)self)->spiObj, cert, entryOut);
}

static HcfResult GetRevokedCerts(HcfX509Crl *self, HcfArray *entrysOut)
{
    if ((self == NULL) || (entrysOut == NULL)) {
        LOGE("Invalid input parameter.");
        return HCF_INVALID_PARAMS;
    }
    if (!IsClassMatch((HcfObjectBase *)self, GetX509CrlClass())) {
        LOGE("Class is not match.");
        return HCF_INVALID_PARAMS;
    }
    return ((HcfX509CrlImpl *)self)->spiObj->engineGetRevokedCerts(
        ((HcfX509CrlImpl *)self)->spiObj, entrysOut);
}

static HcfResult GetTbsInfo(HcfX509Crl *self, HcfBlob *tbsCertListOut)
{
    if ((self == NULL) || (tbsCertListOut == NULL)) {
        LOGE("Invalid input parameter.");
        return HCF_INVALID_PARAMS;
    }
    if (!IsClassMatch((HcfObjectBase *)self, GetX509CrlClass())) {
        LOGE("Class is not match.");
        return HCF_INVALID_PARAMS;
    }
    return ((HcfX509CrlImpl *)self)->spiObj->engineGetTbsInfo(
        ((HcfX509CrlImpl *)self)->spiObj, tbsCertListOut);
}

static HcfResult GetSignature(HcfX509Crl *self, HcfBlob *signature)
{
    if ((self == NULL) || (signature == NULL)) {
        LOGE("Invalid input parameter.");
        return HCF_INVALID_PARAMS;
    }
    if (!IsClassMatch((HcfObjectBase *)self, GetX509CrlClass())) {
        LOGE("Class is not match.");
        return HCF_INVALID_PARAMS;
    }
    return ((HcfX509CrlImpl *)self)->spiObj->engineGetSignature(
        ((HcfX509CrlImpl *)self)->spiObj, signature);
}

static HcfResult GetSignatureAlgName(HcfX509Crl *self, HcfBlob *out)
{
    if ((self == NULL) || (out == NULL)) {
        LOGE("Invalid input parameter.");
        return HCF_INVALID_PARAMS;
    }
    if (!IsClassMatch((HcfObjectBase *)self, GetX509CrlClass())) {
        LOGE("Class is not match.");
        return HCF_INVALID_PARAMS;
    }
    return ((HcfX509CrlImpl *)self)->spiObj->engineGetSignatureAlgName(
        ((HcfX509CrlImpl *)self)->spiObj, out);
}

static HcfResult GetSignatureAlgOid(HcfX509Crl *self, HcfBlob *out)
{
    if ((self == NULL) || (out == NULL)) {
        LOGE("Invalid input parameter.");
        return HCF_INVALID_PARAMS;
    }
    if (!IsClassMatch((HcfObjectBase *)self, GetX509CrlClass())) {
        LOGE("Class is not match.");
        return HCF_INVALID_PARAMS;
    }
    return ((HcfX509CrlImpl *)self)->spiObj->engineGetSignatureAlgOid(
        ((HcfX509CrlImpl *)self)->spiObj, out);
}

static HcfResult GetSignatureAlgParams(HcfX509Crl *self, HcfBlob *sigAlgParamOut)
{
    if ((self == NULL) || (sigAlgParamOut == NULL)) {
        LOGE("Invalid input parameter.");
        return HCF_INVALID_PARAMS;
    }
    if (!IsClassMatch((HcfObjectBase *)self, GetX509CrlClass())) {
        LOGE("Class is not match.");
        return HCF_INVALID_PARAMS;
    }
    return ((HcfX509CrlImpl *)self)->spiObj->engineGetSignatureAlgParams(
        ((HcfX509CrlImpl *)self)->spiObj, sigAlgParamOut);
}

HcfResult HcfX509CrlCreate(const HcfEncodingBlob *inStream, HcfX509Crl **returnObj)
{
    if ((inStream == NULL) || (inStream->data == NULL) || (inStream->len > HCF_MAX_BUFFER_LEN) || (returnObj == NULL)) {
        LOGE("FuncSet is null!");
        return HCF_INVALID_PARAMS;
    }
    const HcfX509CrlFuncSet *funcSet = FindAbility("X509");
    if (funcSet == NULL) {
        return HCF_NOT_SUPPORT;
    }
    HcfX509CrlSpi *spiObj = NULL;
    HcfResult res = funcSet->createFunc(inStream, &spiObj);
    if (res != HCF_SUCCESS) {
        LOGE("Failed to create spi object!");
        return res;
    }
    HcfX509CrlImpl *x509CertImpl = (HcfX509CrlImpl *)HcfMalloc(sizeof(HcfX509CrlImpl), 0);
    if (x509CertImpl == NULL) {
        LOGE("Failed to allocate x509CertImpl memory!");
        return HCF_ERR_MALLOC;
    }
    x509CertImpl->base.base.base.getClass = GetX509CrlClass;
    x509CertImpl->base.base.base.destroy = DestroyX509Crl;
    x509CertImpl->base.base.getType = GetType;
    x509CertImpl->base.base.isRevoked = IsRevoked;
    x509CertImpl->base.verify = Verify;
    x509CertImpl->base.getEncoded = GetEncoded;
    x509CertImpl->base.getVersion = GetVersion;
    x509CertImpl->base.getIssuerName = GetIssuerName;
    x509CertImpl->base.getLastUpdate = GetLastUpdate;
    x509CertImpl->base.getNextUpdate = GetNextUpdate;
    x509CertImpl->base.getRevokedCert = GetRevokedCert;
    x509CertImpl->base.getRevokedCertWithCert = GetRevokedCertWithCert;
    x509CertImpl->base.getRevokedCerts = GetRevokedCerts;
    x509CertImpl->base.getTbsInfo = GetTbsInfo;
    x509CertImpl->base.getSignature = GetSignature;
    x509CertImpl->base.getSignatureAlgName = GetSignatureAlgName;
    x509CertImpl->base.getSignatureAlgOid = GetSignatureAlgOid;
    x509CertImpl->base.getSignatureAlgParams = GetSignatureAlgParams;
    x509CertImpl->spiObj = spiObj;
    *returnObj = (HcfX509Crl *)x509CertImpl;
    return HCF_SUCCESS;
}