/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#include "napi_call_ability_callback.h"

#include <ctime>
#include <securec.h>

#include "call_manager_errors.h"
#include "napi_call_manager_utils.h"
#include "napi_util.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
NapiCallAbilityCallback::NapiCallAbilityCallback()
{
    (void)memset_s(&stateCallback_, sizeof(EventCallback), 0, sizeof(EventCallback));
    (void)memset_s(&eventCallback_, sizeof(EventCallback), 0, sizeof(EventCallback));
    (void)memset_s(&ottRequestCallback_, sizeof(EventCallback), 0, sizeof(EventCallback));
    (void)memset_s(&getWaitingCallback_, sizeof(EventCallback), 0, sizeof(EventCallback));
    (void)memset_s(&setWaitingCallback_, sizeof(EventCallback), 0, sizeof(EventCallback));
    (void)memset_s(&getRestrictionCallback_, sizeof(EventCallback), 0, sizeof(EventCallback));
    (void)memset_s(&setRestrictionCallback_, sizeof(EventCallback), 0, sizeof(EventCallback));
    (void)memset_s(&getTransferCallback_, sizeof(EventCallback), 0, sizeof(EventCallback));
    (void)memset_s(&setTransferCallback_, sizeof(EventCallback), 0, sizeof(EventCallback));
    (void)memset_s(&startRttCallback_, sizeof(EventCallback), 0, sizeof(EventCallback));
    (void)memset_s(&stopRttCallback_, sizeof(EventCallback), 0, sizeof(EventCallback));
    (void)memset_s(&updateCallMediaModeCallback_, sizeof(EventCallback), 0, sizeof(EventCallback));
    (void)memset_s(&callDisconnectCauseCallback_, sizeof(EventCallback), 0, sizeof(EventCallback));
    (void)memset_s(&mmiCodeCallback_, sizeof(EventCallback), 0, sizeof(EventCallback));
    memberFuncMap_[CallResultReportId::GET_CALL_WAITING_REPORT_ID] = &NapiCallAbilityCallback::ReportGetWaitingInfo;
    memberFuncMap_[CallResultReportId::SET_CALL_WAITING_REPORT_ID] = &NapiCallAbilityCallback::ReportSetWaitingInfo;
    memberFuncMap_[CallResultReportId::GET_CALL_RESTRICTION_REPORT_ID] =
        &NapiCallAbilityCallback::ReportGetRestrictionInfo;
    memberFuncMap_[CallResultReportId::SET_CALL_RESTRICTION_REPORT_ID] =
        &NapiCallAbilityCallback::ReportSetRestrictionInfo;
    memberFuncMap_[CallResultReportId::GET_CALL_TRANSFER_REPORT_ID] =
        &NapiCallAbilityCallback::ReportGetTransferInfo;
    memberFuncMap_[CallResultReportId::SET_CALL_TRANSFER_REPORT_ID] =
        &NapiCallAbilityCallback::ReportSetTransferInfo;
    memberFuncMap_[CallResultReportId::START_RTT_REPORT_ID] = &NapiCallAbilityCallback::ReportStartRttInfo;
    memberFuncMap_[CallResultReportId::STOP_RTT_REPORT_ID] = &NapiCallAbilityCallback::ReportStopRttInfo;
    memberFuncMap_[CallResultReportId::UPDATE_MEDIA_MODE_REPORT_ID] =
        &NapiCallAbilityCallback::ReportCallMediaModeInfo;
}

NapiCallAbilityCallback::~NapiCallAbilityCallback() {}

void NapiCallAbilityCallback::RegisterCallStateCallback(EventCallback stateCallback)
{
    stateCallback_ = stateCallback;
}

void NapiCallAbilityCallback::UnRegisterCallStateCallback()
{
    if (stateCallback_.callbackRef) {
        (void)memset_s(&stateCallback_, sizeof(EventCallback), 0, sizeof(EventCallback));
    }
}

void NapiCallAbilityCallback::RegisterMmiCodeCallback(EventCallback eventCallback)
{
    mmiCodeCallback_ = eventCallback;
}

void NapiCallAbilityCallback::UnRegisterMmiCodeCallback()
{
    if (mmiCodeCallback_.callbackRef) {
        (void)memset_s(&mmiCodeCallback_, sizeof(EventCallback), 0, sizeof(EventCallback));
    }
}

void NapiCallAbilityCallback::RegisterCallEventCallback(EventCallback eventCallback)
{
    eventCallback_ = eventCallback;
}

void NapiCallAbilityCallback::UnRegisterCallEventCallback()
{
    if (eventCallback_.callbackRef) {
        (void)memset_s(&eventCallback_, sizeof(EventCallback), 0, sizeof(EventCallback));
    }
}

void NapiCallAbilityCallback::RegisterDisconnectedCauseCallback(EventCallback eventCallback)
{
    callDisconnectCauseCallback_ = eventCallback;
}

void NapiCallAbilityCallback::UnRegisterDisconnectedCauseCallback()
{
    if (callDisconnectCauseCallback_.callbackRef) {
        (void)memset_s(&callDisconnectCauseCallback_, sizeof(EventCallback), 0, sizeof(EventCallback));
    }
}

void NapiCallAbilityCallback::RegisterCallOttRequestCallback(EventCallback ottRequestCallback)
{
    ottRequestCallback_ = ottRequestCallback;
}

void NapiCallAbilityCallback::UnRegisterCallOttRequestCallback()
{
    if (ottRequestCallback_.callbackRef) {
        (void)memset_s(&ottRequestCallback_, sizeof(EventCallback), 0, sizeof(EventCallback));
    }
}

int32_t NapiCallAbilityCallback::RegisterGetWaitingCallback(EventCallback callback)
{
    if (getWaitingCallback_.thisVar) {
        TELEPHONY_LOGE("callback already exist!");
        return CALL_ERR_CALLBACK_ALREADY_EXIST;
    }
    getWaitingCallback_ = callback;
    return TELEPHONY_SUCCESS;
}

void NapiCallAbilityCallback::UnRegisterGetWaitingCallback()
{
    (void)memset_s(&getWaitingCallback_, sizeof(EventCallback), 0, sizeof(EventCallback));
}

int32_t NapiCallAbilityCallback::RegisterSetWaitingCallback(EventCallback callback)
{
    if (setWaitingCallback_.thisVar) {
        TELEPHONY_LOGE("callback already exist!");
        return CALL_ERR_CALLBACK_ALREADY_EXIST;
    }
    setWaitingCallback_ = callback;
    return TELEPHONY_SUCCESS;
}

void NapiCallAbilityCallback::UnRegisterSetWaitingCallback()
{
    (void)memset_s(&setWaitingCallback_, sizeof(EventCallback), 0, sizeof(EventCallback));
}

int32_t NapiCallAbilityCallback::RegisterGetRestrictionCallback(EventCallback callback)
{
    if (getRestrictionCallback_.thisVar) {
        TELEPHONY_LOGE("callback already exist!");
        return CALL_ERR_CALLBACK_ALREADY_EXIST;
    }
    getRestrictionCallback_ = callback;
    return TELEPHONY_SUCCESS;
}

