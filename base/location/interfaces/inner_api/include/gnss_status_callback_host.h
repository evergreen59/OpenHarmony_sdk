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

#ifndef GNSS_STATUS_CALLBACK_HOST_H
#define GNSS_STATUS_CALLBACK_HOST_H

#include <shared_mutex>

#include "iremote_stub.h"
#include "message_option.h"
#include "message_parcel.h"
#include "napi_util.h"
#include "napi/native_api.h"
#include "uv.h"

#include "i_gnss_status_callback.h"
#include "satellite_status.h"

namespace OHOS {
namespace Location {
class GnssStatusCallbackHost : public IRemoteStub<IGnssStatusCallback> {
public:
    GnssStatusCallbackHost();
    virtual ~GnssStatusCallbackHost();
    virtual int OnRemoteRequest(
        uint32_t code, MessageParcel& data, MessageParcel& reply, MessageOption& option) override;
    bool IsRemoteDied();
    bool Send(std::unique_ptr<SatelliteStatus>& statusInfo);
    void OnStatusChange(const std::unique_ptr<SatelliteStatus>& statusInfo) override;
    void DeleteHandler();
    void UvQueueWork(uv_loop_s* loop, uv_work_t* work);

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

    inline bool GetRemoteDied() const
    {
        return remoteDied_;
    }

    inline void SetRemoteDied(const bool remoteDied)
    {
        remoteDied_ = remoteDied;
    }

private:
    napi_env env_;
    napi_ref handlerCb_;
    bool remoteDied_;
    std::shared_mutex mutex_;
};
} // namespace Location
} // namespace OHOS
#endif // GNSS_STATUS_CALLBACK_HOST_H
