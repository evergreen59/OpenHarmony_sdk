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

#include "blob.h"

#include <securec.h>
#include "memory.h"
#include "log.h"

void HcfBlobDataFree(HcfBlob *blob)
{
    if ((blob == NULL) || (blob->data == NULL)) {
        return;
    }
    HcfFree(blob->data);
    blob->data = NULL;
    blob->len = 0;
}

void HcfBlobDataClearAndFree(HcfBlob *blob)
{
    if ((blob == NULL) || (blob->data == NULL)) {
        LOGD("The input blob is null, no need to free.");
        return;
    }
    (void)memset_s(blob->data, blob->len, 0, blob->len);
    HcfFree(blob->data);
    blob->data = NULL;
    blob->len = 0;
}

void HcfEncodingBlobDataFree(HcfEncodingBlob *encodingBlob)
{
    if ((encodingBlob == NULL) || (encodingBlob->data == NULL)) {
        LOGD("The input encodingBlob is null, no need to free.");
        return;
    }
    HcfFree(encodingBlob->data);
    encodingBlob->data = NULL;
    encodingBlob->len = 0;
}

void HcfArrayDataClearAndFree(HcfArray *array)
{
    if (array == NULL) {
        LOGD("The input array is null, no need to free.");
        return;
    }
    for (uint32_t i = 0; i < array->count; ++i) {
        HcfFree(array->data[i].data);
        array->data[i].data = NULL;
        array->data[i].len = 0;
    }
    array->count = 0;
    HcfFree(array->data);
    array->data = NULL;
}
