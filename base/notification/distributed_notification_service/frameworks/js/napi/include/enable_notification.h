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
#ifndef BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_FRAMEWORKS_JS_NAPI_INCLUDE_ENABLE_NOTIFICATION_H
#define BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_FRAMEWORKS_JS_NAPI_INCLUDE_ENABLE_NOTIFICATION_H

#include <utility>

#include "common.h"

namespace OHOS {
namespace NotificationNapi {
using namespace OHOS::Notification;
using RequestEnableTask = std::function<void(bool)>;

struct IsEnableParams {
    NotificationBundleOption option;
    napi_ref callback = nullptr;
    bool hasBundleOption = false;
    int32_t userId = SUBSCRIBE_USER_INIT;
    bool hasUserId = false;
    bool allowToPop = false;
};

struct AsyncCallbackInfoIsEnable {
    napi_env env = nullptr;
    napi_async_work asyncWork = nullptr;
    IsEnableParams params;
    CallbackPromiseInfo info;
    bool newInterface = false;
    bool allowed = false;
};

struct EnableParams {
    NotificationBundleOption option;
    bool enable = false;
    napi_ref callback = nullptr;
};

struct AsyncCallbackInfoEnable {
    napi_env env = nullptr;
    napi_async_work asyncWork = nullptr;
    EnableParams params;
    CallbackPromiseInfo info;
};

napi_value EnableNotification(napi_env env, napi_callback_info info);
napi_value IsNotificationEnabled(napi_env env, napi_callback_info info);
napi_value IsNotificationEnabledSelf(napi_env env, napi_callback_info info);
napi_value RequestEnableNotification(napi_env env, napi_callback_info info);
napi_value ParseParameters(const napi_env &env, const napi_callback_info &info, EnableParams &params);
napi_value ParseParameters(const napi_env &env, const napi_callback_info &info, IsEnableParams &params);

}  // namespace NotificationNapi
}  // namespace OHOS
#endif  // BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_FRAMEWORKS_JS_NAPI_INCLUDE_ENABLE_NOTIFICATION_H