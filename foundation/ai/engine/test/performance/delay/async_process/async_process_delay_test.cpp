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

#include <cstring>
#ifdef __LINUX__
#   include <unistd.h>
#   include <fcntl.c>
#endif

#include "gtest/gtest.h"

#include "client_executor/include/i_aie_client.inl"
#include "platform/time/include/time.h"
#include "protocol/retcode_inner/aie_retcode_inner.h"
#include "service_dead_cb.h"
#include "utils/aie_macros.h"
#include "utils/log/aie_log.h"

using namespace OHOS::AI;
using namespace testing::ext;

namespace {
    const int REQUEST_ID = 1;
    const int OPERATE_ID = 2;
    const long long CLIENT_INFO_VERSION = 1;
    const int SESSION_ID = -1;
    const long long ALGORITHM_INFO_CLIENT_VERSION = 1;
    const int ALGORITHM_ASYNC_TYPE = 1;
    const long long ALGORITHM_VERSION = 1;
    const int EXECUTE_TIMES = 100;
    const int COUNT = 20;
    const int CHAR_TYPE = 4;
    const int DESCRIPTION_LENGTH = 129;
    const int ALPHABET_LENGTH = 26;
    const int DIGIT = 10;
    const int UPPER_POSITION = 1;
    const int LOWER_POSITION = 2;
    const int SPACE_POSITION = 3;
    const int SLEEP_TIME = 2;
    const char MIN_UPPER_CASE_CHAR = 'A';
    const char MIN_LOWER_CASE_CHAR = 'a';
    const char MIN_NUMERIC_CHAR = '0';
    const char TRAILING_CHAR = '\0';
    const char * const PREPARE_INPUT_ASYNC = "Async prepare inputData";
    const int EXCEPTED_INIT_TIME = 30;
    const int EXCEPTED_PREPARE_TIME = 70;
    const int EXCEPTED_ASYNC_PROCESS_TIME = 40;
    const int EXCEPTED_RELEASE_TIME = 70;
    const int EXCEPTED_DESTROY_TIME = 30;
    const int EXCEPTED_SETOPTION_TIME = 30;
    const int EXCEPTED_GETOPTION_TIME = 30;
    std::time_t g_processStartTime;
    long long g_processTotalTime;
    long long g_initTotalTime;
    long long g_prepareTotalTime;
    long long g_setOptionTotalTime;
    long long g_getOptionTotalTime;
    long long g_releaseTotalTime;
    long long g_destroyTotalTime;
}

class ClientCallback : public IClientCb {
public:
    ClientCallback() = default;
    ~ClientCallback() override = default;
    void OnResult(const DataInfo &result, int resultCode, int requestId) override
    {
        std::time_t processEndTime = GetCurTimeMillSec();
        g_processTotalTime += processEndTime - g_processStartTime;
        HILOGI("[Test][AsyncProcess][%lld]", processEndTime - g_processStartTime);
    }
};

static int Random(void)
{
#ifndef __LINUX__
    return -1;
#else
#ifndef O_RDONLY
#define O_RDONLY 0u
#endif
    int r = -1;
    int fd = open("/dev/random", O_RDONLY);
    if (fd > 0) {
        (void)read(fd, &r, sizeof(int));
    }
    (void)close(fd);

    return r;
#endif
}

static void RandStr(const int len, char *str)
{
    int i;
    for (i = 0; i < len - 1; ++i) {
        switch (Random() % CHAR_TYPE) {
            case UPPER_POSITION:
                str[i] = MIN_UPPER_CASE_CHAR + Random() % ALPHABET_LENGTH;
                break;
            case LOWER_POSITION:
                str[i] = MIN_LOWER_CASE_CHAR + Random() % ALPHABET_LENGTH;
                break;
            case SPACE_POSITION:
                str[i] = ' ';
                break;
            default:
                str[i] = MIN_NUMERIC_CHAR + Random() % DIGIT;
                break;
        }
    }

    str[i] = TRAILING_CHAR;
}

