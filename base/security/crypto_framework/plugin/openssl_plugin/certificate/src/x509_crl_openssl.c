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

#include "x509_crl_openssl.h"

#include "securec.h"

#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/pkcs7.h>
#include <openssl/rsa.h>
#include <openssl/x509.h>

#include "config.h"
#include "fwk_class.h"
#include "hcf_string.h"
#include "log.h"
#include "memory.h"
#include "openssl_class.h"
#include "openssl_common.h"
#include "utils.h"
#include "x509_crl.h"
#include "x509_crl_entry_openssl.h"
#include "x509_crl_spi.h"

typedef struct {
    HcfX509CrlSpi base;
    X509_CRL *crl;
    HcfBlob *certIssuer;
} HcfX509CRLOpensslImpl;

#define OPENSSL_INVALID_VERSION (-1)
#define OPENSSL_ERROR 0
#define TYPE_NAME "X509"
#define OID_LENGTH 128
#define MAX_REV_NUM 256
#define MAX_SIGNATURE_LEN 8192

static const char *GetClass(void)
{
    return X509_CRL_OPENSSL_CLASS;
}

static const char *GetType(HcfX509CrlSpi *self)
{
    if (self == NULL) {
        LOGE("Invalid Paramas!");
        return NULL;
    }
    if (!IsClassMatch((HcfObjectBase *)self, GetClass())) {
        LOGE("Input wrong class type!");
        return NULL;
    }
    return TYPE_NAME;
}

static X509_CRL *GetCrl(HcfX509CrlSpi *self)
{
    if (!IsClassMatch((HcfObjectBase *)self, GetClass())) {
        LOGE("Input wrong class type!");
        return NULL;
    }
    return ((HcfX509CRLOpensslImpl *)self)->crl;
}

static X509 *GetX509FromCertificate(const HcfCertificate *cert)
{
    if (!IsClassMatch((HcfObjectBase *)cert, HCF_X509_CERTIFICATE_CLASS)) {
        LOGE("Input wrong openssl class type!");
        return NULL;
    }
    HcfX509CertificateImpl *impl = (HcfX509CertificateImpl *)cert;
    if (!IsClassMatch((HcfObjectBase *)(impl->spiObj), X509_CERT_OPENSSL_CLASS)) {
        LOGE("Input wrong openssl class type!");
        return NULL;
    }
    HcfOpensslX509Cert *realCert = (HcfOpensslX509Cert *)(impl->spiObj);
    return realCert->x509;
}

static bool IsRevoked(HcfX509CrlSpi *self, const HcfCertificate *cert)
{
    if ((self == NULL) || (cert == NULL)) {
        LOGE("Invalid Paramas!");
        return false;
    }
    if (!IsClassMatch((HcfObjectBase *)self, GetClass())) {
        LOGE("Input wrong class type!");
        return false;
    }
    X509 *certOpenssl = GetX509FromCertificate(cert);
    if (certOpenssl == NULL) {
        LOGE("Input Cert is wrong !");
        return false;
    }
    X509_CRL *crl = ((HcfX509CRLOpensslImpl *)self)->crl;
    if (crl == NULL) {
        LOGE("crl is null!");
        return false;
    }
    X509_REVOKED *rev = NULL;
    int32_t res = X509_CRL_get0_by_cert(crl, &rev, certOpenssl);
    return (res != 0);
}

static HcfResult GetEncoded(HcfX509CrlSpi *self, HcfEncodingBlob *encodedOut)
{
    if ((self == NULL) || (encodedOut == NULL)) {
        LOGE("Invalid Paramas!");
        return HCF_INVALID_PARAMS;
    }
    if (!IsClassMatch((HcfObjectBase *)self, GetClass())) {
        LOGE("Input wrong class type!");
        return HCF_INVALID_PARAMS;
    }
    unsigned char *out = NULL;
    X509_CRL *crl = ((HcfX509CRLOpensslImpl *)self)->crl;
    if (crl == NULL) {
        LOGE("crl is null!");
        return HCF_INVALID_PARAMS;
    }
    int32_t length = i2d_X509_CRL(crl, &out);
    if (length <= 0) {
        LOGE("Do i2d_X509_CRL fail!");
        HcfPrintOpensslError();
        return HCF_ERR_CRYPTO_OPERATION;
    }
    encodedOut->data = (uint8_t *)HcfMalloc(length, 0);
    if (encodedOut->data == NULL) {
        LOGE("Failed to malloc for crl encoded data!");
        OPENSSL_free(out);
        return HCF_ERR_MALLOC;
    }
    (void)memcpy_s(encodedOut->data, length, out, length);
    OPENSSL_free(out);
    encodedOut->len = length;
    encodedOut->encodingFormat = HCF_FORMAT_DER;
    return HCF_SUCCESS;
}

