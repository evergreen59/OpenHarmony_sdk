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

#include "task_factory.h"

#include "anonymous_string.h"
#include "disable_task.h"
#include "distributed_hardware_log.h"
#include "enable_task.h"
#include "offline_task.h"
#include "online_task.h"
#include "task_board.h"

namespace OHOS {
namespace DistributedHardware {
#undef DH_LOG_TAG
#define DH_LOG_TAG "TaskFactory"

IMPLEMENT_SINGLE_INSTANCE(TaskFactory);
std::shared_ptr<Task> TaskFactory::CreateTask(TaskType taskType, TaskParam taskParam, std::shared_ptr<Task> fatherTask)
{
    DHLOGI("taskType = %d, networkId = %s, uuid = %s, dhId = %s",
        static_cast<int32_t>(taskType), GetAnonyString(taskParam.networkId).c_str(),
        GetAnonyString(taskParam.uuid).c_str(), GetAnonyString(taskParam.dhId).c_str());
    std::shared_ptr<Task> task = nullptr;
    switch (taskType) {
        case TaskType::ENABLE: {
            task = std::make_shared<EnableTask>(taskParam.networkId, taskParam.uuid, taskParam.dhId, taskParam.dhType);
            break;
        }
        case TaskType::DISABLE: {
            task = std::make_shared<DisableTask>(taskParam.networkId, taskParam.uuid, taskParam.dhId, taskParam.dhType);
            break;
        }
        case TaskType::ON_LINE: {
            task = std::make_shared<OnLineTask>(taskParam.networkId, taskParam.uuid, taskParam.dhId, taskParam.dhType);
            break;
        }
        case TaskType::OFF_LINE: {
            task = std::make_shared<OffLineTask>(taskParam.networkId, taskParam.uuid, taskParam.dhId, taskParam.dhType);
            break;
        }
        default: {
            DHLOGE("CreateTask type invalid, type: %d", taskType);
            return nullptr;
        }
    }

    if (fatherTask != nullptr) {
        task->SetFatherTask(fatherTask);
        fatherTask->AddChildrenTask(task);
    }
    TaskBoard::GetInstance().AddTask(task);

    return task;
}
} // namespace DistributedHardware
} // namespace OHOS
