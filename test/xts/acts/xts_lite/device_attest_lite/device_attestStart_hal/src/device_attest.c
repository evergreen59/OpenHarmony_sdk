/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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


#include "hctest.h"
#include "devattest_interface.h"

#define DEVATTEST_INIT -2
#define DEVATTEST_SUCCESS 0

typedef enum {
    SOFTWARE_RESULT_VERSIONID,
    SOFTWARE_RESULT_PATCHLEVEL,
    SOFTWARE_RESULT_ROOTHASH,
    SOFTWARE_RESULT_PCID,
    SOFTWARE_RESULT_RESERVE,
} SOFTWARE_RESULT_DETAIL_TYPE;

/**
 * @tc.desc      : register a test suite, this suite is used to test basic flow and interface dependency
 * @param        : subsystem name is dfx
 * @param        : module name is dfxlite
 * @param        : test suit name is DfxFuncTestSuite
 */
LITE_TEST_SUIT(device_attest, device_attest_lite, DeviceAttestFuncTestSuite);

/**
 * @tc.setup     : setup for all testcases
 * @return       : setup result, TRUE is success, FALSE is fail
 */
static BOOL DeviceAttestFuncTestSuiteSetUp(void)
{
    return TRUE;
}

/**
 * @tc.teardown  : teardown for all testcases
 * @return       : teardown result, TRUE is success, FALSE is fail
 */
static BOOL DeviceAttestFuncTestSuiteTearDown(void)
{
    printf("+-------------------------------------------+\n");
    return TRUE;
}

bool AttestStatusNumberValid(int32_t attestStatusNumber)
{
    if (attestStatusNumber < DEVATTEST_INIT || attestStatusNumber > DEVATTEST_SUCCESS) {
        return false;
    }
    return true;
}

bool AttestStatusValid(AttestResultInfo attestResultInfo)
{
    bool result = true;
    if (!AttestStatusNumberValid(attestResultInfo.authResult)) {
        result = false;
    }
    if (!AttestStatusNumberValid(attestResultInfo.softwareResult)) {
        result = false;
    }
    if (!AttestStatusNumberValid(attestResultInfo.softwareResultDetail[SOFTWARE_RESULT_VERSIONID])) {
        result = false;
    }
    if (!AttestStatusNumberValid(attestResultInfo.softwareResultDetail[SOFTWARE_RESULT_PATCHLEVEL])) {
        result = false;
    }
    if (!AttestStatusNumberValid(attestResultInfo.softwareResultDetail[SOFTWARE_RESULT_ROOTHASH])) {
        result = false;
    }
    if (!AttestStatusNumberValid(attestResultInfo.softwareResultDetail[SOFTWARE_RESULT_PCID])) {
        result = false;
    }
    if (!AttestStatusNumberValid(attestResultInfo.softwareResultDetail[SOFTWARE_RESULT_RESERVE])) {
        result = false;
    }
    if (attestResultInfo.authResult == DEVATTEST_SUCCESS) {
        if (attestResultInfo.ticketLength <= 0) {
            result = false;
        }
        if (attestResultInfo.ticket == "") {
            result = false;
        }
    }
    if (result) {
        return true;
    } else {
        return false;
    }
}

/**
 * @tc.number    : SUB_Device_Attest_0100
 * @tc.name      : StartDevAttestTask test
 * @tc.desc      : [C- SOFTWARE -0200]
 * @tc.size      : MEDIUM
 * @tc.type      : RELI
 * @tc.level     : Level 0
 */
LITE_TEST_CASE(DeviceAttestFuncTestSuite, subDeviceAttest0100, LEVEL0)
{
    int32_t ret = DEVATTEST_SUCCESS;
    ret = StartDevAttestTask();
    TEST_ASSERT_EQUAL_INT(ret, DEVATTEST_SUCCESS);
};

/**
 * @tc.number    : SUB_Device_Attest_0200
 * @tc.name      : GetAttestStatus test
 * @tc.desc      : [C- SOFTWARE -0200]
 * @tc.size      : MEDIUM
 * @tc.type      : RELI
 * @tc.level     : Level 0
 */
LITE_TEST_CASE(DeviceAttestFuncTestSuite, subDeviceAttest0200, LEVEL0)
{
    int32_t ret = DEVATTEST_SUCCESS;
    AttestResultInfo attestResultInfo = { 0 };
    attestResultInfo.ticket = NULL;
    ret = GetAttestStatus(&attestResultInfo);
    TEST_ASSERT_EQUAL_INT(ret, DEVATTEST_SUCCESS);
    TEST_ASSERT_EQUAL_INT(AttestStatusValid(attestResultInfo), true);  
};

RUN_TEST_SUITE(DeviceAttestFuncTestSuite);
