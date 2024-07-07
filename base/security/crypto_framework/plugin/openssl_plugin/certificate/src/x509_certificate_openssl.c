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

#include "x509_certificate_openssl.h"

#include <securec.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#include <openssl/evp.h>
#include <openssl/pem.h>

#include "config.h"
#include "log.h"
#include "memory.h"
#include "result.h"
#include "utils.h"
#include "x509_certificate.h"
#include "openssl_class.h"
#include "openssl_common.h"

#define X509_CERT_PUBLIC_KEY_OPENSSL_CLASS "X509CertPublicKeyOpensslClass"
#define OID_STR_MAX_LEN 128
#define CHAR_TO_BIT_LEN 8
#define MAX_DATE_STR_LEN 128
#define FLAG_BIT_LEFT_NUM 0x07

typedef struct {
    HcfPubKey base;
    EVP_PKEY *pubKey;
} X509PubKeyOpensslImpl;

static HcfResult DeepCopyDataToOut(const char *data, uint32_t len, HcfBlob *out)
{
    out->data = (uint8_t *)HcfMalloc(len, 0);
    if (out->data == NULL) {
        LOGE("Failed to malloc for sig algorithm params!");
        return HCF_ERR_MALLOC;
    }
    (void)memcpy_s(out->data, len, data, len);
    out->len = len;
    return HCF_SUCCESS;
}

static const char *GetX509CertClass(void)
{
    return X509_CERT_OPENSSL_CLASS;
}

static void DestroyX509Openssl(HcfObjectBase *self)
{
    if (self == NULL) {
        return;
    }
    if (!IsClassMatch(self, GetX509CertClass())) {
        LOGE("Input wrong class type!");
        return;
    }
    HcfOpensslX509Cert *realCert = (HcfOpensslX509Cert *)self;
    X509_free(realCert->x509);
    realCert->x509 = NULL;
    HcfFree(realCert);
}

static const char *GetX509CertPubKeyClass(void)
{
    return X509_CERT_PUBLIC_KEY_OPENSSL_CLASS;
}

static void DestroyX509PubKeyOpenssl(HcfObjectBase *self)
{
    if (self == NULL) {
        return;
    }
    if (!IsClassMatch(self, GetX509CertPubKeyClass())) {
        LOGE("Input wrong class type!");
        return;
    }
    X509PubKeyOpensslImpl *impl = (X509PubKeyOpensslImpl *)self;
    if (impl->pubKey != NULL) {
        EVP_PKEY_free(impl->pubKey);
        impl->pubKey = NULL;
    }
    HcfFree(impl);
}

static const char *GetPubKeyAlgorithm(HcfKey *self)
{
    (void)self;
    LOGD("Not supported!");
    return NULL;
}


static HcfResult GetPubKeyEncoded(HcfKey *self, HcfBlob *returnBlob)
{
    (void)self;
    (void)returnBlob;
    LOGD("Not supported!");
    return HCF_NOT_SUPPORT;
}

static const char *GetPubKeyFormat(HcfKey *self)
{
    (void)self;
    LOGD("Not supported!");
    return NULL;
}

static HcfResult VerifyX509Openssl(HcfX509CertificateSpi *self, HcfPubKey *key)
{
    if ((self == NULL) || (key == NULL)) {
        LOGE("The input data is null!");
        return HCF_INVALID_PARAMS;
    }
    if (!IsClassMatch((HcfObjectBase *)self, GetX509CertClass()) ||
        (!IsClassMatch((HcfObjectBase *)key, GetX509CertPubKeyClass()))) {
        LOGE("Input wrong class type!");
        return HCF_INVALID_PARAMS;
    }
    HcfOpensslX509Cert *realCert = (HcfOpensslX509Cert *)self;
    X509 *x509 = realCert->x509;
    X509PubKeyOpensslImpl *keyImpl = (X509PubKeyOpensslImpl *)key;
    EVP_PKEY *pubKey = keyImpl->pubKey;
    if (X509_verify(x509, pubKey) != HCF_OPENSSL_SUCCESS) {
        LOGE("Failed to verify x509 cert's signature.");
        HcfPrintOpensslError();
        return HCF_ERR_CRYPTO_OPERATION;
    }
    return HCF_SUCCESS;
}

static HcfResult GetEncodedX509Openssl(HcfX509CertificateSpi *self, HcfEncodingBlob *encodedByte)
{
    if ((self == NULL) || (encodedByte == NULL)) {
        LOGE("The input data is null!");
        return HCF_INVALID_PARAMS;
    }
    if (!IsClassMatch((HcfObjectBase *)self, GetX509CertClass())) {
        LOGE("Input wrong class type!");
        return HCF_INVALID_PARAMS;
    }
    HcfOpensslX509Cert *realCert = (HcfOpensslX509Cert *)self;
    X509 *x509 = realCert->x509;
    int32_t length = i2d_X509(x509, NULL);
    if ((length <= 0) || (x509 == NULL)) {
        LOGE("Failed to convert internal x509 to der format!");
        HcfPrintOpensslError();
        return HCF_ERR_CRYPTO_OPERATION;
    }
    unsigned char *der = NULL;
    (void)i2d_X509(x509, &der);
    encodedByte->data = (uint8_t *)HcfMalloc(length, 0);
    if (encodedByte->data == NULL) {
        LOGE("Failed to malloc for x509 der data!");
        OPENSSL_free(der);
        return HCF_ERR_MALLOC;
    }
    (void)memcpy_s(encodedByte->data, length, der, length);
    OPENSSL_free(der);
    encodedByte->len = length;
    encodedByte->encodingFormat = HCF_FORMAT_DER;
    return HCF_SUCCESS;
}