void NapiCallAbilityCallback::UnRegisterGetRestrictionCallback()
{
    (void)memset_s(&getRestrictionCallback_, sizeof(EventCallback), 0, sizeof(EventCallback));
}

int32_t NapiCallAbilityCallback::RegisterSetRestrictionCallback(EventCallback callback)
{
    if (setRestrictionCallback_.thisVar) {
        TELEPHONY_LOGE("callback already exist!");
        return CALL_ERR_CALLBACK_ALREADY_EXIST;
    }
    setRestrictionCallback_ = callback;
    return TELEPHONY_SUCCESS;
}

void NapiCallAbilityCallback::UnRegisterSetRestrictionCallback()
{
    (void)memset_s(&setRestrictionCallback_, sizeof(EventCallback), 0, sizeof(EventCallback));
}

int32_t NapiCallAbilityCallback::RegisterGetTransferCallback(EventCallback callback)
{
    if (getTransferCallback_.thisVar) {
        TELEPHONY_LOGE("callback already exist!");
        return CALL_ERR_CALLBACK_ALREADY_EXIST;
    }
    getTransferCallback_ = callback;
    return TELEPHONY_SUCCESS;
}

void NapiCallAbilityCallback::UnRegisterGetTransferCallback()
{
    (void)memset_s(&getTransferCallback_, sizeof(EventCallback), 0, sizeof(EventCallback));
}

int32_t NapiCallAbilityCallback::RegisterSetTransferCallback(EventCallback callback)
{
    if (setTransferCallback_.thisVar) {
        TELEPHONY_LOGE("callback already exist!");
        return CALL_ERR_CALLBACK_ALREADY_EXIST;
    }
    setTransferCallback_ = callback;
    return TELEPHONY_SUCCESS;
}

void NapiCallAbilityCallback::UnRegisterSetTransferCallback()
{
    (void)memset_s(&setTransferCallback_, sizeof(EventCallback), 0, sizeof(EventCallback));
}

int32_t NapiCallAbilityCallback::RegisterStartRttCallback(EventCallback callback)
{
    if (startRttCallback_.thisVar) {
        TELEPHONY_LOGE("callback already exist!");
        return CALL_ERR_CALLBACK_ALREADY_EXIST;
    }
    startRttCallback_ = callback;
    return TELEPHONY_SUCCESS;
}

void NapiCallAbilityCallback::UnRegisterStartRttCallback()
{
    (void)memset_s(&startRttCallback_, sizeof(EventCallback), 0, sizeof(EventCallback));
}

int32_t NapiCallAbilityCallback::RegisterStopRttCallback(EventCallback callback)
{
    if (stopRttCallback_.thisVar) {
        TELEPHONY_LOGE("callback already exist!");
        return CALL_ERR_CALLBACK_ALREADY_EXIST;
    }
    stopRttCallback_ = callback;
    return TELEPHONY_SUCCESS;
}

void NapiCallAbilityCallback::UnRegisterStopRttCallback()
{
    (void)memset_s(&stopRttCallback_, sizeof(EventCallback), 0, sizeof(EventCallback));
}

int32_t NapiCallAbilityCallback::RegisterUpdateCallMediaModeCallback(EventCallback callback)
{
    if (updateCallMediaModeCallback_.thisVar) {
        TELEPHONY_LOGE("callback already exist!");
        return CALL_ERR_CALLBACK_ALREADY_EXIST;
    }
    updateCallMediaModeCallback_ = callback;
    return TELEPHONY_SUCCESS;
}

void NapiCallAbilityCallback::UnRegisterUpdateCallMediaModeCallback()
{
    (void)memset_s(&updateCallMediaModeCallback_, sizeof(EventCallback), 0, sizeof(EventCallback));
}

