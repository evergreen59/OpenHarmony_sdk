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

#ifndef OHOS_ABILITY_BASE_CONFIGURATION_H
#define OHOS_ABILITY_BASE_CONFIGURATION_H

#include <unordered_map>
#include <string>
#include <vector>
#include <set>
#include "parcel.h"
#include "global_configuration_key.h"

namespace OHOS {
namespace AppExecFwk {
namespace ConfigurationInner {
    constexpr const char* CONNECTION_SYMBOL = "#";
    constexpr const char* EMPTY_STRING = "";
    constexpr const char* APPLICATION_DIRECTION = "ohos.application.direction";
    constexpr const char* APPLICATION_DENSITYDPI = "ohos.application.densitydpi";
    constexpr const char* APPLICATION_DISPLAYID = "ohos.application.displayid";

    /*
     * This must be synchronized with the value in GlobalConfigurationKey
     */
    const std::vector<std::string> SystemConfigurationKeyStore {
        OHOS::AAFwk::GlobalConfigurationKey::SYSTEM_LANGUAGE,
        OHOS::AAFwk::GlobalConfigurationKey::SYSTEM_COLORMODE,
        OHOS::AAFwk::GlobalConfigurationKey::INPUT_POINTER_DEVICE,
        OHOS::AAFwk::GlobalConfigurationKey::DEVICE_TYPE,
        OHOS::AppExecFwk::ConfigurationInner::APPLICATION_DIRECTION,
        OHOS::AppExecFwk::ConfigurationInner::APPLICATION_DENSITYDPI,
        OHOS::AppExecFwk::ConfigurationInner::APPLICATION_DISPLAYID,
    };

    constexpr const char* COLOR_MODE_LIGHT = "light";
    constexpr const char* COLOR_MODE_DARK = "dark";
    constexpr const char* DEVICE_TYPE_DEFAULT = "default";
    constexpr const char* DIRECTION_VERTICAL = "vertical";
    constexpr const char* DIRECTION_HORIZONTAL = "horizontal";
};

class Configuration final: public Parcelable {
public:
    Configuration();

    Configuration(const Configuration &other);

    Configuration& operator=(const Configuration &other);

    ~Configuration();

    /**
     * @brief Compare the difference between the current and the passed in object.
     *
     * @param diffKeyV Out Ginseng. get the current difference item keys.
     * @param other Comparisons obj
     *
     * @return void
     */
    void CompareDifferent(std::vector<std::string> &diffKeyV, const Configuration &other);

    /**
     * @brief Update the content according to the key.
     *
     * @param mergeItemKey The key of the element currently to be updated.
     * @param other Provide updated content obj
     *
     * @return void
     */
    void Merge(const std::vector<std::string> &diffKeyV, const Configuration &other);

    /**
     * @brief obtain the value according to the display number and storage key.
     *
     * @param displayId Currently displayed id.
     * @param key The key of the item to access configura. ej : key = GlobalConfigurationKey::SYSTEM_LANGUAGE
     * Means you want to change the language part
     * @param value Changed value
     * @return return true if the deposit is successful, otherwise return false
     */
    bool AddItem(int displayId, const std::string &key, const std::string &value);

    /**
     * @brief obtain the value according to the display number and storage key.
     *
     * @param displayId Currently displayed id.
     * @param key The key of the item to access configura. ej : key = GlobalConfigurationKey::SYSTEM_LANGUAGE
     * Means you want to change the language part
     *
     * @return return empty string if not found | return val if found
     */
    std::string GetItem(int displayId, const std::string &key) const;

    /**
     * @brief Delete element.
     *
     * @param displayId Currently displayed id.
     * @param key The key of the item to access configura. ej : key = GlobalConfigurationKey::SYSTEM_LANGUAGE
     * Means you want to change the language part
     *
     * @return Return an integer greater than 0 if the deletion succeeds, otherwise it returns 0.
     */
    int RemoveItem(int displayId, const std::string &key);

    /**
     * @brief obtain the value according to the display number and storage key.
     *
     * @param key The key of the item to access configura. ej : key = GlobalConfigurationKey::SYSTEM_LANGUAGE
     * Means you want to change the language part
     * @param value Changed value
     * @return return true if the deposit is successful, otherwise return false
     */
    bool AddItem(const std::string &key, const std::string &value);

    /**
     * @brief obtain the value according to the display number and storage key.
     *
     * @param key The key of the item to access configura. ej : key = GlobalConfigurationKey::SYSTEM_LANGUAGE
     * Means you want to change the language part
     *
     * @return return empty string if not found | return val if found
     */
    std::string GetItem(const std::string &key) const;

    /**
     * @brief Delete element.
     *
     * @param key The key of the item to access configura. ej : key = GlobalConfigurationKey::SYSTEM_LANGUAGE
     * Means you want to change the language part
     *
     * @return Return an integer greater than 0 if the deletion succeeds, otherwise it returns 0.
     */
    int RemoveItem(const std::string &key);

    /**
     * @brief Get the currently existing key-value pairs.
     *
     * @return return currently item size.
     */
    int GetItemSize() const;

    /**
     * @brief Return all current key-value pairs.
     *
     */
    const std::string& GetName() const;

    /**
     * @brief read this Sequenceable object from a Parcel.
     *
     * @param inParcel Indicates the Parcel object into which the Sequenceable object has been marshaled.
     * @return Returns true if read successed; returns false otherwise.
     */
    bool ReadFromParcel(Parcel &parcel);

    /**
     * @brief Marshals this Sequenceable object into a Parcel.
     *
     * @param outParcel Indicates the Parcel object to which the Sequenceable object will be marshaled.
     */
    virtual bool Marshalling(Parcel &parcel) const override;

    /**
     * @brief Unmarshals this Sequenceable object from a Parcel.
     *
     * @param inParcel Indicates the Parcel object into which the Sequenceable object has been marshaled.
     */
    static Configuration *Unmarshalling(Parcel &parcel);

private:

    /**
     * @brief Make the key by id and param
     *
     * @param getKey Key made.
     * @param id displayId.
     * @param param The key of the item to access configura.
     *
     */
    bool MakeTheKey(std::string &getKey, int id, const std::string &param) const;

    /**
     * @brief Get all current keys.
     *
     * @param keychain Out Ginseng. Contains all current keys.
     */
    void GetAllKey(std::vector<std::string> &keychain) const;

    /**
     * @brief Get value by key.
     *
     * @param key the key to get value.
     */
    std::string GetValue(const std::string &key) const;

private:
    int defaultDisplayId_ {0};
    mutable std::string toStrintg_ {""}; /* For interface GetName(), Assign value only when calling the interface */
    std::unordered_map<std::string, std::string> configParameter_;
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // OHOS_ABILITY_BASE_CONFIGURATION_H