class AsyncProcessTimeTest : public testing::Test {
public:
    // SetUpTestCase:The preset action of the test suite is executed before the first TestCase
    static void SetUpTestCase() {};

    // TearDownTestCase:The test suite cleanup action is executed after the last TestCase
    static void TearDownTestCase() {};

    // SetUp:Execute before each test case
    void SetUp() {};

    // TearDown:Execute after each test case
    void TearDown() {};
};

static void CheckTimeInit()
{
    double duration = static_cast<double>(g_initTotalTime)
                        / static_cast<double>(EXECUTE_TIMES);
    HILOGI("[Test][CheckTimeInit][%lf]", duration);
    ASSERT_TRUE((duration > 0) && (duration <= EXCEPTED_INIT_TIME));
}

static void CheckTimePrepare()
{
    double duration = static_cast<double>(g_prepareTotalTime)
                        / static_cast<double>(EXECUTE_TIMES);
    HILOGI("[Test][CheckTimePrepare][%lf]", duration);
    ASSERT_TRUE((duration > 0) && (duration <= EXCEPTED_PREPARE_TIME));
}

static void CheckTimeAsyncProcess()
{
    double duration = static_cast<double>(g_processTotalTime)
                        / static_cast<double>(EXECUTE_TIMES);
    HILOGI("[Test][CheckTimeAsyncProcess][%lf]", duration);
    ASSERT_TRUE((duration > 0) && (duration <= EXCEPTED_ASYNC_PROCESS_TIME));
}

static void CheckTimeRelease()
{
    double duration = static_cast<double>(g_releaseTotalTime)
                        / static_cast<double>(EXECUTE_TIMES);
    HILOGI("[Test][CheckTimeRelease][%lf]", duration);
    ASSERT_TRUE((duration > 0) && (duration <= EXCEPTED_RELEASE_TIME));
}

static void CheckTimeDestroy()
{
    double duration = static_cast<double>(g_destroyTotalTime)
                        / static_cast<double>(EXECUTE_TIMES);
    HILOGI("[Test][CheckTimeDestroy][%lf]", duration);
    ASSERT_TRUE((duration > 0) && (duration <= EXCEPTED_DESTROY_TIME));
}

static void CheckTimeSetOption()
{
    double duration = static_cast<double>(g_setOptionTotalTime)
                        / static_cast<double>(EXECUTE_TIMES);
    HILOGI("[Test][CheckTimeSetOption][%lf]", duration);
    ASSERT_TRUE((duration > 0) && (duration <= EXCEPTED_SETOPTION_TIME));
}

static void CheckTimeGetOption()
{
    double duration = static_cast<double>(g_getOptionTotalTime)
                        / static_cast<double>(EXECUTE_TIMES);
    HILOGI("[Test][CheckTimeGetOption][%lf]", duration);
    ASSERT_TRUE((duration > 0) && (duration <= EXCEPTED_GETOPTION_TIME));
}

/**
 * @tc.name: TestAsyncTime001
 * @tc.desc: Test Async Time Consumption of ALL Interfaces in AI System.
 * @tc.type: PERF
 * @tc.require: AR000F77MI
 */
