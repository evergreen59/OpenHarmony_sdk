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
#include "asy_key_generator.h"
#include "cipher.h"
#include "key_pair.h"
#include "memory.h"
#include "cstring"

using namespace std;
using namespace testing::ext;

namespace {
class CryptoRsaCipherTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void CryptoRsaCipherTest::SetUpTestCase() {}
void CryptoRsaCipherTest::TearDownTestCase() {}
void CryptoRsaCipherTest::SetUp() {}
void CryptoRsaCipherTest::TearDown() {}


HWTEST_F(CryptoRsaCipherTest, CryptoRsaCipherTest90, TestSize.Level0)
{
    HcfResult res = HCF_SUCCESS;
    HcfCipher *cipher = NULL;
    res = HcfCipherCreate("RSA1024|NoPadding", &cipher);
    EXPECT_EQ(res, HCF_SUCCESS);
    EXPECT_NE(cipher, nullptr);
    EXPECT_NE(cipher->base.getClass(), nullptr);
    EXPECT_NE(cipher->base.destroy, nullptr);
    EXPECT_NE(cipher->init, nullptr);
    EXPECT_NE(cipher->update, nullptr);
    EXPECT_NE(cipher->doFinal, nullptr);
    EXPECT_NE(cipher->getAlgorithm, nullptr);
    HcfObjDestroy(cipher);
}


// HcfCipherCreate correct case: RSAXXX + padding
HWTEST_F(CryptoRsaCipherTest, CryptoRsaCipherTest100, TestSize.Level0)
{
    HcfResult res = HCF_SUCCESS;
    HcfCipher *cipher = NULL;
    res = HcfCipherCreate("RSA1024|NoPadding", &cipher);
    EXPECT_EQ(res, HCF_SUCCESS);
    EXPECT_NE(cipher, nullptr);
    EXPECT_NE(cipher->base.getClass(), nullptr);
    EXPECT_NE(cipher->base.destroy, nullptr);
    EXPECT_NE(cipher->init, nullptr);
    EXPECT_NE(cipher->update, nullptr);
    EXPECT_NE(cipher->doFinal, nullptr);
    HcfObjDestroy(cipher);
}

HWTEST_F(CryptoRsaCipherTest, CryptoRsaCipherTest110, TestSize.Level0)
{
    HcfResult res = HCF_SUCCESS;
    HcfCipher *cipher = NULL;
    res = HcfCipherCreate("RSA1024|NoPadding", &cipher);
    EXPECT_EQ(res, HCF_SUCCESS);
    EXPECT_NE(cipher, nullptr);
    EXPECT_NE(cipher->base.getClass(), nullptr);
    EXPECT_NE(cipher->base.destroy, nullptr);
    EXPECT_NE(cipher->init, nullptr);
    EXPECT_NE(cipher->update, nullptr);
    EXPECT_NE(cipher->doFinal, nullptr);
    HcfObjDestroy(cipher);
}

HWTEST_F(CryptoRsaCipherTest, CryptoRsaCipherTest120, TestSize.Level0)
{
    HcfResult res = HCF_SUCCESS;
    HcfCipher *cipher = NULL;
    res = HcfCipherCreate("RSA1024|PKCS1", &cipher);
    EXPECT_EQ(res, HCF_SUCCESS);
    EXPECT_NE(cipher, nullptr);
    EXPECT_NE(cipher->base.getClass(), nullptr);
    EXPECT_NE(cipher->base.destroy, nullptr);
    EXPECT_NE(cipher->init, nullptr);
    EXPECT_NE(cipher->update, nullptr);
    EXPECT_NE(cipher->doFinal, nullptr);
    HcfObjDestroy(cipher);
}

HWTEST_F(CryptoRsaCipherTest, CryptoRsaCipherTest130, TestSize.Level0)
{
    HcfResult res = HCF_SUCCESS;
    HcfCipher *cipher = NULL;
    res = HcfCipherCreate("RSA1024|PKCS1_OAEP|SHA1|MGF1_SHA1", &cipher);
    EXPECT_EQ(res, HCF_SUCCESS);
    EXPECT_NE(cipher, nullptr);
    EXPECT_NE(cipher->base.getClass(), nullptr);
    EXPECT_NE(cipher->base.destroy, nullptr);
    EXPECT_NE(cipher->init, nullptr);
    EXPECT_NE(cipher->update, nullptr);
    EXPECT_NE(cipher->doFinal, nullptr);
    HcfObjDestroy(cipher);
}

HWTEST_F(CryptoRsaCipherTest, CryptoRsaCipherTest140, TestSize.Level0)
{
    HcfResult res = HCF_SUCCESS;
    HcfCipher *cipher = NULL;
    res = HcfCipherCreate("RSA1024|PKCS1_OAEP|SHA256|MGF1_SHA256", &cipher);
    EXPECT_EQ(res, HCF_SUCCESS);
    EXPECT_NE(cipher, nullptr);
    EXPECT_NE(cipher->base.getClass(), nullptr);
    EXPECT_NE(cipher->base.destroy, nullptr);
    EXPECT_NE(cipher->init, nullptr);
    EXPECT_NE(cipher->update, nullptr);
    EXPECT_NE(cipher->doFinal, nullptr);
    HcfObjDestroy(cipher);
}

HWTEST_F(CryptoRsaCipherTest, CryptoRsaCipherTest150, TestSize.Level0)
{
    HcfResult res = HCF_SUCCESS;
    HcfCipher *cipher = NULL;
    res = HcfCipherCreate("RSA1024|PKCS1_OAEP|SHA512|MGF1_SHA256", &cipher);
    EXPECT_EQ(res, HCF_SUCCESS);
    EXPECT_NE(cipher, nullptr);
    EXPECT_NE(cipher->base.getClass(), nullptr);
    EXPECT_NE(cipher->base.destroy, nullptr);
    EXPECT_NE(cipher->init, nullptr);
    EXPECT_NE(cipher->update, nullptr);
    EXPECT_NE(cipher->doFinal, nullptr);
    HcfObjDestroy(cipher);
}

HWTEST_F(CryptoRsaCipherTest, CryptoRsaCipherTest160, TestSize.Level0)
{
    HcfResult res = HCF_SUCCESS;
    HcfCipher *cipher = NULL;
    res = HcfCipherCreate("RSA1024|PKCS1_OAEP|SHA1|MGF1_SHA256", &cipher);
    EXPECT_EQ(res, HCF_SUCCESS);
    EXPECT_NE(cipher, nullptr);
    EXPECT_NE(cipher->base.getClass(), nullptr);
    EXPECT_NE(cipher->base.destroy, nullptr);
    EXPECT_NE(cipher->init, nullptr);
    EXPECT_NE(cipher->update, nullptr);
    EXPECT_NE(cipher->doFinal, nullptr);
    HcfObjDestroy(cipher);
}

HWTEST_F(CryptoRsaCipherTest, CryptoRsaCipherTest170, TestSize.Level0)
{
    HcfResult res = HCF_SUCCESS;
    HcfCipher *cipher = NULL;
    res = HcfCipherCreate("RSA1024|PKCS1_OAEP|SHA1|MGF1_SHA1", &cipher);
    EXPECT_EQ(res, HCF_SUCCESS);
    EXPECT_NE(cipher, nullptr);
    EXPECT_NE(cipher->base.getClass(), nullptr);
    EXPECT_NE(cipher->base.destroy, nullptr);
    EXPECT_NE(cipher->init, nullptr);
    EXPECT_NE(cipher->update, nullptr);
    EXPECT_NE(cipher->doFinal, nullptr);
    HcfObjDestroy(cipher);
}

HWTEST_F(CryptoRsaCipherTest, CryptoRsaCipherTest180, TestSize.Level0)
{
    HcfResult res = HCF_SUCCESS;
    HcfCipher *cipher = NULL;
    res = HcfCipherCreate("RSA1024|PKCS1_OAEP|SHA1|MGF1_MD5", &cipher);
    EXPECT_EQ(res, HCF_SUCCESS);
    EXPECT_NE(cipher, nullptr);
    EXPECT_NE(cipher->base.getClass(), nullptr);
    EXPECT_NE(cipher->base.destroy, nullptr);
    EXPECT_NE(cipher->init, nullptr);
    EXPECT_NE(cipher->update, nullptr);
    EXPECT_NE(cipher->doFinal, nullptr);
    HcfObjDestroy(cipher);
}

HWTEST_F(CryptoRsaCipherTest, CryptoRsaCipherTest190, TestSize.Level0)
{
    HcfResult res = HCF_SUCCESS;
    HcfCipher *cipher = NULL;
    res = HcfCipherCreate("RSA1024|PKCS1_OAEP|SHA1|MGF1_SHA512", &cipher);
    EXPECT_EQ(res, HCF_SUCCESS);
    EXPECT_NE(cipher, nullptr);
    EXPECT_NE(cipher->base.getClass(), nullptr);
    EXPECT_NE(cipher->base.destroy, nullptr);
    EXPECT_NE(cipher->init, nullptr);
    EXPECT_NE(cipher->update, nullptr);
    EXPECT_NE(cipher->doFinal, nullptr);
    HcfObjDestroy(cipher);
}

HWTEST_F(CryptoRsaCipherTest, CryptoRsaCipherTest200, TestSize.Level0)
{
    HcfResult res = HCF_SUCCESS;
    HcfCipher *cipher = NULL;
    res = HcfCipherCreate("RSA512|NoPadding", &cipher);
    EXPECT_EQ(res, HCF_SUCCESS);
    EXPECT_NE(cipher, nullptr);
    EXPECT_NE(cipher->base.getClass(), nullptr);
    EXPECT_NE(cipher->base.destroy, nullptr);
    EXPECT_NE(cipher->init, nullptr);
    EXPECT_NE(cipher->update, nullptr);
    EXPECT_NE(cipher->doFinal, nullptr);
    HcfObjDestroy(cipher);
}

HWTEST_F(CryptoRsaCipherTest, CryptoRsaCipherTest210, TestSize.Level0)
{
    HcfResult res = HCF_SUCCESS;
    HcfCipher *cipher = NULL;
    res = HcfCipherCreate("RSA768|NoPadding", &cipher);
    EXPECT_EQ(res, HCF_SUCCESS);
    EXPECT_NE(cipher, nullptr);
    EXPECT_NE(cipher->base.getClass(), nullptr);
    EXPECT_NE(cipher->base.destroy, nullptr);
    EXPECT_NE(cipher->init, nullptr);
    EXPECT_NE(cipher->update, nullptr);
    EXPECT_NE(cipher->doFinal, nullptr);
    HcfObjDestroy(cipher);
}

HWTEST_F(CryptoRsaCipherTest, CryptoRsaCipherTest220, TestSize.Level0)
{
    HcfResult res = HCF_SUCCESS;
    HcfCipher *cipher = NULL;
    res = HcfCipherCreate("RSA2048|NoPadding", &cipher);
    EXPECT_EQ(res, HCF_SUCCESS);
    EXPECT_NE(cipher, nullptr);
    EXPECT_NE(cipher->base.getClass(), nullptr);
    EXPECT_NE(cipher->base.destroy, nullptr);
    EXPECT_NE(cipher->init, nullptr);
    EXPECT_NE(cipher->update, nullptr);
    EXPECT_NE(cipher->doFinal, nullptr);
    HcfObjDestroy(cipher);
}

HWTEST_F(CryptoRsaCipherTest, CryptoRsaCipherTest230, TestSize.Level0)
{
    HcfResult res = HCF_SUCCESS;
    HcfCipher *cipher = NULL;
    res = HcfCipherCreate("RSA512|PKCS1", &cipher);
    EXPECT_EQ(res, HCF_SUCCESS);
    EXPECT_NE(cipher, nullptr);
    EXPECT_NE(cipher->base.getClass(), nullptr);
    EXPECT_NE(cipher->base.destroy, nullptr);
    EXPECT_NE(cipher->init, nullptr);
    EXPECT_NE(cipher->update, nullptr);
    EXPECT_NE(cipher->doFinal, nullptr);
    HcfObjDestroy(cipher);
}

HWTEST_F(CryptoRsaCipherTest, CryptoRsaCipherTest240, TestSize.Level0)
{
    HcfResult res = HCF_SUCCESS;
    HcfCipher *cipher = NULL;
    res = HcfCipherCreate("RSA768|PKCS1", &cipher);
    EXPECT_EQ(res, HCF_SUCCESS);
    EXPECT_NE(cipher, nullptr);
    EXPECT_NE(cipher->base.getClass(), nullptr);
    EXPECT_NE(cipher->base.destroy, nullptr);
    EXPECT_NE(cipher->init, nullptr);
    EXPECT_NE(cipher->update, nullptr);
    EXPECT_NE(cipher->doFinal, nullptr);
    HcfObjDestroy(cipher);
}

HWTEST_F(CryptoRsaCipherTest, CryptoRsaCipherTest250, TestSize.Level0)
{
    HcfResult res = HCF_SUCCESS;
    HcfCipher *cipher = NULL;
    res = HcfCipherCreate("RSA2048|PKCS1", &cipher);
    EXPECT_EQ(res, HCF_SUCCESS);
    EXPECT_NE(cipher, nullptr);
    EXPECT_NE(cipher->base.getClass(), nullptr);
    EXPECT_NE(cipher->base.destroy, nullptr);
    EXPECT_NE(cipher->init, nullptr);
    EXPECT_NE(cipher->update, nullptr);
    EXPECT_NE(cipher->doFinal, nullptr);
    HcfObjDestroy(cipher);
}

HWTEST_F(CryptoRsaCipherTest, CryptoRsaCipherTest260, TestSize.Level0)
{
    HcfResult res = HCF_SUCCESS;
    HcfCipher *cipher = NULL;
    res = HcfCipherCreate("RSA512|PKCS1_OAEP|SHA256|MGF1_SHA256", &cipher);
    EXPECT_EQ(res, HCF_SUCCESS);
    EXPECT_NE(cipher, nullptr);
    EXPECT_NE(cipher->base.getClass(), nullptr);
    EXPECT_NE(cipher->base.destroy, nullptr);
    EXPECT_NE(cipher->init, nullptr);
    EXPECT_NE(cipher->update, nullptr);
    EXPECT_NE(cipher->doFinal, nullptr);
    HcfObjDestroy(cipher);
}

HWTEST_F(CryptoRsaCipherTest, CryptoRsaCipherTest270, TestSize.Level0)
{
    HcfResult res = HCF_SUCCESS;
    HcfCipher *cipher = NULL;
    res = HcfCipherCreate("RSA768|PKCS1_OAEP|SHA256|MGF1_SHA256", &cipher);
    EXPECT_EQ(res, HCF_SUCCESS);
    EXPECT_NE(cipher, nullptr);
    EXPECT_NE(cipher->base.getClass(), nullptr);
    EXPECT_NE(cipher->base.destroy, nullptr);
    EXPECT_NE(cipher->init, nullptr);
    EXPECT_NE(cipher->update, nullptr);
    EXPECT_NE(cipher->doFinal, nullptr);
    HcfObjDestroy(cipher);
}

HWTEST_F(CryptoRsaCipherTest, CryptoRsaCipherTest280, TestSize.Level0)
{
    HcfResult res = HCF_SUCCESS;
    HcfCipher *cipher = NULL;
    res = HcfCipherCreate("RSA2048|PKCS1_OAEP|SHA256|MGF1_SHA256", &cipher);
    EXPECT_EQ(res, HCF_SUCCESS);
    EXPECT_NE(cipher, nullptr);
    EXPECT_NE(cipher->base.getClass(), nullptr);
    EXPECT_NE(cipher->base.destroy, nullptr);
    EXPECT_NE(cipher->init, nullptr);
    EXPECT_NE(cipher->update, nullptr);
    EXPECT_NE(cipher->doFinal, nullptr);
    HcfObjDestroy(cipher);
}

// HcfCipherCreate Incorrect case
HWTEST_F(CryptoRsaCipherTest, CryptoRsaCipherTest300, TestSize.Level0)
{
    HcfResult res = HCF_SUCCESS;
    res = HcfCipherCreate("RSA2048|PKCS1_OAEP|SHA256|MGF1_SHA256", NULL);
    EXPECT_NE(res, HCF_SUCCESS);
}

HWTEST_F(CryptoRsaCipherTest, CryptoRsaCipherTest310, TestSize.Level0)
{
    HcfResult res = HCF_SUCCESS;
    HcfCipher *cipher = NULL;
    res = HcfCipherCreate(NULL, &cipher);
    EXPECT_NE(res, HCF_SUCCESS);
    EXPECT_EQ(cipher, nullptr);
}

HWTEST_F(CryptoRsaCipherTest, CryptoRsaCipherTest320, TestSize.Level0)
{
    HcfResult res = HCF_SUCCESS;
    HcfCipher *cipher = NULL;
    res = HcfCipherCreate("RSA2048|PKCS1_OAEP|SHA256|MGF1_SHA256|2111111111111111111111111111111111111111111111"
        "111111111111123123", &cipher);
    EXPECT_NE(res, HCF_SUCCESS);
    EXPECT_EQ(cipher, nullptr);
}

HWTEST_F(CryptoRsaCipherTest, CryptoRsaCipherTest330, TestSize.Level0)
{
    HcfResult res = HCF_SUCCESS;
    HcfCipher *cipher = NULL;
    res = HcfCipherCreate("RSA2048|PKCS1_OAEP|SHA256|MGF1_SHA333", &cipher);
    EXPECT_NE(res, HCF_SUCCESS);
    EXPECT_EQ(cipher, nullptr);
}

HWTEST_F(CryptoRsaCipherTest, CryptoRsaCipherTest340, TestSize.Level0)
{
    HcfResult res = HCF_SUCCESS;
    HcfCipher *cipher = NULL;
    res = HcfCipherCreate("RSA2048|PKCS1_OAEP111|SHA256|MGF1_SHA333", &cipher);
    EXPECT_NE(res, HCF_SUCCESS);
    EXPECT_EQ(cipher, nullptr);
}

HWTEST_F(CryptoRsaCipherTest, CryptoRsaCipherTest350, TestSize.Level0)
{
    HcfResult res = HCF_SUCCESS;
    HcfCipher *cipher = NULL;
    res = HcfCipherCreate("RSA2111048|PKCS1_OAEP|SHA256|MGF1_SHA256", &cipher);
    EXPECT_NE(res, HCF_SUCCESS);
    EXPECT_EQ(cipher, nullptr);
}

HWTEST_F(CryptoRsaCipherTest, CryptoRsaCipherTest360, TestSize.Level0)
{
    HcfResult res = HCF_SUCCESS;
    HcfCipher *cipher = NULL;
    res = HcfCipherCreate("RSA2048|PKCS1_OAEP|SHA256111|MGF1_SHA256", &cipher);
    EXPECT_NE(res, HCF_SUCCESS);
    EXPECT_EQ(cipher, nullptr);
}

// Create Cipher without padding
HWTEST_F(CryptoRsaCipherTest, CryptoRsaCipherTest370, TestSize.Level0)
{
    HcfResult res = HCF_SUCCESS;
    HcfCipher *cipher = NULL;
    res = HcfCipherCreate("RSA2048", &cipher);
    EXPECT_NE(res, HCF_SUCCESS);
}

// create Nopadding Cipher with md digest.
HWTEST_F(CryptoRsaCipherTest, CryptoRsaCipherTest380, TestSize.Level0)
{
    HcfResult res = HCF_SUCCESS;
    HcfCipher *cipher = NULL;
    res = HcfCipherCreate("RSA2048|NoPadding|SHA256", &cipher);
    EXPECT_NE(res, HCF_SUCCESS);
}


// destroyCipher
HWTEST_F(CryptoRsaCipherTest, CryptoRsaCipherTest400, TestSize.Level0)
{
    HcfResult res = HCF_SUCCESS;
    HcfCipher *cipher = NULL;
    res = HcfCipherCreate("RSA2048|PKCS1_OAEP|SHA256|MGF1_SHA256", &cipher);
    EXPECT_EQ(res, HCF_SUCCESS);
    EXPECT_NE(cipher, nullptr);
    HcfObjDestroy(cipher);
    cipher = NULL;
    HcfObjDestroy(cipher);
}

// init correct case
HWTEST_F(CryptoRsaCipherTest, CryptoRsaCipherTest500, TestSize.Level0)
{
    HcfResult res = HCF_SUCCESS;
    HcfAsyKeyGenerator *generator = NULL;
    res = HcfAsyKeyGeneratorCreate("RSA1024|PRIMES_2", &generator);
    EXPECT_EQ(res, HCF_SUCCESS);
    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);
    EXPECT_EQ(res, HCF_SUCCESS);

