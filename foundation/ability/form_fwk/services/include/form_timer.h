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
#ifndef OHOS_FORM_FWK_FORM_TIMER_H
#define OHOS_FORM_FWK_FORM_TIMER_H

#include "form_constants.h"
#include "form_util.h"

namespace OHOS {
namespace AppExecFwk {
/**
 * @enum UpdateType
 * Update type.
 */
enum UpdateType {
    TYPE_INTERVAL_CHANGE,
    TYPE_ATTIME_CHANGE,
    TYPE_INTERVAL_TO_ATTIME,
    TYPE_ATTIME_TO_INTERVAL,
    TYPE_INTERVAL_ONCE,
};
/**
 * @class FormTimer
 * form timer task.
 */
class FormTimer {
public:
    int64_t formId;
    int32_t userId;
    int64_t period;
    int hour;
    int min;
    bool isUpdateAt;
    int64_t refreshTime;
    bool isEnable = true;
    bool isCountTimer;
    UpdateType type = UpdateType::TYPE_INTERVAL_CHANGE;

    FormTimer()
    {
        formId = -1;
        userId = -1;
        period = -1;
        hour = -1;
        min = -1;
        isUpdateAt = false;
        isCountTimer = false;
        refreshTime = FormUtil::GetCurrentNanosecond() / Constants::TIME_1000000;
        type = UpdateType::TYPE_INTERVAL_CHANGE;
    }

    FormTimer(int64_t id, bool countTimer, int32_t uId = 0)
    {
        formId = id;
        userId = uId;
        period = -1;
        hour = -1;
        min = -1;
        isUpdateAt = false;
        isCountTimer = countTimer;
        refreshTime = FormUtil::GetCurrentNanosecond() / Constants::TIME_1000000;
        type = UpdateType::TYPE_INTERVAL_CHANGE;
    }

    FormTimer(int64_t id, long repeatTime, int32_t uId = 0)
    {
        formId = id;
        userId = uId;
        period = repeatTime;
        hour = -1;
        min = -1;
        isUpdateAt = false;
        isCountTimer = true;
        refreshTime = FormUtil::GetCurrentNanosecond() / Constants::TIME_1000000;
        type = UpdateType::TYPE_INTERVAL_CHANGE;
    }

    FormTimer(int64_t id, int hourTime, int minTime, int32_t uId = 0)
    {
        formId = id;
        userId = uId;
        hour = hourTime;
        min = minTime;
        period = -1;
        isUpdateAt = true;
        isCountTimer = false;
        refreshTime = FormUtil::GetCurrentNanosecond() / Constants::TIME_1000000;
        type = UpdateType::TYPE_INTERVAL_CHANGE;
    }

    ~FormTimer(void) {}
};
/**
 * @class UpdateAtItem
 * Update item at time.
 */
class UpdateAtItem {
public:
    long updateAtTime = -1;
    FormTimer refreshTask;
};
/**
 * @class DynamicRefreshItem
 * Dynamic refresh item.
 */
class DynamicRefreshItem {
public:
    int64_t formId = 0L;
    int64_t settedTime = INT64_MAX;
    int32_t userId = -1;

    DynamicRefreshItem() {}

    DynamicRefreshItem(int64_t id, int64_t time, int32_t uId = 0)
    {
        formId = id;
        settedTime = time;
        userId = uId;
    }

    ~DynamicRefreshItem(void) {}
};
/**
 * @struct LimitInfo
 * Limit info about a form.
 */
struct LimitInfo {
    int refreshCount = 0;
    bool isReported = false;
    bool remindFlag = false;
};

/**
 * @struct FormTimerCfg
 * Form timer config info.
 */
struct FormTimerCfg {
    bool enableUpdate = false;
    int64_t updateDuration = 0L;
    int updateAtHour = -1;
    int updateAtMin = -1;
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif // OHOS_FORM_FWK_FORM_TIMER_H
