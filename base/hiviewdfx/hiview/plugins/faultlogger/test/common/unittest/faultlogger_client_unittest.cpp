/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#include <atomic>
#include <ctime>
#include <list>
#include <thread>

#include <gtest/gtest.h>
#include <unistd.h>

#include "faultlog_info.h"
#include "faultlog_query_result.h"
#include "faultlogger_client.h"
#include "faultlogger_client_test.h"
#include "file_util.h"
using namespace testing::ext;
using namespace OHOS::HiviewDFX;
namespace OHOS {
namespace HiviewDFX {
class FaultloggerClientUnittest : public testing::Test {
public:
    void SetUp()
    {
        chmod("/data/log/faultlog/", 0777); // 0777: add other user write permission
        chmod("/data/log/faultlog/faultlogger/", 0777); // 0777: add other user write permission
        sleep(1);
    };
    void TearDown()
    {
        chmod("/data/log/faultlog/", 0770); // 0770: restore permission
        chmod("/data/log/faultlog/faultlogger/", 0770); // 0770: restore permission
    };
};

/**
 * @tc.name: ReportCppCrashEventTest001
 * @tc.desc: Test calling ReportCppCrashEvent Func
 * @tc.type: FUNC
 */
HWTEST_F(FaultloggerClientUnittest, ReportCppCrashEventTest001, testing::ext::TestSize.Level3)
{
    auto now = time(nullptr);
    auto info = CreateFaultLogInfo(now, getuid(), FaultLogType::CPP_CRASH, "faultlogtest0");
    ReportCppCrashEvent(&info);
    ASSERT_TRUE(true);
}

/**
 * @tc.name: CheckFaultloggerStatusTest001
 * @tc.desc: Check status of the faultlogger systemcapabilty
 * @tc.type: FUNC
 */
HWTEST_F(FaultloggerClientUnittest, CheckFaultloggerStatusTest001, testing::ext::TestSize.Level3)
{
    bool status = CheckFaultloggerStatus();
    ASSERT_TRUE(status);
}

/**
 * @tc.name: AddFaultLogTest001
 * @tc.desc: add multiple logs into faultlogger, check whether the logs have been created
 * @tc.type: FUNC
 * @tc.require: AR000F83AK
 */
HWTEST_F(FaultloggerClientUnittest, AddFaultLogTest001, testing::ext::TestSize.Level3)
{
    /**
     * @tc.steps: step1. add faultlog with simplified parameters
     * @tc.steps: step2. check the return value of the interface
     * @tc.steps: step3. check the existence of target log file
     * @tc.expected: the calling is success and the file has been created
     */
    auto now = time(nullptr);
    const int32_t loopCount = 10;
    std::atomic<int> counter{0};
    auto task = [](int32_t now, std::atomic<int>& counter) {
        printf("AddFaultLog %d\n", now);
        auto info = CreateFaultLogInfo(now, getuid(), FaultLogType::CPP_CRASH, "faultlogtest1");
        AddFaultLog(info);
        sleep(5); // maybe 5 seconds is enough for process all AddLog request
        if (CheckLogFileExist(now, getuid(), "cppcrash", "faultlogtest1")) {
            counter++;
        }
    };
    printf("start AddFaultLog\n");
    sleep(1);
    for (int32_t i = 0; i < loopCount; i++) {
        now = now + 1;
        task(now, std::ref(counter));
    }

    ASSERT_GT(counter, 0);
    printf("Add %d logs.\n", counter.load());
}

/**
 * @tc.name: QuerySelfFaultLogTest001
 * @tc.desc: check the existence of the previous added faultlog
 * @tc.type: FUNC
 * @tc.require: AR000F83AK
 */
HWTEST_F(FaultloggerClientUnittest, QuerySelfFaultLogTest001, testing::ext::TestSize.Level3)
{
    /**
     * @tc.steps: step1. add multiple fault log by add fault log interface
     * @tc.steps: step2. query the log by QuerySelfFaultLog interfaces
     * @tc.steps: step3. check counts and contents of the log
     * @tc.expected: the count is correct and the contents is complete
     */
    const int maxQueryCount = 10;
    int32_t currentCount = 0;
    auto result = QuerySelfFaultLog(FaultLogType::NO_SPECIFIC, maxQueryCount);
    if (result != nullptr) {
        while (result->HasNext()) {
            if (currentCount >= maxQueryCount) {
                break;
            }
            auto info = result->Next();
            if (info == nullptr) {
                FAIL();
                return;
            }
            info->GetStringFaultType();
            currentCount++;
        }
    }
    printf("currentCount :%d \n", currentCount);
    ASSERT_EQ(currentCount, maxQueryCount);
}

/**
 * @tc.name: FaultLogInfoTest001
 * @tc.desc: check FaultLogInfo class
 * @tc.type: FUNC
 */
HWTEST_F(FaultloggerClientUnittest, FaultLogInfoTest001, testing::ext::TestSize.Level3)
{
    FaultLogInfo info;
    info.SetId(123);
    ASSERT_EQ(info.GetId(), 123);
    info.SetProcessId(1123);
    ASSERT_EQ(info.GetProcessId(), 1123);
    info.SetRawFileDescriptor(11);
    ASSERT_EQ(info.GetRawFileDescriptor(), 11);
    time_t time = std::time(nullptr);
    info.SetTimeStamp(time);
    ASSERT_EQ(info.GetTimeStamp(), time);
    info.SetFaultReason("some reason");
    ASSERT_EQ(info.GetFaultReason(), "some reason");
    info.SetModuleName("some module");
    ASSERT_EQ(info.GetModuleName(), "some module");
    info.SetFaultSummary("some summary");
    ASSERT_EQ(info.GetFaultSummary(), "some summary");
    info.SetFaultType(2);
    ASSERT_EQ(info.GetFaultType(), 2);
    ASSERT_EQ(info.GetStringFaultType(), "CppCrash");
    info.SetFaultType(3);
    ASSERT_EQ(info.GetFaultType(), 3);
    ASSERT_EQ(info.GetStringFaultType(), "JsCrash");
    info.SetFaultType(4);
    ASSERT_EQ(info.GetFaultType(), 4);
    ASSERT_EQ(info.GetStringFaultType(), "AppFreeze");
    info.SetFaultType(5);
    ASSERT_EQ(info.GetFaultType(), 5);
    ASSERT_EQ(info.GetStringFaultType(), "SysFreeze");
    info.SetFaultType(6);
    ASSERT_EQ(info.GetFaultType(), 6);
    ASSERT_EQ(info.GetStringFaultType(), "UnknownFaultType");
}
} // namespace HiviewDFX
} // namespace OHOS