    HcfCipher *cipher = NULL;
    res = HcfCipherCreate("RSA1024|PKCS1", &cipher);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = cipher->init(cipher, ENCRYPT_MODE, (HcfKey *)keyPair->pubKey, NULL);
    EXPECT_EQ(res, HCF_SUCCESS);

    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
    HcfObjDestroy(cipher);
}

HWTEST_F(CryptoRsaCipherTest, CryptoRsaCipherTest501, TestSize.Level0)
{
    HcfResult res = HCF_SUCCESS;
    HcfAsyKeyGenerator *generator = NULL;
    res = HcfAsyKeyGeneratorCreate("RSA1024|PRIMES_2", &generator);
    EXPECT_EQ(res, HCF_SUCCESS);
    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);
    EXPECT_EQ(res, HCF_SUCCESS);

    HcfCipher *cipher = NULL;
    res = HcfCipherCreate("RSA1024|PKCS1", &cipher);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = cipher->init(cipher, (enum HcfCryptoMode)123, (HcfKey *)keyPair->pubKey, NULL);
    EXPECT_NE(res, HCF_SUCCESS);

    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
    HcfObjDestroy(cipher);
}

HWTEST_F(CryptoRsaCipherTest, CryptoRsaCipherTest510, TestSize.Level0)
{
    HcfResult res = HCF_SUCCESS;
    HcfAsyKeyGenerator *generator = NULL;
    res = HcfAsyKeyGeneratorCreate("RSA1024|PRIMES_2", &generator);
    EXPECT_EQ(res, HCF_SUCCESS);
    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);
    EXPECT_EQ(res, HCF_SUCCESS);

    HcfCipher *cipher = NULL;
    res = HcfCipherCreate("RSA1024|PKCS1", &cipher);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = cipher->init(cipher, DECRYPT_MODE, (HcfKey *)keyPair->priKey, NULL);
    EXPECT_EQ(res, HCF_SUCCESS);

    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
    HcfObjDestroy(cipher);
}

