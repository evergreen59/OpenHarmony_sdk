/*
 * Copyright (c) 2021-2023 Huawei Device Co., Ltd.
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

#include "subscribe.h"

#include <mutex>
#include <uv.h>

namespace OHOS {
namespace NotificationNapi {
const int32_t SUBSRIBE_MAX_PARA = 3;
const int32_t NO_DELETE_REASON = -1;
const std::string CONSUME = "onConsume";
const std::string CANCEL = "onCancel";
const std::string UPDATE = "onUpdate";
const std::string CONNECTED = "onConnect";
const std::string DIS_CONNECTED = "onDisconnect";
const std::string DIE = "onDestroy";
const std::string DISTURB_MODE_CHANGE = "onDisturbModeChange";
const std::string DISTURB_DATE_CHANGE = "onDoNotDisturbDateChange";
const std::string ENABLE_NOTIFICATION_CHANGED = "OnEnabledNotificationChanged";

struct NotificationReceiveDataWorker {
    napi_env env = nullptr;
    napi_ref ref = nullptr;
    std::shared_ptr<OHOS::Notification::Notification> request;
    std::shared_ptr<NotificationSortingMap> sortingMap;
    NotificationDoNotDisturbDate date;
    EnabledNotificationCallbackData callbackData;
    int32_t deleteReason = 0;
    int32_t result = 0;
    int32_t disturbMode = 0;
    SubscriberInstance *subscriber = nullptr;
};

napi_value SetSubscribeCallbackData(const napi_env &env,
    const std::shared_ptr<OHOS::Notification::Notification> &request,
    const std::shared_ptr<NotificationSortingMap> &sortingMap, int32_t deleteReason, napi_value &result)
{
    ANS_LOGI("enter");
    if (request == nullptr) {
        ANS_LOGE("request is null");
        return Common::NapiGetBoolean(env, false);
    }

    if (sortingMap == nullptr) {
        ANS_LOGE("sortingMap is null");
        return Common::NapiGetBoolean(env, false);
    }

    // request: NotificationRequest
    napi_value requestResult = nullptr;
    napi_create_object(env, &requestResult);
    if (!Common::SetNotification(env, request.get(), requestResult)) {
        ANS_LOGE("SetNotification call failed");
        return Common::NapiGetBoolean(env, false);
    }
    napi_set_named_property(env, result, "request", requestResult);

    // sortingMap?: NotificationSortingMap
    napi_value sortingMapResult = nullptr;
    napi_create_object(env, &sortingMapResult);
    if (!Common::SetNotificationSortingMap(env, sortingMap, sortingMapResult)) {
        ANS_LOGE("SetNotificationSortingMap call failed");
        return Common::NapiGetBoolean(env, false);
    }
    napi_set_named_property(env, result, "sortingMap", sortingMapResult);

    // reason?: number
    if (deleteReason != NO_DELETE_REASON) {
        napi_value value = nullptr;
        int32_t outReason = 0;
        if (!Common::ReasonCToJS(deleteReason, outReason)) {
            return Common::NapiGetBoolean(env, false);
        }
        napi_create_int32(env, outReason, &value);
        napi_set_named_property(env, result, "reason", value);
    }

    // sound?: string
    napi_value soundResult = nullptr;
    std::string sound;
    if (request->EnableSound()) {
        sound = request->GetSound().ToString();
    }
    napi_create_string_utf8(env, sound.c_str(), NAPI_AUTO_LENGTH, &soundResult);
    napi_set_named_property(env, result, "sound", soundResult);

    // vibrationValues?: Array<number>
    napi_value arr = nullptr;
    napi_create_array(env, &arr);
    if (request->EnableVibrate()) {
        uint32_t count = 0;
        for (auto vec : request->GetVibrationStyle()) {
            napi_value nVibrationValue = nullptr;
            napi_create_int64(env, vec, &nVibrationValue);
            napi_set_element(env, arr, count, nVibrationValue);
            count++;
        }
    }
    napi_set_named_property(env, result, "vibrationValues", arr);

    return Common::NapiGetBoolean(env, true);
}

SubscriberInstance::SubscriberInstance()
{}

SubscriberInstance::~SubscriberInstance()
{
    if (canceCallbackInfo_.ref != nullptr) {
        napi_delete_reference(canceCallbackInfo_.env, canceCallbackInfo_.ref);
    }
    if (consumeCallbackInfo_.ref != nullptr) {
        napi_delete_reference(consumeCallbackInfo_.env, consumeCallbackInfo_.ref);
    }
    if (updateCallbackInfo_.ref != nullptr) {
        napi_delete_reference(updateCallbackInfo_.env, updateCallbackInfo_.ref);
    }
    if (subscribeCallbackInfo_.ref != nullptr) {
        napi_delete_reference(subscribeCallbackInfo_.env, subscribeCallbackInfo_.ref);
    }
    if (unsubscribeCallbackInfo_.ref != nullptr) {
        napi_delete_reference(unsubscribeCallbackInfo_.env, unsubscribeCallbackInfo_.ref);
    }
    if (dieCallbackInfo_.ref != nullptr) {
        napi_delete_reference(dieCallbackInfo_.env, dieCallbackInfo_.ref);
    }
    if (disturbModeCallbackInfo_.ref != nullptr) {
        napi_delete_reference(disturbModeCallbackInfo_.env, disturbModeCallbackInfo_.ref);
    }
    if (enabledNotificationCallbackInfo_.ref != nullptr) {
        napi_delete_reference(enabledNotificationCallbackInfo_.env, enabledNotificationCallbackInfo_.ref);
    }
}

void UvQueueWorkOnCanceled(uv_work_t *work, int status)
{
    ANS_LOGI("OnCanceled uv_work_t start");

    if (work == nullptr) {
        ANS_LOGE("work is null");
        return;
    }

    auto dataWorkerData = reinterpret_cast<NotificationReceiveDataWorker *>(work->data);
    if (dataWorkerData == nullptr) {
        ANS_LOGE("dataWorkerData is null");
        delete work;
        work = nullptr;
        return;
    }

    napi_value result = nullptr;
    napi_handle_scope scope;
    napi_open_handle_scope(dataWorkerData->env, &scope);
    napi_create_object(dataWorkerData->env, &result);
    if (!SetSubscribeCallbackData(dataWorkerData->env,
        dataWorkerData->request,
        dataWorkerData->sortingMap,
        dataWorkerData->deleteReason,
        result)) {
        ANS_LOGE("Failed to convert data to JS");
    } else {
        Common::SetCallback(dataWorkerData->env, dataWorkerData->ref, result);
    }
    napi_close_handle_scope(dataWorkerData->env, scope);

    delete dataWorkerData;
    dataWorkerData = nullptr;
    delete work;
}

void SubscriberInstance::OnCanceled(const std::shared_ptr<OHOS::Notification::Notification> &request,
    const std::shared_ptr<NotificationSortingMap> &sortingMap, int32_t deleteReason)
{
    ANS_LOGI("enter");

    if (canceCallbackInfo_.ref == nullptr) {
        ANS_LOGI("cancel callback unset");
        return;
    }

    if (request == nullptr) {
        ANS_LOGE("request is null");
        return;
    }

    if (sortingMap == nullptr) {
        ANS_LOGE("sortingMap is null");
        return;
    }
    ANS_LOGI("OnCanceled NotificationId = %{public}d", request->GetNotificationRequest().GetNotificationId());
    ANS_LOGI("OnCanceled sortingMap size = %{public}zu", sortingMap->GetKey().size());
    ANS_LOGI("OnCanceled deleteReason = %{public}d", deleteReason);

    uv_loop_s *loop = nullptr;
    napi_get_uv_event_loop(canceCallbackInfo_.env, &loop);
    if (loop == nullptr) {
        ANS_LOGE("loop instance is nullptr");
        return;
    }

    NotificationReceiveDataWorker *dataWorker = new (std::nothrow) NotificationReceiveDataWorker();
    if (dataWorker == nullptr) {
        ANS_LOGE("new dataWorker failed");
        return;
    }

    dataWorker->request = request;
    dataWorker->sortingMap = sortingMap;
    dataWorker->deleteReason = deleteReason;
    dataWorker->env = canceCallbackInfo_.env;
    dataWorker->ref = canceCallbackInfo_.ref;

    uv_work_t *work = new (std::nothrow) uv_work_t;
    if (work == nullptr) {
        ANS_LOGE("new work failed");
        delete dataWorker;
        dataWorker = nullptr;
        return;
    }

    work->data = reinterpret_cast<void *>(dataWorker);

    int ret = uv_queue_work(loop, work, [](uv_work_t *work) {}, UvQueueWorkOnCanceled);
    if (ret != 0) {
        delete dataWorker;
        dataWorker = nullptr;
        delete work;
        work = nullptr;
    }
}

void SubscriberInstance::OnConsumed(const std::shared_ptr<OHOS::Notification::Notification> &request)
{}

void UvQueueWorkOnConsumed(uv_work_t *work, int status)
{
    ANS_LOGI("OnConsumed uv_work_t start");

    if (work == nullptr) {
        ANS_LOGE("work is null");
        return;
    }

    auto dataWorkerData = reinterpret_cast<NotificationReceiveDataWorker *>(work->data);
    if (dataWorkerData == nullptr) {
        ANS_LOGE("dataWorkerData is null");
        delete work;
        work = nullptr;
        return;
    }
    napi_value result = nullptr;
    napi_handle_scope scope;
    napi_open_handle_scope(dataWorkerData->env, &scope);
    napi_create_object(dataWorkerData->env, &result);
    if (!SetSubscribeCallbackData(dataWorkerData->env,
        dataWorkerData->request,
        dataWorkerData->sortingMap,
        NO_DELETE_REASON,
        result)) {
        ANS_LOGE("Failed to convert data to JS");
    } else {
        Common::SetCallback(dataWorkerData->env, dataWorkerData->ref, result);
    }
    napi_close_handle_scope(dataWorkerData->env, scope);

    delete dataWorkerData;
    dataWorkerData = nullptr;
    delete work;
}

void SubscriberInstance::OnConsumed(const std::shared_ptr<OHOS::Notification::Notification> &request,
    const std::shared_ptr<NotificationSortingMap> &sortingMap)
{
    ANS_LOGI("enter");

    if (consumeCallbackInfo_.ref == nullptr) {
        ANS_LOGI("consume callback unset");
        return;
    }

    if (request == nullptr) {
        ANS_LOGE("request is null");
        return;
    }

    if (sortingMap == nullptr) {
        ANS_LOGE("sortingMap is null");
        return;
    }
    ANS_LOGI("OnConsumed Notification key = %{public}s, sortingMap size = %{public}zu",
        request->GetKey().c_str(), sortingMap->GetKey().size());

    uv_loop_s *loop = nullptr;
    napi_get_uv_event_loop(consumeCallbackInfo_.env, &loop);
    if (loop == nullptr) {
        ANS_LOGE("loop instance is nullptr");
        return;
    }

    NotificationReceiveDataWorker *dataWorker = new (std::nothrow) NotificationReceiveDataWorker();
    if (dataWorker == nullptr) {
        ANS_LOGE("new dataWorker failed");
        return;
    }

    dataWorker->request = request;
    dataWorker->sortingMap = sortingMap;
    dataWorker->env = consumeCallbackInfo_.env;
    dataWorker->ref = consumeCallbackInfo_.ref;

    uv_work_t *work = new (std::nothrow) uv_work_t;
    if (work == nullptr) {
        ANS_LOGE("new work failed");
        delete dataWorker;
        dataWorker = nullptr;
        return;
    }

    work->data = reinterpret_cast<void *>(dataWorker);

    int ret = uv_queue_work(loop, work, [](uv_work_t *work) {}, UvQueueWorkOnConsumed);
    if (ret != 0) {
        delete dataWorker;
        dataWorker = nullptr;
        delete work;
        work = nullptr;
    }
}

void UvQueueWorkOnUpdate(uv_work_t *work, int status)
{
    ANS_LOGI("OnUpdate uv_work_t start");

    if (work == nullptr) {
        ANS_LOGE("work is null");
        return;
    }
    auto dataWorkerData = reinterpret_cast<NotificationReceiveDataWorker *>(work->data);
    if (dataWorkerData == nullptr) {
        ANS_LOGE("dataWorkerData is null");
        delete work;
        work = nullptr;
        return;
    }
    napi_value result = nullptr;
    napi_handle_scope scope;
    napi_open_handle_scope(dataWorkerData->env, &scope);
    napi_create_object(dataWorkerData->env, &result);
    if (!Common::SetNotificationSortingMap(dataWorkerData->env, dataWorkerData->sortingMap, result)) {
        ANS_LOGE("Failed to convert data to JS");
    } else {
        Common::SetCallback(dataWorkerData->env, dataWorkerData->ref, result);
    }
    napi_close_handle_scope(dataWorkerData->env, scope);

    delete dataWorkerData;
    dataWorkerData = nullptr;
    delete work;
}

void SubscriberInstance::OnUpdate(const std::shared_ptr<NotificationSortingMap> &sortingMap)
{
    ANS_LOGI("enter");

    if (updateCallbackInfo_.ref == nullptr) {
        ANS_LOGI("update callback unset");
        return;
    }

    if (sortingMap == nullptr) {
        ANS_LOGE("sortingMap is null");
        return;
    }
    ANS_LOGI("OnUpdate sortingMap size = %{public}zu", sortingMap->GetKey().size());

    uv_loop_s *loop = nullptr;
    napi_get_uv_event_loop(updateCallbackInfo_.env, &loop);
    if (loop == nullptr) {
        ANS_LOGE("loop instance is nullptr");
        return;
    }

    NotificationReceiveDataWorker *dataWorker = new (std::nothrow) NotificationReceiveDataWorker();
    if (dataWorker == nullptr) {
        ANS_LOGE("new dataWorker failed");
        return;
    }

    dataWorker->sortingMap = sortingMap;
    dataWorker->env = updateCallbackInfo_.env;
    dataWorker->ref = updateCallbackInfo_.ref;

    uv_work_t *work = new (std::nothrow) uv_work_t;
    if (work == nullptr) {
        ANS_LOGE("new work failed");
        delete dataWorker;
        dataWorker = nullptr;
        return;
    }

    work->data = reinterpret_cast<void *>(dataWorker);

    int ret = uv_queue_work(loop, work, [](uv_work_t *work) {}, UvQueueWorkOnUpdate);
    if (ret != 0) {
        delete dataWorker;
        dataWorker = nullptr;
        delete work;
        work = nullptr;
    }
}

void UvQueueWorkOnConnected(uv_work_t *work, int status)
{
    ANS_LOGI("OnConnected uv_work_t start");

    if (work == nullptr) {
        ANS_LOGE("work is null");
        return;
    }

    auto dataWorkerData = reinterpret_cast<NotificationReceiveDataWorker *>(work->data);
    if (dataWorkerData == nullptr) {
        ANS_LOGE("dataWorkerData is null");
        delete work;
        work = nullptr;
        return;
    }

    Common::SetCallback(dataWorkerData->env, dataWorkerData->ref, Common::NapiGetNull(dataWorkerData->env));

    delete dataWorkerData;
    dataWorkerData = nullptr;
    delete work;
}

void SubscriberInstance::OnConnected()
{
    ANS_LOGI("enter");

    if (subscribeCallbackInfo_.ref == nullptr) {
        ANS_LOGI("subscribe callback unset");
        return;
    }

    uv_loop_s *loop = nullptr;
    napi_get_uv_event_loop(subscribeCallbackInfo_.env, &loop);
    if (loop == nullptr) {
        ANS_LOGE("loop instance is nullptr");
        return;
    }

    NotificationReceiveDataWorker *dataWorker = new (std::nothrow) NotificationReceiveDataWorker();
    if (dataWorker == nullptr) {
        ANS_LOGE("new dataWorker failed");
        return;
    }

    dataWorker->env = subscribeCallbackInfo_.env;
    dataWorker->ref = subscribeCallbackInfo_.ref;

    uv_work_t *work = new (std::nothrow) uv_work_t;
    if (work == nullptr) {
        ANS_LOGE("new work failed");
        delete dataWorker;
        dataWorker = nullptr;
        return;
    }

    work->data = reinterpret_cast<void *>(dataWorker);

    int ret = uv_queue_work(loop, work, [](uv_work_t *work) {}, UvQueueWorkOnConnected);
    if (ret != 0) {
        delete dataWorker;
        dataWorker = nullptr;
        delete work;
        work = nullptr;
    }
}

void UvQueueWorkOnDisconnected(uv_work_t *work, int status)
{
    ANS_LOGI("OnDisconnected uv_work_t start");

    if (work == nullptr) {
        ANS_LOGE("work is null");
        return;
    }

    auto dataWorkerData = reinterpret_cast<NotificationReceiveDataWorker *>(work->data);
    if (dataWorkerData == nullptr) {
        ANS_LOGE("dataWorkerData is null");
        delete work;
        work = nullptr;
        return;
    }

    Common::SetCallback(dataWorkerData->env, dataWorkerData->ref, Common::NapiGetNull(dataWorkerData->env));

    DelSubscriberInstancesInfo(dataWorkerData->env, dataWorkerData->subscriber);

    delete dataWorkerData;
    dataWorkerData = nullptr;
    delete work;
}

void SubscriberInstance::OnDisconnected()
{
    ANS_LOGI("enter");

    if (unsubscribeCallbackInfo_.ref == nullptr) {
        ANS_LOGI("unsubscribe callback unset");
        return;
    }

    uv_loop_s *loop = nullptr;
    napi_get_uv_event_loop(unsubscribeCallbackInfo_.env, &loop);
    if (loop == nullptr) {
        ANS_LOGE("loop instance is nullptr");
        return;
    }

    NotificationReceiveDataWorker *dataWorker = new (std::nothrow) NotificationReceiveDataWorker();
    if (dataWorker == nullptr) {
        ANS_LOGE("new dataWorker failed");
        return;
    }

    dataWorker->env = unsubscribeCallbackInfo_.env;
    dataWorker->ref = unsubscribeCallbackInfo_.ref;
    dataWorker->subscriber = this;

    uv_work_t *work = new (std::nothrow) uv_work_t;
    if (work == nullptr) {
        ANS_LOGE("new work failed");
        delete dataWorker;
        dataWorker = nullptr;
        return;
    }

    work->data = reinterpret_cast<void *>(dataWorker);

    int ret = uv_queue_work(loop, work, [](uv_work_t *work) {}, UvQueueWorkOnDisconnected);
    if (ret != 0) {
        delete dataWorker;
        dataWorker = nullptr;
        delete work;
        work = nullptr;
    }
}

void UvQueueWorkOnDied(uv_work_t *work, int status)
{
    ANS_LOGI("OnDied uv_work_t start");

    if (work == nullptr) {
        ANS_LOGE("work is null");
        return;
    }
    auto dataWorkerData = reinterpret_cast<NotificationReceiveDataWorker *>(work->data);
    if (dataWorkerData == nullptr) {
        ANS_LOGE("dataWorkerData is null");
        delete work;
        work = nullptr;
        return;
    }

    Common::SetCallback(
        dataWorkerData->env, dataWorkerData->ref, Common::NapiGetNull(dataWorkerData->env));

    delete dataWorkerData;
    dataWorkerData = nullptr;
    delete work;
}

void SubscriberInstance::OnDied()
{
    ANS_LOGI("enter");

    if (dieCallbackInfo_.ref == nullptr) {
        ANS_LOGE("die callback unset");
        return;
    }

    uv_loop_s *loop = nullptr;
    napi_get_uv_event_loop(dieCallbackInfo_.env, &loop);
    if (loop == nullptr) {
        ANS_LOGE("loop instance is nullptr");
        return;
    }

    NotificationReceiveDataWorker *dataWorker = new (std::nothrow) NotificationReceiveDataWorker();
    if (dataWorker == nullptr) {
        ANS_LOGE("new dataWorker failed");
        return;
    }

    dataWorker->env = dieCallbackInfo_.env;
    dataWorker->ref = dieCallbackInfo_.ref;

    uv_work_t *work = new (std::nothrow) uv_work_t;
    if (work == nullptr) {
        ANS_LOGE("new work failed");
        delete dataWorker;
        dataWorker = nullptr;
        return;
    }

    work->data = reinterpret_cast<void *>(dataWorker);

    int ret = uv_queue_work(loop, work, [](uv_work_t *work) {}, UvQueueWorkOnDied);
    if (ret != 0) {
        delete dataWorker;
        dataWorker = nullptr;
        delete work;
        work = nullptr;
    }
}

void UvQueueWorkOnDoNotDisturbDateChange(uv_work_t *work, int status)
{
    ANS_LOGI("OnDoNotDisturbDateChange uv_work_t start");

    if (work == nullptr) {
        ANS_LOGE("work is null");
        return;
    }

    auto dataWorkerData = reinterpret_cast<NotificationReceiveDataWorker *>(work->data);
    if (dataWorkerData == nullptr) {
        ANS_LOGE("dataWorkerData is null");
        delete work;
        work = nullptr;
        return;
    }

    napi_value result = nullptr;
    napi_handle_scope scope;
    napi_open_handle_scope(dataWorkerData->env, &scope);
    napi_create_object(dataWorkerData->env, &result);

    if (!Common::SetDoNotDisturbDate(dataWorkerData->env, dataWorkerData->date, result)) {
        result = Common::NapiGetNull(dataWorkerData->env);
    }

    Common::SetCallback(dataWorkerData->env, dataWorkerData->ref, result);
    napi_close_handle_scope(dataWorkerData->env, scope);

    delete dataWorkerData;
    dataWorkerData = nullptr;
    delete work;
}

void SubscriberInstance::OnDoNotDisturbDateChange(const std::shared_ptr<NotificationDoNotDisturbDate> &date)
{
    ANS_LOGI("enter");

    if (disturbDateCallbackInfo_.ref == nullptr) {
        ANS_LOGI("disturbDateCallbackInfo_ callback unset");
        return;
    }

    if (date == nullptr) {
        ANS_LOGE("date is null");
        return;
    }

    uv_loop_s *loop = nullptr;
    napi_get_uv_event_loop(disturbDateCallbackInfo_.env, &loop);
    if (loop == nullptr) {
        ANS_LOGE("loop instance is nullptr");
        return;
    }

    NotificationReceiveDataWorker *dataWorker = new (std::nothrow) NotificationReceiveDataWorker();
    if (dataWorker == nullptr) {
        ANS_LOGE("new dataWorker failed");
        return;
    }

    dataWorker->date = *date;
    dataWorker->env = disturbDateCallbackInfo_.env;
    dataWorker->ref = disturbDateCallbackInfo_.ref;

    uv_work_t *work = new (std::nothrow) uv_work_t;
    if (work == nullptr) {
        ANS_LOGE("new work failed");
        delete dataWorker;
        dataWorker = nullptr;
        return;
    }

    work->data = reinterpret_cast<void *>(dataWorker);

    int ret = uv_queue_work(loop, work, [](uv_work_t *work) {}, UvQueueWorkOnDoNotDisturbDateChange);
    if (ret != 0) {
        delete dataWorker;
        dataWorker = nullptr;
        delete work;
        work = nullptr;
    }
}

void UvQueueWorkOnEnabledNotificationChanged(uv_work_t *work, int status)
{
    ANS_LOGI("OnEnabledNotificationChanged uv_work_t start");

    if (work == nullptr) {
        ANS_LOGE("work is null");
        return;
    }

    auto dataWorkerData = reinterpret_cast<NotificationReceiveDataWorker *>(work->data);
    if (dataWorkerData == nullptr) {
        ANS_LOGE("dataWorkerData is null");
        delete work;
        work = nullptr;
        return;
    }

    napi_value result = nullptr;
    napi_handle_scope scope;
    napi_open_handle_scope(dataWorkerData->env, &scope);
    napi_create_object(dataWorkerData->env, &result);

    if (!Common::SetEnabledNotificationCallbackData(dataWorkerData->env, dataWorkerData->callbackData, result)) {
        result = Common::NapiGetNull(dataWorkerData->env);
    }

    Common::SetCallback(dataWorkerData->env, dataWorkerData->ref, result);
    napi_close_handle_scope(dataWorkerData->env, scope);

    delete dataWorkerData;
    dataWorkerData = nullptr;
    delete work;
}

void SubscriberInstance::OnEnabledNotificationChanged(
    const std::shared_ptr<EnabledNotificationCallbackData> &callbackData)
{
    ANS_LOGI("enter");

    if (enabledNotificationCallbackInfo_.ref == nullptr) {
        ANS_LOGI("enabledNotificationCallbackInfo_ callback unset");
        return;
    }

    if (callbackData == nullptr) {
        ANS_LOGE("callbackData is null");
        return;
    }

    uv_loop_s *loop = nullptr;
    napi_get_uv_event_loop(enabledNotificationCallbackInfo_.env, &loop);
    if (loop == nullptr) {
        ANS_LOGE("loop instance is nullptr");
        return;
    }

    NotificationReceiveDataWorker *dataWorker = new (std::nothrow) NotificationReceiveDataWorker();
    if (dataWorker == nullptr) {
        ANS_LOGE("new dataWorker failed");
        return;
    }

    dataWorker->callbackData = *callbackData;
    dataWorker->env = enabledNotificationCallbackInfo_.env;
    dataWorker->ref = enabledNotificationCallbackInfo_.ref;

    uv_work_t *work = new (std::nothrow) uv_work_t;
    if (work == nullptr) {
        ANS_LOGE("new work failed");
        delete dataWorker;
        dataWorker = nullptr;
        return;
    }

    work->data = reinterpret_cast<void *>(dataWorker);

    int ret = uv_queue_work(loop, work, [](uv_work_t *work) {}, UvQueueWorkOnEnabledNotificationChanged);
    if (ret != 0) {
        delete dataWorker;
        dataWorker = nullptr;
        delete work;
        work = nullptr;
    }
}

void SubscriberInstance::SetCancelCallbackInfo(const napi_env &env, const napi_ref &ref)
{
    canceCallbackInfo_.env = env;
    canceCallbackInfo_.ref = ref;
}

void SubscriberInstance::SetConsumeCallbackInfo(const napi_env &env, const napi_ref &ref)
{
    consumeCallbackInfo_.env = env;
    consumeCallbackInfo_.ref = ref;
}

void SubscriberInstance::SetUpdateCallbackInfo(const napi_env &env, const napi_ref &ref)
{
    updateCallbackInfo_.env = env;
    updateCallbackInfo_.ref = ref;
}

void SubscriberInstance::SetSubscribeCallbackInfo(const napi_env &env, const napi_ref &ref)
{
    subscribeCallbackInfo_.env = env;
    subscribeCallbackInfo_.ref = ref;
}

void SubscriberInstance::SetUnsubscribeCallbackInfo(const napi_env &env, const napi_ref &ref)
{
    unsubscribeCallbackInfo_.env = env;
    unsubscribeCallbackInfo_.ref = ref;
}

void SubscriberInstance::SetDieCallbackInfo(const napi_env &env, const napi_ref &ref)
{
    dieCallbackInfo_.env = env;
    dieCallbackInfo_.ref = ref;
}

void SubscriberInstance::SetDisturbModeCallbackInfo(const napi_env &env, const napi_ref &ref)
{
    disturbModeCallbackInfo_.env = env;
    disturbModeCallbackInfo_.ref = ref;
}

void SubscriberInstance::SetEnabledNotificationCallbackInfo(const napi_env &env, const napi_ref &ref)
{
    enabledNotificationCallbackInfo_.env = env;
    enabledNotificationCallbackInfo_.ref = ref;
}

void SubscriberInstance::SetDisturbDateCallbackInfo(const napi_env &env, const napi_ref &ref)
{
    disturbDateCallbackInfo_.env = env;
    disturbDateCallbackInfo_.ref = ref;
}

void SubscriberInstance::SetCallbackInfo(const napi_env &env, const std::string &type, const napi_ref &ref)
{
    if (type == CONSUME) {
        SetConsumeCallbackInfo(env, ref);
    } else if (type == CANCEL) {
        SetCancelCallbackInfo(env, ref);
    } else if (type == UPDATE) {
        SetUpdateCallbackInfo(env, ref);
    } else if (type == CONNECTED) {
        SetSubscribeCallbackInfo(env, ref);
    } else if (type == DIS_CONNECTED) {
        SetUnsubscribeCallbackInfo(env, ref);
    } else if (type == DIE) {
        SetDieCallbackInfo(env, ref);
    } else if (type == DISTURB_MODE_CHANGE) {
        SetDisturbModeCallbackInfo(env, ref);
    } else if (type == DISTURB_DATE_CHANGE) {
        SetDisturbDateCallbackInfo(env, ref);
    } else if (type == ENABLE_NOTIFICATION_CHANGED) {
        SetEnabledNotificationCallbackInfo(env, ref);
    } else {
        ANS_LOGW("type is error");
    }
}

bool HasNotificationSubscriber(const napi_env &env, const napi_value &value, SubscriberInstancesInfo &subscriberInfo)
{
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto vec : subscriberInstances_) {
        napi_value callback = nullptr;
        napi_get_reference_value(env, vec.ref, &callback);
        bool isEquals = false;
        napi_strict_equals(env, value, callback, &isEquals);
        if (isEquals) {
            subscriberInfo = vec;
            return true;
        }
    }
    return false;
}

napi_value GetNotificationSubscriber(
    const napi_env &env, const napi_value &value, SubscriberInstancesInfo &subscriberInfo)
{
    ANS_LOGI("enter");
    bool hasProperty = false;
    napi_valuetype valuetype = napi_undefined;
    napi_ref result = nullptr;

    subscriberInfo.subscriber = new (std::nothrow) SubscriberInstance();
    if (subscriberInfo.subscriber == nullptr) {
        ANS_LOGE("subscriber is null");
        return nullptr;
    }

    napi_create_reference(env, value, 1, &subscriberInfo.ref);

    // onConsume?:(data: SubscribeCallbackData) => void
    NAPI_CALL(env, napi_has_named_property(env, value, "onConsume", &hasProperty));
    if (hasProperty) {
        napi_value nOnConsumed = nullptr;
        napi_get_named_property(env, value, "onConsume", &nOnConsumed);
        NAPI_CALL(env, napi_typeof(env, nOnConsumed, &valuetype));
        if (valuetype != napi_function) {
            ANS_LOGE("Wrong argument type. Function expected.");
            return nullptr;
        }
        napi_create_reference(env, nOnConsumed, 1, &result);
        subscriberInfo.subscriber->SetCallbackInfo(env, CONSUME, result);
    }
    // onCancel?:(data: SubscribeCallbackData) => void
    NAPI_CALL(env, napi_has_named_property(env, value, "onCancel", &hasProperty));
    if (hasProperty) {
        napi_value nOnCanceled = nullptr;
        napi_get_named_property(env, value, "onCancel", &nOnCanceled);
        NAPI_CALL(env, napi_typeof(env, nOnCanceled, &valuetype));
        if (valuetype != napi_function) {
            ANS_LOGE("Wrong argument type. Function expected.");
            return nullptr;
        }
        napi_create_reference(env, nOnCanceled, 1, &result);
        subscriberInfo.subscriber->SetCallbackInfo(env, CANCEL, result);
    }
    // onUpdate?:(data: NotificationSortingMap) => void
    NAPI_CALL(env, napi_has_named_property(env, value, "onUpdate", &hasProperty));
    if (hasProperty) {
        napi_value nOnUpdate = nullptr;
        napi_get_named_property(env, value, "onUpdate", &nOnUpdate);
        NAPI_CALL(env, napi_typeof(env, nOnUpdate, &valuetype));
        if (valuetype != napi_function) {
            ANS_LOGE("Wrong argument type. Function expected.");
            return nullptr;
        }
        napi_create_reference(env, nOnUpdate, 1, &result);
        subscriberInfo.subscriber->SetCallbackInfo(env, UPDATE, result);
    }
    // onConnect?:() => void
    NAPI_CALL(env, napi_has_named_property(env, value, "onConnect", &hasProperty));
    if (hasProperty) {
        napi_value nOnConnected = nullptr;
        napi_get_named_property(env, value, "onConnect", &nOnConnected);
        NAPI_CALL(env, napi_typeof(env, nOnConnected, &valuetype));
        if (valuetype != napi_function) {
            ANS_LOGE("Wrong argument type. Function expected.");
            return nullptr;
        }
        napi_create_reference(env, nOnConnected, 1, &result);
        subscriberInfo.subscriber->SetCallbackInfo(env, CONNECTED, result);
    }
    // onDisconnect?:() => void
    NAPI_CALL(env, napi_has_named_property(env, value, "onDisconnect", &hasProperty));
    if (hasProperty) {
        napi_value nOnDisConnect = nullptr;
        napi_get_named_property(env, value, "onDisconnect", &nOnDisConnect);
        NAPI_CALL(env, napi_typeof(env, nOnDisConnect, &valuetype));
        if (valuetype != napi_function) {
            ANS_LOGE("Wrong argument type. Function expected.");
            return nullptr;
        }
        napi_create_reference(env, nOnDisConnect, 1, &result);
        subscriberInfo.subscriber->SetCallbackInfo(env, DIS_CONNECTED, result);
    }
    // onDestroy?:() => void
    NAPI_CALL(env, napi_has_named_property(env, value, "onDestroy", &hasProperty));
    if (hasProperty) {
        napi_value nOnDied = nullptr;
        napi_get_named_property(env, value, "onDestroy", &nOnDied);
        NAPI_CALL(env, napi_typeof(env, nOnDied, &valuetype));
        if (valuetype != napi_function) {
            ANS_LOGE("Wrong argument type. Function expected.");
            return nullptr;
        }
        napi_create_reference(env, nOnDied, 1, &result);
        subscriberInfo.subscriber->SetCallbackInfo(env, DIE, result);
    }
    // onDisturbModeChange?:(mode: notification.DoNotDisturbMode) => void
    NAPI_CALL(env, napi_has_named_property(env, value, "onDisturbModeChange", &hasProperty));
    if (hasProperty) {
        napi_value nOnDisturbModeChanged = nullptr;
        napi_get_named_property(env, value, "onDisturbModeChange", &nOnDisturbModeChanged);
        NAPI_CALL(env, napi_typeof(env, nOnDisturbModeChanged, &valuetype));
        if (valuetype != napi_function) {
            ANS_LOGE("Wrong argument type. Function expected.");
            return nullptr;
        }
        napi_create_reference(env, nOnDisturbModeChanged, 1, &result);
        subscriberInfo.subscriber->SetCallbackInfo(env, DISTURB_MODE_CHANGE, result);
    }

    // onDoNotDisturbDateChange?:(mode: notification.DoNotDisturbDate) => void
    NAPI_CALL(env, napi_has_named_property(env, value, "onDoNotDisturbDateChange", &hasProperty));
    if (hasProperty) {
        napi_value nOnDisturbDateChanged = nullptr;
        napi_get_named_property(env, value, "onDoNotDisturbDateChange", &nOnDisturbDateChanged);
        NAPI_CALL(env, napi_typeof(env, nOnDisturbDateChanged, &valuetype));
        if (valuetype != napi_function) {
            ANS_LOGE("Wrong argument type. Function expected.");
            return nullptr;
        }
        napi_create_reference(env, nOnDisturbDateChanged, 1, &result);
        subscriberInfo.subscriber->SetCallbackInfo(env, DISTURB_DATE_CHANGE, result);
    }

    // onEnabledNotificationChanged?:(data: notification.EnabledNotificationCallbackData) => void
    NAPI_CALL(env, napi_has_named_property(env, value, "onEnabledNotificationChanged", &hasProperty));
    if (hasProperty) {
        napi_value nOnEnabledNotificationChanged = nullptr;
        napi_get_named_property(env, value, "onEnabledNotificationChanged", &nOnEnabledNotificationChanged);
        NAPI_CALL(env, napi_typeof(env, nOnEnabledNotificationChanged, &valuetype));
        if (valuetype != napi_function) {
            ANS_LOGE("Wrong argument type. Function expected.");
            return nullptr;
        }
        napi_create_reference(env, nOnEnabledNotificationChanged, 1, &result);
        subscriberInfo.subscriber->SetCallbackInfo(env, ENABLE_NOTIFICATION_CHANGED, result);
    }

    return Common::NapiGetNull(env);
}

bool AddSubscriberInstancesInfo(const napi_env &env, const SubscriberInstancesInfo &subscriberInfo)
{
    ANS_LOGI("enter");
    if (subscriberInfo.ref == nullptr) {
        ANS_LOGE("subscriberInfo.ref is null");
        return false;
    }
    if (subscriberInfo.subscriber == nullptr) {
        ANS_LOGE("subscriberInfo.subscriber is null");
        return false;
    }
    std::lock_guard<std::mutex> lock(mutex_);
    subscriberInstances_.emplace_back(subscriberInfo);

    return true;
}

bool DelSubscriberInstancesInfo(const napi_env &env, const SubscriberInstance *subscriber)
{
    ANS_LOGI("enter");
    if (subscriber == nullptr) {
        ANS_LOGE("subscriber is null");
        return false;
    }

    std::lock_guard<std::mutex> lock(mutex_);
    for (auto it = subscriberInstances_.begin(); it != subscriberInstances_.end(); ++it) {
        if ((*it).subscriber == subscriber) {
            if ((*it).ref != nullptr) {
                napi_delete_reference(env, (*it).ref);
            }
            DelDeletingSubscriber((*it).subscriber);
            delete (*it).subscriber;
            (*it).subscriber = nullptr;
            subscriberInstances_.erase(it);
            return true;
        }
    }
    return false;
}
napi_value ParseParameters(const napi_env &env, const napi_callback_info &info,
    NotificationSubscribeInfo &subscriberInfo, SubscriberInstance *&subscriber, napi_ref &callback)
{
    ANS_LOGI("enter");

    size_t argc = SUBSRIBE_MAX_PARA;
    napi_value argv[SUBSRIBE_MAX_PARA] = {nullptr};
    napi_value thisVar = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &thisVar, NULL));
    if (argc < 1) {
        ANS_LOGE("Wrong number of arguments");
        return nullptr;
    }

    napi_valuetype valuetype = napi_undefined;

    // argv[0]:subscriber
    NAPI_CALL(env, napi_typeof(env, argv[PARAM0], &valuetype));
    if (valuetype != napi_object) {
        ANS_LOGE("Wrong argument type for arg0. NotificationSubscriber object expected.");
        return nullptr;
    }

    SubscriberInstancesInfo subscriberInstancesInfo;
    if (!HasNotificationSubscriber(env, argv[PARAM0], subscriberInstancesInfo)) {
        if (GetNotificationSubscriber(env, argv[PARAM0], subscriberInstancesInfo) == nullptr) {
            ANS_LOGE("NotificationSubscriber parse failed");
            if (subscriberInstancesInfo.subscriber) {
                delete subscriberInstancesInfo.subscriber;
                subscriberInstancesInfo.subscriber = nullptr;
            }
            return nullptr;
        }
        if (!AddSubscriberInstancesInfo(env, subscriberInstancesInfo)) {
            ANS_LOGE("AddSubscriberInstancesInfo add failed");
            if (subscriberInstancesInfo.subscriber) {
                delete subscriberInstancesInfo.subscriber;
                subscriberInstancesInfo.subscriber = nullptr;
            }
            return nullptr;
        }
    }
    subscriber = subscriberInstancesInfo.subscriber;

    // argv[1]:callback or NotificationSubscribeInfo
    if (argc >= SUBSRIBE_MAX_PARA - 1) {
        NAPI_CALL(env, napi_typeof(env, argv[PARAM1], &valuetype));
        if ((valuetype != napi_function) && (valuetype != napi_object)) {
            ANS_LOGE("Wrong argument type for arg1. Function or NotificationSubscribeInfo object expected.");
            return nullptr;
        }
        if (valuetype == napi_function) {
            napi_create_reference(env, argv[PARAM1], 1, &callback);
        } else {
            if (Common::GetNotificationSubscriberInfo(env, argv[PARAM1], subscriberInfo) == nullptr) {
                ANS_LOGE("NotificationSubscribeInfo parse failed");
                return nullptr;
            }
        }
    }

    // argv[2]:callback
    if (argc >= SUBSRIBE_MAX_PARA) {
        NAPI_CALL(env, napi_typeof(env, argv[PARAM2], &valuetype));
        if (valuetype != napi_function) {
            ANS_LOGE("Wrong argument type. Function expected.");
            return nullptr;
        }
        napi_create_reference(env, argv[PARAM2], 1, &callback);
    }

    return Common::NapiGetNull(env);
}

napi_value Subscribe(napi_env env, napi_callback_info info)
{
    ANS_LOGI("enter");

    napi_ref callback = nullptr;
    SubscriberInstance *objectInfo = nullptr;
    NotificationSubscribeInfo subscriberInfo;
    if (ParseParameters(env, info, subscriberInfo, objectInfo, callback) == nullptr) {
        if (objectInfo) {
            delete objectInfo;
            objectInfo = nullptr;
        }
        return Common::NapiGetUndefined(env);
    }

    AsyncCallbackInfoSubscribe *asynccallbackinfo = new (std::nothrow) AsyncCallbackInfoSubscribe {
        .env = env, .asyncWork = nullptr, .objectInfo = objectInfo, .subscriberInfo = subscriberInfo
    };
    if (!asynccallbackinfo) {
        if (objectInfo) {
            delete objectInfo;
            objectInfo = nullptr;
        }
        return Common::JSParaError(env, callback);
    }
    napi_value promise = nullptr;
    Common::PaddingCallbackPromiseInfo(env, callback, asynccallbackinfo->info, promise);

    napi_value resourceName = nullptr;
    napi_create_string_latin1(env, "subscribeNotification", NAPI_AUTO_LENGTH, &resourceName);
    // Asynchronous function call
    napi_create_async_work(env,
        nullptr,
        resourceName,
        [](napi_env env, void *data) {
            ANS_LOGI("Subscribe napi_create_async_work start");
            if (!data) {
                ANS_LOGE("Invalid asynccallbackinfo!");
                return;
            }
            auto asynccallbackinfo = reinterpret_cast<AsyncCallbackInfoSubscribe *>(data);
            if (asynccallbackinfo) {
                if (asynccallbackinfo->subscriberInfo.hasSubscribeInfo) {
                    ANS_LOGI("Subscribe with NotificationSubscribeInfo");
                    OHOS::Notification::NotificationSubscribeInfo subscribeInfo;
                    subscribeInfo.AddAppNames(asynccallbackinfo->subscriberInfo.bundleNames);
                    subscribeInfo.AddAppUserId(asynccallbackinfo->subscriberInfo.userId);
                    asynccallbackinfo->info.errorCode =
                        NotificationHelper::SubscribeNotification(*(asynccallbackinfo->objectInfo), subscribeInfo);
                } else {
                    asynccallbackinfo->info.errorCode =
                        NotificationHelper::SubscribeNotification(*(asynccallbackinfo->objectInfo));
                }
            }
        },
        [](napi_env env, napi_status status, void *data) {
            ANS_LOGI("Subscribe napi_create_async_work end");
            if (!data) {
                ANS_LOGE("Invalid asynccallbackinfo!");
                return;
            }
            auto asynccallbackinfo = reinterpret_cast<AsyncCallbackInfoSubscribe *>(data);
            if (asynccallbackinfo) {
                Common::ReturnCallbackPromise(env, asynccallbackinfo->info, Common::NapiGetNull(env));

                if (asynccallbackinfo->info.callback != nullptr) {
                    napi_delete_reference(env, asynccallbackinfo->info.callback);
                }
                napi_delete_async_work(env, asynccallbackinfo->asyncWork);

                delete asynccallbackinfo;
                asynccallbackinfo = nullptr;
            }
        },
        (void *)asynccallbackinfo,
        &asynccallbackinfo->asyncWork);

    napi_status status = napi_queue_async_work(env, asynccallbackinfo->asyncWork);
    if (status != napi_ok) {
        ANS_LOGE("napi_queue_async_work failed return: %{public}d", status);
        if (asynccallbackinfo->info.callback != nullptr) {
            napi_delete_reference(env, asynccallbackinfo->info.callback);
        }
        napi_delete_async_work(env, asynccallbackinfo->asyncWork);
        delete asynccallbackinfo;
        asynccallbackinfo = nullptr;
        return Common::JSParaError(env, callback);
    }

    if (asynccallbackinfo->info.isCallback) {
        return Common::NapiGetNull(env);
    } else {
        return promise;
    }
}

bool AddDeletingSubscriber(SubscriberInstance *subscriber)
{
    std::lock_guard<std::mutex> lock(delMutex_);
    auto iter = std::find(DeletingSubscriber.begin(), DeletingSubscriber.end(), subscriber);
    if (iter != DeletingSubscriber.end()) {
        return false;
    }

    DeletingSubscriber.push_back(subscriber);
    return true;
}

void DelDeletingSubscriber(SubscriberInstance *subscriber)
{
    std::lock_guard<std::mutex> lock(delMutex_);
    auto iter = std::find(DeletingSubscriber.begin(), DeletingSubscriber.end(), subscriber);
    if (iter != DeletingSubscriber.end()) {
        DeletingSubscriber.erase(iter);
    }
}
}  // namespace NotificationNapi
}  // namespace OHOS