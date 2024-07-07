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

#ifndef OHOS_FORM_FWK_FORM_PROVIDER_INFO_H
#define OHOS_FORM_FWK_FORM_PROVIDER_INFO_H

#include <cstdint>
#include <utility>

#include "form_ashmem.h"
#include "form_provider_data.h"
#include "nlohmann/json_fwd.hpp"
#include "parcel.h"
#include "refbase.h"

namespace OHOS {
namespace AppExecFwk {
/**
 * @struct FormProviderInfo
 * Defines form provider info.
 */
class FormProviderInfo : public Parcelable {
public:
    FormProviderInfo() = default;
    ~FormProviderInfo() = default;

    /**
     * @brief Set the form data.
     * @param formProviderData The form data.
     */
    inline void SetFormData(const FormProviderData &formProviderData)
    {
        jsBindingData_ = formProviderData;
    }

    /**
     * @brief Get the form data.
     * @return the form data.
     */
    inline FormProviderData GetFormData() const
    {
        return jsBindingData_;
    }
    /**
     * @brief Get the form data.
     * @return the form data.
     */
    inline std::string GetFormDataString() const
    {
        return jsBindingData_.GetDataString();
    }

    /**
     * @brief Set the upgrade flg.
     * @param upgradeFlg The upgrade flg.
     */
    inline void SetUpgradeFlg(const bool upgradeFlg)
    {
        upgradeFlg_ = upgradeFlg;
    }
    /**
     * @brief Get the upgrade flg.
     * @return the upgrade flg.
     */
    inline bool GetUpgradeFlg() const
    {
        return upgradeFlg_;
    }

    /**
     * @brief Set form date by string.
     * @param dataString string json data.
     */
    void SetFormDataString(std::string &dataString);

    /**
     * @brief Updates imageDataMap in this {@code FormProviderData} object.
     * @param imageDataMap Indicates the imageDataMap to update.
     */
    void SetImageDataMap(std::map<std::string, std::pair<sptr<FormAshmem>, int32_t>> imageDataMap);

    /**
     * @brief Obtains the imageDataMap stored in this {@code FormProviderData} object.
     * @return Returns the map that contains shared image data.
     */
    std::map<std::string, std::pair<sptr<FormAshmem>, int32_t>> GetImageDataMap() const;

    /**
     * @brief Merge new data to FormProviderData.
     * @param addJsonData data to merge to FormProviderData
     */
    void MergeData(nlohmann::json &addJsonData);

    /**
     * @brief Whether the form provider data needs to be cached
     * @return Returns {@code true} if the data needs to be cached; returns {@code false} otherwise.
     */
    bool NeedCache() const;

    bool ReadFromParcel(Parcel &parcel);
    virtual bool Marshalling(Parcel &parcel) const override;
    static FormProviderInfo *Unmarshalling(Parcel &parcel);
private:
    FormProviderData jsBindingData_;
    bool upgradeFlg_;
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // OHOS_FORM_FWK_FORM_PROVIDER_INFO_H