// init incorrect case
HWTEST_F(CryptoRsaCipherTest, CryptoRsaCipherTest600, TestSize.Level0)
{
    HcfResult res = HCF_SUCCESS;
    HcfAsyKeyGenerator *generator = NULL;
    res = HcfAsyKeyGeneratorCreate("RSA1024|PRIMES_2", &generator);
    EXPECT_EQ(res, HCF_SUCCESS);
    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);
    EXPECT_EQ(res, HCF_SUCCESS);

    HcfCipher *cipher = NULL;
    res = HcfCipherCreate("RSA1024|PKCS1", &cipher);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = cipher->init(cipher, ENCRYPT_MODE, (HcfKey *)keyPair->priKey, NULL);
    EXPECT_NE(res, HCF_SUCCESS);

    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
    HcfObjDestroy(cipher);
}

HWTEST_F(CryptoRsaCipherTest, CryptoRsaCipherTest610, TestSize.Level0)
{
    HcfResult res = HCF_SUCCESS;
    HcfAsyKeyGenerator *generator = NULL;
    res = HcfAsyKeyGeneratorCreate("RSA1024|PRIMES_2", &generator);
    EXPECT_EQ(res, HCF_SUCCESS);
    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);
    EXPECT_EQ(res, HCF_SUCCESS);

    HcfCipher *cipher = NULL;
    res = HcfCipherCreate("RSA1024|PKCS1", &cipher);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = cipher->init(cipher, DECRYPT_MODE, (HcfKey *)keyPair->pubKey, NULL);
    EXPECT_NE(res, HCF_SUCCESS);

    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
    HcfObjDestroy(cipher);
}

