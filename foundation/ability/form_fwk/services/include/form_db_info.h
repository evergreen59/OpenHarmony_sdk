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

#ifndef OHOS_FORM_FWK_FORM_DB_INFO_H
#define OHOS_FORM_FWK_FORM_DB_INFO_H

#include <string>

#include "form_record.h"
#include "nlohmann/json.hpp"

namespace OHOS {
namespace AppExecFwk {
struct FormDBInfo {
    int64_t formId = -1;
    int32_t userId = 0;
    std::string formName;
    std::string bundleName;
    std::string moduleName;
    std::string abilityName;
    std::vector<int> formUserUids;

    /**
     * @brief Constructors
     *
     */
    FormDBInfo() = default;

    /**
     * @brief Constructors
     *
     */
    FormDBInfo(const int64_t formIdTmp, const FormRecord &formRecord)
    {
        formId = formIdTmp;
        userId = formRecord.userId;
        formName = formRecord.formName;
        bundleName = formRecord.bundleName;
        moduleName = formRecord.moduleName;
        abilityName = formRecord.abilityName;
        formUserUids = formRecord.formUserUids;
    }
    bool Contains(const int uId) const
    {
        return std::find(formUserUids.begin(), formUserUids.end(), uId) != formUserUids.end();
    }
    void Remove(const int uId)
    {
        auto itUId = std::find(formUserUids.begin(), formUserUids.end(), uId);
        if (itUId != formUserUids.end()) {
            formUserUids.erase(itUId);
        }
    }
    /**
     * @brief overloaded == for Indicates the formDBInfo by formId
     * @return Returns true if the data equal; returns false otherwise.
     */
    bool operator== (const FormDBInfo &formDBInfo) const
    {
        return (this->formId == formDBInfo.formId);
    }
    /**
     * @brief Compare two data
     * @return Returns true if the data equal; returns false otherwise.
     */
    bool Compare(const FormDBInfo &formDBInfo) const
    {
        if (formId != formDBInfo.formId) {
            return false;
        }
        if (userId != formDBInfo.userId) {
            return false;
        }
        if (formName != formDBInfo.formName) {
            return false;
        }
        if (bundleName != formDBInfo.bundleName) {
            return false;
        }
        if (moduleName != formDBInfo.moduleName) {
            return false;
        }
        if (abilityName != formDBInfo.abilityName) {
            return false;
        }
        if (formUserUids != formDBInfo.formUserUids) {
            return false;
        }

        return true;
    }
};

class InnerFormInfo {
public:
    /**
     * @brief Constructors
     */
    InnerFormInfo()
    {
        formDBInfo_.formId = -1;
    }
    /**
     * @brief Constructors
     *
     */
    InnerFormInfo(const FormDBInfo &formDBInfo)
    {
        formDBInfo_.formId = formDBInfo.formId;
        formDBInfo_.userId = formDBInfo.userId;
        formDBInfo_.formName = formDBInfo.formName;
        formDBInfo_.bundleName = formDBInfo.bundleName;
        formDBInfo_.moduleName = formDBInfo.moduleName;
        formDBInfo_.abilityName = formDBInfo.abilityName;
        formDBInfo_.formUserUids = formDBInfo.formUserUids;
    }
    /**
     * @brief Constructors
     *
     */
    InnerFormInfo(const InnerFormInfo &innerFormInfo)
    {
        formDBInfo_.formId = innerFormInfo.formDBInfo_.formId;
        formDBInfo_.userId = innerFormInfo.formDBInfo_.userId;
        formDBInfo_.formName = innerFormInfo.formDBInfo_.formName;
        formDBInfo_.bundleName = innerFormInfo.formDBInfo_.bundleName;
        formDBInfo_.moduleName = innerFormInfo.formDBInfo_.moduleName;
        formDBInfo_.abilityName = innerFormInfo.formDBInfo_.abilityName;
        formDBInfo_.formUserUids = innerFormInfo.formDBInfo_.formUserUids;
    }
     /**
     * @brief Constructors
     *
     */
    InnerFormInfo(const int64_t formId, const FormRecord &formRecord)
    {
        formDBInfo_.formId = formId;
        formDBInfo_.userId = formRecord.userId;
        formDBInfo_.formName = formRecord.formName;
        formDBInfo_.bundleName = formRecord.bundleName;
        formDBInfo_.moduleName = formRecord.moduleName;
        formDBInfo_.abilityName = formRecord.abilityName;
        formDBInfo_.formUserUids = formRecord.formUserUids;
    }
    std::string ToString() const
    {
        nlohmann::json obj;
        obj["formId"] = formDBInfo_.formId;
        obj["userId"] = formDBInfo_.userId;
        obj["formName"] = formDBInfo_.formName;
        obj["bundleName"] = formDBInfo_.bundleName;
        obj["moduleName"] = formDBInfo_.moduleName;
        obj["abilityName"] = formDBInfo_.abilityName;
        obj["formUserUids"] = formDBInfo_.formUserUids;
        return obj.dump();
    }
    /**
     * @brief Destructor
     *
     */
    virtual ~InnerFormInfo() {};
    /**
     * @brief overloaded == for Indicates the formInfo by formId
     * @return Returns true if the data equal; returns false otherwise.
     */
    bool operator== (const InnerFormInfo &innerFormInfo) const
    {
        return (this->formDBInfo_.formId == innerFormInfo.formDBInfo_.formId);
    }

