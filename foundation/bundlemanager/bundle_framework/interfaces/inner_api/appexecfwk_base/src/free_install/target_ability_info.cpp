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

#include "target_ability_info.h"

#include "app_log_wrapper.h"
#include "bundle_constants.h"
#include "json_util.h"
#include "nlohmann/json.hpp"
#include "parcel_macro.h"
#include "string_ex.h"

namespace OHOS {
namespace AppExecFwk {
namespace {
const std::string JSON_KEY_VERSION = "version";
const std::string JSON_KEY_TARGETINFO = "targetInfo";
const std::string JSON_KEY_TARGETEXTSETTING = "targetExtSetting";
const std::string JSON_KEY_EXTINFO = "extInfo";
const std::string JSON_KEY_TRANSACTID = "transactId";
const std::string JSON_KEY_FLAGS = "flags";
const std::string JSON_KEY_REASONFLAG = "reasonFlag";
const std::string JSON_KEY_CALLINGUID = "callingUid";
const std::string JSON_KEY_CALLINGAPPTYPE = "callingAppType";
const std::string JSON_KEY_CALLINGBUNDLENAMES = "callingBundleNames";
const std::string JSON_KEY_CALLINGAPPIDS = "callingAppIds";
const std::string JSON_KEY_PRELOAD_MODULE_NAMES = "preloadModuleNames";
}  // namespace

void to_json(nlohmann::json &jsonObject, const TargetExtSetting &targetExtSetting)
{
    jsonObject = nlohmann::json {
        {JSON_KEY_EXTINFO, targetExtSetting.extValues},
    };
}

void from_json(const nlohmann::json &jsonObject, TargetExtSetting &targetExtSetting)
{
    const auto &jsonObjectEnd = jsonObject.end();
    int32_t parseResult = ERR_OK;
    GetValueIfFindKey<std::map<std::string, std::string>>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_EXTINFO,
        targetExtSetting.extValues,
        JsonType::OBJECT,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    if (parseResult != ERR_OK) {
        APP_LOGE("read module targetExtSetting from jsonObject error, error code : %{public}d", parseResult);
    }
}

bool TargetExtSetting::ReadFromParcel(Parcel &parcel)
{
    int32_t extValueSize;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, extValueSize);
    CONTAINER_SECURITY_VERIFY(parcel, extValueSize, &extValues);
    for (int32_t i = 0; i < extValueSize; ++i) {
        std::string key = Str16ToStr8(parcel.ReadString16());
        std::string value = Str16ToStr8(parcel.ReadString16());
        extValues.emplace(key, value);
    }
    return true;
}

bool TargetExtSetting::Marshalling(Parcel &parcel) const
{
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, static_cast<int32_t>(extValues.size()));
    for (auto& extValue : extValues) {
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(extValue.first));
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(extValue.second));
    }
    return true;
}

TargetExtSetting *TargetExtSetting::Unmarshalling(Parcel &parcel)
{
    TargetExtSetting *targetExtSettingInfo = new (std::nothrow) TargetExtSetting();
    if (targetExtSettingInfo && !targetExtSettingInfo->ReadFromParcel(parcel)) {
        APP_LOGE("read from parcel failed");
        delete targetExtSettingInfo;
        targetExtSettingInfo = nullptr;
    }
    return targetExtSettingInfo;
}

void to_json(nlohmann::json &jsonObject, const TargetInfo &targetInfo)
{
    jsonObject = nlohmann::json {
        {JSON_KEY_TRANSACTID, targetInfo.transactId},
        {Constants::BUNDLE_NAME, targetInfo.bundleName},
        {Constants::MODULE_NAME, targetInfo.moduleName},
        {Constants::ABILITY_NAME, targetInfo.abilityName},
        {JSON_KEY_FLAGS, targetInfo.flags},
        {JSON_KEY_REASONFLAG, targetInfo.reasonFlag},
        {JSON_KEY_CALLINGUID, targetInfo.callingUid},
        {JSON_KEY_CALLINGAPPTYPE, targetInfo.callingAppType},
        {JSON_KEY_CALLINGBUNDLENAMES, targetInfo.callingBundleNames},
        {JSON_KEY_CALLINGAPPIDS, targetInfo.callingAppIds},
        {JSON_KEY_PRELOAD_MODULE_NAMES, targetInfo.preloadModuleNames}
    };
}

void from_json(const nlohmann::json &jsonObject, TargetInfo &targetInfo)
{
    const auto &jsonObjectEnd = jsonObject.end();
    int32_t parseResult = ERR_OK;
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_TRANSACTID,
        targetInfo.transactId,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        Constants::BUNDLE_NAME,
        targetInfo.bundleName,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        Constants::MODULE_NAME,
        targetInfo.moduleName,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        Constants::ABILITY_NAME,
        targetInfo.abilityName,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::uint32_t>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_FLAGS,
        targetInfo.flags,
        JsonType::NUMBER,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::uint32_t>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_REASONFLAG,
        targetInfo.reasonFlag,
        JsonType::NUMBER,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::uint32_t>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_CALLINGUID,
        targetInfo.callingUid,
        JsonType::NUMBER,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::uint32_t>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_CALLINGAPPTYPE,
        targetInfo.callingAppType,
        JsonType::NUMBER,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<std::vector<std::string>>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_CALLINGBUNDLENAMES,
        targetInfo.callingBundleNames,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::STRING);
    GetValueIfFindKey<std::vector<std::string>>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_CALLINGAPPIDS,
        targetInfo.callingAppIds,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::STRING);
    GetValueIfFindKey<std::vector<std::string>>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_PRELOAD_MODULE_NAMES,
        targetInfo.preloadModuleNames,
        JsonType::ARRAY,
        false,
        parseResult,
        ArrayType::STRING);
    if (parseResult != ERR_OK) {
        APP_LOGE("read module targetInfo from jsonObject error, error code : %{public}d", parseResult);
    }
}