HWTEST_F(CryptoRsaCipherTest, CryptoRsaCipherTest620, TestSize.Level0)
{
    HcfResult res = HCF_SUCCESS;
    HcfAsyKeyGenerator *generator = NULL;
    res = HcfAsyKeyGeneratorCreate("RSA1024|PRIMES_2", &generator);
    EXPECT_EQ(res, HCF_SUCCESS);
    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);
    EXPECT_EQ(res, HCF_SUCCESS);

    HcfCipher *cipher = NULL;
    res = HcfCipherCreate("RSA1024|PKCS1", &cipher);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = cipher->init(NULL, DECRYPT_MODE, (HcfKey *)keyPair->pubKey, NULL);
    EXPECT_NE(res, HCF_SUCCESS);

    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
    HcfObjDestroy(cipher);
}

// incorrect case: use diff class, we ignore it in this version
HWTEST_F(CryptoRsaCipherTest, CryptoRsaCipherTest630, TestSize.Level0)
{
    HcfResult res = HCF_SUCCESS;
    HcfAsyKeyGenerator *generator = NULL;
    res = HcfAsyKeyGeneratorCreate("RSA1024|PRIMES_2", &generator);
    EXPECT_EQ(res, HCF_SUCCESS);
    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);
    EXPECT_EQ(res, HCF_SUCCESS);

    HcfCipher *aesCipher = NULL;
    res = HcfCipherCreate("AES128|ECB|PKCS7", &aesCipher);
    EXPECT_EQ(res, HCF_SUCCESS);

    HcfCipher *cipher = NULL;
    res = HcfCipherCreate("RSA1024|PKCS1", &cipher);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = cipher->init(aesCipher, DECRYPT_MODE, (HcfKey *)keyPair->pubKey, NULL);
    EXPECT_NE(res, HCF_SUCCESS);

    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
    HcfObjDestroy(cipher);
    HcfObjDestroy(aesCipher);
}

