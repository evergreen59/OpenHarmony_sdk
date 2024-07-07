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
#include "blob.h"

using namespace std;
using namespace testing::ext;

namespace {
class CryptoEccAsyKeyGeneratorTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void CryptoEccAsyKeyGeneratorTest::SetUpTestCase() {}
void CryptoEccAsyKeyGeneratorTest::TearDownTestCase() {}
void CryptoEccAsyKeyGeneratorTest::SetUp() {}
void CryptoEccAsyKeyGeneratorTest::TearDown() {}

const int ECC224_PUB_KEY_LEN = 80;
const int ECC224_PRI_KEY_LEN = 44;
const int ECC256_PUB_KEY_LEN = 91;
const int ECC256_PRI_KEY_LEN = 51;
const int ECC384_PUB_KEY_LEN = 120;
const int ECC384_PRI_KEY_LEN = 64;
const int ECC521_PUB_KEY_LEN = 158;
const int ECC521_PRI_KEY_LEN = 82;

static uint8_t mockEcc224PubKeyBlobData[ECC224_PUB_KEY_LEN] = { 48, 78, 48, 16, 6, 7, 42, 134, 72, 206, 61, 2, 1,
    6, 5, 43, 129, 4, 0, 33, 3, 58, 0, 4, 252, 171, 11, 115, 79, 252, 109, 120, 46, 97, 131, 145, 207, 141, 146,
    235, 133, 37, 218, 180, 8, 149, 47, 244, 137, 238, 207, 95, 153, 65, 250, 32, 77, 184, 249, 181, 172, 192, 2,
    99, 194, 170, 25, 44, 255, 87, 246, 42, 133, 83, 66, 197, 97, 95, 12, 84 };

static uint8_t mockEcc224PriKeyBlobData[ECC224_PRI_KEY_LEN] = { 48, 42, 2, 1, 1, 4, 28, 250, 86, 6, 147, 222, 43,
    252, 139, 90, 139, 5, 33, 184, 230, 26, 68, 94, 57, 145, 229, 146, 49, 221, 119, 206, 32, 198, 19, 160, 7, 6,
    5, 43, 129, 4, 0, 33 };

static uint8_t mockEcc256PubKeyBlobData[ECC256_PUB_KEY_LEN] = { 48, 89, 48, 19, 6, 7, 42, 134, 72, 206, 61, 2, 1,
    6, 8, 42, 134, 72, 206, 61, 3, 1, 7, 3, 66, 0, 4, 65, 43, 59, 227, 32, 51, 44, 104, 47, 135, 97, 144, 91, 70,
    231, 67, 2, 214, 197, 176, 161, 160, 227, 133, 158, 30, 118, 217, 243, 155, 88, 55, 214, 86, 86, 122, 166, 64,
    111, 2, 226, 93, 163, 194, 210, 74, 18, 63, 173, 113, 249, 196, 126, 165, 222, 230, 190, 101, 241, 95, 102, 174,
    252, 38 };

static uint8_t mockEcc256PriKeyBlobData[ECC256_PRI_KEY_LEN] = { 48, 49, 2, 1, 1, 4, 32, 223, 134, 255, 219, 45,
    68, 72, 231, 43, 72, 243, 113, 255, 60, 232, 203, 151, 65, 80, 6, 36, 112, 247, 186, 106, 148, 43, 170, 204,
    23, 189, 191, 160, 10, 6, 8, 42, 134, 72, 206, 61, 3, 1, 7 };

static uint8_t mockEcc384PubKeyBlobData[ECC384_PUB_KEY_LEN] = { 48, 118, 48, 16, 6, 7, 42, 134, 72, 206, 61, 2,
    1, 6, 5, 43, 129, 4, 0, 34, 3, 98, 0, 4, 149, 237, 84, 7, 62, 114, 48, 106, 64, 227, 243, 253, 90, 170, 184,
    80, 174, 159, 72, 117, 16, 24, 213, 146, 184, 159, 33, 209, 36, 143, 227, 11, 113, 228, 128, 13, 181, 195, 235,
    12, 255, 85, 187, 197, 109, 82, 242, 226, 186, 53, 128, 9, 133, 4, 170, 96, 150, 94, 197, 196, 107, 120, 55,
    159, 181, 208, 249, 176, 108, 6, 166, 253, 221, 156, 139, 140, 124, 240, 11, 194, 154, 139, 62, 218, 170, 61,
    12, 147, 7, 146, 243, 158, 103, 195, 123, 156 };

static uint8_t mockEcc384PriKeyBlobData[ECC384_PRI_KEY_LEN] = { 48, 62, 2, 1, 1, 4, 48, 137, 184, 12, 183, 201,
    211, 124, 203, 165, 9, 229, 68, 46, 17, 14, 14, 109, 195, 0, 206, 248, 21, 53, 72, 66, 3, 244, 165, 248,
    217, 176, 121, 155, 225, 222, 134, 155, 241, 59, 16, 253, 237, 158, 11, 221, 252, 58, 251, 160, 7, 6, 5,
    43, 129, 4, 0, 34 };

static uint8_t mockEcc521PubKeyBlobData[ECC521_PUB_KEY_LEN] = { 48, 129, 155, 48, 16, 6, 7, 42, 134, 72, 206,
    61, 2, 1, 6, 5, 43, 129, 4, 0, 35, 3, 129, 134, 0, 4, 0, 149, 60, 46, 252, 220, 227, 253, 219, 250, 60, 232,
    80, 190, 119, 38, 79, 202, 173, 35, 126, 228, 244, 207, 174, 191, 250, 147, 188, 22, 132, 125, 44, 26, 57, 242,
    203, 192, 100, 65, 185, 250, 80, 246, 76, 37, 242, 78, 64, 152, 47, 172, 165, 229, 99, 247, 61, 91, 152, 144,
    67, 34, 38, 130, 198, 22, 0, 179, 69, 69, 147, 72, 154, 76, 134, 28, 158, 33, 251, 94, 63, 77, 44, 100, 132,
    207, 159, 210, 113, 194, 174, 170, 194, 129, 215, 209, 50, 217, 204, 48, 53, 92, 231, 57, 179, 170, 6, 26, 77,
    187, 181, 35, 254, 17, 216, 205, 118, 104, 89, 155, 145, 28, 61, 169, 113, 195, 55, 13, 125, 6, 168, 156 };

static uint8_t mockEcc521PriKeyBlobData[ECC521_PRI_KEY_LEN] = { 48, 80, 2, 1, 1, 4, 66, 0, 210, 135, 140, 70,
    98, 28, 121, 169, 5, 202, 132, 165, 11, 56, 9, 110, 32, 9, 146, 185, 239, 69, 113, 79, 213, 24, 165, 194, 147,
    209, 223, 187, 100, 6, 149, 4, 56, 235, 120, 152, 146, 252, 92, 21, 222, 3, 182, 68, 39, 222, 49, 192, 154, 126,
    126, 243, 18, 99, 136, 199, 234, 134, 232, 13, 128, 160, 7, 6, 5, 43, 129, 4, 0, 35 };

static HcfBlob mockEcc224PubKeyBlob = {
    .data = mockEcc224PubKeyBlobData,
    .len = ECC224_PUB_KEY_LEN
};

static HcfBlob mockEcc224PriKeyBlob = {
    .data = mockEcc224PriKeyBlobData,
    .len = ECC224_PRI_KEY_LEN
};

static HcfBlob mockEcc256PubKeyBlob = {
    .data = mockEcc256PubKeyBlobData,
    .len = ECC256_PUB_KEY_LEN
};

static HcfBlob mockEcc256PriKeyBlob = {
    .data = mockEcc256PriKeyBlobData,
    .len = ECC256_PRI_KEY_LEN
};

static HcfBlob mockEcc384PubKeyBlob = {
    .data = mockEcc384PubKeyBlobData,
    .len = ECC384_PUB_KEY_LEN
};

static HcfBlob mockEcc384PriKeyBlob = {
    .data = mockEcc384PriKeyBlobData,
    .len = ECC384_PRI_KEY_LEN
};

static HcfBlob mockEcc521PubKeyBlob = {
    .data = mockEcc521PubKeyBlobData,
    .len = ECC521_PUB_KEY_LEN
};

static HcfBlob mockEcc521PriKeyBlob = {
    .data = mockEcc521PriKeyBlobData,
    .len = ECC521_PRI_KEY_LEN
};

static const char *GetMockClass(void)
{
    return "HcfSymKeyGenerator";
}

static HcfObjectBase obj = {
    .getClass = GetMockClass,
    .destroy = NULL
};

/**
 * @tc.name: CryptoEccAsyKeyGeneratorTest.CryptoEccAsyKeyGeneratorTest001
 * @tc.desc: Verify that the creation of the ECC224 key pair generator is normal.
 * @tc.type: FUNC
 * @tc.require: I5QWEI
 */
HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest001, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(generator, nullptr);

    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest002, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC256", &generator);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(generator, nullptr);

    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest003, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC384", &generator);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(generator, nullptr);

    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest004, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC521", &generator);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(generator, nullptr);

    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest005, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate(nullptr, &generator);

