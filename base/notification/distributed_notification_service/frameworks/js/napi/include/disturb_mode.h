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
#ifndef BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_FRAMEWORKS_JS_NAPI_INCLUDE_DISTURB_MODE_H
#define BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_FRAMEWORKS_JS_NAPI_INCLUDE_DISTURB_MODE_H

#include "common.h"

namespace OHOS {
namespace NotificationNapi {
using namespace OHOS::Notification;

struct SetDoNotDisturbDateParams {
    NotificationDoNotDisturbDate date;
    bool hasUserId = false;
    int32_t userId = SUBSCRIBE_USER_INIT;
    napi_ref callback = nullptr;
};

struct GetDoNotDisturbDateParams {
    bool hasUserId = false;
    int32_t userId = SUBSCRIBE_USER_INIT;
    napi_ref callback = nullptr;
};

struct AsyncCallbackInfoSetDoNotDisturb {
    napi_env env = nullptr;
    napi_async_work asyncWork = nullptr;
    SetDoNotDisturbDateParams params;
    CallbackPromiseInfo info;
};

struct AsyncCallbackInfoGetDoNotDisturb {
    napi_env env = nullptr;
    napi_async_work asyncWork = nullptr;
    GetDoNotDisturbDateParams params;
    NotificationDoNotDisturbDate date;
    CallbackPromiseInfo info;
};

struct AsyncCallbackInfoSupportDoNotDisturb {
    napi_env env = nullptr;
    napi_async_work asyncWork = nullptr;
    napi_ref callback = nullptr;
    bool isSupported = false;
    CallbackPromiseInfo info;
};

napi_value SetDoNotDisturbDate(napi_env env, napi_callback_info info);
napi_value GetDoNotDisturbDate(napi_env env, napi_callback_info info);
napi_value SupportDoNotDisturbMode(napi_env env, napi_callback_info info);

napi_value ParseParameters(const napi_env &env, const napi_callback_info &info, SetDoNotDisturbDateParams &params);
napi_value ParseParameters(const napi_env &env, const napi_callback_info &info, GetDoNotDisturbDateParams &params);
}  // namespace NotificationNapi
}  // namespace OHOS
#endif  // BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_FRAMEWORKS_JS_NAPI_INCLUDE_DISTURB_MODE_H