HWTEST_F(CryptoRsaCipherTest, CryptoRsaCipherTest640, TestSize.Level0)
{
    HcfResult res = HCF_SUCCESS;
    HcfCipher *cipher = NULL;
    res = HcfCipherCreate("RSA1024|PKCS1", &cipher);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = cipher->init(cipher, DECRYPT_MODE, NULL, NULL);
    EXPECT_NE(res, HCF_SUCCESS);
    HcfObjDestroy(cipher);
}

// correct case: update not support
HWTEST_F(CryptoRsaCipherTest, CryptoRsaCipherTest700, TestSize.Level0)
{
    uint8_t plan[] = "this is rsa cipher test!\0";
    HcfResult res = HCF_SUCCESS;
    HcfAsyKeyGenerator *generator = NULL;
    res = HcfAsyKeyGeneratorCreate("RSA1024|PRIMES_2", &generator);
    EXPECT_EQ(res, HCF_SUCCESS);
    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);
    EXPECT_EQ(res, HCF_SUCCESS);

    HcfBlob input = {.data = (uint8_t *)plan, .len = strlen((char *)plan)};
    HcfBlob encoutput = {.data = NULL, .len = 0};
    HcfCipher *cipher = NULL;
    res = HcfCipherCreate("RSA1024|PKCS1", &cipher);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = cipher->init(cipher, ENCRYPT_MODE, (HcfKey *)keyPair->pubKey, NULL);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = cipher->update(cipher, &input, &encoutput);
    EXPECT_NE(res, HCF_SUCCESS);
    EXPECT_EQ(encoutput.data, nullptr);

    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
    HcfObjDestroy(cipher);
}

