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

#include "ability_start_setting.h"
#include <cstring>
#include "string_ex.h"

using namespace OHOS;

namespace OHOS {
namespace AAFwk {
const std::string AbilityStartSetting::BOUNDS_KEY = "bounds";
const std::string AbilityStartSetting::WINDOW_DISPLAY_ID_KEY = "windowId";
const std::string AbilityStartSetting::WINDOW_MODE_KEY = "windowMode";
constexpr uint32_t CYCLE_LIMIT = 1000;

/**
 * @brief Construct copy function.
 * @param other indicates instance of abilitystartsetting object
 * @return none.
 */
AbilityStartSetting::AbilityStartSetting(const AbilityStartSetting &other)
{
    abilityStarKey_.clear();
    abilityStarKey_ = other.abilityStarKey_;
}
/**
 * @brief Overload assignment operation.
 * @param other indicates instance of abilitystartsetting object.
 * @return Returns current instance of abilitystartsetting object.
 */
AbilityStartSetting &AbilityStartSetting::operator=(const AbilityStartSetting &other)
{
    if (this != &other) {
        abilityStarKey_.clear();
        abilityStarKey_ = other.abilityStarKey_;
    }
    return *this;
}
/**
 * @brief Inner function to create AbilityStartSetting
 *
 * @return Returns the shared_ptr of AbilityStartSetting object.
 */
std::shared_ptr<AbilityStartSetting> AbilityStartSettingCreator()
{
    std::shared_ptr<AbilityStartSetting> abilityStartSetting {new (std::nothrow) AbilityStartSetting()};
    return abilityStartSetting;
}

/**
 * @brief Obtains an empty AbilityStartSetting object.
 *
 * @return Returns the btains an empty AbilityStartSetting object.
 */
std::shared_ptr<AbilityStartSetting> AbilityStartSetting::GetEmptySetting()
{
    return AbilityStartSettingCreator();
}

/**
 * @brief Obtains the names of all the attributes that have been added to this AbilityStartSetting object.
 *
 * @return Returns the set of attribute names included in this AbilityStartSetting object.
 */
std::set<std::string> AbilityStartSetting::GetPropertiesKey()
{
    std::set<std::string> abilityStartSet;
    abilityStartSet.clear();

    for (auto it : abilityStarKey_) {
        abilityStartSet.emplace(it.first);
    }
    return abilityStartSet;
}

/**
 * @brief Checks whether this AbilityStartSetting object is empty.
 *
 * @return Returns true if this AbilityStartSetting object is empty and animatorOption is null; returns false otherwise.
 */
bool AbilityStartSetting::IsEmpty()
{
    return (abilityStarKey_.size() == 0);
}

/**
 * @brief Sets the names of all the attributes of the AbilityStartSetting object.
 *
 * @param key Indicates the name of the key.
 * @param value The window display mode of the values.
 */
void AbilityStartSetting::AddProperty(const std::string &key, const std::string &value)
{
    abilityStarKey_[key] = value;
}

/**
 * @brief Gets the name of the attributes of the AbilityStartSetting object.
 *
 * @param key Indicates the name of the key.
 * @return Returns value Indicates the value of the attributes of the AbilityStartSetting object
 */
std::string AbilityStartSetting::GetProperty(const std::string &key)
{
    auto it = abilityStarKey_.find(key);
    if (it == abilityStarKey_.end()) {
        return std::string();
    }
    return abilityStarKey_[key];
}

/**
 * @brief Write the data of AbilityStartSetting to the file stream
 * @param parcel indicates write the data of AbilityStartSetting to the file stream through parcel
 * @return bool
 */
bool AbilityStartSetting::Marshalling(Parcel &parcel) const
{
    size_t size = abilityStarKey_.size();

    // 1. Number of key value pairs written
    parcel.WriteUint32((uint32_t)size);

    std::map<std::string, std::string>::const_iterator it;

    // 2. Write the key and value strings
    for (auto pair : abilityStarKey_) {
        // 1.key
        parcel.WriteString16(Str8ToStr16(pair.first));
        // 2.data content
        parcel.WriteString16(Str8ToStr16(pair.second));
    }

    return true;
}

/**
 * @brief Reading file stream through parcel to generate AbilityStartSetting instance
 * @param parcel indicates reading file stream through parcel to generate AbilityStartSetting instance
 * @return AbilityStartSetting shared_ptr
 */
AbilityStartSetting *AbilityStartSetting::Unmarshalling(Parcel &parcel)
{
    AbilityStartSetting *abilityStartSetting = new (std::nothrow) AbilityStartSetting();
    if (abilityStartSetting == nullptr) {
        return nullptr;
    }
    // 1. Number of key value pairs read
    uint32_t size = 0;
    parcel.ReadUint32(size);

    if (size > CYCLE_LIMIT) {
        return nullptr;
    }

    std::u16string keyReadString16;
    std::u16string dataReadString16;
    for (size_t i = 0; (i < size) && abilityStartSetting; i++) {
        // 1.key
        keyReadString16 = parcel.ReadString16();
        // 2.data content
        dataReadString16 = parcel.ReadString16();
        abilityStartSetting->abilityStarKey_[Str16ToStr8(keyReadString16)] = Str16ToStr8(dataReadString16);
        keyReadString16.clear();
        dataReadString16.clear();
    }

    return abilityStartSetting;
}
}  // namespace AAFwk
}  // namespace OHOS
