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

#include "securec.h"

#include <gtest/gtest.h>
#include <openssl/x509.h>

#include "asy_key_generator.h"
#include "cipher.h"
#include "key_pair.h"
#include "memory.h"
#include "memory_mock.h"
#include "openssl_class.h"
#include "x509_crl.h"
#include "x509_crl_openssl.h"
#include"x509_crl_entry_openssl.h"

using namespace std;
using namespace testing::ext;

namespace {
constexpr int TEST_VERSION = 3;
constexpr int TEST_EXT_VERSION = 4;
constexpr int TEST_OFFSET_TIME = 1000;
constexpr int TEST_SN = 1000;
constexpr int TEST_TIME = 1986598400;
constexpr int TEST_OFFSET = 10;
constexpr int TEST_CRL_LEN = 256;

static char g_testErrorCert[] =
"-----BEGIN CERTIFICATE-----\r\n"
"MIIEQDCCAyigAwIBAgIQICAIMRlDU0ytSEUfNeOUJTANBgkqhkiG9w0BAQsFADBcMQswCQYDVQQG\r\n"
"EwJDTjEPMA0GA1UECgwGSHVhd2VpMRMwEQYDVQQLDApIdWF3ZWkgQ0JHMScwJQYDVQQDDB5IdWF3\r\n"
"ZWkgQ0JHIE1vYmlsZSBFcXVpcG1lbnQgQ0EwHhcNMjAwODMxMTE0MzUzWhcNMzAwODI5MTE0MzUz\r\n"
"WjBvMQswCQYDVQQGEwJDTjEPMA0GA1UECgwGSHVhd2VpMRMwEQYDVQQLDApIdWF3ZWkgQ0JHMTow\r\n"
"OAYDVQQDDDFIVUFXRUlfSFdKQURfODE4ZjhjNDUtOGNmNC00ZTM2LTkxOTMtNTQ5OWMwNzM0YzM4\r\n"
"MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEArZcfL6ot4z6k3T4X3C26EI557Zvrx9Ci\r\n"
"hNx6RMy+vjXa3E4BkxwZ3r0ADbc+msJOq0IyQJNujaLq35oQvJgMIvBn1xFurBXdOzbygc7G9GKt\r\n"
"sb4rmKUP0QEPHM2/TrxAJT2LNPLrOE047ESe2X76FpDY6oZdsoGJ5I9m/mlfGsxR1l1TeUjwA/Rs\r\n"
"UtISR66aastBy2tU7IubP0B0Gceqy9DnjTQtY9OpkOql08H20C30iCVijK6BmP43X4OMz2MS0leV\r\n"
"K0AHmhiv6ufu166Xtc2JOXRk/MJ+53iprvVEUowKY/ZATUz6iDHDZYM3MdQV+VbFrOevUceOSweY\r\n"
"PaXCzwIDAQABo4HqMIHnMB8GA1UdIwQYMBaAFDXT2UhPcFFNI7Ey1dXdJSHOBS7dMB0GA1UdDgQW\r\n"
"BBSndBqCYYcTB1kMNhYMM4r/vDLteTARBglghkgBhvhCAQEEBAMCBsAwCwYDVR0PBAQDAgTwMGYG\r\n"
"A1UdHwRfMF0wW6BZoFeGVWh0dHA6Ly9jcGtpLWNhd2ViLmh1YXdlaS5jb20vY3BraS9zZXJ2bGV0\r\n"
"L2NybEZpbGVEb3duLmNybD9jZXJ0eXBlPTQmeWVhcj0vY3JsMjAyMC5jcmwwHQYDVR0lBBYwFAYI\r\n"
"KwYBBQUHAwEGCCsGAQUFBwMCMA0GCSqGSIb3DQEBCwUAA4IBAQCNTqZHRy7BJ+KFOflqwYkeD1Yd\r\n"
"K5XxcZUykw8AefX3SMYzSBzy7IIIhM5bvhWF6r5NnBJYqmyQfy+3K0Z0LQXfsY95U9JBkKl3nQPn\r\n"
"p1PzV8wLp/XYE7ePsbIbjQ1gQdC47sTDjoA73X4wEchdEVJBNUs2e15HRxbzaVJ6ABSKI9AHkxKv\r\n"
"o9iYKFozQaL4y+3Y+Yei/N1kiZZayJqC1uXq45kelc3SCJrVPE4g/Uspf2jjp7xsS+MkmxvSAT9X\r\n"
"OJeDWEeXyt7tvJeodRQgGZVhdtN78mtoaqWqD9Z6a6wpWdC4sZCWJfMjCu4Wd889Pn4MT5DBKBN3\r\n"
"f3+JpOzUJlM9\r\n"
"-----END CERTIFICATE-----\r\n";

static char g_testCert[] =
"-----BEGIN CERTIFICATE-----\r\n"
"MIID/jCCAuagAwIBAgIBATANBgkqhkiG9w0BAQsFADCBjDELMAkGA1UEBhMCQ04x\r\n"
"ETAPBgNVBAgMCHNoYW5naGFpMQ8wDQYDVQQHDAZodWF3ZWkxFTATBgNVBAoMDHd3\r\n"
"dy50ZXN0LmNvbTENMAsGA1UECwwEdGVzdDEVMBMGA1UEAwwMd3d3LnRlc3QuY29t\r\n"
"MRwwGgYJKoZIhvcNAQkBFg10ZXN0QHRlc3QuY29tMB4XDTIyMDgyOTA2NTUwM1oX\r\n"
"DTIzMDgyOTA2NTUwM1owezELMAkGA1UEBhMCQ04xETAPBgNVBAgMCHNoYW5naGFp\r\n"
"MRUwEwYDVQQKDAx3d3cudGVzdC5jb20xDTALBgNVBAsMBHRlc3QxFTATBgNVBAMM\r\n"
"DHd3dy50ZXN0LmNvbTEcMBoGCSqGSIb3DQEJARYNdGVzdEB0ZXN0LmNvbTCCASIw\r\n"
"DQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAJmY9T4SzXXwKvfMvnvMWY7TqUJK\r\n"
"jnWf2Puv0YUQ2fdvyoKQ2LQXdtzoUL53j587oI+IXelOr7dg020zPyun0cmZHZ4y\r\n"
"l/qAcrWbDjZeEGcbbb5UtQtn1WOEnv8pkXluO355mbZQUKK9L3gFWseXJKGbIXw0\r\n"
"NRpaJZzqvPor4m3a5pmJKPHOlivUdYfLaKSkNj3DlaFzCWKV82k5ee6gzVyETtG+\r\n"
"XN+vq8qLybT+fIFsLNMmAHzRxlqz3NiH7yh+1/p/Knvf8bkkRVR2btH51RyX2RSu\r\n"
"DjPM0/VRL8fxDSDeWBq+Gvn/E6AbOVMmkx63tcyWHhklCSaZtyz7kq39TQMCAwEA\r\n"
"AaN7MHkwCQYDVR0TBAIwADAsBglghkgBhvhCAQ0EHxYdT3BlblNTTCBHZW5lcmF0\r\n"
"ZWQgQ2VydGlmaWNhdGUwHQYDVR0OBBYEFFiFDysfADQCzRZCOSPupQxFicwzMB8G\r\n"
"A1UdIwQYMBaAFNYQRQiPsG8HefOTsmsVhaVjY7IPMA0GCSqGSIb3DQEBCwUAA4IB\r\n"
"AQAeppxf6sKQJxJQXKPTT3xHKaskidNwDBbOSIvnVvWXicZXDs+1sF6tUaRgvPxL\r\n"
"OL58+P2Jy0tfSwj2WhqQRGe9MvQ5iFHcdelZc0ciW6EQ0VDHIaDAQc2nQzej/79w\r\n"
"UE7BJJV3b9n1be2iCsuodKO14pOkMb84WcIxng+8SD+MiFqV5BPO1QyKGdO1PE1b\r\n"
"+evjyTpFSTgZf2Mw3fGtu5hfEXyHw1lnsFY2MlSwiRlAym/gm4aXy+4H6LyXKd56\r\n"
"UYQ6fituD0ziaw3RI6liyIe7aENHCkZf6bAvMRhk4QiU4xu6emwX8Qt1bT7RthP0\r\n"
"1Vsro0IOeXT9WAcqEtQUegsi\r\n"
"-----END CERTIFICATE-----\r\n";

static char g_testCrl[] =
"-----BEGIN X509 CRL-----\r\n"
"MIIB/DCB5QIBATANBgkqhkiG9w0BAQsFADCBjDELMAkGA1UEBhMCQ04xETAPBgNV\r\n"
"BAgMCHNoYW5naGFpMQ8wDQYDVQQHDAZodWF3ZWkxFTATBgNVBAoMDHd3dy50ZXN0\r\n"
"LmNvbTENMAsGA1UECwwEdGVzdDEVMBMGA1UEAwwMd3d3LnRlc3QuY29tMRwwGgYJ\r\n"
"KoZIhvcNAQkBFg10ZXN0QHRlc3QuY29tFw0yMjA4MjkwNzAwMTRaFw0yMjA5Mjgw\r\n"
"NzAwMTRaMBQwEgIBARcNMjIwODI5MDY1OTUzWqAOMAwwCgYDVR0UBAMCAQAwDQYJ\r\n"
"KoZIhvcNAQELBQADggEBAHpfFhhUR59OAvOSuKDQUC5tKeLEuPbY8bYdmQVI8EFd\r\n"
"xDkZTXmT3CX1aDPYKVsG/jH9KPAmCV/ODKEGiJzclb3Z4am7tT+Wy4mpXypNS1od\r\n"
"wPDcQGsMrjT6iSp6JImiB0dDDSleBTBcYR/hhtFaiGSncyqJ0mhyaXPxIkNOO6nY\r\n"
"v+rcTEPQWavViDRyNDhnTbN868I3fzFVBcidF13CA0sCJ91ZvsE9h/YmPO2+e0YE\r\n"
"IUgzn37UOiLGObCVBY12QjGiuvVvCl7ncncsFEJuGfvONOqyFHjyxDHo5W0fqTn2\r\n"
"eCtiNcgUr9Kz2bwCmvEXhP7PuF4RMLq4vfzi0YjCG98=\r\n"
"-----END X509 CRL-----\r\n";

HcfX509Crl *g_x509Crl = nullptr;
HcfKeyPair *g_keyPair = nullptr;
ASN1_TIME *g_lastUpdate = nullptr;
ASN1_TIME *g_nextUpdate = nullptr;
ASN1_TIME *g_rvTime = nullptr;
HcfEncodingBlob *g_crlDerInStream = nullptr;
unsigned char *g_tbs = nullptr;
unsigned char *g_signatureStr = nullptr;
int g_signatureLen = 0;
class CryptoX509CrlTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

static unsigned char *GetCrlStream()
{
    unsigned char *buf, *p;
    time_t t;
    X509_NAME *issuer;
    X509_CRL *crl = nullptr;
    X509_REVOKED *revoked;
    EVP_PKEY *prikey;
    ASN1_INTEGER *serial;

    // Generate keyPair
    HcfAsyKeyGenerator *generator = nullptr;
    HcfAsyKeyGeneratorCreate("RSA1024|PRIMES_3", &generator);
    generator->generateKeyPair(generator, nullptr, &g_keyPair);
    RSA *rsaPrikey = (reinterpret_cast<HcfOpensslRsaPriKey *>(g_keyPair->priKey))->sk;
    prikey = EVP_PKEY_new();
    EVP_PKEY_assign_RSA(prikey, rsaPrikey);

    // Set version
    crl = X509_CRL_new();
    int ret = X509_CRL_set_version(crl, TEST_VERSION);

    // Set Issuer
    const char *tmp = "CRL issuer";
    issuer = X509_NAME_new();
    ret = X509_NAME_add_entry_by_NID(issuer, NID_commonName, V_ASN1_PRINTABLESTRING,
        reinterpret_cast<const unsigned char *>(tmp), 10, -1, 0);
    ret = X509_CRL_set_issuer_name(crl, issuer);

    // Set last time
    g_lastUpdate = ASN1_TIME_new();
    t = time(nullptr);
    ASN1_TIME_set(g_lastUpdate, t + TEST_OFFSET_TIME);
    ret = X509_CRL_set_lastUpdate(crl, g_lastUpdate);

    // Set next time
    g_nextUpdate = ASN1_TIME_new();
    t = TEST_TIME;
    ASN1_TIME_set(g_nextUpdate, t);
    ret = X509_CRL_set_nextUpdate(crl, g_nextUpdate);

    // Add serial number
    revoked = X509_REVOKED_new();
    serial = ASN1_INTEGER_new();
    ret = ASN1_INTEGER_set(serial, TEST_SN);
    ret = X509_REVOKED_set_serialNumber(revoked, serial);

    // Set revocationDate
    g_rvTime = ASN1_TIME_new();
    t = TEST_TIME;
    ASN1_TIME_set(g_rvTime, t);
    ret = X509_CRL_set_nextUpdate(crl, g_rvTime);
    ret = X509_REVOKED_set_revocationDate(revoked, g_rvTime);
    ret = X509_CRL_add0_revoked(crl, revoked);

    // Sort
    ret = X509_CRL_sort(crl);

    // Sign
    ret = X509_CRL_sign(crl, prikey, EVP_md5());

    int len = i2d_X509_CRL(crl, nullptr);
    buf =  static_cast<unsigned char *>(malloc(len + TEST_OFFSET));
    p = buf;
    len = i2d_X509_CRL(crl, &p);

    // Get sign
    const ASN1_BIT_STRING *asn1Signature = nullptr;
    X509_CRL_get0_signature(crl, &asn1Signature, nullptr);
    g_signatureStr = const_cast<unsigned char *>(ASN1_STRING_get0_data(asn1Signature));
    g_signatureLen = ASN1_STRING_length(asn1Signature);
    // Get Tbs
    i2d_re_X509_CRL_tbs(crl, &g_tbs);

    return buf;
}

void CryptoX509CrlTest::SetUpTestCase()
{
    HcfX509Crl *x509Crl = nullptr;
    g_crlDerInStream = (HcfEncodingBlob *)HcfMalloc(sizeof(HcfEncodingBlob), 0);
    unsigned char *crlStream = GetCrlStream();
    g_crlDerInStream->data = (uint8_t *)crlStream;
    g_crlDerInStream->encodingFormat = HCF_FORMAT_DER;
    g_crlDerInStream->len = TEST_CRL_LEN;
    HcfX509CrlCreate(g_crlDerInStream, &x509Crl);
    g_x509Crl = (HcfX509Crl *)x509Crl;
}
void CryptoX509CrlTest::TearDownTestCase()
{
    if (g_x509Crl != nullptr) {
        HcfObjDestroy(g_x509Crl);
        g_x509Crl = nullptr;
    }
    if (g_keyPair != nullptr) {
        HcfObjDestroy(g_keyPair);
        g_keyPair = nullptr;
    }
    if (g_lastUpdate != nullptr) {
        ASN1_TIME_free(g_lastUpdate);
        g_lastUpdate = nullptr;
    }
    if (g_nextUpdate != nullptr) {
        ASN1_TIME_free(g_nextUpdate);
        g_nextUpdate = nullptr;
    }
    if (g_rvTime != nullptr) {
        ASN1_TIME_free(g_rvTime);
        g_rvTime = nullptr;
    }
    if (g_crlDerInStream != nullptr) {
        HcfFree(g_crlDerInStream->data);
        HcfFree(g_crlDerInStream);
        g_crlDerInStream = nullptr;
    }
}
void CryptoX509CrlTest::SetUp() {}
void CryptoX509CrlTest::TearDown() {}

// Begin test crl create, test crl create PEM true
HWTEST_F(CryptoX509CrlTest, X509CrlTest001, TestSize.Level0)
{
    HcfX509Crl *x509Crl = nullptr;
    HcfEncodingBlob inStreamCrl = { 0 };
    inStreamCrl.data = (uint8_t *)g_testCrl;
    inStreamCrl.encodingFormat = HCF_FORMAT_PEM;
    inStreamCrl.len = strlen(g_testCrl) + 1;
    HcfResult ret = HcfX509CrlCreate(&inStreamCrl, &x509Crl);
    EXPECT_EQ(ret, HCF_SUCCESS);
    EXPECT_NE(x509Crl, nullptr);
    HcfObjDestroy(x509Crl);
}

// Test crl create DER true
HWTEST_F(CryptoX509CrlTest, X509CrlTest002, TestSize.Level0)
{
    HcfX509Crl *x509Crl = nullptr;
    HcfResult ret = HcfX509CrlCreate(g_crlDerInStream, &x509Crl);
    EXPECT_EQ(ret, HCF_SUCCESS);
    EXPECT_NE(x509Crl, nullptr);
    HcfObjDestroy(x509Crl);
}

// Test crl create error | encodingFormat
HWTEST_F(CryptoX509CrlTest, X509CrlTest003, TestSize.Level0)
{
    HcfX509Crl *x509Crl = nullptr;
    HcfEncodingBlob inStreamCrl = { 0 };
    inStreamCrl.data = (uint8_t *)g_testCrl;
    inStreamCrl.encodingFormat = HCF_FORMAT_DER;
    inStreamCrl.len = strlen(g_testCrl) + 1;
    HcfResult ret = HcfX509CrlCreate(&inStreamCrl, &x509Crl);
    EXPECT_NE(ret, HCF_SUCCESS);
    EXPECT_EQ(x509Crl, nullptr);
}

// Test crl create error | Crl data
HWTEST_F(CryptoX509CrlTest, X509CrlTest004, TestSize.Level0)
{
    HcfX509Crl *x509Crl = nullptr;
    HcfEncodingBlob inStreamCrl = { 0 };
    inStreamCrl.data = nullptr;
    inStreamCrl.encodingFormat = HCF_FORMAT_PEM;
    inStreamCrl.len = strlen(g_testCrl) + 1;
    HcfResult ret = HcfX509CrlCreate(&inStreamCrl, &x509Crl);
    EXPECT_NE(ret, HCF_SUCCESS);
    EXPECT_EQ(x509Crl, nullptr);
}

// Test crl create error | Crl len
HWTEST_F(CryptoX509CrlTest, X509CrlTest005, TestSize.Level0)
{
    HcfX509Crl *x509Crl = nullptr;
    HcfEncodingBlob inStreamCrl = { 0 };
    inStreamCrl.data = (uint8_t *)g_testCrl;
    inStreamCrl.encodingFormat = HCF_FORMAT_PEM;
    inStreamCrl.len = 0;
    HcfResult ret = HcfX509CrlCreate(&inStreamCrl, &x509Crl);
    EXPECT_NE(ret, HCF_SUCCESS);
    EXPECT_EQ(x509Crl, nullptr);
}

// Test crl create error | Crl nullptr
HWTEST_F(CryptoX509CrlTest, X509CrlTest006, TestSize.Level0)
{
    HcfX509Crl *x509Crl = nullptr;
    HcfEncodingBlob *inStreamCrl = nullptr;
    HcfResult ret = HcfX509CrlCreate(inStreamCrl, &x509Crl);
    EXPECT_NE(ret, HCF_SUCCESS);
    EXPECT_EQ(x509Crl, nullptr);
}

// Begin test crl isRevoked, test crl isRevoked true
HWTEST_F(CryptoX509CrlTest, X509CrlTest011, TestSize.Level0)
{
    // Get cert
    HcfX509Certificate *x509Cert = nullptr;
    HcfEncodingBlob inStreamCert = { 0 };
    inStreamCert.data = (uint8_t *)g_testCert;
    inStreamCert.encodingFormat = HCF_FORMAT_PEM;
    inStreamCert.len = strlen(g_testCert) + 1;
    HcfResult ret = HcfX509CertificateCreate(&inStreamCert, &x509Cert);
    EXPECT_EQ(ret, HCF_SUCCESS);
    EXPECT_NE(x509Cert, nullptr);

    // Get crl
    HcfX509Crl *x509Crl = nullptr;
    HcfEncodingBlob inStreamCrl = { 0 };
    inStreamCrl.data = (uint8_t *)g_testCrl;
    inStreamCrl.encodingFormat = HCF_FORMAT_PEM;
    inStreamCrl.len = strlen(g_testCrl) + 1;
    ret = HcfX509CrlCreate(&inStreamCrl, &x509Crl);
    EXPECT_EQ(ret, HCF_SUCCESS);
    EXPECT_NE(x509Crl, nullptr);

    bool resIsRevoked = x509Crl->base.isRevoked((HcfCrl *)x509Crl, (HcfCertificate *)x509Cert);
    EXPECT_EQ(resIsRevoked, true);
    HcfObjDestroy(x509Crl);
    HcfObjDestroy(x509Cert);
}

// Test crl isRevoked error | crl null
HWTEST_F(CryptoX509CrlTest, X509CrlTest012, TestSize.Level0)
{
    // Get cert
    HcfX509Certificate *x509Cert = nullptr;
    HcfEncodingBlob inStreamCert = { 0 };
    inStreamCert.data = (uint8_t *)g_testCert;
    inStreamCert.encodingFormat = HCF_FORMAT_PEM;
    inStreamCert.len = strlen(g_testCert) + 1;
    HcfResult ret = HcfX509CertificateCreate(&inStreamCert, &x509Cert);
    EXPECT_EQ(ret, HCF_SUCCESS);
    EXPECT_NE(x509Cert, nullptr);

    bool resIsRevoked = g_x509Crl->base.isRevoked(nullptr, (HcfCertificate *)x509Cert);
    EXPECT_EQ(resIsRevoked, false);
    HcfObjDestroy(x509Cert);
}

// Test crl isRevoked error | x509Cert null
HWTEST_F(CryptoX509CrlTest, X509CrlTest013, TestSize.Level0)
{
    // Get crl
    HcfX509Crl *x509Crl = nullptr;
    HcfEncodingBlob inStreamCrl = { 0 };
    inStreamCrl.data = (uint8_t *)g_testCrl;
    inStreamCrl.encodingFormat = HCF_FORMAT_PEM;
    inStreamCrl.len = strlen(g_testCrl) + 1;
    HcfResult ret = HcfX509CrlCreate(&inStreamCrl, &x509Crl);
    EXPECT_EQ(ret, HCF_SUCCESS);
    EXPECT_NE(x509Crl, nullptr);

    bool resIsRevoked = x509Crl->base.isRevoked((HcfCrl *)x509Crl, nullptr);
    EXPECT_EQ(resIsRevoked, false);
    HcfObjDestroy(x509Crl);
}

// Test crl isRevoked error | x509Crl error
HWTEST_F(CryptoX509CrlTest, X509CrlTest014, TestSize.Level0)
{
    // Get cert
    HcfX509Certificate *x509Cert = nullptr;
    HcfEncodingBlob inStreamCert = { 0 };
    inStreamCert.data = (uint8_t *)g_testCert;
    inStreamCert.encodingFormat = HCF_FORMAT_PEM;
    inStreamCert.len = strlen(g_testCert) + 1;
    HcfResult ret = HcfX509CertificateCreate(&inStreamCert, &x509Cert);
    EXPECT_EQ(ret, HCF_SUCCESS);
    EXPECT_NE(x509Cert, nullptr);

    // Get crl
    HcfX509Crl *x509Crl = nullptr;
    ret = HcfX509CrlCreate(g_crlDerInStream, &x509Crl);
    EXPECT_EQ(ret, HCF_SUCCESS);
    EXPECT_NE(x509Crl, nullptr);

    bool resIsRevoked = x509Crl->base.isRevoked((HcfCrl *)x509Crl, (HcfCertificate *)x509Cert);
    EXPECT_EQ(resIsRevoked, false);
    HcfObjDestroy(x509Cert);
    HcfObjDestroy(x509Crl);
}

// Test crl isRevoked error | x509Crl error
HWTEST_F(CryptoX509CrlTest, X509CrlTest015, TestSize.Level0)
{
    // Get cert
    HcfX509Certificate *x509Cert = nullptr;
    HcfEncodingBlob inStreamCert = { 0 };
    inStreamCert.data = (uint8_t *)g_testErrorCert;
    inStreamCert.encodingFormat = HCF_FORMAT_PEM;
    inStreamCert.len = strlen(g_testErrorCert) + 1;
    HcfResult ret = HcfX509CertificateCreate(&inStreamCert, &x509Cert);
    EXPECT_EQ(ret, HCF_SUCCESS);
    EXPECT_NE(x509Cert, nullptr);

    // Get crl
    HcfX509Crl *x509Crl = nullptr;
    HcfEncodingBlob inStreamCrl = { 0 };
    inStreamCrl.data = (uint8_t *)g_testCrl;
    inStreamCrl.encodingFormat = HCF_FORMAT_PEM;
    inStreamCrl.len = strlen(g_testCrl) + 1;
    ret = HcfX509CrlCreate(&inStreamCrl, &x509Crl);
    EXPECT_EQ(ret, HCF_SUCCESS);
    EXPECT_NE(x509Crl, nullptr);

    bool resIsRevoked = x509Crl->base.isRevoked((HcfCrl *)x509Crl, (HcfCertificate *)x509Cert);
    EXPECT_EQ(resIsRevoked, false);
    HcfObjDestroy(x509Cert);
    HcfObjDestroy(x509Crl);
}

// Test crl GetType true
HWTEST_F(CryptoX509CrlTest, X509CrlTest021, TestSize.Level0)
{
    // Get crl
    HcfX509Crl *x509Crl = nullptr;
    HcfEncodingBlob inStreamCrl = { 0 };
    inStreamCrl.data = (uint8_t *)g_testCrl;
    inStreamCrl.encodingFormat = HCF_FORMAT_PEM;
    inStreamCrl.len = strlen(g_testCrl) + 1;
    HcfResult ret = HcfX509CrlCreate(&inStreamCrl, &x509Crl);
    EXPECT_EQ(ret, HCF_SUCCESS);
    EXPECT_NE(x509Crl, nullptr);

    const char *resStr = x509Crl->base.getType((HcfCrl *)x509Crl);
    EXPECT_STREQ(resStr, "X509");
    HcfObjDestroy(x509Crl);
}

// Test crl GetType error
HWTEST_F(CryptoX509CrlTest, X509CrlTest022, TestSize.Level0)
{
    // Get crl
    HcfX509Crl *x509Crl = nullptr;
    HcfEncodingBlob inStreamCrl = { 0 };
    inStreamCrl.data = (uint8_t *)g_testCrl;
    inStreamCrl.encodingFormat = HCF_FORMAT_PEM;
    inStreamCrl.len = strlen(g_testCrl) + 1;
    HcfResult ret = HcfX509CrlCreate(&inStreamCrl, &x509Crl);
    EXPECT_EQ(ret, HCF_SUCCESS);
    EXPECT_NE(x509Crl, nullptr);

    const char *resStr = x509Crl->base.getType(nullptr);
    EXPECT_EQ(resStr, nullptr);
    HcfObjDestroy(x509Crl);
}

// Test crl getEncoded DER true
HWTEST_F(CryptoX509CrlTest, X509CrlTest031, TestSize.Level0)
{
    HcfEncodingBlob inStreamInput = { 0 };
    HcfResult ret = g_x509Crl->getEncoded(g_x509Crl, &inStreamInput);
    EXPECT_EQ(ret, HCF_SUCCESS);
    HcfX509Crl *crl2 = nullptr;
    ret = HcfX509CrlCreate(&inStreamInput, &crl2);
    EXPECT_EQ(ret, HCF_SUCCESS);
    EXPECT_NE(crl2, nullptr);
    HcfObjDestroy(crl2);
    HcfFree(inStreamInput.data);
}

// Test crl getEncoded PEM true
HWTEST_F(CryptoX509CrlTest, X509CrlTest032, TestSize.Level0)
{
    // Get crl
    HcfX509Crl *x509Crl = nullptr;
    HcfEncodingBlob inStreamCrl = { 0 };
    inStreamCrl.data = (uint8_t *)g_testCrl;
    inStreamCrl.encodingFormat = HCF_FORMAT_PEM;
    inStreamCrl.len = strlen(g_testCrl) + 1;
    HcfResult ret = HcfX509CrlCreate(&inStreamCrl, &x509Crl);
    EXPECT_EQ(ret, HCF_SUCCESS);
    EXPECT_NE(x509Crl, nullptr);

    HcfEncodingBlob inStreamInput = { 0 };
    ret = x509Crl->getEncoded(x509Crl, &inStreamInput);
    EXPECT_EQ(ret, HCF_SUCCESS);
    HcfX509Crl *crl2 = nullptr;
    ret = HcfX509CrlCreate(&inStreamInput, &crl2);
    EXPECT_EQ(ret, HCF_SUCCESS);
    EXPECT_NE(crl2, nullptr);
    HcfFree(inStreamInput.data);
    HcfObjDestroy(crl2);
    HcfObjDestroy(x509Crl);
}

// Test crl getEncoded error
HWTEST_F(CryptoX509CrlTest, X509CrlTest033, TestSize.Level0)
{
    HcfResult ret = g_x509Crl->getEncoded(g_x509Crl, nullptr);
    EXPECT_NE(ret, HCF_SUCCESS);
}

// Test crl getEncoded error
HWTEST_F(CryptoX509CrlTest, X509CrlTest034, TestSize.Level0)
{
    HcfEncodingBlob inStreamInput = { 0 };
    HcfResult ret = g_x509Crl->getEncoded(nullptr, &inStreamInput);
    EXPECT_NE(ret, HCF_SUCCESS);
}

// Test crl getEncoded error
HWTEST_F(CryptoX509CrlTest, X509CrlTest035, TestSize.Level0)
{
    HcfResult ret = g_x509Crl->getEncoded(nullptr, nullptr);
    EXPECT_NE(ret, HCF_SUCCESS);
}

// Test crl verify true
HWTEST_F(CryptoX509CrlTest, X509CrlTest041, TestSize.Level0)
{
    HcfResult ret = g_x509Crl->verify(g_x509Crl, g_keyPair->pubKey);
    EXPECT_EQ(ret, HCF_SUCCESS);
}

// Test crl verify false
HWTEST_F(CryptoX509CrlTest, X509CrlTest042, TestSize.Level0)
{
    HcfKeyPair *keyPair = nullptr;
    HcfAsyKeyGenerator *generator = nullptr;
    HcfAsyKeyGeneratorCreate("RSA1024|PRIMES_3", &generator);
    generator->generateKeyPair(generator, nullptr, &keyPair);

    HcfResult ret = g_x509Crl->verify(g_x509Crl, keyPair->pubKey);
    EXPECT_NE(ret, HCF_SUCCESS);
    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

// Test crl verify false
HWTEST_F(CryptoX509CrlTest, X509CrlTest043, TestSize.Level0)
{
    HcfResult ret = g_x509Crl->verify(nullptr, g_keyPair->pubKey);
    EXPECT_NE(ret, HCF_SUCCESS);
}

// Test crl verify false
HWTEST_F(CryptoX509CrlTest, X509CrlTest044, TestSize.Level0)
{
    HcfResult ret = g_x509Crl->verify(g_x509Crl, nullptr);
    EXPECT_NE(ret, HCF_SUCCESS);
}

// Test crl verify false
HWTEST_F(CryptoX509CrlTest, X509CrlTest045, TestSize.Level0)
{
    // Get crl
    HcfX509Crl *x509Crl = nullptr;
    HcfEncodingBlob inStreamCrl = { 0 };
    inStreamCrl.data = (uint8_t *)g_testCrl;
    inStreamCrl.encodingFormat = HCF_FORMAT_PEM;
    inStreamCrl.len = strlen(g_testCrl) + 1;
    HcfResult ret = HcfX509CrlCreate(&inStreamCrl, &x509Crl);
    EXPECT_EQ(ret, HCF_SUCCESS);
    EXPECT_NE(x509Crl, nullptr);

    ret = x509Crl->verify(x509Crl, nullptr);
    EXPECT_NE(ret, HCF_SUCCESS);
    HcfObjDestroy(x509Crl);
}

// Test crl getVersion true
HWTEST_F(CryptoX509CrlTest, X509CrlTest051, TestSize.Level0)
{
    long version = g_x509Crl->getVersion(g_x509Crl);
    EXPECT_EQ(version, TEST_EXT_VERSION);
}

// Test crl getVersion false
HWTEST_F(CryptoX509CrlTest, X509CrlTest052, TestSize.Level0)
{
    long version = g_x509Crl->getVersion(nullptr);
    EXPECT_EQ(version, -1);
}

// Test crl getIssuerName true
HWTEST_F(CryptoX509CrlTest, X509CrlTest061, TestSize.Level0)
{
    HcfBlob out = { 0 };
    HcfResult ret = g_x509Crl->getIssuerName(g_x509Crl, &out);
    EXPECT_EQ(ret, HCF_SUCCESS);
    EXPECT_NE(out.data, nullptr);
    EXPECT_STREQ("/CN=CRL issuer", (char *)(out.data));
    HcfFree(out.data);
}

// Test crl getIssuerName false
HWTEST_F(CryptoX509CrlTest, X509CrlTest062, TestSize.Level0)
{
    HcfBlob out = { 0 };
    HcfResult ret = g_x509Crl->getIssuerName(nullptr, &out);
    EXPECT_NE(ret, HCF_SUCCESS);
    EXPECT_EQ(out.data, nullptr);
}

// Test crl getIssuerName false
HWTEST_F(CryptoX509CrlTest, X509CrlTest063, TestSize.Level0)
{
    HcfResult ret = g_x509Crl->getIssuerName(g_x509Crl, nullptr);
    EXPECT_NE(ret, HCF_SUCCESS);
}

// Test crl getIssuerName false
HWTEST_F(CryptoX509CrlTest, X509CrlTest064, TestSize.Level0)
{
    HcfResult ret = g_x509Crl->getIssuerName(nullptr, nullptr);
    EXPECT_NE(ret, HCF_SUCCESS);
}

// Test crl getLastUpdate true
HWTEST_F(CryptoX509CrlTest, X509CrlTest071, TestSize.Level0)
{
    HcfBlob out = { 0 };
    HcfResult ret = g_x509Crl->getLastUpdate(g_x509Crl, &out);
    EXPECT_EQ(ret, HCF_SUCCESS);
    EXPECT_NE(out.data, nullptr);
    EXPECT_STREQ((char *)g_lastUpdate->data, (char *)out.data);
    HcfFree(out.data);
}

// Test crl getLastUpdate false
HWTEST_F(CryptoX509CrlTest, X509CrlTest072, TestSize.Level0)
{
    HcfBlob out = { 0 };
    HcfResult ret = g_x509Crl->getLastUpdate(nullptr, &out);
    EXPECT_NE(ret, HCF_SUCCESS);
    EXPECT_EQ(out.data, nullptr);
}

// Test crl getLastUpdate false
HWTEST_F(CryptoX509CrlTest, X509CrlTest073, TestSize.Level0)
{
    HcfResult ret = g_x509Crl->getLastUpdate(g_x509Crl, nullptr);
    EXPECT_NE(ret, HCF_SUCCESS);
}

// Test crl getLastUpdate false
HWTEST_F(CryptoX509CrlTest, X509CrlTest074, TestSize.Level0)
{
    HcfResult ret = g_x509Crl->getLastUpdate(nullptr, nullptr);
    EXPECT_NE(ret, HCF_SUCCESS);
}

// Test crl getNextUpdate true
HWTEST_F(CryptoX509CrlTest, X509CrlTest081, TestSize.Level0)
{
    HcfBlob out = { 0 };
    HcfResult ret = g_x509Crl->getNextUpdate(g_x509Crl, &out);
    EXPECT_EQ(ret, HCF_SUCCESS);
    EXPECT_NE(out.data, nullptr);
    EXPECT_STREQ((char *)g_nextUpdate->data, (char *)out.data);
    HcfFree(out.data);
}

// Test crl getNextUpdate false
HWTEST_F(CryptoX509CrlTest, X509CrlTest082, TestSize.Level0)
{
    HcfBlob out = { 0 };
    HcfResult ret = g_x509Crl->getNextUpdate(nullptr, &out);
    EXPECT_NE(ret, HCF_SUCCESS);
    EXPECT_EQ(out.data, nullptr);
}

// Test crl getNextUpdate false
HWTEST_F(CryptoX509CrlTest, X509CrlTest083, TestSize.Level0)
{
    HcfResult ret = g_x509Crl->getNextUpdate(g_x509Crl, nullptr);
    EXPECT_NE(ret, HCF_SUCCESS);
}

// Test crl getNextUpdate false
HWTEST_F(CryptoX509CrlTest, X509CrlTest084, TestSize.Level0)
{
    HcfResult ret = g_x509Crl->getNextUpdate(nullptr, nullptr);
    EXPECT_NE(ret, HCF_SUCCESS);
}

// Test crl getRevokedCert true
HWTEST_F(CryptoX509CrlTest, X509CrlTest091, TestSize.Level0)
{
    HcfX509CrlEntry *crlEntry = nullptr;
    HcfResult ret = g_x509Crl->getRevokedCert(g_x509Crl, 1000, &crlEntry);
    EXPECT_EQ(ret, HCF_SUCCESS);
    EXPECT_NE(crlEntry, nullptr);
    HcfObjDestroy(crlEntry);
}

// Test crl getRevokedCert false
HWTEST_F(CryptoX509CrlTest, X509CrlTest092, TestSize.Level0)
{
    HcfX509CrlEntry *crlEntry = nullptr;
    HcfResult ret = g_x509Crl->getRevokedCert(g_x509Crl, 9999, &crlEntry);
    EXPECT_NE(ret, HCF_SUCCESS);
}

// Test crl getRevokedCert false
HWTEST_F(CryptoX509CrlTest, X509CrlTest093, TestSize.Level0)
{
    HcfResult ret = g_x509Crl->getRevokedCert(g_x509Crl, 1000, nullptr);
    EXPECT_NE(ret, HCF_SUCCESS);
}

// Test crl getRevokedCert false
HWTEST_F(CryptoX509CrlTest, X509CrlTest094, TestSize.Level0)
{
    HcfX509CrlEntry *crlEntry = nullptr;
    HcfResult ret = g_x509Crl->getRevokedCert(nullptr, 1000, &crlEntry);
    EXPECT_NE(ret, HCF_SUCCESS);
}

// Test crl getRevokedCert false
HWTEST_F(CryptoX509CrlTest, X509CrlTest095, TestSize.Level0)
{
    HcfResult ret = g_x509Crl->getRevokedCert(nullptr, 1000, nullptr);
    EXPECT_NE(ret, HCF_SUCCESS);
}

// Test crl entry getSerialNumber true
HWTEST_F(CryptoX509CrlTest, X509CrlTest101, TestSize.Level0)
{
    HcfX509CrlEntry *crlEntry = nullptr;
    HcfResult ret = g_x509Crl->getRevokedCert(g_x509Crl, 1000, &crlEntry);
    EXPECT_EQ(ret, HCF_SUCCESS);
    EXPECT_NE(crlEntry, nullptr);
    long sn = crlEntry->getSerialNumber(crlEntry);
    EXPECT_EQ(sn, 1000);
    HcfObjDestroy(crlEntry);
}

// Test crl entry getSerialNumber false
HWTEST_F(CryptoX509CrlTest, X509CrlTest102, TestSize.Level0)
{
    HcfX509CrlEntry *crlEntry = nullptr;
    HcfResult ret = g_x509Crl->getRevokedCert(g_x509Crl, 1000, &crlEntry);
    EXPECT_EQ(ret, HCF_SUCCESS);
    EXPECT_NE(crlEntry, nullptr);
    long sn = crlEntry->getSerialNumber(nullptr);
    EXPECT_EQ(sn, -1);
    HcfObjDestroy(crlEntry);
}

// Test crl entry getSerialNumber false
HWTEST_F(CryptoX509CrlTest, X509CrlTest103, TestSize.Level0)
{
    HcfX509CrlEntry *crlEntry = nullptr;
    HcfResult ret = g_x509Crl->getRevokedCert(g_x509Crl, 1000, &crlEntry);
    EXPECT_EQ(ret, HCF_SUCCESS);
    EXPECT_NE(crlEntry, nullptr);
    long sn = crlEntry->getSerialNumber(nullptr);
    EXPECT_EQ(sn, -1);
    HcfObjDestroy(crlEntry);
}

// Test crl entry getEncoded true
HWTEST_F(CryptoX509CrlTest, X509CrlTest111, TestSize.Level0)
{
    HcfX509CrlEntry *crlEntry = nullptr;
    HcfResult ret = g_x509Crl->getRevokedCert(g_x509Crl, 1000, &crlEntry);
    EXPECT_EQ(ret, HCF_SUCCESS);
    EXPECT_NE(crlEntry, nullptr);

    HcfEncodingBlob encodingBlob = { 0 };
    ret = crlEntry->getEncoded(crlEntry, &encodingBlob);
    EXPECT_EQ(ret, HCF_SUCCESS);
    EXPECT_NE(encodingBlob.data, nullptr);
    HcfObjDestroy(crlEntry);
    HcfFree(encodingBlob.data);
}

// Test crl entry getEncoded false
HWTEST_F(CryptoX509CrlTest, X509CrlTest112, TestSize.Level0)
{
    HcfX509CrlEntry *crlEntry = nullptr;
    HcfResult ret = g_x509Crl->getRevokedCert(g_x509Crl, 1000, &crlEntry);
    EXPECT_EQ(ret, HCF_SUCCESS);
    EXPECT_NE(crlEntry, nullptr);

    HcfEncodingBlob encodingBlob = { 0 };
    ret = crlEntry->getEncoded(nullptr, &encodingBlob);
    EXPECT_NE(ret, HCF_SUCCESS);
    EXPECT_EQ(encodingBlob.data, nullptr);
    HcfObjDestroy(crlEntry);
}

// Test crl entry getEncoded false
HWTEST_F(CryptoX509CrlTest, X509CrlTest113, TestSize.Level0)
{
    HcfX509CrlEntry *crlEntry = nullptr;
    HcfResult ret = g_x509Crl->getRevokedCert(g_x509Crl, 1000, &crlEntry);
    EXPECT_EQ(ret, HCF_SUCCESS);
    EXPECT_NE(crlEntry, nullptr);

    ret = crlEntry->getEncoded(crlEntry, nullptr);
    EXPECT_NE(ret, HCF_SUCCESS);
    HcfObjDestroy(crlEntry);
}

// Test crl entry getEncoded false
HWTEST_F(CryptoX509CrlTest, X509CrlTest114, TestSize.Level0)
{
    HcfX509CrlEntry *crlEntry = nullptr;
    HcfResult ret = g_x509Crl->getRevokedCert(g_x509Crl, 1000, &crlEntry);
    EXPECT_EQ(ret, HCF_SUCCESS);
    EXPECT_NE(crlEntry, nullptr);

    ret = crlEntry->getEncoded(nullptr, nullptr);
    EXPECT_NE(ret, HCF_SUCCESS);
    HcfObjDestroy(crlEntry);
}

// Test crl entry getCertIssuer true
HWTEST_F(CryptoX509CrlTest, X509CrlTest121, TestSize.Level0)
{
    HcfX509CrlEntry *crlEntry = nullptr;
    HcfResult ret = g_x509Crl->getRevokedCert(g_x509Crl, 1000, &crlEntry);
    EXPECT_EQ(ret, HCF_SUCCESS);
    EXPECT_NE(crlEntry, nullptr);

    HcfBlob out = { 0 };
    ret = crlEntry->getCertIssuer(crlEntry, &out);
    EXPECT_EQ(ret, HCF_SUCCESS);
    EXPECT_NE(out.data, nullptr);
    EXPECT_STREQ("/CN=CRL issuer", (char *)out.data);
    HcfObjDestroy(crlEntry);
    HcfFree(out.data);
}

// Test crl entry getCertIssuer false
HWTEST_F(CryptoX509CrlTest, X509CrlTest122, TestSize.Level0)
{
    HcfX509CrlEntry *crlEntry = nullptr;
    HcfResult ret = g_x509Crl->getRevokedCert(g_x509Crl, 1000, &crlEntry);
    EXPECT_EQ(ret, HCF_SUCCESS);
    EXPECT_NE(crlEntry, nullptr);

    HcfBlob out = { 0 };
    ret = crlEntry->getCertIssuer(nullptr, &out);
    EXPECT_NE(ret, HCF_SUCCESS);
    EXPECT_EQ(out.data, nullptr);
    HcfObjDestroy(crlEntry);
}

// Test crl entry getCertIssuer false
HWTEST_F(CryptoX509CrlTest, X509CrlTest123, TestSize.Level0)
{
    HcfX509CrlEntry *crlEntry = nullptr;
    HcfResult ret = g_x509Crl->getRevokedCert(g_x509Crl, 1000, &crlEntry);
    EXPECT_EQ(ret, HCF_SUCCESS);
    EXPECT_NE(crlEntry, nullptr);

    ret = crlEntry->getCertIssuer(crlEntry, nullptr);
    EXPECT_NE(ret, HCF_SUCCESS);
    HcfObjDestroy(crlEntry);
}

// Test crl entry getRevocationDate true
HWTEST_F(CryptoX509CrlTest, X509CrlTest131, TestSize.Level0)
{
    HcfX509CrlEntry *crlEntry = nullptr;
    HcfResult ret = g_x509Crl->getRevokedCert(g_x509Crl, 1000, &crlEntry);
    EXPECT_EQ(ret, HCF_SUCCESS);
    EXPECT_NE(crlEntry, nullptr);

    HcfBlob out = { 0 };
    ret = crlEntry->getRevocationDate(crlEntry, &out);
    EXPECT_EQ(ret, HCF_SUCCESS);
    EXPECT_NE(out.data, nullptr);
    EXPECT_STREQ((char *)g_rvTime->data, (char *)out.data);
    HcfObjDestroy(crlEntry);
    HcfFree(out.data);
}

// Test crl entry getRevocationDate false
HWTEST_F(CryptoX509CrlTest, X509CrlTest132, TestSize.Level0)
{
    HcfX509CrlEntry *crlEntry = nullptr;
    HcfResult ret = g_x509Crl->getRevokedCert(g_x509Crl, 1000, &crlEntry);
    EXPECT_EQ(ret, HCF_SUCCESS);
    EXPECT_NE(crlEntry, nullptr);

    HcfBlob out = { 0 };
    ret = crlEntry->getRevocationDate(nullptr, &out);
    EXPECT_NE(ret, HCF_SUCCESS);
    EXPECT_EQ(out.data, nullptr);
    HcfObjDestroy(crlEntry);
}

// Test crl entry getRevocationDate false
HWTEST_F(CryptoX509CrlTest, X509CrlTest133, TestSize.Level0)
{
    HcfX509CrlEntry *crlEntry = nullptr;
    HcfResult ret = g_x509Crl->getRevokedCert(g_x509Crl, 1000, &crlEntry);
    EXPECT_EQ(ret, HCF_SUCCESS);
    EXPECT_NE(crlEntry, nullptr);

    ret = crlEntry->getRevocationDate(crlEntry, nullptr);
    EXPECT_NE(ret, HCF_SUCCESS);
    HcfObjDestroy(crlEntry);
}

// Test crl getRevokedCertWithCert true
HWTEST_F(CryptoX509CrlTest, X509CrlTest141, TestSize.Level0)
{
    // Get cert
    HcfX509Certificate *x509Cert = nullptr;
    HcfEncodingBlob inStreamCert = { 0 };
    inStreamCert.data = (uint8_t *)g_testCert;
    inStreamCert.encodingFormat = HCF_FORMAT_PEM;
    inStreamCert.len = strlen(g_testCert) + 1;
    HcfResult ret = HcfX509CertificateCreate(&inStreamCert, &x509Cert);
    EXPECT_EQ(ret, HCF_SUCCESS);
    EXPECT_NE(x509Cert, nullptr);

    // Get crl
    HcfX509Crl *x509Crl = nullptr;
    HcfEncodingBlob inStreamCrl = { 0 };
    inStreamCrl.data = (uint8_t *)g_testCrl;
    inStreamCrl.encodingFormat = HCF_FORMAT_PEM;
    inStreamCrl.len = strlen(g_testCrl) + 1;
    ret = HcfX509CrlCreate(&inStreamCrl, &x509Crl);
    EXPECT_EQ(ret, HCF_SUCCESS);
    EXPECT_NE(x509Crl, nullptr);

    HcfX509CrlEntry *crlEntry = nullptr;
    ret = x509Crl->getRevokedCertWithCert(x509Crl, x509Cert, &crlEntry);
    EXPECT_EQ(ret, HCF_SUCCESS);
    EXPECT_NE(crlEntry, nullptr);

    HcfBlob out = { 0 };
    ret = crlEntry->getRevocationDate(crlEntry, &out);
    EXPECT_EQ(ret, HCF_SUCCESS);
    EXPECT_NE(out.data, nullptr);
    EXPECT_STREQ("220829065953Z", (char *)out.data);

    HcfObjDestroy(x509Cert);
    HcfObjDestroy(x509Crl);
    HcfObjDestroy(crlEntry);
    HcfFree(out.data);
}

// Test crl getRevokedCertWithCert true
HWTEST_F(CryptoX509CrlTest, X509CrlTest142, TestSize.Level0)
{
    // Get cert
    HcfX509Certificate *x509Cert = nullptr;
    HcfEncodingBlob inStreamCert = { 0 };
    inStreamCert.data = (uint8_t *)g_testCert;
    inStreamCert.encodingFormat = HCF_FORMAT_PEM;
    inStreamCert.len = strlen(g_testCert) + 1;
    HcfResult ret = HcfX509CertificateCreate(&inStreamCert, &x509Cert);
    EXPECT_EQ(ret, HCF_SUCCESS);
    EXPECT_NE(x509Cert, nullptr);

    // Get crl
    HcfX509Crl *x509Crl = nullptr;
    HcfEncodingBlob inStreamCrl = { 0 };
    inStreamCrl.data = (uint8_t *)g_testCrl;
    inStreamCrl.encodingFormat = HCF_FORMAT_PEM;
    inStreamCrl.len = strlen(g_testCrl) + 1;
    ret = HcfX509CrlCreate(&inStreamCrl, &x509Crl);
    EXPECT_EQ(ret, HCF_SUCCESS);
    EXPECT_NE(x509Crl, nullptr);

    HcfX509CrlEntry *crlEntry = nullptr;
    ret = x509Crl->getRevokedCertWithCert(x509Crl, x509Cert, &crlEntry);
    EXPECT_EQ(ret, HCF_SUCCESS);
    EXPECT_NE(crlEntry, nullptr);

    HcfBlob out = { 0 };
    ret = crlEntry->getCertIssuer(crlEntry, &out);
    EXPECT_EQ(ret, HCF_SUCCESS);
    EXPECT_NE(out.data, nullptr);
    EXPECT_STREQ("/C=CN/ST=shanghai/L=huawei/O=www.test.com/OU=test/CN=www.test.com/emailAddress=test@test.com",
        (char *)out.data);

    HcfObjDestroy(x509Cert);
    HcfObjDestroy(x509Crl);
    HcfObjDestroy(crlEntry);
    HcfFree(out.data);
}

// Test crl getRevokedCertWithCert true
HWTEST_F(CryptoX509CrlTest, X509CrlTest143, TestSize.Level0)
{
    // Get cert
    HcfX509Certificate *x509Cert = nullptr;
    HcfEncodingBlob inStreamCert = { 0 };
    inStreamCert.data = (uint8_t *)g_testCert;
    inStreamCert.encodingFormat = HCF_FORMAT_PEM;
    inStreamCert.len = strlen(g_testCert) + 1;
    HcfResult ret = HcfX509CertificateCreate(&inStreamCert, &x509Cert);
    EXPECT_EQ(ret, HCF_SUCCESS);
    EXPECT_NE(x509Cert, nullptr);

    // Get crl
    HcfX509Crl *x509Crl = nullptr;
    HcfEncodingBlob inStreamCrl = { 0 };
    inStreamCrl.data = (uint8_t *)g_testCrl;
    inStreamCrl.encodingFormat = HCF_FORMAT_PEM;
    inStreamCrl.len = strlen(g_testCrl) + 1;
    ret = HcfX509CrlCreate(&inStreamCrl, &x509Crl);
    EXPECT_EQ(ret, HCF_SUCCESS);
    EXPECT_NE(x509Crl, nullptr);

    HcfX509CrlEntry *crlEntry = nullptr;
    ret = x509Crl->getRevokedCertWithCert(x509Crl, x509Cert, &crlEntry);
    EXPECT_EQ(ret, HCF_SUCCESS);
    EXPECT_NE(crlEntry, nullptr);

    HcfEncodingBlob encodingBlob = { 0 };
    ret = crlEntry->getEncoded(crlEntry, &encodingBlob);
    EXPECT_EQ(ret, HCF_SUCCESS);
    EXPECT_NE(encodingBlob.data, nullptr);

    HcfFree(encodingBlob.data);
    HcfObjDestroy(x509Cert);
    HcfObjDestroy(x509Crl);
    HcfObjDestroy(crlEntry);
}

// Test crl getRevokedCertWithCert false
HWTEST_F(CryptoX509CrlTest, X509CrlTest144, TestSize.Level0)
{
    // Get crl
    HcfX509Crl *x509Crl = nullptr;
    HcfEncodingBlob inStreamCrl = { 0 };
    inStreamCrl.data = (uint8_t *)g_testCrl;
    inStreamCrl.encodingFormat = HCF_FORMAT_PEM;
    inStreamCrl.len = strlen(g_testCrl) + 1;
    HcfResult ret = HcfX509CrlCreate(&inStreamCrl, &x509Crl);
    EXPECT_EQ(ret, HCF_SUCCESS);
    EXPECT_NE(x509Crl, nullptr);

    HcfX509CrlEntry *crlEntry = nullptr;
    ret = x509Crl->getRevokedCertWithCert(x509Crl, nullptr, &crlEntry);
    EXPECT_NE(ret, HCF_SUCCESS);
    EXPECT_EQ(crlEntry, nullptr);

    HcfObjDestroy(x509Crl);
}

// Test crl getRevokedCertWithCert false
HWTEST_F(CryptoX509CrlTest, X509CrlTest145, TestSize.Level0)
{
    // Get cert
    HcfX509Certificate *x509Cert = nullptr;
    HcfEncodingBlob inStreamCert = { 0 };
    inStreamCert.data = (uint8_t *)g_testCert;
    inStreamCert.encodingFormat = HCF_FORMAT_PEM;
    inStreamCert.len = strlen(g_testCert) + 1;
    HcfResult ret = HcfX509CertificateCreate(&inStreamCert, &x509Cert);
    EXPECT_EQ(ret, HCF_SUCCESS);
    EXPECT_NE(x509Cert, nullptr);

    // Get crl
    HcfX509Crl *x509Crl = nullptr;
    HcfEncodingBlob inStreamCrl = { 0 };
    inStreamCrl.data = (uint8_t *)g_testCrl;
    inStreamCrl.encodingFormat = HCF_FORMAT_PEM;
    inStreamCrl.len = strlen(g_testCrl) + 1;
    ret = HcfX509CrlCreate(&inStreamCrl, &x509Crl);
    EXPECT_EQ(ret, HCF_SUCCESS);
    EXPECT_NE(x509Crl, nullptr);

    HcfX509CrlEntry *crlEntry = nullptr;
    ret = x509Crl->getRevokedCertWithCert(nullptr, x509Cert, &crlEntry);
    EXPECT_NE(ret, HCF_SUCCESS);
    EXPECT_EQ(crlEntry, nullptr);

    HcfObjDestroy(x509Cert);
    HcfObjDestroy(x509Crl);
}

// Test crl getRevokedCertWithCert false
HWTEST_F(CryptoX509CrlTest, X509CrlTest146, TestSize.Level0)
{
    // Get cert
    HcfX509Certificate *x509Cert = nullptr;
    HcfEncodingBlob inStreamCert = { 0 };
    inStreamCert.data = (uint8_t *)g_testCert;
    inStreamCert.encodingFormat = HCF_FORMAT_PEM;
    inStreamCert.len = strlen(g_testCert) + 1;
    HcfResult ret = HcfX509CertificateCreate(&inStreamCert, &x509Cert);
    EXPECT_EQ(ret, HCF_SUCCESS);
    EXPECT_NE(x509Cert, nullptr);

    // Get crl
    HcfX509Crl *x509Crl = nullptr;
    HcfEncodingBlob inStreamCrl = { 0 };
    inStreamCrl.data = (uint8_t *)g_testCrl;
    inStreamCrl.encodingFormat = HCF_FORMAT_PEM;
    inStreamCrl.len = strlen(g_testCrl) + 1;
    ret = HcfX509CrlCreate(&inStreamCrl, &x509Crl);
    EXPECT_EQ(ret, HCF_SUCCESS);
    EXPECT_NE(x509Crl, nullptr);

    ret = x509Crl->getRevokedCertWithCert(x509Crl, x509Cert, nullptr);
    EXPECT_NE(ret, HCF_SUCCESS);

    HcfObjDestroy(x509Cert);
    HcfObjDestroy(x509Crl);
}

// Test crl getRevokedCertWithCert false
HWTEST_F(CryptoX509CrlTest, X509CrlTest147, TestSize.Level0)
{
    // Get cert
    HcfX509Certificate *x509Cert = nullptr;
    HcfEncodingBlob inStreamCert = { 0 };
    inStreamCert.data = (uint8_t *)g_testCert;
    inStreamCert.encodingFormat = HCF_FORMAT_PEM;
    inStreamCert.len = strlen(g_testCert) + 1;
    HcfResult ret = HcfX509CertificateCreate(&inStreamCert, &x509Cert);
    EXPECT_EQ(ret, HCF_SUCCESS);
    EXPECT_NE(x509Cert, nullptr);

    // Get crl
    HcfX509Crl *x509Crl = nullptr;
    HcfEncodingBlob inStreamCrl = { 0 };
    inStreamCrl.data = (uint8_t *)g_testCrl;
    inStreamCrl.encodingFormat = HCF_FORMAT_PEM;
    inStreamCrl.len = strlen(g_testCrl) + 1;
    ret = HcfX509CrlCreate(&inStreamCrl, &x509Crl);
    EXPECT_EQ(ret, HCF_SUCCESS);
    EXPECT_NE(x509Crl, nullptr);

    HcfX509CrlEntry *crlEntry = nullptr;
    ret = x509Crl->getRevokedCertWithCert(x509Crl, x509Cert, &crlEntry);
    EXPECT_EQ(ret, HCF_SUCCESS);
    EXPECT_NE(crlEntry, nullptr);

    HcfBlob out = { 0 };
    ret = crlEntry->getRevocationDate(nullptr, &out);
    EXPECT_NE(ret, HCF_SUCCESS);
    EXPECT_EQ(out.data, nullptr);

    HcfObjDestroy(x509Cert);
    HcfObjDestroy(x509Crl);
    HcfObjDestroy(crlEntry);
}

// Test crl entry getRevokedCerts true
HWTEST_F(CryptoX509CrlTest, X509CrlTest151, TestSize.Level0)
{
    HcfArray entrysOut = { 0 };
    HcfResult ret = g_x509Crl->getRevokedCerts(g_x509Crl, &entrysOut);
    EXPECT_EQ(ret, HCF_SUCCESS);
    EXPECT_NE(entrysOut.data, nullptr);

    HcfX509CrlEntry *crlEntry = reinterpret_cast<HcfX509CrlEntry *>(entrysOut.data[0].data);
    HcfBlob out = { 0 };
    ret = crlEntry->getRevocationDate(crlEntry, &out);
    EXPECT_EQ(ret, HCF_SUCCESS);
    EXPECT_NE(out.data, nullptr);
    EXPECT_STREQ((char *)g_rvTime->data, (char *)out.data);

    HcfFree(out.data);
    HcfObjDestroy(crlEntry);
}

// Test crl entry getRevokedCerts false
HWTEST_F(CryptoX509CrlTest, X509CrlTest152, TestSize.Level0)
{
    HcfResult ret = g_x509Crl->getRevokedCerts(g_x509Crl, nullptr);
    EXPECT_NE(ret, HCF_SUCCESS);
}

// Test crl entry getRevokedCerts false
HWTEST_F(CryptoX509CrlTest, X509CrlTest153, TestSize.Level0)
{
    HcfArray entrysOut = { 0 };
    HcfResult ret = g_x509Crl->getRevokedCerts(nullptr, &entrysOut);
    EXPECT_NE(ret, HCF_SUCCESS);
    EXPECT_EQ(entrysOut.data, nullptr);
}

// Test crl entry getRevokedCerts false
HWTEST_F(CryptoX509CrlTest, X509CrlTest154, TestSize.Level0)
{
    HcfResult ret = g_x509Crl->getRevokedCerts(nullptr, nullptr);
    EXPECT_NE(ret, HCF_SUCCESS);
}

// Test crl getTbsInfo true
HWTEST_F(CryptoX509CrlTest, X509CrlTest161, TestSize.Level0)
{
    HcfBlob tbsCertListOut = { 0 };
    HcfResult ret = g_x509Crl->getTbsInfo(g_x509Crl, &tbsCertListOut);
    EXPECT_EQ(ret, HCF_SUCCESS);
    EXPECT_NE(tbsCertListOut.data, nullptr);

    EXPECT_STREQ((char *)g_tbs, (char *)tbsCertListOut.data);
    HcfFree(tbsCertListOut.data);
}

// Test crl getTbsInfo false
HWTEST_F(CryptoX509CrlTest, X509CrlTest162, TestSize.Level0)
{
    HcfBlob tbsCertListOut = { 0 };
    HcfResult ret = g_x509Crl->getTbsInfo(nullptr, &tbsCertListOut);
    EXPECT_NE(ret, HCF_SUCCESS);
    EXPECT_EQ(tbsCertListOut.data, nullptr);
}

// Test crl  getTbsInfo false
HWTEST_F(CryptoX509CrlTest, X509CrlTest163, TestSize.Level0)
{
    HcfResult ret = g_x509Crl->getTbsInfo(g_x509Crl, nullptr);
    EXPECT_NE(ret, HCF_SUCCESS);
}

// Test crl getTbsInfo false
HWTEST_F(CryptoX509CrlTest, X509CrlTest164, TestSize.Level0)
{
    HcfResult ret = g_x509Crl->getTbsInfo(nullptr, nullptr);
    EXPECT_NE(ret, HCF_SUCCESS);
}

// Test crl getSignature true
HWTEST_F(CryptoX509CrlTest, X509CrlTest171, TestSize.Level0)
{
    HcfBlob signature = { 0 };
    HcfResult ret = g_x509Crl->getSignature(g_x509Crl, &signature);
    EXPECT_EQ(ret, HCF_SUCCESS);
    EXPECT_NE(signature.data, nullptr);
    bool isEqual = (memcmp(g_signatureStr, signature.data, g_signatureLen) == 0);
    EXPECT_EQ(isEqual, true);
    HcfFree(signature.data);
}

// Test crl getSignature false
HWTEST_F(CryptoX509CrlTest, X509CrlTest172, TestSize.Level0)
{
    HcfBlob signature = { 0 };
    HcfResult ret = g_x509Crl->getSignature(nullptr, &signature);
    EXPECT_NE(ret, HCF_SUCCESS);
    EXPECT_EQ(signature.data, nullptr);
}

// Test crl getSignature false
HWTEST_F(CryptoX509CrlTest, X509CrlTest173, TestSize.Level0)
{
    HcfResult ret = g_x509Crl->getSignature(g_x509Crl, nullptr);
    EXPECT_NE(ret, HCF_SUCCESS);
}

// Test crl getSignature false
HWTEST_F(CryptoX509CrlTest, X509CrlTest174, TestSize.Level0)
{
    HcfResult ret = g_x509Crl->getSignature(nullptr, nullptr);
    EXPECT_NE(ret, HCF_SUCCESS);
}

// Test crl getSignatureAlgName true
HWTEST_F(CryptoX509CrlTest, X509CrlTest181, TestSize.Level0)
{
    HcfBlob out = { 0 };
    HcfResult ret = g_x509Crl->getSignatureAlgName(g_x509Crl, &out);
    EXPECT_EQ(ret, HCF_SUCCESS);
    EXPECT_NE(out.data, nullptr);
    EXPECT_STREQ("MD5withRSA", (char *)out.data);
    HcfFree(out.data);
}

// Test crl getSignatureAlgName false
HWTEST_F(CryptoX509CrlTest, X509CrlTest182, TestSize.Level0)
{
    HcfBlob out = { 0 };
    HcfResult ret = g_x509Crl->getSignatureAlgName(nullptr, &out);
    EXPECT_NE(ret, HCF_SUCCESS);
    EXPECT_EQ(out.data, nullptr);
}

// Test crl getSignatureAlgName false
HWTEST_F(CryptoX509CrlTest, X509CrlTest183, TestSize.Level0)
{
    HcfResult ret = g_x509Crl->getSignatureAlgName(g_x509Crl, nullptr);
    EXPECT_NE(ret, HCF_SUCCESS);
}

// Test crl getSignatureAlgName false
HWTEST_F(CryptoX509CrlTest, X509CrlTest184, TestSize.Level0)
{
    HcfResult ret = g_x509Crl->getSignatureAlgName(nullptr, nullptr);
    EXPECT_NE(ret, HCF_SUCCESS);
}

// Test crl getSignatureAlgOid true
HWTEST_F(CryptoX509CrlTest, X509CrlTest191, TestSize.Level0)
{
    HcfBlob out = { 0 };
    HcfResult ret = g_x509Crl->getSignatureAlgOid(g_x509Crl, &out);
    EXPECT_EQ(ret, HCF_SUCCESS);
    EXPECT_NE(out.data, nullptr);
    EXPECT_STREQ("1.2.840.113549.1.1.4", (char *)out.data);
    HcfFree(out.data);
}

// Test crl getSignatureAlgOid false
HWTEST_F(CryptoX509CrlTest, X509CrlTest192, TestSize.Level0)
{
    HcfBlob out = { 0 };
    HcfResult ret = g_x509Crl->getSignatureAlgOid(nullptr, &out);
    EXPECT_NE(ret, HCF_SUCCESS);
    EXPECT_EQ(out.data, nullptr);
}

// Test crl getSignatureAlgOid false
HWTEST_F(CryptoX509CrlTest, X509CrlTest193, TestSize.Level0)
{
    HcfResult ret = g_x509Crl->getSignatureAlgOid(g_x509Crl, nullptr);
    EXPECT_NE(ret, HCF_SUCCESS);
}

// Test crl getSignatureAlgOid false
HWTEST_F(CryptoX509CrlTest, X509CrlTest194, TestSize.Level0)
{
    HcfResult ret = g_x509Crl->getSignatureAlgOid(nullptr, nullptr);
    EXPECT_NE(ret, HCF_SUCCESS);
}

// Test crl getSignatureAlgParams true
HWTEST_F(CryptoX509CrlTest, X509CrlTest201, TestSize.Level0)
{
    HcfBlob sigAlgParamOut = { 0 };
    HcfResult ret = g_x509Crl->getSignatureAlgParams(g_x509Crl, &sigAlgParamOut);
    EXPECT_EQ(ret, HCF_SUCCESS);
    EXPECT_NE(sigAlgParamOut.data, nullptr);
    HcfFree(sigAlgParamOut.data);
}

// Test crl getSignatureAlgParams false
HWTEST_F(CryptoX509CrlTest, X509CrlTest202, TestSize.Level0)
{
    HcfBlob sigAlgParamOut = { 0 };
    HcfResult ret = g_x509Crl->getSignatureAlgParams(nullptr, &sigAlgParamOut);
    EXPECT_NE(ret, HCF_SUCCESS);
    EXPECT_EQ(sigAlgParamOut.data, nullptr);
}

// Test crl getSignatureAlgParams false
HWTEST_F(CryptoX509CrlTest, X509CrlTest203, TestSize.Level0)
{
    HcfResult ret = g_x509Crl->getSignatureAlgParams(g_x509Crl, nullptr);
    EXPECT_NE(ret, HCF_SUCCESS);
}

// Test crl getSignatureAlgParams false
HWTEST_F(CryptoX509CrlTest, X509CrlTest204, TestSize.Level0)
{
    HcfResult ret = g_x509Crl->getSignatureAlgParams(nullptr, nullptr);
    EXPECT_NE(ret, HCF_SUCCESS);
}

HWTEST_F(CryptoX509CrlTest, NullSpi, TestSize.Level0)
{
    (void)HcfCX509CrlSpiCreate(nullptr, nullptr);
    (void)HcfCX509CRLEntryCreate(nullptr, nullptr, nullptr);
    HcfX509CrlSpi *spiObj = nullptr;
    HcfResult ret = HcfCX509CrlSpiCreate(g_crlDerInStream, &spiObj);
    EXPECT_EQ(ret, HCF_SUCCESS);
    EXPECT_NE(spiObj, nullptr);

    (void)spiObj->base.destroy(nullptr);
    const char *tmp = spiObj->engineGetType(nullptr);
    EXPECT_EQ(tmp, nullptr);
    bool flag = spiObj->engineIsRevoked(nullptr, nullptr);
    EXPECT_EQ(flag, false);
    ret = spiObj->engineGetEncoded(nullptr, nullptr);
    EXPECT_NE(ret, HCF_SUCCESS);
    ret = spiObj->engineVerify(nullptr, nullptr);
    EXPECT_NE(ret, HCF_SUCCESS);
    long ver = spiObj->engineGetVersion(nullptr);
    EXPECT_EQ(ver, -1);
    ret = spiObj->engineGetIssuerName(nullptr, nullptr);
    EXPECT_NE(ret, HCF_SUCCESS);
    ret = spiObj->engineGetLastUpdate(nullptr, nullptr);
    EXPECT_NE(ret, HCF_SUCCESS);
    ret = spiObj->engineGetNextUpdate(nullptr, nullptr);
    EXPECT_NE(ret, HCF_SUCCESS);
    ret = spiObj->engineGetRevokedCert(nullptr, 0, nullptr);
    EXPECT_NE(ret, HCF_SUCCESS);
    ret = spiObj->engineGetRevokedCertWithCert(nullptr, nullptr, nullptr);
    EXPECT_NE(ret, HCF_SUCCESS);
    ret = spiObj->engineGetRevokedCerts(nullptr, nullptr);
    EXPECT_NE(ret, HCF_SUCCESS);
    ret = spiObj->engineGetTbsInfo(nullptr, nullptr);
    EXPECT_NE(ret, HCF_SUCCESS);
    ret = spiObj->engineGetSignature(nullptr, nullptr);
    EXPECT_NE(ret, HCF_SUCCESS);
    ret = spiObj->engineGetSignatureAlgName(nullptr, nullptr);
    EXPECT_NE(ret, HCF_SUCCESS);
    ret = spiObj->engineGetSignatureAlgOid(nullptr, nullptr);
    EXPECT_NE(ret, HCF_SUCCESS);
    ret = spiObj->engineGetSignatureAlgParams(nullptr, nullptr);
    EXPECT_NE(ret, HCF_SUCCESS);

    HcfObjDestroy(spiObj);
}

static const char *GetInvalidCrlClass(void)
{
    return "INVALID_CRL_CLASS";
}

HWTEST_F(CryptoX509CrlTest, InvalidCrlSpiClass, TestSize.Level0)
{
    HcfX509CrlSpi invalidSpi = { {0} };
    invalidSpi.base.getClass = GetInvalidCrlClass;
    HcfBlob invalidOut = { 0 };
    HcfEncodingBlob encoding = { 0 };
    HcfX509CrlEntry *entry = nullptr;
    HcfX509CrlSpi *spiObj = nullptr;
    HcfResult ret = HcfCX509CrlSpiCreate(g_crlDerInStream, &spiObj);
    (void)spiObj->base.destroy(&(invalidSpi.base));
    const char *tmp = spiObj->engineGetType(&invalidSpi);
    EXPECT_EQ(tmp, nullptr);
    HcfCertificate cert;
    bool flag = spiObj->engineIsRevoked(&invalidSpi, &cert);
    EXPECT_EQ(flag, false);
    ret = spiObj->engineGetEncoded(&invalidSpi, &encoding);
    EXPECT_NE(ret, HCF_SUCCESS);
    HcfPubKey pubKey;
    ret = spiObj->engineVerify(&invalidSpi, &pubKey);
    EXPECT_NE(ret, HCF_SUCCESS);
    long ver = spiObj->engineGetVersion(&invalidSpi);
    EXPECT_EQ(ver, -1);
    ret = spiObj->engineGetIssuerName(&invalidSpi, &invalidOut);
    EXPECT_NE(ret, HCF_SUCCESS);
    ret = spiObj->engineGetLastUpdate(&invalidSpi, &invalidOut);
    EXPECT_NE(ret, HCF_SUCCESS);
    ret = spiObj->engineGetNextUpdate(&invalidSpi, &invalidOut);
    EXPECT_NE(ret, HCF_SUCCESS);
    ret = spiObj->engineGetRevokedCert(&invalidSpi, 0, &entry);
    EXPECT_NE(ret, HCF_SUCCESS);
    HcfX509Certificate x509Cert;
    ret = spiObj->engineGetRevokedCertWithCert(&invalidSpi, &x509Cert, &entry);
    EXPECT_NE(ret, HCF_SUCCESS);
    HcfArray invalidArr = { 0 };
    ret = spiObj->engineGetRevokedCerts(&invalidSpi, &invalidArr);
    EXPECT_NE(ret, HCF_SUCCESS);
    ret = spiObj->engineGetTbsInfo(&invalidSpi, &invalidOut);
    EXPECT_NE(ret, HCF_SUCCESS);
    ret = spiObj->engineGetSignature(&invalidSpi, &invalidOut);
    EXPECT_NE(ret, HCF_SUCCESS);
    ret = spiObj->engineGetSignatureAlgName(&invalidSpi, &invalidOut);
    EXPECT_NE(ret, HCF_SUCCESS);
    ret = spiObj->engineGetSignatureAlgOid(&invalidSpi, &invalidOut);
    EXPECT_NE(ret, HCF_SUCCESS);
    ret = spiObj->engineGetSignatureAlgParams(&invalidSpi, &invalidOut);
    EXPECT_NE(ret, HCF_SUCCESS);
    HcfObjDestroy(spiObj);
}

HWTEST_F(CryptoX509CrlTest, InvalidCrlClass, TestSize.Level0)
{
    HcfX509Crl invalidCrl;
    invalidCrl.base.base.getClass = GetInvalidCrlClass;
    HcfBlob invalidOut = { 0 };
    HcfEncodingBlob encoding = { 0 };
    HcfX509CrlEntry *entry = nullptr;

    g_x509Crl->base.base.destroy(nullptr);
    g_x509Crl->base.base.destroy(&(invalidCrl.base.base));
    const char *tmp = g_x509Crl->base.getType(&(invalidCrl.base));
    EXPECT_EQ(tmp, nullptr);
    HcfCertificate cert;
    bool flag = g_x509Crl->base.isRevoked(&(invalidCrl.base), &cert);
    EXPECT_EQ(flag, false);
    HcfResult ret = g_x509Crl->getEncoded(&invalidCrl, &encoding);
    EXPECT_NE(ret, HCF_SUCCESS);
    HcfPubKey pubKey;
    ret = g_x509Crl->verify(&invalidCrl, &pubKey);
    EXPECT_NE(ret, HCF_SUCCESS);
    long ver = g_x509Crl->getVersion(&invalidCrl);
    EXPECT_EQ(ver, -1);
    ret = g_x509Crl->getIssuerName(&invalidCrl, &invalidOut);
    EXPECT_NE(ret, HCF_SUCCESS);
    ret = g_x509Crl->getLastUpdate(&invalidCrl, &invalidOut);
    EXPECT_NE(ret, HCF_SUCCESS);
    ret = g_x509Crl->getNextUpdate(&invalidCrl, &invalidOut);
    EXPECT_NE(ret, HCF_SUCCESS);
    ret = g_x509Crl->getRevokedCert(&invalidCrl, 0, &entry);
    EXPECT_NE(ret, HCF_SUCCESS);
    HcfX509Certificate x509Cert;
    ret = g_x509Crl->getRevokedCertWithCert(&invalidCrl, &x509Cert, &entry);
    EXPECT_NE(ret, HCF_SUCCESS);
    HcfArray invalidArr = { 0 };
    ret = g_x509Crl->getRevokedCerts(&invalidCrl, &invalidArr);
    EXPECT_NE(ret, HCF_SUCCESS);
    ret = g_x509Crl->getTbsInfo(&invalidCrl, &invalidOut);
    EXPECT_NE(ret, HCF_SUCCESS);
    ret = g_x509Crl->getSignature(&invalidCrl, &invalidOut);
    EXPECT_NE(ret, HCF_SUCCESS);
    ret = g_x509Crl->getSignatureAlgName(&invalidCrl, &invalidOut);
    EXPECT_NE(ret, HCF_SUCCESS);
    ret = g_x509Crl->getSignatureAlgOid(&invalidCrl, &invalidOut);
    EXPECT_NE(ret, HCF_SUCCESS);
    ret = g_x509Crl->getSignatureAlgParams(&invalidCrl, &invalidOut);
    EXPECT_NE(ret, HCF_SUCCESS);
}

HWTEST_F(CryptoX509CrlTest, InvalidMalloc, TestSize.Level0)
{
    SetMockFlag(true);
    HcfBlob out = { 0 };
    HcfEncodingBlob encoding = { 0 };
    HcfX509CrlEntry *entry = nullptr;
    HcfResult ret = g_x509Crl->getEncoded(g_x509Crl, &encoding);
    EXPECT_NE(ret, HCF_SUCCESS);
    ret = g_x509Crl->getIssuerName(g_x509Crl, &out);
    EXPECT_NE(ret, HCF_SUCCESS);
    ret = g_x509Crl->getLastUpdate(g_x509Crl, &out);
    EXPECT_NE(ret, HCF_SUCCESS);
    ret = g_x509Crl->getNextUpdate(g_x509Crl, &out);
    EXPECT_NE(ret, HCF_SUCCESS);
    ret = g_x509Crl->getRevokedCert(g_x509Crl, 0, &entry);
    EXPECT_NE(ret, HCF_SUCCESS);
    HcfArray arr = { 0 };
    ret = g_x509Crl->getRevokedCerts(g_x509Crl, &arr);
    EXPECT_NE(ret, HCF_SUCCESS);
    ret = g_x509Crl->getTbsInfo(g_x509Crl, &out);
    EXPECT_NE(ret, HCF_SUCCESS);
    ret = g_x509Crl->getSignature(g_x509Crl, &out);
    EXPECT_NE(ret, HCF_SUCCESS);
    ret = g_x509Crl->getSignatureAlgName(g_x509Crl, &out);
    EXPECT_NE(ret, HCF_SUCCESS);
    ret = g_x509Crl->getSignatureAlgOid(g_x509Crl, &out);
    EXPECT_NE(ret, HCF_SUCCESS);
    ret = g_x509Crl->getSignatureAlgParams(g_x509Crl, &out);
    EXPECT_NE(ret, HCF_SUCCESS);
    SetMockFlag(false);
}
}