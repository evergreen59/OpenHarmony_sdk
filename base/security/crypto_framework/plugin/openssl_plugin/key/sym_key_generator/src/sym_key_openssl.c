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

#include <openssl/rand.h>
#include "log.h"
#include "memory.h"
#include "result.h"
#include "securec.h"
#include "utils.h"
#include "sym_common_defines.h"
#include "openssl_common.h"

#define MAX_KEY_STR_SIZE 12
#define MAX_KEY_LEN 4096
#define KEY_BIT 8
#define AES_ALG_NAME "AES"
#define DES_ALG_NAME "3DES"

typedef struct {
    HcfSymKeyGeneratorSpi base;
    SymKeyAttr attr;
} HcfSymKeyGeneratorSpiOpensslImpl;

static HcfResult GetEncoded(HcfKey *self, HcfBlob *key)
{
    if ((self == NULL) || (key == NULL)) {
        LOGE("Invalid input parameter!");
        return HCF_INVALID_PARAMS;
    }
    if (!IsClassMatch((const HcfObjectBase *)self, OPENSSL_SYM_KEY_CLASS)) {
        LOGE("Class is not match.");
        return HCF_INVALID_PARAMS;
    }
    SymKeyImpl *impl = (SymKeyImpl *)self;
    if ((impl->keyMaterial.data == NULL) || (impl->keyMaterial.len == 0)) {
        LOGE("Invalid SymKeyImpl parameter!");
        return HCF_INVALID_PARAMS;
    }
    key->data = (uint8_t *)HcfMalloc(impl->keyMaterial.len, 0);
    if (key->data == NULL) {
        LOGE("malloc keyMaterial failed!");
        return HCF_ERR_MALLOC;
    }
    (void)memcpy_s(key->data, impl->keyMaterial.len, impl->keyMaterial.data, impl->keyMaterial.len);
    key->len = impl->keyMaterial.len;
    return HCF_SUCCESS;
}

static void ClearMem(HcfSymKey *self)
{
    if (self == NULL) {
        LOGE("symKey is NULL.");
        return;
    }
    if (!IsClassMatch((const HcfObjectBase *)self, OPENSSL_SYM_KEY_CLASS)) {
        LOGE("Class is not match.");
        return;
    }
    SymKeyImpl *impl = (SymKeyImpl *)self;
    if ((impl->keyMaterial.data != NULL) && (impl->keyMaterial.len > 0)) {
        (void)memset_s(impl->keyMaterial.data, impl->keyMaterial.len, 0, impl->keyMaterial.len);
    }
}

static const char *GetFormat(HcfKey *self)
{
    if (self == NULL) {
        LOGE("Invalid input parameter!");
        return NULL;
    }
    if (!IsClassMatch((const HcfObjectBase *)self, OPENSSL_SYM_KEY_CLASS)) {
        LOGE("Class is not match.");
        return NULL;
    }

    return "PKCS#8";
}

static const char *GetSymKeyGeneratorClass(void)
{
    return OPENSSL_SYM_GENERATOR_CLASS;
}

static const char *GetSymKeyClass(void)
{
    return OPENSSL_SYM_KEY_CLASS;
}

static const char *GetAlgorithm(HcfKey *self)
{
    if (self == NULL) {
        LOGE("Invalid input parameter!");
        return NULL;
    }
    if (!IsClassMatch((const HcfObjectBase *)self, OPENSSL_SYM_KEY_CLASS)) {
        LOGE("Class is not match.");
        return NULL;
    }
    SymKeyImpl *impl = (SymKeyImpl *)self;
    return (const char *)impl->algoName;
}

static HcfResult RandomSymmKey(int32_t keyLen, HcfBlob *symmKey)
{
    uint8_t *keyMaterial = (uint8_t *)HcfMalloc(keyLen, 0);
    if (keyMaterial == NULL) {
        LOGE("keyMaterial malloc failed!");
        return HCF_ERR_MALLOC;
    }
    int ret = RAND_priv_bytes(keyMaterial, keyLen);
    if (ret != HCF_OPENSSL_SUCCESS) {
        LOGE("RAND_bytes failed!");
        HcfPrintOpensslError();
        HcfFree(keyMaterial);
        return HCF_ERR_CRYPTO_OPERATION;
    }
    symmKey->data = keyMaterial;
    symmKey->len = keyLen;
    return HCF_SUCCESS;
}