bool TargetInfo::ReadFromParcel(Parcel &parcel)
{
    transactId = Str16ToStr8(parcel.ReadString16());
    bundleName = Str16ToStr8(parcel.ReadString16());
    moduleName = Str16ToStr8(parcel.ReadString16());
    abilityName = Str16ToStr8(parcel.ReadString16());
    flags = parcel.ReadInt32();
    reasonFlag = parcel.ReadInt32();
    callingUid = parcel.ReadInt32();
    callingAppType = parcel.ReadInt32();
    int32_t callingBundleNamesSize = 0;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, callingBundleNamesSize);
    CONTAINER_SECURITY_VERIFY(parcel, callingBundleNamesSize, &callingBundleNames);
    for (int32_t i = 0; i < callingBundleNamesSize; i++) {
        callingBundleNames.emplace_back(Str16ToStr8(parcel.ReadString16()));
    }
    int32_t callingAppIdsSize = 0;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, callingAppIdsSize);
    CONTAINER_SECURITY_VERIFY(parcel, callingAppIdsSize, &callingAppIds);
    for (int32_t i = 0; i < callingAppIdsSize; i++) {
        callingAppIds.emplace_back(Str16ToStr8(parcel.ReadString16()));
    }
    int32_t preloadModuleNamesSize = 0;
    READ_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, preloadModuleNamesSize);
    CONTAINER_SECURITY_VERIFY(parcel, preloadModuleNamesSize, &preloadModuleNames);
    for (int32_t i = 0; i < preloadModuleNamesSize; i++) {
        preloadModuleNames.emplace_back(Str16ToStr8(parcel.ReadString16()));
    }
    return true;
}

bool TargetInfo::Marshalling(Parcel &parcel) const
{
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(transactId));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(bundleName));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(moduleName));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(abilityName));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, flags);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, reasonFlag);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, callingUid);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, callingAppType);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, callingBundleNames.size());
    for (auto &callingBundleName : callingBundleNames) {
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(callingBundleName));
    }
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, callingAppIds.size());
    for (auto &callingAppId : callingAppIds) {
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(callingAppId));
    }
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, parcel, preloadModuleNames.size());
    for (auto &preloadItem : preloadModuleNames) {
        WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(preloadItem));
    }
    return true;
}

TargetInfo *TargetInfo::Unmarshalling(Parcel &parcel)
{
    TargetInfo *targetInfo = new (std::nothrow) TargetInfo();
    if (targetInfo && !targetInfo->ReadFromParcel(parcel)) {
        APP_LOGE("read from parcel failed");
        delete targetInfo;
        targetInfo = nullptr;
    }
    return targetInfo;
}

void to_json(nlohmann::json &jsonObject, const TargetAbilityInfo &targetAbilityInfo)
{
    jsonObject = nlohmann::json {
        {JSON_KEY_VERSION, targetAbilityInfo.version},
        {JSON_KEY_TARGETINFO, targetAbilityInfo.targetInfo},
        {JSON_KEY_TARGETEXTSETTING, targetAbilityInfo.targetExtSetting},
    };
}

void from_json(const nlohmann::json &jsonObject, TargetAbilityInfo &targetAbilityInfo)
{
    const auto &jsonObjectEnd = jsonObject.end();
    int32_t parseResult = ERR_OK;
    GetValueIfFindKey<std::string>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_VERSION,
        targetAbilityInfo.version,
        JsonType::STRING,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<TargetInfo>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_TARGETINFO,
        targetAbilityInfo.targetInfo,
        JsonType::OBJECT,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    GetValueIfFindKey<TargetExtSetting>(jsonObject,
        jsonObjectEnd,
        JSON_KEY_TARGETEXTSETTING,
        targetAbilityInfo.targetExtSetting,
        JsonType::OBJECT,
        false,
        parseResult,
        ArrayType::NOT_ARRAY);
    if (parseResult != ERR_OK) {
        APP_LOGE("read module targetAbilityInfo from jsonObject error, error code : %{public}d", parseResult);
    }
}

bool TargetAbilityInfo::ReadFromParcel(Parcel &parcel)
{
    version = Str16ToStr8(parcel.ReadString16());
    auto params = parcel.ReadParcelable<TargetInfo>();
    if (params != nullptr) {
        targetInfo = *params;
        delete params;
        params = nullptr;
    } else {
        return false;
    }
    auto extSetting = parcel.ReadParcelable<TargetExtSetting>();
    if (extSetting != nullptr) {
        targetExtSetting = *extSetting;
        delete extSetting;
        extSetting = nullptr;
    } else {
        return false;
    }
    return true;
}

bool TargetAbilityInfo::Marshalling(Parcel &parcel) const
{
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(String16, parcel, Str8ToStr16(version));
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Parcelable, parcel, &targetInfo);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Parcelable, parcel, &targetExtSetting);
    return true;
}

TargetAbilityInfo *TargetAbilityInfo::Unmarshalling(Parcel &parcel)
{
    TargetAbilityInfo *targetAbilityInfo = new (std::nothrow) TargetAbilityInfo();
    if (targetAbilityInfo && !targetAbilityInfo->ReadFromParcel(parcel)) {
        APP_LOGE("read from parcel failed");
        delete targetAbilityInfo;
        targetAbilityInfo = nullptr;
    }
    return targetAbilityInfo;
}
}  //  namespace AppExecFwk
}  //  namespace OHOS