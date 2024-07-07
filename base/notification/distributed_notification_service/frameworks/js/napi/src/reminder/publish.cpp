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

#include "reminder/publish.h"

#include "ans_log_wrapper.h"
#include "common.h"
#include "reminder_request_alarm.h"
#include "reminder_request_calendar.h"
#include "reminder_request_timer.h"

namespace OHOS {
namespace ReminderAgentNapi {
static const int32_t PUBLISH_PARAM_LEN = 2;
static const int32_t CANCEL_PARAM_LEN = 2;
static const int32_t CANCEL_ALL_PARAM_LEN = 1;
static const int32_t GET_VALID_PARAM_LEN = 1;
static const int32_t ADD_SLOT_PARAM_LEN = 2;

struct AsyncCallbackInfo {
    explicit AsyncCallbackInfo(napi_env napiEnv) : env(napiEnv) {}
    ~AsyncCallbackInfo()
    {
        if (asyncWork) {
            napi_delete_async_work(env, asyncWork);
            asyncWork = nullptr;
        }
        if (callback) {
            napi_delete_reference(env, callback);
            callback = nullptr;
        }
    }

    napi_env env = nullptr;
    napi_async_work asyncWork = nullptr;
    napi_ref callback = nullptr;
    napi_value result = nullptr;
    int32_t reminderId = -1;
    bool isThrow = false;
    NotificationNapi::NotificationConstant::SlotType inType
        = NotificationNapi::NotificationConstant::SlotType::CONTENT_INFORMATION;
    std::shared_ptr<ReminderRequest> reminder = nullptr;
    std::vector<sptr<ReminderRequest>> validReminders;
    CallbackPromiseInfo info;
};

struct Parameters {
    int32_t reminderId = -1;
    int32_t errCode = ERR_OK;
    NotificationNapi::NotificationConstant::SlotType inType
        = NotificationNapi::NotificationConstant::SlotType::CONTENT_INFORMATION;
    std::shared_ptr<ReminderRequest> reminder = nullptr;
};

napi_value GetCallback(const napi_env &env, const napi_value &value, AsyncCallbackInfo &asyncCallbackInfo)
{
    napi_valuetype valuetype = napi_undefined;
    NAPI_CALL(env, napi_typeof(env, value, &valuetype));
    if (valuetype != napi_function) {
        ANSR_LOGW("Wrong argument type. Function expected.");
        return nullptr;
    }
    napi_create_reference(env, value, 1, &asyncCallbackInfo.callback);
    return NotificationNapi::Common::NapiGetNull(env);
}

void SetAsynccallbackinfo(const napi_env &env, AsyncCallbackInfo& asynccallbackinfo, napi_value& promise)
{
    ReminderCommon::PaddingCallbackPromiseInfo(
        env, asynccallbackinfo.callback, asynccallbackinfo.info, promise);
}

napi_value ParseParameters(const napi_env &env, const napi_callback_info &info, Parameters &params,
    AsyncCallbackInfo &asyncCallbackInfo, bool isThrow)
{
    size_t argc = PUBLISH_PARAM_LEN;
    napi_value argv[PUBLISH_PARAM_LEN] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));
    if (argc < 1) {
        ANSR_LOGW("Wrong number of arguments");
        if (isThrow) {
            ReminderCommon::HandleErrCode(env, ERR_REMINDER_INVALID_PARAM);
        }
        return nullptr;
    }

    // argv[1]: callback
    if (argc >= PUBLISH_PARAM_LEN) {
        if (GetCallback(env, argv[1], asyncCallbackInfo) == nullptr) {
            ANSR_LOGW("[reminderAgent]GetCallbak returns nullptr");
            if (isThrow) {
                ReminderCommon::HandleErrCode(env, ERR_REMINDER_INVALID_PARAM);
            }
            return nullptr;
        }
    }

    // argv[0] : reminderRequest
    if (ReminderCommon::GetReminderRequest(env, argv[0], params.reminder) == nullptr) {
        ANSR_LOGW("[reminderAgent]CreateReminder returns nullptr");
        if (isThrow) {
            ReminderCommon::HandleErrCode(env, ERR_REMINDER_INVALID_PARAM);
        }
        return nullptr;
    }

    return NotificationNapi::Common::NapiGetNull(env);
}

