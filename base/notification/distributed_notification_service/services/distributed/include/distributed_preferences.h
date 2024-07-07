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

#ifndef BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_SERVICES_DISTRIBUTED_INCLUDE_DISTRIBUTED_PREFERENCES_H
#define BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_SERVICES_DISTRIBUTED_INCLUDE_DISTRIBUTED_PREFERENCES_H

#include <memory>
#include <string>

#include "singleton.h"
#include "refbase.h"

#include "ans_inner_errors.h"
#include "distributed_preferences_database.h"
#include "distributed_preferences_info.h"
#include "notification_bundle_option.h"

namespace OHOS {
namespace Notification {
class DistributedPreferences : public DelayedSingleton<DistributedPreferences> {
public:
    /**
     * @brief Set whether the device supports distributed notifications.
     *
     * @param isEnable Specifies whether to enable the device to support distributed notification.
     * @return Returns enable distributed result.
     */
    ErrCode SetDistributedEnable(bool isEnable);

    /**
     * @brief Check if the device supports distributed notification.
     *
     * @param isEnable True if the device supports distributed notification; false otherwise.
     * @return Returns is distributed enabled result.
     */
    ErrCode GetDistributedEnable(bool &isEnable);

    /**
     * @brief Set whether an application supports distributed notifications.
     *
     * @param bundleOption Indicates the bundle name and uid of an application.
     * @param isEnable Specifies whether to enable an application to support distributed notification.
     * @return Returns enable distributed by bundle result.
     */
    ErrCode SetDistributedBundleEnable(const sptr<NotificationBundleOption> &bundleOption, bool isEnable);

    /**
     * @brief Check whether an application supports distributed notifications.
     *
     * @param bundleOption Indicates the bundle name and uid of an application.
     * @param isEnable True if the application supports distributed notification; false otherwise.
     * @return Returns is distributed enabled by bundle result.
     */
    ErrCode GetDistributedBundleEnable(const sptr<NotificationBundleOption> &bundleOption, bool &isEnable);

    /**
     * @brief Remove the setting of whether the application supports distributed notification.
     *
     * @param bundleOption Indicates the bundle name and uid of an application.
     * @return Returns remove the setting result.
     */
    ErrCode DeleteDistributedBundleInfo(const sptr<NotificationBundleOption> &bundleOption);

    /**
     * @brief Remove all distributed enabled setting info from DB.
     *
     * @return Returns remove the setting result.
     */
    ErrCode ClearDataInRestoreFactorySettings();

    /**
     * @brief Set whether to sync notifications to devices that do not have the app installed.
     *
     * @param userId Indicates the specific user.
     * @param enabled Allow or disallow sync notifications.
     * @return Returns set enabled result.
     */
    ErrCode SetSyncEnabledWithoutApp(const int32_t userId, const bool enabled);
    ErrCode GetSyncEnabledWithoutApp(const int32_t userId, bool &enabled);

private:
    bool InitDistributedAllInfo(void);
    void GetDistributedMainKey(std::string &key);
    void GetDistributedBundleKey(const sptr<NotificationBundleOption> &bundleOption, std::string &key);
    bool ResolveDistributedKey(const DistributedKv::Entry &entry);
    void GetEnabledWithoutApp(const int32_t userId, std::string &key);

    bool ResolveDistributedEnable(const std::string &value);
    bool ResolveDistributedBundleEnable(const std::string &key, const int32_t startPos, const std::string &value);
    bool ResolveSyncWithoutAppEnable(const std::string &key, const int32_t startPos, const std::string &value);

private:
    std::unique_ptr<DistributedPreferencesInfo> preferencesInfo_ = nullptr;
    std::unique_ptr<DistributedPreferencesDatabase> database_ = nullptr;

    DECLARE_DELAYED_SINGLETON(DistributedPreferences);
    DISALLOW_COPY_AND_MOVE(DistributedPreferences);
};
}  // namespace Notification
}  // namespace OHOS
#endif // BASE_NOTIFICATION_DISTRIBUTED_NOTIFICATION_SERVICE_SERVICES_DISTRIBUTED_INCLUDE_DISTRIBUTED_PREFERENCES_H
