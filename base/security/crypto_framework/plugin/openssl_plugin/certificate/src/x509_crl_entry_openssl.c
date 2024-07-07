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

#include"x509_crl_entry_openssl.h"

#include "securec.h"

#include <openssl/x509.h>
#include <openssl/bio.h>

#include "config.h"
#include "hcf_string.h"
#include "log.h"
#include "memory.h"
#include "openssl_common.h"
#include "utils.h"
#include "x509_crl.h"
#include "x509_crl_entry.h"
#include "x509_crl_openssl.h"

#define OPENSSL_ERROR_SERIAL_NUMBER (-1)

typedef struct {
    HcfX509CrlEntry base;
    X509_REVOKED *rev;
    HcfBlob *certIssuer;
} HcfX509CRLEntryOpensslImpl;

static const char *GetClass(void)
{
    return "HcfX509CRLEntryOpensslImpl.HcfX509CrlEntry";
}

static X509_REVOKED *GetSelfRev(const HcfX509CrlEntry *self)
{
    if (!IsClassMatch((HcfObjectBase *)self, GetClass())) {
        LOGE("Input wrong class type!");
        return NULL;
    }
    return ((HcfX509CRLEntryOpensslImpl *)self)->rev;
}

static HcfResult GetEncoded(HcfX509CrlEntry *self, HcfEncodingBlob *encodedOut)
{
    if ((self == NULL) || (encodedOut == NULL)) {
        LOGE("Invalid Paramas for calling GetEncoded!");
        return HCF_INVALID_PARAMS;
    }
    X509_REVOKED *rev = GetSelfRev(self);
    if (rev == NULL) {
        LOGE("Rev is null!");
        return HCF_INVALID_PARAMS;
    }
    unsigned char *out = NULL;
    int32_t length = i2d_X509_REVOKED(rev, &out);
    if (length <= 0) {
        LOGE("Do i2d_X509_REVOKED fail!");
        HcfPrintOpensslError();
        return HCF_ERR_CRYPTO_OPERATION;
    }
    encodedOut->data = (uint8_t *)HcfMalloc(length, 0);
    if (encodedOut->data == NULL) {
        LOGE("Failed to malloc for encodedOut!");
        OPENSSL_free(out);
        return HCF_ERR_MALLOC;
    }
    (void)memcpy_s(encodedOut->data, length, out, length);
    encodedOut->len = length;
    encodedOut->encodingFormat = HCF_FORMAT_DER;
    OPENSSL_free(out);
    return HCF_SUCCESS;
}

static long GetSerialNumber(HcfX509CrlEntry *self)
{
    if (self == NULL) {
        LOGE("Invalid Paramas for calling GetSerialNumber!");
        return OPENSSL_ERROR_SERIAL_NUMBER;
    }
    X509_REVOKED *rev = GetSelfRev(self);
    if (rev == NULL) {
        LOGE("Rev is null!");
        return OPENSSL_ERROR_SERIAL_NUMBER;
    }
    const ASN1_INTEGER *serialNumber = X509_REVOKED_get0_serialNumber(rev);
    if (serialNumber == NULL) {
        LOGE("Get serial number fail!");
        HcfPrintOpensslError();
        return OPENSSL_ERROR_SERIAL_NUMBER;
    }
    return ASN1_INTEGER_get(serialNumber);
}

static HcfResult GetCertIssuer(HcfX509CrlEntry *self, HcfBlob *encodedOut)
{
    if ((self == NULL) || (encodedOut == NULL)) {
        LOGE("Invalid Paramas for calling GetCertIssuer!");
        return HCF_INVALID_PARAMS;
    }
    if (!IsClassMatch((HcfObjectBase *)self, GetClass())) {
        LOGE("Input wrong class type!");
        return HCF_INVALID_PARAMS;
    }
    HcfBlob *certIssuer = ((HcfX509CRLEntryOpensslImpl *)self)->certIssuer;
    if (!IsBlobValid(certIssuer)) {
        LOGE("Get certIssuer fail! No certIssuer in CRL entry.");
        return HCF_NOT_SUPPORT;
    }
    uint32_t length = certIssuer->len;
    encodedOut->data = (uint8_t *)HcfMalloc(length, 0);
    if (encodedOut->data == NULL) {
        LOGE("Failed to malloc for encodedOut!");
        return HCF_ERR_MALLOC;
    }
    (void)memcpy_s(encodedOut->data, length, certIssuer->data, length);
    encodedOut->len = length;
    return HCF_SUCCESS;
}

