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

#ifndef HCF_CERTIFICATE_H
#define HCF_CERTIFICATE_H

#include "blob.h"
#include "object_base.h"
#include "pub_key.h"
#include "result.h"

typedef struct HcfCertificate HcfCertificate;

struct HcfCertificate {
    struct HcfObjectBase base;

    /** Verify that this certificate corresponding to the specified public key. */
    HcfResult (*verify)(HcfCertificate *self, HcfPubKey *key);

    /** Get the serialized cert data.*/
    HcfResult (*getEncoded)(HcfCertificate *self, HcfEncodingBlob *encodedByte);

    /** Get the public key from this certificate. */
    HcfResult (*getPublicKey)(HcfCertificate *self, HcfPubKey **keyOut);
};

#endif // HCF_CERTIFICATE_H