static HcfResult Verify(HcfX509CrlSpi *self, HcfPubKey *key)
{
    if ((self == NULL) || (key == NULL)) {
        LOGE("Invalid Paramas!");
        return HCF_INVALID_PARAMS;
    }
    if (!IsClassMatch((HcfObjectBase *)self, GetClass()) ||
        (!IsClassMatch((HcfObjectBase *)key, OPENSSL_RSA_PUBKEY_CLASS))) {
        LOGE("Input wrong class type!");
        return HCF_INVALID_PARAMS;
    }
    RSA *rsaPubkey = ((HcfOpensslRsaPubKey *)key)->pk;
    if (rsaPubkey == NULL) {
        LOGE("rsaPubkey is null!");
        return HCF_INVALID_PARAMS;
    }
    EVP_PKEY *pubKey = EVP_PKEY_new();
    if (pubKey == NULL) {
        LOGE("pubKey is null!");
        HcfPrintOpensslError();
        return HCF_ERR_CRYPTO_OPERATION;
    }

    HcfResult ret = HCF_SUCCESS;
    do {
        if (EVP_PKEY_set1_RSA(pubKey, rsaPubkey) <= 0) {
            LOGE("Do EVP_PKEY_assign_RSA fail!");
            HcfPrintOpensslError();
            ret = HCF_ERR_CRYPTO_OPERATION;
            break;
        }

        X509_CRL *crl = ((HcfX509CRLOpensslImpl *)self)->crl;
        if (crl == NULL) {
            LOGE("crl is null!");
            ret = HCF_INVALID_PARAMS;
            break;
        }

        int32_t res = X509_CRL_verify(crl, pubKey);
        if (res != HCF_OPENSSL_SUCCESS) {
            LOGE("Verify fail!");
            HcfPrintOpensslError();
            ret = HCF_ERR_CRYPTO_OPERATION;
            break;
        }
    } while (0);

    EVP_PKEY_free(pubKey);
    return ret;
}

static long GetVersion(HcfX509CrlSpi *self)
{
    if (self == NULL) {
        LOGE("Invalid Paramas!");
        return OPENSSL_INVALID_VERSION;
    }
    if (!IsClassMatch((HcfObjectBase *)self, GetClass())) {
        LOGE("Input wrong class type!");
        return OPENSSL_INVALID_VERSION;
    }
    X509_CRL *crl = ((HcfX509CRLOpensslImpl *)self)->crl;
    if (crl == NULL) {
        LOGE("crl is null!");
        return OPENSSL_INVALID_VERSION;
    }
    return X509_CRL_get_version(crl) + 1;
}

static HcfResult GetIssuerName(HcfX509CrlSpi *self, HcfBlob *out)
{
    if ((self == NULL) || (out == NULL)) {
        LOGE("Invalid Paramas for calling GetIssuerName!");
        return HCF_INVALID_PARAMS;
    }
    X509_CRL *crl = GetCrl(self);
    if (crl == NULL) {
        LOGE("crl is null!");
        return HCF_INVALID_PARAMS;
    }
    X509_NAME *x509Name = X509_CRL_get_issuer(crl);
    if (x509Name == NULL) {
        LOGE("Get Issuer DN fail!");
        HcfPrintOpensslError();
        return HCF_ERR_CRYPTO_OPERATION;
    }
    const char *issuer = X509_NAME_oneline(x509Name, NULL, 0);
    if ((issuer == NULL) || (strlen(issuer) > HCF_MAX_STR_LEN)) {
        LOGE("X509Name convert char fail or issuer name is too long!");
        HcfPrintOpensslError();
        return HCF_ERR_CRYPTO_OPERATION;
    }
    uint32_t length = strlen(issuer) + 1;
    out->data = (uint8_t *)HcfMalloc(length, 0);
    if (out->data == NULL) {
        LOGE("Failed to malloc for crl issuer data!");
        return HCF_ERR_MALLOC;
    }
    (void)memcpy_s(out->data, length, issuer, length);
    out->len = length;
    return HCF_SUCCESS;
}

