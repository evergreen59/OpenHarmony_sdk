/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include "network_utils.h"

#include <cinttypes>

#include "hril_network_parcel.h"
#include "network_search_manager.h"

namespace OHOS {
namespace Telephony {
/**
 * @brief function pointer of class ITelRilManager.
 *
 */
using RilFunc_Event = int32_t (ITelRilManager::*)(int32_t, const AppExecFwk::InnerEvent::Pointer &);
using RilFunc_Int_Event = int32_t (ITelRilManager::*)(int32_t, int32_t, const AppExecFwk::InnerEvent::Pointer &);
using RilFunc_Int_Int_Event = int32_t (ITelRilManager::*)(
    int32_t, int32_t, int32_t, const AppExecFwk::InnerEvent::Pointer &);
using RilFunc_Int_String_Event = int32_t (ITelRilManager::*)(
    int32_t, int32_t, std::string, const AppExecFwk::InnerEvent::Pointer &);

// group
static const int32_t GSM = static_cast<int32_t>(RadioProtocolTech::RADIO_PROTOCOL_TECH_GSM);
static const int32_t CDMA = static_cast<int32_t>(RadioProtocolTech::RADIO_PROTOCOL_TECH_1XRTT);
static const int32_t EVDO = static_cast<int32_t>(RadioProtocolTech::RADIO_PROTOCOL_TECH_EVDO) |
                            static_cast<int32_t>(RadioProtocolTech::RADIO_PROTOCOL_TECH_EHRPD);
static const int32_t WCDMA = static_cast<int32_t>(RadioProtocolTech::RADIO_PROTOCOL_TECH_WCDMA) |
                             static_cast<int32_t>(RadioProtocolTech::RADIO_PROTOCOL_TECH_HSPA) |
                             static_cast<int32_t>(RadioProtocolTech::RADIO_PROTOCOL_TECH_HSPAP);
static const int32_t TDSCDMA = static_cast<int32_t>(RadioProtocolTech::RADIO_PROTOCOL_TECH_TD_SCDMA);
static const int32_t LTE = static_cast<int32_t>(RadioProtocolTech::RADIO_PROTOCOL_TECH_LTE) |
                           static_cast<int32_t>(RadioProtocolTech::RADIO_PROTOCOL_TECH_LTE_CA);
static const int32_t NR = static_cast<int32_t>(RadioProtocolTech::RADIO_PROTOCOL_TECH_NR);

// generation
static const int32_t RAF_2G = GSM | CDMA;
static const int32_t RAF_3G = WCDMA | EVDO | TDSCDMA;
static const int32_t RAF_4G = LTE;
static const int32_t RAF_5G = NR;
// auto mode , support all radio mode
static const int32_t RAF_AUTO = RAF_2G | RAF_3G | RAF_4G | RAF_5G;

static const std::map<int32_t, PreferredNetworkMode> mapNetworkModeFromRaf = {
    { RAF_AUTO, PreferredNetworkMode::CORE_NETWORK_MODE_AUTO },
    { GSM, PreferredNetworkMode::CORE_NETWORK_MODE_GSM },
    { WCDMA, PreferredNetworkMode::CORE_NETWORK_MODE_WCDMA },
    { LTE, PreferredNetworkMode::CORE_NETWORK_MODE_LTE },
    { LTE | WCDMA, PreferredNetworkMode::CORE_NETWORK_MODE_LTE_WCDMA },
    { LTE | WCDMA | GSM, PreferredNetworkMode::CORE_NETWORK_MODE_LTE_WCDMA_GSM },
    { CDMA, PreferredNetworkMode::CORE_NETWORK_MODE_CDMA },
    { WCDMA | GSM, PreferredNetworkMode::CORE_NETWORK_MODE_WCDMA_GSM },
    { EVDO, PreferredNetworkMode::CORE_NETWORK_MODE_EVDO },
    { EVDO | CDMA, PreferredNetworkMode::CORE_NETWORK_MODE_EVDO_CDMA },
    { WCDMA | GSM | EVDO | CDMA, PreferredNetworkMode::CORE_NETWORK_MODE_WCDMA_GSM_EVDO_CDMA },
    { LTE | EVDO | CDMA, PreferredNetworkMode::CORE_NETWORK_MODE_LTE_EVDO_CDMA },
    { LTE | WCDMA | GSM | EVDO | CDMA, PreferredNetworkMode::CORE_NETWORK_MODE_LTE_WCDMA_GSM_EVDO_CDMA },
    { TDSCDMA, PreferredNetworkMode::CORE_NETWORK_MODE_TDSCDMA },
    { TDSCDMA | GSM, PreferredNetworkMode::CORE_NETWORK_MODE_TDSCDMA_GSM },
    { TDSCDMA | WCDMA, PreferredNetworkMode::CORE_NETWORK_MODE_TDSCDMA_WCDMA },
    { TDSCDMA | WCDMA | GSM, PreferredNetworkMode::CORE_NETWORK_MODE_TDSCDMA_WCDMA_GSM },
    { LTE | TDSCDMA, PreferredNetworkMode::CORE_NETWORK_MODE_LTE_TDSCDMA },
    { LTE | TDSCDMA | GSM, PreferredNetworkMode::CORE_NETWORK_MODE_LTE_TDSCDMA_GSM },
    { LTE | TDSCDMA | WCDMA, PreferredNetworkMode::CORE_NETWORK_MODE_LTE_TDSCDMA_WCDMA },
    { LTE | TDSCDMA | WCDMA | GSM, PreferredNetworkMode::CORE_NETWORK_MODE_LTE_TDSCDMA_WCDMA_GSM },
    { TDSCDMA | WCDMA | GSM | EVDO | CDMA, PreferredNetworkMode::CORE_NETWORK_MODE_TDSCDMA_WCDMA_GSM_EVDO_CDMA },
    { LTE | TDSCDMA | WCDMA | GSM | EVDO | CDMA,
        PreferredNetworkMode::CORE_NETWORK_MODE_LTE_TDSCDMA_WCDMA_GSM_EVDO_CDMA },
    { NR, PreferredNetworkMode::CORE_NETWORK_MODE_NR },
    { NR | LTE, PreferredNetworkMode::CORE_NETWORK_MODE_NR_LTE },
    { NR | LTE | WCDMA, PreferredNetworkMode::CORE_NETWORK_MODE_NR_LTE_WCDMA },
    { NR | LTE | WCDMA | GSM, PreferredNetworkMode::CORE_NETWORK_MODE_NR_LTE_WCDMA_GSM },
    { NR | LTE | EVDO | CDMA, PreferredNetworkMode::CORE_NETWORK_MODE_NR_LTE_EVDO_CDMA },
    { NR | LTE | WCDMA | GSM | EVDO | CDMA, PreferredNetworkMode::CORE_NETWORK_MODE_NR_LTE_WCDMA_GSM_EVDO_CDMA },
    { NR | LTE | TDSCDMA, PreferredNetworkMode::CORE_NETWORK_MODE_NR_LTE_TDSCDMA },
    { NR | LTE | TDSCDMA | GSM, PreferredNetworkMode::CORE_NETWORK_MODE_NR_LTE_TDSCDMA_GSM },
    { NR | LTE | TDSCDMA | WCDMA, PreferredNetworkMode::CORE_NETWORK_MODE_NR_LTE_TDSCDMA_WCDMA },
    { NR | LTE | TDSCDMA | WCDMA | GSM, PreferredNetworkMode::CORE_NETWORK_MODE_NR_LTE_TDSCDMA_WCDMA_GSM },
    { NR | LTE | TDSCDMA | GSM | EVDO | CDMA,
        PreferredNetworkMode::CORE_NETWORK_MODE_NR_LTE_TDSCDMA_WCDMA_GSM_EVDO_CDMA },
};

static const int32_t ALL_RAF[] = { GSM, CDMA, EVDO, WCDMA, TDSCDMA, LTE, NR };

PreferredNetworkMode NetworkUtils::GetNetworkModeFromRaf(int32_t raf)
{
    for (int32_t i = 0; i < static_cast<int32_t>(sizeof(ALL_RAF) / sizeof(ALL_RAF[0])); i++) {
        if (static_cast<uint32_t>(ALL_RAF[i]) & static_cast<uint32_t>(raf)) {
            raf = static_cast<int32_t>(static_cast<uint32_t>(ALL_RAF[i]) | static_cast<uint32_t>(raf));
        }
    }

    auto iter = mapNetworkModeFromRaf.find(raf);
    if (iter != mapNetworkModeFromRaf.end()) {
        return iter->second;
    }
    return PreferredNetworkMode::CORE_NETWORK_MODE_AUTO;
}

static const std::map<PreferredNetworkMode, int32_t> mapRafFromNetworkMode = {
    { PreferredNetworkMode::CORE_NETWORK_MODE_AUTO, RAF_AUTO },
    { PreferredNetworkMode::CORE_NETWORK_MODE_GSM, GSM },
    { PreferredNetworkMode::CORE_NETWORK_MODE_WCDMA, WCDMA },
    { PreferredNetworkMode::CORE_NETWORK_MODE_LTE, LTE },
    { PreferredNetworkMode::CORE_NETWORK_MODE_LTE_WCDMA, LTE | WCDMA },
    { PreferredNetworkMode::CORE_NETWORK_MODE_LTE_WCDMA_GSM, LTE | WCDMA | GSM },
    { PreferredNetworkMode::CORE_NETWORK_MODE_WCDMA_GSM, WCDMA | GSM },
    { PreferredNetworkMode::CORE_NETWORK_MODE_CDMA, CDMA },
    { PreferredNetworkMode::CORE_NETWORK_MODE_EVDO, EVDO },
    { PreferredNetworkMode::CORE_NETWORK_MODE_EVDO_CDMA, EVDO | CDMA },
    { PreferredNetworkMode::CORE_NETWORK_MODE_WCDMA_GSM_EVDO_CDMA, WCDMA | GSM | EVDO | CDMA },
    { PreferredNetworkMode::CORE_NETWORK_MODE_LTE_EVDO_CDMA, LTE | EVDO | CDMA },
    { PreferredNetworkMode::CORE_NETWORK_MODE_LTE_WCDMA_GSM_EVDO_CDMA, LTE | WCDMA | GSM | EVDO | CDMA },
    { PreferredNetworkMode::CORE_NETWORK_MODE_TDSCDMA, TDSCDMA },
    { PreferredNetworkMode::CORE_NETWORK_MODE_TDSCDMA_GSM, TDSCDMA | GSM },
    { PreferredNetworkMode::CORE_NETWORK_MODE_TDSCDMA_WCDMA, TDSCDMA | WCDMA },
    { PreferredNetworkMode::CORE_NETWORK_MODE_TDSCDMA_WCDMA_GSM, TDSCDMA | WCDMA | GSM },
    { PreferredNetworkMode::CORE_NETWORK_MODE_LTE_TDSCDMA, LTE | TDSCDMA },
    { PreferredNetworkMode::CORE_NETWORK_MODE_LTE_TDSCDMA_GSM, LTE | TDSCDMA | GSM },
    { PreferredNetworkMode::CORE_NETWORK_MODE_LTE_TDSCDMA_WCDMA, LTE | TDSCDMA | WCDMA },
    { PreferredNetworkMode::CORE_NETWORK_MODE_LTE_TDSCDMA_WCDMA_GSM, LTE | TDSCDMA | WCDMA | GSM },
    { PreferredNetworkMode::CORE_NETWORK_MODE_TDSCDMA_WCDMA_GSM_EVDO_CDMA, TDSCDMA | WCDMA | GSM | EVDO | CDMA },
    { PreferredNetworkMode::CORE_NETWORK_MODE_LTE_TDSCDMA_WCDMA_GSM_EVDO_CDMA,
        LTE | TDSCDMA | WCDMA | GSM | EVDO | CDMA },
    { PreferredNetworkMode::CORE_NETWORK_MODE_NR, NR },
    { PreferredNetworkMode::CORE_NETWORK_MODE_NR_LTE, NR | LTE },
    { PreferredNetworkMode::CORE_NETWORK_MODE_NR_LTE_WCDMA, NR | LTE | WCDMA },
    { PreferredNetworkMode::CORE_NETWORK_MODE_NR_LTE_WCDMA_GSM, NR | LTE | WCDMA | GSM },
    { PreferredNetworkMode::CORE_NETWORK_MODE_NR_LTE_EVDO_CDMA, NR | LTE | EVDO | CDMA },
    { PreferredNetworkMode::CORE_NETWORK_MODE_NR_LTE_WCDMA_GSM_EVDO_CDMA, NR | LTE | WCDMA | GSM | EVDO | CDMA },
    { PreferredNetworkMode::CORE_NETWORK_MODE_NR_LTE_TDSCDMA, NR | LTE | TDSCDMA },
    { PreferredNetworkMode::CORE_NETWORK_MODE_NR_LTE_TDSCDMA_GSM, NR | LTE | TDSCDMA | GSM },
    { PreferredNetworkMode::CORE_NETWORK_MODE_NR_LTE_TDSCDMA_WCDMA, NR | LTE | TDSCDMA | WCDMA },
    { PreferredNetworkMode::CORE_NETWORK_MODE_NR_LTE_TDSCDMA_WCDMA_GSM, NR | LTE | TDSCDMA | WCDMA | GSM },
    { PreferredNetworkMode::CORE_NETWORK_MODE_NR_LTE_TDSCDMA_WCDMA_GSM_EVDO_CDMA,
        NR | LTE | TDSCDMA | GSM | EVDO | CDMA },
};

std::unordered_map<int64_t, std::shared_ptr<NetworkSearchCallbackInfo>> NetworkUtils::networkSearchCacheMap_;
std::mutex NetworkUtils::callbackMapMutex_;
std::atomic<int64_t> NetworkUtils::callbackIndex64bit_ = MIN_INDEX;
int32_t NetworkUtils::GetRafFromNetworkMode(PreferredNetworkMode PreferredNetworkMode)
{
    auto iter = mapRafFromNetworkMode.find(PreferredNetworkMode);
    if (iter != mapRafFromNetworkMode.end()) {
        return iter->second;
    }
    return static_cast<int32_t>(RadioProtocolTech::RADIO_PROTOCOL_TECH_UNKNOWN);
}

std::vector<std::string> NetworkUtils::SplitString(const std::string &inputString, const std::string &flag)
{
    std::vector<std::string> result;
    if (inputString.empty()) {
        TELEPHONY_LOGE("inputString is null");
        return result;
    }
    std::string::size_type start = 0;
    std::string::size_type position = 0;
    while ((position = inputString.find(flag, start)) != std::string::npos) {
        result.push_back(inputString.substr(start, position - start));
        start = position + flag.size();
    }
    if (start != inputString.size()) {
        result.push_back(inputString.substr(start, inputString.size() - start));
    }
    return result;
}

bool NetworkUtils::AddNetworkSearchCallBack(int64_t index, std::shared_ptr<NetworkSearchCallbackInfo> &callback)
{
    TELEPHONY_LOGI("NetworkUtils::AddNetworkSearchCallBack index=(%{public}" PRId64 ")", index);
    if (callback != nullptr) {
        std::lock_guard<std::mutex> guard(callbackMapMutex_);
        auto result = networkSearchCacheMap_.emplace(index, callback);
        return result.second;
    }
    TELEPHONY_LOGE("NetworkUtils::AddNetworkSearchCallBack callback is null!");
    return false;
}

int64_t NetworkUtils::GetCallbackIndex64bit()
{
    if (callbackIndex64bit_ > MAX_INDEX || callbackIndex64bit_ < MIN_INDEX) {
        callbackIndex64bit_ = MIN_INDEX;
    }
    return ++callbackIndex64bit_;
}

std::shared_ptr<NetworkSearchCallbackInfo> NetworkUtils::FindNetworkSearchCallback(int64_t index)
{
    TELEPHONY_LOGI("NetworkUtils::FindNetworkSearchCallback index=%{public}" PRId64 "", index);

    std::lock_guard<std::mutex> guard(callbackMapMutex_);
    auto iter = networkSearchCacheMap_.find(index);
    if (iter != networkSearchCacheMap_.end()) {
        std::shared_ptr<NetworkSearchCallbackInfo> callback = iter->second;
        return callback;
    }
    return nullptr;
}

bool NetworkUtils::RemoveCallbackFromMap(int64_t index)
{
    TELEPHONY_LOGI("NetworkUtils::RemoveCallbackFromMap index=%{public}" PRId64 "", index);
    std::lock_guard<std::mutex> guard(callbackMapMutex_);
    return (networkSearchCacheMap_.erase(index));
}

const std::map<RadioEvent, std::any> EventSender::mapFunctions_ = {
    { RadioEvent::RADIO_GET_NETWORK_SELECTION_MODE, &ITelRilManager::GetNetworkSelectionMode },
    { RadioEvent::RADIO_SET_NETWORK_SELECTION_MODE, &ITelRilManager::SetNetworkSelectionMode },
    { RadioEvent::RADIO_GET_PREFERRED_NETWORK_MODE, &ITelRilManager::GetPreferredNetwork },
    { RadioEvent::RADIO_SET_PREFERRED_NETWORK_MODE, &ITelRilManager::SetPreferredNetwork },
    { RadioEvent::RADIO_SET_STATUS, &ITelRilManager::SetRadioState },
    { RadioEvent::RADIO_GET_STATUS, &ITelRilManager::GetRadioState },
    { RadioEvent::RADIO_GET_IMEI, &ITelRilManager::GetImei },
    { RadioEvent::RADIO_GET_MEID, &ITelRilManager::GetMeid },
    { RadioEvent::RADIO_NETWORK_SEARCH_RESULT, &ITelRilManager::GetNetworkSearchInformation },
    { RadioEvent::RADIO_GET_VOICE_TECH, &ITelRilManager::GetVoiceRadioTechnology },
    { RadioEvent::RADIO_OPERATOR, &ITelRilManager::GetOperatorInfo },
};

AppExecFwk::InnerEvent::Pointer EventSender::GetEvent(int32_t slotId, RadioEvent radioEvent, int32_t param)
{
    AppExecFwk::InnerEvent::Pointer event(nullptr, nullptr);
    std::shared_ptr<NetworkSearchManager> nsm = networkSearchManager_.lock();
    if (nsm == nullptr) {
        TELEPHONY_LOGE("failed to get NetworkSearchManager");
        return event;
    }

    auto inner = nsm->FindManagerInner(slotId);
    if (inner != nullptr) {
        event = AppExecFwk::InnerEvent::Get(static_cast<int32_t>(radioEvent), param);
        if (event == nullptr) {
            return event;
        }
        event->SetOwner(inner->networkSearchHandler_);
        return event;
    }
    return event;
}

AppExecFwk::InnerEvent::Pointer EventSender::GetEvent(int32_t slotId, RadioEvent radioEvent)
{
    AppExecFwk::InnerEvent::Pointer event(nullptr, nullptr);
    std::shared_ptr<NetworkSearchManager> nsm = networkSearchManager_.lock();
    if (nsm == nullptr) {
        TELEPHONY_LOGE("failed to get NetworkSearchManager");
        return event;
    }

    auto inner = nsm->FindManagerInner(slotId);
    if (inner != nullptr) {
        event = AppExecFwk::InnerEvent::Get(static_cast<int32_t>(radioEvent));
        if (event == nullptr) {
            return event;
        }
        event->SetOwner(inner->networkSearchHandler_);
        return event;
    }
    return event;
}

AppExecFwk::InnerEvent::Pointer EventSender::GetEvent(
    int32_t slotId, RadioEvent radioEvent, int32_t param, const sptr<INetworkSearchCallback> &callback)
{
    AppExecFwk::InnerEvent::Pointer event(nullptr, nullptr);
    std::shared_ptr<NetworkSearchManager> nsm = networkSearchManager_.lock();
    if (nsm == nullptr) {
        TELEPHONY_LOGE("failed to get NetworkSearchManager");
        return event;
    }

    auto inner = nsm->FindManagerInner(slotId);
    if (inner != nullptr) {
        int64_t index = NetworkUtils::GetCallbackIndex64bit();
        std::shared_ptr<NetworkSearchCallbackInfo> callbackInfo =
            std::make_shared<NetworkSearchCallbackInfo>(param, callback);
        if (callbackInfo == nullptr) {
            TELEPHONY_LOGE("EventSender::GetEvent callbackInfo is null!! slotId:%{public}d", slotId);
            return event;
        }
        if (!NetworkUtils::AddNetworkSearchCallBack(index, callbackInfo)) {
            TELEPHONY_LOGE("EventSender::GetEvent AddNetworkSearchCallBack Error!! slotId:%{public}d", slotId);
            return event;
        }
        event = AppExecFwk::InnerEvent::Get(static_cast<int32_t>(radioEvent), index);
        if (event == nullptr) {
            NetworkUtils::RemoveCallbackFromMap(index);
            return event;
        }
        event->SetOwner(inner->networkSearchHandler_);
        return event;
    }
    return event;
}

bool EventSender::SendBase(int32_t slotId, RadioEvent radioEvent)
{
    auto fun = GetFunctionOfEvent<RilFunc_Event>(radioEvent);
    std::tuple<int32_t, RadioEvent, int32_t, const sptr<INetworkSearchCallback> *, RilFunc_Event> parameters(
        slotId, radioEvent, 0, nullptr, fun);
    return Send<EventGetMode::GET_EVENT_BY_HANDLERID, RilFunc_Event>(parameters);
}

bool EventSender::SendBase(int32_t slotId, RadioEvent radioEvent, int32_t param)
{
    auto fun = GetFunctionOfEvent<RilFunc_Int_Event>(radioEvent);
    std::tuple<int32_t, RadioEvent, int32_t, const sptr<INetworkSearchCallback> *, RilFunc_Int_Event> parameters(
        slotId, radioEvent, param, nullptr, fun);
    return Send<EventGetMode::GET_EVENT_BY_PARAM, RilFunc_Int_Event, int32_t>(parameters, param);
}

bool EventSender::SendBase(int32_t slotId, RadioEvent radioEvent, int32_t firstParam, int32_t secondParam)
{
    auto fun = GetFunctionOfEvent<RilFunc_Int_Int_Event>(radioEvent);
    std::tuple<int32_t, RadioEvent, int32_t, const sptr<INetworkSearchCallback> *, RilFunc_Int_Int_Event> parameters(
        slotId, radioEvent, firstParam, nullptr, fun);
    return Send<EventGetMode::GET_EVENT_BY_PARAM, RilFunc_Int_Int_Event, int32_t, int32_t>(
        parameters, firstParam, secondParam);
}

bool EventSender::SendBase(int32_t slotId, RadioEvent radioEvent, int32_t firstParam, std::string secondParam)
{
    auto fun = GetFunctionOfEvent<RilFunc_Int_String_Event>(radioEvent);
    std::tuple<int32_t, RadioEvent, int32_t, const sptr<INetworkSearchCallback> *, RilFunc_Int_String_Event> parameters(
        slotId, radioEvent, firstParam, nullptr, fun);
    return Send<EventGetMode::GET_EVENT_BY_PARAM, RilFunc_Int_String_Event, int32_t, std::string>(
        parameters, firstParam, secondParam);
}

bool EventSender::SendCallback(int32_t slotId, RadioEvent radioEvent, const sptr<INetworkSearchCallback> *callback)
{
    auto fun = GetFunctionOfEvent<RilFunc_Event>(radioEvent);
    std::tuple<int32_t, RadioEvent, int32_t, const sptr<INetworkSearchCallback> *, RilFunc_Event> parameters(
        slotId, radioEvent, 0, callback, fun);
    return Send<EventGetMode::GET_EVENT_BY_INDEX, RilFunc_Event>(parameters);
}

bool EventSender::SendCallback(
    int32_t slotId, RadioEvent radioEvent, const sptr<INetworkSearchCallback> *callback, int32_t param)
{
    auto fun = GetFunctionOfEvent<RilFunc_Event>(radioEvent);
    std::tuple<int32_t, RadioEvent, int32_t, const sptr<INetworkSearchCallback> *, RilFunc_Event> parameters(
        slotId, radioEvent, param, callback, fun);
    return Send<EventGetMode::GET_EVENT_BY_PARAM, RilFunc_Event>(parameters);
}

bool EventSender::SendCallbackEx(
    int32_t slotId, RadioEvent radioEvent, const sptr<INetworkSearchCallback> *callback, int32_t param)
{
    auto fun = GetFunctionOfEvent<RilFunc_Int_Event>(radioEvent);
    std::tuple<int32_t, RadioEvent, int32_t, const sptr<INetworkSearchCallback> *, RilFunc_Int_Event> parameters(
        slotId, radioEvent, param, callback, fun);
    return Send<EventGetMode::GET_EVENT_BY_INDEX, RilFunc_Int_Event, int32_t>(parameters, param);
}

bool EventSender::SendCallback(int32_t slotId, RadioEvent radioEvent, const sptr<INetworkSearchCallback> *callback,
    int32_t firstParam, int32_t secondParam)
{
    auto fun = GetFunctionOfEvent<RilFunc_Int_Int_Event>(radioEvent);
    std::tuple<int32_t, RadioEvent, int32_t, const sptr<INetworkSearchCallback> *, RilFunc_Int_Int_Event> parameters(
        slotId, radioEvent, firstParam, callback, fun);
    return Send<EventGetMode::GET_EVENT_BY_INDEX, RilFunc_Int_Int_Event, int32_t, int32_t>(
        parameters, firstParam, secondParam);
}

bool EventSender::SendCallback(int32_t slotId, RadioEvent radioEvent, const sptr<INetworkSearchCallback> *callback,
    int32_t firstParam, std::string secondParam)
{
    auto fun = GetFunctionOfEvent<RilFunc_Int_String_Event>(radioEvent);
    std::tuple<int32_t, RadioEvent, int32_t, const sptr<INetworkSearchCallback> *, RilFunc_Int_String_Event> parameters(
        slotId, radioEvent, firstParam, callback, fun);
    return Send<EventGetMode::GET_EVENT_BY_INDEX, RilFunc_Int_String_Event, int32_t, std::string>(
        parameters, firstParam, secondParam);
}
} // namespace Telephony
} // namespace OHOS