int32_t NapiCallAbilityCallback::ReportStartRttInfo(AppExecFwk::PacMap &resultInfo)
{
    if (startRttCallback_.thisVar == nullptr) {
        TELEPHONY_LOGE("startRttCallback_ is null!");
        return CALL_ERR_CALLBACK_NOT_EXIST;
    }
    uv_loop_s *loop = nullptr;
#if NAPI_VERSION >= 2
    napi_get_uv_event_loop(startRttCallback_.env, &loop);
#endif
    CallSupplementWorker *dataWorker = std::make_unique<CallSupplementWorker>().release();
    if (dataWorker == nullptr) {
        TELEPHONY_LOGE("dataWorker is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    dataWorker->info = resultInfo;
    dataWorker->callback = startRttCallback_;
    uv_work_t *work = std::make_unique<uv_work_t>().release();
    if (work == nullptr) {
        TELEPHONY_LOGE("work is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    work->data = (void *)dataWorker;
    uv_queue_work(
        loop, work, [](uv_work_t *work) {}, ReportStartRttInfoWork);
    if (startRttCallback_.thisVar) {
        (void)memset_s(&startRttCallback_, sizeof(EventCallback), 0, sizeof(EventCallback));
    }
    return TELEPHONY_SUCCESS;
}

int32_t NapiCallAbilityCallback::ReportStopRttInfo(AppExecFwk::PacMap &resultInfo)
{
    if (stopRttCallback_.thisVar == nullptr) {
        TELEPHONY_LOGE("startRttCallback_ is null!");
        return CALL_ERR_CALLBACK_NOT_EXIST;
    }
    uv_loop_s *loop = nullptr;
#if NAPI_VERSION >= 2
    napi_get_uv_event_loop(stopRttCallback_.env, &loop);
#endif
    CallSupplementWorker *dataWorker = std::make_unique<CallSupplementWorker>().release();
    if (dataWorker == nullptr) {
        TELEPHONY_LOGE("dataWorker is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    dataWorker->info = resultInfo;
    dataWorker->callback = stopRttCallback_;
    uv_work_t *work = std::make_unique<uv_work_t>().release();
    if (work == nullptr) {
        TELEPHONY_LOGE("work is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    work->data = (void *)dataWorker;
    uv_queue_work(
        loop, work, [](uv_work_t *work) {}, ReportStopRttInfoWork);
    if (stopRttCallback_.thisVar) {
        (void)memset_s(&stopRttCallback_, sizeof(EventCallback), 0, sizeof(EventCallback));
    }
    return TELEPHONY_SUCCESS;
}

int32_t NapiCallAbilityCallback::ReportCallMediaModeInfo(AppExecFwk::PacMap &resultInfo)
{
    if (updateCallMediaModeCallback_.thisVar == nullptr) {
        TELEPHONY_LOGE("updateCallMediaModeCallback_ is null!");
        return CALL_ERR_CALLBACK_NOT_EXIST;
    }
    uv_loop_s *loop = nullptr;
#if NAPI_VERSION >= 2
    napi_get_uv_event_loop(updateCallMediaModeCallback_.env, &loop);
#endif
    CallSupplementWorker *dataWorker = std::make_unique<CallSupplementWorker>().release();
    if (dataWorker == nullptr) {
        TELEPHONY_LOGE("dataWorker is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    dataWorker->info = resultInfo;
    dataWorker->callback = updateCallMediaModeCallback_;
    uv_work_t *work = std::make_unique<uv_work_t>().release();
    if (work == nullptr) {
        TELEPHONY_LOGE("work is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    work->data = (void *)dataWorker;
    uv_queue_work(
        loop, work, [](uv_work_t *work) {}, ReportCallMediaModeInfoWork);
    if (updateCallMediaModeCallback_.thisVar) {
        (void)memset_s(&updateCallMediaModeCallback_, sizeof(EventCallback), 0, sizeof(EventCallback));
    }
    return TELEPHONY_SUCCESS;
}

int32_t NapiCallAbilityCallback::UpdateCallStateInfo(const CallAttributeInfo &info)
{
    if (stateCallback_.thisVar == nullptr) {
        TELEPHONY_LOGE("stateCallback is null!");
        return CALL_ERR_CALLBACK_NOT_EXIST;
    }
    uv_loop_s *loop = nullptr;
#if NAPI_VERSION >= 2
    napi_get_uv_event_loop(stateCallback_.env, &loop);
#endif
    CallStateWorker *dataWorker = std::make_unique<CallStateWorker>().release();
    if (dataWorker == nullptr) {
        TELEPHONY_LOGE("dataWorker is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    dataWorker->info = info;
    dataWorker->callback = stateCallback_;
    uv_work_t *work = std::make_unique<uv_work_t>().release();
    if (work == nullptr) {
        TELEPHONY_LOGE("work is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    work->data = (void *)dataWorker;
    uv_queue_work(
        loop, work, [](uv_work_t *work) {}, ReportCallStateWork);

    return TELEPHONY_SUCCESS;
}

void NapiCallAbilityCallback::ReportCallStateWork(uv_work_t *work, int32_t status)
{
    CallStateWorker *dataWorkerData = (CallStateWorker *)work->data;
    if (dataWorkerData == nullptr) {
        TELEPHONY_LOGE("dataWorkerData is nullptr!");
        return;
    }
    int32_t ret = ReportCallState(dataWorkerData->info, dataWorkerData->callback);
    TELEPHONY_LOGI("ReportCallState result = %{public}d", ret);
    delete dataWorkerData;
    dataWorkerData = nullptr;
    delete work;
    work = nullptr;
}

/**
 * To notify an application of a call status change, register a callback with on() first.
 */
int32_t NapiCallAbilityCallback::ReportCallState(CallAttributeInfo &info, EventCallback stateCallback)
{
    napi_value callbackFunc = nullptr;
    napi_env env = stateCallback.env;
    napi_value callbackValues[ARRAY_INDEX_THIRD] = { 0 };
    napi_create_object(env, &callbackValues[ARRAY_INDEX_FIRST]);
    NapiCallManagerUtils::SetPropertyStringUtf8(
        env, callbackValues[ARRAY_INDEX_FIRST], "accountNumber", info.accountNumber);
    NapiCallManagerUtils::SetPropertyInt32(env, callbackValues[ARRAY_INDEX_FIRST], "accountId", info.accountId);
    NapiCallManagerUtils::SetPropertyInt32(
        env, callbackValues[ARRAY_INDEX_FIRST], "videoState", static_cast<int32_t>(info.videoState));
    NapiCallManagerUtils::SetPropertyInt32(env, callbackValues[ARRAY_INDEX_FIRST], "startTime", info.startTime);
    NapiCallManagerUtils::SetPropertyBoolean(env, callbackValues[ARRAY_INDEX_FIRST], "isEcc", info.isEcc);
    NapiCallManagerUtils::SetPropertyInt32(
        env, callbackValues[ARRAY_INDEX_FIRST], "callType", static_cast<int32_t>(info.callType));
    NapiCallManagerUtils::SetPropertyInt32(env, callbackValues[ARRAY_INDEX_FIRST], "callId", info.callId);
    NapiCallManagerUtils::SetPropertyInt32(
        env, callbackValues[ARRAY_INDEX_FIRST], "callState", static_cast<int32_t>(info.callState));
    NapiCallManagerUtils::SetPropertyInt32(
        env, callbackValues[ARRAY_INDEX_FIRST], "conferenceState", static_cast<int32_t>(info.conferenceState));
    napi_get_reference_value(env, stateCallback.callbackRef, &callbackFunc);
    if (callbackFunc == nullptr) {
        TELEPHONY_LOGE("callbackFunc is null!");
        return CALL_ERR_CALLBACK_NOT_EXIST;
    }
    napi_value thisVar = nullptr;
    napi_get_reference_value(env, stateCallback.thisVar, &thisVar);
    napi_value callbackResult = nullptr;
    napi_call_function(env, thisVar, callbackFunc, DATA_LENGTH_ONE, callbackValues, &callbackResult);
    return TELEPHONY_SUCCESS;
}

int32_t NapiCallAbilityCallback::UpdateCallEvent(const CallEventInfo &info)
{
    if (eventCallback_.thisVar == nullptr) {
        TELEPHONY_LOGE("eventCallback is null!");
        return CALL_ERR_CALLBACK_NOT_EXIST;
    }
    uv_loop_s *loop = nullptr;
#if NAPI_VERSION >= 2
    napi_get_uv_event_loop(eventCallback_.env, &loop);
#endif
    CallEventWorker *dataWorker = std::make_unique<CallEventWorker>().release();
    if (dataWorker == nullptr) {
        TELEPHONY_LOGE("dataWorker is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    dataWorker->info = info;
    dataWorker->callback = eventCallback_;
    uv_work_t *work = std::make_unique<uv_work_t>().release();
    if (work == nullptr) {
        TELEPHONY_LOGE("work is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    work->data = (void *)dataWorker;
    uv_queue_work(
        loop, work, [](uv_work_t *work) {}, ReportCallEventWork);
    return TELEPHONY_SUCCESS;
}

void NapiCallAbilityCallback::ReportCallEventWork(uv_work_t *work, int32_t status)
{
    CallEventWorker *dataWorkerData = (CallEventWorker *)work->data;
    if (dataWorkerData == nullptr) {
        TELEPHONY_LOGE("dataWorkerData is nullptr!");
        return;
    }
    int32_t ret = ReportCallEvent(dataWorkerData->info, dataWorkerData->callback);
    TELEPHONY_LOGI("ReportCallEvent results %{public}d", ret);
    delete dataWorkerData;
    dataWorkerData = nullptr;
    delete work;
    work = nullptr;
}

int32_t NapiCallAbilityCallback::ReportCallEvent(CallEventInfo &info, EventCallback eventCallback)
{
    napi_value callbackFunc = nullptr;
    napi_env env = eventCallback.env;
    napi_value callbackValues[ARRAY_INDEX_THIRD] = { 0 };
    napi_create_object(env, &callbackValues[ARRAY_INDEX_FIRST]);
    NapiCallManagerUtils::SetPropertyInt32(
        env, callbackValues[ARRAY_INDEX_FIRST], "eventId", static_cast<int32_t>(info.eventId));
    NapiCallManagerUtils::SetPropertyStringUtf8(env, callbackValues[ARRAY_INDEX_FIRST], "accountNumber", info.phoneNum);
    NapiCallManagerUtils::SetPropertyStringUtf8(env, callbackValues[ARRAY_INDEX_FIRST], "bundleName", info.bundleName);
    napi_get_reference_value(env, eventCallback.callbackRef, &callbackFunc);
    if (callbackFunc == nullptr) {
        TELEPHONY_LOGE("callbackFunc is null!");
        return CALL_ERR_CALLBACK_NOT_EXIST;
    }
    napi_value thisVar = nullptr;
    napi_get_reference_value(env, eventCallback.thisVar, &thisVar);
    napi_value callbackResult = nullptr;
    napi_call_function(env, thisVar, callbackFunc, DATA_LENGTH_ONE, callbackValues, &callbackResult);
    return TELEPHONY_SUCCESS;
}

int32_t NapiCallAbilityCallback::UpdateCallDisconnectedCause(const DisconnectedDetails &details)
{
    if (callDisconnectCauseCallback_.thisVar == nullptr) {
        TELEPHONY_LOGE("callDisconnectCauseCallback_ is null!");
        return CALL_ERR_CALLBACK_NOT_EXIST;
    }
    uv_loop_s *loop = nullptr;
#if NAPI_VERSION >= 2
    napi_get_uv_event_loop(callDisconnectCauseCallback_.env, &loop);
#endif
    CallDisconnectedCauseWorker *dataWorker = std::make_unique<CallDisconnectedCauseWorker>().release();
    if (dataWorker == nullptr) {
        TELEPHONY_LOGE("dataWorker is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    dataWorker->details = details;
    dataWorker->callback = callDisconnectCauseCallback_;
    uv_work_t *work = std::make_unique<uv_work_t>().release();
    if (work == nullptr) {
        TELEPHONY_LOGE("work is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    work->data = (void *)dataWorker;
    uv_queue_work(
        loop, work, [](uv_work_t *work) {}, ReportCallDisconnectedCauseWork);
    if (callDisconnectCauseCallback_.thisVar) {
        (void)memset_s(&callDisconnectCauseCallback_, sizeof(EventCallback), 0, sizeof(EventCallback));
    }
    return TELEPHONY_SUCCESS;
}

void NapiCallAbilityCallback::ReportCallDisconnectedCauseWork(uv_work_t *work, int32_t status)
{
    CallDisconnectedCauseWorker *dataWorkerData = (CallDisconnectedCauseWorker *)work->data;
    if (dataWorkerData == nullptr) {
        TELEPHONY_LOGE("dataWorkerData is nullptr!");
        return;
    }
    int32_t ret = ReportDisconnectedCause(dataWorkerData->details, dataWorkerData->callback);
    TELEPHONY_LOGI("ReportDisconnectedCause results %{public}d", ret);
    delete dataWorkerData;
    dataWorkerData = nullptr;
    delete work;
    work = nullptr;
}

int32_t NapiCallAbilityCallback::ReportDisconnectedCause(
    const DisconnectedDetails &details, EventCallback eventCallback)
{
    napi_value callbackFunc = nullptr;
    napi_env env = eventCallback.env;
    napi_value callbackValues[ARRAY_INDEX_THIRD] = { 0 };
    napi_create_object(env, &callbackValues[ARRAY_INDEX_SECOND]);
    NapiCallManagerUtils::SetPropertyInt32(
        env, callbackValues[ARRAY_INDEX_SECOND], "disconnectedCause", static_cast<int32_t>(details.reason));
    NapiCallManagerUtils::SetPropertyStringUtf8(env, callbackValues[ARRAY_INDEX_SECOND], "message", details.message);
    napi_get_reference_value(env, eventCallback.callbackRef, &callbackFunc);
    if (callbackFunc == nullptr) {
        TELEPHONY_LOGE("callbackFunc is null!");
        return CALL_ERR_CALLBACK_NOT_EXIST;
    }
    napi_value thisVar = nullptr;
    napi_get_reference_value(env, eventCallback.thisVar, &thisVar);
    napi_value callbackResult = nullptr;
    napi_call_function(env, thisVar, callbackFunc, DATA_LENGTH_ONE, callbackValues, &callbackResult);
    return TELEPHONY_SUCCESS;
}

int32_t NapiCallAbilityCallback::UpdateAsyncResultsInfo(
    const CallResultReportId reportId, AppExecFwk::PacMap &resultInfo)
{
    int32_t result = TELEPHONY_ERR_FAIL;
    TELEPHONY_LOGI("UpdateAsyncResultsInfo reportId = %{public}d", reportId);
    auto itFunc = memberFuncMap_.find(reportId);
    if (itFunc != memberFuncMap_.end() && itFunc->second != nullptr) {
        auto memberFunc = itFunc->second;
        result = (this->*memberFunc)(resultInfo);
    }
    return result;
}

int32_t NapiCallAbilityCallback::UpdateMmiCodeResultsInfo(const MmiCodeInfo &info)
{
    if (mmiCodeCallback_.thisVar == nullptr) {
        TELEPHONY_LOGE("mmiCodeCallback is null!");
        return CALL_ERR_CALLBACK_NOT_EXIST;
    }
    uv_loop_s *loop = nullptr;
#if NAPI_VERSION >= 2
    napi_get_uv_event_loop(mmiCodeCallback_.env, &loop);
#endif
    MmiCodeWorker *dataWorker = std::make_unique<MmiCodeWorker>().release();
    if (dataWorker == nullptr) {
        TELEPHONY_LOGE("dataWorker is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    dataWorker->info = info;
    dataWorker->callback = mmiCodeCallback_;
    uv_work_t *work = std::make_unique<uv_work_t>().release();
    if (work == nullptr) {
        TELEPHONY_LOGE("work is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    work->data = (void *)dataWorker;
    uv_queue_work(
        loop, work, [](uv_work_t *work) {}, ReportMmiCodeWork);

    return TELEPHONY_SUCCESS;
}

void NapiCallAbilityCallback::ReportMmiCodeWork(uv_work_t *work, int32_t status)
{
    MmiCodeWorker *dataWorkerData = (MmiCodeWorker *)work->data;
    if (dataWorkerData == nullptr) {
        TELEPHONY_LOGE("dataWorkerData is nullptr!");
        return;
    }
    int32_t ret = ReportMmiCode(dataWorkerData->info, dataWorkerData->callback);
    TELEPHONY_LOGI("ReportMmiCode result = %{public}d", ret);
    delete dataWorkerData;
    dataWorkerData = nullptr;
    delete work;
    work = nullptr;
}

/**
 * To notify an application of MMI code result, register a callback with on() first.
 */
int32_t NapiCallAbilityCallback::ReportMmiCode(MmiCodeInfo &info, EventCallback eventCallback)
{
    napi_value callbackFunc = nullptr;
    napi_env env = eventCallback.env;
    napi_value callbackValues[ARRAY_INDEX_THIRD] = { 0 };
    callbackValues[ARRAY_INDEX_FIRST] = NapiCallManagerUtils::CreateUndefined(env);
    napi_create_object(env, &callbackValues[ARRAY_INDEX_SECOND]);
    NapiCallManagerUtils::SetPropertyInt32(
        env, callbackValues[ARRAY_INDEX_SECOND], "result", static_cast<int32_t>(info.result));
    NapiCallManagerUtils::SetPropertyStringUtf8(env, callbackValues[ARRAY_INDEX_SECOND], "message", info.message);
    napi_get_reference_value(env, eventCallback.callbackRef, &callbackFunc);
    if (callbackFunc == nullptr) {
        TELEPHONY_LOGE("callbackFunc is null!");
        return CALL_ERR_CALLBACK_NOT_EXIST;
    }
    napi_value thisVar = nullptr;
    napi_get_reference_value(env, eventCallback.thisVar, &thisVar);
    napi_value callbackResult = nullptr;
    napi_call_function(env, thisVar, callbackFunc, DATA_LENGTH_TWO, callbackValues, &callbackResult);
    return TELEPHONY_SUCCESS;
}

int32_t NapiCallAbilityCallback::OttCallRequest(OttCallRequestId requestId, AppExecFwk::PacMap &info)
{
    if (ottRequestCallback_.thisVar == nullptr) {
        TELEPHONY_LOGE("stateCallback is null!");
        return CALL_ERR_CALLBACK_NOT_EXIST;
    }
    uv_loop_s *loop = nullptr;
#if NAPI_VERSION >= 2
    napi_get_uv_event_loop(ottRequestCallback_.env, &loop);
#endif
    CallOttWorker *dataWorker = std::make_unique<CallOttWorker>().release();
    if (dataWorker == nullptr) {
        TELEPHONY_LOGE("dataWorker is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    dataWorker->requestId = requestId;
    dataWorker->info = info;
    dataWorker->callback = ottRequestCallback_;
    uv_work_t *work = std::make_unique<uv_work_t>().release();
    if (work == nullptr) {
        TELEPHONY_LOGE("work is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    work->data = (void *)dataWorker;
    uv_queue_work(
        loop, work, [](uv_work_t *work) {}, ReportCallOttWork);

    return TELEPHONY_SUCCESS;
}

int32_t NapiCallAbilityCallback::ReportGetWaitingInfo(AppExecFwk::PacMap &resultInfo)
{
    if (getWaitingCallback_.thisVar == nullptr) {
        TELEPHONY_LOGE("getWaitingCallback is null!");
        return CALL_ERR_CALLBACK_NOT_EXIST;
    }
    uv_loop_s *loop = nullptr;
#if NAPI_VERSION >= 2
    napi_get_uv_event_loop(getWaitingCallback_.env, &loop);
#endif
    CallSupplementWorker *dataWorker = std::make_unique<CallSupplementWorker>().release();
    if (dataWorker == nullptr) {
        TELEPHONY_LOGE("dataWorker is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    dataWorker->info = resultInfo;
    dataWorker->callback = getWaitingCallback_;
    uv_work_t *work = std::make_unique<uv_work_t>().release();
    if (work == nullptr) {
        TELEPHONY_LOGE("work is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    work->data = (void *)dataWorker;
    uv_queue_work(
        loop, work, [](uv_work_t *work) {}, ReportWaitAndLimitInfoWork);
    if (getWaitingCallback_.thisVar) {
        (void)memset_s(&getWaitingCallback_, sizeof(EventCallback), 0, sizeof(EventCallback));
    }
    return TELEPHONY_SUCCESS;
}

int32_t NapiCallAbilityCallback::ReportSetWaitingInfo(AppExecFwk::PacMap &resultInfo)
{
    if (setWaitingCallback_.thisVar == nullptr) {
        TELEPHONY_LOGE("setWaitingCallback is null!");
        return CALL_ERR_CALLBACK_NOT_EXIST;
    }
    uv_loop_s *loop = nullptr;
#if NAPI_VERSION >= 2
    napi_get_uv_event_loop(setWaitingCallback_.env, &loop);
#endif
    CallSupplementWorker *dataWorker = std::make_unique<CallSupplementWorker>().release();
    if (dataWorker == nullptr) {
        TELEPHONY_LOGE("dataWorker is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    dataWorker->info = resultInfo;
    dataWorker->callback = setWaitingCallback_;
    uv_work_t *work = std::make_unique<uv_work_t>().release();
    if (work == nullptr) {
        TELEPHONY_LOGE("work is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    work->data = (void *)dataWorker;
    uv_queue_work(
        loop, work, [](uv_work_t *work) {}, ReportExecutionResultWork);
    if (setWaitingCallback_.thisVar) {
        (void)memset_s(&setWaitingCallback_, sizeof(EventCallback), 0, sizeof(EventCallback));
    }
    return TELEPHONY_SUCCESS;
}

int32_t NapiCallAbilityCallback::ReportGetRestrictionInfo(AppExecFwk::PacMap &resultInfo)
{
    if (getRestrictionCallback_.thisVar == nullptr) {
        TELEPHONY_LOGE("getRestrictionCallback is null!");
        return CALL_ERR_CALLBACK_NOT_EXIST;
    }
    uv_loop_s *loop = nullptr;
#if NAPI_VERSION >= 2
    napi_get_uv_event_loop(getRestrictionCallback_.env, &loop);
#endif
    CallSupplementWorker *dataWorker = std::make_unique<CallSupplementWorker>().release();
    if (dataWorker == nullptr) {
        TELEPHONY_LOGE("dataWorker is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    dataWorker->info = resultInfo;
    dataWorker->callback = getRestrictionCallback_;
    uv_work_t *work = std::make_unique<uv_work_t>().release();
    if (work == nullptr) {
        TELEPHONY_LOGE("work is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    work->data = (void *)dataWorker;
    uv_queue_work(
        loop, work, [](uv_work_t *work) {}, ReportWaitAndLimitInfoWork);
    if (getRestrictionCallback_.thisVar) {
        (void)memset_s(&getRestrictionCallback_, sizeof(EventCallback), 0, sizeof(EventCallback));
    }
    return TELEPHONY_SUCCESS;
}

int32_t NapiCallAbilityCallback::ReportSetRestrictionInfo(AppExecFwk::PacMap &resultInfo)
{
    if (setRestrictionCallback_.thisVar == nullptr) {
        TELEPHONY_LOGE("setRestrictionCallback is null!");
        return CALL_ERR_CALLBACK_NOT_EXIST;
    }
    uv_loop_s *loop = nullptr;
#if NAPI_VERSION >= 2
    napi_get_uv_event_loop(setRestrictionCallback_.env, &loop);
#endif
    CallSupplementWorker *dataWorker = std::make_unique<CallSupplementWorker>().release();
    if (dataWorker == nullptr) {
        TELEPHONY_LOGE("dataWorker is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    dataWorker->info = resultInfo;
    dataWorker->callback = setRestrictionCallback_;
    uv_work_t *work = std::make_unique<uv_work_t>().release();
    if (work == nullptr) {
        TELEPHONY_LOGE("work is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    work->data = (void *)dataWorker;
    uv_queue_work(
        loop, work, [](uv_work_t *work) {}, ReportExecutionResultWork);
    if (setRestrictionCallback_.thisVar) {
        (void)memset_s(&setRestrictionCallback_, sizeof(EventCallback), 0, sizeof(EventCallback));
    }
    return TELEPHONY_SUCCESS;
}

int32_t NapiCallAbilityCallback::ReportGetTransferInfo(AppExecFwk::PacMap &resultInfo)
{
    if (getTransferCallback_.thisVar == nullptr) {
        TELEPHONY_LOGE("getTransferCallback is null!");
        return CALL_ERR_CALLBACK_NOT_EXIST;
    }
    uv_loop_s *loop = nullptr;
#if NAPI_VERSION >= 2
    napi_get_uv_event_loop(getTransferCallback_.env, &loop);
#endif
    CallSupplementWorker *dataWorker = std::make_unique<CallSupplementWorker>().release();
    if (dataWorker == nullptr) {
        TELEPHONY_LOGE("dataWorker is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    dataWorker->info = resultInfo;
    dataWorker->callback = getTransferCallback_;
    uv_work_t *work = std::make_unique<uv_work_t>().release();
    if (work == nullptr) {
        TELEPHONY_LOGE("work is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    work->data = (void *)dataWorker;
    uv_queue_work(
        loop, work, [](uv_work_t *work) {}, ReportSupplementInfoWork);
    if (getTransferCallback_.thisVar) {
        (void)memset_s(&getTransferCallback_, sizeof(EventCallback), 0, sizeof(EventCallback));
    }
    return TELEPHONY_SUCCESS;
}

int32_t NapiCallAbilityCallback::ReportSetTransferInfo(AppExecFwk::PacMap &resultInfo)
{
    if (setTransferCallback_.thisVar == nullptr) {
        TELEPHONY_LOGE("setTransferCallback is null!");
        return CALL_ERR_CALLBACK_NOT_EXIST;
    }
    uv_loop_s *loop = nullptr;
#if NAPI_VERSION >= 2
    napi_get_uv_event_loop(setTransferCallback_.env, &loop);
#endif
    CallSupplementWorker *dataWorker = std::make_unique<CallSupplementWorker>().release();
    if (dataWorker == nullptr) {
        TELEPHONY_LOGE("dataWorker is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    dataWorker->info = resultInfo;
    dataWorker->callback = setTransferCallback_;
    uv_work_t *work = std::make_unique<uv_work_t>().release();
    if (work == nullptr) {
        TELEPHONY_LOGE("work is nullptr!");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }
    work->data = (void *)dataWorker;
    uv_queue_work(
        loop, work, [](uv_work_t *work) {}, ReportExecutionResultWork);
    if (setTransferCallback_.thisVar) {
        (void)memset_s(&setTransferCallback_, sizeof(EventCallback), 0, sizeof(EventCallback));
    }
    return TELEPHONY_SUCCESS;
}

void NapiCallAbilityCallback::ReportWaitAndLimitInfoWork(uv_work_t *work, int32_t status)
{
    CallSupplementWorker *dataWorkerData = (CallSupplementWorker *)work->data;
    if (dataWorkerData == nullptr) {
        TELEPHONY_LOGE("dataWorkerData is nullptr!");
        return;
    }
    ReportWaitAndLimitInfo(dataWorkerData->info, dataWorkerData->callback);
    delete dataWorkerData;
    dataWorkerData = nullptr;
    delete work;
    work = nullptr;
}

void NapiCallAbilityCallback::ReportWaitAndLimitInfo(AppExecFwk::PacMap &resultInfo, EventCallback supplementInfo)
{
    napi_env env = supplementInfo.env;
    int32_t result = resultInfo.GetIntValue("result");
    int32_t status = resultInfo.GetIntValue("status");
    if (supplementInfo.callbackRef != nullptr) {
        napi_value callbackValues[ARRAY_INDEX_THIRD] = { 0 };
        if (result == TELEPHONY_SUCCESS) {
            callbackValues[ARRAY_INDEX_FIRST] = NapiCallManagerUtils::CreateUndefined(env);
            napi_create_int32(env, status, &callbackValues[ARRAY_INDEX_SECOND]);
        } else {
            std::string errTip = std::to_string(CALL_ERR_NAPI_INTERFACE_FAILED);
            callbackValues[ARRAY_INDEX_FIRST] = NapiCallManagerUtils::CreateErrorMessage(env, errTip);
            callbackValues[ARRAY_INDEX_SECOND] = NapiCallManagerUtils::CreateUndefined(env);
        }
        napi_value callbackFunc = nullptr;
        napi_get_reference_value(env, supplementInfo.callbackRef, &callbackFunc);
        napi_value thisVar = nullptr;
        napi_get_reference_value(env, supplementInfo.thisVar, &thisVar);
        napi_value callbackResult = nullptr;
        napi_call_function(env, thisVar, callbackFunc, DATA_LENGTH_TWO, callbackValues, &callbackResult);
        napi_delete_reference(env, supplementInfo.callbackRef);
        napi_delete_reference(env, supplementInfo.thisVar);
    } else if (supplementInfo.deferred != nullptr) {
        if (result == TELEPHONY_SUCCESS) {
            napi_value promiseValue = nullptr;
            napi_create_int32(env, status, &promiseValue);
            napi_resolve_deferred(env, supplementInfo.deferred, promiseValue);
        } else {
            std::string errTip = std::to_string(CALL_ERR_NAPI_INTERFACE_FAILED);
            napi_reject_deferred(env, supplementInfo.deferred, NapiCallManagerUtils::CreateErrorMessage(env, errTip));
        }
    }
}

void NapiCallAbilityCallback::ReportSupplementInfoWork(uv_work_t *work, int32_t status)
{
    CallSupplementWorker *dataWorkerData = (CallSupplementWorker *)work->data;
    if (dataWorkerData == nullptr) {
        TELEPHONY_LOGE("dataWorkerData is nullptr!");
        return;
    }
    ReportSupplementInfo(dataWorkerData->info, dataWorkerData->callback);
    delete dataWorkerData;
    dataWorkerData = nullptr;
    delete work;
    work = nullptr;
}

void NapiCallAbilityCallback::ReportSupplementInfo(AppExecFwk::PacMap &resultInfo, EventCallback supplementInfo)
{
    napi_env env = supplementInfo.env;
    napi_value callbackValue = nullptr;
    napi_create_object(env, &callbackValue);
    NapiCallManagerUtils::SetPropertyInt32(env, callbackValue, "status", resultInfo.GetIntValue("status"));
    NapiCallManagerUtils::SetPropertyStringUtf8(env, callbackValue, "number", resultInfo.GetStringValue("number"));
    int32_t result = resultInfo.GetIntValue("result");
    if (supplementInfo.callbackRef != nullptr) {
        napi_value callbackValues[ARRAY_INDEX_THIRD] = { 0 };
        if (result == TELEPHONY_SUCCESS) {
            callbackValues[ARRAY_INDEX_FIRST] = NapiCallManagerUtils::CreateUndefined(env);
            callbackValues[ARRAY_INDEX_SECOND] = callbackValue;
        } else {
            std::string errTip = std::to_string(CALL_ERR_NAPI_INTERFACE_FAILED);
            callbackValues[ARRAY_INDEX_FIRST] = NapiCallManagerUtils::CreateErrorMessage(env, errTip);
            callbackValues[ARRAY_INDEX_SECOND] = NapiCallManagerUtils::CreateUndefined(env);
        }
        napi_value callbackFunc = nullptr;
        napi_get_reference_value(env, supplementInfo.callbackRef, &callbackFunc);
        napi_value thisVar = nullptr;
        napi_get_reference_value(env, supplementInfo.thisVar, &thisVar);
        napi_value callbackResult = nullptr;
        napi_call_function(env, thisVar, callbackFunc, DATA_LENGTH_TWO, callbackValues, &callbackResult);
        napi_delete_reference(env, supplementInfo.callbackRef);
        napi_delete_reference(env, supplementInfo.thisVar);
    } else if (supplementInfo.deferred != nullptr) {
        if (result == TELEPHONY_SUCCESS) {
            napi_resolve_deferred(env, supplementInfo.deferred, callbackValue);
        } else {
            std::string errTip = std::to_string(CALL_ERR_NAPI_INTERFACE_FAILED);
            napi_reject_deferred(
                env, supplementInfo.deferred, NapiCallManagerUtils::CreateErrorMessage(env, errTip));
        }
    }
}

void NapiCallAbilityCallback::ReportExecutionResultWork(uv_work_t *work, int32_t status)
{
    CallSupplementWorker *dataWorkerData = (CallSupplementWorker *)work->data;
    if (dataWorkerData == nullptr) {
        TELEPHONY_LOGE("dataWorkerData is nullptr!");
        return;
    }
    ReportExecutionResult(dataWorkerData->callback, dataWorkerData->info);
    delete dataWorkerData;
    dataWorkerData = nullptr;
    delete work;
    work = nullptr;
}

void NapiCallAbilityCallback::ReportExecutionResult(EventCallback &settingInfo, AppExecFwk::PacMap &resultInfo)
{
    napi_env env = settingInfo.env;
    napi_value callbackValue = nullptr;
    napi_create_object(env, &callbackValue);
    int32_t result = resultInfo.GetIntValue("result");
    if (settingInfo.callbackRef != nullptr) {
        napi_value callbackValues[ARRAY_INDEX_THIRD] = { 0 };
        if (result == TELEPHONY_SUCCESS) {
            callbackValues[ARRAY_INDEX_FIRST] = NapiCallManagerUtils::CreateUndefined(env);
            napi_get_null(env, &callbackValues[ARRAY_INDEX_SECOND]);
        } else {
            JsError error = NapiUtil::ConverErrorMessageForJs(result);
            callbackValues[ARRAY_INDEX_FIRST] =
                NapiCallManagerUtils::CreateErrorMessageWithErrorCode(env, error.errorMessage, error.errorCode);
            callbackValues[ARRAY_INDEX_SECOND] = NapiCallManagerUtils::CreateUndefined(env);
        }
        napi_value callbackFunc = nullptr;
        napi_get_reference_value(env, settingInfo.callbackRef, &callbackFunc);
        napi_value thisVar = nullptr;
        napi_get_reference_value(env, settingInfo.thisVar, &thisVar);
        napi_value callbackResult = nullptr;
        napi_call_function(env, thisVar, callbackFunc, DATA_LENGTH_TWO, callbackValues, &callbackResult);
        napi_delete_reference(env, settingInfo.callbackRef);
        napi_delete_reference(env, settingInfo.thisVar);
    } else if (settingInfo.deferred != nullptr) {
        if (result == TELEPHONY_SUCCESS) {
            napi_value promiseValue = nullptr;
            napi_get_null(env, &promiseValue);
            napi_resolve_deferred(env, settingInfo.deferred, promiseValue);
        } else {
            JsError error = NapiUtil::ConverErrorMessageForJs(result);
            napi_reject_deferred(env, settingInfo.deferred,
                NapiCallManagerUtils::CreateErrorMessageWithErrorCode(env, error.errorMessage, error.errorCode));
        }
    }
}

void NapiCallAbilityCallback::ReportStartRttInfoWork(uv_work_t *work, int32_t status)
{
    CallSupplementWorker *dataWorkerData = (CallSupplementWorker *)work->data;
    if (dataWorkerData == nullptr) {
        TELEPHONY_LOGE("dataWorkerData is nullptr!");
        return;
    }
    ReportStartRttInfo(dataWorkerData->info, dataWorkerData->callback);
    delete dataWorkerData;
    dataWorkerData = nullptr;
    delete work;
    work = nullptr;
}

void NapiCallAbilityCallback::ReportStartRttInfo(AppExecFwk::PacMap &resultInfo, EventCallback supplementInfo)
{
    napi_env env = supplementInfo.env;
    int32_t result = resultInfo.GetIntValue("result");
    if (supplementInfo.callbackRef != nullptr) {
        napi_value callbackValues[ARRAY_INDEX_THIRD] = { 0 };
        if (result == TELEPHONY_SUCCESS) {
            callbackValues[ARRAY_INDEX_FIRST] = NapiCallManagerUtils::CreateUndefined(env);
        } else {
            std::string errTip = std::to_string(CALL_ERR_NAPI_INTERFACE_FAILED);
            callbackValues[ARRAY_INDEX_FIRST] = NapiCallManagerUtils::CreateErrorMessage(env, errTip);
            callbackValues[ARRAY_INDEX_SECOND] = NapiCallManagerUtils::CreateUndefined(env);
        }
        napi_value callbackFunc = nullptr;
        napi_get_reference_value(env, supplementInfo.callbackRef, &callbackFunc);
        napi_value thisVar = nullptr;
        napi_get_reference_value(env, supplementInfo.thisVar, &thisVar);
        napi_value callbackResult = nullptr;
        napi_call_function(env, thisVar, callbackFunc, DATA_LENGTH_TWO, callbackValues, &callbackResult);
        napi_delete_reference(env, supplementInfo.callbackRef);
        napi_delete_reference(env, supplementInfo.thisVar);
    } else if (supplementInfo.deferred != nullptr) {
        if (result == TELEPHONY_SUCCESS) {
            napi_value promiseValue = nullptr;
            napi_get_null(env, &promiseValue);
            napi_resolve_deferred(env, supplementInfo.deferred, promiseValue);
        } else {
            std::string errTip = std::to_string(CALL_ERR_NAPI_INTERFACE_FAILED);
            napi_reject_deferred(env, supplementInfo.deferred, NapiCallManagerUtils::CreateErrorMessage(env, errTip));
        }
    }
}

void NapiCallAbilityCallback::ReportStopRttInfoWork(uv_work_t *work, int32_t status)
{
    CallSupplementWorker *dataWorkerData = (CallSupplementWorker *)work->data;
    if (dataWorkerData == nullptr) {
        TELEPHONY_LOGE("dataWorkerData is nullptr!");
        return;
    }
    ReportStopRttInfo(dataWorkerData->info, dataWorkerData->callback);
    delete dataWorkerData;
    dataWorkerData = nullptr;
    delete work;
    work = nullptr;
}

void NapiCallAbilityCallback::ReportStopRttInfo(AppExecFwk::PacMap &resultInfo, EventCallback supplementInfo)
{
    napi_env env = supplementInfo.env;
    int32_t result = resultInfo.GetIntValue("result");
    if (supplementInfo.callbackRef != nullptr) {
        napi_value callbackValues[ARRAY_INDEX_THIRD] = { 0 };
        if (result == TELEPHONY_SUCCESS) {
            callbackValues[ARRAY_INDEX_FIRST] = NapiCallManagerUtils::CreateUndefined(env);
        } else {
            std::string errTip = std::to_string(CALL_ERR_NAPI_INTERFACE_FAILED);
            callbackValues[ARRAY_INDEX_FIRST] = NapiCallManagerUtils::CreateErrorMessage(env, errTip);
            callbackValues[ARRAY_INDEX_SECOND] = NapiCallManagerUtils::CreateUndefined(env);
        }
        napi_value callbackFunc = nullptr;
        napi_get_reference_value(env, supplementInfo.callbackRef, &callbackFunc);
        napi_value thisVar = nullptr;
        napi_get_reference_value(env, supplementInfo.thisVar, &thisVar);
        napi_value callbackResult = nullptr;
        napi_call_function(env, thisVar, callbackFunc, DATA_LENGTH_TWO, callbackValues, &callbackResult);
        napi_delete_reference(env, supplementInfo.callbackRef);
        napi_delete_reference(env, supplementInfo.thisVar);
    } else if (supplementInfo.deferred != nullptr) {
        if (result == TELEPHONY_SUCCESS) {
            napi_value promiseValue = nullptr;
            napi_get_null(env, &promiseValue);
            napi_resolve_deferred(env, supplementInfo.deferred, promiseValue);
        } else {
            std::string errTip = std::to_string(CALL_ERR_NAPI_INTERFACE_FAILED);
            napi_reject_deferred(env, supplementInfo.deferred, NapiCallManagerUtils::CreateErrorMessage(env, errTip));
        }
    }
}

void NapiCallAbilityCallback::ReportCallMediaModeInfoWork(uv_work_t *work, int32_t status)
{
    CallSupplementWorker *dataWorkerData = (CallSupplementWorker *)work->data;
    if (dataWorkerData == nullptr) {
        TELEPHONY_LOGE("dataWorkerData is nullptr!");
        return;
    }
    ReportCallMediaModeInfo(dataWorkerData->info, dataWorkerData->callback);
    delete dataWorkerData;
    dataWorkerData = nullptr;
    delete work;
    work = nullptr;
}

void NapiCallAbilityCallback::ReportCallMediaModeInfo(AppExecFwk::PacMap &resultInfo, EventCallback supplementInfo)
{
    napi_env env = supplementInfo.env;
    int32_t result = resultInfo.GetIntValue("result");
    if (supplementInfo.callbackRef != nullptr) {
        napi_value callbackValues[ARRAY_INDEX_THIRD] = { 0 };
        if (result == TELEPHONY_SUCCESS) {
            callbackValues[ARRAY_INDEX_FIRST] = NapiCallManagerUtils::CreateUndefined(env);
        } else {
            std::string errTip = std::to_string(result);
            callbackValues[ARRAY_INDEX_FIRST] = NapiCallManagerUtils::CreateErrorMessage(env, errTip);
            callbackValues[ARRAY_INDEX_SECOND] = NapiCallManagerUtils::CreateUndefined(env);
        }
        napi_value callbackFunc = nullptr;
        napi_get_reference_value(env, supplementInfo.callbackRef, &callbackFunc);
        napi_value thisVar = nullptr;
        napi_get_reference_value(env, supplementInfo.thisVar, &thisVar);
        napi_value callbackResult = nullptr;
        napi_call_function(env, thisVar, callbackFunc, DATA_LENGTH_TWO, callbackValues, &callbackResult);
        napi_delete_reference(env, supplementInfo.callbackRef);
        napi_delete_reference(env, supplementInfo.thisVar);
    } else if (supplementInfo.deferred != nullptr) {
        if (result == TELEPHONY_SUCCESS) {
            napi_value promiseValue = nullptr;
            napi_get_null(env, &promiseValue);
            napi_resolve_deferred(env, supplementInfo.deferred, promiseValue);
        } else {
            std::string errTip = std::to_string(result);
            napi_reject_deferred(env, supplementInfo.deferred, NapiCallManagerUtils::CreateErrorMessage(env, errTip));
        }
    }
}

void NapiCallAbilityCallback::ReportCallOttWork(uv_work_t *work, int32_t status)
{
    CallOttWorker *dataWorkerData = (CallOttWorker *)work->data;
    if (dataWorkerData == nullptr) {
        TELEPHONY_LOGE("dataWorkerData is nullptr!");
        return;
    }
    ReportCallOtt(dataWorkerData->callback, dataWorkerData->info, dataWorkerData->requestId);
    delete dataWorkerData;
    dataWorkerData = nullptr;
    delete work;
    work = nullptr;
}

int32_t NapiCallAbilityCallback::ReportCallOtt(
    EventCallback &settingInfo, AppExecFwk::PacMap &resultInfo, OttCallRequestId requestId)
{
    napi_value callbackFunc = nullptr;
    napi_env env = settingInfo.env;
    napi_value callbackValues[ARRAY_INDEX_THIRD] = { 0 };
    napi_create_object(env, &callbackValues[ARRAY_INDEX_FIRST]);
    NapiCallManagerUtils::SetPropertyInt32(
        env, callbackValues[ARRAY_INDEX_FIRST], "requestId", static_cast<int32_t>(requestId));
    NapiCallManagerUtils::SetPropertyStringUtf8(
        env, callbackValues[ARRAY_INDEX_FIRST], "phoneNumber", resultInfo.GetStringValue("phoneNumber").c_str());
    NapiCallManagerUtils::SetPropertyStringUtf8(
        env, callbackValues[ARRAY_INDEX_FIRST], "bundleName", resultInfo.GetStringValue("bundleName").c_str());
    NapiCallManagerUtils::SetPropertyInt32(
        env, callbackValues[ARRAY_INDEX_FIRST], "videoState", resultInfo.GetIntValue("videoState"));
    switch (requestId) {
        case OttCallRequestId::OTT_REQUEST_INVITE_TO_CONFERENCE:
            NapiCallManagerUtils::SetPropertyStringUtf8(
                env, callbackValues[ARRAY_INDEX_FIRST], "number", resultInfo.GetStringValue("number").c_str());
            break;
        case OttCallRequestId::OTT_REQUEST_UPDATE_CALL_MEDIA_MODE:
            NapiCallManagerUtils::SetPropertyInt32(
                env, callbackValues[ARRAY_INDEX_FIRST], "callMediaMode", resultInfo.GetIntValue("callMediaMode"));
            break;
        default:
            break;
    }

    napi_get_reference_value(env, settingInfo.callbackRef, &callbackFunc);
    if (callbackFunc == nullptr) {
        TELEPHONY_LOGE("callbackFunc is null!");
        return CALL_ERR_CALLBACK_NOT_EXIST;
    }
    napi_value thisVar = nullptr;
    napi_get_reference_value(env, settingInfo.thisVar, &thisVar);
    napi_value callbackResult = nullptr;
    napi_call_function(env, thisVar, callbackFunc, DATA_LENGTH_ONE, callbackValues, &callbackResult);
    return TELEPHONY_SUCCESS;
}
} // namespace Telephony
} // namespace OHOS