// dofinal correct case
HWTEST_F(CryptoRsaCipherTest, CryptoRsaCipherTest800, TestSize.Level0)
{
    HcfResult res = HCF_SUCCESS;
    uint8_t plan[] = "this is rsa cipher test!\0";
    HcfAsyKeyGenerator *generator = NULL;
    res = HcfAsyKeyGeneratorCreate("RSA1024|PRIMES_2", &generator);
    EXPECT_EQ(res, HCF_SUCCESS);
    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);
    EXPECT_EQ(res, HCF_SUCCESS);
    EXPECT_NE(keyPair, nullptr);
    EXPECT_NE(keyPair->priKey, nullptr);
    EXPECT_NE(keyPair->pubKey, nullptr);

    HcfBlob input = {.data = (uint8_t *)plan, .len = strlen((char *)plan)};
    HcfBlob encoutput = {.data = NULL, .len = 0};
    HcfCipher *cipher = NULL;
    res = HcfCipherCreate("RSA1024|PKCS1", &cipher);
    EXPECT_EQ(res, HCF_SUCCESS);

    res = cipher->init(cipher, ENCRYPT_MODE, (HcfKey *)keyPair->pubKey, NULL);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = cipher->doFinal(cipher, &input, &encoutput);
    EXPECT_EQ(res, HCF_SUCCESS);
    HcfObjDestroy(cipher);

    HcfBlob decoutput = {.data = NULL, .len = 0};
    cipher = NULL;
    res = HcfCipherCreate("RSA1024|PKCS1", &cipher);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = cipher->init(cipher, DECRYPT_MODE, (HcfKey *)keyPair->priKey, NULL);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = cipher->doFinal(cipher, &encoutput, &decoutput);
    EXPECT_EQ(res, HCF_SUCCESS);
    HcfObjDestroy(cipher);
    EXPECT_STREQ((char *)plan, (char *)decoutput.data);

    HcfFree(encoutput.data);
    HcfFree(decoutput.data);

    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoRsaCipherTest, CryptoRsaCipherTest810, TestSize.Level0)
{
    HcfResult res = HCF_SUCCESS;
    uint8_t plan[] = "this is rsa cipher test!\0";
    HcfAsyKeyGenerator *generator = NULL;
    res = HcfAsyKeyGeneratorCreate("RSA4096|PRIMES_3", &generator);
    EXPECT_EQ(res, HCF_SUCCESS);
    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);
    EXPECT_EQ(res, HCF_SUCCESS);
    EXPECT_NE(keyPair, nullptr);
    EXPECT_NE(keyPair->priKey, nullptr);
    EXPECT_NE(keyPair->pubKey, nullptr);

    HcfBlob input = {.data = (uint8_t *)plan, .len = strlen((char *)plan)};
    HcfBlob encoutput = {.data = NULL, .len = 0};
    HcfCipher *cipher = NULL;
    res = HcfCipherCreate("RSA1024|PKCS1", &cipher);
    EXPECT_EQ(res, HCF_SUCCESS);

    res = cipher->init(cipher, ENCRYPT_MODE, (HcfKey *)keyPair->pubKey, NULL);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = cipher->doFinal(cipher, &input, &encoutput);
    EXPECT_EQ(res, HCF_SUCCESS);
    HcfObjDestroy(cipher);

    HcfBlob decoutput = {.data = NULL, .len = 0};
    cipher = NULL;
    res = HcfCipherCreate("RSA1024|PKCS1", &cipher);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = cipher->init(cipher, DECRYPT_MODE, (HcfKey *)keyPair->priKey, NULL);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = cipher->doFinal(cipher, &encoutput, &decoutput);
    EXPECT_EQ(res, HCF_SUCCESS);
    HcfObjDestroy(cipher);
    EXPECT_STREQ((char *)plan, (char *)decoutput.data);

    HcfFree(encoutput.data);
    HcfFree(decoutput.data);

    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoRsaCipherTest, CryptoRsaCipherTest820, TestSize.Level0)
{
    HcfResult res = HCF_SUCCESS;
    uint8_t plan[] = "this is rsa cipher test!\0";
    HcfAsyKeyGenerator *generator = NULL;
    res = HcfAsyKeyGeneratorCreate("RSA1024|PRIMES_2", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);
    EXPECT_EQ(res, HCF_SUCCESS);
    EXPECT_NE(keyPair, nullptr);
    EXPECT_NE(keyPair->priKey, nullptr);
    EXPECT_NE(keyPair->pubKey, nullptr);

    HcfBlob input = {.data = (uint8_t *)plan, .len = strlen((char *)plan)};
    HcfBlob encoutput = {.data = NULL, .len = 0};
    HcfCipher *cipher = NULL;
    res = HcfCipherCreate("RSA1024|PKCS1_OAEP|SHA256|MGF1_SHA256", &cipher);
    EXPECT_EQ(res, HCF_SUCCESS);

    res = cipher->init(cipher, ENCRYPT_MODE, (HcfKey *)keyPair->pubKey, NULL);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = cipher->doFinal(cipher, &input, &encoutput);
    EXPECT_EQ(res, HCF_SUCCESS);
    HcfObjDestroy(cipher);

    HcfBlob decoutput = {.data = NULL, .len = 0};
    cipher = NULL;
    res = HcfCipherCreate("RSA1024|PKCS1_OAEP|SHA256|MGF1_SHA256", &cipher);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = cipher->init(cipher, DECRYPT_MODE, (HcfKey *)keyPair->priKey, NULL);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = cipher->doFinal(cipher, &encoutput, &decoutput);
    EXPECT_EQ(res, HCF_SUCCESS);
    HcfObjDestroy(cipher);
    EXPECT_STREQ((char *)plan, (char *)decoutput.data);

    HcfFree(encoutput.data);
    HcfFree(decoutput.data);

    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

// incorrect case: algName out of boundary
HWTEST_F(CryptoRsaCipherTest, CryptoRsaCipherTest830, TestSize.Level0)
{
    HcfResult res = HCF_SUCCESS;
    uint8_t plan[] = "this is rsa cipher test aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
        "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa!\0";
    HcfAsyKeyGenerator *generator = NULL;
    res = HcfAsyKeyGeneratorCreate("RSA1024|PRIMES_2", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);
    EXPECT_EQ(res, HCF_SUCCESS);
    EXPECT_NE(keyPair, nullptr);
    EXPECT_NE(keyPair->priKey, nullptr);
    EXPECT_NE(keyPair->pubKey, nullptr);

    HcfBlob input = {.data = (uint8_t *)plan, .len = strlen((char *)plan)};
    HcfBlob encoutput = {.data = NULL, .len = 0};
    HcfCipher *cipher = NULL;
    res = HcfCipherCreate("RSA1024|PKCS1_OAEP|SHA256|MGF1_SHA256", &cipher);
    EXPECT_EQ(res, HCF_SUCCESS);

    res = cipher->init(cipher, ENCRYPT_MODE, (HcfKey *)keyPair->pubKey, NULL);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = cipher->doFinal(cipher, &input, &encoutput);
    EXPECT_NE(res, HCF_SUCCESS);
    HcfObjDestroy(cipher);

    HcfBlob decoutput = {.data = NULL, .len = 0};
    cipher = NULL;
    res = HcfCipherCreate("RSA1024|PKCS1_OAEP|SHA256|MGF1_SHA256", &cipher);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = cipher->init(cipher, DECRYPT_MODE, (HcfKey *)keyPair->priKey, NULL);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = cipher->doFinal(cipher, &encoutput, &decoutput);
    EXPECT_NE(res, HCF_SUCCESS);
    HcfObjDestroy(cipher);
    EXPECT_STRNE((char *)plan, (char *)decoutput.data);

    HcfFree(encoutput.data);
    HcfFree(decoutput.data);

    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}


// Incorrect case: use OAEP pading without mgf1md
HWTEST_F(CryptoRsaCipherTest, CryptoRsaCipherTest840, TestSize.Level0)
{
    HcfResult res = HCF_SUCCESS;

    HcfAsyKeyGenerator *generator = NULL;
    res = HcfAsyKeyGeneratorCreate("RSA1024|PRIMES_2", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);
    HcfObjDestroy(generator);
    HcfObjDestroy(keyPair);

    HcfCipher *cipher = NULL;
    res = HcfCipherCreate("RSA1024|PKCS1_OAEP|SHA256", &cipher);
    EXPECT_NE(res, HCF_SUCCESS);
    EXPECT_EQ(cipher, nullptr);
}

// Incorrect case: use OAEP pading without md
HWTEST_F(CryptoRsaCipherTest, CryptoRsaCipherTest845, TestSize.Level0)
{
    HcfResult res = HCF_SUCCESS;

    HcfAsyKeyGenerator *generator = NULL;
    res = HcfAsyKeyGeneratorCreate("RSA1024|PRIMES_2", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);
    HcfObjDestroy(generator);
    HcfObjDestroy(keyPair);

    HcfCipher *cipher = NULL;
    res = HcfCipherCreate("RSA1024|PKCS1_OAEP|MGF1_SHA256", &cipher);
    EXPECT_NE(res, HCF_SUCCESS);
    EXPECT_EQ(cipher, nullptr);
}

// Correct case: test genEncoded and convert key
HWTEST_F(CryptoRsaCipherTest, CryptoRsaCipherTest850, TestSize.Level0)
{
    uint8_t plan[] = "this is rsa cipher test!\0";
    HcfAsyKeyGenerator *generator = NULL;
    HcfResult res = HcfAsyKeyGeneratorCreate("RSA1024", &generator);
    EXPECT_EQ(res, HCF_SUCCESS);
    EXPECT_NE(generator, nullptr);

    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);
    EXPECT_EQ(res, HCF_SUCCESS);
    HcfBlob pubKeyBlob = {.data = NULL, .len = 0};
    HcfBlob priKeyBlob = {.data = NULL, .len = 0};
    res = keyPair->pubKey->base.getEncoded((HcfKey *)keyPair->pubKey, &pubKeyBlob);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = keyPair->priKey->base.getEncoded((HcfKey *)keyPair->priKey, &priKeyBlob);
    EXPECT_EQ(res, HCF_SUCCESS);

    HcfKeyPair *dupKeyPair = NULL;
    res = generator->convertKey(generator, NULL, &pubKeyBlob, &priKeyBlob, &dupKeyPair);
    EXPECT_EQ(res, HCF_SUCCESS);

    HcfObjDestroy(generator);
    HcfObjDestroy(keyPair);

    HcfBlob input = {.data = (uint8_t *)plan, .len = strlen((char *)plan)};
    HcfBlob encoutput = {.data = NULL, .len = 0};
    HcfCipher *cipher = NULL;
    res = HcfCipherCreate("RSA1024|PKCS1", &cipher);
    EXPECT_EQ(res, HCF_SUCCESS);

    res = cipher->init(cipher, ENCRYPT_MODE, (HcfKey *)dupKeyPair->pubKey, NULL);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = cipher->doFinal(cipher, &input, &encoutput);
    EXPECT_EQ(res, HCF_SUCCESS);
    HcfObjDestroy(cipher);

    HcfBlob decoutput = {.data = NULL, .len = 0};
    cipher = NULL;
    res = HcfCipherCreate("RSA1024|PKCS1", &cipher);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = cipher->init(cipher, DECRYPT_MODE, (HcfKey *)dupKeyPair->priKey, NULL);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = cipher->doFinal(cipher, &encoutput, &decoutput);
    EXPECT_EQ(res, HCF_SUCCESS);
    HcfObjDestroy(cipher);
    EXPECT_STREQ((char *)plan, (char *)decoutput.data);

    HcfFree(encoutput.data);
    HcfFree(decoutput.data);
    HcfFree(pubKeyBlob.data);
    HcfFree(priKeyBlob.data);
    HcfObjDestroy(dupKeyPair);
}