    /**
     * @brief Transform the InnerFormInfo object to json.
     * @param jsonObject Indicates the obtained json object.
     */
    void ToJson(nlohmann::json &jsonObject) const;

    /**
     * @brief Transform the json object to InnerFormInfo object.
     * @param jsonObject Indicates the obtained json object.
     * @return Returns true on success, false on failure.
     */
    bool FromJson(const nlohmann::json &jsonObject);

    /**
     * @brief Get application form id.
     * @return Returns the form id.
     */
    int64_t GetFormId() const
    {
        return formDBInfo_.formId;
    }

    /**
     * @brief Set application form id.
     * @param formId Indicates the form id to be set.
     */
    void SetFormId(const int64_t formId)
    {
        formDBInfo_.formId = formId;
    }

   /**
     * @brief Get application user id.
     * @return Returns the user id.
     */
    int64_t GetUserId() const
    {
        return formDBInfo_.userId;
    }

    /**
     * @brief Set application user id.
     * @param userId Indicates the user id to be set.
     */
    void SetUserId(const int64_t userId)
    {
        formDBInfo_.userId = userId;
    }

    /**
     * @brief Get application form name.
     * @return Returns the form name.
     */
    std::string GetModuleName() const
    {
        return formDBInfo_.moduleName;
    }

    /**
     * @brief Set application form name.
     * @param formName Indicates the form name to be set.
     */
    void SetModuleName(const std::string &moduleName)
    {
        formDBInfo_.moduleName = moduleName;
    }

    /**
     * @brief Get application bundle name.
     * @return Returns the bundle name.
     */
    std::string GetBundleName() const
    {
        return formDBInfo_.bundleName;
    }

    /**
     * @brief Set application bundle name.
     * @param bundleName Indicates the bundle name to be set.
     */
    void SetBundleName(const std::string &bundleName)
    {
        formDBInfo_.bundleName = bundleName;
    }

    /**
     * @brief Get application ability name.
     * @return Returns the ability name.
     */
    std::string GetAbilityName() const
    {
        return formDBInfo_.abilityName;
    }

    /**
     * @brief Set application ability name.
     * @param abilityName Indicates the ability name to be set.
     */
    void SetAbilityName(const std::string &abilityName)
    {
        formDBInfo_.abilityName = abilityName;
    }

    /**
     * @brief Get application formName.
     * @return Returns the formName.
     */
    std::string GetFormName() const
    {
        return formDBInfo_.formName;
    }

    /**
     * @brief Set application formName.
     * @param userId Indicates the formName to be set.
     */
    void SetFormName(std::string formName)
    {
        formDBInfo_.formName = formName;
    }

    /**
     * @brief Get application user uids.
     * @return Returns the user uids.
     */
    std::vector<int> GetUserUids() const
    {
        return formDBInfo_.formUserUids;
    }

    /**
     * @brief Set application user uids.
     * @param userId Indicates the user uids to be set.
     */
    void SetUserUids(const std::vector<int> &formUserUids)
    {
        formDBInfo_.formUserUids.insert(formDBInfo_.formUserUids.end(), formUserUids.begin(), formUserUids.end());
    }

    /**
     * @brief Set application formRecord
     * @param formRecord Indicates the formRecord to be set.
     */
    void SetFormDBInfo(const FormDBInfo &formDBInfo)
    {
        formDBInfo_ = formDBInfo;
    }

    /**
     * @brief Get application formRecord.
     * @return Returns the formRecord
     */
    FormDBInfo GetFormDBInfo() const
    {
        return formDBInfo_;
    }

    void AddUserUid(const int callingUid);
    bool DeleteUserUid(const int callingUid);

private:
    FormDBInfo formDBInfo_;
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // OHOS_FORM_FWK_FORM_DB_INFO_H
