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

#include "form_data_mgr.h"

#include <cinttypes>
#include <type_traits>

#include "form_cache_mgr.h"
#include "form_constants.h"
#include "form_mgr_errors.h"
#include "form_provider_mgr.h"
#include "form_render_mgr.h"
#include "form_util.h"
#include "hilog_wrapper.h"
#include "ipc_skeleton.h"

namespace OHOS {
namespace AppExecFwk {
FormDataMgr::FormDataMgr()
{
    HILOG_INFO("create form data manager instance");
}
FormDataMgr::~FormDataMgr()
{
    HILOG_INFO("destroy form data manager instance");
}

/**
 * @brief Allot form info by item info.
 * @param formInfo Form item info.
 * @param callingUid The UID of the proxy.
 * @param userId User ID.
 * @return Returns form record.
 */
FormRecord FormDataMgr::AllotFormRecord(const FormItemInfo &formInfo, const int callingUid, const int32_t userId)
{
    HILOG_INFO("%{public}s, allot form info", __func__);
    if (formInfo.IsTemporaryForm() && !ExistTempForm(formInfo.GetFormId())) {
        std::lock_guard<std::recursive_mutex> lock(formTempMutex_);
        tempForms_.emplace_back(formInfo.GetFormId());
    }
    FormRecord record;
    {
        std::lock_guard<std::recursive_mutex> lock(formRecordMutex_);
        if (formRecords_.empty()) { // formRecords_ is empty, create a new one
            HILOG_DEBUG("%{public}s, form info not exist", __func__);
            record = CreateFormRecord(formInfo, callingUid, userId);
            formRecords_.emplace(formInfo.GetFormId(), record);
        } else {
            auto info = formRecords_.find(formInfo.GetFormId());
            if (info == formRecords_.end()) {
                HILOG_DEBUG("%{public}s, form info not find", __func__);
                record = CreateFormRecord(formInfo, callingUid, userId);
                formRecords_.emplace(formInfo.GetFormId(), record);
            } else {
                record = info->second;
            }
        }
    }
    HILOG_INFO("%{public}s end", __func__);
    return record;
}
/**
 * @brief Delete form js info by form record.
 * @param formId The Id of the form.
 * @return Returns true if this function is successfully called; returns false otherwise.
 */
bool FormDataMgr::DeleteFormRecord(const int64_t formId)
{
    HILOG_INFO("%{public}s, delete form info", __func__);
    std::lock_guard<std::recursive_mutex> lock(formRecordMutex_);
    auto iter = formRecords_.find(formId);
    if (iter == formRecords_.end()) {
        HILOG_ERROR("%{public}s, form info is not exist", __func__);
        return false;
    }
    formRecords_.erase(iter);
    return true;
}
/**
 * @brief Allot form host record by caller token.
 * @param info The form item info.
 * @param callerToken callerToken
 * @param formId The Id of the form.
 * @param callingUid The UID of the proxy.
 * @return Returns true if this function is successfully called; returns false otherwise.
 */
bool FormDataMgr::AllotFormHostRecord(const FormItemInfo &info, const sptr<IRemoteObject> &callerToken,
    const int64_t formId, const int callingUid)
{
    HILOG_INFO("%{public}s, allot form Host info", __func__);
    std::lock_guard<std::recursive_mutex> lock(formHostRecordMutex_);
    for (auto &record : clientRecords_) {
        if (callerToken == record.GetFormHostClient()) {
            record.AddForm(formId);
            HILOG_INFO("%{public}s end", __func__);
            return true;
        }
    }
    FormHostRecord hostRecord;
    bool isCreated = CreateHostRecord(info, callerToken, callingUid, hostRecord);
    if (isCreated) {
        hostRecord.AddForm(formId);
        clientRecords_.emplace_back(hostRecord);
        HILOG_INFO("%{public}s end", __func__);
        return true;
    }
    HILOG_INFO("%{public}s end", __func__);
    return false;
}
/**
 * @brief Create host record.
 * @param info The form item info.
 * @param callerToken The UID of the proxy.
 * @param callingUid The UID of the proxy.
 * @param record The form host record.
 * @return Returns true if this function is successfully called; returns false otherwise.
 */
bool FormDataMgr::CreateHostRecord(const FormItemInfo &info, const sptr<IRemoteObject> &callerToken,
    const int callingUid, FormHostRecord& record)
{
    if (callerToken == nullptr) {
        HILOG_ERROR("%{public}s, invalid param", __func__);
        return false;
    }

    record = FormHostRecord::CreateRecord(info, callerToken, callingUid);
    return true;
}
/**
 * @brief Create form record.
 * @param formInfo The form item info.
 * @param callingUid The UID of the proxy.
 * @param userId User ID.
 * @return Form record.
 */
FormRecord FormDataMgr::CreateFormRecord(const FormItemInfo &formInfo, const int callingUid, const int32_t userId) const
{
    HILOG_INFO("%{public}s, create form info", __func__);
    FormRecord newRecord;
    newRecord.formId = formInfo.GetFormId();
    newRecord.userId = userId;
    newRecord.packageName = formInfo.GetPackageName();
    newRecord.bundleName = formInfo.GetProviderBundleName();
    newRecord.moduleName = formInfo.GetModuleName();
    newRecord.abilityName = formInfo.GetAbilityName();
    newRecord.formName = formInfo.GetFormName();
    newRecord.specification = formInfo.GetSpecificationId();
    newRecord.isEnableUpdate = formInfo.IsEnableUpdateFlag();
    newRecord.formTempFlag = formInfo.IsTemporaryForm();
    newRecord.formVisibleNotify = formInfo.IsFormVisibleNotify();
    newRecord.jsFormCodePath = formInfo.GetHapSourceByModuleName(newRecord.moduleName);
    newRecord.formSrc = formInfo.GetFormSrc();
    newRecord.formWindow = formInfo.GetFormWindow();
    newRecord.versionName = formInfo.GetVersionName();
    newRecord.versionCode = formInfo.GetVersionCode();
    newRecord.compatibleVersion = formInfo.GetCompatibleVersion();
    newRecord.formVisibleNotifyState = 0;
    newRecord.type = formInfo.GetType();
    newRecord.uiSyntax = formInfo.GetUiSyntax();
    if (newRecord.isEnableUpdate) {
        ParseUpdateConfig(newRecord, formInfo);
    }
    if (std::find(newRecord.formUserUids.begin(), newRecord.formUserUids.end(),
        callingUid) == newRecord.formUserUids.end()) {
        newRecord.formUserUids.emplace_back(callingUid);
    }

    formInfo.GetHapSourceDirs(newRecord.hapSourceDirs);
    HILOG_INFO("%{public}s end", __func__);
    return newRecord;
}
/**
 * @brief Create form js info by form record.
 * @param formId The Id of the form.
 * @param record Form record.
 * @param formInfo Js form info.
 */
void FormDataMgr::CreateFormJsInfo(const int64_t formId, const FormRecord &record, FormJsInfo &formInfo)
{
    formInfo.formId = formId;
    formInfo.bundleName = record.bundleName;
    formInfo.abilityName = record.abilityName;
    formInfo.formName = record.formName;
    formInfo.moduleName = record.moduleName;
    formInfo.formTempFlag = record.formTempFlag;
    formInfo.jsFormCodePath = record.jsFormCodePath;
    formInfo.formSrc = record.formSrc;
    formInfo.formWindow = record.formWindow;
    formInfo.versionCode = record.versionCode;
    formInfo.versionName = record.versionName;
    formInfo.compatibleVersion = record.compatibleVersion;
    formInfo.type = record.type;
    formInfo.uiSyntax = record.uiSyntax;
}
/**
 * @brief Check temp form count is max.
 * @return Returns ERR_OK if the temp form not reached; returns ERR_MAX_SYSTEM_TEMP_FORMS is reached.
 */
int FormDataMgr::CheckTempEnoughForm() const
{
    std::lock_guard<std::recursive_mutex> lock(formTempMutex_);
    if (tempForms_.size() >= Constants::MAX_TEMP_FORMS) {
        HILOG_WARN("%{public}s, already exist %{public}d temp forms in system", __func__, Constants::MAX_TEMP_FORMS);
        return ERR_APPEXECFWK_FORM_MAX_SYSTEM_TEMP_FORMS;
    }
    return ERR_OK;
}
/**
 * @brief Check form count is max.
 * @param callingUid The UID of the proxy.
 * @param currentUserId The current userId.
 * @return Returns true if this function is successfully called; returns false otherwise.
 */
int FormDataMgr::CheckEnoughForm(const int callingUid, const int32_t currentUserId) const
{
    HILOG_INFO("%{public}s, callingUid: %{public}d, current userId: %{public}d", __func__, callingUid, currentUserId);

    int formsInSystem = 0;
    int callingUidFormCounts = 0;
    std::lock_guard<std::recursive_mutex> lock(formRecordMutex_);
    for (const auto &recordPair : formRecords_) {
        FormRecord record = recordPair.second;
        if ((record.userId == currentUserId) && !record.formTempFlag) {
            if (++formsInSystem >= Constants::MAX_FORMS) {
                HILOG_WARN("%{public}s, already exist %{public}d forms in system", __func__, Constants::MAX_FORMS);
                return ERR_APPEXECFWK_FORM_MAX_SYSTEM_FORMS;
            }
            for (const auto &userUid : record.formUserUids) {
                if (userUid != callingUid) {
                    continue;
                }
                if (++callingUidFormCounts >= Constants::MAX_RECORD_PER_APP) {
                    HILOG_WARN("%{public}s, already use %{public}d forms", __func__, Constants::MAX_RECORD_PER_APP);
                    return ERR_APPEXECFWK_FORM_MAX_FORMS_PER_CLIENT;
                }
                break;
            }
        }
    }
    return ERR_OK;
}
/**
 * @brief Delete temp form.
 * @param formId The Id of the form.
 * @return Returns true if this function is successfully called; returns false otherwise.
 */
bool FormDataMgr::DeleteTempForm(const int64_t formId)
{
    std::lock_guard<std::recursive_mutex> lock(formTempMutex_);
    auto iter = std::find(tempForms_.begin(), tempForms_.end(), formId);
    if (iter == tempForms_.end()) {
        HILOG_ERROR("%{public}s, temp form is not exist", __func__);
        return false;
    }
    tempForms_.erase(iter);
    return true;
}
/**
 * @brief Check temp form is exist.
 * @param formId The Id of the form.
 * @return Returns true if the temp form is exist; returns false is not exist.
 */
bool FormDataMgr::ExistTempForm(const int64_t formId) const
{
    std::lock_guard<std::recursive_mutex> lock(formTempMutex_);
    return (std::find(tempForms_.begin(), tempForms_.end(), formId) != tempForms_.end());
}
/**
 * @brief Check calling uid is valid.
 * @param formUserUids The form user uids.
 * @return Returns true if this user uid is valid; returns false otherwise.
 */
bool FormDataMgr::IsCallingUidValid(const std::vector<int> &formUserUids) const
{
    if (formUserUids.empty()) {
        return false;
    }
    for (const auto &userUid : formUserUids) {
        if (userUid == IPCSkeleton::GetCallingUid()) {
            return true;
        }
    }
    return false;
}
/**
 * @brief Modify form temp flag by formId.
 * @param formId The Id of the form.
 * @param formTempFlag The form temp flag.
 * @return Returns true if this function is successfully called; returns false otherwise.
 */
bool FormDataMgr::ModifyFormTempFlag(const int64_t formId, const bool formTempFlag)
{
    HILOG_INFO("%{public}s, modify form temp flag by formId", __func__);
    std::lock_guard<std::recursive_mutex> lock(formRecordMutex_);
    if (!ExistFormRecord(formId)) {
        HILOG_ERROR("%{public}s, form info is not exist", __func__);
        return false;
    }
    formRecords_[formId].formTempFlag = formTempFlag;
    return true;
}
/**
 * @brief Add form user uid from form record.
 * @param formId The Id of the form.
 * @param formRecord The form record.
 * @return Returns true if this function is successfully called; returns false otherwise.
 */
bool FormDataMgr::AddFormUserUid(const int64_t formId, const int formUserUid)
{
    HILOG_INFO("%{public}s, add form user uid by formId", __func__);
    std::lock_guard<std::recursive_mutex> lock(formRecordMutex_);
    if (!ExistFormRecord(formId)) {
        HILOG_ERROR("%{public}s, form info is not exist", __func__);
        return false;
    }
    if (std::find(formRecords_[formId].formUserUids.begin(), formRecords_[formId].formUserUids.end(),
        formUserUid) == formRecords_[formId].formUserUids.end()) {
        formRecords_[formId].formUserUids.emplace_back(formUserUid);
    }
    return true;
}
/**
 * @brief Delete form user uid from form record.
 * @param formId The Id of the form.
 * @param uid calling user id.
 * @return Returns true if this function is successfully called; returns false otherwise.
 */
bool FormDataMgr::DeleteFormUserUid(const int64_t formId, const int uid)
{
    HILOG_INFO("%{public}s, delete form user uid from form record", __func__);
    std::lock_guard<std::recursive_mutex> lock(formRecordMutex_);
    if (ExistFormRecord(formId)) {
        auto iter = std::find(formRecords_.at(formId).formUserUids.begin(),
            formRecords_.at(formId).formUserUids.end(), uid);
        if (iter != formRecords_.at(formId).formUserUids.end()) {
            formRecords_.at(formId).formUserUids.erase(iter);
        }
        return true;
    } else {
        HILOG_ERROR("%{public}s, form info not find", __func__);
        return false;
    }
}
/**
 * @brief Update form record.
 * @param formId The Id of the form.
 * @param formRecord The form record.
 * @return Returns true if this function is successfully called; returns false otherwise.
 */
bool FormDataMgr::UpdateFormRecord(const int64_t formId, const FormRecord &formRecord)
{
    HILOG_INFO("%{public}s, get form record by formId", __func__);
    std::lock_guard<std::recursive_mutex> lock(formRecordMutex_);
    auto info = formRecords_.find(formId);
    if (info != formRecords_.end()) {
        formRecords_[formId] = formRecord;
        return true;
    }
    return false;
}
/**
 * @brief Get form record.
 * @param formId The Id of the form.
 * @param formRecord The form record.
 * @return Returns true if this function is successfully called; returns false otherwise.
 */
bool FormDataMgr::GetFormRecord(const int64_t formId, FormRecord &formRecord) const
{
    HILOG_INFO("%{public}s, get form record by formId", __func__);
    std::lock_guard<std::recursive_mutex> lock(formRecordMutex_);
    auto info = formRecords_.find(formId);
    if (info == formRecords_.end()) {
        HILOG_ERROR("%{public}s, form info not find", __func__);
        return false;
    }
    formRecord = info->second;

    HILOG_INFO("%{public}s, get form record successfully", __func__);
    return true;
}
/**
 * @brief Get form record.
 * @param bundleName Bundle name.
 * @param formInfos The form record.
 * @return Returns true if this function is successfully called; returns false otherwise.
 */
bool FormDataMgr::GetFormRecord(const std::string &bundleName, std::vector<FormRecord> &formInfos)
{
    HILOG_INFO("%{public}s, get form record by bundleName", __func__);
    std::lock_guard<std::recursive_mutex> lock(formRecordMutex_);
    std::map<int64_t, FormRecord>::iterator itFormRecord;
    for (itFormRecord = formRecords_.begin(); itFormRecord != formRecords_.end(); itFormRecord++) {
        if (bundleName == itFormRecord->second.bundleName) {
            formInfos.emplace_back(itFormRecord->second);
        }
    }
    if (formInfos.size() > 0) {
        return true;
    } else {
        HILOG_INFO("%{public}s, form info not find", __func__);
        return false;
    }
}
/**
 * @brief Check form record is exist.
 * @param formId The Id of the form.
 * @return Returns true if the form record is exist; returns false is not exist.
 */
bool FormDataMgr::ExistFormRecord(const int64_t formId) const
{
    HILOG_INFO("%{public}s, check form record is exist", __func__);
    std::lock_guard<std::recursive_mutex> lock(formRecordMutex_);
    return (formRecords_.count(formId) > 0);
}
/**
 * @brief Has form user uids in form record.
 * @param formId The Id of the form.
 * @return Returns true if this form has form user uids; returns false is not has.
 */
bool FormDataMgr::HasFormUserUids(const int64_t formId) const
{
    HILOG_INFO("%{public}s, check form has user uids", __func__);
    FormRecord record;
    if (GetFormRecord(formId, record)) {
        return record.formUserUids.empty() ? false : true;
    }
    return false;
}
/**
 * @brief Get form host record.
 * @param formId The id of the form.
 * @param formHostRecord The form host record.
 */
void FormDataMgr::GetFormHostRecord(const int64_t formId, std::vector<FormHostRecord> &formHostRecords) const
{
    std::lock_guard<std::recursive_mutex> lock(formHostRecordMutex_);
    for (auto &record : clientRecords_) {
        if (record.Contains(formId)) {
            formHostRecords.emplace_back(record);
        }
    }
    HILOG_DEBUG("%{public}s, get form host record by formId, size is %{public}zu", __func__, formHostRecords.size());
}
void FormDataMgr::GetFormHostRemoteObj(const int64_t formId, std::vector<sptr<IRemoteObject>> &formHostObjs) const
{
    std::lock_guard<std::recursive_mutex> lock(formHostRecordMutex_);
    for (auto &record : clientRecords_) {
        if (record.Contains(formId)) {
            formHostObjs.emplace_back(record.GetFormHostClient());
        }
    }
    HILOG_DEBUG("Get form host remote object by formId, size is %{public}zu", formHostObjs.size());
}
/**
 * @brief Delete form host record.
 * @param callerToken The client stub of the form host record.
 * @param formId The id of the form.
 * @return Returns true if this function is successfully called; returns false otherwise.
 */
bool FormDataMgr::DeleteHostRecord(const sptr<IRemoteObject> &callerToken, const int64_t formId)
{
    HILOG_INFO("%{public}s start, delete form host record", __func__);
    std::lock_guard<std::recursive_mutex> lock(formHostRecordMutex_);
    std::vector<FormHostRecord>::iterator iter;
    for (iter = clientRecords_.begin(); iter != clientRecords_.end(); ++iter) {
        if (callerToken == iter->GetFormHostClient()) {
            iter->DelForm(formId);
            if (iter->IsEmpty()) {
                iter->CleanResource();
                iter = clientRecords_.erase(iter);
                FormRenderMgr::GetInstance().CleanFormHost(callerToken);
            }
            break;
        }
    }
    HILOG_INFO("%{public}s end", __func__);
    return true;
}
/**
 * @brief Clean removed forms form host.
 * @param removedFormIds The id list of the forms.
 */
void FormDataMgr::CleanHostRemovedForms(const std::vector<int64_t> &removedFormIds)
{
    HILOG_INFO("%{public}s start, delete form host record by formId list", __func__);
    std::vector<int64_t> matchedIds;
    std::lock_guard<std::recursive_mutex> lock(formHostRecordMutex_);
    std::vector<FormHostRecord>::iterator itHostRecord;
    for (itHostRecord = clientRecords_.begin(); itHostRecord != clientRecords_.end(); itHostRecord++) {
        for (const int64_t& formId : removedFormIds) {
            if (itHostRecord->Contains(formId)) {
                matchedIds.emplace_back(formId);
                itHostRecord->DelForm(formId);
            }
        }
        if (!matchedIds.empty()) {
            HILOG_INFO("%{public}s, OnFormUninstalled called", __func__);
            itHostRecord->OnFormUninstalled(matchedIds);
        }
    }

    HILOG_INFO("%{public}s end", __func__);
}

void FormDataMgr::UpdateHostForms(const std::vector<int64_t> &updateFormIds)
{
    HILOG_INFO("%{public}s start, update form host record by formId list", __func__);
    std::vector<int64_t> matchedIds;
    std::lock_guard<std::recursive_mutex> lock(formHostRecordMutex_);
    for (FormHostRecord &record : clientRecords_) {
        for (const int64_t &formId : updateFormIds) {
            if (record.Contains(formId)) {
                matchedIds.emplace_back(formId);
            }
        }
        if (!matchedIds.empty()) {
            HILOG_INFO("%{public}s, OnFormUninstalled called", __func__);
            record.OnFormUninstalled(matchedIds);
            matchedIds.clear();
        }
    }
    HILOG_INFO("%{public}s end", __func__);
}

/**
 * @brief Handle form host died.
 * @param remoteHost Form host proxy object.
 */
void FormDataMgr::HandleHostDied(const sptr<IRemoteObject> &remoteHost)
{
    std::vector<int64_t> recordTempForms;
    {
        std::lock_guard<std::recursive_mutex> lock(formHostRecordMutex_);
        std::vector<FormHostRecord>::iterator itHostRecord;
        for (itHostRecord = clientRecords_.begin(); itHostRecord != clientRecords_.end();) {
            if (remoteHost == itHostRecord->GetFormHostClient()) {
                HandleHostDiedForTempForms(*itHostRecord, recordTempForms);
                HILOG_INFO("find died client, remove it");
                itHostRecord->CleanResource();
                itHostRecord = clientRecords_.erase(itHostRecord);
                break;
            } else {
                itHostRecord++;
            }
        }
    }
    {
        std::lock_guard<std::recursive_mutex> lock(formRecordMutex_);
        std::map<int64_t, FormRecord>::iterator itFormRecord;
        for (itFormRecord = formRecords_.begin(); itFormRecord != formRecords_.end();) {
            int64_t formId = itFormRecord->first;
            // if temp form, remove it
            if (std::find(recordTempForms.begin(), recordTempForms.end(), formId) != recordTempForms.end()) {
                FormRecord formRecord = itFormRecord->second;
                itFormRecord = formRecords_.erase(itFormRecord);
                FormProviderMgr::GetInstance().NotifyProviderFormDelete(formId, formRecord);
            } else {
                itFormRecord++;
            }
        }
    }
    {
        std::lock_guard<std::recursive_mutex> lock(formStateRecordMutex_);
        std::map<std::string, FormHostRecord>::iterator itFormStateRecord;
        for (itFormStateRecord = formStateRecord_.begin(); itFormStateRecord != formStateRecord_.end();) {
            if (remoteHost == itFormStateRecord->second.GetFormHostClient()) {
                HILOG_INFO("find died client, remove it from formStateRecord_");
                itFormStateRecord->second.CleanResource();
                itFormStateRecord = formStateRecord_.erase(itFormStateRecord);
                break;
            } else {
                itFormStateRecord++;
            }
        }
    }
    FormRenderMgr::GetInstance().CleanFormHost(remoteHost);
}

/**
 * @brief Get the temp forms from host and delete temp form in cache.
 * @param record The form record.
 * @param recordTempForms gotten the temp forms.
 */
void FormDataMgr::HandleHostDiedForTempForms(const FormHostRecord &record, std::vector<int64_t> &recordTempForms)
{
    std::lock_guard<std::recursive_mutex> lock(formTempMutex_);
    std::vector<int64_t>::iterator itForm;
    for (itForm = tempForms_.begin(); itForm != tempForms_.end();) {
        if (record.Contains(*itForm)) {
            recordTempForms.emplace_back(*itForm);
            itForm = tempForms_.erase(itForm);
        } else {
            itForm++;
        }
    }
}

/**
 * @brief Refresh enable or not.
 * @param formId The Id of the form.
 * @return true on enable, false on disable.
 */
bool FormDataMgr::IsEnableRefresh(int64_t formId)
{
    std::lock_guard<std::recursive_mutex> lock(formHostRecordMutex_);
    for (auto &record : clientRecords_) {
        if (record.IsEnableRefresh(formId)) {
            return true;
        }
    }

    return false;
}

/**
 * @brief update enable or not.
 * @param formId The Id of the form.
 * @return true on enable, false on disable.
 */
bool FormDataMgr::IsEnableUpdate(int64_t formId)
{
    std::lock_guard<std::recursive_mutex> lock(formHostRecordMutex_);
    for (auto &record : clientRecords_) {
        if (record.IsEnableUpdate(formId)) {
            return true;
        }
    }
    return false;
}

int64_t FormDataMgr::PaddingUdidHash(const int64_t formId)
{
    if (!GenerateUdidHash()) {
        return -1;
    }
    // Compatible with int form id.
    uint64_t unsignedFormId = static_cast<uint64_t>(formId);
    if ((unsignedFormId & 0xffffffff00000000L) == 0) {
        uint64_t unsignedUdidHash  = static_cast<uint64_t>(udidHash_);
        uint64_t unsignedUdidHashFormId = unsignedUdidHash | unsignedFormId;
        int64_t udidHashFormId = static_cast<int64_t>(unsignedUdidHashFormId);
        return udidHashFormId;
    }
    return formId;
}

/**
 * @brief Generate form id.
 * @return form id.
 */
int64_t FormDataMgr::GenerateFormId()
{
    // generate by udidHash_
    if (!GenerateUdidHash()) {
        HILOG_ERROR("%{public}s fail, generateFormId no invalid udidHash_", __func__);
        return -1;
    }
    return FormUtil::GenerateFormId(udidHash_);
}
/**
 * @brief Generate udid.
 * @return Returns true if this function is successfully called; returns false otherwise.
 */
bool FormDataMgr::GenerateUdidHash()
{
    if (udidHash_ != Constants::INVALID_UDID_HASH) {
        return true;
    }

    bool genUdid = FormUtil::GenerateUdidHash(udidHash_);
    if (!genUdid) {
        HILOG_ERROR("%{public}s, Failed to generate udid.", __func__);
        return false;
    }

    return true;
}
/**
 * @brief Get udid.
 * @return udid.
 */
int64_t FormDataMgr::GetUdidHash() const
{
    return udidHash_;
}
/**
 * @brief Set udid.
 * @param udidHash udid.
 */
void FormDataMgr::SetUdidHash(const int64_t udidHash)
{
    udidHash_ = udidHash;
}

/**
 * @brief Get the matched form host record by client stub.
 *
 * @param callerToken The client stub of the form host record.
 * @param formHostRecord The form host record.
 * @return Returns true if this function is successfully called, returns false otherwise.
 */
bool FormDataMgr::GetMatchedHostClient(const sptr<IRemoteObject> &callerToken, FormHostRecord &formHostRecord) const
{
    HILOG_INFO("%{public}s, get the matched form host record by client stub.", __func__);
    std::lock_guard<std::recursive_mutex> lock(formHostRecordMutex_);
    for (const FormHostRecord &record : clientRecords_) {
        if (callerToken == record.GetFormHostClient()) {
            formHostRecord = record;
            return true;
        }
    }

    HILOG_ERROR("%{public}s, form host record not find.", __func__);
    return false;
}

/**
 * @brief Set needRefresh for FormRecord.
 * @param formId The Id of the form.
 * @param needRefresh true or false.
 */
void FormDataMgr::SetNeedRefresh(const int64_t formId, const bool needRefresh)
{
    std::lock_guard<std::recursive_mutex> lock(formRecordMutex_);
    auto itFormRecord = formRecords_.find(formId);
    if (itFormRecord == formRecords_.end()) {
        HILOG_ERROR("%{public}s, form info not find", __func__);
        return;
    }
    itFormRecord->second.needRefresh = needRefresh;
}

/**
 * @brief Set isCountTimerRefresh for FormRecord.
 * @param formId The Id of the form.
 * @param countTimerRefresh true or false.
 */
void FormDataMgr::SetCountTimerRefresh(const int64_t formId, const bool countTimerRefresh)
{
    std::lock_guard<std::recursive_mutex> lock(formRecordMutex_);
    auto itFormRecord = formRecords_.find(formId);
    if (itFormRecord == formRecords_.end()) {
        HILOG_ERROR("%{public}s, form info not find", __func__);
        return;
    }
    itFormRecord->second.isCountTimerRefresh = countTimerRefresh;
}

/**
 * @brief Set isTimerRefresh for FormRecord.
 * @param formId The Id of the form.
 * @param timerRefresh true or false.
 */
void FormDataMgr::SetTimerRefresh(const int64_t formId, const bool timerRefresh)
{
    std::lock_guard<std::recursive_mutex> lock(formRecordMutex_);
    auto itFormRecord = formRecords_.find(formId);
    if (itFormRecord == formRecords_.end()) {
        HILOG_ERROR("%{public}s, form info not find", __func__);
        return;
    }
    itFormRecord->second.isTimerRefresh = timerRefresh;
}

/**
 * @brief Get updated form.
 * @param record FormRecord.
 * @param targetForms Target forms.
 * @param updatedForm Updated formnfo.
 * @return Returns true on success, false on failure.
 */
bool FormDataMgr::GetUpdatedForm(const FormRecord &record, const std::vector<FormInfo> &targetForms,
    FormInfo &updatedForm)
{
    if (targetForms.empty()) {
        HILOG_ERROR("%{public}s error, targetForms is empty.", __func__);
        return false;
    }

    for (const FormInfo &item : targetForms) {
        if (IsSameForm(record, item)) {
            updatedForm = item;
            HILOG_DEBUG("%{public}s, find matched form.", __func__);
            return true;
        }
    }
    return false;
}
/**
 * @brief Set isEnableUpdate for FormRecord.
 * @param formId The Id of the form.
 * @param enableUpdate true or false.
 */
void FormDataMgr::SetEnableUpdate(const int64_t formId, const bool enableUpdate)
{
    std::lock_guard<std::recursive_mutex> lock(formRecordMutex_);
    auto itFormRecord = formRecords_.find(formId);
    if (itFormRecord == formRecords_.end()) {
        HILOG_ERROR("%{public}s, form info not find", __func__);
        return;
    }
    itFormRecord->second.isEnableUpdate = enableUpdate;
}
/**
 * @brief Set update info for FormRecord.
 * @param formId The Id of the form.
 * @param enableUpdate true or false.
 * @param updateDuration Update duration.
 * @param updateAtHour Update at hour.
 * @param updateAtMin Update at minute.
 */
void FormDataMgr::SetUpdateInfo(
    const int64_t formId,
    const bool enableUpdate,
    const long updateDuration,
    const int updateAtHour,
    const int updateAtMin)
{
    std::lock_guard<std::recursive_mutex> lock(formRecordMutex_);
    auto itFormRecord = formRecords_.find(formId);
    if (itFormRecord == formRecords_.end()) {
        HILOG_ERROR("%{public}s, form info not find", __func__);
        return;
    }

    itFormRecord->second.isEnableUpdate = enableUpdate;
    itFormRecord->second.updateDuration = updateDuration;
    itFormRecord->second.updateAtHour = updateAtHour;
    itFormRecord->second.updateAtMin = updateAtMin;
}
/**
 * @brief Check if two forms is same or not.
 * @param record FormRecord.
 * @param formInfo FormInfo.
 * @return Returns true on success, false on failure.
 */
bool FormDataMgr::IsSameForm(const FormRecord &record, const FormInfo &formInfo)
{
    if (record.bundleName == formInfo.bundleName
        && record.moduleName == formInfo.moduleName
        && record.abilityName == formInfo.abilityName
        && record.formName == formInfo.name
        && std::find(formInfo.supportDimensions.begin(), formInfo.supportDimensions.end(), record.specification)
        != formInfo.supportDimensions.end()) {
        return true;
    }

    return false;
}
/**
 * @brief Clean removed form records.
 * @param removedForms The id list of the forms.
 */
void FormDataMgr::CleanRemovedFormRecords(const std::string &bundleName, std::set<int64_t> &removedForms)
{
    HILOG_INFO("%{public}s, clean removed form records", __func__);
    std::lock_guard<std::recursive_mutex> lock(formRecordMutex_);
    std::map<int64_t, FormRecord>::iterator itFormRecord;
    for (itFormRecord = formRecords_.begin(); itFormRecord != formRecords_.end();) {
        auto itForm = std::find(removedForms.begin(), removedForms.end(), itFormRecord->first);
        if (itForm != removedForms.end()) {
            FormRenderMgr::GetInstance().StopRenderingForm(itFormRecord->first, itFormRecord->second);
            itFormRecord = formRecords_.erase(itFormRecord);
        } else {
            itFormRecord++;
        }
    }
}
/**
 * @brief Clean removed temp form records.
 * @param  bundleName BundleName.
 * @param removedForms The id list of the forms.
 */
void FormDataMgr::CleanRemovedTempFormRecords(const std::string &bundleName, const int32_t userId,
    std::set<int64_t> &removedForms)
{
    HILOG_INFO("%{public}s, clean removed form records", __func__);
    std::set<int64_t> removedTempForms;
    {
        std::lock_guard<std::recursive_mutex> lock(formRecordMutex_);
        std::map<int64_t, FormRecord>::iterator itFormRecord;
        for (itFormRecord = formRecords_.begin(); itFormRecord != formRecords_.end();) {
            if ((itFormRecord->second.formTempFlag) && (bundleName == itFormRecord->second.bundleName)
                && (userId == itFormRecord->second.userId)) {
                removedTempForms.emplace(itFormRecord->second.formId);
                FormRenderMgr::GetInstance().StopRenderingForm(itFormRecord->first, itFormRecord->second);
                itFormRecord = formRecords_.erase(itFormRecord);
            } else {
                itFormRecord++;
            }
        }
    }

    if (removedTempForms.size() > 0) {
        std::lock_guard<std::recursive_mutex> lock(formTempMutex_);
        std::vector<int64_t>::iterator itTemp;
        for (itTemp = tempForms_.begin(); itTemp != tempForms_.end();) {
            if (removedTempForms.find(*itTemp) != removedTempForms.end()) {
                itTemp = tempForms_.erase(itTemp);
            } else {
                itTemp++;
            }
        }
        removedForms.merge(removedTempForms);
    }
}
/**
 * @brief Get recreate form records.
 * @param reCreateForms The id list of the forms.
 */
void FormDataMgr::GetReCreateFormRecordsByBundleName(const std::string &bundleName, std::set<int64_t> &reCreateForms)
{
    std::lock_guard<std::recursive_mutex> lock(formRecordMutex_);
    std::map<int64_t, FormRecord>::iterator itFormRecord;
    for (itFormRecord = formRecords_.begin(); itFormRecord != formRecords_.end(); itFormRecord++) {
        if (bundleName == itFormRecord->second.bundleName) {
            reCreateForms.emplace(itFormRecord->second.formId);
        }
    }
}
/**
 * @brief Set form isInited flag.
 * @param formId The Id of the form.
 * @param isInited isInited property
 */
void FormDataMgr::SetFormCacheInited(const int64_t formId, bool isInited)
{
    std::lock_guard<std::recursive_mutex> lock(formRecordMutex_);
    auto itFormRecord = formRecords_.find(formId);
    if (itFormRecord == formRecords_.end()) {
        HILOG_ERROR("%{public}s, form info not find", __func__);
        return;
    }
    itFormRecord->second.isInited = isInited;
    itFormRecord->second.needRefresh = !isInited;
}
/**
 * @brief Set versionUpgrade.
 * @param formId The Id of the form.
 * @param versionUpgrade true or false
 */
void FormDataMgr::SetVersionUpgrade(const int64_t formId, const bool versionUpgrade)
{
    std::lock_guard<std::recursive_mutex> lock(formRecordMutex_);
    auto itFormRecord = formRecords_.find(formId);
    if (itFormRecord == formRecords_.end()) {
        HILOG_ERROR("%{public}s, form info not find", __func__);
        return;
    }
    itFormRecord->second.versionUpgrade = versionUpgrade;
}
/**
 * @brief Update form for host clients.
 * @param formId The Id of the form.
 * @param needRefresh true or false
 */
void FormDataMgr::UpdateHostNeedRefresh(const int64_t formId, const bool needRefresh)
{
    std::lock_guard<std::recursive_mutex> lock(formHostRecordMutex_);
    std::vector<FormHostRecord>::iterator itHostRecord;
    for (itHostRecord = clientRecords_.begin(); itHostRecord != clientRecords_.end(); itHostRecord++) {
        if (itHostRecord->Contains(formId)) {
            itHostRecord->SetNeedRefresh(formId, needRefresh);
        }
    }
}

/**
 * @brief Update form for host clients.
 * @param formId The Id of the form.
 * @param formRecord The form info.
 * @return Returns true if form update, false if other.
 */
bool FormDataMgr::UpdateHostForm(const int64_t formId, const FormRecord &formRecord)
{
    bool isUpdated = false;
    std::lock_guard<std::recursive_mutex> lock(formHostRecordMutex_);
    std::vector<FormHostRecord>::iterator itHostRecord;
    for (itHostRecord = clientRecords_.begin(); itHostRecord != clientRecords_.end(); itHostRecord++) {
        bool enableRefresh = formRecord.isVisible || itHostRecord->IsEnableUpdate(formId) ||
                             itHostRecord->IsEnableRefresh(formId);
        HILOG_INFO("formId:%{public}" PRId64 " enableRefresh:%{public}d", formId, enableRefresh);
        if (enableRefresh) {
            // update form
            itHostRecord->OnUpdate(formId, formRecord);
            // set needRefresh
            itHostRecord->SetNeedRefresh(formId, false);
            isUpdated = true;
        }
    }
    return isUpdated;
}

ErrCode FormDataMgr::HandleUpdateHostFormFlag(const std::vector<int64_t> &formIds, bool flag, bool isOnlyEnableUpdate,
                                              FormHostRecord &formHostRecord, std::vector<int64_t> &refreshForms)
{
    for (const int64_t formId : formIds) {
        if (formId <= 0) {
            HILOG_WARN("%{public}s, formId %{public}" PRId64 " is less than 0", __func__, formId);
            continue;
        }

        int64_t matchedFormId = FindMatchedFormId(formId);
        if (!formHostRecord.Contains(matchedFormId)) {
            HILOG_WARN("%{public}s, form %{public}" PRId64 "is not owned by this client, don't need to update flag",
                __func__, formId);
            continue;
        }

        if (isOnlyEnableUpdate) {
            // new API: this flag is used only to control enable update
            formHostRecord.SetEnableUpdate(matchedFormId, flag);
            formHostRecord.SetEnableRefresh(matchedFormId, false);
        } else {
            // old API: this flag is used to control enable update and visible update
            formHostRecord.SetEnableRefresh(matchedFormId, flag);
        }

        // set disable
        if (!flag) {
            HILOG_INFO("%{public}s, flag is disable", __func__);
            continue;
        }
        FormRecord formRecord;
        if (GetFormRecord(matchedFormId, formRecord)) {
            if (formRecord.needRefresh) {
                HILOG_INFO("%{public}s, formRecord need refresh", __func__);
                refreshForms.emplace_back(matchedFormId);
                continue;
            }
        } else {
            HILOG_WARN("%{public}s, not exist such form:%{public}" PRId64 "", __func__, matchedFormId);
            continue;
        }

        // if set enable flag, should check whether to refresh form
        if (!formHostRecord.IsNeedRefresh(matchedFormId)) {
            HILOG_INFO("%{public}s, host need not refresh", __func__);
            continue;
        }

        if (IsFormCached(formRecord)) {
            HILOG_INFO("%{public}s, form cached", __func__);
            formHostRecord.OnUpdate(matchedFormId, formRecord);
            formHostRecord.SetNeedRefresh(matchedFormId, false);
        } else {
            HILOG_INFO("%{public}s, form no cache", __func__);
            refreshForms.emplace_back(matchedFormId);
            continue;
        }
    }
    return ERR_OK;
}

/**
 * @brief handle update form flag.
 * @param formIDs The id of the forms.
 * @param callerToken Caller ability token.
 * @param flag form flag.
 * @param isOnlyEnableUpdate form enable update form flag.
 * @param refreshForms Refresh forms
 * @return Returns ERR_OK on success, others on failure.
 */
ErrCode FormDataMgr::UpdateHostFormFlag(const std::vector<int64_t> &formIds, const sptr<IRemoteObject> &callerToken,
                                        bool flag, bool isOnlyEnableUpdate, std::vector<int64_t> &refreshForms)
{
    HILOG_INFO("%{public}s start, flag: %{public}d", __func__, flag);
    std::lock_guard<std::recursive_mutex> lock(formHostRecordMutex_);
    std::vector<FormHostRecord>::iterator itHostRecord;
    for (itHostRecord = clientRecords_.begin(); itHostRecord != clientRecords_.end(); itHostRecord++) {
        if (callerToken == itHostRecord->GetFormHostClient()) {
            HandleUpdateHostFormFlag(formIds, flag, isOnlyEnableUpdate, *itHostRecord, refreshForms);
            HILOG_INFO("%{public}s end.", __func__);
            return ERR_OK;
        }
    }
    HILOG_ERROR("%{public}s, can't find target client", __func__);
    return ERR_APPEXECFWK_FORM_OPERATION_NOT_SELF;
}
/**
 * @brief Find matched form id.
 * @param formId The form id.
 * @return Matched form id.
 */
int64_t FormDataMgr::FindMatchedFormId(const int64_t formId)
{
    uint64_t unsignedFormId = static_cast<uint64_t>(formId);
    if ((unsignedFormId & 0xffffffff00000000L) != 0) {
        return formId;
    }
    std::lock_guard<std::recursive_mutex> lock(formRecordMutex_);
    std::map<int64_t, FormRecord>::iterator itFormRecord;
    for (itFormRecord = formRecords_.begin(); itFormRecord != formRecords_.end(); itFormRecord++) {
        uint64_t unsignedFormId = static_cast<uint64_t>(formId);
        uint64_t unsignedItFormRecordFirst = static_cast<uint64_t>(itFormRecord->first);
        if ((unsignedItFormRecordFirst & 0x00000000ffffffffL) == (unsignedFormId & 0x00000000ffffffffL)) {
            return itFormRecord->first;
        }
    }
    return formId;
}

/**
 * @brief Clear host data by uId.
 * @param uId The caller uId.
 */
void FormDataMgr::ClearHostDataByUId(const int uId)
{
    std::lock_guard<std::recursive_mutex> lock(formHostRecordMutex_);
    std::vector<FormHostRecord>::iterator itHostRecord;
    for (itHostRecord = clientRecords_.begin(); itHostRecord != clientRecords_.end();) {
        if (itHostRecord->GetCallerUid() == uId) {
            itHostRecord->CleanResource();
            itHostRecord = clientRecords_.erase(itHostRecord);
        } else {
            itHostRecord++;
        }
    }
}
/**
 * @brief Get no host temp forms.
 * @param uid The caller uid.
 * @param noHostTempFormsMap no host temp forms.
 * @param foundFormsMap Form Id list.
 */
void FormDataMgr::GetNoHostTempForms(
    const int uid, std::map<FormIdKey,
    std::set<int64_t>> &noHostTempFormsMap,
    std::map<int64_t, bool> &foundFormsMap)
{
    std::lock_guard<std::recursive_mutex> lock(formRecordMutex_);
    std::map<int64_t, FormRecord>::iterator itFormRecord;
    for (itFormRecord = formRecords_.begin(); itFormRecord != formRecords_.end(); itFormRecord++) {
        if (!itFormRecord->second.formTempFlag) {
            continue; // Not temp form, skip
        }

        auto itUid = std::find(itFormRecord->second.formUserUids.begin(),
            itFormRecord->second.formUserUids.end(), uid);
        if (itUid == itFormRecord->second.formUserUids.end()) {
            foundFormsMap.emplace(itFormRecord->second.formId, false);
            continue;
        }

        itFormRecord->second.formUserUids.erase(itUid);
        if (!itFormRecord->second.formUserUids.empty()) {
            continue;
        }

        FormIdKey formIdKey(itFormRecord->second.bundleName, itFormRecord->second.abilityName);
        auto itIdsSet = noHostTempFormsMap.find(formIdKey);
        if (itIdsSet == noHostTempFormsMap.end()) {
            std::set<int64_t> formIdsSet;
            formIdsSet.emplace(itFormRecord->second.formId);
            noHostTempFormsMap.emplace(formIdKey, formIdsSet);
        } else {
            itIdsSet->second.emplace(itFormRecord->second.formId);
        }
    }
}
/**
 * @brief Parse update config.
 * @param record The form record.
 * @param info The form item info.
 */
void FormDataMgr::ParseUpdateConfig(FormRecord &record, const FormItemInfo &info) const
{
    int configDuration = info.GetUpdateDuration();
    if (configDuration > 0) {
        ParseIntervalConfig(record, configDuration);
    } else {
        ParseAtTimerConfig(record, info);
    }
}

/**
 * @brief Parse update interval config.
 * @param record The form record.
 * @param configDuration interval duration.
 */
void FormDataMgr::ParseIntervalConfig(FormRecord &record, const int configDuration) const
{
    HILOG_INFO("%{public}s, configDuration:%{public}d", __func__, configDuration);
    if (configDuration <= Constants::MIN_CONFIG_DURATION) {
        record.updateDuration = Constants::MIN_PERIOD;
    } else if (configDuration >= Constants::MAX_CONFIG_DURATION) {
        record.updateDuration = Constants::MAX_PERIOD;
    } else {
        record.updateDuration = configDuration * Constants::TIME_CONVERSION;
    }
    HILOG_INFO("%{public}s end", __func__);
}

/**
 * @brief Parse at time config.
 * @param record The form record.
 * @param info form item info.
 */
void FormDataMgr::ParseAtTimerConfig(FormRecord &record, const FormItemInfo &info) const
{
    record.isEnableUpdate = false;
    record.updateDuration = 0;
    std::string configAtTime = info.GetScheduledUpdateTime();
    HILOG_INFO("%{public}s, parseAsUpdateAt updateAt:%{public}s", __func__, configAtTime.c_str());
    if (configAtTime.empty()) {
        return;
    }

    std::vector<std::string> temp = FormUtil::StringSplit(configAtTime, Constants::TIME_DELIMETER);
    if (temp.size() != Constants::UPDATE_AT_CONFIG_COUNT) {
        HILOG_ERROR("%{public}s, invalid config", __func__);
        return;
    }
    int hour = -1;
    int min = -1;
    hour = std::stoi(temp[0]);
    min = std::stoi(temp[1]);
    if (hour < Constants::MIN_TIME || hour > Constants::MAX_HOUR || min < Constants::MIN_TIME || min >
        Constants::MAX_MINUTE) {
        HILOG_ERROR("%{public}s, time is invalid", __func__);
        return;
    }
    record.updateAtHour = hour;
    record.updateAtMin = min;
    record.isEnableUpdate = true;
}
/**
 * @brief check if form cached.
 * @param record The form record.
 * @return Returns ERR_OK on cached, others on not cached.
 */
bool FormDataMgr::IsFormCached(const FormRecord record)
{
    if (record.versionUpgrade) {
        return false;
    }
    return FormCacheMgr::GetInstance().IsExist(record.formId);
}

/**
 * @brief Create form state host record.
 * @param provider The provider of the form state
 * @param info The form item info.
 * @param callerToken The UID of the proxy.
 * @param callingUid The UID of the proxy.
 * @return Returns true if this function is successfully called; returns false otherwise.
 */
bool FormDataMgr::CreateFormStateRecord(std::string &provider, const FormItemInfo &info,
                                        const sptr<IRemoteObject> &callerToken, int callingUid)
{
    std::lock_guard<std::recursive_mutex> lock(formStateRecordMutex_);
    auto iter = formStateRecord_.find(provider);
    if (iter != formStateRecord_.end()) {
        if (iter->second.GetFormHostClient() != callerToken) {
            iter->second.SetFormHostClient(callerToken);
        }
        return true;
    }

    FormHostRecord hostRecord;
    bool isCreated = CreateHostRecord(info, callerToken, callingUid, hostRecord);
    if (isCreated) {
        formStateRecord_.emplace(provider, hostRecord);
        return true;
    }

    return false;
}

/**
 * @brief acquire form state callback.
 * @param state form state.
 * @param provider provider info.
 * @param want The want of onAcquireFormState.
 * @return Returns true if this function is successfully called; returns false otherwise.
 */
ErrCode FormDataMgr::AcquireFormStateBack(AppExecFwk::FormState state, const std::string &provider,
                                          const Want &want)
{
    std::lock_guard<std::recursive_mutex> lock(formStateRecordMutex_);
    auto iter = formStateRecord_.find(provider);
    if (iter == formStateRecord_.end()) {
        HILOG_ERROR("filed to get form state host record");
        return ERR_APPEXECFWK_FORM_GET_HOST_FAILED;
    }
    iter->second.OnAcquireState(state, want);
    iter->second.CleanResource();
    formStateRecord_.erase(iter);
    return ERR_OK;
}

/**
 * @brief Notify the form is visible or not.
 * @param formIds Indicates the ID of the forms.
 * @param isVisible Visible or not.
 * @param callerToken Host client.
 * @return Returns ERR_OK on success, others on failure.
 */
ErrCode FormDataMgr::NotifyFormsVisible(const std::vector<int64_t> &formIds, bool isVisible,
                                        const sptr<IRemoteObject> &callerToken)
{
    if (formIds.empty() || callerToken == nullptr) {
        HILOG_ERROR("%{public}s failed, formIds empty.", __func__);
        return ERR_APPEXECFWK_FORM_INVALID_PARAM;
    }

    std::vector<int64_t> foundFormIds {};
    {
        HILOG_INFO("%{public}s, get the matched form host record by client stub.", __func__);
        std::lock_guard<std::recursive_mutex> lock(formHostRecordMutex_);
        for (const FormHostRecord &record : clientRecords_) {
            if (callerToken != record.GetFormHostClient()) {
                continue;
            }
            for (int64_t formId : formIds) {
                int64_t matchedFormId = FormDataMgr::GetInstance().FindMatchedFormId(formId);
                if (!record.Contains(matchedFormId)) {
                    HILOG_ERROR("%{public}s fail, form is not self-owned, form:%{public}" PRId64 ".", __func__,
                        matchedFormId);
                } else {
                    foundFormIds.push_back(matchedFormId);
                }
            }
            break;
        }
    }

    if (foundFormIds.empty()) {
        HILOG_ERROR("%{public}s failed, no valid forms found.", __func__);
        return ERR_APPEXECFWK_FORM_OPERATION_NOT_SELF;
    }

    for (auto matchedFormId : foundFormIds) {
        SetRecordVisible(matchedFormId, isVisible);
    }
    return ERR_OK;
}

/**
 * @brief set form record visible.
 * @param matchedFormId form id.
 * @param isVisible is visible.
 * @return Returns true if this function is successfully called; returns false otherwise.
 */
ErrCode FormDataMgr::SetRecordVisible(int64_t matchedFormId, bool isVisible)
{
    HILOG_INFO("%{public}s, set form record visible", __func__);
    std::lock_guard<std::recursive_mutex> lock(formRecordMutex_);
    auto info = formRecords_.find(matchedFormId);
    if (info == formRecords_.end()) {
        HILOG_ERROR("%{public}s, form info not find", __func__);
        return ERR_APPEXECFWK_FORM_INVALID_FORM_ID;
    }
    info->second.isVisible = isVisible;
    HILOG_DEBUG("set isVisible to %{public}d, formId: %{public}" PRId64 " ", isVisible, matchedFormId);
    return ERR_OK;
}

/**
 * @brief delete forms by userId.
 *
 * @param userId user ID.
 * @param removedFormIds removed userId.
 */
void FormDataMgr::DeleteFormsByUserId(const int32_t userId, std::vector<int64_t> &removedFormIds)
{
    HILOG_INFO("%{public}s,  delete forms by userId", __func__);

    // handle formRecords_
    std::vector<int64_t> removedTempForms;
    {
        std::lock_guard<std::recursive_mutex> lock(formRecordMutex_);
        auto itFormRecord = formRecords_.begin();
        while (itFormRecord != formRecords_.end()) {
            if (userId == itFormRecord->second.userId) {
                if (itFormRecord->second.formTempFlag) {
                    removedTempForms.emplace_back(itFormRecord->second.formId);
                }
                removedFormIds.emplace_back(itFormRecord->second.formId);
                itFormRecord = formRecords_.erase(itFormRecord);
            } else {
                ++itFormRecord;
            }
        }
    }

    // handle tempForms_
    if (removedTempForms.size() > 0) {
        std::lock_guard<std::recursive_mutex> lock(formTempMutex_);
        std::vector<int64_t>::iterator itTemp;
        for (itTemp = tempForms_.begin();itTemp != tempForms_.end();) {
            if (std::find(removedTempForms.begin(), removedTempForms.end(), *itTemp) != removedTempForms.end()) {
                itTemp = tempForms_.erase(itTemp);
            } else {
                itTemp++;
            }
        }
    }
}
/**
 * @brief Clear form records for st limit value test.
 */
void FormDataMgr::ClearFormRecords()
{
    {
        std::lock_guard<std::recursive_mutex> lock(formRecordMutex_);
        formRecords_.clear();
    }
    {
        std::lock_guard<std::recursive_mutex> lock(formTempMutex_);
        tempForms_.clear();
    }
}

/**
 * @brief handle get no host invalid temp forms.
 * @param userId User ID.
 * @param callingUid The UID of the proxy.
 * @param matchedFormIds The set of the valid forms.
 * @param noHostTempFormsMap The map of the no host forms.
 * @param foundFormsMap The map of the found forms.
 */
void FormDataMgr::GetNoHostInvalidTempForms(int32_t userId, int32_t callingUid, std::set<int64_t> &matchedFormIds,
                                            std::map<FormIdKey, std::set<int64_t>> &noHostTempFormsMap,
                                            std::map<int64_t, bool> &foundFormsMap)
{
    std::lock_guard<std::recursive_mutex> lock(formRecordMutex_);
    for (auto &formRecordInfo : formRecords_) {
        int64_t formId = formRecordInfo.first;
        FormRecord &formRecord = formRecordInfo.second;

        // check userID and temp flag
        if (userId != formRecord.userId || !formRecord.formTempFlag) {
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
            auto itIdsSet = noHostTempFormsMap.find(formIdKey);
            if (itIdsSet == noHostTempFormsMap.end()) {
                std::set<int64_t> formIdsSet;
                formIdsSet.emplace(formId);
                noHostTempFormsMap.emplace(formIdKey, formIdsSet);
            } else {
                itIdsSet->second.emplace(formId);
            }
        } else {
            foundFormsMap.emplace(formId, false);
        }
    }
}

/**
 * @brief handle delete no host temp forms.
 * @param callingUid The UID of the proxy.
 * @param noHostTempFormsMap The map of the no host forms.
 * @param foundFormsMap The map of the found forms.
 */
void FormDataMgr::BatchDeleteNoHostTempForms(int32_t callingUid, std::map<FormIdKey,
                                             std::set<int64_t>> &noHostTempFormsMap,
                                             std::map<int64_t, bool> &foundFormsMap)
{
    std::set<FormIdKey> removableModuleSet;
    for (auto &noHostTempForm : noHostTempFormsMap) {
        FormIdKey formIdKey = noHostTempForm.first;
        std::set<int64_t> &formIdsSet = noHostTempForm.second;
        std::string bundleName = formIdKey.bundleName;
        std::string abilityName = formIdKey.abilityName;
        FormProviderMgr::GetInstance().NotifyProviderFormsBatchDelete(bundleName, abilityName, formIdsSet);
        for (int64_t formId: formIdsSet) {
            foundFormsMap.emplace(formId, true);
            StopRenderingForm(formId);
            DeleteFormRecord(formId);
            DeleteTempForm(formId);
        }
    }
}

/**
 * @brief StopRenderingForm.
 * @param formId The form id.
 */
void FormDataMgr::StopRenderingForm(int32_t formId)
{
    FormRecord formrecord;
    GetFormRecord(formId, formrecord);
    FormRenderMgr::GetInstance().StopRenderingForm(formId, formrecord);
}

/**
 * @brief delete invalid temp forms.
 * @param userId User ID.
 * @param callingUid The UID of the proxy.
 * @param matchedFormIds The set of the valid forms.
 * @param removedFormsMap The map of the removed invalid forms.
 * @return Returns ERR_OK on success, others on failure.
 */
int32_t FormDataMgr::DeleteInvalidTempForms(int32_t userId, int32_t callingUid, std::set<int64_t> &matchedFormIds,
                                            std::map<int64_t, bool> &removedFormsMap)
{
    HILOG_INFO("DeleteInvalidTempForms start, userId = %{public}d, callingUid = %{public}d", userId, callingUid);
    std::map<int64_t, bool> foundFormsMap {};
    std::map<FormIdKey, std::set<int64_t>> noHostTempFormsMap {};
    GetNoHostInvalidTempForms(userId, callingUid, matchedFormIds, noHostTempFormsMap, foundFormsMap);
    BatchDeleteNoHostTempForms(callingUid, noHostTempFormsMap, foundFormsMap);
    HILOG_DEBUG("foundFormsMap size: %{public}zu", foundFormsMap.size());
    HILOG_DEBUG("noHostTempFormsMap size: %{public}zu", noHostTempFormsMap.size());

    if (!foundFormsMap.empty()) {
        removedFormsMap.insert(foundFormsMap.begin(), foundFormsMap.end());
    }
    HILOG_INFO("DeleteInvalidTempForms done");
    return ERR_OK;
}

/**
 * @brief delete publish forms temp data
 * @param userId User ID.
 * @param bundleName BundleName.
 * @param validFormIds The set of the valid forms.
 */
void FormDataMgr::DeleteInvalidPublishForms(int32_t userId, std::string bundleName, std::set<int64_t> &validFormIds)
{
    HILOG_INFO("DeleteInvalidPublishForms start, userId = %{public}d, bundleName = %{public}s",
        userId, bundleName.c_str());

    int32_t deleteNum = 0;
    std::lock_guard<std::recursive_mutex> lock(formRequestPublishFormsMutex_);
    for (auto iter = formRequestPublishForms_.begin(); iter != formRequestPublishForms_.end();) {
        int64_t formId = iter->first;
        // check valid form set
        if (validFormIds.find(formId) != validFormIds.end()) {
            ++iter;
            continue;
        }

        Want want = iter->second.first;
        if (bundleName != want.GetStringParam(Constants::PARAM_BUNDLE_NAME_KEY)) {
            ++iter;
            continue;
        }
        if (userId != want.GetIntParam(Constants::PARAM_FORM_USER_ID, -1)) {
            ++iter;
            continue;
        }
        ++deleteNum;
        iter = formRequestPublishForms_.erase(iter);
    }

    HILOG_INFO("DeleteInvalidPublishForms done, delete num is %{public}d", deleteNum);
}

/**
 * @brief clear host data by invalid forms.
 * @param callingUid The UID of the proxy.
 * @param removedFormsMap The map of the removed invalid forms.
 * @return Returns ERR_OK on success, others on failure.
 */
int32_t FormDataMgr::ClearHostDataByInvalidForms(int32_t callingUid, std::map<int64_t, bool> &removedFormsMap)
{
    HILOG_INFO("DeleteInvalidForms host start");
    std::lock_guard<std::recursive_mutex> lock(formHostRecordMutex_);
    std::vector<FormHostRecord>::iterator itHostRecord;
    for (itHostRecord = clientRecords_.begin(); itHostRecord != clientRecords_.end();) {
        if (itHostRecord->GetCallerUid() != callingUid) {
            itHostRecord++;
            continue;
        }
        for (auto &removedForm : removedFormsMap) {
            if (itHostRecord->Contains(removedForm.first)) {
                itHostRecord->DelForm(removedForm.first);
            }
        }
        if (itHostRecord->IsEmpty()) {
            itHostRecord->CleanResource();
            itHostRecord = clientRecords_.erase(itHostRecord);
        } else {
            itHostRecord++;
        }
    }
    HILOG_INFO("DeleteInvalidForms host done");
    return ERR_OK;
}

ErrCode FormDataMgr::AddRequestPublishFormInfo(int64_t formId, const Want &want,
                                               std::unique_ptr<FormProviderData> &formProviderData)
{
    HILOG_INFO("add request publish form info, formId: %{public}" PRId64 "", formId);
    std::lock_guard<std::recursive_mutex> lock(formRequestPublishFormsMutex_);

    auto insertResult = formRequestPublishForms_.insert(
        std::make_pair(formId, std::make_pair(want, std::move(formProviderData))));
    if (!insertResult.second) {
        HILOG_ERROR("Failed to emplace requestPublishFormInfo");
        return ERR_APPEXECFWK_FORM_COMMON_CODE;
    }
    return ERR_OK;
}

ErrCode FormDataMgr::RemoveRequestPublishFormInfo(int64_t formId)
{
    HILOG_INFO("remove request publish form info, formId: %{public}" PRId64 "", formId);
    std::lock_guard<std::recursive_mutex> lock(formRequestPublishFormsMutex_);
    formRequestPublishForms_.erase(formId);
    return ERR_OK;
}

bool FormDataMgr::IsRequestPublishForm(int64_t formId)
{
    std::lock_guard<std::recursive_mutex> lock(formRequestPublishFormsMutex_);
    return formRequestPublishForms_.find(formId) != formRequestPublishForms_.end();
}

ErrCode FormDataMgr::GetRequestPublishFormInfo(int64_t formId, Want &want,
                                               std::unique_ptr<FormProviderData> &formProviderData)
{
    HILOG_INFO("get request publish form info, formId: %{public}" PRId64 "", formId);
    std::lock_guard<std::recursive_mutex> lock(formRequestPublishFormsMutex_);
    auto result = formRequestPublishForms_.find(formId);
    if (result == formRequestPublishForms_.end()) {
        HILOG_INFO("request publish form id not found, formId: %{public}" PRId64 "", formId);
        return ERR_APPEXECFWK_FORM_INVALID_FORM_ID;
    }

    want = result->second.first;
    formProviderData = std::move(result->second.second);
    formRequestPublishForms_.erase(result);
    return ERR_OK;
}

bool FormDataMgr::GetPackageForm(const FormRecord &record, const BundlePackInfo &bundlePackInfo,
    AbilityFormInfo &abilityFormInfo)
{
    HILOG_INFO("%{public}s start, moduleName is %{public}s", __func__, record.moduleName.c_str());
    std::vector<PackageModule> modules = bundlePackInfo.summary.modules;
    for (const auto &cfg : modules) {
        HILOG_INFO("%{public}s, try module %{public}s", __func__, cfg.distro.moduleName.c_str());
        if (record.moduleName != cfg.distro.moduleName) {
            continue;
        }
        HILOG_INFO("%{public}s, has the same module", __func__);
        std::vector<ModuleAbilityInfo> abilities = cfg.abilities;
        std::vector<ExtensionAbilities> extensionAbilities = cfg.extensionAbilities;
        if (!abilities.empty()) {
            return GetAbilityFormInfo(record, abilities, abilityFormInfo);
        }
        if (!extensionAbilities.empty()) {
            return GetAbilityFormInfo(record, extensionAbilities, abilityFormInfo);
        }
        HILOG_WARN("%{public}s, no ability in module:%{public}s", __func__, record.moduleName.c_str());
        return false;
    }
    return false;
}

template<typename T>
bool FormDataMgr::GetAbilityFormInfo(const FormRecord &record, const std::vector<T> &abilities,
    AbilityFormInfo &abilityFormInfo)
{
    for (const T &abilityInfo : abilities) {
        if (abilityInfo.name != record.abilityName) {
            continue;
        }
        std::vector<AbilityFormInfo> forms = abilityInfo.forms;
        for (auto &item : forms) {
            if (IsSameForm(record, item)) {
                abilityFormInfo = item;
                HILOG_INFO("%{public}s find matched abilityFormInfo", __func__);
                return true;
            }
        }
    }
    HILOG_INFO("%{public}s, no matched abilityFormInfo, module is %{public}s", __func__, record.moduleName.c_str());
    return false;
}

bool FormDataMgr::IsSameForm(const FormRecord &record, const AbilityFormInfo &abilityFormInfo)
{
    auto dimensionIter = Constants::DIMENSION_MAP.find(static_cast<Constants::Dimension>(record.specification));
    if (dimensionIter == Constants::DIMENSION_MAP.end()) {
        HILOG_ERROR("%{public}s, specification:%{public}d is invalid", __func__, record.specification);
        return false;
    }
    auto dimension = dimensionIter->second;
    auto supportDimensions = abilityFormInfo.supportDimensions;
    if (record.formName == abilityFormInfo.name &&
        std::find(supportDimensions.begin(), supportDimensions.end(), dimension) != supportDimensions.end()) {
        return true;
    }

    HILOG_INFO("%{public}s, no same form, record is %{public}s, dimension is %{public}s, abilityFormInfo is %{public}s",
        __func__, record.formName.c_str(), dimension.c_str(), abilityFormInfo.name.c_str());

    return false;
}

bool FormDataMgr::SetRecordNeedFreeInstall(int64_t formId, bool isNeedFreeInstall)
{
    HILOG_INFO("%{public}s, get form record by formId", __func__);
    std::lock_guard<std::recursive_mutex> lock(formRecordMutex_);
    auto item = formRecords_.find(formId);
    if (item == formRecords_.end()) {
        HILOG_ERROR("%{public}s, form record not found", __func__);
        return false;
    }
    item->second.needFreeInstall = isNeedFreeInstall;
    HILOG_INFO("%{public}s, set form record need free install flag successfully", __func__);
    return true;
}
}  // namespace AppExecFwk
}  // namespace OHOS