// correct case: test OAEP Plaintext boundary:
// RSA_size - (md_result_len * mgf1md_result_len) - 2 = 128 - (32 + 32) - 2 = 62
HWTEST_F(CryptoRsaCipherTest, CryptoRsaCipherTest860, TestSize.Level0)
{
    HcfResult res = HCF_SUCCESS;
    uint8_t plan1[] = "00112233445566778899aabbccddeeff0aaaaaaaaaaaaaaaaaaaaaaaaaa";
    uint8_t plan2[] = "00112233445566778899aabbccddeeffaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
    uint8_t plan3[] = "00112233445566778899aabbccddeeff00112233445566778899aaaaaaaaaa";
    HcfAsyKeyGenerator *generator = NULL;
    res = HcfAsyKeyGeneratorCreate("RSA1024|PRIMES_2", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);
    EXPECT_EQ(res, HCF_SUCCESS);
    EXPECT_NE(keyPair, nullptr);
    EXPECT_NE(keyPair->priKey, nullptr);
    EXPECT_NE(keyPair->pubKey, nullptr);

    HcfCipher *cipher = NULL;
    res = HcfCipherCreate("RSA1024|PKCS1_OAEP|SHA256|MGF1_SHA256", &cipher);
    EXPECT_EQ(res, HCF_SUCCESS);

    res = cipher->init(cipher, ENCRYPT_MODE, (HcfKey *)keyPair->pubKey, NULL);
    EXPECT_EQ(res, HCF_SUCCESS);

    HcfBlob encoutput1 = {.data = NULL, .len = 0};
    HcfBlob encoutput2 = {.data = NULL, .len = 0};
    HcfBlob encoutput3 = {.data = NULL, .len = 0};
    HcfBlob input1 = {.data = (uint8_t *)plan1, .len = strlen((char *)plan1)};
    HcfBlob input2 = {.data = (uint8_t *)plan2, .len = strlen((char *)plan2)};
    HcfBlob input3 = {.data = (uint8_t *)plan3, .len = strlen((char *)plan3)};
    res = cipher->doFinal(cipher, &input1, &encoutput1);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = cipher->doFinal(cipher, &input2, &encoutput2);
    EXPECT_NE(res, HCF_SUCCESS);
    res = cipher->doFinal(cipher, &input3, &encoutput2);
    EXPECT_EQ(res, HCF_SUCCESS);


    HcfObjDestroy(cipher);
    HcfFree(encoutput1.data);
    HcfFree(encoutput2.data);
    HcfFree(encoutput3.data);
    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

// correct case: test nopadding boundary < RSA_size(rsa)
HWTEST_F(CryptoRsaCipherTest, CryptoRsaCipherTest870, TestSize.Level0)
{
    HcfResult res = HCF_SUCCESS;
    uint8_t plan1[] = "00112233445566778899aabbccddeeff00112233445566778899aabbccddeeffasdasdbccddeeff0011223344556600"
        "1122334455111111111116";
    uint8_t plan2[] = "00112233445566778899aabbccddeeff00112233445566778899aabbccddeeffasdasdbccddeeff0011223344556600"
        "112233445511111111111611111111111";
    
    HcfAsyKeyGenerator *generator = NULL;
    res = HcfAsyKeyGeneratorCreate("RSA1024|PRIMES_2", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);
    EXPECT_EQ(res, HCF_SUCCESS);
    EXPECT_NE(keyPair, nullptr);
    EXPECT_NE(keyPair->priKey, nullptr);
    EXPECT_NE(keyPair->pubKey, nullptr);

    HcfCipher *cipher = NULL;
    res = HcfCipherCreate("RSA1024|NoPadding", &cipher);
    EXPECT_EQ(res, HCF_SUCCESS);

    res = cipher->init(cipher, ENCRYPT_MODE, (HcfKey *)keyPair->pubKey, NULL);
    EXPECT_EQ(res, HCF_SUCCESS);


    HcfBlob encoutput1 = {.data = NULL, .len = 0};
    HcfBlob encoutput2 = {.data = NULL, .len = 0};

    HcfBlob input1 = {.data = (uint8_t *)plan1, .len = strlen((char *)plan1)};
    HcfBlob input2 = {.data = (uint8_t *)plan2, .len = strlen((char *)plan2)};

    res = cipher->doFinal(cipher, &input1, &encoutput1);
    EXPECT_NE(res, HCF_SUCCESS);
    res = cipher->doFinal(cipher, &input2, &encoutput2);
    EXPECT_EQ(res, HCF_SUCCESS);

    HcfObjDestroy(cipher);
    HcfFree(encoutput1.data);
    HcfFree(encoutput2.data);
    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

// correct case: test PKCS1 boundary < RSA_size(rsa) - 11
HWTEST_F(CryptoRsaCipherTest, CryptoRsaCipherTest880, TestSize.Level0)
{
    HcfResult res = HCF_SUCCESS;
    uint8_t plan1[] = "00112233445566778899aabbccddeeff00112233445566778899aabbccddeeffasdasdbccddeeff0011223344556600"
        "1122334455111111111116";
    uint8_t plan2[] = "00112233445566778899aabbccddeeff00112233445566778899aabbccddeeffasdasdbccddeeff0011223344556600"
        "11223344551111111111161";
    
    HcfAsyKeyGenerator *generator = NULL;
    res = HcfAsyKeyGeneratorCreate("RSA1024|PRIMES_2", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);
    EXPECT_EQ(res, HCF_SUCCESS);
    EXPECT_NE(keyPair, nullptr);
    EXPECT_NE(keyPair->priKey, nullptr);
    EXPECT_NE(keyPair->pubKey, nullptr);

    HcfCipher *cipher = NULL;
    res = HcfCipherCreate("RSA1024|PKCS1", &cipher);
    EXPECT_EQ(res, HCF_SUCCESS);

    res = cipher->init(cipher, ENCRYPT_MODE, (HcfKey *)keyPair->pubKey, NULL);
    EXPECT_EQ(res, HCF_SUCCESS);

    HcfBlob encoutput1 = {.data = NULL, .len = 0};
    HcfBlob encoutput2 = {.data = NULL, .len = 0};

    HcfBlob input1 = {.data = (uint8_t *)plan1, .len = strlen((char *)plan1)};
    HcfBlob input2 = {.data = (uint8_t *)plan2, .len = strlen((char *)plan2)};

    res = cipher->doFinal(cipher, &input1, &encoutput1);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = cipher->doFinal(cipher, &input2, &encoutput2);
    EXPECT_NE(res, HCF_SUCCESS);

    HcfObjDestroy(cipher);
    HcfFree(encoutput1.data);
    HcfFree(encoutput2.data);
    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoRsaCipherTest, CryptoRsaCipherTest890, TestSize.Level0)
{
    HcfResult res = HCF_SUCCESS;
    HcfAsyKeyGenerator *generator = NULL;
    res = HcfAsyKeyGeneratorCreate("RSA1024|PRIMES_2", &generator);
    EXPECT_EQ(res, HCF_SUCCESS);

    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);
    HcfCipher *cipher = NULL;
    res = HcfCipherCreate("RSA1024|PKCS1", &cipher);
    EXPECT_EQ(res, HCF_SUCCESS);

    res = cipher->init((HcfCipher *)generator, ENCRYPT_MODE, (HcfKey *)keyPair->pubKey, NULL);
    EXPECT_NE(res, HCF_SUCCESS);

    HcfObjDestroy(generator);
    HcfObjDestroy(cipher);
    HcfObjDestroy(keyPair);
}

HWTEST_F(CryptoRsaCipherTest, CryptoRsaCipherTest900, TestSize.Level0)
{
    HcfResult res = HCF_SUCCESS;
    HcfAsyKeyGenerator *generator = NULL;
    res = HcfAsyKeyGeneratorCreate("RSA1024|PRIMES_2", &generator);
    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);
    EXPECT_EQ(res, HCF_SUCCESS);
    EXPECT_NE(keyPair, nullptr);
    EXPECT_NE(keyPair->priKey, nullptr);
    EXPECT_NE(keyPair->pubKey, nullptr);

    HcfCipher *cipher = NULL;
    res = HcfCipherCreate("RSA1024|PKCS1", &cipher);
    EXPECT_EQ(res, HCF_SUCCESS);

    uint8_t plan[] = "12312123123";
    HcfBlob input = {.data = (uint8_t *)plan, .len = strlen((char *)plan)};

    res = cipher->init(cipher, ENCRYPT_MODE, (HcfKey *)keyPair->pubKey, NULL);
    EXPECT_EQ(res, HCF_SUCCESS);

    res = cipher->doFinal(cipher, &input, NULL);
    EXPECT_NE(res, HCF_SUCCESS);

    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
    HcfObjDestroy(cipher);
}