static HcfResult GetPublicKeyX509Openssl(HcfX509CertificateSpi *self, HcfPubKey **keyOut)
{
    if ((self == NULL) || (keyOut == NULL)) {
        LOGE("The input data is null!");
        return HCF_INVALID_PARAMS;
    }
    if (!IsClassMatch((HcfObjectBase *)self, GetX509CertClass())) {
        LOGE("Input wrong class type!");
        return HCF_INVALID_PARAMS;
    }
    HcfOpensslX509Cert *realCert = (HcfOpensslX509Cert *)self;
    X509 *x509 = realCert->x509;
    EVP_PKEY *pubKey = X509_get_pubkey(x509);
    if (pubKey == NULL) {
        LOGE("Failed to get publick key from x509 cert.");
        HcfPrintOpensslError();
        return HCF_ERR_CRYPTO_OPERATION;
    }
    X509PubKeyOpensslImpl *keyImpl = (X509PubKeyOpensslImpl *)HcfMalloc(sizeof(X509PubKeyOpensslImpl), 0);
    if (keyImpl == NULL) {
        LOGE("Failed to malloc for public key obj!");
        EVP_PKEY_free(pubKey);
        return HCF_ERR_MALLOC;
    }
    keyImpl->pubKey = pubKey;
    keyImpl->base.base.base.destroy = DestroyX509PubKeyOpenssl;
    keyImpl->base.base.base.getClass = GetX509CertPubKeyClass;
    keyImpl->base.base.getEncoded = GetPubKeyEncoded;
    keyImpl->base.base.getAlgorithm = GetPubKeyAlgorithm;
    keyImpl->base.base.getFormat = GetPubKeyFormat;
    *keyOut = (HcfPubKey *)keyImpl;
    return HCF_SUCCESS;
}

static HcfResult CompareDateWithCertTime(const X509 *x509, const ASN1_TIME *inputDate)
{
    ASN1_TIME *startDate = X509_get_notBefore(x509);
    ASN1_TIME *expirationDate = X509_get_notAfter(x509);
    if ((startDate == NULL) || (expirationDate == NULL)) {
        LOGE("Date is null in x509 cert!");
        HcfPrintOpensslError();
        return HCF_ERR_CRYPTO_OPERATION;
    }
    HcfResult res = HCF_SUCCESS;
    /* 0: equal in ASN1_TIME_compare, -1: a < b, 1: a > b, -2: error. */
    if (ASN1_TIME_compare(inputDate, startDate) < 0) {
        LOGE("Date is not validate in x509 cert!");
        res = HCF_ERR_CERT_NOT_YET_VALID;
    } else if (ASN1_TIME_compare(expirationDate, inputDate) < 0) {
        LOGE("Date is expired in x509 cert!");
        res = HCF_ERR_CERT_HAS_EXPIRED;
    }
    return res;
}

static HcfResult CheckValidityWithDateX509Openssl(HcfX509CertificateSpi *self, const char *date)
{
    if ((self == NULL) || (date == NULL)) {
        LOGE("The input data is null!");
        return HCF_INVALID_PARAMS;
    }
    if (!IsClassMatch((HcfObjectBase *)self, GetX509CertClass())) {
        LOGE("Input wrong class type!");
        return HCF_INVALID_PARAMS;
    }
    HcfOpensslX509Cert *realCert = (HcfOpensslX509Cert *)self;
    X509 *x509 = realCert->x509;
    ASN1_TIME *asn1InputDate = ASN1_TIME_new();
    if (asn1InputDate == NULL) {
        LOGE("Failed to malloc for asn1 time.");
        return HCF_ERR_MALLOC;
    }
    if (ASN1_TIME_set_string(asn1InputDate, date) != HCF_OPENSSL_SUCCESS) {
        LOGE("Failed to set time for asn1 time.");
        HcfPrintOpensslError();
        ASN1_TIME_free(asn1InputDate);
        return HCF_ERR_CRYPTO_OPERATION;
    }
    HcfResult res = CompareDateWithCertTime(x509, asn1InputDate);
    ASN1_TIME_free(asn1InputDate);
    return res;
}

