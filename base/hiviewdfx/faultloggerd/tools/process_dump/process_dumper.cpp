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

/* This files contains process dump main module. */

#include "process_dumper.h"

#include <cerrno>
#include <cinttypes>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <memory>
#include <string>

#include <fcntl.h>
#include <pthread.h>
#include <syscall.h>
#include <ucontext.h>
#include <unistd.h>

#include <faultloggerd_client.h>
#include <securec.h>

#include "cppcrash_reporter.h"
#include "dfx_config.h"
#include "dfx_define.h"
#include "dfx_dump_res.h"
#include "dfx_logger.h"
#include "dfx_process.h"
#include "dfx_ring_buffer_wrapper.h"
#include "dfx_signal.h"
#include "dfx_thread.h"
#include "dfx_unwind_remote.h"
#include "dfx_util.h"

namespace OHOS {
namespace HiviewDFX {
ProcessDumper &ProcessDumper::GetInstance()
{
    static ProcessDumper ins;
    return ins;
}

void ProcessDumper::PrintDumpProcessWithSignalContextHeader(std::shared_ptr<ProcessDumpRequest> request)
{
    auto info = request->GetSiginfo();
    auto msg = request->GetLastFatalMessage();
    if (info.si_signo != SIGDUMP) {
        DfxRingBufferWrapper::GetInstance().AppendMsg("Timestamp:" + GetCurrentTimeStr(request->GetTimeStamp()));
    } else {
        DfxRingBufferWrapper::GetInstance().AppendMsg("Timestamp:" + GetCurrentTimeStr());
    }
    DfxRingBufferWrapper::GetInstance().AppendBuf("Pid:%d\n", targetProcess_->GetPid());
    DfxRingBufferWrapper::GetInstance().AppendBuf("Uid:%d\n", targetProcess_->GetUid());
    DfxRingBufferWrapper::GetInstance().AppendBuf("Process name:%s\n", targetProcess_->GetProcessName().c_str());

    if (info.si_signo != SIGDUMP) {
        DfxRingBufferWrapper::GetInstance().AppendBuf("Reason:");
        DfxRingBufferWrapper::GetInstance().AppendMsg(PrintSignal(info));
        if (info.si_signo == SIGABRT && !msg.empty()) {
            DfxRingBufferWrapper::GetInstance().AppendBuf("LastFatalMessage:%s\n", msg.c_str());
        }

        auto traceId = request->GetTraceInfo();
        if (traceId.chainId != 0) {
            DfxRingBufferWrapper::GetInstance().AppendBuf("TraceId:%llx\n",
                static_cast<unsigned long long>(traceId.chainId));
        }

        if (targetProcess_->GetThreads().size() != 0) {
            DfxRingBufferWrapper::GetInstance().AppendBuf("Fault thread Info:\n");
        }
    }
}

int ProcessDumper::InitPrintThread(bool fromSignalHandler, std::shared_ptr<ProcessDumpRequest> request)
{
    int fd = -1;
    if (!fromSignalHandler) {
        fd = STDOUT_FILENO;
        DfxRingBufferWrapper::GetInstance().SetWriteFunc(ProcessDumper::WriteDumpBuf);
    } else {
        int32_t pid = request->GetPid();
        int32_t signo = request->GetSiginfo().si_signo;
        bool isCrash = (signo != SIGDUMP);
        FaultLoggerType type = isCrash ? FaultLoggerType::CPP_CRASH : FaultLoggerType::CPP_STACKTRACE;

        struct FaultLoggerdRequest faultloggerdRequest;
        if (memset_s(&faultloggerdRequest, sizeof(faultloggerdRequest), 0, sizeof(struct FaultLoggerdRequest)) != 0) {
            DfxLogError("memset_s error.");
            return fd;
        }

        if (isCrash) {
            if (DfxConfig::GetInstance().GetLogPersist()) {
                InitDebugLog((int)type, targetPid_, request->GetTid(), request->GetUid());
            }

            faultloggerdRequest.type = (int32_t)type;
            faultloggerdRequest.pid = request->GetPid();
            faultloggerdRequest.tid = request->GetTid();
            faultloggerdRequest.uid = request->GetUid();
            faultloggerdRequest.time = request->GetTimeStamp();
            if (strncpy_s(faultloggerdRequest.module, sizeof(faultloggerdRequest.module),
                targetProcess_->GetProcessName().c_str(), sizeof(faultloggerdRequest.module) - 1) != 0) {
                DfxLogWarn("Failed to set process name.");
                return fd;
            }
            fd = RequestFileDescriptorEx(&faultloggerdRequest);

            DfxRingBufferWrapper::GetInstance().SetWriteFunc(ProcessDumper::WriteDumpBuf);
            reporter_ = std::make_shared<CppCrashReporter>(request->GetTimeStamp(), request->GetSiginfo(),
                targetProcess_);
        } else {
            fd = RequestPipeFd(pid, FaultLoggerPipeType::PIPE_FD_WRITE_BUF);
            DfxLogDebug("write buf fd: %d", fd);

            resFd_ = RequestPipeFd(pid, FaultLoggerPipeType::PIPE_FD_WRITE_RES);
            DfxLogDebug("write res fd: %d", resFd_);
        }

        if (fd < 0) {
            DfxLogWarn("Failed to request fd from faultloggerd.");
        }
    }

    DfxRingBufferWrapper::GetInstance().SetWriteBufFd(fd);
    DfxRingBufferWrapper::GetInstance().StartThread();
    return fd;
}

void ProcessDumper::CreateVmProcessIfNeed(std::shared_ptr<ProcessDumpRequest> request, bool enableNs)
{
    if (request == nullptr) {
        return;
    }

    int vmPid = request->GetVmPid();
    if (vmPid <= 0) {
        return;
    }

    if (getppid() != vmPid) {
        DfxLogError("VM Process should be our parent.");
        return;
    }

    if (!enableNs) {
        targetVmPid_ = vmPid;
        targetVmNsPid_ = vmPid;
    }

    std::shared_ptr<DfxThread> vmThread = std::make_shared<DfxThread>(targetVmPid_, targetVmPid_, vmPid);
    if (!vmThread->Attach()) {
        DfxLogError("Fail to attach vm thread.");
        return;
    }

    vmProcess_ = DfxProcess::CreateProcessWithKeyThread(targetVmPid_, vmThread);
    vmProcess_->SetNsPid(targetVmNsPid_);
}

int ProcessDumper::DumpProcessWithSignalContext(std::shared_ptr<ProcessDumpRequest> request)
{
    int dumpRes = ProcessDumpRes::DUMP_ESUCCESS;
    do {
        ssize_t readCount = read(STDIN_FILENO, request.get(), sizeof(ProcessDumpRequest));
        if (readCount != static_cast<long>(sizeof(ProcessDumpRequest))) {
            DfxLogError("Fail to read DumpRequest(%d).", errno);
            dumpRes = ProcessDumpRes::DUMP_EREADREQUEST;
            break;
        }

        bool isCrash = (request->GetSiginfo().si_signo != SIGDUMP);
        std::string storeProcessName = request->GetProcessNameString();
        // We need check pid is same with getppid().
        // As in signal handler, current process is a child process, and target pid is our parent process.
        // If pid namespace is enalbed, both ppid and pid are equal one.
        // In this case, we have to parse /proc/self/stat
        if ((!isCrash && (syscall(SYS_getppid) != request->GetPid())) ||
            (isCrash && (syscall(SYS_getppid) != request->GetVmPid()))) {
            DfxLogError("Target process(%s:%d) is not parent pid(%d), exit processdump for signal(%d).",
                storeProcessName.c_str(), request->GetPid(), syscall(SYS_getppid), request->GetSiginfo().si_signo);
            dumpRes = ProcessDumpRes::DUMP_EGETPPID;
            break;
        }

        targetPid_ = request->GetPid();
        bool isPidNsEnabled = (targetPid_ == 1);
        if (isPidNsEnabled) {
            isPidNsEnabled = InitProcessNsInfo(request, isCrash);
        }

        CreateVmProcessIfNeed(request, isPidNsEnabled);
        DfxLogInfo("Processdump SigVal:%d, TargetPid:%d, TargetTid:%d.",
            request->GetSiginfo().si_value.sival_int, targetPid_, request->GetTid());
        if (InitProcessInfo(request, isCrash, isPidNsEnabled) < 0) {
            DfxLogError("Failed to init crash process info.");
            dumpRes = ProcessDumpRes::DUMP_EATTACH;
            break;
        }

        if (InitPrintThread(true, request) < 0) {
            DfxLogError("Failed to init print thread.");
            dumpRes = ProcessDumpRes::DUMP_EGETFD;
        }

        PrintDumpProcessWithSignalContextHeader(request);
        if (DfxUnwindRemote::GetInstance().UnwindProcess(targetProcess_) == false) {
            DfxLogError("Failed to unwind process.");
            dumpRes = ProcessDumpRes::DUMP_ESTOPUNWIND;
        }

        if (!isPidNsEnabled && (syscall(SYS_getppid) != request->GetPid()) &&
            (syscall(SYS_getppid) != request->GetVmPid())) {
            DfxRingBufferWrapper::GetInstance().AppendBuf(
                "Target process has been killed, the crash log may not be fully generated.");
            dumpRes = ProcessDumpRes::DUMP_EGETPPID;
            break;
        }
    } while (false);

    return dumpRes;
}

bool ProcessDumper::InitProcessNsInfo(std::shared_ptr<ProcessDumpRequest> request, bool isCrash)
{
    ProcInfo procInfo;
    bool ret = false;
    (void)memset_s(&procInfo, sizeof(procInfo), 0, sizeof(struct ProcInfo));
    if (GetProcStatus(procInfo) == -1) {
        return ret;
    }

    if (!isCrash) {
        if (procInfo.ppid == 0) { // real init case
            targetPid_ = 1;
            targetNsPid_ = 1;
            ret = false;
        } else {
            targetPid_ = procInfo.ppid;
            targetNsPid_ = getppid();
            ret = true;
        }
        DfxLogInfo("Dump in targetPid:%d targetNsPid:%d.", targetPid_, targetNsPid_);
    } else {
        ret = true;
        targetVmPid_ = procInfo.ppid;
        targetVmNsPid_ = getppid();
        DfxLogInfo("Crash in vmPid:%d nsVmPid:%d.", targetVmPid_, targetVmNsPid_);
        (void)memset_s(&procInfo, sizeof(procInfo), 0, sizeof(struct ProcInfo));
        if (GetProcStatusByPid(targetVmPid_, procInfo) == -1) {
            DfxLogError("Failed to read real target pid.");
            targetPid_ = targetVmPid_;
            targetNsPid_ = targetVmNsPid_;
        } else {
            targetPid_ = procInfo.ppid;
            targetNsPid_ = request->GetPid();
            DfxLogInfo("Crash in targetPid:%d targetNsPid:%d.", targetPid_, targetNsPid_);
        }
    }

    request->SetPid(targetPid_);
    return ret;
}

int ProcessDumper::InitProcessInfo(std::shared_ptr<ProcessDumpRequest> request, bool isCrash, bool enableNs)
{
    // if Nspid is enabled, target tid and real tid should be paresed from /proc/pid/task
    int tid = request->GetSiginfo().si_value.sival_int;
    int targetTid = request->GetTid();
    if (!enableNs) {
        targetNsPid_ = targetPid_;
    }

    std::shared_ptr<DfxThread> keyThread = isCrash ?
        std::make_shared<DfxThread>(targetPid_, targetPid_, targetTid, request->GetContext()) :
        std::make_shared<DfxThread>(targetPid_, tid == 0 ? targetPid_ : tid, targetTid);
    if (!keyThread->Attach()) {
        DfxLogError("Fail to attach key thread(%d).", targetTid);
        if (!isCrash || vmProcess_ == nullptr) {
            return -1;
        }
    }

    keyThread->SetIsCrashThread(true);
    if ((keyThread->GetThreadName()).empty()) {
        keyThread->SetThreadName(request->GetThreadNameString());
    }

    targetProcess_ = DfxProcess::CreateProcessWithKeyThread(targetPid_, keyThread);
    if (!targetProcess_) {
        return -1;
    }

    if ((targetProcess_->GetProcessName()).empty()) {
        targetProcess_->SetProcessName(request->GetProcessNameString());
    }

    targetProcess_->SetNsPid(targetNsPid_);
    targetProcess_->SetIsSignalDump(!isCrash);
    targetProcess_->SetUid(request->GetUid());
    targetProcess_->SetRecycleTid(request->GetRecycleTid());
    targetProcess_->SetFatalMessage(request->GetLastFatalMessage());
    if (tid == 0) {
        targetProcess_->InitOtherThreads(isCrash);
    }

    return 0;
}

int32_t ProcessDumper::GetTargetPid()
{
    if (vmProcess_ != nullptr) {
        return vmProcess_->GetPid();
    }

    if (targetProcess_ != nullptr) {
        return targetProcess_->GetPid();
    }

    return -1;
}

int32_t ProcessDumper::GetTargetNsPid()
{
    if (vmProcess_ != nullptr) {
        return vmProcess_->GetNsPid();
    }

    if (targetProcess_ != nullptr) {
        return targetProcess_->GetNsPid();
    }

    return -1;
}

void ProcessDumper::Dump()
{
    std::shared_ptr<ProcessDumpRequest> request = std::make_shared<ProcessDumpRequest>();
    resDump_ = DumpProcessWithSignalContext(request);
    if (targetProcess_ == nullptr) {
        DfxLogError("Dump process failed, please check permission and whether pid is valid.");
    } else {
        if (targetProcess_->GetIsSignalDump()) {
            targetProcess_->Detach();
        }

        if (vmProcess_ != nullptr) {
            vmProcess_->Detach();
        }
    }

    WriteDumpRes(resDump_);
    DfxRingBufferWrapper::GetInstance().StopThread();
    DfxLogInfo("Finish dump stacktrace for %s(%d:%d).",
        request->GetProcessNameString().c_str(), request->GetPid(), request->GetTid());
    CloseDebugLog();

    // check dump result ?
    if (reporter_ != nullptr) {
        reporter_->ReportToHiview();
    }

    _exit(0);
}

int ProcessDumper::WriteDumpBuf(int fd, const char* buf, const int len)
{
    if (buf == nullptr) {
        return -1;
    }
    return WriteLog(fd, "%s", buf);
}

void ProcessDumper::WriteDumpRes(int32_t res)
{
    DfxLogDebug("%s :: res: %d", __func__, res);
    DumpResMsg dumpResMsg;
    dumpResMsg.res = res;
    const char* strRes = DfxDumpRes::GetInstance().GetResStr(res);
    if (strncpy_s(dumpResMsg.strRes, sizeof(dumpResMsg.strRes), strRes, strlen(strRes)) != 0) {
        DfxLogError("%s :: strncpy failed.", __func__);
    }
    if (resFd_ > 0) {
        write(resFd_, &dumpResMsg, sizeof(struct DumpResMsg));
    } else {
        if (res != DUMP_ESUCCESS) {
            DfxRingBufferWrapper::GetInstance().AppendMsg("Result:\n");
            DfxRingBufferWrapper::GetInstance().AppendMsg(DfxDumpRes::GetInstance().ToString() + "\n");
        }
    }
}
} // namespace HiviewDFX
} // namespace OHOS
