/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef DFX_PROCESSDUMP_TEST_H
#define DFX_PROCESSDUMP_TEST_H

#include <gtest/gtest.h>

#include "dfx_define.h"

#if defined(__arm__)
    #define REGISTERS           "r0:","r1:","r2:","r3:","r4:","r5:","r6:",\
                                "r7:","r8:","r9:","r10:","fp:","ip:","sp:","lr:","pc:"
    #define REGISTERS_NUM       16
    #define REGISTERS_LENGTH    10
#elif defined(__aarch64__)
    #define REGISTERS           "x0:","x1:","x2:","x3:","x4:","x5:","x6:","x7:","x8:",\
                                "x9:","x10:","x11:","x12:","x13:","x14:","x15:","x16:",\
                                "x17:","x18:","x19:","x20:","x21:","x22:","x23:","x24:",\
                                "x25:","x26:","x27:","x28:","x29:","lr:","sp:","pc:"
    #define REGISTERS_NUM       33
    #define REGISTERS_LENGTH    18
#endif

namespace OHOS {
namespace HiviewDFX {
class DfxProcessDumpTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};
} // namespace HiviewDFX
} // namespace OHOS
#endif // DFX_PROCESSDUMP_TEST_H