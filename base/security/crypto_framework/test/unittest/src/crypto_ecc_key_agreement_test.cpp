/*
 * Copyright (C) 2022-2023 Huawei Device Co., Ltd.
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

#include "asy_key_generator.h"
#include "key_agreement.h"

using namespace std;
using namespace testing::ext;

namespace {
class CryptoEccKeyAgreementTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

    static HcfKeyPair *ecc224KeyPair_;
    static HcfKeyPair *ecc256KeyPair_;
    static HcfKeyPair *ecc384KeyPair_;
    static HcfKeyPair *ecc521KeyPair_;
};

HcfKeyPair *CryptoEccKeyAgreementTest::ecc224KeyPair_ = nullptr;
HcfKeyPair *CryptoEccKeyAgreementTest::ecc256KeyPair_ = nullptr;
HcfKeyPair *CryptoEccKeyAgreementTest::ecc384KeyPair_ = nullptr;
HcfKeyPair *CryptoEccKeyAgreementTest::ecc521KeyPair_ = nullptr;

void CryptoEccKeyAgreementTest::SetUp() {}
void CryptoEccKeyAgreementTest::TearDown() {}

void CryptoEccKeyAgreementTest::SetUpTestCase()
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);
    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(generator, nullptr);

    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);
    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyPair, nullptr);

    ecc224KeyPair_ = keyPair;

    HcfObjDestroy(generator);

    HcfAsyKeyGenerator *generator2 = NULL;
    res = HcfAsyKeyGeneratorCreate("ECC256", &generator2);
    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(generator2, nullptr);

    HcfKeyPair *keyPair2 = NULL;
    res = generator2->generateKeyPair(generator2, NULL, &keyPair2);
    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyPair2, nullptr);

    ecc256KeyPair_ = keyPair2;

    HcfObjDestroy(generator2);

    HcfAsyKeyGenerator *generator3 = NULL;
    res = HcfAsyKeyGeneratorCreate("ECC384", &generator3);
    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(generator3, nullptr);

    HcfKeyPair *keyPair3 = NULL;
    res = generator3->generateKeyPair(generator3, NULL, &keyPair3);
    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyPair3, nullptr);

    ecc384KeyPair_ = keyPair3;

    HcfObjDestroy(generator3);

    HcfAsyKeyGenerator *generator4 = NULL;
    res = HcfAsyKeyGeneratorCreate("ECC521", &generator4);
    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(generator4, nullptr);

    HcfKeyPair *keyPair4 = NULL;
    res = generator4->generateKeyPair(generator4, NULL, &keyPair4);
    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyPair4, nullptr);

    ecc521KeyPair_ = keyPair4;

    HcfObjDestroy(generator4);
}

void CryptoEccKeyAgreementTest::TearDownTestCase()
{
    HcfObjDestroy(ecc224KeyPair_);
    HcfObjDestroy(ecc256KeyPair_);
    HcfObjDestroy(ecc384KeyPair_);
    HcfObjDestroy(ecc521KeyPair_);
}

static const char *GetMockClass(void)
{
    return "HcfSymKeyGenerator";
}

static HcfObjectBase obj = {
    .getClass = GetMockClass,
    .destroy = NULL
};

HWTEST_F(CryptoEccKeyAgreementTest, CryptoEccKeyAgreementTest001, TestSize.Level0)
{
    HcfKeyAgreement *keyAgreement = NULL;
    int32_t res = HcfKeyAgreementCreate("ECC224", &keyAgreement);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyAgreement, nullptr);

    HcfObjDestroy(keyAgreement);
}

HWTEST_F(CryptoEccKeyAgreementTest, CryptoEccKeyAgreementTest002, TestSize.Level0)
{
    HcfKeyAgreement *keyAgreement = NULL;
    int32_t res = HcfKeyAgreementCreate("ECC256", &keyAgreement);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyAgreement, nullptr);

    HcfObjDestroy(keyAgreement);
}

HWTEST_F(CryptoEccKeyAgreementTest, CryptoEccKeyAgreementTest003, TestSize.Level0)
{
    HcfKeyAgreement *keyAgreement = NULL;
    int32_t res = HcfKeyAgreementCreate("ECC384", &keyAgreement);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyAgreement, nullptr);

    HcfObjDestroy(keyAgreement);
}

HWTEST_F(CryptoEccKeyAgreementTest, CryptoEccKeyAgreementTest004, TestSize.Level0)
{
    HcfKeyAgreement *keyAgreement = NULL;
    int32_t res = HcfKeyAgreementCreate("ECC521", &keyAgreement);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyAgreement, nullptr);

    HcfObjDestroy(keyAgreement);
}

HWTEST_F(CryptoEccKeyAgreementTest, CryptoEccKeyAgreementTest005, TestSize.Level0)
{
    HcfKeyAgreement *keyAgreement = NULL;
    int32_t res = HcfKeyAgreementCreate(NULL, &keyAgreement);

    ASSERT_EQ(res, HCF_INVALID_PARAMS);
    ASSERT_EQ(keyAgreement, nullptr);
}

HWTEST_F(CryptoEccKeyAgreementTest, CryptoEccKeyAgreementTest006, TestSize.Level0)
{
    HcfKeyAgreement *keyAgreement = NULL;
    int32_t res = HcfKeyAgreementCreate("ABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCD"
        "ABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCD", &keyAgreement);

    ASSERT_EQ(res, HCF_INVALID_PARAMS);
    ASSERT_EQ(keyAgreement, nullptr);
}

HWTEST_F(CryptoEccKeyAgreementTest, CryptoEccKeyAgreementTest007, TestSize.Level0)
{
    HcfKeyAgreement *keyAgreement = NULL;
    int32_t res = HcfKeyAgreementCreate("ECC257", &keyAgreement);

    ASSERT_EQ(res, HCF_INVALID_PARAMS);
    ASSERT_EQ(keyAgreement, nullptr);
}

HWTEST_F(CryptoEccKeyAgreementTest, CryptoEccKeyAgreementTest008, TestSize.Level0)
{
    int32_t res = HcfKeyAgreementCreate("ECC256", NULL);

    ASSERT_EQ(res, HCF_INVALID_PARAMS);
}

HWTEST_F(CryptoEccKeyAgreementTest, CryptoEccKeyAgreementTest101, TestSize.Level0)
{
    HcfKeyAgreement *keyAgreement = NULL;
    int32_t res = HcfKeyAgreementCreate("ECC256", &keyAgreement);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyAgreement, nullptr);

    const char *className = keyAgreement->base.getClass();

    ASSERT_NE(className, nullptr);

    HcfObjDestroy(keyAgreement);
}

HWTEST_F(CryptoEccKeyAgreementTest, CryptoEccKeyAgreementTest102, TestSize.Level0)
{
    HcfKeyAgreement *keyAgreement = NULL;
    int32_t res = HcfKeyAgreementCreate("ECC256", &keyAgreement);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyAgreement, nullptr);

    keyAgreement->base.destroy((HcfObjectBase *)keyAgreement);
}

HWTEST_F(CryptoEccKeyAgreementTest, CryptoEccKeyAgreementTest103, TestSize.Level0)
{
    HcfKeyAgreement *keyAgreement = NULL;
    int32_t res = HcfKeyAgreementCreate("ECC256", &keyAgreement);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyAgreement, nullptr);

    keyAgreement->base.destroy(NULL);

    HcfObjDestroy(keyAgreement);
}

HWTEST_F(CryptoEccKeyAgreementTest, CryptoEccKeyAgreementTest104, TestSize.Level0)
{
    HcfKeyAgreement *keyAgreement = NULL;
    int32_t res = HcfKeyAgreementCreate("ECC256", &keyAgreement);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyAgreement, nullptr);

    keyAgreement->base.destroy(&obj);

    HcfObjDestroy(keyAgreement);
}

HWTEST_F(CryptoEccKeyAgreementTest, CryptoEccKeyAgreementTest105, TestSize.Level0)
{
    HcfKeyAgreement *keyAgreement = NULL;
    int32_t res = HcfKeyAgreementCreate("ECC256", &keyAgreement);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyAgreement, nullptr);

    const char *algName = keyAgreement->getAlgoName(keyAgreement);

    ASSERT_NE(algName, nullptr);

    HcfObjDestroy(keyAgreement);
}

HWTEST_F(CryptoEccKeyAgreementTest, CryptoEccKeyAgreementTest106, TestSize.Level0)
{
    HcfKeyAgreement *keyAgreement = NULL;
    int32_t res = HcfKeyAgreementCreate("ECC256", &keyAgreement);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyAgreement, nullptr);

    const char *algName = keyAgreement->getAlgoName(NULL);

    ASSERT_EQ(algName, nullptr);

    HcfObjDestroy(keyAgreement);
}

HWTEST_F(CryptoEccKeyAgreementTest, CryptoEccKeyAgreementTest107, TestSize.Level0)
{
    HcfKeyAgreement *keyAgreement = NULL;
    int32_t res = HcfKeyAgreementCreate("ECC256", &keyAgreement);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyAgreement, nullptr);

    const char *algName = keyAgreement->getAlgoName((HcfKeyAgreement *)(&obj));

    ASSERT_EQ(algName, nullptr);

    HcfObjDestroy(keyAgreement);
}

HWTEST_F(CryptoEccKeyAgreementTest, CryptoEccKeyAgreementTest201, TestSize.Level0)
{
    HcfKeyAgreement *keyAgreement = NULL;
    int32_t res = HcfKeyAgreementCreate("ECC224", &keyAgreement);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyAgreement, nullptr);

    HcfBlob out = {
        .data = NULL,
        .len = 0
    };
    res = keyAgreement->generateSecret(keyAgreement, ecc224KeyPair_->priKey, ecc224KeyPair_->pubKey, &out);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(out.data, nullptr);
    ASSERT_NE(out.len, (const unsigned int)0);

    free(out.data);
    HcfObjDestroy(keyAgreement);
}

HWTEST_F(CryptoEccKeyAgreementTest, CryptoEccKeyAgreementTest202, TestSize.Level0)
{
    HcfKeyAgreement *keyAgreement = NULL;
    int32_t res = HcfKeyAgreementCreate("ECC256", &keyAgreement);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyAgreement, nullptr);

    HcfBlob out = {
        .data = NULL,
        .len = 0
    };
    res = keyAgreement->generateSecret(keyAgreement, ecc256KeyPair_->priKey, ecc256KeyPair_->pubKey, &out);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(out.data, nullptr);
    ASSERT_NE(out.len, (const unsigned int)0);

    free(out.data);
    HcfObjDestroy(keyAgreement);
}

HWTEST_F(CryptoEccKeyAgreementTest, CryptoEccKeyAgreementTest203, TestSize.Level0)
{
    HcfKeyAgreement *keyAgreement = NULL;
    int32_t res = HcfKeyAgreementCreate("ECC384", &keyAgreement);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyAgreement, nullptr);

    HcfBlob out = {
        .data = NULL,
        .len = 0
    };
    res = keyAgreement->generateSecret(keyAgreement, ecc384KeyPair_->priKey, ecc384KeyPair_->pubKey, &out);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(out.data, nullptr);
    ASSERT_NE(out.len, (const unsigned int)0);

    free(out.data);
    HcfObjDestroy(keyAgreement);
}

HWTEST_F(CryptoEccKeyAgreementTest, CryptoEccKeyAgreementTest204, TestSize.Level0)
{
    HcfKeyAgreement *keyAgreement = NULL;
    int32_t res = HcfKeyAgreementCreate("ECC521", &keyAgreement);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyAgreement, nullptr);

    HcfBlob out = {
        .data = NULL,
        .len = 0
    };
    res = keyAgreement->generateSecret(keyAgreement, ecc521KeyPair_->priKey, ecc521KeyPair_->pubKey, &out);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(out.data, nullptr);
    ASSERT_NE(out.len, (const unsigned int)0);

    free(out.data);
    HcfObjDestroy(keyAgreement);
}

HWTEST_F(CryptoEccKeyAgreementTest, CryptoEccKeyAgreementTest205, TestSize.Level0)
{
    HcfKeyAgreement *keyAgreement = NULL;
    int32_t res = HcfKeyAgreementCreate("ECC256", &keyAgreement);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyAgreement, nullptr);

    HcfBlob out = {
        .data = NULL,
        .len = 0
    };
    res = keyAgreement->generateSecret(NULL, ecc256KeyPair_->priKey, ecc256KeyPair_->pubKey, &out);

    ASSERT_EQ(res, HCF_INVALID_PARAMS);
    ASSERT_EQ(out.data, nullptr);
    ASSERT_EQ(out.len, 0);

    HcfObjDestroy(keyAgreement);
}

HWTEST_F(CryptoEccKeyAgreementTest, CryptoEccKeyAgreementTest206, TestSize.Level0)
{
    HcfKeyAgreement *keyAgreement = NULL;
    int32_t res = HcfKeyAgreementCreate("ECC256", &keyAgreement);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyAgreement, nullptr);

    HcfBlob out = {
        .data = NULL,
        .len = 0
    };
    res = keyAgreement->generateSecret((HcfKeyAgreement *)(&obj), ecc256KeyPair_->priKey, ecc256KeyPair_->pubKey, &out);

    ASSERT_EQ(res, HCF_INVALID_PARAMS);
    ASSERT_EQ(out.data, nullptr);
    ASSERT_EQ(out.len, 0);

    HcfObjDestroy(keyAgreement);
}

HWTEST_F(CryptoEccKeyAgreementTest, CryptoEccKeyAgreementTest207, TestSize.Level0)
{
    HcfKeyAgreement *keyAgreement = NULL;
    int32_t res = HcfKeyAgreementCreate("ECC256", &keyAgreement);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyAgreement, nullptr);

    HcfBlob out = {
        .data = NULL,
        .len = 0
    };
    res = keyAgreement->generateSecret(keyAgreement, (HcfPriKey *)(&obj), ecc256KeyPair_->pubKey, &out);

    ASSERT_EQ(res, HCF_INVALID_PARAMS);
    ASSERT_EQ(out.data, nullptr);
    ASSERT_EQ(out.len, 0);

    HcfObjDestroy(keyAgreement);
}

HWTEST_F(CryptoEccKeyAgreementTest, CryptoEccKeyAgreementTest208, TestSize.Level0)
{
    HcfKeyAgreement *keyAgreement = NULL;
    int32_t res = HcfKeyAgreementCreate("ECC256", &keyAgreement);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyAgreement, nullptr);

    HcfBlob out = {
        .data = NULL,
        .len = 0
    };
    res = keyAgreement->generateSecret(keyAgreement, ecc256KeyPair_->priKey, (HcfPubKey *)(&obj), &out);

    ASSERT_EQ(res, HCF_INVALID_PARAMS);
    ASSERT_EQ(out.data, nullptr);
    ASSERT_EQ(out.len, 0);

    HcfObjDestroy(keyAgreement);
}

HWTEST_F(CryptoEccKeyAgreementTest, CryptoEccKeyAgreementTest209, TestSize.Level0)
{
    HcfKeyAgreement *keyAgreement = NULL;
    int32_t res = HcfKeyAgreementCreate("ECC256", &keyAgreement);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyAgreement, nullptr);

    HcfBlob out = {
        .data = NULL,
        .len = 0
    };
    res = keyAgreement->generateSecret(keyAgreement, NULL, NULL, &out);

    ASSERT_EQ(res, HCF_INVALID_PARAMS);
    ASSERT_EQ(out.data, nullptr);
    ASSERT_EQ(out.len, 0);

    HcfObjDestroy(keyAgreement);
}

HWTEST_F(CryptoEccKeyAgreementTest, CryptoEccKeyAgreementTest210, TestSize.Level0)
{
    HcfKeyAgreement *keyAgreement = NULL;
    int32_t res = HcfKeyAgreementCreate("ECC256", &keyAgreement);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyAgreement, nullptr);

    res = keyAgreement->generateSecret(keyAgreement, ecc256KeyPair_->priKey, ecc256KeyPair_->pubKey, NULL);

    ASSERT_EQ(res, HCF_INVALID_PARAMS);

    HcfObjDestroy(keyAgreement);
}
}