    ASSERT_EQ(res, HCF_INVALID_PARAMS);
    ASSERT_EQ(generator, nullptr);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest006, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCD"
        "ABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCDABCD", &generator);

    ASSERT_EQ(res, HCF_INVALID_PARAMS);
    ASSERT_EQ(generator, nullptr);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest007, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC225", &generator);

    ASSERT_EQ(res, HCF_INVALID_PARAMS);
    ASSERT_EQ(generator, nullptr);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest008, TestSize.Level0)
{
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", NULL);

    ASSERT_EQ(res, HCF_INVALID_PARAMS);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest101, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(generator, nullptr);

    const char *className = generator->base.getClass();
    ASSERT_NE(className, nullptr);

    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest102, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(generator, nullptr);

    generator->base.destroy((HcfObjectBase *)generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest103, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(generator, nullptr);

    generator->base.destroy(NULL);

    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest104, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(generator, nullptr);

    generator->base.destroy(&obj);

    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest105, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(generator, nullptr);

    const char *algName = generator->getAlgoName(generator);

    ASSERT_NE(algName, nullptr);

    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest106, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(generator, nullptr);

    const char *algName = generator->getAlgoName(NULL);

    ASSERT_EQ(algName, nullptr);

    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest107, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(generator, nullptr);

    const char *algName = generator->getAlgoName((HcfAsyKeyGenerator *)&obj);

    ASSERT_EQ(algName, nullptr);

    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest201, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(generator, nullptr);

    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyPair, nullptr);

    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest202, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC256", &generator);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(generator, nullptr);

    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyPair, nullptr);

    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest203, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC384", &generator);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(generator, nullptr);

    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyPair, nullptr);

    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest204, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC521", &generator);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(generator, nullptr);

    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyPair, nullptr);

    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest205, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(generator, nullptr);

    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(NULL, NULL, &keyPair);

    ASSERT_EQ(res, HCF_INVALID_PARAMS);
    ASSERT_EQ(keyPair, nullptr);

    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest206, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(generator, nullptr);

    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair((HcfAsyKeyGenerator *)&obj, NULL, &keyPair);

    ASSERT_EQ(res, HCF_INVALID_PARAMS);
    ASSERT_EQ(keyPair, nullptr);

    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest207, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(generator, nullptr);

    res = generator->generateKeyPair(generator, NULL, NULL);

    ASSERT_EQ(res, HCF_INVALID_PARAMS);

    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest301, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyPair, nullptr);

    const char *className = keyPair->base.getClass();

    ASSERT_NE(className, nullptr);

    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest302, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyPair, nullptr);

    keyPair->base.destroy((HcfObjectBase *)(&(keyPair->base)));

    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest303, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyPair, nullptr);

    keyPair->base.destroy(NULL);

    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest304, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyPair, nullptr);

    keyPair->base.destroy(&obj);

    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest305, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyPair, nullptr);

    const char *className = keyPair->pubKey->base.base.getClass();
    ASSERT_NE(className, nullptr);

    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest306, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyPair, nullptr);

    keyPair->pubKey->base.base.destroy((HcfObjectBase *)(&(keyPair->pubKey->base.base)));
    keyPair->pubKey = NULL;

    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest307, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyPair, nullptr);

    keyPair->pubKey->base.base.destroy(NULL);

    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest308, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyPair, nullptr);

    keyPair->pubKey->base.base.destroy(&obj);

    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest309, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyPair, nullptr);

    const char *format = keyPair->pubKey->base.getFormat(&(keyPair->pubKey->base));

    ASSERT_NE(format, nullptr);

    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest310, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyPair, nullptr);

    const char *format = keyPair->pubKey->base.getFormat(NULL);

    ASSERT_EQ(format, nullptr);

    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest311, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyPair, nullptr);

    const char *format = keyPair->pubKey->base.getFormat((HcfKey *)&obj);

    ASSERT_EQ(format, nullptr);

    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest312, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyPair, nullptr);

    const char *algName = keyPair->pubKey->base.getAlgorithm(&(keyPair->pubKey->base));

    ASSERT_NE(algName, nullptr);

    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest313, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyPair, nullptr);

    const char *algName = keyPair->pubKey->base.getAlgorithm(NULL);

    ASSERT_EQ(algName, nullptr);

    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest314, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyPair, nullptr);

    const char *algName = keyPair->pubKey->base.getAlgorithm((HcfKey *)&obj);

    ASSERT_EQ(algName, nullptr);

    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest315, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyPair, nullptr);

    HcfBlob blob = {
        .data = NULL,
        .len = 0
    };
    res = keyPair->pubKey->base.getEncoded(&(keyPair->pubKey->base), &blob);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(blob.data, nullptr);
    ASSERT_NE(blob.len, 0);

    free(blob.data);

    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest316, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyPair, nullptr);

    HcfBlob blob = {
        .data = NULL,
        .len = 0
    };
    res = keyPair->pubKey->base.getEncoded(NULL, &blob);

    ASSERT_NE(res, HCF_SUCCESS);
    ASSERT_EQ(blob.data, nullptr);
    ASSERT_EQ(blob.len, 0);

    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest317, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyPair, nullptr);

    HcfBlob blob = {
        .data = NULL,
        .len = 0
    };
    res = keyPair->pubKey->base.getEncoded((HcfKey *)&obj, &blob);

    ASSERT_NE(res, HCF_SUCCESS);
    ASSERT_EQ(blob.data, nullptr);
    ASSERT_EQ(blob.len, 0);

    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest318, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyPair, nullptr);

    res = keyPair->pubKey->base.getEncoded(&(keyPair->pubKey->base), NULL);

    ASSERT_NE(res, HCF_SUCCESS);

    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest319, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyPair, nullptr);

    keyPair->priKey->clearMem(keyPair->priKey);

    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest320, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyPair, nullptr);

    keyPair->priKey->clearMem(NULL);

    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest321, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyPair, nullptr);

    keyPair->priKey->clearMem((HcfPriKey *)&obj);

    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest322, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyPair, nullptr);

    const char *algName = keyPair->priKey->base.base.getClass();

    ASSERT_NE(algName, nullptr);

    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest323, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyPair, nullptr);

    keyPair->priKey->base.base.destroy((HcfObjectBase *)(&(keyPair->priKey->base.base)));
    keyPair->priKey = NULL;

    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest324, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyPair, nullptr);

    keyPair->priKey->base.base.destroy(NULL);

    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest325, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyPair, nullptr);

    keyPair->priKey->base.base.destroy(&obj);

    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest326, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyPair, nullptr);

    const char *format = keyPair->priKey->base.getFormat(&keyPair->priKey->base);

    ASSERT_NE(format, nullptr);

    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest327, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyPair, nullptr);

    const char *format = keyPair->priKey->base.getFormat(NULL);

    ASSERT_EQ(format, nullptr);

    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest328, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyPair, nullptr);

    const char *format = keyPair->priKey->base.getFormat((HcfKey *)&obj);

    ASSERT_EQ(format, nullptr);

    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest329, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyPair, nullptr);

    const char *algName = keyPair->priKey->base.getAlgorithm(&keyPair->priKey->base);

    ASSERT_NE(algName, nullptr);

    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest330, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyPair, nullptr);

    const char *algName = keyPair->priKey->base.getAlgorithm(NULL);

    ASSERT_EQ(algName, nullptr);

    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest331, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyPair, nullptr);

    const char *algName = keyPair->priKey->base.getAlgorithm((HcfKey *)&obj);

    ASSERT_EQ(algName, nullptr);

    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest332, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyPair, nullptr);

    HcfBlob blob = {
        .data = NULL,
        .len = 0
    };
    res = keyPair->priKey->base.getEncoded(&(keyPair->priKey->base), &blob);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(blob.data, nullptr);
    ASSERT_NE(blob.len, 0);

    free(blob.data);

    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest333, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyPair, nullptr);

    HcfBlob blob = {
        .data = NULL,
        .len = 0
    };
    res = keyPair->priKey->base.getEncoded(NULL, &blob);

    ASSERT_NE(res, HCF_SUCCESS);
    ASSERT_EQ(blob.data, nullptr);
    ASSERT_EQ(blob.len, 0);

    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest334, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyPair, nullptr);

    HcfBlob blob = {
        .data = NULL,
        .len = 0
    };
    res = keyPair->priKey->base.getEncoded((HcfKey *)&obj, &blob);

    ASSERT_NE(res, HCF_SUCCESS);
    ASSERT_EQ(blob.data, nullptr);
    ASSERT_EQ(blob.len, 0);

    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest335, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyPair, nullptr);

    res = keyPair->priKey->base.getEncoded(&(keyPair->priKey->base), NULL);

    ASSERT_NE(res, HCF_SUCCESS);

    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest401, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);

    HcfKeyPair *outKeyPair = NULL;
    res = generator->convertKey(generator, NULL, &mockEcc224PubKeyBlob, &mockEcc224PriKeyBlob, &outKeyPair);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(outKeyPair, nullptr);

    HcfObjDestroy(outKeyPair);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest402, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC256", &generator);

    HcfKeyPair *outKeyPair = NULL;
    res = generator->convertKey(generator, NULL, &mockEcc256PubKeyBlob, &mockEcc256PriKeyBlob, &outKeyPair);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(outKeyPair, nullptr);

    HcfObjDestroy(outKeyPair);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest403, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC384", &generator);

    HcfKeyPair *outKeyPair = NULL;
    res = generator->convertKey(generator, NULL, &mockEcc384PubKeyBlob, &mockEcc384PriKeyBlob, &outKeyPair);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(outKeyPair, nullptr);

    HcfObjDestroy(outKeyPair);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest404, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC521", &generator);

    HcfKeyPair *outKeyPair = NULL;
    res = generator->convertKey(generator, NULL, &mockEcc521PubKeyBlob, &mockEcc521PriKeyBlob, &outKeyPair);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(outKeyPair, nullptr);

    HcfObjDestroy(outKeyPair);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest405, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);

    HcfKeyPair *outKeyPair = NULL;
    res = generator->convertKey(NULL, NULL, &mockEcc224PubKeyBlob, &mockEcc224PriKeyBlob, &outKeyPair);

    ASSERT_NE(res, HCF_SUCCESS);
    ASSERT_EQ(outKeyPair, nullptr);

    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest406, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);

    HcfKeyPair *outKeyPair = NULL;
    res = generator->convertKey((HcfAsyKeyGenerator *)&obj, NULL, &mockEcc224PubKeyBlob, &mockEcc224PriKeyBlob,
        &outKeyPair);

    ASSERT_NE(res, HCF_SUCCESS);
    ASSERT_EQ(outKeyPair, nullptr);

    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest407, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);

    HcfKeyPair *outKeyPair = NULL;
    res = generator->convertKey(generator, NULL, NULL, &mockEcc224PriKeyBlob, &outKeyPair);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(outKeyPair, nullptr);

    HcfObjDestroy(outKeyPair);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest408, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);

    HcfKeyPair *outKeyPair = NULL;
    res = generator->convertKey(generator, NULL, &mockEcc224PubKeyBlob, NULL, &outKeyPair);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(outKeyPair, nullptr);

    HcfObjDestroy(outKeyPair);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest409, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);

    HcfKeyPair *outKeyPair = NULL;
    res = generator->convertKey(generator, NULL, NULL, NULL, &outKeyPair);

    ASSERT_NE(res, HCF_SUCCESS);
    ASSERT_EQ(outKeyPair, nullptr);

    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest410, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);

    res = generator->convertKey(generator, NULL, &mockEcc224PubKeyBlob, &mockEcc224PriKeyBlob, NULL);

    ASSERT_NE(res, HCF_SUCCESS);

    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest501, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->convertKey(generator, NULL, &mockEcc224PubKeyBlob, &mockEcc224PriKeyBlob, &keyPair);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyPair, nullptr);

    const char *className = keyPair->base.getClass();

    ASSERT_NE(className, nullptr);

    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest502, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->convertKey(generator, NULL, &mockEcc224PubKeyBlob, &mockEcc224PriKeyBlob, &keyPair);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyPair, nullptr);

    keyPair->base.destroy((HcfObjectBase *)(&(keyPair->base)));

    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest503, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->convertKey(generator, NULL, &mockEcc224PubKeyBlob, &mockEcc224PriKeyBlob, &keyPair);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyPair, nullptr);

    keyPair->base.destroy(NULL);

    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest504, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->convertKey(generator, NULL, &mockEcc224PubKeyBlob, &mockEcc224PriKeyBlob, &keyPair);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyPair, nullptr);

    keyPair->base.destroy(&obj);

    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest505, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->convertKey(generator, NULL, &mockEcc224PubKeyBlob, &mockEcc224PriKeyBlob, &keyPair);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyPair, nullptr);

    const char *className = keyPair->pubKey->base.base.getClass();
    ASSERT_NE(className, nullptr);

    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest506, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->convertKey(generator, NULL, &mockEcc224PubKeyBlob, &mockEcc224PriKeyBlob, &keyPair);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyPair, nullptr);

    keyPair->pubKey->base.base.destroy((HcfObjectBase *)(&(keyPair->pubKey->base.base)));
    keyPair->pubKey = NULL;

    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest507, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->convertKey(generator, NULL, &mockEcc224PubKeyBlob, &mockEcc224PriKeyBlob, &keyPair);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyPair, nullptr);

    keyPair->pubKey->base.base.destroy(NULL);

    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest508, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->convertKey(generator, NULL, &mockEcc224PubKeyBlob, &mockEcc224PriKeyBlob, &keyPair);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyPair, nullptr);

    keyPair->pubKey->base.base.destroy(&obj);

    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest509, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->convertKey(generator, NULL, &mockEcc224PubKeyBlob, &mockEcc224PriKeyBlob, &keyPair);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyPair, nullptr);

    const char *format = keyPair->pubKey->base.getFormat(&(keyPair->pubKey->base));

    ASSERT_NE(format, nullptr);

    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest510, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->convertKey(generator, NULL, &mockEcc224PubKeyBlob, &mockEcc224PriKeyBlob, &keyPair);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyPair, nullptr);

    const char *format = keyPair->pubKey->base.getFormat(NULL);

    ASSERT_EQ(format, nullptr);

    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest511, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->convertKey(generator, NULL, &mockEcc224PubKeyBlob, &mockEcc224PriKeyBlob, &keyPair);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyPair, nullptr);

    const char *format = keyPair->pubKey->base.getFormat((HcfKey *)&obj);

    ASSERT_EQ(format, nullptr);

    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest512, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->convertKey(generator, NULL, &mockEcc224PubKeyBlob, &mockEcc224PriKeyBlob, &keyPair);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyPair, nullptr);

    const char *algName = keyPair->pubKey->base.getAlgorithm(&(keyPair->pubKey->base));

    ASSERT_NE(algName, nullptr);

    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest513, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->convertKey(generator, NULL, &mockEcc224PubKeyBlob, &mockEcc224PriKeyBlob, &keyPair);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyPair, nullptr);

    const char *algName = keyPair->pubKey->base.getAlgorithm(NULL);

    ASSERT_EQ(algName, nullptr);

    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest514, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->convertKey(generator, NULL, &mockEcc224PubKeyBlob, &mockEcc224PriKeyBlob, &keyPair);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyPair, nullptr);

    const char *algName = keyPair->pubKey->base.getAlgorithm((HcfKey *)&obj);

    ASSERT_EQ(algName, nullptr);

    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest515, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->convertKey(generator, NULL, &mockEcc224PubKeyBlob, &mockEcc224PriKeyBlob, &keyPair);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyPair, nullptr);

    HcfBlob blob = {
        .data = NULL,
        .len = 0
    };
    res = keyPair->pubKey->base.getEncoded(&(keyPair->pubKey->base), &blob);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(blob.data, nullptr);
    ASSERT_NE(blob.len, 0);

    free(blob.data);

    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest516, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->convertKey(generator, NULL, &mockEcc224PubKeyBlob, &mockEcc224PriKeyBlob, &keyPair);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyPair, nullptr);

    HcfBlob blob = {
        .data = NULL,
        .len = 0
    };
    res = keyPair->pubKey->base.getEncoded(NULL, &blob);

    ASSERT_NE(res, HCF_SUCCESS);
    ASSERT_EQ(blob.data, nullptr);
    ASSERT_EQ(blob.len, 0);

    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest517, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->convertKey(generator, NULL, &mockEcc224PubKeyBlob, &mockEcc224PriKeyBlob, &keyPair);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyPair, nullptr);

    HcfBlob blob = {
        .data = NULL,
        .len = 0
    };
    res = keyPair->pubKey->base.getEncoded((HcfKey *)&obj, &blob);

    ASSERT_NE(res, HCF_SUCCESS);
    ASSERT_EQ(blob.data, nullptr);
    ASSERT_EQ(blob.len, 0);

    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest518, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->convertKey(generator, NULL, &mockEcc224PubKeyBlob, &mockEcc224PriKeyBlob, &keyPair);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyPair, nullptr);

    res = keyPair->pubKey->base.getEncoded(&(keyPair->pubKey->base), NULL);

    ASSERT_NE(res, HCF_SUCCESS);

    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest519, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->convertKey(generator, NULL, &mockEcc224PubKeyBlob, &mockEcc224PriKeyBlob, &keyPair);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyPair, nullptr);

    keyPair->priKey->clearMem(keyPair->priKey);

    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest520, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->convertKey(generator, NULL, &mockEcc224PubKeyBlob, &mockEcc224PriKeyBlob, &keyPair);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyPair, nullptr);

    keyPair->priKey->clearMem(NULL);

    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest521, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->convertKey(generator, NULL, &mockEcc224PubKeyBlob, &mockEcc224PriKeyBlob, &keyPair);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyPair, nullptr);

    keyPair->priKey->clearMem((HcfPriKey *)&obj);

    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest522, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->convertKey(generator, NULL, &mockEcc224PubKeyBlob, &mockEcc224PriKeyBlob, &keyPair);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyPair, nullptr);

    const char *algName = keyPair->priKey->base.base.getClass();

    ASSERT_NE(algName, nullptr);

    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest523, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->convertKey(generator, NULL, &mockEcc224PubKeyBlob, &mockEcc224PriKeyBlob, &keyPair);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyPair, nullptr);

    keyPair->priKey->base.base.destroy((HcfObjectBase *)(&(keyPair->priKey->base.base)));
    keyPair->priKey = NULL;

    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest524, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->convertKey(generator, NULL, &mockEcc224PubKeyBlob, &mockEcc224PriKeyBlob, &keyPair);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyPair, nullptr);

    keyPair->priKey->base.base.destroy(NULL);

    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest525, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->convertKey(generator, NULL, &mockEcc224PubKeyBlob, &mockEcc224PriKeyBlob, &keyPair);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyPair, nullptr);

    keyPair->priKey->base.base.destroy(&obj);

    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest526, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->convertKey(generator, NULL, &mockEcc224PubKeyBlob, &mockEcc224PriKeyBlob, &keyPair);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyPair, nullptr);

    const char *format = keyPair->priKey->base.getFormat(&keyPair->priKey->base);

    ASSERT_NE(format, nullptr);

    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest527, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->convertKey(generator, NULL, &mockEcc224PubKeyBlob, &mockEcc224PriKeyBlob, &keyPair);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyPair, nullptr);

    const char *format = keyPair->priKey->base.getFormat(NULL);

    ASSERT_EQ(format, nullptr);

    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest528, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->convertKey(generator, NULL, &mockEcc224PubKeyBlob, &mockEcc224PriKeyBlob, &keyPair);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyPair, nullptr);

    const char *format = keyPair->priKey->base.getFormat((HcfKey *)&obj);

    ASSERT_EQ(format, nullptr);

    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest529, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->convertKey(generator, NULL, &mockEcc224PubKeyBlob, &mockEcc224PriKeyBlob, &keyPair);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyPair, nullptr);

    const char *algName = keyPair->priKey->base.getAlgorithm(&keyPair->priKey->base);

    ASSERT_NE(algName, nullptr);

    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest530, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->convertKey(generator, NULL, &mockEcc224PubKeyBlob, &mockEcc224PriKeyBlob, &keyPair);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyPair, nullptr);

    const char *algName = keyPair->priKey->base.getAlgorithm(NULL);

    ASSERT_EQ(algName, nullptr);

    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest531, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->convertKey(generator, NULL, &mockEcc224PubKeyBlob, &mockEcc224PriKeyBlob, &keyPair);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyPair, nullptr);

    const char *algName = keyPair->priKey->base.getAlgorithm((HcfKey *)&obj);

    ASSERT_EQ(algName, nullptr);

    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest532, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->convertKey(generator, NULL, &mockEcc224PubKeyBlob, &mockEcc224PriKeyBlob, &keyPair);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyPair, nullptr);

    HcfBlob blob = {
        .data = NULL,
        .len = 0
    };
    res = keyPair->priKey->base.getEncoded(&(keyPair->priKey->base), &blob);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(blob.data, nullptr);
    ASSERT_NE(blob.len, 0);

    free(blob.data);

    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest533, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->convertKey(generator, NULL, &mockEcc224PubKeyBlob, &mockEcc224PriKeyBlob, &keyPair);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyPair, nullptr);

    HcfBlob blob = {
        .data = NULL,
        .len = 0
    };
    res = keyPair->priKey->base.getEncoded(NULL, &blob);

    ASSERT_NE(res, HCF_SUCCESS);
    ASSERT_EQ(blob.data, nullptr);
    ASSERT_EQ(blob.len, 0);

    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest534, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->convertKey(generator, NULL, &mockEcc224PubKeyBlob, &mockEcc224PriKeyBlob, &keyPair);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyPair, nullptr);

    HcfBlob blob = {
        .data = NULL,
        .len = 0
    };
    res = keyPair->priKey->base.getEncoded((HcfKey *)&obj, &blob);

    ASSERT_NE(res, HCF_SUCCESS);
    ASSERT_EQ(blob.data, nullptr);
    ASSERT_EQ(blob.len, 0);

    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest535, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->convertKey(generator, NULL, &mockEcc224PubKeyBlob, &mockEcc224PriKeyBlob, &keyPair);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyPair, nullptr);

    res = keyPair->priKey->base.getEncoded(&(keyPair->priKey->base), NULL);

    ASSERT_NE(res, HCF_SUCCESS);

    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest536, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyPair, nullptr);

    HcfBlob pubKeyBlob = {
        .data = NULL,
        .len = 0
    };
    res = keyPair->pubKey->base.getEncoded(&(keyPair->pubKey->base), &pubKeyBlob);

    ASSERT_EQ(res, HCF_SUCCESS);

    HcfBlob priKeyBlob = {
        .data = NULL,
        .len = 0
    };
    res = keyPair->priKey->base.getEncoded(&(keyPair->priKey->base), &priKeyBlob);

    ASSERT_EQ(res, HCF_SUCCESS);

    HcfKeyPair *outKeyPair = NULL;
    res = generator->convertKey(generator, NULL, &pubKeyBlob, &priKeyBlob, &outKeyPair);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(outKeyPair, nullptr);

    HcfBlob outPubKeyBlob = {
        .data = NULL,
        .len = 0
    };
    res = outKeyPair->pubKey->base.getEncoded(&(outKeyPair->pubKey->base), &outPubKeyBlob);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(outPubKeyBlob.data, nullptr);
    ASSERT_NE(outPubKeyBlob.len, 0);

    HcfBlob outPriKeyBlob = {
        .data = NULL,
        .len = 0
    };
    res = outKeyPair->priKey->base.getEncoded(&(outKeyPair->priKey->base), &outPriKeyBlob);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(outPriKeyBlob.data, nullptr);
    ASSERT_NE(outPriKeyBlob.len, 0);

    free(pubKeyBlob.data);
    free(priKeyBlob.data);
    free(outPubKeyBlob.data);
    free(outPriKeyBlob.data);
    HcfObjDestroy(outKeyPair);
    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest537, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyPair, nullptr);

    HcfBlob pubKeyBlob = {
        .data = NULL,
        .len = 0
    };
    res = keyPair->pubKey->base.getEncoded(&(keyPair->pubKey->base), &pubKeyBlob);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(pubKeyBlob.data, nullptr);
    ASSERT_NE(pubKeyBlob.len, 0);

    HcfKeyPair *outKeyPair = NULL;
    res = generator->convertKey(generator, NULL, &pubKeyBlob, NULL, &outKeyPair);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(outKeyPair, nullptr);
    ASSERT_NE(outKeyPair->pubKey, nullptr);
    ASSERT_EQ(outKeyPair->priKey, nullptr);

    HcfBlob outPubKeyBlob = {
        .data = NULL,
        .len = 0
    };
    res = outKeyPair->pubKey->base.getEncoded(&(outKeyPair->pubKey->base), &outPubKeyBlob);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(outPubKeyBlob.data, nullptr);
    ASSERT_NE(outPubKeyBlob.len, 0);

    free(pubKeyBlob.data);
    free(outPubKeyBlob.data);
    HcfObjDestroy(outKeyPair);
    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}

