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

#include "memory.h"
#include "memory_mock.h"

#include "log.h"
#include "securec.h"

static bool g_isMock = false;

void SetMockFlag(bool flag)
{
    g_isMock = flag;
}

void *HcfMalloc(uint32_t size, char val)
{
    if (g_isMock) {
        return NULL;
    }
    void *addr = malloc(size);
    if (addr != NULL) {
        (void)memset_s(addr, size, val, size);
    }
    return addr;
}

void HcfFree(void *addr)
{
    if (addr != NULL) {
        free(addr);
    }
}