HWTEST_F(AsyncProcessTimeTest, TestAsyncTime001, TestSize.Level0)
{
    HILOGI("[Test]AsyncProcessTimeTest001.");
    const char *str = PREPARE_INPUT_ASYNC;
    char *inputData = const_cast<char*>(str);
    int len = strlen(str) + 1;

    for (int i = 0; i < COUNT; ++i) {
        HILOGI("[Test]Test delay times:[%d]", i);
        char config[DESCRIPTION_LENGTH];
        RandStr(DESCRIPTION_LENGTH, config);
        ConfigInfo configInfo {.description = config};
        ClientInfo clientInfo = {
                .clientVersion = CLIENT_INFO_VERSION,
                .clientId = INVALID_CLIENT_ID,
                .sessionId = SESSION_ID,
                .serverUid = INVALID_UID,
                .clientUid = INVALID_UID,
                .extendLen = len,
                .extendMsg = reinterpret_cast<unsigned char*>(inputData),
                };

        AlgorithmInfo algoInfo = {
                .clientVersion = ALGORITHM_INFO_CLIENT_VERSION,
                .isAsync = true,
                .algorithmType = ALGORITHM_ASYNC_TYPE,
                .algorithmVersion = ALGORITHM_VERSION,
                .isCloud = true,
                .operateId = OPERATE_ID,
                .requestId = REQUEST_ID,
                .extendLen = len,
                .extendMsg = reinterpret_cast<unsigned char*>(inputData),
                };

        std::time_t initStartTime = GetCurTimeMillSec();
        ServiceDeadCb *cb = nullptr;
        AIE_NEW(cb, ServiceDeadCb());
        ASSERT_NE(cb, nullptr);
        int resultCode = AieClientInit(configInfo, clientInfo, algoInfo, cb);
        ASSERT_EQ(resultCode, RETCODE_SUCCESS);
        std::time_t initEndTime = GetCurTimeMillSec();
        g_initTotalTime += initEndTime - initStartTime;

        DataInfo inputInfo = {
                .data = reinterpret_cast<unsigned char*>(inputData),
                .length = len,
                };
        DataInfo outputInfo;

        ClientCallback *callback = nullptr;
        AIE_NEW(callback, ClientCallback());
        ASSERT_NE(cb, nullptr);
        std::time_t prepareStartTime = GetCurTimeMillSec();
        resultCode = AieClientPrepare(clientInfo, algoInfo, inputInfo, outputInfo, callback);
        ASSERT_EQ(resultCode, RETCODE_SUCCESS);
        std::time_t prepareEndTime = GetCurTimeMillSec();
        g_prepareTotalTime += prepareEndTime - prepareStartTime;

        g_processStartTime = GetCurTimeMillSec();
        resultCode = AieClientAsyncProcess(clientInfo, algoInfo, inputInfo);
        ASSERT_EQ(resultCode, RETCODE_SUCCESS);
        sleep(SLEEP_TIME);

        int optionType = 0;
        std::time_t setOptionStartTime = GetCurTimeMillSec();
        resultCode = AieClientSetOption(clientInfo, optionType, inputInfo);
        ASSERT_EQ(resultCode, RETCODE_SUCCESS);
        std::time_t setOptionEndTime = GetCurTimeMillSec();
        g_setOptionTotalTime += setOptionEndTime - setOptionStartTime;

        outputInfo = {
                .data = nullptr,
                .length = 0
        };
        std::time_t getOptionStartTime = GetCurTimeMillSec();
        resultCode = AieClientGetOption(clientInfo, optionType, inputInfo, outputInfo);
        ASSERT_EQ(resultCode, RETCODE_SUCCESS);
        std::time_t getOptionEndTime = GetCurTimeMillSec();
        g_getOptionTotalTime += getOptionEndTime - getOptionStartTime;

        std::time_t releaseStartTime = GetCurTimeMillSec();
        resultCode = AieClientRelease(clientInfo, algoInfo, inputInfo);
        ASSERT_EQ(resultCode, RETCODE_SUCCESS);
        std::time_t releaseEndTime = GetCurTimeMillSec();
        g_releaseTotalTime += releaseEndTime - releaseStartTime;

        std::time_t destroyStartTime = GetCurTimeMillSec();
        resultCode = AieClientDestroy(clientInfo);
        ASSERT_EQ(resultCode, RETCODE_SUCCESS);
        std::time_t destroyEndTime = GetCurTimeMillSec();
        g_destroyTotalTime += destroyEndTime - destroyStartTime;

        AIE_DELETE(cb);
        AIE_DELETE(callback);
    }
    CheckTimeInit();
    CheckTimePrepare();
    CheckTimeAsyncProcess();
    CheckTimeRelease();
    CheckTimeDestroy();
    CheckTimeSetOption();
    CheckTimeGetOption();
}