static HcfResult SetCertIssuer(HcfX509CrlSpi *self)
{
    ((HcfX509CRLOpensslImpl *)self)->certIssuer = (HcfBlob *)HcfMalloc(sizeof(HcfBlob), 0);
    if (((HcfX509CRLOpensslImpl *)self)->certIssuer == NULL) {
        LOGE("Failed to malloc for certIssuer!");
        return HCF_ERR_MALLOC;
    }
    HcfResult res = GetIssuerName(self, ((HcfX509CRLOpensslImpl *)self)->certIssuer);
    if (res != HCF_SUCCESS) {
        HcfFree(((HcfX509CRLOpensslImpl *)self)->certIssuer);
        ((HcfX509CRLOpensslImpl *)self)->certIssuer = NULL;
    }
    return res;
}

static HcfResult GetLastUpdate(HcfX509CrlSpi *self, HcfBlob *out)
{
    if ((self == NULL) || (out == NULL)) {
        LOGE("Invalid Paramas for calling GetLastUpdate!");
        return HCF_INVALID_PARAMS;
    }
    X509_CRL *crl = GetCrl(self);
    if (crl == NULL) {
        LOGE("crl is null!");
        return HCF_INVALID_PARAMS;
    }
    const ASN1_TIME *time = X509_CRL_get0_lastUpdate(crl);
    if (time == NULL) {
        LOGE("Get this update time fail!");
        HcfPrintOpensslError();
        return HCF_ERR_CRYPTO_OPERATION;
    }
    const char *thisUpdate = (const char *)(time->data);
    if (thisUpdate == NULL || strlen(thisUpdate) > HCF_MAX_STR_LEN) {
        LOGE("ThisUpdate convert String fail, or thisUpdate is too long!");
        return HCF_ERR_CRYPTO_OPERATION;
    }
    uint32_t length = strlen(thisUpdate) + 1;
    out->data = (uint8_t *)HcfMalloc(length, 0);
    if (out->data == NULL) {
        LOGE("Failed to malloc for thisUpdate!");
        return HCF_ERR_MALLOC;
    }
    (void)memcpy_s(out->data, length, thisUpdate, length);
    out->len = length;
    return HCF_SUCCESS;
}

static HcfResult GetNextUpdate(HcfX509CrlSpi *self, HcfBlob *out)
{
    if ((self == NULL) || (out == NULL)) {
        LOGE("Invalid Paramas for calling GetNextUpdate!");
        return HCF_INVALID_PARAMS;
    }
    X509_CRL *crl = GetCrl(self);
    if (crl == NULL) {
        LOGE("crl is null!");
        return HCF_INVALID_PARAMS;
    }
    const ASN1_TIME *time = X509_CRL_get0_nextUpdate(crl);
    if (time == NULL) {
        LOGE("Get next update time fail!");
        HcfPrintOpensslError();
        return HCF_ERR_CRYPTO_OPERATION;
    }
    const char *nextUpdate = (const char *)(time->data);
    if ((nextUpdate == NULL) || (strlen(nextUpdate) > HCF_MAX_STR_LEN)) {
        LOGE("Get next update time is null, or nextUpdate is too long!");
        return HCF_ERR_CRYPTO_OPERATION;
    }
    uint32_t length = strlen(nextUpdate) + 1;
    out->data = (uint8_t *)HcfMalloc(length, 0);
    if (out->data == NULL) {
        LOGE("Failed to malloc for nextUpdate!");
        return HCF_ERR_MALLOC;
    }
    (void)memcpy_s(out->data, length, nextUpdate, length);
    out->len = length;
    return HCF_SUCCESS;
}