napi_value ParseSlotParameters(const napi_env &env, const napi_callback_info &info, Parameters &params,
    AsyncCallbackInfo &asyncCallbackInfo, bool isThrow)
{
    size_t argc = ADD_SLOT_PARAM_LEN;
    napi_value argv[ADD_SLOT_PARAM_LEN] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));
    if (argc < 1) {
        ANSR_LOGW("Wrong number of arguments");
        if (isThrow) {
            ReminderCommon::HandleErrCode(env, ERR_REMINDER_INVALID_PARAM);
        }
        return nullptr;
    }

    // argv[1]: callback
    if (argc >= ADD_SLOT_PARAM_LEN) {
        if (GetCallback(env, argv[1], asyncCallbackInfo) == nullptr) {
            ANSR_LOGW("GetCallbak returns nullptr");
            if (isThrow) {
                ReminderCommon::HandleErrCode(env, ERR_REMINDER_INVALID_PARAM);
            }
            return nullptr;
        }
    }

    // argv[0] : notificationSlot
    // slotType
    const char* propertyKey = "type";
    int32_t propertyVal = 0;
    if (!ReminderCommon::GetInt32(env, argv[0], propertyKey, propertyVal, false)) {
        ANSR_LOGW("Failed to get valid slot type.");
        params.errCode = ERR_REMINDER_INVALID_PARAM;
        if (isThrow) {
            ReminderCommon::HandleErrCode(env, ERR_REMINDER_INVALID_PARAM);
        }
        return nullptr;
    }

    if (!NotificationNapi::Common::SlotTypeJSToC(NotificationNapi::SlotType(propertyVal), params.inType)) {
        ANSR_LOGW("Failed to get valid slot type");
        if (isThrow) {
            ReminderCommon::HandleErrCode(env, ERR_REMINDER_INVALID_PARAM);
        }
        return nullptr;
    }
    return NotificationNapi::Common::NapiGetNull(env);
}

napi_value ParseCanCelParameter(const napi_env &env, const napi_callback_info &info, Parameters &params,
    AsyncCallbackInfo &asyncCallbackInfo, bool isThrow)
{
    ANSR_LOGI("ParseCanCelParameter");
    size_t argc = CANCEL_PARAM_LEN;
    napi_value argv[CANCEL_PARAM_LEN] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));
    if (argc < 1) {
        ANSR_LOGW("Wrong number of arguments");
        if (isThrow) {
            ReminderCommon::HandleErrCode(env, ERR_REMINDER_INVALID_PARAM);
        }
        return nullptr;
    }

    // argv[1]: callback
    if (argc >= CANCEL_PARAM_LEN) {
        if (GetCallback(env, argv[1], asyncCallbackInfo) == nullptr) {
            ANSR_LOGW("GetCallbak is nullptr");
            if (isThrow) {
                ReminderCommon::HandleErrCode(env, ERR_REMINDER_INVALID_PARAM);
            }
            return nullptr;
        }
    }

    // argv[0]: reminder id
    int32_t reminderId = -1;
    if (!ReminderCommon::GetInt32(env, argv[0], nullptr, reminderId, true)) {
        if (isThrow) {
            ReminderCommon::HandleErrCode(env, ERR_REMINDER_INVALID_PARAM);
        }
        return nullptr;
    }
    if (reminderId < 0) {
        ANSR_LOGW("Param id of cancels Reminder is illegal.");
        if (isThrow) {
            ReminderCommon::HandleErrCode(env, ERR_REMINDER_INVALID_PARAM);
        }
        return nullptr;
    }
    params.reminderId = reminderId;

    return NotificationNapi::Common::NapiGetNull(env);
}

napi_value ParseCanCelAllParameter(const napi_env &env, const napi_callback_info &info, Parameters &params,
    AsyncCallbackInfo &asyncCallbackInfo, bool isThrow)
{
    ANSR_LOGI("ParseCanCelAllParameter");
    size_t argc = CANCEL_ALL_PARAM_LEN;
    napi_value argv[CANCEL_ALL_PARAM_LEN] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));

    // argv[0]: callback
    if (argc >= CANCEL_ALL_PARAM_LEN) {
        if (GetCallback(env, argv[0], asyncCallbackInfo) == nullptr) {
            ANSR_LOGW("getCallbak is nullptr");
            if (isThrow) {
                ReminderCommon::HandleErrCode(env, ERR_REMINDER_INVALID_PARAM);
            }
            return nullptr;
        }
    }
    return NotificationNapi::Common::NapiGetNull(env);
}