static long GetVersionX509Openssl(HcfX509CertificateSpi *self)
{
    if (self == NULL) {
        LOGE("The input data is null!");
        return INVALID_VERSION;
    }
    if (!IsClassMatch((HcfObjectBase *)self, GetX509CertClass())) {
        LOGE("Input wrong class type!");
        return INVALID_VERSION;
    }
    HcfOpensslX509Cert *realCert = (HcfOpensslX509Cert *)self;
    X509 *x509 = realCert->x509;
    return X509_get_version(x509) + 1;
}

static long GetSerialNumberX509Openssl(HcfX509CertificateSpi *self)
{
    if (self == NULL) {
        LOGE("The input data is null!");
        return INVALID_SERIAL_NUMBER;
    }
    if (!IsClassMatch((HcfObjectBase *)self, GetX509CertClass())) {
        LOGE("Input wrong class type!");
        return INVALID_SERIAL_NUMBER;
    }
    HcfOpensslX509Cert *realCert = (HcfOpensslX509Cert *)self;
    X509 *x509 = realCert->x509;
    const ASN1_INTEGER *serial = X509_get0_serialNumber(x509);
    if (serial == NULL) {
        LOGE("Failed to get serial number!");
        return INVALID_SERIAL_NUMBER;
    }
    return ASN1_INTEGER_get(serial);
}

static HcfResult GetIssuerDNX509Openssl(HcfX509CertificateSpi *self, HcfBlob *out)
{
    if ((self == NULL) || (out == NULL)) {
        LOGE("[Get issuerDN openssl] The input data is null!");
        return HCF_INVALID_PARAMS;
    }
    if (!IsClassMatch((HcfObjectBase *)self, GetX509CertClass())) {
        LOGE("Input wrong class type!");
        return HCF_INVALID_PARAMS;
    }
    HcfOpensslX509Cert *realCert = (HcfOpensslX509Cert *)self;
    X509 *x509 = realCert->x509;
    X509_NAME *issuerName = X509_get_issuer_name(x509);
    if (issuerName == NULL) {
        LOGE("Failed to get x509 issuerName in openssl!");
        HcfPrintOpensslError();
        return HCF_ERR_CRYPTO_OPERATION;
    }
    char *issuer = (char *)HcfMalloc(HCF_MAX_STR_LEN + 1, 0);
    if (issuer == NULL) {
        LOGE("Failed to malloc for issuer buffer!");
        return HCF_ERR_MALLOC;
    }

    HcfResult res = HCF_SUCCESS;
    do {
        X509_NAME_oneline(issuerName, issuer, HCF_MAX_STR_LEN);
        size_t length = strlen(issuer) + 1;
        if (length == 1) {
            LOGE("Failed to get oneline issuerName in openssl!");
            res = HCF_ERR_CRYPTO_OPERATION;
            HcfPrintOpensslError();
            break;
        }
        res = DeepCopyDataToOut(issuer, length, out);
    } while (0);
    HcfFree(issuer);
    return res;
}

static HcfResult GetSubjectDNX509Openssl(HcfX509CertificateSpi *self, HcfBlob *out)
{
    if ((self == NULL) || (out == NULL)) {
        LOGE("[Get subjectDN openssl]The input data is null!");
        return HCF_INVALID_PARAMS;
    }
    if (!IsClassMatch((HcfObjectBase *)self, GetX509CertClass())) {
        LOGE("Input wrong class type!");
        return HCF_INVALID_PARAMS;
    }
    HcfOpensslX509Cert *realCert = (HcfOpensslX509Cert *)self;
    X509 *x509 = realCert->x509;
    X509_NAME *subjectName = X509_get_subject_name(x509);
    if (subjectName == NULL) {
        LOGE("Failed to get x509 subjectName in openssl!");
        HcfPrintOpensslError();
        return HCF_ERR_CRYPTO_OPERATION;
    }
    char *subject = (char *)HcfMalloc(HCF_MAX_STR_LEN + 1, 0);
    if (subject == NULL) {
        LOGE("Failed to malloc for subject buffer!");
        return HCF_ERR_MALLOC;
    }

    HcfResult res = HCF_SUCCESS;
    do {
        X509_NAME_oneline(subjectName, subject, HCF_MAX_STR_LEN);
        size_t length = strlen(subject) + 1;
        if (length == 1) {
            LOGE("Failed to get oneline subjectName in openssl!");
            HcfPrintOpensslError();
            res = HCF_ERR_CRYPTO_OPERATION;
            break;
        }
        res = DeepCopyDataToOut(subject, length, out);
    } while (0);
    HcfFree(subject);
    return res;
}

