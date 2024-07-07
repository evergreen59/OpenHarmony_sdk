/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef CM_RESPONSE_H
#define CM_RESPONSE_H

#include "cm_type_inner.h"

#ifdef __cplusplus
extern "C" {
#endif

void CmSendResponse(const struct CmContext *context, int32_t result, const struct CmBlob *response);

int32_t CmGetProcessInfoForIPC(struct CmContext *cmContext);

#ifdef __cplusplus
}
#endif

#endif
