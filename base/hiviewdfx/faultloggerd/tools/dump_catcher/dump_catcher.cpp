/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* This files contains dumpcatcher main module. */

#include <iostream>
#include "dfx_define.h"
#include "dfx_dump_catcher.h"
#include "dfx_logger.h"
#include "dump_catcher.h"

namespace OHOS {
namespace HiviewDFX {
DumpCatcher &DumpCatcher::GetInstance()
{
    static DumpCatcher ins;
    return ins;
}

void DumpCatcher::Dump(int32_t type, int32_t pid, int32_t tid) const
{
    DfxDumpCatcher dfxDump;
    std::string msg = "";
    bool dumpRet = false;
    switch (type) {
        case DUMP_TYPE_NATIVE:
            dumpRet = dfxDump.DumpCatch(pid, tid, msg);
            break;
        case DUMP_TYPE_MIX:
            dumpRet = dfxDump.DumpCatchMix(pid, tid, msg);
            break;
        case DUMP_TYPE_KERNEL:
            break;
        default:
            DfxLogWarn("type(%d) invalid, must %d(native), %d(mix), %d(kernel)", \
                type, DUMP_TYPE_NATIVE, DUMP_TYPE_MIX, DUMP_TYPE_KERNEL);
            break;
    }

    if (!dumpRet) {
        std::cout << "Dump Failed." << std::endl;
    }

    if (!msg.empty()) {
        std::cout << msg << std::endl;
    } else {
        std::cout << "Dump msg empty." << std::endl;
    }
}
} // namespace HiviewDFX
} // namespace OHOS
