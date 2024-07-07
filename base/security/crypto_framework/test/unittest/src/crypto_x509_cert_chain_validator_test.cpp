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

#include <gtest/gtest.h>
#include "securec.h"
#include "string"

#include "cert_chain_validator.h"
#include "blob.h"
#include "memory_mock.h"
#include "object_base.h"
#include "result.h"
#include "x509_cert_chain_validator_openssl.h"

using namespace std;
using namespace testing::ext;

namespace {
class CryptoX509CertChainValidatorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

constexpr int32_t CERT_HEADER_LEN = 2;
constexpr int32_t INVALID_MAX_CERT_LEN = 8194;

static char g_caCert[] =
"-----BEGIN CERTIFICATE-----\r\n"
"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\r\n"
"-----END CERTIFICATE-----\r\n";

static char g_secondCaCert[] =
"-----BEGIN CERTIFICATE-----\r\n"
"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\r\n"
"-----END CERTIFICATE-----\r\n";

static char g_invalidCaCert[] =
"-----BEGIN CERTIFICATE-----\r\n"
"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\r\n"
"-----END CERTIFICATE-----\r\n";

static HcfCertChainValidator *g_validator = nullptr;

void CryptoX509CertChainValidatorTest::SetUpTestCase()
{
    (void)HcfCertChainValidatorCreate("PKIX", &g_validator);
}
void CryptoX509CertChainValidatorTest::TearDownTestCase()
{
    HcfObjDestroy(g_validator);
}

void CryptoX509CertChainValidatorTest::SetUp()
{
}

void CryptoX509CertChainValidatorTest::TearDown()
{
}

HWTEST_F(CryptoX509CertChainValidatorTest, GetAlgorithm001, TestSize.Level0)
{
    const char *algo = g_validator->getAlgorithm(g_validator);
    EXPECT_NE(algo, nullptr);
    if (algo == nullptr) {
        HcfObjDestroy(g_validator);
        return;
    }
    string st("PKIX");
    ASSERT_STREQ(algo, st.c_str());
}

HWTEST_F(CryptoX509CertChainValidatorTest, GetAlgorithm002, TestSize.Level0)
{
    HcfCertChainValidator *pathValidator = nullptr;
    HcfResult res = HcfCertChainValidatorCreate("invalidPKIX", &pathValidator);
    EXPECT_EQ(res, HCF_NOT_SUPPORT);
    EXPECT_EQ(pathValidator, nullptr);
}

/* valid cert chain. */
HWTEST_F(CryptoX509CertChainValidatorTest, VerifyTest001, TestSize.Level0)
{
    HcfResult res = HCF_SUCCESS;
    HcfCertChainData certsData = { 0 };
    certsData.format = HCF_FORMAT_PEM;
    certsData.count = 2; /* level-2 cert chain. */
    uint32_t caCertLen = strlen(g_caCert) + 1;
    uint32_t secondCaCertLen = strlen(g_secondCaCert) + 1;
    certsData.dataLen = CERT_HEADER_LEN + secondCaCertLen + CERT_HEADER_LEN + caCertLen;
    certsData.data = (uint8_t *)malloc(certsData.dataLen);
    if (certsData.data == nullptr) {
        return;
    }
    if (memcpy_s(certsData.data, CERT_HEADER_LEN + secondCaCertLen + CERT_HEADER_LEN + caCertLen,
        &secondCaCertLen, CERT_HEADER_LEN) != EOK) {
        goto OUT;
    }
    if (memcpy_s(certsData.data + CERT_HEADER_LEN, secondCaCertLen + CERT_HEADER_LEN + caCertLen,
        g_secondCaCert, secondCaCertLen) != EOK) {
        goto OUT;
    }
    if (memcpy_s(certsData.data + CERT_HEADER_LEN + secondCaCertLen, CERT_HEADER_LEN + caCertLen,
        &caCertLen, CERT_HEADER_LEN) != EOK) {
        goto OUT;
    }
    if (memcpy_s(certsData.data + CERT_HEADER_LEN + secondCaCertLen + CERT_HEADER_LEN, caCertLen,
        g_caCert, caCertLen) != EOK) {
        goto OUT;
    }

    res = g_validator->validate(g_validator, &certsData);
    EXPECT_EQ(res, HCF_SUCCESS);
OUT:
    free(certsData.data);
}

/* invalid cert chain. */
HWTEST_F(CryptoX509CertChainValidatorTest, VerifyTest002, TestSize.Level0)
{
    HcfResult res = HCF_SUCCESS;
    HcfCertChainData certsData = { 0 };
    certsData.format = HCF_FORMAT_PEM;
    certsData.count = 3; /* level-3 cert chain. */
    uint32_t caCertLen = strlen(g_caCert) + 1;
    uint32_t secondCaCertLen = strlen(g_secondCaCert) + 1;
    uint32_t thirdCertLen = strlen(g_invalidCaCert) + 1;
    certsData.dataLen = CERT_HEADER_LEN + thirdCertLen + CERT_HEADER_LEN +
        secondCaCertLen + CERT_HEADER_LEN + caCertLen;
    certsData.data = (uint8_t *)malloc(certsData.dataLen);
    EXPECT_NE(certsData.data, nullptr);
    if (certsData.data == nullptr) {
        return;
    }
    if (memcpy_s(certsData.data,
        CERT_HEADER_LEN + thirdCertLen + CERT_HEADER_LEN + secondCaCertLen + CERT_HEADER_LEN + caCertLen,
        &thirdCertLen, CERT_HEADER_LEN) != EOK) {
        goto OUT;
    }
    if (memcpy_s(certsData.data + CERT_HEADER_LEN,
        thirdCertLen + CERT_HEADER_LEN + secondCaCertLen + CERT_HEADER_LEN + caCertLen,
        g_invalidCaCert, thirdCertLen) != EOK) {
        return;
    }
    if (memcpy_s(certsData.data + CERT_HEADER_LEN + thirdCertLen,
        CERT_HEADER_LEN + secondCaCertLen + CERT_HEADER_LEN + caCertLen, &secondCaCertLen, CERT_HEADER_LEN) != EOK) {
        goto OUT;
    }
    if (memcpy_s(certsData.data + CERT_HEADER_LEN + thirdCertLen + CERT_HEADER_LEN,
        secondCaCertLen + CERT_HEADER_LEN + caCertLen, g_secondCaCert, secondCaCertLen) != EOK) {
        goto OUT;
    }
    if (memcpy_s(certsData.data + CERT_HEADER_LEN + thirdCertLen + CERT_HEADER_LEN + secondCaCertLen,
        CERT_HEADER_LEN + caCertLen, &caCertLen, CERT_HEADER_LEN) != EOK) {
        goto OUT;
    }
    if (memcpy_s(certsData.data + CERT_HEADER_LEN + thirdCertLen + CERT_HEADER_LEN + secondCaCertLen + CERT_HEADER_LEN,
        caCertLen, g_caCert, caCertLen) != EOK) {
        goto OUT;
    }

    res = g_validator->validate(g_validator, &certsData);
    EXPECT_NE(res, HCF_SUCCESS);
OUT:
    free(certsData.data);
}

/* invalid cert chain data len. */
HWTEST_F(CryptoX509CertChainValidatorTest, VerifyTest003, TestSize.Level0)
{
    HcfCertChainData certsData = { 0 };
    certsData.format = HCF_FORMAT_PEM;
    certsData.count = 3; /* level-3 cert chain. */
    certsData.dataLen = INVALID_MAX_CERT_LEN;
    certsData.data = (uint8_t *)malloc(certsData.dataLen);
    EXPECT_NE(certsData.data, nullptr);
    if (certsData.data == nullptr) {
        return;
    }

    HcfResult res = g_validator->validate(g_validator, &certsData);
    EXPECT_NE(res, HCF_SUCCESS);
    free(certsData.data);
}

/* invalid cert number(1). */
HWTEST_F(CryptoX509CertChainValidatorTest, VerifyTest004, TestSize.Level0)
{
    HcfResult res = HCF_SUCCESS;
    HcfCertChainData certsData = { 0 };
    certsData.format = HCF_FORMAT_PEM;
    certsData.count = 1; /* level-3 cert chain. */
    uint32_t caCertLen = strlen(g_caCert) + 1;
    certsData.dataLen = CERT_HEADER_LEN + caCertLen;
    certsData.data = (uint8_t *)malloc(certsData.dataLen);
    EXPECT_NE(certsData.data, nullptr);
    if (certsData.data == nullptr) {
        return;
    }
    if (memcpy_s(certsData.data,
        CERT_HEADER_LEN + caCertLen, &caCertLen, CERT_HEADER_LEN) != EOK) {
        goto OUT;
    }
    if (memcpy_s(certsData.data + CERT_HEADER_LEN,
        caCertLen, g_caCert, caCertLen) != EOK) {
        goto OUT;
    }

    res = g_validator->validate(g_validator, &certsData);
    EXPECT_NE(res, HCF_SUCCESS);
OUT:
    free(certsData.data);
}

static const char *GetInvalidValidatorClass(void)
{
    return "INVALID_VALIDATOR_CLASS";
}


HWTEST_F(CryptoX509CertChainValidatorTest, NullInput, TestSize.Level0)
{
    HcfResult res = HcfCertChainValidatorCreate("PKIX", nullptr);
    EXPECT_NE(res, HCF_SUCCESS);
    res = g_validator->validate(g_validator, nullptr);
    EXPECT_NE(res, HCF_SUCCESS);
    const char *algo = g_validator->getAlgorithm(nullptr);
    EXPECT_EQ(algo, nullptr);
    (void)g_validator->base.destroy(nullptr);
}

HWTEST_F(CryptoX509CertChainValidatorTest, InvalidClass, TestSize.Level0)
{
    HcfCertChainValidator invalidValidator;
    invalidValidator.base.getClass = GetInvalidValidatorClass;
    HcfCertChainData certsData = { 0 };
    HcfResult res = g_validator->validate(&invalidValidator, &certsData);
    EXPECT_NE(res, HCF_SUCCESS);
    const char *algo = g_validator->getAlgorithm(&invalidValidator);
    EXPECT_EQ(algo, nullptr);
    (void)g_validator->base.destroy(&(invalidValidator.base));
}

HWTEST_F(CryptoX509CertChainValidatorTest, NullSpiInput, TestSize.Level0)
{
    HcfCertChainValidatorSpi *spiObj = nullptr;
    HcfResult res = HcfCertChainValidatorSpiCreate(nullptr);
    EXPECT_NE(res, HCF_SUCCESS);
    res = HcfCertChainValidatorSpiCreate(&spiObj);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = spiObj->engineValidate(spiObj, nullptr);
    EXPECT_NE(res, HCF_SUCCESS);
    (void)spiObj->base.destroy(nullptr);
}

HWTEST_F(CryptoX509CertChainValidatorTest, InvalidSpiClass, TestSize.Level0)
{
    HcfCertChainValidatorSpi *spiObj = nullptr;
    HcfResult res = HcfCertChainValidatorSpiCreate(&spiObj);
    HcfCertChainValidatorSpi invalidSpi;
    invalidSpi.base.getClass = GetInvalidValidatorClass;
    HcfArray data = { 0 };
    res = spiObj->engineValidate(&invalidSpi, &data);
    EXPECT_NE(res, HCF_SUCCESS);
    (void)spiObj->base.destroy(&(invalidSpi.base));
}

HWTEST_F(CryptoX509CertChainValidatorTest, InvalidMalloc, TestSize.Level0)
{
    SetMockFlag(true);
    HcfCertChainValidator *pathValidator = nullptr;
    HcfResult res = HcfCertChainValidatorCreate("PKIX", &pathValidator);
    EXPECT_EQ(res, HCF_ERR_MALLOC);
    HcfCertChainData certsData = { 0 };
    certsData.dataLen = 1;
    res = g_validator->validate(g_validator, &certsData);
    EXPECT_NE(res, HCF_SUCCESS);
    SetMockFlag(false);
}
}