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

#ifndef OHOS_ABILITY_RUNTIME_STAGE_CONTEXT_CONTEXT_H
#define OHOS_ABILITY_RUNTIME_STAGE_CONTEXT_CONTEXT_H

#include <memory>
#include <mutex>

#include "application_info.h"
#include "bindable.h"
#include "configuration.h"
#include "hap_module_info.h"
#include "foundation/communication/ipc/interfaces/innerkits/ipc_core/include/iremote_object.h"
#include "resource_manager.h"

using IRemoteObject = OHOS::IRemoteObject;

namespace OHOS {
namespace AbilityRuntime {
class ApplicationContext;

class Context : public Bindable {
public:
    Context() = default;
    ~Context() override = default;

    /**
     * @brief Obtains the Context object of the application.
     *
     * @return Returns the Context object of the application.
     */
    static std::shared_ptr<ApplicationContext> GetApplicationContext();

    /**
     * @brief Obtains the bundle name of the current ability.
     *
     * @return Returns the bundle name of the current ability.
     */
    virtual std::string GetBundleName() const = 0;

    /**
     * @brief Creates a Context object for an application with the given bundle name.
     *
     * @param bundleName Indicates the bundle name of the application.
     *
     * @return Returns a Context object created for the specified application.
     */
    virtual std::shared_ptr<Context> CreateBundleContext(const std::string &bundleName) = 0;

    /**
     * @brief Obtains information about the current application. The returned application information includes basic
     * information such as the application name and application permissions.
     *
     * @return Returns the ApplicationInfo for the current application.
     */
    virtual std::shared_ptr<AppExecFwk::ApplicationInfo> GetApplicationInfo() const = 0;

    /**
    * @brief Obtains a resource manager.
    *
    * @return Returns a ResourceManager object.
    */
    virtual std::shared_ptr<Global::Resource::ResourceManager> GetResourceManager() const = 0;

    /**
     * @brief Obtains the path of the package containing the current ability. The returned path contains the resources,
     *  source code, and configuration files of a module.
     *
     * @return Returns the path of the package file.
     */
    virtual std::string GetBundleCodePath() const = 0;

    /**
     * @brief Obtains the HapModuleInfo object of the application.
     *
     * @return Returns the HapModuleInfo object of the application.
     */
    virtual std::shared_ptr<AppExecFwk::HapModuleInfo> GetHapModuleInfo() const = 0;

    /**
     * @brief Obtains the path of the package containing the current ability. The returned path contains the resources,
     *  source code, and configuration files of a module.
     *
     * @return Returns the path of the package file.
     */
    virtual std::string GetBundleCodeDir() = 0;

    /**
     * @brief Obtains the application-specific cache directory on the device's internal storage. The system
     * automatically deletes files from the cache directory if disk space is required elsewhere on the device.
     * Older files are always deleted first.
     *
     * @return Returns the application-specific cache directory.
     */
    virtual std::string GetCacheDir() = 0;

    /**
     * @brief Obtains the temporary directory.
     *
     * @return Returns the application temporary directory.
     */
    virtual std::string GetTempDir() = 0;

    /**
     * @brief Obtains the directory for storing files for the application on the device's internal storage.
     *
     * @return Returns the application file directory.
     */
    virtual std::string GetFilesDir() = 0;

    /**
     * @brief Checks whether the configuration of this ability is changing.
     *
     * @return Returns true if the configuration of this ability is changing and false otherwise.
     */
    virtual bool IsUpdatingConfigurations() = 0;

    /**
     * @brief Informs the system of the time required for drawing this Page ability.
     *
     * @return Returns the notification is successful or fail
     */
    virtual bool PrintDrawnCompleted() = 0;

    /**
     * @brief Obtains the local database path.
     * If the local database path does not exist, the system creates one and returns the created path.
     *
     * @return Returns the local database file.
     */
    virtual std::string GetDatabaseDir() = 0;

    /**
     * @brief Obtains the path storing the storage file of the application.
     *
     * @return Returns the local storage file.
     */
    virtual std::string GetPreferencesDir() = 0;

    /**
     * @brief Obtains the path distributed file of the application
     *
     * @return Returns the distributed file.
     */
    virtual std::string GetDistributedFilesDir() = 0;

     /**
     * @brief Obtains token.
     *
     * @return Returns the token.
     */
    virtual sptr<IRemoteObject> GetToken() = 0;

    /**
     * @brief Attachs ability's token.
     *
     * @param token The token represents ability.
     */
    virtual void SetToken(const sptr<IRemoteObject> &token) = 0;

    /**
     * @brief Switch file area
     *
     * @param mode file area.
     */
    virtual void SwitchArea(int mode) = 0;

    /**
     * @brief Creates a Context object for a hap with the given module name.
     *
     * @param moduleName Indicates the module name of the hap.
     *
     * @return Returns a Context object created for the specified hap and app.
     */
    virtual std::shared_ptr<Context> CreateModuleContext(const std::string &moduleName) = 0;

    /**
     * @brief Creates a Context object for a hap with the given hap name and app name.
     *
     * @param bundleName Indicates the app name of the application.
     *
     * @param moduleName Indicates the module name of the hap.
     *
     * @return Returns a Context object created for the specified hap and app.
     */
    virtual std::shared_ptr<Context> CreateModuleContext(const std::string &bundleName,
                                                          const std::string &moduleName) = 0;

    /**
     * @brief Get file area
     *
     * @return file area.
     */
    virtual int GetArea() = 0;

    /**
     * @brief Obtains the configuration of application.
     *
     * @return configuration of application.
     */
    virtual std::shared_ptr<AppExecFwk::Configuration> GetConfiguration() const = 0;

    /**
     * @brief Obtains the application base directory on the device's internal storage.
     *
     * @return Returns the application base directory.
     */
    virtual std::string GetBaseDir() const = 0;

    /**
     * @brief Obtains the Device Type.
     *
     * @return Returns the Device Type.
     */
    virtual Global::Resource::DeviceType GetDeviceType() const = 0;

    /**
     * @brief Getting derived class
     *
     * @tparam T template
     * @param context the context object
     * @return std::shared_ptr<T> derived class
     */
    template<class T>
    static std::shared_ptr<T> ConvertTo(const std::shared_ptr<Context>& context)
    {
        if constexpr (!std::is_same_v<T, typename T::SelfType>) {
            return nullptr;
        }

        if (context && context->IsContext(T::CONTEXT_TYPE_ID)) {
            return std::static_pointer_cast<T>(context);
        }

        return nullptr;
    }

    using SelfType = Context;
    static const size_t CONTEXT_TYPE_ID;

protected:
    virtual bool IsContext(size_t contextTypeId)
    {
        return contextTypeId == CONTEXT_TYPE_ID;
    }

    static std::shared_ptr<ApplicationContext> applicationContext_;
    static std::mutex contextMutex_;
};
}  // namespace AbilityRuntime
}  // namespace OHOS
#endif  // OHOS_ABILITY_RUNTIME_STAGE_CONTEXT_CONTEXT_H
