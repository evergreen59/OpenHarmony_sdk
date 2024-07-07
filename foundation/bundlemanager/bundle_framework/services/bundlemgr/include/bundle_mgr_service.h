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

#ifndef FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BUNDLE_MGR_SERVICE_H
#define FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BUNDLE_MGR_SERVICE_H

#include <memory>

#include "singleton.h"
#include "system_ability.h"
#include "thread_pool.h"

#ifdef BUNDLE_FRAMEWORK_APP_CONTROL
#include "app_control_manager_host_impl.h"
#endif
#ifdef DEVICE_MANAGER_ENABLE
#include "bms_device_manager.h"
#endif
#include "bms_param.h"
#ifdef BUNDLE_FRAMEWORK_FREE_INSTALL
#include "aging/bundle_aging_mgr.h"
#include "bundle_connect_ability_mgr.h"
#include "bundle_distributed_manager.h"
#endif
#ifdef BUNDLE_FRAMEWORK_DEFAULT_APP
#include "default_app_host_impl.h"
#endif
#include "bundle_constants.h"
#include "bundle_data_mgr.h"
#include "bundle_installer_host.h"
#include "bundle_mgr_host_impl.h"
#include "bundle_mgr_service_event_handler.h"
#include "bundle_user_mgr_host_impl.h"
#include "hidump_helper.h"
#ifdef BUNDLE_FRAMEWORK_QUICK_FIX
#include "quick_fix_manager_host_impl.h"
#endif

namespace OHOS {
namespace AppExecFwk {
class BundleMgrService : public SystemAbility {
    DECLARE_DELAYED_SINGLETON(BundleMgrService);
    DECLEAR_SYSTEM_ABILITY(BundleMgrService);

public:
    /**
     * @brief Start the bundle manager service.
     * @return
     */
    virtual void OnStart() override;
    /**
     * @brief Stop the bundle manager service.
     * @return
     */
    virtual void OnStop() override;
    /**
     * @brief Check whether if the bundle manager service is ready.
     * @return Returns true if the bundle manager service is ready; returns false otherwise.
     */
    bool IsServiceReady() const;

    const std::shared_ptr<BundleDataMgr> GetDataMgr() const;
#ifdef BUNDLE_FRAMEWORK_FREE_INSTALL
    const std::shared_ptr<BundleAgingMgr> GetAgingMgr() const;
    /**
     * @brief Get a util object for FA  Distribution center
     * @return Returns the pointer of BundleConnectAbility object.
     */
    const std::shared_ptr<BundleConnectAbilityMgr> GetConnectAbility() const;

    const std::shared_ptr<BundleDistributedManager> GetBundleDistributedManager() const;
#endif
    /**
     * @brief Get a IBundleInstaller object for IPC
     * @return Returns the pointer of IBundleInstaller object.
     */
    sptr<IBundleInstaller> GetBundleInstaller() const;
    /**
     * @brief Get a IBundleUserMgr object for IPC
     * @return Returns the pointer of IBundleUserMgr object.
     */
    sptr<BundleUserMgrHostImpl> GetBundleUserMgr() const;

#ifdef BUNDLE_FRAMEWORK_DEFAULT_APP
    sptr<IDefaultApp> GetDefaultAppProxy() const;
#endif

#ifdef BUNDLE_FRAMEWORK_APP_CONTROL
    sptr<IAppControlMgr> GetAppControlProxy() const;
#endif

#ifdef DEVICE_MANAGER_ENABLE
    const std::shared_ptr<BmsDeviceManager> GetDeviceManager() const;
#endif

#ifdef BUNDLE_FRAMEWORK_QUICK_FIX
    sptr<QuickFixManagerHostImpl> GetQuickFixManagerProxy() const;
#endif
    /**
     * @brief Check all user.
     */
    void CheckAllUser();
    /**
     * @brief register bms sa to samgr
     */
    void RegisterService();
    /**
     * @brief send bundle scan finished common event
     */
    void NotifyBundleScanStatus();
    /**
     * @brief Process hidump.
     * @param args Indicates the args.
     * @param result Indicates the result.
     * @return Returns whether the interface is called successfully.
     */
    bool Hidump(const std::vector<std::string> &args, std::string& result) const;

    void RegisterDataMgr(std::shared_ptr<BundleDataMgr> dataMgrImpl);

    ThreadPool &GetThreadPool();

    const std::shared_ptr<BmsParam> GetBmsParam() const;

protected:
    void OnAddSystemAbility(int32_t systemAbilityId, const std::string& deviceId) override;
    void OnRemoveSystemAbility(int32_t systemAbilityId, const std::string& deviceId) override;

private:
    bool Init();
    void SelfClean();

    void InitThreadPool();
    void InitBmsParam();
    bool InitBundleMgrHost();
    bool InitBundleInstaller();
    void InitBundleDataMgr();
    bool InitBundleUserMgr();
    bool InitBundleEventHandler();
    void InitDeviceManager();
    void InitHidumpHelper();
    void InitFreeInstall();
    bool InitDefaultApp();
    bool InitAppControl();
    bool InitQuickFixManager();

private:
    bool ready_ = false;
    bool registerToService_ = false;
    bool notifyBundleScanStatus = false;
    std::shared_ptr<EventRunner> runner_;
    std::shared_ptr<BMSEventHandler> handler_;
    std::shared_ptr<BundleDataMgr> dataMgr_;
#ifdef DEVICE_MANAGER_ENABLE
    std::shared_ptr<BmsDeviceManager> deviceManager_;
#endif
    std::shared_ptr<HidumpHelper> hidumpHelper_;
#ifdef BUNDLE_FRAMEWORK_FREE_INSTALL
    std::shared_ptr<BundleAgingMgr> agingMgr_;
    std::shared_ptr<BundleConnectAbilityMgr> connectAbilityMgr_;
    std::shared_ptr<BundleDistributedManager> bundleDistributedManager_;
#endif
    sptr<BundleMgrHostImpl> host_;
    sptr<BundleInstallerHost> installer_;
    sptr<BundleUserMgrHostImpl> userMgrHost_;
    std::shared_ptr<BmsParam> bmsParam_;
    // Thread pool used to start installation or quick fix in parallel.
    ThreadPool bmsThreadPool_ = ThreadPool(Constants::BMS_SERVICE_NAME);
    const int THREAD_NUMBER = std::thread::hardware_concurrency();

#ifdef BUNDLE_FRAMEWORK_DEFAULT_APP
    sptr<DefaultAppHostImpl> defaultAppHostImpl_;
#endif

#ifdef BUNDLE_FRAMEWORK_APP_CONTROL
    sptr<AppControlManagerHostImpl> appControlManagerHostImpl_;
#endif

#ifdef BUNDLE_FRAMEWORK_QUICK_FIX
    sptr<QuickFixManagerHostImpl> quickFixManagerHostImpl_;
#endif

#define CHECK_INIT_RESULT(result, errmsg)                                         \
    do {                                                                          \
        if (!(result)) {                                                          \
            APP_LOGE(errmsg);                                                     \
            return result;                                                        \
        }                                                                         \
    } while (0)

    DISALLOW_COPY_AND_MOVE(BundleMgrService);
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_SERVICES_BUNDLEMGR_INCLUDE_BUNDLE_MGR_SERVICE_H
