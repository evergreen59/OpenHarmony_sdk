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

#include "task_test.h"

#include <chrono>
#include <thread>

#include "dh_utils_tool.h"
#include "distributed_hardware_errno.h"
#include "mock_disable_task.h"
#include "mock_enable_task.h"
#include "mock_offline_task.h"
#include "mock_online_task.h"
#include "mock_task_factory.h"
#include "mock_task_utils.h"

#include "task_board.h"
#include "task_executor.h"

using namespace testing::ext;

namespace OHOS {
namespace DistributedHardware {
namespace {
const std::string DEV_ID_1 = "bb536a637105409e904d4da83790a4a8";
const std::string DEV_NETWORK_ID_1 = "nt36a637105409e904d4da83790a4a8";
const MockDevInfo DEV_INFO_11 = { DEV_NETWORK_ID_1, DEV_ID_1, "Camera_1", DHType::CAMERA };
const MockDevInfo DEV_INFO_12 = { DEV_NETWORK_ID_1, DEV_ID_1, "Gps_1", DHType::GPS };
const MockDevInfo DEV_INFO_13 = { DEV_NETWORK_ID_1, DEV_ID_1, "Audio_1", DHType::AUDIO };
const MockDevInfo DEV_INFO_14 = { DEV_NETWORK_ID_1, DEV_ID_1, "Display_1", DHType::SCREEN };
const MockDevInfo DEV_INFO_15 = { DEV_NETWORK_ID_1, DEV_ID_1, "Input_1", DHType::INPUT };

const std::string DEV_ID_2 = "06d177ffa09543389f3b445b4722f9be";
const std::string DEV_NETWORK_ID_2 = "ntd177ffa09543389f3b445b4722f9be";
const MockDevInfo DEV_INFO_21 = { DEV_NETWORK_ID_2, DEV_ID_2, "Camera_1", DHType::CAMERA };
const MockDevInfo DEV_INFO_22 = { DEV_NETWORK_ID_2, DEV_ID_2, "Gps_1", DHType::GPS };
const MockDevInfo DEV_INFO_23 = { DEV_NETWORK_ID_2, DEV_ID_2, "Audio_1", DHType::AUDIO };
const MockDevInfo DEV_INFO_24 = { DEV_NETWORK_ID_2, DEV_ID_2, "Display_1", DHType::SCREEN };
const MockDevInfo DEV_INFO_25 = { DEV_NETWORK_ID_2, DEV_ID_2, "Input_1", DHType::INPUT };

const TaskParam TASK_PARAM_1 = {
    .networkId = DEV_NETWORK_ID_1,
    .uuid = DEV_ID_1,
    .dhId = "",
    .dhType = DHType::UNKNOWN
};

const TaskParam TASK_PARAM_2 = {
    .networkId = DEV_NETWORK_ID_2,
    .uuid = DEV_ID_2,
    .dhId = "",
    .dhType = DHType::UNKNOWN
};
}

void TaskTest::SetUpTestCase(void)
{
}

void TaskTest::TearDownTestCase(void)
{
}

void TaskTest::SetUp()
{
}

void TaskTest::TearDown()
{
}

/**
 * @tc.name: task_test_001
 * @tc.desc: Verify the Execute OnLineTask function
 * @tc.type: FUNC
 * @tc.require: AR000GHSKP
 */
HWTEST_F(TaskTest, task_test_001, TestSize.Level0)
{
    std::shared_ptr<MockOnLineTask> onlineTask =
        std::static_pointer_cast<MockOnLineTask>(
        MockTaskFactory::GetInstance().CreateTask(TaskType::ON_LINE, TASK_PARAM_1, nullptr));
    onlineTask->SetOnLineDevInfos({ DEV_INFO_11, DEV_INFO_12, DEV_INFO_13, DEV_INFO_14, DEV_INFO_15 });
    TaskExecutor::GetInstance().PushTask(onlineTask);

    ASSERT_EQ(DH_FWK_SUCCESS, TaskBoard::GetInstance().WaitForALLTaskFinish());
    ASSERT_TRUE(TaskBoard::GetInstance().IsAllTaskFinish());
}

/**
 * @tc.name: task_test_002
 * @tc.desc: Verify the Execute OnLineTask function
 * @tc.type: FUNC
 * @tc.require: AR000GHSKN
 */
HWTEST_F(TaskTest, task_test_002, TestSize.Level0)
{
    std::shared_ptr<MockOnLineTask> onlineTask1 = std::static_pointer_cast<MockOnLineTask>(
        MockTaskFactory::GetInstance().CreateTask(TaskType::ON_LINE, TASK_PARAM_1, nullptr));
    onlineTask1->SetOnLineDevInfos({ DEV_INFO_11, DEV_INFO_12, DEV_INFO_13, DEV_INFO_14, DEV_INFO_15 });

    std::shared_ptr<MockOnLineTask> onlineTask2 = std::static_pointer_cast<MockOnLineTask>(
        MockTaskFactory::GetInstance().CreateTask(TaskType::ON_LINE, TASK_PARAM_2, nullptr));
    onlineTask2->SetOnLineDevInfos({ DEV_INFO_21, DEV_INFO_22, DEV_INFO_23, DEV_INFO_24, DEV_INFO_25 });

    TaskExecutor::GetInstance().PushTask(onlineTask1);
    TaskExecutor::GetInstance().PushTask(onlineTask2);

    ASSERT_EQ(DH_FWK_SUCCESS, TaskBoard::GetInstance().WaitForALLTaskFinish());
    ASSERT_TRUE(TaskBoard::GetInstance().IsAllTaskFinish());
}

/**
 * @tc.name: task_test_003
 * @tc.desc: Verify the Execute OffLineTask function
 * @tc.type: FUNC
 * @tc.require: AR000GHSJM
 */
HWTEST_F(TaskTest, task_test_003, TestSize.Level0)
{
    std::shared_ptr<MockOffLineTask> offlineTask =
        std::static_pointer_cast<MockOffLineTask>(
        MockTaskFactory::GetInstance().CreateTask(TaskType::OFF_LINE, TASK_PARAM_1, nullptr));
    offlineTask->SetOffLineDevInfos({ DEV_INFO_11, DEV_INFO_12, DEV_INFO_13, DEV_INFO_14, DEV_INFO_15 });
    TaskExecutor::GetInstance().PushTask(offlineTask);

    ASSERT_EQ(DH_FWK_SUCCESS, TaskBoard::GetInstance().WaitForALLTaskFinish());
    ASSERT_TRUE(TaskBoard::GetInstance().IsAllTaskFinish());
}

/**
 * @tc.name: task_test_004
 * @tc.desc: Verify the Execute OnLineTask and OffLineTask function
 * @tc.type: FUNC
 * @tc.require: AR000GHSJE
 */
HWTEST_F(TaskTest, task_test_004, TestSize.Level0)
{
    std::shared_ptr<MockOnLineTask> onlineTask =
        std::static_pointer_cast<MockOnLineTask>(
        MockTaskFactory::GetInstance().CreateTask(TaskType::ON_LINE, TASK_PARAM_1, nullptr));
    onlineTask->SetOnLineDevInfos({ DEV_INFO_11, DEV_INFO_12, DEV_INFO_13, DEV_INFO_14, DEV_INFO_15 });

    std::shared_ptr<MockOffLineTask> offlineTask =
        std::static_pointer_cast<MockOffLineTask>(
        MockTaskFactory::GetInstance().CreateTask(TaskType::OFF_LINE, TASK_PARAM_1, nullptr));
    offlineTask->SetOffLineDevInfos({ DEV_INFO_11, DEV_INFO_12, DEV_INFO_13, DEV_INFO_14, DEV_INFO_15 });

    TaskExecutor::GetInstance().PushTask(onlineTask);

    ASSERT_EQ(ERR_DH_FWK_TASK_TIMEOUT, TaskBoard::GetInstance().WaitForALLTaskFinish());
    ASSERT_FALSE(TaskBoard::GetInstance().IsAllTaskFinish());

    TaskExecutor::GetInstance().PushTask(offlineTask);
    ASSERT_EQ(DH_FWK_SUCCESS, TaskBoard::GetInstance().WaitForALLTaskFinish());
    ASSERT_TRUE(TaskBoard::GetInstance().IsAllTaskFinish());
}
} // namespace DistributedHardware
} // namespace OHOS
