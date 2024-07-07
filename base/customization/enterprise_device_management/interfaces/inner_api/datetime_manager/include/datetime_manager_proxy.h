/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef INTERFACES_INNER_API_DATETIME_MANAGER_INCLUDE_DATETIME_MANAGER_PROXY_H
#define INTERFACES_INNER_API_DATETIME_MANAGER_INCLUDE_DATETIME_MANAGER_PROXY_H
#include "enterprise_device_mgr_proxy.h"

namespace OHOS {
namespace EDM {
class DatetimeManagerProxy {
public:
    DatetimeManagerProxy();
    ~DatetimeManagerProxy();
    static std::shared_ptr<DatetimeManagerProxy> GetDatetimeManagerProxy();
    int32_t SetDateTime(AppExecFwk::ElementName &admin, int64_t time);

private:
    static std::shared_ptr<EnterpriseDeviceMgrProxy> proxy_;
    static std::shared_ptr<DatetimeManagerProxy> instance_;
    static std::mutex mutexLock_;
};
} // namespace EDM
} // namespace OHOS
#endif // INTERFACES_INNER_API_DATETIME_MANAGER_INCLUDE_DATETIME_MANAGER_PROXY_H