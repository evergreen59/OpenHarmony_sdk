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

#ifndef HCF_CRL_H
#define HCF_CRL_H

#include <stdbool.h>

#include "certificate.h"

typedef struct HcfCrl HcfCrl;

struct HcfCrl {
    /** HcfCrl inherit HcfObjectBase. */
    struct HcfObjectBase base;

    /** Check if the given certificate is on this CRL. */
    bool (*isRevoked)(HcfCrl *self, const HcfCertificate *cert);

    /** Returns the type of this CRL. */
    const char *(*getType)(HcfCrl *self);
};

#endif // HCF_CRL_H