static HcfResult HcfSymmKeySpiCreate(int32_t keyLen, SymKeyImpl *symKey)
{
    if ((keyLen == 0) || (symKey == NULL)) {
        LOGE("Invalid input parameter!");
        return HCF_INVALID_PARAMS;
    }
    int32_t res = RandomSymmKey(keyLen, &symKey->keyMaterial);
    if (res != HCF_SUCCESS) {
        LOGE("RandomSymmKey failed!");
        return res;
    }
    return res;
}

static void DestroySymKeyGeneratorSpi(HcfObjectBase *base)
{
    if (base == NULL) {
        LOGE("Invalid input parameter!");
        return;
    }
    if (!IsClassMatch(base, GetSymKeyGeneratorClass())) {
        LOGE("Class is not match!");
        return;
    }
    HcfFree(base);
}

static void DestroySymKeySpi(HcfObjectBase *base)
{
    if (base == NULL) {
        LOGE("Invalid input parameter!");
        return;
    }
    if (!IsClassMatch(base, OPENSSL_SYM_KEY_CLASS)) {
        LOGE("Class is not match.");
        return;
    }
    SymKeyImpl *impl = (SymKeyImpl *)base;
    if (impl->algoName != NULL) {
        HcfFree(impl->algoName);
        impl->algoName = NULL;
    }
    if (impl->keyMaterial.data != NULL) {
        (void)memset_s(impl->keyMaterial.data, impl->keyMaterial.len, 0, impl->keyMaterial.len);
        HcfFree(impl->keyMaterial.data);
        impl->keyMaterial.data = NULL;
        impl->keyMaterial.len = 0;
    }
    HcfFree(impl);
}

static char *GetAlgoName(HcfSymKeyGeneratorSpiOpensslImpl *impl)
{
    char keySizeChar[MAX_KEY_STR_SIZE] = { 0 };
    if (sprintf_s(keySizeChar, MAX_KEY_STR_SIZE, "%d", impl->attr.keySize) < 0) {
        LOGE("Invalid input parameter!");
        return NULL;
    }

    char *algoName = (char *)HcfMalloc(MAX_KEY_STR_SIZE, 0);
    if (algoName == NULL) {
        LOGE("algoName malloc failed!");
        return NULL;
    }
    int32_t aesSize = strlen(AES_ALG_NAME);
    int32_t desSize = strlen(DES_ALG_NAME);
    HCF_ALG_VALUE type = impl->attr.algo;
    if (type == HCF_ALG_AES) {
        if (strcpy_s(algoName, MAX_KEY_STR_SIZE, AES_ALG_NAME) != EOK) {
            LOGE("aes algoName strcpy_s failed!");
            goto clearup;
        }
        if (strcpy_s(algoName + aesSize, MAX_KEY_STR_SIZE - aesSize, keySizeChar) != EOK) {
            LOGE("aes algoName size strcpy_s failed!");
            goto clearup;
        }
    } else if (type == HCF_ALG_DES) {
        if (strcpy_s(algoName, MAX_KEY_STR_SIZE, DES_ALG_NAME) != EOK) {
            LOGE("des algoName strcpy_s failed!");
            goto clearup;
        }
        if (strcpy_s(algoName + desSize, MAX_KEY_STR_SIZE - desSize, keySizeChar) != EOK) {
            LOGE("des algoName size strcpy_s failed!");
            goto clearup;
        }
    } else {
        LOGE("unsupport algo!");
        goto clearup;
    }
    return algoName;
clearup:
    HcfFree(algoName);
    return NULL;
}

static HcfResult CopySymmKey(const HcfBlob *srcKey, HcfBlob *dstKey)
{
    if ((srcKey->data == NULL) || (srcKey->len == 0)) {
        LOGE("Invalid input parameter!");
        return HCF_INVALID_PARAMS;
    }
    uint8_t *keyMaterial = (uint8_t *)HcfMalloc(srcKey->len, 0);
    if (keyMaterial == NULL) {
        LOGE("keyMaterial malloc failed!");
        return HCF_ERR_MALLOC;
    }
    (void)memcpy_s(keyMaterial, srcKey->len, srcKey->data, srcKey->len);
    dstKey->data = keyMaterial;
    dstKey->len = srcKey->len;
    return HCF_SUCCESS;
}