napi_value ParseGetValidParameter(const napi_env &env, const napi_callback_info &info, Parameters &params,
    AsyncCallbackInfo &asyncCallbackInfo, bool isThrow)
{
    size_t argc = GET_VALID_PARAM_LEN;
    napi_value argv[GET_VALID_PARAM_LEN] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));

    // argv[0]: callback
    if (argc >= GET_VALID_PARAM_LEN) {
        if (GetCallback(env, argv[0], asyncCallbackInfo) == nullptr) {
            ANSR_LOGW("getCallbak is nullptr");
            if (isThrow) {
                ReminderCommon::HandleErrCode(env, ERR_REMINDER_INVALID_PARAM);
            }
            return nullptr;
        }
    }
    return NotificationNapi::Common::NapiGetNull(env);
}

napi_value DealErrorReturn(const napi_env &env, const napi_ref &callbackIn, const napi_value &result, bool isThrow)
{
    if (isThrow) {
        return nullptr;
    }
    if (callbackIn) {
        NotificationNapi::Common::SetCallback(env, callbackIn, ERR_REMINDER_INVALID_PARAM,
            result, false);
    }
    return NotificationNapi::Common::JSParaError(env, callbackIn);
}

napi_value CancelReminderInner(napi_env env, napi_callback_info info, bool isThrow)
{
    ANSR_LOGI("Cancel reminder");

    AsyncCallbackInfo *asynccallbackinfo = new (std::nothrow) AsyncCallbackInfo(env);
    if (!asynccallbackinfo) {
        ANSR_LOGE("Low memory.");
        return NotificationNapi::Common::NapiGetNull(env);
    }
    std::unique_ptr<AsyncCallbackInfo> callbackPtr { asynccallbackinfo };

    // param
    Parameters params;
    if (ParseCanCelParameter(env, info, params, *asynccallbackinfo, isThrow) == nullptr) {
        return DealErrorReturn(env, asynccallbackinfo->callback, NotificationNapi::Common::NapiGetNull(env), isThrow);
    }

    // promise
    napi_value promise = nullptr;
    SetAsynccallbackinfo(env, *asynccallbackinfo, promise);
    asynccallbackinfo->reminderId = params.reminderId;
    asynccallbackinfo->isThrow = isThrow;

    // resource name
    napi_value resourceName = nullptr;
    napi_create_string_latin1(env, "cancelReminder", NAPI_AUTO_LENGTH, &resourceName);

    // create and queue async work
    napi_create_async_work(env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            ANSR_LOGI("Cancel napi_create_async_work start");
            auto asynccallbackinfo = reinterpret_cast<AsyncCallbackInfo *>(data);
            if (asynccallbackinfo) {
                asynccallbackinfo->info.errorCode = ReminderHelper::CancelReminder(asynccallbackinfo->reminderId);
            }
        },
        [](napi_env env, napi_status status, void *data) {
            ANSR_LOGI("Cancel napi_create_async_work complete start");
            auto asynccallbackinfo = reinterpret_cast<AsyncCallbackInfo *>(data);
            std::unique_ptr<AsyncCallbackInfo> callbackPtr { asynccallbackinfo };

            ReminderCommon::ReturnCallbackPromise(
                env, asynccallbackinfo->info, NotificationNapi::Common::NapiGetNull(env), asynccallbackinfo->isThrow);
            ANSR_LOGI("Cancel napi_create_async_work complete end");
        },
        (void *)asynccallbackinfo,
        &asynccallbackinfo->asyncWork);
    NAPI_CALL(env, napi_queue_async_work(env, asynccallbackinfo->asyncWork));
    callbackPtr.release();

    if (asynccallbackinfo->info.isCallback) {
        return NotificationNapi::Common::NapiGetNull(env);
    } else {
        return promise;
    }
}

napi_value CancelReminderMgr(napi_env env, napi_callback_info info)
{
    return CancelReminderInner(env, info, true);
}

napi_value CancelReminder(napi_env env, napi_callback_info info)
{
    return CancelReminderInner(env, info, false);
}