static HcfResult GetRevokedCert(HcfX509CrlSpi *self, long serialNumber, HcfX509CrlEntry **entryOut)
{
    if ((self == NULL) || (entryOut == NULL)) {
        LOGE("Invalid Paramas!");
        return HCF_INVALID_PARAMS;
    }
    X509_CRL *crl = GetCrl(self);
    if (crl == NULL) {
        LOGE("crl is null!");
        return HCF_INVALID_PARAMS;
    }
    ASN1_INTEGER *serial = ASN1_INTEGER_new();
    if (serial == NULL) {
        LOGE("Serial init fail!");
        HcfPrintOpensslError();
        return HCF_ERR_CRYPTO_OPERATION;
    }
    if (!ASN1_INTEGER_set(serial, serialNumber)) {
        LOGE("Set serial number fail!");
        HcfPrintOpensslError();
        ASN1_INTEGER_free(serial);
        return HCF_ERR_CRYPTO_OPERATION;
    }
    X509_REVOKED *rev = NULL;
    int32_t opensslRes = X509_CRL_get0_by_serial(crl, &rev, serial);
    ASN1_INTEGER_free(serial);
    if (opensslRes != HCF_OPENSSL_SUCCESS) {
        LOGE("Get revoked certificate fail, res : %d!", opensslRes);
        HcfPrintOpensslError();
        return HCF_ERR_CRYPTO_OPERATION;
    }
    HcfResult res = HcfCX509CRLEntryCreate(rev, entryOut, ((HcfX509CRLOpensslImpl *)self)->certIssuer);
    if (res != HCF_SUCCESS) {
        LOGE("X509 CRL entry create fail, res : %d!", res);
        return res;
    }
    return HCF_SUCCESS;
}

static HcfResult GetRevokedCertWithCert(HcfX509CrlSpi *self, HcfX509Certificate *cert,
    HcfX509CrlEntry **entryOut)
{
    if ((self == NULL) || (cert == NULL) || (entryOut == NULL)) {
        LOGE("Invalid Paramas!");
        return HCF_INVALID_PARAMS;
    }
    if (!IsClassMatch((HcfObjectBase *)self, GetClass())) {
        LOGE("Input wrong class type!");
        return HCF_INVALID_PARAMS;
    }
    X509 *certOpenssl = GetX509FromCertificate((HcfCertificate *)cert);
    if (certOpenssl == NULL) {
        LOGE("Input Cert is wrong !");
        return HCF_INVALID_PARAMS;
    }
    X509_CRL *crl = ((HcfX509CRLOpensslImpl *)self)->crl;
    if (crl == NULL) {
        LOGE("crl is null!");
        return HCF_INVALID_PARAMS;
    }
    X509_REVOKED *revokedRet = NULL;
    int32_t opensslRes = X509_CRL_get0_by_cert(crl, &revokedRet, certOpenssl);
    if (opensslRes != HCF_OPENSSL_SUCCESS) {
        LOGE("Get revoked certificate with cert fail, res : %d!", opensslRes);
        HcfPrintOpensslError();
        return HCF_ERR_CRYPTO_OPERATION;
    }
    HcfResult res = HcfCX509CRLEntryCreate(revokedRet, entryOut, ((HcfX509CRLOpensslImpl *)self)->certIssuer);
    if (res != HCF_SUCCESS) {
        LOGE("X509 CRL entry create fail, res : %d!", res);
        return res;
    }
    return HCF_SUCCESS;
}