HWTEST_F(CryptoEccAsyKeyGeneratorTest, CryptoEccAsyKeyGeneratorTest538, TestSize.Level0)
{
    HcfAsyKeyGenerator *generator = NULL;
    int32_t res = HcfAsyKeyGeneratorCreate("ECC224", &generator);

    HcfKeyPair *keyPair = NULL;
    res = generator->generateKeyPair(generator, NULL, &keyPair);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(keyPair, nullptr);

    HcfBlob priKeyBlob = {
        .data = NULL,
        .len = 0
    };
    res = keyPair->priKey->base.getEncoded(&(keyPair->priKey->base), &priKeyBlob);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(priKeyBlob.data, nullptr);
    ASSERT_NE(priKeyBlob.len, 0);

    HcfKeyPair *outKeyPair = NULL;
    res = generator->convertKey(generator, NULL, NULL, &priKeyBlob, &outKeyPair);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(outKeyPair, nullptr);
    ASSERT_EQ(outKeyPair->pubKey, nullptr);
    ASSERT_NE(outKeyPair->priKey, nullptr);

    HcfBlob outPriKeyBlob = {
        .data = NULL,
        .len = 0
    };
    res = outKeyPair->priKey->base.getEncoded(&(outKeyPair->priKey->base), &outPriKeyBlob);

    ASSERT_EQ(res, HCF_SUCCESS);
    ASSERT_NE(outPriKeyBlob.data, nullptr);
    ASSERT_NE(outPriKeyBlob.len, 0);

    free(priKeyBlob.data);
    free(outPriKeyBlob.data);
    HcfObjDestroy(outKeyPair);
    HcfObjDestroy(keyPair);
    HcfObjDestroy(generator);
}
}
