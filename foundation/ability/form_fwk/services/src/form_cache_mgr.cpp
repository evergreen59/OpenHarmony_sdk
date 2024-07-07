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

#include "hilog_wrapper.h"
#include "form_cache_mgr.h"

namespace OHOS {
namespace AppExecFwk {
FormCacheMgr::FormCacheMgr()
{
    HILOG_INFO("create form cache manager instance");
}
FormCacheMgr::~FormCacheMgr()
{
    HILOG_INFO("destroy form cache manager instance");
}

bool FormCacheMgr::GetData(const int64_t formId, std::string &data) const
{
    HILOG_INFO("get cache data");
    std::lock_guard<std::mutex> lock(cacheMutex_);
    if (cacheData_.empty()) {
        HILOG_ERROR("form cache is empty");
        return false;
    }
    auto formData = cacheData_.find(formId);
    if (formData != cacheData_.end()) {
        data = formData->second;
    }

    if (data.empty()) {
        HILOG_ERROR("form cache not find");
        return false;
    } else {
        return true;
    }
}

/**
 * @brief Add form data.
 * @param formId Form id.
 * @param data Cache data.
 * @return Returns true if this function is successfully called; returns false otherwise.
 */
bool FormCacheMgr::AddData(const int64_t formId, const std::string &data)
{
    std::lock_guard<std::mutex> lock(cacheMutex_);
    auto formData = cacheData_.find(formId);
    if (formData == cacheData_.end()) {
        HILOG_INFO("add new cache data.");
        auto result = cacheData_.emplace(formId, data);
        if (!result.second) {
            HILOG_ERROR("Failed to emplace cache data.");
            return false;
        }
    } else {
        HILOG_INFO("update cache data.");
        formData->second = data;
    }
    return true;
}

/**
 * @brief Delete form data.
 * @param formId, Form id.
 * @return Returns true if this function is successfully called; returns false otherwise.
 */
bool FormCacheMgr::DeleteData(const int64_t formId)
{
    HILOG_INFO("delete cache data");
    std::lock_guard<std::mutex> lock(cacheMutex_);
    auto formData = cacheData_.find(formId);
    if (formData != cacheData_.end()) {
        cacheData_.erase(formId);
    } else {
        HILOG_WARN("cache data is not exist");
    }
    return true;
}

/**
 * @brief Update form data.
 * @param formId, Form id.
 * @param data, Cache data.
 * @return Returns true if this function is successfully called; returns false otherwise.
 */
bool FormCacheMgr::UpdateData(const int64_t formId, const std::string &data)
{
    HILOG_INFO("update cache data");
    std::lock_guard<std::mutex> lock(cacheMutex_);
    auto formData = cacheData_.find(formId);
    if (formData == cacheData_.end()) {
        HILOG_ERROR("cache data is not exist");
        return false;
    }

    formData->second = data;
    return true;
}

/**
 * @brief Check if form data is exist or not.
 * @param formId, Form id.
 * @return Returns true if this function is successfully called; returns false otherwise.
 */
bool FormCacheMgr::IsExist(const int64_t formId) const
{
    HILOG_INFO("get cache data");
    std::lock_guard<std::mutex> lock(cacheMutex_);
    if (cacheData_.empty()) {
        HILOG_ERROR("form cache is empty");
        return false;
    }
    auto formData = cacheData_.find(formId);
    if (formData == cacheData_.end()) {
        HILOG_ERROR("cache data not find");
        return false;
    }

    return true;
}
}  // namespace AppExecFwk
}  // namespace OHOS