static HcfResult GetNotBeforeX509Openssl(HcfX509CertificateSpi *self, HcfBlob *outDate)
{
    if ((self == NULL) || (outDate == NULL)) {
        LOGE("Get not before, input is null!");
        return HCF_INVALID_PARAMS;
    }
    if (!IsClassMatch((HcfObjectBase *)self, GetX509CertClass())) {
        LOGE("Get not before, input wrong class type!");
        return HCF_INVALID_PARAMS;
    }
    HcfOpensslX509Cert *realCert = (HcfOpensslX509Cert *)self;
    X509 *x509 = realCert->x509;
    ASN1_TIME *notBeforeDate = X509_get_notBefore(x509);
    if (notBeforeDate == NULL) {
        LOGE("NotBeforeDate is null in x509 cert!");
        HcfPrintOpensslError();
        return HCF_ERR_CRYPTO_OPERATION;
    }
    if (ASN1_TIME_normalize(notBeforeDate) != HCF_OPENSSL_SUCCESS) {
        LOGE("Failed to normalize notBeforeDate!");
        HcfPrintOpensslError();
        return HCF_ERR_CRYPTO_OPERATION;
    }
    const char *date = (const char *)(notBeforeDate->data);
    if ((date == NULL) || (strlen(date) > HCF_MAX_STR_LEN)) {
        LOGE("Failed to get notBeforeDate data!");
        return HCF_ERR_CRYPTO_OPERATION;
    }
    uint32_t length = strlen(date) + 1;
    return DeepCopyDataToOut(date, length, outDate);
}

static HcfResult GetNotAfterX509Openssl(HcfX509CertificateSpi *self, HcfBlob *outDate)
{
    if ((self == NULL) || (outDate == NULL)) {
        LOGE("Get not after, input data is null!");
        return HCF_INVALID_PARAMS;
    }
    if (!IsClassMatch((HcfObjectBase *)self, GetX509CertClass())) {
        LOGE("Get not after, input wrong class type!");
        return HCF_INVALID_PARAMS;
    }
    HcfOpensslX509Cert *realCert = (HcfOpensslX509Cert *)self;
    X509 *x509 = realCert->x509;
    ASN1_TIME *notAfterDate = X509_get_notAfter(x509);
    if (notAfterDate == NULL) {
        LOGE("NotAfterDate is null in x509 cert!");
        HcfPrintOpensslError();
        return HCF_ERR_CRYPTO_OPERATION;
    }
    if (ASN1_TIME_normalize(notAfterDate) != HCF_OPENSSL_SUCCESS) {
        LOGE("Failed to normalize notAfterDate!");
        HcfPrintOpensslError();
        return HCF_ERR_CRYPTO_OPERATION;
    }
    const char *date = (const char *)(notAfterDate->data);
    if ((date == NULL) || (strlen(date) > HCF_MAX_STR_LEN)) {
        LOGE("Failed to get notAfterDate data!");
        return HCF_ERR_CRYPTO_OPERATION;
    }
    uint32_t length = strlen(date) + 1;
    return DeepCopyDataToOut(date, length, outDate);
}

static HcfResult GetSignatureX509Openssl(HcfX509CertificateSpi *self, HcfBlob *sigOut)
{
    if ((self == NULL) || (sigOut == NULL)) {
        LOGE("The input data is null!");
        return HCF_INVALID_PARAMS;
    }
    if (!IsClassMatch((HcfObjectBase *)self, GetX509CertClass())) {
        LOGE("Input wrong class type!");
        return HCF_INVALID_PARAMS;
    }
    HcfOpensslX509Cert *realCert = (HcfOpensslX509Cert *)self;
    X509 *x509 = realCert->x509;
    const ASN1_BIT_STRING *signature;
    X509_get0_signature(&signature, NULL, x509);
    if ((signature == NULL) || (signature->length == 0) || (signature->length > HCF_MAX_BUFFER_LEN)) {
        LOGE("Failed to get x509 signature in openssl!");
        HcfPrintOpensslError();
        return HCF_ERR_CRYPTO_OPERATION;
    }
    sigOut->data = (uint8_t *)HcfMalloc(signature->length, 0);
    if (sigOut->data == NULL) {
        LOGE("Failed to malloc for signature data!");
        return HCF_ERR_MALLOC;
    }
    (void)memcpy_s(sigOut->data, signature->length, signature->data, signature->length);
    sigOut->len = signature->length;
    return HCF_SUCCESS;
}

static HcfResult GetSigAlgNameX509Openssl(HcfX509CertificateSpi *self, HcfBlob *outName)
{
    if ((self == NULL) || (outName == NULL)) {
        LOGE("[GetSigAlgName openssl] The input data is null!");
        return HCF_INVALID_PARAMS;
    }
    if (!IsClassMatch((HcfObjectBase *)self, GetX509CertClass())) {
        LOGE("[GetSigAlgName openssl] Input wrong class type!");
        return HCF_INVALID_PARAMS;
    }
    HcfOpensslX509Cert *realCert = (HcfOpensslX509Cert *)self;
    X509 *x509 = realCert->x509;
    const X509_ALGOR *alg;
    X509_get0_signature(NULL, &alg, x509);
    const ASN1_OBJECT *oidObj;
    X509_ALGOR_get0(&oidObj, NULL, NULL, alg);
    char oidStr[OID_STR_MAX_LEN] = { 0 };
    int32_t resLen = OBJ_obj2txt(oidStr, OID_STR_MAX_LEN, oidObj, 1);
    if ((resLen < 0) || (resLen >= OID_STR_MAX_LEN)) {
        LOGE("Failed to convert x509 object to text!");
        HcfPrintOpensslError();
        return HCF_ERR_CRYPTO_OPERATION;
    }
    const char *algName = GetAlgorithmName(oidStr);
    if (algName == NULL) {
        return HCF_ERR_CRYPTO_OPERATION;
    }
    uint32_t len = strlen(algName) + 1;
    return DeepCopyDataToOut(algName, len, outName);
}

