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

#ifndef MOCK_OHOS_ABILITY_RUNTIME_MOCK_APP_SERVICE_MGR_H
#define MOCK_OHOS_ABILITY_RUNTIME_MOCK_APP_SERVICE_MGR_H

#include "iremote_object.h"
#include "app_service_manager.h"
#include "hilog_wrapper.h"
#include "mock_app_mgr_service.h"

namespace OHOS {
namespace AppExecFwk {
class MockAppServiceMgr : public AppServiceManager {
public:
    MockAppServiceMgr() = default;
    virtual ~MockAppServiceMgr() = default;

    virtual sptr<IRemoteObject> GetAppMgrService() const override
    {
        HILOG_DEBUG("Mock MockAppServiceMgr GetAppMgrService called");
        return new MockAppMgrService();
    }
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // MOCK_OHOS_ABILITY_RUNTIME_MOCK_APP_SERVICE_MGR_H