static HcfResult GetRevocationDate(HcfX509CrlEntry *self, HcfBlob *out)
{
    if ((self == NULL) || (out == NULL)) {
        LOGE("invalid Paramas for calling GetRevocationDate!");
        return HCF_INVALID_PARAMS;
    }
    X509_REVOKED *rev = GetSelfRev(self);
    if (rev == NULL) {
        LOGE("Rev is null!");
        return HCF_INVALID_PARAMS;
    }
    const ASN1_TIME *time = X509_REVOKED_get0_revocationDate(rev);
    if (time == NULL) {
        LOGE("Get revocation date fail!");
        HcfPrintOpensslError();
        return HCF_ERR_CRYPTO_OPERATION;
    }
    const char *revTime = (const char *)(time->data);
    if ((revTime == NULL) || (strlen(revTime) > HCF_MAX_STR_LEN)) {
        LOGE("Get revocation date from ASN1_TIME fail!");
        return HCF_ERR_CRYPTO_OPERATION;
    }
    uint32_t length = strlen(revTime) + 1;
    out->data = (uint8_t *)HcfMalloc(length, 0);
    if (out->data == NULL) {
        LOGE("Failed to malloc for revTime!");
        return HCF_ERR_MALLOC;
    }
    (void)memcpy_s(out->data, length, revTime, length);
    out->len = length;
    return HCF_SUCCESS;
}

static HcfResult DeepCopyCertIssuer(HcfX509CRLEntryOpensslImpl *returnCRLEntry, HcfBlob *certIssuer)
{
    returnCRLEntry->certIssuer = (HcfBlob *)HcfMalloc(sizeof(HcfBlob), 0);
    if (returnCRLEntry->certIssuer == NULL) {
        LOGE("Failed to malloc certIssuer!");
        return HCF_ERR_MALLOC;
    }
    size_t len = certIssuer->len;
    returnCRLEntry->certIssuer->len = len;
    returnCRLEntry->certIssuer->data = (uint8_t *)HcfMalloc(len, 0);
    if (returnCRLEntry->certIssuer->data == NULL) {
        LOGE("Failed to malloc certIssuer data!");
        return HCF_ERR_MALLOC;
    }
    (void)memcpy_s(returnCRLEntry->certIssuer->data, len, certIssuer->data, len);
    return HCF_SUCCESS;
}

static void Destroy(HcfObjectBase *self)
{
    if (self == NULL) {
        LOGE("Invalid Paramas!");
        return;
    }
    if (!IsClassMatch((HcfObjectBase *)self, GetClass())) {
        LOGE("Input wrong class type!");
        return;
    }
    HcfX509CRLEntryOpensslImpl *realCrlEntry = (HcfX509CRLEntryOpensslImpl *)self;
    if (realCrlEntry->rev != NULL) {
        X509_REVOKED_free(realCrlEntry->rev);
        realCrlEntry->rev = NULL;
    }
    if (realCrlEntry->certIssuer != NULL) {
        HcfFree(realCrlEntry->certIssuer->data);
        realCrlEntry->certIssuer->data = NULL;
        HcfFree(realCrlEntry->certIssuer);
        realCrlEntry->certIssuer = NULL;
    }
    HcfFree(realCrlEntry);
}

HcfResult HcfCX509CRLEntryCreate(X509_REVOKED *rev, HcfX509CrlEntry **crlEntryOut, HcfBlob *certIssuer)
{
    if ((rev == NULL) || (crlEntryOut == NULL) || certIssuer == NULL) {
        LOGE("Invalid Paramas!");
        return HCF_INVALID_PARAMS;
    }
    HcfX509CRLEntryOpensslImpl *returnCRLEntry = (HcfX509CRLEntryOpensslImpl *)HcfMalloc(
        sizeof(HcfX509CRLEntryOpensslImpl), 0);
    if (returnCRLEntry == NULL) {
        LOGE("Failed to malloc for x509 entry instance!");
        return HCF_ERR_MALLOC;
    }

    X509_REVOKED *tmp = X509_REVOKED_dup(rev);
    if (tmp == NULL) {
        HcfFree(returnCRLEntry);
        LOGE("Failed to dup x509 revoked");
        return HCF_ERR_MALLOC;
    }
    returnCRLEntry->rev = tmp;
    returnCRLEntry->certIssuer = NULL;
    returnCRLEntry->base.base.getClass = GetClass;
    returnCRLEntry->base.base.destroy = Destroy;
    returnCRLEntry->base.getEncoded = GetEncoded;
    returnCRLEntry->base.getSerialNumber = GetSerialNumber;
    returnCRLEntry->base.getCertIssuer = GetCertIssuer;
    returnCRLEntry->base.getRevocationDate = GetRevocationDate;
    if (DeepCopyCertIssuer(returnCRLEntry, certIssuer) != HCF_SUCCESS) {
        LOGI("No cert issuer find or deep copy cert issuer fail!");
    }
    *crlEntryOut = (HcfX509CrlEntry *)returnCRLEntry;
    return HCF_SUCCESS;
}