static HcfResult GetSigAlgOidX509Openssl(HcfX509CertificateSpi *self, HcfBlob *out)
{
    if ((self == NULL) || (out == NULL)) {
        LOGE("[GetSigAlgOID openssl] The input data is null!");
        return HCF_INVALID_PARAMS;
    }
    if (!IsClassMatch((HcfObjectBase *)self, GetX509CertClass())) {
        LOGE("[GetSigAlgOID openssl] Input wrong class type!");
        return HCF_INVALID_PARAMS;
    }
    HcfOpensslX509Cert *realCert = (HcfOpensslX509Cert *)self;
    X509 *x509 = realCert->x509;
    const X509_ALGOR *alg;
    X509_get0_signature(NULL, &alg, x509);
    const ASN1_OBJECT *oid;
    X509_ALGOR_get0(&oid, NULL, NULL, alg);
    char algOid[OID_STR_MAX_LEN] = { 0 };
    int32_t resLen = OBJ_obj2txt(algOid, OID_STR_MAX_LEN, oid, 1);
    if ((resLen < 0) || (resLen >= OID_STR_MAX_LEN)) {
        LOGE("Failed to convert x509 object to text!");
        HcfPrintOpensslError();
        return HCF_ERR_CRYPTO_OPERATION;
    }
    uint32_t len = strlen(algOid) + 1;
    return DeepCopyDataToOut(algOid, len, out);
}

static HcfResult GetSigAlgParamsX509Openssl(HcfX509CertificateSpi *self, HcfBlob *sigAlgParamsOut)
{
    if ((self == NULL) || (sigAlgParamsOut == NULL)) {
        LOGE("[GetSigAlgParams openssl] The input data is null!");
        return HCF_INVALID_PARAMS;
    }
    if (!IsClassMatch((HcfObjectBase *)self, GetX509CertClass())) {
        LOGE("[GetSigAlgParams openssl] Input wrong class type!");
        return HCF_INVALID_PARAMS;
    }
    HcfOpensslX509Cert *realCert = (HcfOpensslX509Cert *)self;
    X509 *x509 = realCert->x509;
    const X509_ALGOR *alg;
    X509_get0_signature(NULL, &alg, x509);
    int32_t paramType = 0;
    const void *paramValue = NULL;
    X509_ALGOR_get0(NULL, &paramType, &paramValue, alg);
    if (paramType == V_ASN1_UNDEF) {
        LOGE("get_X509_ALGOR_parameter, no parameters!");
        return HCF_NOT_SUPPORT;
    }
    ASN1_TYPE *param = ASN1_TYPE_new();
    if (param == NULL) {
        LOGE("Failed to malloc for asn1 type data!");
        return HCF_ERR_MALLOC;
    }
    if (ASN1_TYPE_set1(param, paramType, paramValue) != HCF_OPENSSL_SUCCESS) {
        LOGE("Failed to set asn1 type in openssl!");
        HcfPrintOpensslError();
        ASN1_TYPE_free(param);
        return HCF_ERR_CRYPTO_OPERATION;
    }
    unsigned char *out = NULL;
    int32_t len = i2d_ASN1_TYPE(param, NULL);
    if (len <= 0) {
        LOGE("Failed to convert ASN1_TYPE!");
        HcfPrintOpensslError();
        ASN1_TYPE_free(param);
        return HCF_ERR_CRYPTO_OPERATION;
    }
    (void)i2d_ASN1_TYPE(param, &out);
    ASN1_TYPE_free(param);
    HcfResult res = DeepCopyDataToOut((const char *)out, len, sigAlgParamsOut);
    OPENSSL_free(out);
    return res;
}

static HcfResult ConvertAsn1String2BoolArray(const ASN1_BIT_STRING *string, HcfBlob *boolArr)
{
    uint32_t length = ASN1_STRING_length(string) * CHAR_TO_BIT_LEN;
    if (string->flags & ASN1_STRING_FLAG_BITS_LEFT) {
        length -= string->flags & FLAG_BIT_LEFT_NUM;
    }
    boolArr->data = (uint8_t *)HcfMalloc(length, 0);
    if (boolArr->data == NULL) {
        LOGE("Failed to malloc for bit array data!");
        return HCF_ERR_MALLOC;
    }
    for (uint32_t i = 0; i < length; i++) {
        boolArr->data[i] = ASN1_BIT_STRING_get_bit(string, i);
    }
    boolArr->len = length;
    return HCF_SUCCESS;
}

