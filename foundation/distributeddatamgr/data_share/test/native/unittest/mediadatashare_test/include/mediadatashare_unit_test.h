/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#ifndef MEDIA_DATASHARE_UNIT_TEST_H
#define MEDIA_DATASHARE_UNIT_TEST_H

#include <gtest/gtest.h>
#include "data_ability_observer_interface.h"

namespace OHOS {
namespace Media {
class MediaDataShareUnitTest : public testing::Test {
public:
    /* SetUpTestCase:The preset action of the test suite is executed before the first TestCase */
    static void SetUpTestCase(void);

    /* TearDownTestCase:The test suite cleanup action is executed after the last TestCase */
    static void TearDownTestCase(void);

    /* SetUp:Execute before each test case */
    void SetUp();

    /* TearDown:Execute after each test case */
    void TearDown();
};

class IDataShareObserverTest : public AAFwk::IDataAbilityObserver {
public:
    IDataShareObserverTest();
    ~IDataShareObserverTest()
    {}
    void OnChange()
    {
        GTEST_LOG_(INFO) << "OnChange enter";
    }
};
} // namespace Media
} // namespace OHOS

#endif  // MEDIA_DATASHARE_UNIT_TEST_H
