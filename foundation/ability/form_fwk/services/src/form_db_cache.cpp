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

#include "form_db_cache.h"

#include <cinttypes>

#include "form_bms_helper.h"
#include "form_data_mgr.h"
#include "form_db_info.h"
#include "form_mgr_errors.h"
#include "form_provider_mgr.h"
#include "hilog_wrapper.h"

namespace OHOS {
namespace AppExecFwk {
FormDbCache::FormDbCache()
{
    HILOG_INFO("FormDbCache is created");
}

FormDbCache::~FormDbCache()
{
    HILOG_INFO("FormDbCache is destroyed");
}

/**
 * @brief Load form data from DB to DbCache when starting.
 * @return Void.
 */
void FormDbCache::Start()
{
    HILOG_INFO("%{public}s called.", __func__);
    std::lock_guard<std::mutex> lock(formDBInfosMutex_);
    std::vector<InnerFormInfo> innerFormInfos;
    innerFormInfos.clear();
    if (FormInfoRdbStorageMgr::GetInstance().LoadFormData(innerFormInfos) != ERR_OK) {
        HILOG_ERROR("%{public}s, LoadFormData failed.", __func__);
        return;
    }

    for (unsigned int i = 0; i < innerFormInfos.size(); i++) {
        FormDBInfo formDBInfo = innerFormInfos.at(i).GetFormDBInfo();
        formDBInfos_.emplace_back(formDBInfo);
    }
}

/**
 * @brief Save or update form data to DbCache and DB.
 * @param formDBInfo Form data.
 * @return Returns ERR_OK on success, others on failure.
 */
ErrCode FormDbCache::SaveFormInfo(const FormDBInfo &formDBInfo)
{
    HILOG_INFO("%{public}s called, formId:%{public}" PRId64 "", __func__, formDBInfo.formId);
    std::lock_guard<std::mutex> lock(formDBInfosMutex_);
    auto iter = find(formDBInfos_.begin(), formDBInfos_.end(), formDBInfo);
    if (iter != formDBInfos_.end()) {
        if (iter->Compare(formDBInfo) == false) {
            HILOG_WARN("%{public}s, need update, formId[%{public}" PRId64 "].", __func__, formDBInfo.formId);
            *iter = formDBInfo;
            InnerFormInfo innerFormInfo(formDBInfo);
            return FormInfoRdbStorageMgr::GetInstance().ModifyStorageFormData(innerFormInfo);
        } else {
            HILOG_WARN("%{public}s, already exist, formId[%{public}" PRId64 "].", __func__, formDBInfo.formId);
            return ERR_OK;
        }
    } else {
        formDBInfos_.emplace_back(formDBInfo);
        InnerFormInfo innerFormInfo(formDBInfo);
        return FormInfoRdbStorageMgr::GetInstance().SaveStorageFormData(innerFormInfo);
    }
}

/**
 * @brief Save or update form data to DbCache and DB.
 * @param formDBInfo Form data.
 * @return Returns ERR_OK on success, others on failure.(NoLock)
 */
ErrCode FormDbCache::SaveFormInfoNolock(const FormDBInfo &formDBInfo)
{
    HILOG_INFO("%{public}s called, formId:%{public}" PRId64 "", __func__, formDBInfo.formId);
    auto iter = find(formDBInfos_.begin(), formDBInfos_.end(), formDBInfo);
    if (iter != formDBInfos_.end()) {
        if (iter->Compare(formDBInfo) == false) {
            HILOG_WARN("%{public}s, need update, formId[%{public}" PRId64 "].", __func__, formDBInfo.formId);
            *iter = formDBInfo;
            InnerFormInfo innerFormInfo(formDBInfo);
            return FormInfoRdbStorageMgr::GetInstance().ModifyStorageFormData(innerFormInfo);
        } else {
            HILOG_WARN("%{public}s, already exist, formId[%{public}" PRId64 "].", __func__, formDBInfo.formId);
            return ERR_OK;
        }
    } else {
        formDBInfos_.emplace_back(formDBInfo);
        InnerFormInfo innerFormInfo(formDBInfo);
        return FormInfoRdbStorageMgr::GetInstance().SaveStorageFormData(innerFormInfo);
    }
}

/**
 * @brief Delete form data in DbCache and DB with formId.
 * @param formId form data Id.
 * @return Returns ERR_OK on success, others on failure.
 */
ErrCode FormDbCache::DeleteFormInfo(int64_t formId)
{
    std::lock_guard<std::mutex> lock(formDBInfosMutex_);
    FormDBInfo tmpForm;
    tmpForm.formId = formId;
    auto iter = find(formDBInfos_.begin(), formDBInfos_.end(), tmpForm);
    if (iter == formDBInfos_.end()) {
        HILOG_WARN("%{public}s, not find formId[%{public}" PRId64 "]", __func__, formId);
    } else {
        formDBInfos_.erase(iter);
    }
    if (FormInfoRdbStorageMgr::GetInstance().DeleteStorageFormData(std::to_string(formId)) == ERR_OK) {
        return ERR_OK;
    } else {
        return ERR_APPEXECFWK_FORM_COMMON_CODE;
    }
}
/**
 * @brief Delete form data in DbCache and DB with formId.
 * @param formId form data Id.
 * @param removedDBForms Removed db form infos
 * @return Returns ERR_OK on success, others on failure.
 */
ErrCode FormDbCache::DeleteFormInfoByBundleName(const std::string &bundleName, const int32_t userId,
    std::vector<FormDBInfo> &removedDBForms)
{
    std::lock_guard<std::mutex> lock(formDBInfosMutex_);
    std::vector<FormDBInfo>::iterator itRecord;
    for (itRecord = formDBInfos_.begin(); itRecord != formDBInfos_.end();) {
        if ((bundleName == itRecord->bundleName) && (userId == itRecord->userId)) {
            int64_t formId = itRecord->formId;
            if (FormInfoRdbStorageMgr::GetInstance().DeleteStorageFormData(std::to_string(formId)) == ERR_OK) {
                removedDBForms.emplace_back(*itRecord);
                itRecord = formDBInfos_.erase(itRecord);
            } else {
                itRecord++;
            }
        } else {
            itRecord++;
        }
    }
    return ERR_OK;
}

/**
 * @brief Get all form data from DbCache.
 * @param formDBInfos Storage all DbCache.
 * @return Void.
 */
void FormDbCache::GetAllFormInfo(std::vector<FormDBInfo> &formDBInfos)
{
    HILOG_INFO("%{public}s called.", __func__);
    std::lock_guard<std::mutex> lock(formDBInfosMutex_);
    formDBInfos = formDBInfos_;
}

/**
 * @brief Get record from DB cache with formId
 * @param formId Form data Id
 * @param record Form data
 * @return Returns ERR_OK on success, others on failure.
 */
ErrCode FormDbCache::GetDBRecord(const int64_t formId, FormRecord &record) const
{
    std::lock_guard<std::mutex> lock(formDBInfosMutex_);
    for (const FormDBInfo &dbInfo : formDBInfos_) {
        if (dbInfo.formId == formId) {
            record.userId = dbInfo.userId;
            record.formName = dbInfo.formName;
            record.bundleName = dbInfo.bundleName;
            record.moduleName = dbInfo.moduleName;
            record.abilityName = dbInfo.abilityName;
            record.formUserUids = dbInfo.formUserUids;
            return ERR_OK;
        }
    }
    HILOG_ERROR("%{public}s, not find formId[%{public}" PRId64 "]", __func__, formId);
    return ERR_APPEXECFWK_FORM_NOT_EXIST_ID;
}
/**
 * @brief Get record from DB cache with formId
 * @param formId Form data Id
 * @param record Form db data
 * @return Returns ERR_OK on success, others on failure.
 */
ErrCode FormDbCache::GetDBRecord(const int64_t formId, FormDBInfo &record) const
{
    std::lock_guard<std::mutex> lock(formDBInfosMutex_);
    for (const FormDBInfo &dbInfo : formDBInfos_) {
        if (dbInfo.formId == formId) {
            record = dbInfo;
            return ERR_OK;
        }
    }
    HILOG_ERROR("%{public}s, not find formId[%{public}" PRId64 "]", __func__, formId);
    return ERR_APPEXECFWK_FORM_NOT_EXIST_ID;
}
/**
 * @brief Use record save or update DB data and DB cache with formId
 * @param formId Form data Id
 * @param record Form data
 * @return Returns ERR_OK on success, others on failure.
 */
ErrCode FormDbCache::UpdateDBRecord(const int64_t formId, const FormRecord &record) const
{
    FormDBInfo formDBInfo(formId, record);
    return FormDbCache::GetInstance().SaveFormInfo(formDBInfo);
}
/**
 * @brief Get no host db record.
 * @param uid The caller uid.
 * @param noHostFormDBList no host db record list.
 * @param foundFormsMap Form Id list.
 * @return Returns ERR_OK on success, others on failure.
 */
ErrCode FormDbCache::GetNoHostDBForms(const int uid, std::map<FormIdKey,
    std::set<int64_t>> &noHostFormDBList, std::map<int64_t, bool> &foundFormsMap)
{
    std::lock_guard<std::mutex> lock(formDBInfosMutex_);
    for (FormDBInfo& dbInfo : formDBInfos_) {
        if (dbInfo.Contains(uid)) {
            dbInfo.Remove(uid);
            if (dbInfo.formUserUids.empty()) {
                FormIdKey formIdKey(dbInfo.bundleName, dbInfo.abilityName);
                auto itIdsSet = noHostFormDBList.find(formIdKey);
                if (itIdsSet == noHostFormDBList.end()) {
                    std::set<int64_t> formIdsSet;
                    formIdsSet.emplace(dbInfo.formId);
                    noHostFormDBList.emplace(formIdKey, formIdsSet);
                } else {
                    itIdsSet->second.emplace(dbInfo.formId);
                }
            } else {
                foundFormsMap.emplace(dbInfo.formId, false);
                SaveFormInfoNolock(dbInfo);
                FormBmsHelper::GetInstance().NotifyModuleNotRemovable(dbInfo.bundleName, dbInfo.moduleName);
            }
        }
    }
    return ERR_OK;
}
/**
 * @brief Get match count by bundleName and moduleName.
 * @param bundleName BundleName.
 * @param moduleName ModuleName.
 * @return Returns match count.
 */
int FormDbCache::GetMatchCount(const std::string &bundleName, const std::string &moduleName)
{
    int32_t matchCount = 0;
    std::vector<FormDBInfo> formDBInfos;
    std::lock_guard<std::mutex> lock(formDBInfosMutex_);
    for (const FormDBInfo &dbInfo : formDBInfos_) {
        if (dbInfo.bundleName == bundleName && dbInfo.moduleName == moduleName) {
            ++matchCount;
        }
    }
    return matchCount;
}
/**
 * @brief delete forms bu userId.
 *
 * @param userId user ID.
 */
void FormDbCache::DeleteDBFormsByUserId(const int32_t userId)
{
    std::lock_guard<std::mutex> lock(formDBInfosMutex_);
    std::vector<FormDBInfo>::iterator itRecord;
    for (itRecord = formDBInfos_.begin(); itRecord != formDBInfos_.end();) {
        if (userId == itRecord->userId) {
            int64_t formId = itRecord->formId;
            if (FormInfoRdbStorageMgr::GetInstance().DeleteStorageFormData(std::to_string(formId)) == ERR_OK) {
                itRecord = formDBInfos_.erase(itRecord);
            } else {
                HILOG_ERROR("%{public}s, failed to delete form, formId[%{public}" PRId64 "]", __func__, formId);
                itRecord++;
            }
        } else {
            itRecord++;
        }
    }
}

/**
 * @brief handle get no host invalid DB forms.
 * @param userId User ID.
 * @param callingUid The UID of the proxy.
 * @param matchedFormIds The set of the valid forms.
 * @param noHostDBFormsMap The map of the no host forms.
 * @param foundFormsMap The map of the found forms.
 */
void FormDbCache::GetNoHostInvalidDBForms(int32_t userId, int32_t callingUid, std::set<int64_t> &matchedFormIds,
                                          std::map<FormIdKey, std::set<int64_t>> &noHostDBFormsMap,
                                          std::map<int64_t, bool> &foundFormsMap)
{
    std::lock_guard<std::mutex> lock(formDBInfosMutex_);
    for (auto &formRecord: formDBInfos_) {
        int64_t formId = formRecord.formId;

        // check userID
        if (userId != formRecord.userId) {
            continue;
        }
        // check UID
        auto iter = std::find(formRecord.formUserUids.begin(), formRecord.formUserUids.end(), callingUid);
        if (iter == formRecord.formUserUids.end()) {
            continue;
        }
        // check valid form set
        if (matchedFormIds.find(formId) != matchedFormIds.end()) {
            continue;
        }

        HILOG_DEBUG("found invalid form: %{public}" PRId64 "", formId);
        formRecord.formUserUids.erase(iter);
        if (formRecord.formUserUids.empty()) {
            FormIdKey formIdKey(formRecord.bundleName, formRecord.abilityName);
            auto itIdsSet = noHostDBFormsMap.find(formIdKey);
            if (itIdsSet == noHostDBFormsMap.end()) {
                std::set<int64_t> formIdsSet;
                formIdsSet.emplace(formId);
                noHostDBFormsMap.emplace(formIdKey, formIdsSet);
            } else {
                itIdsSet->second.emplace(formId);
            }
        } else {
            foundFormsMap.emplace(formId, false);
            SaveFormInfoNolock(formRecord);
            FormBmsHelper::GetInstance().NotifyModuleNotRemovable(formRecord.bundleName, formRecord.moduleName);
        }
    }
}

/**
 * @brief handle delete no host DB forms.
 * @param callingUid The UID of the proxy.
 * @param noHostFormDbMap The map of the no host forms.
 * @param foundFormsMap The map of the found forms.
 */
void FormDbCache::BatchDeleteNoHostDBForms(int32_t callingUid, std::map<FormIdKey, std::set<int64_t>> &noHostDBFormsMap,
                                           std::map<int64_t, bool> &foundFormsMap)
{
    std::set<FormIdKey> removableModuleSet;
    for (auto &element : noHostDBFormsMap) {
        std::set<int64_t> &formIds = element.second;
        FormIdKey formIdKey = element.first;
        std::string bundleName = formIdKey.bundleName;
        std::string abilityName = formIdKey.abilityName;
        FormProviderMgr::GetInstance().NotifyProviderFormsBatchDelete(bundleName, abilityName, formIds);
        for (const int64_t formId : formIds) {
            foundFormsMap.emplace(formId, true);
            FormDBInfo dbInfo;
            int errCode = GetDBRecord(formId, dbInfo);
            if (errCode == ERR_OK) {
                FormIdKey removableModuleFormIdKey(dbInfo.bundleName, dbInfo.moduleName);
                removableModuleSet.emplace(removableModuleFormIdKey);
                DeleteFormInfo(formId);
            }
            FormDataMgr::GetInstance().StopRenderingForm(formId);
            FormDataMgr::GetInstance().DeleteFormRecord(formId);
        }
    }

    for (const FormIdKey &item : removableModuleSet) {
        int32_t matchCount = GetMatchCount(item.bundleName, item.moduleName);
        if (matchCount == 0) {
            FormBmsHelper::GetInstance().NotifyModuleRemovable(item.bundleName, item.moduleName);
        }
    }
}

/**
 * @brief handle delete invalid DB forms.
 * @param userId User ID.
 * @param callingUid The UID of the proxy.
 * @param matchedFormIds The set of the valid forms.
 * @param removedFormsMap The map of the removed invalid forms.
 * @return Returns ERR_OK on success, others on failure.
 */
ErrCode FormDbCache::DeleteInvalidDBForms(int32_t userId, int32_t callingUid, std::set<int64_t> &matchedFormIds,
                                          std::map<int64_t, bool> &removedFormsMap)
{
    HILOG_INFO("DeleteInvalidDBForms start, userId = %{public}d, callingUid = %{public}d", userId, callingUid);
    std::map<int64_t, bool> foundFormsMap {};
    std::map<FormIdKey, std::set<int64_t>> noHostDBFormsMap {};

    GetNoHostInvalidDBForms(userId, callingUid, matchedFormIds, noHostDBFormsMap, foundFormsMap);

    if (!foundFormsMap.empty()) {
        for (const auto &element : foundFormsMap) {
            FormDataMgr::GetInstance().DeleteFormUserUid(element.first, callingUid);
        }
    }

    BatchDeleteNoHostDBForms(callingUid, noHostDBFormsMap, foundFormsMap);
    HILOG_DEBUG("foundFormsMap size: %{public}zu", foundFormsMap.size());
    HILOG_DEBUG("noHostDBFormsMap size: %{public}zu", noHostDBFormsMap.size());

    if (!foundFormsMap.empty()) {
        removedFormsMap.insert(foundFormsMap.begin(), foundFormsMap.end());
    }

    HILOG_INFO("DeleteInvalidDBForms done");
    return ERR_OK;
}

bool FormDbCache::IsHostOwner(int64_t formId, int32_t hostUid)
{
    FormDBInfo dbInfo;
    if (GetDBRecord(formId, dbInfo) != ERR_OK) {
        HILOG_ERROR("get db record failed. formId: %{public}s", std::to_string(formId).c_str());
        return false;
    }

    auto iter = std::find(dbInfo.formUserUids.begin(), dbInfo.formUserUids.end(), hostUid);
    if (iter != dbInfo.formUserUids.end()) {
        return true;
    }

    return false;
}
} // namespace AppExecFwk
} // namespace OHOS
