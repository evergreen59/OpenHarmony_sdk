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

#ifndef HCF_SIGNATURE_H
#define HCF_SIGNATURE_H

#include <stdint.h>
#include <stdbool.h>
#include "algorithm_parameter.h"
#include "result.h"
#include "key_pair.h"

typedef struct HcfSign HcfSign;

struct HcfSign {
    HcfObjectBase base;

    HcfResult (*init)(HcfSign *self, HcfParamsSpec *params, HcfPriKey *privateKey);

    HcfResult (*update)(HcfSign *self, HcfBlob *data);

    HcfResult (*sign)(HcfSign *self, HcfBlob *data, HcfBlob *returnSignatureData);

    const char *(*getAlgoName)(HcfSign *self);
};

typedef struct HcfVerify HcfVerify;

struct HcfVerify {
    HcfObjectBase base;

    HcfResult (*init)(HcfVerify *self, HcfParamsSpec *params, HcfPubKey *publicKey);

    HcfResult (*update)(HcfVerify *self, HcfBlob *data);

    bool (*verify)(HcfVerify *self, HcfBlob *data, HcfBlob *signatureData);

    const char *(*getAlgoName)(HcfVerify *self);
};

#ifdef __cplusplus
extern "C" {
#endif

HcfResult HcfSignCreate(const char *algoName, HcfSign **returnObj);

HcfResult HcfVerifyCreate(const char *algoName, HcfVerify **returnObj);

#ifdef __cplusplus
}
#endif

#endif