static HcfResult GetKeyUsageX509Openssl(HcfX509CertificateSpi *self, HcfBlob *boolArr)
{
    if ((self == NULL) || (boolArr == NULL)) {
        LOGE("[GetKeyUsage openssl] The input data is null!");
        return HCF_INVALID_PARAMS;
    }
    if (!IsClassMatch((HcfObjectBase *)self, GetX509CertClass())) {
        LOGE("Input wrong class type!");
        return HCF_INVALID_PARAMS;
    }
    HcfOpensslX509Cert *realCert = (HcfOpensslX509Cert *)self;
    X509 *x509 = realCert->x509;

    ASN1_BIT_STRING *keyUsage = (ASN1_BIT_STRING *)X509_get_ext_d2i(x509, NID_key_usage, NULL, NULL);
    if ((keyUsage == NULL) || (keyUsage->length <= 0)|| (keyUsage->length >= HCF_MAX_STR_LEN)) {
        LOGE("Failed to get x509 keyUsage in openssl!");
        HcfPrintOpensslError();
        return HCF_ERR_CRYPTO_OPERATION;
    }
    HcfResult res = ConvertAsn1String2BoolArray(keyUsage, boolArr);
    ASN1_BIT_STRING_free(keyUsage);
    return res;
}

static HcfResult DeepCopyExtendedKeyUsage(const STACK_OF(ASN1_OBJECT) *extUsage,
    int32_t i, HcfArray *keyUsageOut)
{
    char usage[OID_STR_MAX_LEN] = { 0 };
    int32_t resLen = OBJ_obj2txt(usage, OID_STR_MAX_LEN, sk_ASN1_OBJECT_value(extUsage, i), 1);
    if ((resLen < 0) || (resLen >= OID_STR_MAX_LEN)) {
        LOGE("Failed to convert x509 object to text!");
        HcfPrintOpensslError();
        return HCF_ERR_CRYPTO_OPERATION;
    }
    uint32_t len = strlen(usage) + 1;
    keyUsageOut->data[i].data = (uint8_t *)HcfMalloc(len, 0);
    if (keyUsageOut->data[i].data == NULL) {
        LOGE("Failed to malloc for key usage!");
        return HCF_ERR_MALLOC;
    }
    (void)memcpy_s(keyUsageOut->data[i].data, len, usage, len);
    keyUsageOut->data[i].len = len;
    return HCF_SUCCESS;
}

static HcfResult GetExtendedKeyUsageX509Openssl(HcfX509CertificateSpi *self, HcfArray *keyUsageOut)
{
    if ((self == NULL) || (keyUsageOut == NULL)) {
        LOGE("The input data is null!");
        return HCF_INVALID_PARAMS;
    }
    if (!IsClassMatch((HcfObjectBase *)self, GetX509CertClass())) {
        LOGE("Input wrong class type!");
        return HCF_INVALID_PARAMS;
    }
    HcfOpensslX509Cert *realCert = (HcfOpensslX509Cert *)self;
    X509 *x509 = realCert->x509;
    STACK_OF(ASN1_OBJECT) *extUsage = X509_get_ext_d2i(x509, NID_ext_key_usage, NULL, NULL);
    if (extUsage == NULL) {
        LOGE("Failed to get x509 extended keyUsage in openssl!");
        return HCF_ERR_CRYPTO_OPERATION;
    }
    HcfResult res = HCF_SUCCESS;
    do {
        int32_t size = sk_ASN1_OBJECT_num(extUsage);
        if (size <= 0) {
            LOGE("The extended key usage size in openssl is invalid!");
            HcfPrintOpensslError();
            res = HCF_ERR_CRYPTO_OPERATION;
            break;
        }
        int32_t blobSize = sizeof(HcfBlob) * size;
        keyUsageOut->data = (HcfBlob *)HcfMalloc(blobSize, 0);
        if (keyUsageOut->data == NULL) {
            LOGE("Failed to malloc for keyUsageOut array!");
            res = HCF_ERR_MALLOC;
            break;
        }
        keyUsageOut->count = size;
        for (int32_t i = 0; i < size; ++i) {
            res = DeepCopyExtendedKeyUsage(extUsage, i, keyUsageOut);
            if (res != HCF_SUCCESS) {
                LOGE("Falied to copy extended key usage!");
                break;
            }
        }
    } while (0);
    if (res != HCF_SUCCESS) {
        HcfArrayDataClearAndFree(keyUsageOut);
    }
    sk_ASN1_OBJECT_pop_free(extUsage, ASN1_OBJECT_free);
    return res;
}

