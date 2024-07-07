/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#ifndef MODULETEST_OHOS_ABILITY_RUNTIME_MOCK_APP_MGR_CLIENT_H
#define MODULETEST_OHOS_ABILITY_RUNTIME_MOCK_APP_MGR_CLIENT_H

#include <gmock/gmock.h>
#include "app_mgr_client.h"

namespace OHOS {
namespace AppExecFwk {
class MockAppMgrClient : public AppMgrClient {
public:
    MockAppMgrClient();
    ~MockAppMgrClient();
    MOCK_METHOD5(LoadAbility, AppMgrResultCode(const sptr<IRemoteObject> &, const sptr<IRemoteObject> &,
                                  const AbilityInfo &, const ApplicationInfo &, const AAFwk::Want &));
    MOCK_METHOD2(TerminateAbility, AppMgrResultCode(const sptr<IRemoteObject> &, bool));
    MOCK_METHOD2(UpdateAbilityState, AppMgrResultCode(const sptr<IRemoteObject> &token, const AbilityState state));
    MOCK_METHOD1(KillApplication, AppMgrResultCode(const std::string &));
    MOCK_METHOD1(KillProcessByAbilityToken, AppMgrResultCode(const sptr<IRemoteObject> &token));
    MOCK_METHOD1(KillProcessesByUserId, AppMgrResultCode(int32_t userId));
    MOCK_METHOD1(AbilityAttachTimeOut, void(const sptr<IRemoteObject> &token));
    MOCK_METHOD2(GetRunningProcessInfoByToken, void((const sptr<IRemoteObject> &token,
        AppExecFwk::RunningProcessInfo &info)));
    MOCK_METHOD1(GetAllRunningProcesses, AppMgrResultCode(std::vector<RunningProcessInfo> &info));

    AppMgrResultCode GetProcessRunningInfosByUserId(std::vector<RunningProcessInfo> &info, int32_t userId);

    AppMgrResultCode AbilityBehaviorAnalysis(const sptr<IRemoteObject> &token, const sptr<IRemoteObject> &preToken,
        const int32_t visibility, const int32_t perceptibility, const int32_t connectionState) override;
    AppMgrResultCode ConnectAppMgrService() override;
    AppMgrResultCode RegisterAppStateCallback(const sptr<IAppStateCallback> &callback) override;
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // MODULETEST_OHOS_ABILITY_RUNTIME_MOCK_APP_MGR_CLIENT_H
