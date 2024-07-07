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

#include <gtest/gtest.h>

#include <fstream>
#include <map>
#include <csignal>
#include <string>
#include <unistd.h>
#include <vector>

#include "dfx_define.h"
#include "dfx_signal_local_handler.h"
#include "directory_ex.h"

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

using namespace testing;
using namespace testing::ext;
using namespace std;

namespace OHOS {
namespace HiviewDFX {
class SignalHandlerTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();
};

void SignalHandlerTest::SetUpTestCase()
{}

void SignalHandlerTest::TearDownTestCase()
{}

void SignalHandlerTest::SetUp()
{}

void SignalHandlerTest::TearDown()
{}

static int CountLines(const string& filename)
{
    ifstream readStream;
    string tempData;
    readStream.open(filename.c_str(), ios::in);
    if (readStream.fail()) {
        return 0;
    } else {
        int n = 0;
        while (getline(readStream, tempData, '\n')) {
            n++;
        }
        readStream.close();
        return n;
    }
}

static string GetCppCrashFileName(pid_t pid)
{
    if (pid <= 0) {
        return "";
    }
    vector<string> files;
    OHOS::GetDirFiles("/data/log/faultlog/temp", files);
    string fileNamePrefix = "cppcrash-" + to_string(pid);
    for (const auto& file : files) {
        if (file.find(fileNamePrefix) != string::npos) {
            return file;
        }
    }
    return "";
}

static int CheckKeyWords(const string& filePath, string *keywords, int length)
{
    ifstream file;
    file.open(filePath.c_str(), ios::in);
    long lines = CountLines(filePath);
    vector<string> t(lines * 4); // 4 : max string blocks of one line
    int i = 0;
    int j = 0;
    string::size_type idx;
    int count = 0;
    int minRegIdx = 6; // 6 : index of REGISTERS
    int maxRegIdx = minRegIdx + REGISTERS_NUM + 1;
    while (!file.eof()) {
        file >> t.at(i);
        idx = t.at(i).find(keywords[j]);
        if (idx != string::npos) {
            GTEST_LOG_(INFO) << t.at(i);
            if (j > minRegIdx && j < maxRegIdx && t.at(i).size() < REGISTERS_LENGTH) {
                count--;
            }
            count++;
            j++;
            if (j == length) {
                break;
            }
            continue;
        }
        i++;
    }
    file.close();
    GTEST_LOG_(INFO) << count << " keys matched.";
    return count;
}

static bool CheckLocalCrashKeyWords(const string& filePath, pid_t pid, int sig)
{
    if (filePath.empty() || pid <= 0) {
        return false;
    }
    map<int, string> sigKey = {
        { SIGILL, string("Signal(4)") },
        { SIGABRT, string("Signal(6)") },
        { SIGBUS, string("Signal(7)") },
        { SIGSEGV, string("Signal(11)") },
    };
    string sigKeyword = "";
    map<int, string>::iterator iter = sigKey.find(sig);
    if (iter != sigKey.end()) {
        sigKeyword = iter->second;
    }
    string keywords[] = {
        "Pid:" + to_string(pid), "Uid:", "name:./test_signalhandler", sigKeyword, "Tid:", "#00", "test_signalhandler"
    };
    int length = sizeof(keywords) / sizeof(keywords[0]);
    return CheckKeyWords(filePath, keywords, length) == length;
}

/**
 * @tc.name: LocalHandlerTest001
 * @tc.desc: test crashlocalhandler signo(SIGILL)
 * @tc.type: FUNC
 */
HWTEST_F(SignalHandlerTest, LocalHandlerTest001, TestSize.Level2)
{
    GTEST_LOG_(INFO) << "DfxProcessDumpTest020: start.";
    pid_t pid = fork();
    if (pid < 0) {
        GTEST_LOG_(ERROR) << "Failed to fork new test process.";
    } else if (pid == 0) {
        DFX_InstallLocalSignalHandler();
        sleep(1);
    } else {
        usleep(10000); // 10000 : sleep 10ms
        GTEST_LOG_(INFO) << "process(" << getpid() << ") is ready to kill process(" << pid << ")";
        kill(pid, SIGILL);
        sleep(2); // 2 : wait for cppcrash generating
        bool ret = CheckLocalCrashKeyWords(GetCppCrashFileName(pid), pid, SIGILL);
        ASSERT_TRUE(ret);
    }
    GTEST_LOG_(INFO) << "DfxProcessDumpTest020: end.";
}

/**
 * @tc.name: LocalHandlerTest002
 * @tc.desc: test crashlocalhandler signo(SIGABRT)
 * @tc.type: FUNC
 */
HWTEST_F(SignalHandlerTest, LocalHandlerTest002, TestSize.Level2)
{
    GTEST_LOG_(INFO) << "LocalHandlerTest002: start.";
    pid_t pid = fork();
    if (pid < 0) {
        GTEST_LOG_(ERROR) << "Failed to fork new test process.";
    } else if (pid == 0) {
        DFX_InstallLocalSignalHandler();
        sleep(1);
    } else {
        usleep(10000); // 10000 : sleep 10ms
        GTEST_LOG_(INFO) << "process(" << getpid() << ") is ready to kill process(" << pid << ")";
        kill(pid, SIGABRT);
        sleep(2); // 2 : wait for cppcrash generating
        bool ret = CheckLocalCrashKeyWords(GetCppCrashFileName(pid), pid, SIGABRT);
        ASSERT_TRUE(ret);
    }
    GTEST_LOG_(INFO) << "LocalHandlerTest002: end.";
}

/**
 * @tc.name: LocalHandlerTest003
 * @tc.desc: test crashlocalhandler signo(SIGBUS)
 * @tc.type: FUNC
 */
HWTEST_F(SignalHandlerTest, LocalHandlerTest003, TestSize.Level2)
{
    GTEST_LOG_(INFO) << "LocalHandlerTest003: start.";
    pid_t pid = fork();
    if (pid < 0) {
        GTEST_LOG_(ERROR) << "Failed to fork new test process.";
    } else if (pid == 0) {
        DFX_InstallLocalSignalHandler();
        sleep(1);
    } else {
        usleep(10000); // 10000 : sleep 10ms
        GTEST_LOG_(INFO) << "process(" << getpid() << ") is ready to kill process(" << pid << ")";
        kill(pid, SIGBUS);
        sleep(2); // 2 : wait for cppcrash generating
        bool ret = CheckLocalCrashKeyWords(GetCppCrashFileName(pid), pid, SIGBUS);
        ASSERT_TRUE(ret);
    }
    GTEST_LOG_(INFO) << "LocalHandlerTest003: end.";
}

/**
 * @tc.name: LocalHandlerTest004
 * @tc.desc: test crashlocalhandler signo(SIGSEGV)
 * @tc.type: FUNC
 */
HWTEST_F(SignalHandlerTest, LocalHandlerTest004, TestSize.Level2)
{
    GTEST_LOG_(INFO) << "LocalHandlerTest004: start.";
    pid_t pid = fork();
    if (pid < 0) {
        GTEST_LOG_(ERROR) << "Failed to fork new test process.";
    } else if (pid == 0) {
        DFX_InstallLocalSignalHandler();
        sleep(1);
    } else {
        usleep(10000); // 10000 : sleep 10ms
        GTEST_LOG_(INFO) << "process(" << getpid() << ") is ready to kill process(" << pid << ")";
        kill(pid, SIGSEGV);
        sleep(2); // 2 : wait for cppcrash generating
        bool ret = CheckLocalCrashKeyWords(GetCppCrashFileName(pid), pid, SIGSEGV);
        ASSERT_TRUE(ret);
    }
    GTEST_LOG_(INFO) << "LocalHandlerTest004: end.";
}
} // namespace HiviewDFX
} // namepsace OHOS