static int32_t GetBasicConstraintsX509Openssl(HcfX509CertificateSpi *self)
{
    if (self == NULL) {
        LOGE("The input data is null!");
        return INVALID_CONSTRAINTS_LEN;
    }
    if (!IsClassMatch((HcfObjectBase *)self, GetX509CertClass())) {
        LOGE("Input wrong class type!");
        return INVALID_CONSTRAINTS_LEN;
    }
    HcfOpensslX509Cert *realCert = (HcfOpensslX509Cert *)self;
    X509 *x509 = realCert->x509;
    BASIC_CONSTRAINTS *constraints = (BASIC_CONSTRAINTS *)X509_get_ext_d2i(x509, NID_basic_constraints, NULL, NULL);
    if (constraints == NULL) {
        LOGE("Failed to get basic constraints in openssl!");
        return INVALID_CONSTRAINTS_LEN;
    }
    /* Path len is only valid for CA cert. */
    if (!constraints->ca) {
        LOGI("The cert in not a CA!");
        return INVALID_CONSTRAINTS_LEN;
    }
    if ((constraints->pathlen == NULL) || (constraints->pathlen->type == V_ASN1_NEG_INTEGER)) {
        LOGE("The cert path len is negative in openssl!");
        return INVALID_CONSTRAINTS_LEN;
    }
    long pathLen = ASN1_INTEGER_get(constraints->pathlen);
    if ((pathLen < 0) || (pathLen > INT_MAX)) {
        LOGE("Get the overflow path length in openssl!");
        return INVALID_CONSTRAINTS_LEN;
    }
    return (int32_t)pathLen;
}

static HcfResult DeepCopyAlternativeNames(const STACK_OF(GENERAL_NAME) *altNames, int32_t i, HcfArray *outName)
{
    GENERAL_NAME *general = sk_GENERAL_NAME_value(altNames, i);
    int32_t generalType = 0;
    ASN1_STRING *ans1Str = GENERAL_NAME_get0_value(general, &generalType);
    const char *str = (const char *)ASN1_STRING_get0_data(ans1Str);
    if ((str == NULL) || (strlen(str) > HCF_MAX_STR_LEN)) {
        LOGE("Failed to get x509 altNames string in openssl!");
        HcfPrintOpensslError();
        return HCF_ERR_CRYPTO_OPERATION;
    }
    uint32_t nameLen = strlen(str) + 1;
    outName->data[i].data = (uint8_t *)HcfMalloc(nameLen, 0);
    if (outName->data[i].data == NULL) {
        LOGE("Failed to malloc for outName!");
        return HCF_ERR_MALLOC;
    }
    (void)memcpy_s(outName->data[i].data, nameLen, str, nameLen);
    outName->data[i].len = nameLen;
    return HCF_SUCCESS;
}

static HcfResult GetSubjectAltNamesX509Openssl(HcfX509CertificateSpi *self, HcfArray *outName)
{
    if ((self == NULL) || (outName == NULL)) {
        LOGE("[GetSubjectAltNames openssl] The input data is null!");
        return HCF_INVALID_PARAMS;
    }
    if (!IsClassMatch((HcfObjectBase *)self, GetX509CertClass())) {
        LOGE("Input wrong class type!");
        return HCF_INVALID_PARAMS;
    }
    HcfOpensslX509Cert *realCert = (HcfOpensslX509Cert *)self;
    X509 *x509 = realCert->x509;
    STACK_OF(GENERAL_NAME) *subjectAltName = X509_get_ext_d2i(x509, NID_subject_alt_name, NULL, NULL);
    if (subjectAltName == NULL) {
        LOGE("Failed to get subjectAltName in openssl!");
        HcfPrintOpensslError();
        return HCF_ERR_CRYPTO_OPERATION;
    }
    HcfResult res = HCF_SUCCESS;
    do {
        int32_t size = sk_GENERAL_NAME_num(subjectAltName);
        if (size <= 0) {
            LOGE("The subjectAltName number in openssl is invalid!");
            HcfPrintOpensslError();
            res = HCF_ERR_CRYPTO_OPERATION;
            break;
        }
        int32_t blobSize = sizeof(HcfBlob) * size;
        outName->data = (HcfBlob *)HcfMalloc(blobSize, 0);
        if (outName->data == NULL) {
            LOGE("Failed to malloc for subjectAltName array!");
            res = HCF_ERR_MALLOC;
            break;
        }
        outName->count = size;
        for (int32_t i = 0; i < size; ++i) {
            res = DeepCopyAlternativeNames(subjectAltName, i, outName);
            if (res != HCF_SUCCESS) {
                LOGE("Falied to copy subjectAltName!");
                break;
            }
        }
    } while (0);
    if (res != HCF_SUCCESS) {
        HcfArrayDataClearAndFree(outName);
    }
    GENERAL_NAMES_free(subjectAltName);
    return res;
}