static HcfResult DeepCopyRevokedCertificates(HcfX509CrlSpi *self, const STACK_OF(X509_REVOKED) *entrys,
    int32_t i, HcfArray *entrysOut)
{
    X509_REVOKED *rev = sk_X509_REVOKED_value(entrys, i);
    if (rev == NULL) {
        LOGE("sk_X509_REVOKED_value fail!");
        HcfPrintOpensslError();
        return HCF_ERR_CRYPTO_OPERATION;
    }
    HcfX509CrlEntry *crlEntry = NULL;
    HcfResult res = HcfCX509CRLEntryCreate(rev, &crlEntry, ((HcfX509CRLOpensslImpl *)self)->certIssuer);
    if (res != HCF_SUCCESS || crlEntry == NULL) {
        LOGE("X509 CRL entry create fail, res : %d!", res);
        return res;
    }
    entrysOut->data[i].data = (uint8_t *)crlEntry;
    entrysOut->data[i].len = sizeof(HcfX509CrlEntry);
    return HCF_SUCCESS;
}

static void DestroyCRLEntryArray(HcfArray *arr)
{
    if (arr == NULL) {
        LOGD("The input array is null, no need to free.");
        return;
    }
    for (uint32_t i = 0; i < arr->count; ++i) {
        if (arr->data[i].data == NULL) {
            continue;
        }
        HcfX509CrlEntry *crlEntry = (HcfX509CrlEntry *)(arr->data[i].data);
        crlEntry->base.destroy((HcfObjectBase *)crlEntry);
        arr->data[i].data = NULL;
        arr->data[i].len = 0;
    }
    HcfFree(arr->data);
    arr->data = NULL;
}

static HcfResult GetRevokedCerts(HcfX509CrlSpi *self, HcfArray *entrysOut)
{
    if ((self == NULL) || (entrysOut == NULL)) {
        LOGE("Invalid Paramas!");
        return HCF_INVALID_PARAMS;
    }
    X509_CRL *crl = GetCrl(self);
    if (crl == NULL) {
        LOGE("crl is null!");
        return HCF_INVALID_PARAMS;
    }
    STACK_OF(X509_REVOKED) *entrys = X509_CRL_get_REVOKED(crl);
    if (entrys == NULL) {
        LOGE("Get revoked certificates fail!");
        HcfPrintOpensslError();
        return HCF_ERR_CRYPTO_OPERATION;
    }
    int32_t revokedNum = sk_X509_REVOKED_num(entrys);
    if ((revokedNum <= 0) || (revokedNum > MAX_REV_NUM)) {
        LOGE("Get revoked invalid number!");
        HcfPrintOpensslError();
        return HCF_ERR_CRYPTO_OPERATION;
    }
    uint32_t blobSize = sizeof(HcfBlob) * revokedNum;
    entrysOut->data = (HcfBlob *)HcfMalloc(blobSize, 0);
    if (entrysOut->data == NULL) {
        LOGE("Failed to malloc for entrysOut array!");
        return HCF_ERR_MALLOC;
    }
    entrysOut->count = revokedNum;
    for (int32_t i = 0; i < revokedNum; i++) {
        if (DeepCopyRevokedCertificates(self, entrys, i, entrysOut) != HCF_SUCCESS) {
            LOGE("Falied to copy revoked certificates!");
            DestroyCRLEntryArray(entrysOut);
            return HCF_ERR_MALLOC;
        }
    }
    return HCF_SUCCESS;
}

static HcfResult GetTbsList(HcfX509CrlSpi *self, HcfBlob *tbsCertListOut)
{
    if ((self == NULL) || (tbsCertListOut == NULL)) {
        LOGE("Invalid Paramas!");
        return HCF_INVALID_PARAMS;
    }
    X509_CRL *crl = GetCrl(self);
    if (crl == NULL) {
        LOGE("crl is null!");
        return HCF_INVALID_PARAMS;
    }
    unsigned char *tbs = NULL;
    int32_t length = i2d_re_X509_CRL_tbs(crl, &tbs);
    if ((length <= 0) || (tbs == NULL)) {
        LOGE("Get TBS certList fail!");
        HcfPrintOpensslError();
        return HCF_ERR_CRYPTO_OPERATION;
    }
    tbsCertListOut->data = (uint8_t *)HcfMalloc(length, 0);
    if (tbsCertListOut->data == NULL) {
        LOGE("Failed to malloc for tbs!");
        OPENSSL_free(tbs);
        return HCF_ERR_MALLOC;
    }
    (void)memcpy_s(tbsCertListOut->data, length, tbs, length);
    OPENSSL_free(tbs);
    tbsCertListOut->len = length;
    return HCF_SUCCESS;
}