napi_value CancelAllRemindersInner(napi_env env, napi_callback_info info, bool isThrow)
{
    ANSR_LOGI("Cancel all reminder");

    AsyncCallbackInfo *asynccallbackinfo = new (std::nothrow) AsyncCallbackInfo(env);
    if (!asynccallbackinfo) {
        ANSR_LOGE("Low memory.");
        return NotificationNapi::Common::NapiGetNull(env);
    }
    std::unique_ptr<AsyncCallbackInfo> callbackPtr { asynccallbackinfo };

    // param
    Parameters params;
    if (ParseCanCelAllParameter(env, info, params, *asynccallbackinfo, isThrow) == nullptr) {
        return DealErrorReturn(env, asynccallbackinfo->callback, NotificationNapi::Common::NapiGetNull(env), isThrow);
    }

    // promise
    napi_value promise = nullptr;
    SetAsynccallbackinfo(env, *asynccallbackinfo, promise);
    asynccallbackinfo->isThrow = isThrow;

    // resource name
    napi_value resourceName = nullptr;
    napi_create_string_latin1(env, "cancelAllReminders", NAPI_AUTO_LENGTH, &resourceName);

    // create and queue async work
    napi_create_async_work(env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            ANSR_LOGI("CancelAll napi_create_async_work start");
            auto asynccallbackinfo = reinterpret_cast<AsyncCallbackInfo *>(data);
            if (asynccallbackinfo) {
                asynccallbackinfo->info.errorCode = ReminderHelper::CancelAllReminders();
            }
        },
        [](napi_env env, napi_status status, void *data) {
            ANSR_LOGD("CancelAll napi_create_async_work complete start");
            auto asynccallbackinfo = reinterpret_cast<AsyncCallbackInfo *>(data);
            std::unique_ptr<AsyncCallbackInfo> callbackPtr { asynccallbackinfo };

            ReminderCommon::ReturnCallbackPromise(
                env, asynccallbackinfo->info, NotificationNapi::Common::NapiGetNull(env), asynccallbackinfo->isThrow);
            ANSR_LOGD("CancelAll napi_create_async_work complete end");
        },
        (void *)asynccallbackinfo,
        &asynccallbackinfo->asyncWork);
    NAPI_CALL(env, napi_queue_async_work(env, asynccallbackinfo->asyncWork));
    callbackPtr.release();

    if (asynccallbackinfo->info.isCallback) {
        return NotificationNapi::Common::NapiGetNull(env);
    } else {
        return promise;
    }
}

napi_value CancelAllRemindersMgr(napi_env env, napi_callback_info info)
{
    return CancelAllRemindersInner(env, info, true);
}

napi_value CancelAllReminders(napi_env env, napi_callback_info info)
{
    return CancelAllRemindersInner(env, info, false);
}

void ParseReminderTimer(const napi_env &env, const ReminderRequest &reminder, napi_value &result)
{
    napi_value value = nullptr;
    ReminderRequestTimer& timer = (ReminderRequestTimer&)reminder;
    napi_create_uint32(env, timer.GetInitInfo(), &value);
    napi_set_named_property(env, result, TIMER_COUNT_DOWN_TIME, value);
}

void ParseReminderAlarm(const napi_env &env, const ReminderRequest &reminder, napi_value &result)
{
    // hour
    napi_value value = nullptr;
    ReminderRequestAlarm& alarm = (ReminderRequestAlarm&)reminder;
    napi_create_uint32(env, static_cast<uint32_t>(alarm.GetHour()), &value);
    napi_set_named_property(env, result, ALARM_HOUR, value);

    // minute
    napi_create_uint32(env, static_cast<uint32_t>(alarm.GetMinute()), &value);
    napi_set_named_property(env, result, ALARM_MINUTE, value);

    // daysOfWeek
    napi_create_array(env, &value);
    napi_set_named_property(env, result, ALARM_DAYS_OF_WEEK, value);
    int32_t count = 0;
    for (auto day : alarm.GetDaysOfWeek()) {
        if (day) {
            napi_value napiDay = nullptr;
            napi_create_int32(env, day, &napiDay);
            napi_set_element(env, value, count, napiDay);
            count++;
        }
    }
}

