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

#ifndef HCF_SIGNATURE_SPI_H
#define HCF_SIGNATURE_SPI_H

#include <stdbool.h>
#include "algorithm_parameter.h"
#include "pri_key.h"
#include "pub_key.h"
#include "result.h"

#define OPENSSL_RSA_SIGN_CLASS "OPENSSL.RSA.SIGN"

#define OPENSSL_RSA_VERIFY_CLASS "OPENSSL.RSA.VERIFY"

typedef struct HcfSignSpi HcfSignSpi;

struct HcfSignSpi {
    HcfObjectBase base;

    HcfResult (*engineInit)(HcfSignSpi *self, HcfParamsSpec *params, HcfPriKey *privateKey);

    HcfResult (*engineUpdate)(HcfSignSpi *self, HcfBlob *data);

    HcfResult (*engineSign)(HcfSignSpi *self, HcfBlob *data, HcfBlob *returnSignatureData);
};

typedef struct HcfVerifySpi HcfVerifySpi;

struct HcfVerifySpi {
    HcfObjectBase base;

    HcfResult (*engineInit)(HcfVerifySpi *self, HcfParamsSpec *params, HcfPubKey *publicKey);

    HcfResult (*engineUpdate)(HcfVerifySpi *self, HcfBlob *data);

    bool (*engineVerify)(HcfVerifySpi *self, HcfBlob *data, HcfBlob *signatureData);
};

#endif