static HcfResult GetSignature(HcfX509CrlSpi *self, HcfBlob *signature)
{
    if ((self == NULL) || (signature == NULL)) {
        LOGE("Invalid Paramas!");
        return HCF_INVALID_PARAMS;
    }
    X509_CRL *crl = GetCrl(self);
    if (crl == NULL) {
        LOGE("crl is null!");
        return HCF_INVALID_PARAMS;
    }
    const ASN1_BIT_STRING *asn1Signature = NULL;
    X509_CRL_get0_signature(((HcfX509CRLOpensslImpl *)self)->crl, &asn1Signature, NULL);
    if (asn1Signature == NULL) {
        LOGE("Get signature is null!");
        HcfPrintOpensslError();
        return HCF_ERR_CRYPTO_OPERATION;
    }
    int32_t signatureLen = ASN1_STRING_length(asn1Signature);
    if (signatureLen <= 0) {
        LOGE("Get signature length is invalid!");
        HcfPrintOpensslError();
        return HCF_ERR_CRYPTO_OPERATION;
    }
    const unsigned char *signatureStr = ASN1_STRING_get0_data(asn1Signature);
    if ((signatureStr == NULL) || (signatureLen > MAX_SIGNATURE_LEN)) {
        LOGE("ASN1 get string fail, or signature length is too long!");
        HcfPrintOpensslError();
        return HCF_ERR_CRYPTO_OPERATION;
    }
    signature->data = (uint8_t *)HcfMalloc(signatureLen, 0);
    if (signature->data == NULL) {
        LOGE("Failed to malloc for signature!");
        return HCF_ERR_MALLOC;
    }
    (void)memcpy_s(signature->data, signatureLen, signatureStr, signatureLen);
    signature->len = signatureLen;
    return HCF_SUCCESS;
}

static HcfResult GetSignatureAlgOidInner(X509_CRL *crl, HcfBlob *oidOut)
{
    const X509_ALGOR *palg = NULL;
    X509_CRL_get0_signature(crl, NULL, &palg);
    if (palg == NULL) {
        LOGE("alg is null!");
        HcfPrintOpensslError();
        return HCF_ERR_CRYPTO_OPERATION;
    }
    const ASN1_OBJECT *oid = NULL;
    X509_ALGOR_get0(&oid, NULL, NULL, palg);
    if (oid == NULL) {
        LOGE("oid is null!");
        HcfPrintOpensslError();
        return HCF_ERR_CRYPTO_OPERATION;
    }
    char *output = (char *)HcfMalloc(OID_LENGTH, 0);
    if (output == NULL) {
        LOGE("Failed to malloc the output!");
        return HCF_ERR_MALLOC;
    }
    int32_t resLen = OBJ_obj2txt(output, OID_LENGTH, oid, 1);
    if (resLen < 0) {
        LOGE("Failed to do OBJ_obj2txt!");
        HcfPrintOpensslError();
        HcfFree(output);
        return HCF_ERR_CRYPTO_OPERATION;
    }
    uint32_t length = strlen(output) + 1;
    oidOut->data = (uint8_t *)HcfMalloc(length, 0);
    if (oidOut->data == NULL) {
        LOGE("Failed to malloc for oidOut!");
        HcfFree(output);
        return HCF_ERR_MALLOC;
    }
    (void)memcpy_s(oidOut->data, length, output, length);
    HcfFree(output);
    oidOut->len = length;
    return HCF_SUCCESS;
}

static HcfResult GetSignatureAlgOid(HcfX509CrlSpi *self, HcfBlob *oidOut)
{
    if ((self == NULL) || (oidOut == NULL)) {
        LOGE("Invalid Paramas!");
        return HCF_INVALID_PARAMS;
    }
    X509_CRL *crl = GetCrl(self);
    if (crl == NULL) {
        LOGE("crl is null!");
        return HCF_INVALID_PARAMS;
    }
    return GetSignatureAlgOidInner(crl, oidOut);
}