void ParseReminderCalendar(const napi_env &env, const ReminderRequest &reminder, napi_value &result)
{
    // dateTime
    napi_value value = nullptr;
    ReminderRequestCalendar& calendar = (ReminderRequestCalendar&)reminder;
    napi_value dateTime = nullptr;
    napi_create_object(env, &dateTime);
    napi_set_named_property(env, result, CALENDAR_DATE_TIME, dateTime);

    napi_create_uint32(env, static_cast<uint32_t>(calendar.GetYear()), &value);
    napi_set_named_property(env, dateTime, CALENDAR_YEAR, value);
    napi_create_uint32(env, static_cast<uint32_t>(calendar.GetMonth()), &value);
    napi_set_named_property(env, dateTime, CALENDAR_MONTH, value);
    napi_create_uint32(env, static_cast<uint32_t>(calendar.GetDay()), &value);
    napi_set_named_property(env, dateTime, CALENDAR_DAY, value);
    napi_create_uint32(env, static_cast<uint32_t>(calendar.GetHour()), &value);
    napi_set_named_property(env, dateTime, CALENDAR_HOUR, value);
    napi_create_uint32(env, static_cast<uint32_t>(calendar.GetMinute()), &value);
    napi_set_named_property(env, dateTime, CALENDAR_MINUTE, value);
    napi_create_uint32(env, static_cast<uint32_t>(calendar.GetSecond()), &value);
    napi_set_named_property(env, dateTime, CALENDAR_SECOND, value);

    // repeatMonths
    napi_create_array(env, &value);
    napi_set_named_property(env, result, CALENDAR_REPEAT_MONTHS, value);
    int32_t count = 0;
    for (auto month : calendar.GetRepeatMonths()) {
        napi_value napiDay = nullptr;
        napi_create_int32(env, month, &napiDay);
        napi_set_element(env, value, count, napiDay);
        count++;
    }

    // repeatDays
    napi_create_array(env, &value);
    napi_set_named_property(env, result, CALENDAR_REPEAT_DAYS, value);
    count = 0;
    for (auto day : calendar.GetRepeatDays()) {
        napi_value napiDay = nullptr;
        napi_create_int32(env, day, &napiDay);
        napi_set_element(env, value, count, napiDay);
        count++;
    }
}

void ParseReminder(
    const napi_env &env, const ReminderRequest::ReminderType &type, const ReminderRequest &reminder, napi_value &result)
{
    switch (type) {
        case ReminderRequest::ReminderType::TIMER: {
            ParseReminderTimer(env, reminder, result);
            break;
        }
        case ReminderRequest::ReminderType::ALARM: {
            ParseReminderAlarm(env, reminder, result);
            break;
        }
        case ReminderRequest::ReminderType::CALENDAR: {
            ParseReminderCalendar(env, reminder, result);
            break;
        }
        default: {
            break;
        }
    }
}

void ParseActionButtons(const napi_env &env, ReminderRequest &reminder, napi_value &result)
{
    auto actionButtonsMap = reminder.GetActionButtons();

    // create array
    napi_value array = nullptr;
    napi_create_array(env, &array);
    napi_set_named_property(env, result, ACTION_BUTTON, array);
    int32_t index = 0;
    for (std::map<ReminderRequest::ActionButtonType, ReminderRequest::ActionButtonInfo>::iterator it
        = actionButtonsMap.begin(); it != actionButtonsMap.end(); ++it) {
        // create obj
        napi_value actionButton = nullptr;
        napi_create_object(env, &actionButton);

        napi_value buttonInfo = nullptr;
        napi_create_uint32(env, static_cast<int32_t>(it->second.type), &buttonInfo);
        napi_set_named_property(env, actionButton, ACTION_BUTTON_TYPE, buttonInfo);
        napi_create_string_utf8(env, (it->second.title).c_str(), NAPI_AUTO_LENGTH, &buttonInfo);
        napi_set_named_property(env, actionButton, ACTION_BUTTON_TITLE, buttonInfo);

        // add obj to array
        napi_set_element(env, array, index, actionButton);
        index++;
    }
}

void ParseWantAgent(const napi_env &env, ReminderRequest &reminder, napi_value &result)
{
    // create obj
    napi_value wantAgentInfo = nullptr;
    napi_create_object(env, &wantAgentInfo);
    napi_set_named_property(env, result, WANT_AGENT, wantAgentInfo);

    napi_value info = nullptr;
    napi_create_string_utf8(env, (reminder.GetWantAgentInfo()->pkgName).c_str(), NAPI_AUTO_LENGTH, &info);
    napi_set_named_property(env, wantAgentInfo, WANT_AGENT_PKG, info);
    napi_create_string_utf8(env, (reminder.GetWantAgentInfo()->abilityName).c_str(), NAPI_AUTO_LENGTH, &info);
    napi_set_named_property(env, wantAgentInfo, WANT_AGENT_ABILITY, info);
}