static HcfResult GetIssuerAltNamesX509Openssl(HcfX509CertificateSpi *self, HcfArray *outName)
{
    if ((self == NULL) || (outName == NULL)) {
        LOGE("[GetIssuerAltNames openssl] The input data is null!");
        return HCF_INVALID_PARAMS;
    }
    if (!IsClassMatch((HcfObjectBase *)self, GetX509CertClass())) {
        LOGE("Input wrong class type!");
        return HCF_INVALID_PARAMS;
    }
    HcfOpensslX509Cert *realCert = (HcfOpensslX509Cert *)self;
    X509 *x509 = realCert->x509;
    STACK_OF(GENERAL_NAME) *issuerAltName = X509_get_ext_d2i(x509, NID_issuer_alt_name, NULL, NULL);
    if (issuerAltName == NULL) {
        LOGE("Failed to get issuerAltName in openssl!");
        HcfPrintOpensslError();
        return HCF_ERR_CRYPTO_OPERATION;
    }
    HcfResult res = HCF_SUCCESS;
    do {
        int32_t size = sk_GENERAL_NAME_num(issuerAltName);
        if (size <= 0) {
            LOGE("The issuerAltName number in openssl is invalid!");
            HcfPrintOpensslError();
            res = HCF_ERR_CRYPTO_OPERATION;
            break;
        }
        int32_t blobSize = sizeof(HcfBlob) * size;
        outName->data = (HcfBlob *)HcfMalloc(blobSize, 0);
        if (outName->data == NULL) {
            LOGE("Failed to malloc for issuerAltName array!");
            res = HCF_ERR_MALLOC;
            break;
        }
        outName->count = size;
        for (int32_t i = 0; i < size; ++i) {
            res = DeepCopyAlternativeNames(issuerAltName, i, outName);
            if (res != HCF_SUCCESS) {
                LOGE("Falied to copy issuerAltName!");
                break;
            }
        }
    } while (0);
    if (res != HCF_SUCCESS) {
        HcfArrayDataClearAndFree(outName);
    }
    GENERAL_NAMES_free(issuerAltName);
    return res;
}

static X509 *CreateX509CertInner(const HcfEncodingBlob *encodingBlob)
{
    X509 *x509 = NULL;
    BIO *bio = BIO_new_mem_buf(encodingBlob->data, encodingBlob->len);
    if (bio == NULL) {
        LOGE("Openssl bio new buf failed.");
        return NULL;
    }
    LOGD("The input cert format is: %d.", encodingBlob->encodingFormat);
    if (encodingBlob->encodingFormat == HCF_FORMAT_DER) {
        x509 = d2i_X509_bio(bio, NULL);
    } else if (encodingBlob->encodingFormat == HCF_FORMAT_PEM) {
        x509 = PEM_read_bio_X509(bio, NULL, NULL, NULL);
    }
    BIO_free(bio);
    return x509;
}

HcfResult OpensslX509CertSpiCreate(const HcfEncodingBlob *inStream, HcfX509CertificateSpi **spi)
{
    if ((inStream == NULL) || (inStream->data == NULL) || (spi == NULL)) {
        LOGE("The input data blob is null!");
        return HCF_INVALID_PARAMS;
    }
    HcfOpensslX509Cert *realCert = (HcfOpensslX509Cert *)HcfMalloc(sizeof(HcfOpensslX509Cert), 0);
    if (realCert == NULL) {
        LOGE("Failed to malloc for x509 instance!");
        return HCF_ERR_MALLOC;
    }
    realCert->x509 = CreateX509CertInner(inStream);
    if (realCert->x509 == NULL) {
        HcfFree(realCert);
        LOGE("Failed to create x509 cert from input data!");
        return HCF_INVALID_PARAMS;
    }
    realCert->base.base.getClass = GetX509CertClass;
    realCert->base.base.destroy = DestroyX509Openssl;
    realCert->base.engineVerify = VerifyX509Openssl;
    realCert->base.engineGetEncoded = GetEncodedX509Openssl;
    realCert->base.engineGetPublicKey = GetPublicKeyX509Openssl;
    realCert->base.engineCheckValidityWithDate = CheckValidityWithDateX509Openssl;
    realCert->base.engineGetVersion = GetVersionX509Openssl;
    realCert->base.engineGetSerialNumber = GetSerialNumberX509Openssl;
    realCert->base.engineGetIssuerName = GetIssuerDNX509Openssl;
    realCert->base.engineGetSubjectName = GetSubjectDNX509Openssl;
    realCert->base.engineGetNotBeforeTime = GetNotBeforeX509Openssl;
    realCert->base.engineGetNotAfterTime = GetNotAfterX509Openssl;
    realCert->base.engineGetSignature = GetSignatureX509Openssl;
    realCert->base.engineGetSignatureAlgName = GetSigAlgNameX509Openssl;
    realCert->base.engineGetSignatureAlgOid = GetSigAlgOidX509Openssl;
    realCert->base.engineGetSignatureAlgParams = GetSigAlgParamsX509Openssl;
    realCert->base.engineGetKeyUsage = GetKeyUsageX509Openssl;
    realCert->base.engineGetExtKeyUsage = GetExtendedKeyUsageX509Openssl;
    realCert->base.engineGetBasicConstraints = GetBasicConstraintsX509Openssl;
    realCert->base.engineGetSubjectAltNames = GetSubjectAltNamesX509Openssl;
    realCert->base.engineGetIssuerAltNames = GetIssuerAltNamesX509Openssl;
    *spi = (HcfX509CertificateSpi *)realCert;
    return HCF_SUCCESS;
}