static HcfResult GetSignatureAlgName(HcfX509CrlSpi *self, HcfBlob *algNameOut)
{
    if ((self == NULL) || (algNameOut == NULL)) {
        LOGE("Invalid Paramas!");
        return HCF_INVALID_PARAMS;
    }
    if (!IsClassMatch((HcfObjectBase *)self, GetClass())) {
        LOGE("Input wrong class type!");
        return HCF_INVALID_PARAMS;
    }
    HcfBlob *oidOut = (HcfBlob *)HcfMalloc(sizeof(HcfBlob), 0);
    HcfResult res = GetSignatureAlgOid(self, oidOut);
    if (res != HCF_SUCCESS) {
        LOGE("Get signature algor oid failed!");
        HcfFree(oidOut);
        return res;
    }
    const char *algName = GetAlgorithmName((const char*)(oidOut->data));
    HcfFree(oidOut->data);
    HcfFree(oidOut);
    if (algName == NULL) {
        LOGE("Can not find algorithmName!");
        return HCF_ERR_CRYPTO_OPERATION;
    }
    uint32_t length = strlen(algName) + 1;
    algNameOut->data = (uint8_t *)HcfMalloc(length, 0);
    if (algNameOut->data == NULL) {
        LOGE("Failed to malloc for algName!");
        return HCF_ERR_MALLOC;
    }
    (void)memcpy_s(algNameOut->data, length, algName, length);
    algNameOut->len = length;
    return HCF_SUCCESS;
}

static HcfResult GetSignatureAlgParamsInner(X509_CRL *crl, HcfBlob *sigAlgParamOut)
{
    const X509_ALGOR *palg = NULL;
    X509_CRL_get0_signature(crl, NULL, &palg);
    if (palg == NULL) {
        LOGE("Get alg is null!");
        HcfPrintOpensslError();
        return HCF_ERR_CRYPTO_OPERATION;
    }
    int32_t paramType = 0;
    const void *paramValue = NULL;
    X509_ALGOR_get0(NULL, &paramType, &paramValue, palg);
    if (paramType == V_ASN1_UNDEF) {
        LOGE("get_X509_ALGOR_parameter, no parameters!");
        HcfPrintOpensslError();
        return HCF_NOT_SUPPORT;
    }
    ASN1_TYPE *param = ASN1_TYPE_new();
    if (ASN1_TYPE_set1(param, paramType, paramValue) != HCF_OPENSSL_SUCCESS) {
        LOGE("Set type fail!");
        ASN1_TYPE_free(param);
        HcfPrintOpensslError();
        return HCF_ERR_CRYPTO_OPERATION;
    }
    unsigned char *outParams = NULL;
    int32_t length = i2d_ASN1_TYPE(param, &outParams);
    ASN1_TYPE_free(param);
    if (length <= 0) {
        LOGE("Do i2d_ASN1_TYPE fail!");
        HcfPrintOpensslError();
        return HCF_ERR_CRYPTO_OPERATION;
    }
    sigAlgParamOut->data = (uint8_t *)HcfMalloc(length, 0);
    if (sigAlgParamOut->data == NULL) {
        LOGE("Failed to malloc for sigAlgParam!");
        OPENSSL_free(outParams);
        return HCF_ERR_MALLOC;
    }
    (void)memcpy_s(sigAlgParamOut->data, length, outParams, length);
    sigAlgParamOut->len = length;
    OPENSSL_free(outParams);
    return HCF_SUCCESS;
}

static HcfResult GetSignatureAlgParams(HcfX509CrlSpi *self, HcfBlob *sigAlgParamOut)
{
    if ((self == NULL) || (sigAlgParamOut == NULL)) {
        LOGE("Invalid Paramas!");
        return HCF_INVALID_PARAMS;
    }
    X509_CRL *crl = GetCrl(self);
    if (crl == NULL) {
        LOGE("crl is null!");
        return HCF_INVALID_PARAMS;
    }
    return GetSignatureAlgParamsInner(crl, sigAlgParamOut);
}

