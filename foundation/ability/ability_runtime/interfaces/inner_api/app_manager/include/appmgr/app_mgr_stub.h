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

#ifndef OHOS_ABILITY_RUNTIME_APP_MGR_STUB_H
#define OHOS_ABILITY_RUNTIME_APP_MGR_STUB_H

#include <map>

#include "iremote_stub.h"
#include "nocopyable.h"
#include "string_ex.h"
#include "app_mgr_interface.h"

namespace OHOS {
namespace AppExecFwk {
class AppMgrStub : public IRemoteStub<IAppMgr> {
public:
    AppMgrStub();
    virtual ~AppMgrStub();

    virtual int OnRemoteRequest(
        uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

    /**
     * Register application or process state observer.
     * @param observer, ability token.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual int32_t RegisterApplicationStateObserver(const sptr<IApplicationStateObserver> &observer,
        const std::vector<std::string> &bundleNameList = {}) override;

    /**
     * Unregister application or process state observer.
     * @param observer, ability token.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual int32_t UnregisterApplicationStateObserver(const sptr<IApplicationStateObserver> &observer) override;

    /**
     * Get foreground applications.
     * @param list, foreground apps.
     * @return Returns ERR_OK on success, others on failure.
     */
    virtual int32_t GetForegroundApplications(std::vector<AppStateData> &list) override;

private:
    int32_t HandleAttachApplication(MessageParcel &data, MessageParcel &reply);
    int32_t HandleApplicationForegrounded(MessageParcel &data, MessageParcel &reply);
    int32_t HandleApplicationBackgrounded(MessageParcel &data, MessageParcel &reply);
    int32_t HandleApplicationTerminated(MessageParcel &data, MessageParcel &reply);
    int32_t HandleCheckPermission(MessageParcel &data, MessageParcel &reply);
    int32_t HandleAbilityCleaned(MessageParcel &data, MessageParcel &reply);
    int32_t HandleGetAmsMgr(MessageParcel &data, MessageParcel &reply);
    int32_t HandleClearUpApplicationData(MessageParcel &data, MessageParcel &reply);
    int32_t HandleGetAllRunningProcesses(MessageParcel &data, MessageParcel &reply);
    int32_t HandleGetProcessRunningInfosByUserId(MessageParcel &data, MessageParcel &reply);
    int32_t HandleGetProcessRunningInformation(MessageParcel &data, MessageParcel &reply);
    int32_t HandleAddAbilityStageDone(MessageParcel &data, MessageParcel &reply);
    int32_t HandleNotifyMemoryLevel(MessageParcel &data, MessageParcel &reply);
    int32_t HandleStartupResidentProcess(MessageParcel &data, MessageParcel &reply);
    int32_t HandleRegisterApplicationStateObserver(MessageParcel &data, MessageParcel &reply);
    int32_t HandleUnregisterApplicationStateObserver(MessageParcel &data, MessageParcel &reply);
    int32_t HandleGetForegroundApplications(MessageParcel &data, MessageParcel &reply);
    int32_t HandleStartUserTestProcess(MessageParcel &data, MessageParcel &reply);
    int32_t HandleFinishUserTest(MessageParcel &data, MessageParcel &reply);
    int32_t HandleScheduleAcceptWantDone(MessageParcel &data, MessageParcel &reply);
    int32_t HandleGetAbilityRecordsByProcessID(MessageParcel &data, MessageParcel &reply);
    int32_t HandlePreStartNWebSpawnProcess(MessageParcel &data, MessageParcel &reply);
    int32_t HandleStartRenderProcess(MessageParcel &data, MessageParcel &reply);
    int32_t HandleAttachRenderProcess(MessageParcel &data, MessageParcel &reply);
    int32_t HandleGetRenderProcessTerminationStatus(MessageParcel &data, MessageParcel &reply);
    int32_t HandleGetConfiguration(MessageParcel &data, MessageParcel &reply);
    int32_t HandleUpdateConfiguration(MessageParcel &data, MessageParcel &reply);
    int32_t HandleRegisterConfigurationObserver(MessageParcel &data, MessageParcel &reply);
    int32_t HandleUnregisterConfigurationObserver(MessageParcel &data, MessageParcel &reply);
#ifdef ABILITY_COMMAND_FOR_TEST
    int32_t HandleBlockAppServiceDone(MessageParcel &data, MessageParcel &reply);
#endif
    int32_t HandleGetAppRunningStateByBundleName(MessageParcel &data, MessageParcel &reply);
    int32_t HandleNotifyLoadRepairPatch(MessageParcel &data, MessageParcel &reply);
    int32_t HandleNotifyHotReloadPage(MessageParcel &data, MessageParcel &reply);
    int32_t HandleNotifyUnLoadRepairPatch(MessageParcel &data, MessageParcel &reply);
#ifdef BGTASKMGR_CONTINUOUS_TASK_ENABLE
    int32_t HandleSetContinuousTaskProcess(MessageParcel &data, MessageParcel &reply);
#endif

    using AppMgrFunc = int32_t (AppMgrStub::*)(MessageParcel &data, MessageParcel &reply);
    std::map<uint32_t, AppMgrFunc> memberFuncMap_;

    DISALLOW_COPY_AND_MOVE(AppMgrStub);
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // OHOS_ABILITY_RUNTIME_APP_MGR_STUB_H
