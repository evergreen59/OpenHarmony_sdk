/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#ifndef DFX_UNWIND_LOCAL_H
#define DFX_UNWIND_LOCAL_H
#include <atomic>
#include <chrono>
#include <cinttypes>
#include <condition_variable>
#include <csignal>
#include <libunwind.h>
#include <mutex>
#include "dfx_define.h"
#include "dfx_dump_request.h"
#include "dfx_frame.h"
#include "dfx_logger.h"
#include "dfx_maps.h"
#include "dfx_process.h"
#include "dfx_symbols_cache.h"
#include "dfx_thread.h"
#include "dfx_util.h"
#include "nocopyable.h"

namespace OHOS {
namespace HiviewDFX {
class DfxUnwindLocal {
public:
    static DfxUnwindLocal &GetInstance();
    ~DfxUnwindLocal() = default;

    bool Init();
    void Destroy();
    bool HasInit();

    bool ExecLocalDumpUnwind(size_t skipFramNum);
    bool ExecLocalDumpUnwindByWait();
    std::string CollectUnwindResult(int32_t tid);
    void CollectUnwindFrames(std::vector<std::shared_ptr<DfxFrame>>& frames);
    bool SendAndWaitRequest(int32_t tid);

private:
    DfxUnwindLocal();
    DISALLOW_COPY_AND_MOVE(DfxUnwindLocal);

    void InstallLocalDumper(int sig);
    void UninstallLocalDumper(int sig);

    bool SendLocalDumpRequest(int32_t tid);
    bool WaitLocalDumpRequest();

    void ResolveFrameInfo(size_t index, DfxFrame& frame);

    static void LocalDumpering(int sig, siginfo_t *si, void *context);
    void LocalDumper(int sig, siginfo_t *si, void *context);

    bool ExecLocalDumpUnwinding(unw_context_t *ctx, size_t skipFramNum);

private:
    unw_addr_space_t as_;
    unw_context_t context_;
    std::vector<DfxFrame> frames_;
    uint32_t curIndex_ = 0;
    std::unique_ptr<DfxSymbolsCache> cache_;
    sigset_t mask_;
    struct sigaction oldSigaction_;
    std::condition_variable localDumperCV_;
    std::mutex localDumperMutex_;
    std::atomic<bool> insideSignalHandler_;
    std::atomic<int> initTimes_;
    bool isInited_ = false;
};
} // namespace HiviewDFX
} // namespace OHOS
#endif // DFX_UNWIND_LOCAL_H