HWTEST_F(CryptoRsaCipherTest, CryptoRsaCipherTest910, TestSize.Level0)
{
    HcfResult res = HCF_SUCCESS;
    HcfAsyKeyGenerator *generator = NULL;
    res = HcfAsyKeyGeneratorCreate("RSA1024|PRIMES_2", &generator);
    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);
    EXPECT_EQ(res, HCF_SUCCESS);
    EXPECT_NE(keyPair, nullptr);
    EXPECT_NE(keyPair->priKey, nullptr);
    EXPECT_NE(keyPair->pubKey, nullptr);

    HcfCipher *cipher = NULL;
    res = HcfCipherCreate("RSA1024|PKCS1", &cipher);
    EXPECT_EQ(res, HCF_SUCCESS);

    uint8_t plan[] = "12312123123";
    HcfBlob input = {.data = (uint8_t *)plan, .len = strlen((char *)plan)};

    res = cipher->init(cipher, ENCRYPT_MODE, (HcfKey *)keyPair->pubKey, NULL);
    EXPECT_EQ(res, HCF_SUCCESS);

    HcfBlob blob;
    res = cipher->doFinal((HcfCipher *)generator, &input, &blob);
    EXPECT_NE(res, HCF_SUCCESS);

    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
    HcfObjDestroy(cipher);
}

HWTEST_F(CryptoRsaCipherTest, CryptoRsaCipherTest920, TestSize.Level0)
{
    HcfResult res = HCF_SUCCESS;
    HcfCipher *cipher = NULL;
    res = HcfCipherCreate("RSA10|PK1", &cipher);
    EXPECT_NE(res, HCF_SUCCESS);
}

HWTEST_F(CryptoRsaCipherTest, CryptoRsaCipherTest930, TestSize.Level0)
{
    HcfResult res = HCF_SUCCESS;
    HcfCipher *cipher = NULL;
    res = HcfCipherCreate("RSA1024|PKCS1|RSA1024|PKCS1|RSA1024|PKCS1|RSA1024|PKCS1|RSA1024|PKCS1|"
        "RSA1024|PKCS1|RSA1024|PKCS1|RSA1024|PKCS1|RSA1024|PKCS1|RSA1024|PKCS1", &cipher);
    EXPECT_NE(res, HCF_SUCCESS);
}

// incorrect : init Cipher twice
HWTEST_F(CryptoRsaCipherTest, CryptoRsaCipherTest940, TestSize.Level0)
{
    HcfResult res = HCF_SUCCESS;
    HcfAsyKeyGenerator *generator = NULL;
    res = HcfAsyKeyGeneratorCreate("RSA1024|PRIMES_2", &generator);
    EXPECT_EQ(res, HCF_SUCCESS);
    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);
    EXPECT_EQ(res, HCF_SUCCESS);

    HcfCipher *cipher = NULL;
    res = HcfCipherCreate("RSA1024|PKCS1", &cipher);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = cipher->init(cipher, DECRYPT_MODE, (HcfKey *)keyPair->priKey, NULL);
    EXPECT_EQ(res, HCF_SUCCESS);
    res = cipher->init(cipher, DECRYPT_MODE, (HcfKey *)keyPair->priKey, NULL);
    EXPECT_NE(res, HCF_SUCCESS);

    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
    HcfObjDestroy(cipher);
}
}