void ParseMaxScreenWantAgent(const napi_env &env, ReminderRequest &reminder, napi_value &result)
{
    // create obj
    napi_value maxScreenWantAgentInfo = nullptr;
    napi_create_object(env, &maxScreenWantAgentInfo);
    napi_set_named_property(env, result, MAX_SCREEN_WANT_AGENT, maxScreenWantAgentInfo);

    napi_value info = nullptr;
    napi_create_string_utf8(env, (reminder.GetMaxScreenWantAgentInfo()->pkgName).c_str(), NAPI_AUTO_LENGTH, &info);
    napi_set_named_property(env, maxScreenWantAgentInfo, MAX_SCREEN_WANT_AGENT_PKG, info);
    napi_create_string_utf8(env, (reminder.GetMaxScreenWantAgentInfo()->abilityName).c_str(), NAPI_AUTO_LENGTH, &info);
    napi_set_named_property(env, maxScreenWantAgentInfo, MAX_SCREEN_WANT_AGENT_ABILITY, info);
}

napi_value SetValidReminder(const napi_env &env, ReminderRequest &reminder, napi_value &result)
{
    ANSR_LOGI("enter");
    napi_value value = nullptr;

    napi_create_string_utf8(env, reminder.Dump().c_str(), NAPI_AUTO_LENGTH, &value);
    napi_set_named_property(env, result, "reminder", value);

    // type
    ReminderRequest::ReminderType type = reminder.GetReminderType();
    napi_create_int32(env, static_cast<int32_t>(type), &value);
    napi_set_named_property(env, result, REMINDER_TYPE, value);

    // reminder
    ParseReminder(env, type, reminder, result);

    // title
    napi_create_string_utf8(env, reminder.GetTitle().c_str(), NAPI_AUTO_LENGTH, &value);
    napi_set_named_property(env, result, TITLE, value);

    // content
    napi_create_string_utf8(env, reminder.GetContent().c_str(), NAPI_AUTO_LENGTH, &value);
    napi_set_named_property(env, result, CONTENT, value);

    // expiredContent
    napi_create_string_utf8(env, reminder.GetExpiredContent().c_str(), NAPI_AUTO_LENGTH, &value);
    napi_set_named_property(env, result, EXPIRED_CONTENT, value);

    // snoozeContent
    napi_create_string_utf8(env, reminder.GetSnoozeContent().c_str(), NAPI_AUTO_LENGTH, &value);
    napi_set_named_property(env, result, SNOOZE_CONTENT, value);

    // ringDuration
    napi_create_int64(env, reminder.GetRingDuration(), &value);
    napi_set_named_property(env, result, RING_DURATION, value);

    // timeInterval
    napi_create_int64(env, reminder.GetTimeInterval(), &value);
    napi_set_named_property(env, result, TIME_INTERVAL, value);

    // notificationId
    napi_create_int32(env, reminder.GetNotificationId(), &value);
    napi_set_named_property(env, result, NOTIFICATION_ID, value);

    // snoozeTimes
    napi_create_int32(env, reminder.GetSnoozeTimes(), &value);
    napi_set_named_property(env, result, SNOOZE_TIMES, value);

    // slotType
    NotificationNapi::SlotType jsSlotType;
    NotificationNapi::Common::SlotTypeCToJS(reminder.GetSlotType(), jsSlotType);
    napi_create_int32(env, static_cast<int32_t>(jsSlotType), &value);
    napi_set_named_property(env, result, SLOT_TYPE, value);

    // wantAgent
    ParseWantAgent(env, reminder, result);

    // maxScreenWantAgent
    ParseMaxScreenWantAgent(env, reminder, result);

    // actionButtons
    ParseActionButtons(env, reminder, result);
    return NotificationNapi::Common::NapiGetBoolean(env, true);
}

void GetValidRemindersInner(napi_env env, const std::vector<sptr<ReminderRequest>>& validReminders, napi_value& arr)
{
    int32_t count = 0;
    napi_create_array(env, &arr);
    for (auto reminder : validReminders) {
        if (reminder == nullptr) {
            ANSR_LOGW("reminder is null");
            continue;
        }
        napi_value result = nullptr;
        napi_create_object(env, &result);
        if (!SetValidReminder(env, *reminder, result)) {
            ANSR_LOGW("Set reminder object failed");
            continue;
        }
        napi_set_element(env, arr, count, result);
        count++;
    }
    ANSR_LOGI("GetValid reminders count = %{public}d", count);
}

