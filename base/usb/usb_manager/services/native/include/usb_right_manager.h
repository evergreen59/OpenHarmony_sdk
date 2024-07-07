/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef USB_RIGHT_MANAGER_H
#define USB_RIGHT_MANAGER_H
#include <algorithm>
#include <map>
#include <mutex>
#include <semaphore.h>
#include <string>
#include <vector>

#include "ability_connect_callback_stub.h"
#include "bundle_mgr_interface.h"
#include "usb_common.h"

namespace OHOS {
namespace USB {
class UsbRightManager {
public:
    typedef std::vector<std::string> BundleNameList;
    typedef std::map<std::string, BundleNameList> RightMap;
    RightMap rightMap;

    void Init();
    bool HasRight(const std::string &deviceName, const std::string &bundleName);
    int32_t RequestRight(const std::string &deviceName, const std::string &bundleName);
    bool AddDeviceRight(const std::string &deviceName, const std::string &bundleName);
    bool RemoveDeviceRight(const std::string &deviceName, const std::string &bundleName);
    bool RemoveDeviceAllRight(const std::string &deviceName);
    bool IsSystemHap();

private:
    bool GetUserAgreementByDiag(const std::string &deviceName, const std::string &bundleName);
    bool ShowUsbDialog(const std::string &deviceName, const std::string &bundleName);
    sptr<AppExecFwk::IBundleMgr> GetBundleMgr();

    static sem_t waitDialogDisappear_;
    class UsbAbilityConn : public AAFwk::AbilityConnectionStub {
        void OnAbilityConnectDone(
            const AppExecFwk::ElementName &element, const sptr<IRemoteObject> &remoteObject, int resultCode) override
        {
            USB_HILOGI(MODULE_USB_SERVICE, "connect done");
        }
        void OnAbilityDisconnectDone(const AppExecFwk::ElementName &element, int resultCode) override
        {
            USB_HILOGI(MODULE_USB_SERVICE, "disconnect done");
            sem_post(&waitDialogDisappear_);
        }
    };

    std::mutex dialogRunning_;
};
} // namespace USB
} // namespace OHOS

#endif
