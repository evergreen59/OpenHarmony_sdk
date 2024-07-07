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

#ifndef HCF_ASY_KEY_GENERATOR_H
#define HCF_ASY_KEY_GENERATOR_H

#include <stdint.h>
#include "algorithm_parameter.h"
#include "result.h"
#include "key_pair.h"

enum HcfRsaKeySize {
    HCF_RSA_KEY_SIZE_512 = 512,
    HCF_RSA_KEY_SIZE_768 = 768,
    HCF_RSA_KEY_SIZE_1024 = 1024,
    HCF_RSA_KEY_SIZE_2048 = 2048,
    HCF_RSA_KEY_SIZE_3072 = 3072,
    HCF_RSA_KEY_SIZE_4096 = 4096,
    HCF_RSA_KEY_SIZE_8192 = 8192,
};

enum HcfRsaPrimesSize {
    HCF_RSA_PRIMES_SIZE_2 = 2,
    HCF_RSA_PRIMES_SIZE_3 = 3,
    HCF_RSA_PRIMES_SIZE_4 = 4,
    HCF_RSA_PRIMES_SIZE_5 = 5,
};

typedef struct HcfAsyKeyGenerator HcfAsyKeyGenerator;

struct HcfAsyKeyGenerator {
    HcfObjectBase base;

    HcfResult (*generateKeyPair)(HcfAsyKeyGenerator *self, HcfParamsSpec *params,
        HcfKeyPair **returnKeyPair);

    HcfResult (*convertKey)(HcfAsyKeyGenerator *self, HcfParamsSpec *params, HcfBlob *pubKeyBlob,
        HcfBlob *priKeyBlob, HcfKeyPair **returnKeyPair);

    const char *(*getAlgoName)(HcfAsyKeyGenerator *self);
};

#ifdef __cplusplus
extern "C" {
#endif

HcfResult HcfAsyKeyGeneratorCreate(const char *algoName, HcfAsyKeyGenerator **returnObj);

#ifdef __cplusplus
}
#endif

#endif
