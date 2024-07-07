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
#ifndef NFC_NAPI_EVENT_H_
#define NFC_NAPI_EVENT_H_
#include <string>
#include <set>
#include <map>
#include <vector>
#include <shared_mutex>
#include "define.h"
#include "napi/native_api.h"

namespace OHOS {
namespace NFC {
class AsyncEventData {
public:
    napi_env env;
    napi_ref callbackRef;
    napi_value jsEvent;

    explicit AsyncEventData(napi_env e, napi_ref r, napi_value v)
    {
        env = e;
        callbackRef = r;
        jsEvent = v;
    }

    AsyncEventData() = delete;

    virtual ~AsyncEventData() {
    }
};

class RegObj {
public:
    RegObj() : m_regEnv(0), m_regHanderRef(nullptr) {
    }
    explicit RegObj(const napi_env& env, const napi_ref& ref)
    {
        m_regEnv = env;
        m_regHanderRef = ref;
    }

    ~RegObj() {
    }

    napi_env m_regEnv;
    napi_ref m_regHanderRef;
};

namespace NFC {
    static std::shared_mutex g_regInfoMutex;
    static std::map<std::string, std::vector<RegObj>> g_eventRegisterInfo;
}

class NapiEvent {
public:
    bool CheckIsRegister(const std::string& type);
    void EventNotify(AsyncEventData *asyncEvent);

    template<typename T>
    void CheckAndNotify(const std::string& type, const T& obj)
    {
        std::shared_lock<std::shared_mutex> guard(NFC::g_regInfoMutex);
        if (!CheckIsRegister(type)) {
            return;
        }

        std::vector<RegObj>& vecObj = NFC::g_eventRegisterInfo[type];
        for (auto& each : vecObj) {
            napi_value result;
            napi_create_int32(each.m_regEnv, obj, &result);
            AsyncEventData *asyncEvent = new AsyncEventData(each.m_regEnv, each.m_regHanderRef, result);
            if (asyncEvent == nullptr) {
                return;
            }
            EventNotify(asyncEvent);
        }
    }
};

class EventRegister {
public:
    EventRegister() {
    }
    ~EventRegister() {
    }

    static EventRegister& GetInstance();

    void Register(const napi_env& env, const std::string& type, napi_value handler);
    void Unregister(const napi_env& env, const std::string& type, napi_value handler);

private:
    ErrCode RegisterNfcEvents();
    bool IsEventSupport(const std::string& type);
    void DeleteRegisterObj(std::vector<RegObj>& vecRegObjs, napi_value& handler);
    void DeleteAllRegisterObj(std::vector<RegObj>& vecRegObjs);

    static bool isEventRegistered;
};

napi_value On(napi_env env, napi_callback_info cbinfo);
napi_value Off(napi_env env, napi_callback_info cbinfo);
}  // namespace NFC
}  // namespace OHOS

#endif
