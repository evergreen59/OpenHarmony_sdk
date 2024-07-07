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

#ifndef LOCATOR_CALLBACK_HOST_H
#define LOCATOR_CALLBACK_HOST_H

#include <shared_mutex>

#include "iremote_stub.h"
#include "napi/native_api.h"
#include "uv.h"

#include "common_utils.h"
#include "constant_definition.h"
#include "i_locator_callback.h"
#include "location.h"

namespace OHOS {
namespace Location {
class LocatorCallbackHost : public IRemoteStub<ILocatorCallback> {
public:
    LocatorCallbackHost();
    virtual ~LocatorCallbackHost();
    virtual int OnRemoteRequest(uint32_t code,
        MessageParcel& data, MessageParcel& reply, MessageOption& option) override;
    void DoSendWork(uv_loop_s *&loop, uv_work_t *&work);
    void DoSendErrorCode(uv_loop_s *&loop, uv_work_t *&work);
    bool SendErrorCode(const int& errorCode);

    void OnLocationReport(const std::unique_ptr<Location>& location) override;
    void OnLocatingStatusChange(const int status) override;
    void OnErrorReport(const int errorCode) override;
    void DeleteAllCallbacks();
    void DeleteHandler();
    void DeleteSuccessHandler();
    void DeleteFailHandler();
    void DeleteCompleteHandler();
    void InitLatch();
    bool IsSystemGeoLocationApi();
    bool IsSingleLocationRequest();
    void CountDown();
    void Wait(int time);
    int GetCount();
    void SetCount(int count);

    template <typename T>
    bool InitContext(T* context)
    {
        if (context == nullptr) {
            LBSLOGE(LOCATOR_CALLBACK, "context == nullptr.");
            return false;
        }
        context->env = env_;
        if (IsSystemGeoLocationApi()) {
            context->callback[SUCCESS_CALLBACK] = successHandlerCb_;
            context->callback[FAIL_CALLBACK] = failHandlerCb_;
            context->callback[COMPLETE_CALLBACK] = completeHandlerCb_;
        } else {
            context->callback[SUCCESS_CALLBACK] = handlerCb_;
            context->deferred = deferred_;
        }
        return true;
    }

    inline napi_env GetEnv() const
    {
        return env_;
    }

    inline void SetEnv(const napi_env& env)
    {
        env_ = env;
    }

    inline napi_ref GetHandleCb() const
    {
        return handlerCb_;
    }

    inline void SetHandleCb(const napi_ref& handlerCb)
    {
        handlerCb_ = handlerCb;
    }

    inline napi_ref GetSuccHandleCb() const
    {
        return successHandlerCb_;
    }

    inline void SetSuccHandleCb(const napi_ref& successHandlerCb)
    {
        successHandlerCb_ = successHandlerCb;
    }

    inline napi_ref GetFailHandleCb() const
    {
        return failHandlerCb_;
    }

    inline void SetFailHandleCb(const napi_ref& failHandlerCb)
    {
        failHandlerCb_ = failHandlerCb;
    }

    inline napi_ref GetCompleteHandleCb() const
    {
        return completeHandlerCb_;
    }

    inline void SetCompleteHandleCb(const napi_ref& completeHandlerCb)
    {
        completeHandlerCb_ = completeHandlerCb;
    }

    inline int GetFixNumber() const
    {
        return fixNumber_;
    }

    inline void SetFixNumber(const int fixNumber)
    {
        fixNumber_ = fixNumber;
    }

    inline napi_deferred GetDeferred() const
    {
        return deferred_;
    }

    inline void SetDeferred(const napi_deferred& deferred)
    {
        deferred_ = deferred;
    }

    inline std::shared_ptr<Location> GetSingleLocation() const
    {
        return singleLocation_;
    }

private:
    napi_env env_;
    napi_ref handlerCb_;
    napi_ref successHandlerCb_;
    napi_ref failHandlerCb_;
    napi_ref completeHandlerCb_;
    int fixNumber_;
    napi_deferred deferred_;
    std::shared_mutex mutex_;
    CountDownLatch* latch_;
    std::shared_ptr<Location> singleLocation_;
};
} // namespace Location
} // namespace OHOS
#endif // LOCATOR_CALLBACK_HOST_H
