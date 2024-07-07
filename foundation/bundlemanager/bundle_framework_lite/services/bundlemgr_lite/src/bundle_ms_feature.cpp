/*
 * Copyright (c) 2020 Huawei Device Co., Ltd.
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

#include "bundle_ms_feature.h"

#include "ability_info_utils.h"
#include "appexecfwk_errors.h"
#include "bundle_info_utils.h"
#include "bundle_inner_interface.h"
#include "bundle_manager_service.h"
#include "bundle_message_id.h"
#include "convert_utils.h"
#include "ipc_skeleton.h"
#include "bundle_log.h"
#include "message.h"
#include "ohos_init.h"
#include "samgr_lite.h"
#include "securec.h"
#include "utils.h"
#include "want_utils.h"

namespace OHOS {
#ifdef __LINUX__
constexpr static uint32_t MAX_IPC_STRING_LENGTH = 8192UL;
#endif
static BmsImpl g_bmsImpl = {
    SERVER_IPROXY_IMPL_BEGIN,
    .Invoke = BundleMsFeature::Invoke,
    .QueryAbilityInfo = BundleMsFeature::QueryAbilityInfo,
    .GetBundleInfo = BundleMsFeature::GetBundleInfo,
    .GetBundleInfos = BundleMsFeature::GetBundleInfos,
    .QueryKeepAliveBundleInfos = BundleMsFeature::QueryKeepAliveBundleInfos,
    .GetBundleNameForUid = BundleMsFeature::GetBundleNameForUid,
    .GetBundleSize = BundleMsFeature::GetBundleSize,
    IPROXY_END
};

BundleInvokeType BundleMsFeature::BundleMsInvokeFuc[BMS_INNER_BEGIN] {
    QueryInnerAbilityInfo,
    GetInnerBundleInfo,
    ChangeInnerCallbackServiceId,
    GetInnerBundleNameForUid,
    HandleGetBundleInfos,
    HandleGetBundleInfos,
    HandleGetBundleInfos,
    HasSystemCapability,
    GetInnerBundleSize,
    HandleGetBundleInfosLength,
    HandleGetBundleInfosByIndex,
    GetSystemAvailableCapabilities,
};

IUnknown *GetBmsFeatureApi(Feature *feature)
{
    g_bmsImpl.bundleMsFeature = reinterpret_cast<BundleMsFeature *>(feature);
    return GET_IUNKNOWN(g_bmsImpl);
}

static void Init()
{
    SamgrLite *sm = SAMGR_GetInstance();
    if (sm == nullptr) {
        return;
    }
    sm->RegisterFeature(BMS_SERVICE, reinterpret_cast<Feature *>(BundleMsFeature::GetInstance()));
    sm->RegisterFeatureApi(BMS_SERVICE, BMS_FEATURE,
        GetBmsFeatureApi(reinterpret_cast<Feature *>(BundleMsFeature::GetInstance())));
    HILOG_DEBUG(HILOG_MODULE_APP, "BundleMS feature start success");
}
APP_FEATURE_INIT(Init);

BundleMsFeature::BundleMsFeature() : identity_()
{
    this->Feature::GetName = BundleMsFeature::GetFeatureName;
    this->Feature::OnInitialize = BundleMsFeature::OnFeatureInitialize;
    this->Feature::OnStop = BundleMsFeature::OnFeatureStop;
    this->Feature::OnMessage = BundleMsFeature::OnFeatureMessage;
}

BundleMsFeature::~BundleMsFeature() {}

const char *BundleMsFeature::GetFeatureName(Feature *feature)
{
    (void) feature;
    return BMS_FEATURE;
}

void BundleMsFeature::OnFeatureInitialize(Feature *feature, Service *parent, Identity identity)
{
    if (feature == nullptr) {
        return;
    }
    (reinterpret_cast<BundleMsFeature *>(feature))->identity_ = identity;
    bool ret = GetInstance()->BundleServiceTaskInit();
    HILOG_DEBUG(HILOG_MODULE_APP, "BundleMS feature initialized, result is %d", ret);
}

void BundleMsFeature::OnFeatureStop(Feature *feature, Identity identity)
{
    (void) feature;
    (void) identity;
}

BOOL BundleMsFeature::OnFeatureMessage(Feature *feature, Request *request)
{
    if (feature == nullptr || request == nullptr) {
        return FALSE;
    }
    ManagerService::GetInstance().ServiceMsgProcess(request);
    return TRUE;
}

uint8_t BundleMsFeature::HasSystemCapability(const uint8_t funcId, IpcIo *req, IpcIo *reply)
{
    if ((req == nullptr) || (reply == nullptr)) {
        return ERR_APPEXECFWK_OBJECT_NULL;
    }
    size_t size = 0;
    char *sysCapName = reinterpret_cast<char *>(ReadString(req, &size));
    if (sysCapName == nullptr) {
        return ERR_APPEXECFWK_DESERIALIZATION_FAILED;
    }
    bool hasSysCap = OHOS::ManagerService::GetInstance().HasSystemCapability(sysCapName);
    if (hasSysCap) {
        WriteUint8(reply, static_cast<uint8_t>(OHOS_SUCCESS));
        return OHOS_SUCCESS;
    }
    return ERR_APPEXECFWK_OBJECT_NULL;
}

uint8_t BundleMsFeature::GetSystemAvailableCapabilities(const uint8_t funcId, IpcIo *req, IpcIo *reply)
{
    if ((req == nullptr) || (reply == nullptr)) {
        return ERR_APPEXECFWK_OBJECT_NULL;
    }

    char sysCaps[MAX_SYSCAP_NUM][MAX_SYSCAP_NAME_LEN];
    int32_t len = MAX_SYSCAP_NUM;
    uint8_t errorCode = OHOS::ManagerService::GetInstance().GetSystemAvailableCapabilities(sysCaps, &len);
    if (errorCode != OHOS_SUCCESS) {
        return errorCode;
    }
    WriteUint8(reply, static_cast<uint8_t>(OHOS_SUCCESS));
    WriteInt32(reply, len);
    for (int32_t index = 0; index < len; index++) {
        WriteString(reply, sysCaps[index]);
    }
    return errorCode;
}

uint8_t BundleMsFeature::QueryInnerAbilityInfo(const uint8_t funcId, IpcIo *req, IpcIo *reply)
{
    if ((req == nullptr) || (reply == nullptr)) {
        return ERR_APPEXECFWK_OBJECT_NULL;
    }
    Want want;
    if (memset_s(&want, sizeof(Want), 0, sizeof(Want)) != EOK) {
        HILOG_ERROR(HILOG_MODULE_APP, "BundleMS feature memset want failed");
        return ERR_APPEXECFWK_SYSTEM_INTERNAL_ERROR;
    }
    if (!DeserializeWant(&want, req)) {
        HILOG_ERROR(HILOG_MODULE_APP, "BundleMS feature deserialize failed");
        return ERR_APPEXECFWK_SYSTEM_INTERNAL_ERROR;
    }
    AbilityInfo abilityInfo;
    if (memset_s(&abilityInfo, sizeof(AbilityInfo), 0, sizeof(AbilityInfo)) != EOK) {
        ClearWant(&want);
        HILOG_ERROR(HILOG_MODULE_APP, "BundleMS feature memset ability info failed");
        return ERR_APPEXECFWK_SYSTEM_INTERNAL_ERROR;
    }
    uint8_t errorCode = QueryAbilityInfo(&want, &abilityInfo);
    ClearWant(&want);
    if (errorCode != OHOS_SUCCESS) {
        ClearAbilityInfo(&abilityInfo);
        return errorCode;
    }
    char *str = ConvertUtils::ConvertAbilityInfoToString(&abilityInfo);
    if (str == nullptr) {
        ClearAbilityInfo(&abilityInfo);
        return ERR_APPEXECFWK_SERIALIZATION_FAILED;
    }
#ifdef __LINUX__
    if (strlen(str) > MAX_IPC_STRING_LENGTH) {
        ClearAbilityInfo(&abilityInfo);
        return ERR_APPEXECFWK_SERIALIZATION_FAILED;
    }
#endif
    WriteUint8(reply, static_cast<uint8_t>(OHOS_SUCCESS));
    WriteString(reply, str);
    ClearAbilityInfo(&abilityInfo);
    return OHOS_SUCCESS;
}

uint8_t BundleMsFeature::GetInnerBundleInfo(const uint8_t funcId, IpcIo *req, IpcIo *reply)
{
    if ((req == nullptr) || (reply == nullptr)) {
        return ERR_APPEXECFWK_OBJECT_NULL;
    }
    size_t size = 0;
    char *bundleName = reinterpret_cast<char *>(ReadString(req, &size));
    if (bundleName == nullptr) {
        return ERR_APPEXECFWK_DESERIALIZATION_FAILED;
    }
    BundleInfo bundleInfo;
    if (memset_s(&bundleInfo, sizeof(BundleInfo), 0, sizeof(BundleInfo)) != EOK) {
        return ERR_APPEXECFWK_SYSTEM_INTERNAL_ERROR;
    }
    int32_t flag;
    ReadInt32(req, &flag);
    uint8_t errorCode = GetBundleInfo(bundleName, flag, &bundleInfo);
    if (errorCode != OHOS_SUCCESS) {
        HILOG_ERROR(HILOG_MODULE_APP, "BundleMS GET_BUNDLE_INFO errorcode: %{public}d\n", errorCode);
        return errorCode;
    }
    char *str = ConvertUtils::ConvertBundleInfoToString(&bundleInfo);
    if (str == nullptr) {
        return ERR_APPEXECFWK_SERIALIZATION_FAILED;
    }
#ifdef __LINUX__
    if (strlen(str) > MAX_IPC_STRING_LENGTH) {
        return ERR_APPEXECFWK_SERIALIZATION_FAILED;
    }
#endif
    WriteUint8(reply, static_cast<uint8_t>(OHOS_SUCCESS));
    WriteString(reply, str);
    return OHOS_SUCCESS;
}

uint8_t BundleMsFeature::GetInnerBundleSize(const uint8_t funcId, IpcIo *req, IpcIo *reply)
{
    if ((req == nullptr) || (reply == nullptr)) {
        return ERR_APPEXECFWK_OBJECT_NULL;
    }
    size_t size = 0;
    uint32_t bundleSize = 0;
    char *bundleName = reinterpret_cast<char *>(ReadString(req, &size));
    if (bundleName == nullptr) {
        WriteUint8(reply, static_cast<uint8_t>(OHOS_SUCCESS));
        WriteUint32(reply, bundleSize);
        return OHOS_SUCCESS;
    }
    bundleSize = GetBundleSize(bundleName);
    WriteUint8(reply, static_cast<uint8_t>(OHOS_SUCCESS));
    WriteUint32(reply, bundleSize);
    return OHOS_SUCCESS;
}

uint8_t BundleMsFeature::HandleGetBundleInfos(const uint8_t funcId, IpcIo *req, IpcIo *reply)
{
    if ((req == nullptr) || (reply == nullptr)) {
        return ERR_APPEXECFWK_OBJECT_NULL;
    }
    BundleInfo *bundleInfos = nullptr;
    char *metaDataKey = nullptr;
    int32_t lengthOfBundleInfo = 0;
    uint8_t errorCode = 0;
    size_t length = 0;
    if (funcId == GET_BUNDLE_INFOS) {
        int32_t flag;
        ReadInt32(req, &flag);
        errorCode = GetBundleInfos(flag, &bundleInfos, &lengthOfBundleInfo);
    } else if (funcId == QUERY_KEEPALIVE_BUNDLE_INFOS) {
        errorCode = QueryKeepAliveBundleInfos(&bundleInfos, &lengthOfBundleInfo);
    } else if (funcId == GET_BUNDLE_INFOS_BY_METADATA) {
        metaDataKey = reinterpret_cast<char *>(ReadString(req, &length));
        if (metaDataKey == nullptr) {
            return ERR_APPEXECFWK_DESERIALIZATION_FAILED;
        }
        errorCode = GetBundleInfosByMetaData(metaDataKey, &bundleInfos, &lengthOfBundleInfo);
    } else {
        return ERR_APPEXECFWK_COMMAND_ERROR;
    }
    if (errorCode != OHOS_SUCCESS) {
        BundleInfoUtils::FreeBundleInfos(bundleInfos, lengthOfBundleInfo);
        return errorCode;
    }
    char *strs = ConvertUtils::ConvertBundleInfosToString(&bundleInfos, lengthOfBundleInfo);
    if (strs == nullptr) {
        BundleInfoUtils::FreeBundleInfos(bundleInfos, lengthOfBundleInfo);
        return ERR_APPEXECFWK_SERIALIZATION_FAILED;
    }
#ifdef __LINUX__
    if (strlen(strs) > MAX_IPC_STRING_LENGTH) {
        BundleInfoUtils::FreeBundleInfos(bundleInfos, lengthOfBundleInfo);
        return ERR_APPEXECFWK_SERIALIZATION_FAILED;
    }
#endif
    WriteUint8(reply, static_cast<uint8_t>(OHOS_SUCCESS));
    WriteInt32(reply, lengthOfBundleInfo);
    WriteString(reply, strs);
    BundleInfoUtils::FreeBundleInfos(bundleInfos, lengthOfBundleInfo);
    return OHOS_SUCCESS;
}

uint8_t BundleMsFeature::GetInnerBundleNameForUid(const uint8_t funcId, IpcIo *req, IpcIo *reply)
{
    if ((req == nullptr) || (reply == nullptr)) {
        return ERR_APPEXECFWK_OBJECT_NULL;
    }
    char *bundleName = nullptr;
    int32_t readUid;
    ReadInt32(req, &readUid);
    uint8_t errorCode = GetBundleNameForUid(readUid, &bundleName);
    if (errorCode != OHOS_SUCCESS) {
        AdapterFree(bundleName);
        return errorCode;
    }
    WriteUint8(reply, static_cast<uint8_t>(OHOS_SUCCESS));
    WriteString(reply, bundleName);
    AdapterFree(bundleName);
    return errorCode;
}

uint8_t BundleMsFeature::ChangeInnerCallbackServiceId(const uint8_t funcId, IpcIo *req, IpcIo *reply)
{
    if ((req == nullptr) || (reply == nullptr)) {
        return ERR_APPEXECFWK_OBJECT_NULL;
    }
    bool flag;
    ReadBool(req, &flag);
    SvcIdentity svc;
    if (!(ReadRemoteObject(req, &svc))) {
        return ERR_APPEXECFWK_DESERIALIZATION_FAILED;
    }

    auto svcIdentity = reinterpret_cast<SvcIdentity *>(AdapterMalloc(sizeof(SvcIdentity)));
    if (svcIdentity == nullptr) {
        return ERR_APPEXECFWK_SYSTEM_INTERNAL_ERROR;
    }
    *svcIdentity = svc;
    Request request = {
        .msgId = BUNDLE_CHANGE_CALLBACK,
        .len = static_cast<int16>(sizeof(SvcIdentity)),
        .data = reinterpret_cast<void *>(svcIdentity),
        .msgValue = static_cast<uint32>(flag ? 1 : 0)
    };
    int32 propRet = SAMGR_SendRequest(&(GetInstance()->identity_), &request, nullptr);
    if (propRet != OHOS_SUCCESS) {
        AdapterFree(svcIdentity);
        return ERR_APPEXECFWK_UNINSTALL_FAILED_SEND_REQUEST_ERROR;
    }
    return ERR_OK;
}

int32 BundleMsFeature::Invoke(IServerProxy *iProxy, int funcId, void *origin, IpcIo *req, IpcIo *reply)
{
    if (req == nullptr) {
        return ERR_APPEXECFWK_OBJECT_NULL;
    }
    WriteUint8(reply, static_cast<uint8_t>(funcId));
    uint8_t ret = OHOS_SUCCESS;
    if (funcId >= GET_BUNDLE_INFOS && funcId <= GET_BUNDLE_INFOS_BY_METADATA) {
        ret = BundleMsInvokeFuc[GET_BUNDLE_INFOS](funcId, req, reply);
    } else if (funcId >= QUERY_ABILITY_INFO && funcId <= GET_BUNDLENAME_FOR_UID) {
        ret = BundleMsInvokeFuc[funcId](funcId, req, reply);
    } else if (funcId >= CHECK_SYS_CAP && funcId <= GET_SYS_CAP) {
        ret = BundleMsInvokeFuc[funcId](funcId, req, reply);
    } else {
        ret = ERR_APPEXECFWK_COMMAND_ERROR;
    }

    if (ret != OHOS_SUCCESS) {
        WriteUint8(reply, ret);
    }
    return ret;
}

bool BundleMsFeature::BundleServiceTaskInit()
{
    Request request = {
        .msgId = BUNDLE_SERVICE_INITED,
        .len = 0,
        .data = nullptr,
        .msgValue = 0
    };
    int32 propRet = SAMGR_SendRequest(&identity_, &request, nullptr);
    if (propRet != OHOS_SUCCESS) {
        return false;
    }
    return true;
}

uint8_t BundleMsFeature::QueryAbilityInfo(const Want *want, AbilityInfo *abilityInfo)
{
    if ((abilityInfo == nullptr) || (want == nullptr) || (want->element == nullptr)) {
        return ERR_APPEXECFWK_OBJECT_NULL;
    }

    BundleInfo *bundleInfo = OHOS::ManagerService::GetInstance().QueryBundleInfo(want->element->bundleName);
    if (bundleInfo == nullptr) {
        return ERR_APPEXECFWK_QUERY_NO_INFOS;
    }

    if ((bundleInfo->abilityInfos == nullptr) || (bundleInfo->numOfAbility == 0)) {
        return ERR_APPEXECFWK_QUERY_NO_INFOS;
    }

    for (int32_t i = 0; i < bundleInfo->numOfAbility; ++i) {
        if ((want->element->abilityName != nullptr) && ((bundleInfo->abilityInfos)[i].name != nullptr) &&
            (strcmp(want->element->abilityName, (bundleInfo->abilityInfos)[i].name) == 0)) {
            OHOS::AbilityInfoUtils::CopyAbilityInfo(abilityInfo, bundleInfo->abilityInfos[i]);
            return OHOS_SUCCESS;
        }
    }
    return ERR_APPEXECFWK_QUERY_NO_INFOS;
}

uint8_t BundleMsFeature::GetBundleInfo(const char *bundleName, int32_t flags, BundleInfo *bundleInfo)
{
    return OHOS::ManagerService::GetInstance().GetBundleInfo(bundleName, flags, *bundleInfo);
}

uint8_t BundleMsFeature::GetBundleInfos(const int flags, BundleInfo **bundleInfos, int32_t *len)
{
    return OHOS::ManagerService::GetInstance().GetBundleInfos(flags, bundleInfos, len);
}

uint32_t BundleMsFeature::GetBundleSize(const char *bundleName)
{
    return OHOS::ManagerService::GetInstance().GetBundleSize(bundleName);
}

uint8_t BundleMsFeature::QueryKeepAliveBundleInfos(BundleInfo **bundleInfos, int32_t *len)
{
    if ((bundleInfos == nullptr) || (len == nullptr)) {
        return ERR_APPEXECFWK_OBJECT_NULL;
    }
    BundleInfo *allBundleInfos = nullptr;
    int32_t numOfAllBundleInfos = 0;
    uint8_t errorCode = GetBundleInfos(1, &allBundleInfos, &numOfAllBundleInfos);
    if (errorCode != OHOS_SUCCESS) {
        return errorCode;
    }
    *len = 0;
    for (int32_t i = 0; i < numOfAllBundleInfos; i++) {
        if (allBundleInfos[i].isKeepAlive && allBundleInfos[i].isSystemApp) {
            (*len)++;
        }
    }

    if (*len == 0) {
        BundleInfoUtils::FreeBundleInfos(allBundleInfos, numOfAllBundleInfos);
        return ERR_APPEXECFWK_QUERY_NO_INFOS;
    }
    *bundleInfos = reinterpret_cast<BundleInfo *>(AdapterMalloc(sizeof(BundleInfo) * (*len)));
    if (*bundleInfos == nullptr || memset_s(*bundleInfos, sizeof(BundleInfo) * (*len), 0,
        sizeof(BundleInfo) * (*len)) != EOK) {
        BundleInfoUtils::FreeBundleInfos(allBundleInfos, numOfAllBundleInfos);
        return ERR_APPEXECFWK_QUERY_INFOS_INIT_ERROR;
    }

    int32_t count = 0;
    for (int32_t i = 0; i < numOfAllBundleInfos && count < *len; i++) {
        if (allBundleInfos[i].isKeepAlive && allBundleInfos[i].isSystemApp) {
            OHOS::BundleInfoUtils::CopyBundleInfo(1, *bundleInfos + count, allBundleInfos[i]);
            count++;
        }
    }
    BundleInfoUtils::FreeBundleInfos(allBundleInfos, numOfAllBundleInfos);
    return OHOS_SUCCESS;
}

static bool CheckBundleInfoWithSpecialMetaData(const BundleInfo &bundleInfo, const char *metaDataKey)
{
    if (metaDataKey == nullptr) {
        return false;
    }

    for (int32_t i = 0; i < bundleInfo.numOfModule; i++) {
        for (int32_t j = 0; j < METADATA_SIZE; j++) {
            if ((bundleInfo.moduleInfos[i].metaData[j] != nullptr) &&
                (bundleInfo.moduleInfos[i].metaData[j]->name != nullptr) &&
                strcmp(metaDataKey, bundleInfo.moduleInfos[i].metaData[j]->name) == 0) {
                return true;
            }
        }
    }
    return false;
}

uint8_t BundleMsFeature::GetBundleInfosByMetaData(const char *metaDataKey, BundleInfo **bundleInfos, int32_t *len)
{
    if (metaDataKey == nullptr || bundleInfos == nullptr) {
        return ERR_APPEXECFWK_OBJECT_NULL;
    }

    BundleInfo *allBundleInfos = nullptr;
    int32_t numOfAllBundleInfos = 0;
    uint8_t errorCode = GetBundleInfos(1, &allBundleInfos, &numOfAllBundleInfos);
    if (errorCode != OHOS_SUCCESS) {
        return errorCode;
    }
    *len = 0;
    for (int32_t i = 0; i < numOfAllBundleInfos; i++) {
        if (CheckBundleInfoWithSpecialMetaData(allBundleInfos[i], metaDataKey)) {
            (*len)++;
        }
    }

    if (*len == 0) {
        BundleInfoUtils::FreeBundleInfos(allBundleInfos, numOfAllBundleInfos);
        return ERR_APPEXECFWK_QUERY_NO_INFOS;
    }

    *bundleInfos = reinterpret_cast<BundleInfo *>(AdapterMalloc(sizeof(BundleInfo) * (*len)));
    if (*bundleInfos == nullptr || memset_s(*bundleInfos, sizeof(BundleInfo) * (*len), 0,
        sizeof(BundleInfo) * (*len)) != EOK) {
        BundleInfoUtils::FreeBundleInfos(allBundleInfos, numOfAllBundleInfos);
        return ERR_APPEXECFWK_QUERY_INFOS_INIT_ERROR;
    }

    int32_t count = 0;
    for (int32_t i = 0; i < numOfAllBundleInfos && count < *len; i++) {
        if (CheckBundleInfoWithSpecialMetaData(allBundleInfos[i], metaDataKey)) {
            BundleInfoUtils::CopyBundleInfo(1, *bundleInfos + count, allBundleInfos[i]);
            count++;
        }
    }
    BundleInfoUtils::FreeBundleInfos(allBundleInfos, numOfAllBundleInfos);
    return OHOS_SUCCESS;
}

uint8_t BundleMsFeature::GetBundleNameForUid(int32_t uid, char **bundleName)
{
    if (bundleName == nullptr) {
        return ERR_APPEXECFWK_OBJECT_NULL;
    }
    BundleInfo *infos = nullptr;
    int32_t numOfInfos = 0;
    uint8_t errorCode = GetBundleInfos(0, &infos, &numOfInfos);
    if (errorCode != OHOS_SUCCESS) {
        BundleInfoUtils::FreeBundleInfos(infos, numOfInfos);
        return errorCode;
    }

    for (int i = 0; i < numOfInfos; ++i) {
        if (infos[i].uid == uid) {
            *bundleName = Utils::Strdup(infos[i].bundleName);
            break;
        }
    }
    BundleInfoUtils::FreeBundleInfos(infos, numOfInfos);
    if (*bundleName == nullptr) {
        return ERR_APPEXECFWK_NO_BUNDLENAME_FOR_UID;
    }
    return OHOS_SUCCESS;
}

BundleInfo *BundleMsFeature::GetInnerBundleInfos(IpcIo *req, IpcIo *reply, int32_t *length)
{
    HILOG_INFO(HILOG_MODULE_APP, "BundleMS GetInnerBundleInfos start");
    if ((req == nullptr) || (reply == nullptr)) {
        return nullptr;
    }
    BundleInfo *bundleInfos = nullptr;
    uint8_t errorCode = 0;
    int32_t codeFlag = -1;
    ReadInt32(req, &codeFlag);
    if (codeFlag == GET_BUNDLE_INFOS) {
        int32_t flag;
        ReadInt32(req, &flag);
        errorCode = GetBundleInfos(flag, &bundleInfos, length);
    } else if (codeFlag == QUERY_KEEPALIVE_BUNDLE_INFOS) {
        errorCode = QueryKeepAliveBundleInfos(&bundleInfos, length);
    } else if (codeFlag == GET_BUNDLE_INFOS_BY_METADATA) {
        size_t len = 0;
        char *metaDataKey = reinterpret_cast<char *>(ReadString(req, &len));
        if (metaDataKey == nullptr) {
            return nullptr;
        }
        errorCode = GetBundleInfosByMetaData(metaDataKey, &bundleInfos, length);
    } else {
        return nullptr;
    }
    if (errorCode != OHOS_SUCCESS) {
        HILOG_ERROR(HILOG_MODULE_APP, "BundleMS GetInnerBundleInfos failed with errorcode: %{public}d\n", errorCode);
        BundleInfoUtils::FreeBundleInfos(bundleInfos, *length);
        return nullptr;
    }
    HILOG_DEBUG(HILOG_MODULE_APP, "BundleMS GetInnerBundleInfos with length is: %{public}d\n", *length);
    return bundleInfos;
}

uint8_t BundleMsFeature::HandleGetBundleInfosLength(const uint8_t funcId, IpcIo *req, IpcIo *reply)
{
    HILOG_INFO(HILOG_MODULE_APP, "BundleMS HandleGetBundleInfosLength start");
    if ((req == nullptr) || (reply == nullptr)) {
        return ERR_APPEXECFWK_OBJECT_NULL;
    }
    int32_t lengthOfBundleInfo = 0;
    BundleInfo *bundleInfos = GetInnerBundleInfos(req, reply, &lengthOfBundleInfo);
    if (bundleInfos == nullptr) {
        HILOG_ERROR(HILOG_MODULE_APP, "BundleMS bundleInfos is nullptr");
        return ERR_APPEXECFWK_OBJECT_NULL;
    }
    WriteUint8(reply, static_cast<uint8_t>(OHOS_SUCCESS));
    WriteInt32(reply, lengthOfBundleInfo);
    BundleInfoUtils::FreeBundleInfos(bundleInfos, lengthOfBundleInfo);
    HILOG_INFO(HILOG_MODULE_APP, "BundleMS HandleGetBundleInfosLength finished");
    return OHOS_SUCCESS;
}

uint8_t BundleMsFeature::HandleGetBundleInfosByIndex(const uint8_t funcId, IpcIo *req, IpcIo *reply)
{
    HILOG_INFO(HILOG_MODULE_APP, "BundleMS HandleGetBundleInfosByIndex start");
    if ((req == nullptr) || (reply == nullptr)) {
        return ERR_APPEXECFWK_OBJECT_NULL;
    }
    int32_t lengthOfBundleInfo = 0;
    BundleInfo *bundleInfos = GetInnerBundleInfos(req, reply, &lengthOfBundleInfo);
    int32_t index = 0;
    ReadInt32(req, &index);
    HILOG_INFO(HILOG_MODULE_APP, "BundleMS index is : %{public}d\n", index);
    char *str = ConvertUtils::ConvertBundleInfoToString(bundleInfos + index);
    if (str == nullptr) {
        BundleInfoUtils::FreeBundleInfos(bundleInfos, lengthOfBundleInfo);
        HILOG_ERROR(HILOG_MODULE_APP, "BundleMS HandleGetBundleInfosByIndex strs is nullptr");
        return ERR_APPEXECFWK_SERIALIZATION_FAILED;
    }
    HILOG_DEBUG(HILOG_MODULE_APP, "BundleMS length of str is: %{public}d\n", static_cast<int32_t>(strlen(str)));
#ifdef __LINUX__
    if (strlen(str) > MAX_IPC_STRING_LENGTH) {
        BundleInfoUtils::FreeBundleInfos(bundleInfos, lengthOfBundleInfo);
        HILOG_ERROR(HILOG_MODULE_APP, "BundleMS HandleGetBundleInfosByIndex is too larger to be transformed by ipc");
        cJSON_free(str);
        return ERR_APPEXECFWK_SERIALIZATION_FAILED;
    }
#endif
    WriteUint8(reply, static_cast<uint8_t>(OHOS_SUCCESS));
    WriteString(reply, str);
    HILOG_INFO(HILOG_MODULE_APP, "BundleMS bundleInfo length is %{public}d of index %{public}d",
        static_cast<int32_t>(strlen(str)), index);
    BundleInfoUtils::FreeBundleInfos(bundleInfos, lengthOfBundleInfo);

    cJSON_free(str);
    HILOG_INFO(HILOG_MODULE_APP, "BundleMS HandleGetBundleInfosByIndex finished");
    return OHOS_SUCCESS;
}
} // namespace OHOS