napi_value InnerGetValidReminders(napi_env env, napi_callback_info info, bool isThrow)
{
    ANSR_LOGI("Get valid reminders");

    AsyncCallbackInfo *asynccallbackinfo = new (std::nothrow) AsyncCallbackInfo(env);
    if (!asynccallbackinfo) {
        ANSR_LOGE("Low memory.");
        return NotificationNapi::Common::NapiGetNull(env);
    }
    std::unique_ptr<AsyncCallbackInfo> callbackPtr { asynccallbackinfo };

    // param
    Parameters params;
    if (ParseGetValidParameter(env, info, params, *asynccallbackinfo, isThrow) == nullptr) {
        return DealErrorReturn(env, asynccallbackinfo->callback, NotificationNapi::Common::NapiGetNull(env), isThrow);
    }

    // promise
    napi_value promise = nullptr;
    SetAsynccallbackinfo(env, *asynccallbackinfo, promise);
    asynccallbackinfo->isThrow = isThrow;

    // resource name
    napi_value resourceName = nullptr;
    napi_create_string_latin1(env, "getValidReminders", NAPI_AUTO_LENGTH, &resourceName);

    // create and start async work
    napi_create_async_work(env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            ANSR_LOGI("GetValid reminders napi_create_async_work start");
            AsyncCallbackInfo *asynccallbackinfo = static_cast<AsyncCallbackInfo *>(data);
            if (asynccallbackinfo) {
                asynccallbackinfo->info.errorCode = ReminderHelper::GetValidReminders(
                    asynccallbackinfo->validReminders);
            }
        },
        [](napi_env env, napi_status status, void *data) {
            AsyncCallbackInfo *asynccallbackinfo = static_cast<AsyncCallbackInfo *>(data);
            std::unique_ptr<AsyncCallbackInfo> callbackPtr { asynccallbackinfo };

            if (asynccallbackinfo) {
                if (asynccallbackinfo->info.errorCode != ERR_OK) {
                    asynccallbackinfo->result = NotificationNapi::Common::NapiGetNull(env);
                } else {
                    GetValidRemindersInner(env, asynccallbackinfo->validReminders, asynccallbackinfo->result);
                }

                ReminderCommon::ReturnCallbackPromise(
                    env, asynccallbackinfo->info, asynccallbackinfo->result, asynccallbackinfo->isThrow);
            }
        },
        (void *)asynccallbackinfo,
        &asynccallbackinfo->asyncWork);
    NAPI_CALL(env, napi_queue_async_work(env, asynccallbackinfo->asyncWork));
    callbackPtr.release();

    if (asynccallbackinfo->info.isCallback) {
        return NotificationNapi::Common::NapiGetNull(env);
    } else {
        return promise;
    }
}

napi_value GetValidRemindersMgr(napi_env env, napi_callback_info info)
{
    return InnerGetValidReminders(env, info, true);
}

napi_value GetValidReminders(napi_env env, napi_callback_info info)
{
    return InnerGetValidReminders(env, info, false);
}

napi_value PublishReminderInner(napi_env env, napi_callback_info info, bool isThrow)
{
    ANSR_LOGI("PublishReminder");

    AsyncCallbackInfo *asynccallbackinfo = new (std::nothrow) AsyncCallbackInfo(env);
    if (!asynccallbackinfo) {
        ANSR_LOGE("Low memory.");
        return NotificationNapi::Common::NapiGetNull(env);
    }
    std::unique_ptr<AsyncCallbackInfo> callbackPtr { asynccallbackinfo };

    // param
    Parameters params;
    if (ParseParameters(env, info, params, *asynccallbackinfo, isThrow) == nullptr) {
        napi_create_int32(env, -1, &(asynccallbackinfo->result));
        return DealErrorReturn(env, asynccallbackinfo->callback, asynccallbackinfo->result, isThrow);
    }

    // promise
    napi_value promise = nullptr;
    SetAsynccallbackinfo(env, *asynccallbackinfo, promise);
    asynccallbackinfo->reminder = params.reminder;
    asynccallbackinfo->isThrow = isThrow;

    // resource name
    napi_value resourceName = nullptr;
    napi_create_string_latin1(env, "publishReminder", NAPI_AUTO_LENGTH, &resourceName);

    // create and start async work
    napi_create_async_work(env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            ANSR_LOGI("Publish napi_create_async_work start");
            AsyncCallbackInfo *asynccallbackinfo = static_cast<AsyncCallbackInfo *>(data);
            if (asynccallbackinfo) {
                asynccallbackinfo->info.errorCode = ReminderHelper::PublishReminder(*(asynccallbackinfo->reminder));
                ANSR_LOGD("Return reminderId=%{public}d", asynccallbackinfo->reminder->GetReminderId());
            }
        },
        [](napi_env env, napi_status status, void *data) {
            ANSR_LOGI("Publish napi_create_async_work complete start");
            AsyncCallbackInfo *asynccallbackinfo = static_cast<AsyncCallbackInfo *>(data);
            std::unique_ptr<AsyncCallbackInfo> callbackPtr { asynccallbackinfo };

            // reminderId
            if (asynccallbackinfo) {
                if (asynccallbackinfo->info.errorCode == ERR_OK) {
                    napi_create_int32(env, asynccallbackinfo->reminder->GetReminderId(), &(asynccallbackinfo->result));
                } else {
                    napi_create_int32(env, -1, &(asynccallbackinfo->result));
                }

                ReminderCommon::ReturnCallbackPromise(
                    env, asynccallbackinfo->info, asynccallbackinfo->result, asynccallbackinfo->isThrow);
                ANSR_LOGI("Publish napi_create_async_work complete end");
            }
        },
        (void *)asynccallbackinfo,
        &asynccallbackinfo->asyncWork);

    NAPI_CALL(env, napi_queue_async_work(env, asynccallbackinfo->asyncWork));
    callbackPtr.release();

    if (asynccallbackinfo->info.isCallback) {
        return NotificationNapi::Common::NapiGetNull(env);
    } else {
        return promise;
    }
}

