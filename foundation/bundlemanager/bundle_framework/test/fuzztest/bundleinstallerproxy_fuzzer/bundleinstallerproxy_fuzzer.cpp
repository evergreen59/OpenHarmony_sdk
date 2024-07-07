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

#include <cstddef>
#include <cstdint>

#include "bundle_installer_proxy.h"

#include "bundleinstallerproxy_fuzzer.h"

using namespace OHOS::AppExecFwk;
namespace OHOS {
    bool DoSomethingInterestingWithMyAPI(const uint8_t* data, size_t size)
    {
        sptr<IRemoteObject> object;
        BundleInstallerProxy bundleinstallerProxy(object);
        std::string bundleFilePath (reinterpret_cast<const char*>(data), size);
        InstallParam installParam;
        sptr<IStatusReceiver> statusReceiver;
        bundleinstallerProxy.Install(bundleFilePath, installParam, statusReceiver);

        std::string bundleName (reinterpret_cast<const char*>(data), size);
        bundleinstallerProxy.Recover(bundleName, installParam, statusReceiver);

        std::vector<std::string> bundleFilePaths;
        bundleFilePaths.push_back(bundleFilePath);
        bundleinstallerProxy.Install(bundleFilePaths, installParam, statusReceiver);

        bundleinstallerProxy.Uninstall(bundleName, installParam, statusReceiver);
        std::string modulePackage (reinterpret_cast<const char*>(data), size);
        bundleinstallerProxy.Uninstall(bundleFilePath, modulePackage, installParam, statusReceiver);
        int32_t dlpType = reinterpret_cast<uintptr_t>(data);
        int32_t userId = reinterpret_cast<uintptr_t>(data);
        int32_t appIndex = reinterpret_cast<uintptr_t>(data);
        bundleinstallerProxy.InstallSandboxApp(bundleName, dlpType, userId, appIndex);
        bundleinstallerProxy.UninstallSandboxApp(bundleName, appIndex, userId);
        bundleinstallerProxy.CreateStreamInstaller(installParam, statusReceiver);
        bundleinstallerProxy.DestoryBundleStreamInstaller(reinterpret_cast<uintptr_t>(data));
        bundleinstallerProxy.StreamInstall(bundleFilePaths, installParam, statusReceiver);
        return true;
    }
}

// Fuzzer entry point.
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    // Run your code on data.
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}