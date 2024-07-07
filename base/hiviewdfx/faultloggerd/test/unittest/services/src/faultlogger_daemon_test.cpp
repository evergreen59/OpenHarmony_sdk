/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

/* This files contains unit test of daemon module. */

#include "faultlogger_daemon_test.h"

#include <securec.h>
#include <unistd.h>

#define private public
#include "fault_logger_daemon.h"
#undef private

using namespace OHOS::HiviewDFX;
using namespace testing::ext;
using namespace std;

void FaultLoggerDaemonTest::SetUpTestCase(void)
{
}

void FaultLoggerDaemonTest::TearDownTestCase(void)
{
}

void FaultLoggerDaemonTest::SetUp(void)
{
}

void FaultLoggerDaemonTest::TearDown(void)
{
}

/**
 * @tc.name: FaultLoggerDaemonTest001
 * @tc.desc: test HandleDefaultClientRequest/HandleLogFileDesClientRequest func
 * @tc.type: FUNC
 */
HWTEST_F (FaultLoggerDaemonTest, FaultLoggerDaemonTest001, TestSize.Level2)
{
    GTEST_LOG_(INFO) << "FaultLoggerDaemonTest001: start.";
    std::shared_ptr<FaultLoggerDaemon> daemon = std::make_shared<FaultLoggerDaemon>();
    bool ret = daemon->InitEnvironment();
    ASSERT_TRUE(ret);
    struct FaultLoggerdRequest faultloggerdRequest;
    if (memset_s(&faultloggerdRequest, sizeof(faultloggerdRequest), 0, sizeof(struct FaultLoggerdRequest)) != 0) {
        GTEST_LOG_(ERROR) << "memset_s failed" ;
        ASSERT_TRUE(false);
    }
    faultloggerdRequest.type = 0;
    faultloggerdRequest.pid = getpid();
    faultloggerdRequest.tid = gettid();
    faultloggerdRequest.uid = getuid();
    daemon->HandleDefaultClientRequest(-1, &faultloggerdRequest);
    faultloggerdRequest.type = 2; // 2 : CPP_CRASH
    daemon->HandleDefaultClientRequest(-1, &faultloggerdRequest);
    daemon->HandleLogFileDesClientRequest(-1, &faultloggerdRequest);
    faultloggerdRequest.type = 101; // 101 : CPP_STACKTRACE
    daemon->HandleDefaultClientRequest(-1, &faultloggerdRequest);
    faultloggerdRequest.type = 102; // 102 : JS_STACKTRACE
    daemon->HandleDefaultClientRequest(-1, &faultloggerdRequest);
    faultloggerdRequest.type = 103; // 103 : JS_HEAP_SNAPSHOT
    daemon->HandleDefaultClientRequest(-1, &faultloggerdRequest);
    GTEST_LOG_(INFO) << "FaultLoggerDaemonTest001: end.";
}

/**
 * @tc.name: FaultLoggerDaemonTest002
 * @tc.desc: test HandleSdkDumpRequest/HandlePipeFdClientRequest func
 * @tc.type: FUNC
 */
HWTEST_F (FaultLoggerDaemonTest, FaultLoggerDaemonTest002, TestSize.Level2)
{
    GTEST_LOG_(INFO) << "FaultLoggerDaemonTest002: start.";
    std::shared_ptr<FaultLoggerDaemon> daemon = std::make_shared<FaultLoggerDaemon>();
    bool ret = daemon->InitEnvironment();
    ASSERT_TRUE(ret);
    struct FaultLoggerdRequest faultloggerdRequest;
    if (memset_s(&faultloggerdRequest, sizeof(faultloggerdRequest), 0, sizeof(struct FaultLoggerdRequest)) != 0) {
        GTEST_LOG_(ERROR) << "memset_s failed" ;
        ASSERT_TRUE(false);
    }
    faultloggerdRequest.type = 2; // 2 : CPP_CRASH
    faultloggerdRequest.pid = getpid();
    faultloggerdRequest.tid = gettid();
    faultloggerdRequest.uid = getuid();
    daemon->HandleSdkDumpRequest(-1, &faultloggerdRequest);
    faultloggerdRequest.pipeType = FaultLoggerPipeType::PIPE_FD_READ_BUF;
    daemon->HandlePipeFdClientRequest(-1, &faultloggerdRequest);
    faultloggerdRequest.pipeType = FaultLoggerPipeType::PIPE_FD_WRITE_BUF;
    daemon->HandlePipeFdClientRequest(-1, &faultloggerdRequest);
    faultloggerdRequest.pipeType = FaultLoggerPipeType::PIPE_FD_READ_RES;
    daemon->HandlePipeFdClientRequest(-1, &faultloggerdRequest);
    faultloggerdRequest.pipeType = FaultLoggerPipeType::PIPE_FD_WRITE_RES;
    daemon->HandlePipeFdClientRequest(-1, &faultloggerdRequest);
    faultloggerdRequest.pipeType = FaultLoggerPipeType::PIPE_FD_DELETE;
    daemon->HandlePipeFdClientRequest(-1, &faultloggerdRequest);
    GTEST_LOG_(INFO) << "FaultLoggerDaemonTest002: end.";
}

/**
 * @tc.name: FaultLoggerDaemonTest003
 * @tc.desc: test HandleSdkDumpRequest func
 * @tc.type: FUNC
 */
HWTEST_F (FaultLoggerDaemonTest, FaultLoggerDaemonTest003, TestSize.Level2)
{
    GTEST_LOG_(INFO) << "FaultLoggerDaemonTest003: start.";
    std::shared_ptr<FaultLoggerDaemon> daemon = std::make_shared<FaultLoggerDaemon>();
    bool ret = daemon->InitEnvironment();
    ASSERT_TRUE(ret);
    struct FaultLoggerdRequest faultloggerdRequest;
    if (memset_s(&faultloggerdRequest, sizeof(faultloggerdRequest), 0, sizeof(struct FaultLoggerdRequest)) != 0) {
        GTEST_LOG_(ERROR) << "memset_s failed" ;
        ASSERT_TRUE(false);
    }
    faultloggerdRequest.type = 2; // 2 : CPP_CRASH
    faultloggerdRequest.pid = getpid();
    faultloggerdRequest.tid = gettid();
    faultloggerdRequest.uid = getuid();
    daemon->HandleSdkDumpRequest(-1, &faultloggerdRequest);
    faultloggerdRequest.tid = 0;
    daemon->HandleSdkDumpRequest(-1, &faultloggerdRequest);
    GTEST_LOG_(INFO) << "FaultLoggerDaemonTest003: end.";
}