napi_value PublishReminderMgr(napi_env env, napi_callback_info info)
{
    return PublishReminderInner(env, info, true);
}

napi_value PublishReminder(napi_env env, napi_callback_info info)
{
    return PublishReminderInner(env, info, false);
}

napi_value AddSlotInner(napi_env env, napi_callback_info info, bool isThrow)
{
    ANSR_LOGI("AddSlot");

    AsyncCallbackInfo *asynccallbackinfo = new (std::nothrow) AsyncCallbackInfo(env);
    if (!asynccallbackinfo) {
        ANSR_LOGE("Low memory.");
        return NotificationNapi::Common::NapiGetNull(env);
    }
    std::unique_ptr<AsyncCallbackInfo> callbackPtr { asynccallbackinfo };

    // param
    Parameters params;
    if (ParseSlotParameters(env, info, params, *asynccallbackinfo, isThrow) == nullptr) {
        return DealErrorReturn(env, asynccallbackinfo->callback, NotificationNapi::Common::NapiGetNull(env), isThrow);
    }

    // promise
    napi_value promise = nullptr;
    SetAsynccallbackinfo(env, *asynccallbackinfo, promise);
    asynccallbackinfo->inType = params.inType;
    asynccallbackinfo->info.errorCode = params.errCode;
    asynccallbackinfo->isThrow = isThrow;

    // resource name
    napi_value resourceName = nullptr;
    napi_create_string_latin1(env, "AddSlot", NAPI_AUTO_LENGTH, &resourceName);

    // create and start async work
    napi_create_async_work(env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            ANSR_LOGI("AddSlot napi_create_async_work start");
            AsyncCallbackInfo *asynccallbackinfo = static_cast<AsyncCallbackInfo *>(data);
            if (asynccallbackinfo && (asynccallbackinfo->info.errorCode == ERR_OK)) {
                asynccallbackinfo->info.errorCode = NotificationHelper::AddSlotByType(asynccallbackinfo->inType);
            }
        },
        [](napi_env env, napi_status status, void *data) {
            AsyncCallbackInfo *asynccallbackinfo = static_cast<AsyncCallbackInfo *>(data);
            if (asynccallbackinfo) {
                std::unique_ptr<AsyncCallbackInfo> callbackPtr { asynccallbackinfo };
                ReminderCommon::ReturnCallbackPromise(env, asynccallbackinfo->info,
                    NotificationNapi::Common::NapiGetNull(env), asynccallbackinfo->isThrow);
                ANSR_LOGD("AddSlot napi_create_async_work complete end.");
            }
        },
        (void *)asynccallbackinfo,
        &asynccallbackinfo->asyncWork);

    NAPI_CALL(env, napi_queue_async_work(env, asynccallbackinfo->asyncWork));
    callbackPtr.release();

    if (asynccallbackinfo->info.isCallback) {
        return NotificationNapi::Common::NapiGetNull(env);
    } else {
        return promise;
    }
}

napi_value AddSlotMgr(napi_env env, napi_callback_info info)
{
    return AddSlotInner(env, info, true);
}

napi_value AddSlot(napi_env env, napi_callback_info info)
{
    return AddSlotInner(env, info, false);
}
}
}