static void Destroy(HcfObjectBase *self)
{
    if (self == NULL) {
        return;
    }
    if (!IsClassMatch(self, GetClass())) {
        LOGE("Input wrong class type!");
        return;
    }
    HcfX509CRLOpensslImpl *realCrl = (HcfX509CRLOpensslImpl *)self;
    X509_CRL_free(realCrl->crl);
    realCrl->crl = NULL;
    if (realCrl->certIssuer != NULL) {
        HcfFree(realCrl->certIssuer->data);
        realCrl->certIssuer->data = NULL;
        HcfFree(realCrl->certIssuer);
        realCrl->certIssuer = NULL;
    }
    HcfFree(realCrl);
}

static X509_CRL *ParseX509CRL(const HcfEncodingBlob *inStream)
{
    if ((inStream->data == NULL) || (inStream->len <= 0)) {
        LOGE("Invalid Paramas!");
        return NULL;
    }
    BIO *bio = BIO_new_mem_buf(inStream->data, inStream->len);
    if (bio == NULL) {
        LOGE("bio get null!");
        HcfPrintOpensslError();
        return NULL;
    }
    X509_CRL *crlOut = NULL;
    switch (inStream->encodingFormat) {
        case HCF_FORMAT_DER:
            crlOut = d2i_X509_CRL_bio(bio, NULL);
            break;
        case HCF_FORMAT_PEM:
            crlOut = PEM_read_bio_X509_CRL(bio, NULL, NULL, NULL);
            break;
        default:
            LOGE("Not support format!");
            break;
    }
    BIO_free_all(bio);
    if (crlOut == NULL) {
        LOGE("Parse X509 CRL fail!");
        HcfPrintOpensslError();
        return NULL;
    }
    return crlOut;
}

HcfResult HcfCX509CrlSpiCreate(const HcfEncodingBlob *inStream, HcfX509CrlSpi **spi)
{
    if ((inStream == NULL) || (inStream->data == NULL) || (spi == NULL)) {
        LOGE("Invalid Paramas!");
        return HCF_INVALID_PARAMS;
    }
    HcfX509CRLOpensslImpl *returnCRL = (HcfX509CRLOpensslImpl *)HcfMalloc(sizeof(HcfX509CRLOpensslImpl), 0);
    if (returnCRL == NULL) {
        LOGE("Failed to malloc for x509 instance!");
        return HCF_ERR_MALLOC;
    }
    X509_CRL *crl = ParseX509CRL(inStream);
    if (crl == NULL) {
        LOGE("Failed to Parse x509 CRL!");
        HcfFree(returnCRL);
        return HCF_INVALID_PARAMS;
    }
    returnCRL->crl = crl;
    returnCRL->certIssuer = NULL;
    returnCRL->base.base.getClass = GetClass;
    returnCRL->base.base.destroy = Destroy;
    returnCRL->base.engineIsRevoked = IsRevoked;
    returnCRL->base.engineGetType = GetType;
    returnCRL->base.engineGetEncoded = GetEncoded;
    returnCRL->base.engineVerify = Verify;
    returnCRL->base.engineGetVersion = GetVersion;
    returnCRL->base.engineGetIssuerName = GetIssuerName;
    returnCRL->base.engineGetLastUpdate = GetLastUpdate;
    returnCRL->base.engineGetNextUpdate = GetNextUpdate;
    returnCRL->base.engineGetRevokedCert = GetRevokedCert;
    returnCRL->base.engineGetRevokedCertWithCert = GetRevokedCertWithCert;
    returnCRL->base.engineGetRevokedCerts = GetRevokedCerts;
    returnCRL->base.engineGetTbsInfo = GetTbsList;
    returnCRL->base.engineGetSignature = GetSignature;
    returnCRL->base.engineGetSignatureAlgName = GetSignatureAlgName;
    returnCRL->base.engineGetSignatureAlgOid = GetSignatureAlgOid;
    returnCRL->base.engineGetSignatureAlgParams = GetSignatureAlgParams;
    if (SetCertIssuer((HcfX509CrlSpi *)returnCRL) != HCF_SUCCESS) {
        LOGI("No cert issuer find or set cert issuer fail!");
    }
    *spi = (HcfX509CrlSpi *)returnCRL;
    return HCF_SUCCESS;
}