static HcfResult GenerateSymmKey(HcfSymKeyGeneratorSpi *self, HcfSymKey **symmKey)
{
    if ((self == NULL) || (symmKey == NULL)) {
        LOGE("Invalid input parameter!");
        return HCF_INVALID_PARAMS;
    }
    if (!IsClassMatch((const HcfObjectBase *)self, GetSymKeyGeneratorClass())) {
        LOGE("Class is not match!");
        return HCF_INVALID_PARAMS;
    }
    SymKeyImpl *returnSymmKey = (SymKeyImpl *)HcfMalloc(sizeof(SymKeyImpl), 0);
    if (returnSymmKey == NULL) {
        LOGE("Failed to allocate returnKeyPair memory!");
        return HCF_ERR_MALLOC;
    }
    HcfSymKeyGeneratorSpiOpensslImpl *impl = (HcfSymKeyGeneratorSpiOpensslImpl *)self;
    int32_t res = HcfSymmKeySpiCreate(impl->attr.keySize / KEY_BIT, returnSymmKey);
    if (res != HCF_SUCCESS) {
        HcfFree(returnSymmKey);
        return res;
    }
    returnSymmKey->algoName = GetAlgoName(impl);
    returnSymmKey->key.clearMem = ClearMem;
    returnSymmKey->key.key.getEncoded = GetEncoded;
    returnSymmKey->key.key.getFormat = GetFormat;
    returnSymmKey->key.key.getAlgorithm = GetAlgorithm;
    returnSymmKey->key.key.base.destroy = DestroySymKeySpi;
    returnSymmKey->key.key.base.getClass = GetSymKeyClass;
    *symmKey = (HcfSymKey *)returnSymmKey;
    return HCF_SUCCESS;
}

static HcfResult ConvertSymmKey(HcfSymKeyGeneratorSpi *self, const HcfBlob *key, HcfSymKey **symmKey)
{
    if ((self == NULL) || (symmKey == NULL) || !IsBlobValid(key)) {
        LOGE("Invalid input parameter.");
        return HCF_INVALID_PARAMS;
    }
    if (!IsClassMatch((const HcfObjectBase *)self, GetSymKeyGeneratorClass())) {
        LOGE("Class is not match.");
        return HCF_INVALID_PARAMS;
    }
    HcfSymKeyGeneratorSpiOpensslImpl *impl = (HcfSymKeyGeneratorSpiOpensslImpl *)self;

    if ((key->len == 0) || (key->len > MAX_KEY_LEN) || ((impl->attr.keySize / KEY_BIT) != (int32_t)key->len)) {
        LOGE("Invalid param: input key length is invalid!");
        return HCF_INVALID_PARAMS;
    }

    SymKeyImpl *returnSymmKey = (SymKeyImpl *)HcfMalloc(sizeof(SymKeyImpl), 0);
    if (returnSymmKey == NULL) {
        LOGE("Failed to allocate returnKeyPair memory!");
        return HCF_ERR_MALLOC;
    }
    int32_t res = CopySymmKey(key, &returnSymmKey->keyMaterial);
    if (res != HCF_SUCCESS) {
        HcfFree(returnSymmKey);
        return res;
    }
    returnSymmKey->algoName = GetAlgoName(impl);
    returnSymmKey->key.clearMem = ClearMem;
    returnSymmKey->key.key.getEncoded = GetEncoded;
    returnSymmKey->key.key.getFormat = GetFormat;
    returnSymmKey->key.key.getAlgorithm = GetAlgorithm;
    returnSymmKey->key.key.base.destroy = DestroySymKeySpi;
    returnSymmKey->key.key.base.getClass = GetSymKeyClass;
    *symmKey = (HcfSymKey *)returnSymmKey;
    return HCF_SUCCESS;
}

HcfResult HcfSymKeyGeneratorSpiCreate(SymKeyAttr *attr, HcfSymKeyGeneratorSpi **generator)
{
    if ((attr == NULL) || (generator == NULL)) {
        LOGE("Invalid input parameter.");
        return HCF_INVALID_PARAMS;
    }
    HcfSymKeyGeneratorSpiOpensslImpl *returnGenerator = (HcfSymKeyGeneratorSpiOpensslImpl *)HcfMalloc(
        sizeof(HcfSymKeyGeneratorSpiOpensslImpl), 0);
    if (returnGenerator == NULL) {
        LOGE("Failed to allocate returnGenerator memory!");
        return HCF_ERR_MALLOC;
    }
    (void)memcpy_s(&returnGenerator->attr, sizeof(SymKeyAttr), attr, sizeof(SymKeyAttr));
    returnGenerator->base.engineGenerateSymmKey = GenerateSymmKey;
    returnGenerator->base.engineConvertSymmKey = ConvertSymmKey;
    returnGenerator->base.base.destroy = DestroySymKeyGeneratorSpi;
    returnGenerator->base.base.getClass = GetSymKeyGeneratorClass;
    *generator = (HcfSymKeyGeneratorSpi *)returnGenerator;
    return HCF_SUCCESS;
}

