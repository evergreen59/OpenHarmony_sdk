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
#include "bundle_manager.h"

#include "ability_info_utils.h"
#include "adapter.h"
#include "bundle_callback.h"
#include "bundle_callback_utils.h"
#include "bundle_info_utils.h"
#include "bundle_inner_interface.h"
#include "bundle_self_callback.h"
#include "convert_utils.h"
#include "iproxy_client.h"
#include "ipc_skeleton.h"
#include "bundle_log.h"
#include "ohos_types.h"
#include "pms_interface.h"
#include "samgr_lite.h"
#include "securec.h"
#include "want_utils.h"

extern "C" {
constexpr static char PERMISSION_INSTALL_BUNDLE[] = "ohos.permission.INSTALL_BUNDLE";
constexpr static char PERMISSION_GET_BUNDLE_INFO[] = "ohos.permission.GET_BUNDLE_INFO";
constexpr static char PERMISSION_LISTEN_BUNDLE_CHANGE[] = "ohos.permission.LISTEN_BUNDLE_CHANGE";
constexpr static uint8_t MAX_BUNDLE_NAME = 128;
constexpr static uint8_t OBJECT_NUMBER_IN_WANT = 2;
#ifdef __LINUX__
constexpr static uint8_t OBJECT_NUMBER_IN_INSTALLATION = 1;
#else
constexpr static uint8_t OBJECT_NUMBER_IN_INSTALLATION = 2;
#endif

int32_t RegisterCallback(BundleStatusCallback *bundleStatusCallback)
{
    if ((bundleStatusCallback == nullptr) || (bundleStatusCallback->callBack == nullptr)) {
        return ERR_APPEXECFWK_OBJECT_NULL;
    }
    if (CheckSelfPermission(static_cast<const char *>(PERMISSION_LISTEN_BUNDLE_CHANGE)) != GRANTED) {
        HILOG_ERROR(HILOG_MODULE_APP, "BundleManager register callback failed due to permission denied");
        return ERR_APPEXECFWK_PERMISSION_DENIED;
    }

    return OHOS::BundleCallback::GetInstance().RegisterBundleStateCallback(bundleStatusCallback->callBack,
        bundleStatusCallback->bundleName, bundleStatusCallback->data);
}

int32_t UnregisterCallback()
{
    return OHOS::BundleCallback::GetInstance().UnregisterBundleStateCallback();
}

static uint8_t DeserializeInnerAbilityInfo(IOwner owner, IpcIo *reply)
{
    if ((reply == nullptr) || (owner == nullptr)) {
        return OHOS_FAILURE;
    }
    uint8_t resultCode;
    ReadUint8(reply, &resultCode);
    ResultOfQueryAbilityInfo *info = reinterpret_cast<ResultOfQueryAbilityInfo *>(owner);
    if (resultCode != ERR_OK) {
        info->resultCode = resultCode;
        return resultCode;
    }
    size_t len = 0;
    char *jsonStr = reinterpret_cast<char *>(ReadString(reply, &len));
    if (jsonStr == nullptr) {
        info->resultCode = ERR_APPEXECFWK_DESERIALIZATION_FAILED;
        HILOG_ERROR(HILOG_MODULE_APP, "AbilityInfo DeserializeAbilityInfo buff is empty!");
        return ERR_APPEXECFWK_DESERIALIZATION_FAILED;
    }
    info->abilityInfo = OHOS::ConvertUtils::ConvertStringToAbilityInfo(jsonStr, len);
    if (info->abilityInfo == nullptr) {
        info->resultCode = ERR_APPEXECFWK_DESERIALIZATION_FAILED;
        return ERR_APPEXECFWK_DESERIALIZATION_FAILED;
    }
    info->resultCode = resultCode;
    return resultCode;
}

static uint8_t DeserializeInnerBundleInfo(IOwner owner, IpcIo *reply)
{
    HILOG_DEBUG(HILOG_MODULE_APP, "DeserializeInnerBundleInfo start");
    if ((reply == nullptr) || (owner == nullptr)) {
        return OHOS_FAILURE;
    }
    uint8_t resultCode;
    ReadUint8(reply, &resultCode);
    ResultOfGetBundleInfo *info = reinterpret_cast<ResultOfGetBundleInfo *>(owner);
    if (resultCode != ERR_OK) {
        info->resultCode = resultCode;
        return resultCode;
    }
    size_t len = 0;
    char *jsonStr = reinterpret_cast<char *>(ReadString(reply, &len));
    if (jsonStr == nullptr) {
        info->resultCode = ERR_APPEXECFWK_DESERIALIZATION_FAILED;
        HILOG_ERROR(HILOG_MODULE_APP, "BundleInfo DeserializeBundleInfo buff is empty!");
        return ERR_APPEXECFWK_DESERIALIZATION_FAILED;
    }
    info->bundleInfo = OHOS::ConvertUtils::ConvertStringToBundleInfo(jsonStr, len);
    if (info->bundleInfo == nullptr) {
        info->resultCode = ERR_APPEXECFWK_DESERIALIZATION_FAILED;
        HILOG_ERROR(HILOG_MODULE_APP, "ConvertStringToBundleInfo failed");
        return ERR_APPEXECFWK_DESERIALIZATION_FAILED;
    }
    info->resultCode = resultCode;
    HILOG_DEBUG(HILOG_MODULE_APP, "DeserializeInnerBundleInfo finished");
    return resultCode;
}

static uint8_t DeserializeInnerBundleInfos(IOwner owner, IpcIo *reply)
{
    HILOG_DEBUG(HILOG_MODULE_APP, "DeserializeInnerBundleInfos start");
    if ((reply == nullptr) || (owner == nullptr)) {
        return OHOS_FAILURE;
    }
    uint8_t resultCode;
    ReadUint8(reply, &resultCode);
    ResultOfGetBundleInfos *info = reinterpret_cast<ResultOfGetBundleInfos *>(owner);
    if (resultCode != ERR_OK) {
        info->resultCode = resultCode;
        return resultCode;
    }

    ReadInt32(reply, &(info->length));
    size_t len = 0;
    char *jsonStr = reinterpret_cast<char*>(ReadString(reply, &len));
    if (jsonStr == nullptr) {
        info->resultCode = ERR_APPEXECFWK_DESERIALIZATION_FAILED;
        HILOG_ERROR(HILOG_MODULE_APP, "BundleInfo DeserializeBundleInfos buff is empty!");
        return ERR_APPEXECFWK_DESERIALIZATION_FAILED;
    }
    if (!OHOS::ConvertUtils::ConvertStringToBundleInfos(jsonStr, &(info->bundleInfo), info->length, len)) {
        info->resultCode = ERR_APPEXECFWK_DESERIALIZATION_FAILED;
        return ERR_APPEXECFWK_DESERIALIZATION_FAILED;
    }

    info->resultCode = resultCode;
    HILOG_DEBUG(HILOG_MODULE_APP, "DeserializeInnerBundleInfos finished");
    return resultCode;
}

static uint8_t DeserializeInnerBundleName(IOwner owner, IpcIo *reply)
{
    if ((reply == nullptr) || (owner == nullptr)) {
        return OHOS_FAILURE;
    }
    uint8_t resultCode;
    ReadUint8(reply, &resultCode);
    ResultOfGetBundleNameForUid *info = reinterpret_cast<ResultOfGetBundleNameForUid *>(owner);
    if (resultCode != ERR_OK) {
        info->resultCode = resultCode;
        return resultCode;
    }

    size_t length = 0;
    char *bundleName = reinterpret_cast<char *>(ReadString(reply, &length));
    if (bundleName == nullptr) {
        info->resultCode = ERR_APPEXECFWK_DESERIALIZATION_FAILED;
        return ERR_APPEXECFWK_DESERIALIZATION_FAILED;
    }
    if (length > MAX_BUNDLE_NAME) {
        info->resultCode = ERR_APPEXECFWK_DESERIALIZATION_FAILED;
        return ERR_APPEXECFWK_DESERIALIZATION_FAILED;
    }
    info->bundleName = reinterpret_cast<char *>(AdapterMalloc(length + 1));
    if (info->bundleName == nullptr) {
        HILOG_ERROR(HILOG_MODULE_APP, "BundleManager DeserializeInnerBundleName failed");
        info->resultCode = ERR_APPEXECFWK_OBJECT_NULL;
        return ERR_APPEXECFWK_OBJECT_NULL;
    }
    errno_t err = strncpy_s(info->bundleName, length + 1, bundleName, length);
    if (err != EOK) {
        AdapterFree(info->bundleName);
        info->resultCode = ERR_APPEXECFWK_SYSTEM_INTERNAL_ERROR;
        return ERR_APPEXECFWK_SYSTEM_INTERNAL_ERROR;
    }
    info->length = length;
    info->resultCode = resultCode;
    return resultCode;
}

static uint8_t DeserializeBundleSize(IOwner owner, IpcIo *reply)
{
    if ((reply == nullptr) || (owner == nullptr)) {
        return OHOS_FAILURE;
    }
    uint8_t resultCode;
    ReadUint8(reply, &resultCode);
    ResultOfGetBundleSize *info = reinterpret_cast<ResultOfGetBundleSize *>(owner);
    if (resultCode != ERR_OK) {
        info->resultCode = resultCode;
        return resultCode;
    }
    ReadUint32(reply, &(info->bundleSize));
    info->resultCode = resultCode;
    return resultCode;
}

static uint8_t DeserializeSystemCapabilities(IOwner owner, IpcIo *reply)
{
    if ((reply == nullptr) || (owner == nullptr)) {
        return OHOS_FAILURE;
    }
    uint8_t resultCode;
    ReadUint8(reply, &resultCode);
    ResultOfGetSysCap *info = reinterpret_cast<ResultOfGetSysCap *>(owner);
    if (resultCode != ERR_OK) {
        info->resultCode = resultCode;
        return resultCode;
    }
    int32_t sysCapCount;
    ReadInt32(reply, &sysCapCount);
    info->systemCap.systemCapName = reinterpret_cast<SystemCapName *>(AdapterMalloc(sizeof(SystemCapName) *
        sysCapCount));
    if (info->systemCap.systemCapName == nullptr) {
        HILOG_ERROR(HILOG_MODULE_APP, "BundleManager DeserializeSystemCapabilities failed");
        info->resultCode = ERR_APPEXECFWK_SYSTEM_INTERNAL_ERROR;
        return ERR_APPEXECFWK_OBJECT_NULL;
    }
    if (memset_s(info->systemCap.systemCapName, sizeof(SystemCapName) * sysCapCount,
        0, sizeof(SystemCapName) * sysCapCount)) {
        HILOG_ERROR(HILOG_MODULE_APP, "BundleManager DeserializeSystemCapabilities failed");
        AdapterFree(info->systemCap.systemCapName);
        info->resultCode = ERR_APPEXECFWK_SYSTEM_INTERNAL_ERROR;
        return ERR_APPEXECFWK_OBJECT_NULL;
    }
    for (int32_t index = 0; index < sysCapCount; index++) {
        size_t sysCapNameLen;
        char *sysCapName = reinterpret_cast<char *>(ReadString(reply, &sysCapNameLen));
        errno_t err = strncpy_s(info->systemCap.systemCapName[index].name, MAX_SYSCAP_NAME_LEN,
            sysCapName, sysCapNameLen);
        if (err != EOK) {
            AdapterFree(info->systemCap.systemCapName);
            info->resultCode = ERR_APPEXECFWK_SYSTEM_INTERNAL_ERROR;
            return ERR_APPEXECFWK_SYSTEM_INTERNAL_ERROR;
        }
    }
    info->systemCap.systemCapNum = sysCapCount;
    info->resultCode = resultCode;
    return resultCode;
}

static int Notify(IOwner owner, int code, IpcIo *reply)
{
    if ((reply == nullptr) || (owner == nullptr)) {
        HILOG_ERROR(HILOG_MODULE_APP, "BundleManager Notify ipc is nullptr");
        return OHOS_FAILURE;
    }
    uint8_t resultCode;
    ReadUint8(reply, &resultCode);
    switch (resultCode) {
        case INSTALL:
        case UNINSTALL: {
            uint8_t *ret = reinterpret_cast<uint8_t *>(owner);
            ReadUint8(reply, ret);
            HILOG_INFO(HILOG_MODULE_APP, "BundleManager install or uninstall invoke return: %{public}d", *ret);
            break;
        }
        case QUERY_ABILITY_INFO: {
            return DeserializeInnerAbilityInfo(owner, reply);
        }
        case GET_BUNDLE_INFO: {
            return DeserializeInnerBundleInfo(owner, reply);
        }
        case GET_BUNDLE_INFOS:
        case QUERY_KEEPALIVE_BUNDLE_INFOS:
        case GET_BUNDLE_INFOS_BY_METADATA: {
            return DeserializeInnerBundleInfos(owner, reply);
        }
        case GET_BUNDLE_INFO_LENGTH: {
            ResultOfGetBundleInfos *resultOfGetBundleInfos = reinterpret_cast<ResultOfGetBundleInfos *>(owner);
            uint8_t errCode;
            ReadUint8(reply, &errCode);
            if (errCode != ERR_OK) {
                HILOG_INFO(HILOG_MODULE_APP, "BundleManager get bundleInfos length failed due to %{public}d", errCode);
                resultOfGetBundleInfos->resultCode = errCode;
                return errCode;
            }
            ReadInt32(reply, &(resultOfGetBundleInfos->length));
            HILOG_INFO(HILOG_MODULE_APP, "BundleManager bundleInfo len is: %{public}d", resultOfGetBundleInfos->length);
            break;
        }
        case GET_BUNDLE_INFO_BY_INDEX: {
            return DeserializeInnerBundleInfo(owner, reply);
        }
        case GET_BUNDLENAME_FOR_UID: {
            return DeserializeInnerBundleName(owner, reply);
        }
        case CHECK_SYS_CAP: {
            uint8_t *ret = reinterpret_cast<uint8_t *>(owner);
            ReadUint8(reply, ret);
            HILOG_INFO(HILOG_MODULE_APP, "BundleManager HasSystemCapability invoke return: %{public}d", *ret);
            break;
        }
        case GET_BUNDLE_SIZE: {
            return DeserializeBundleSize(owner, reply);
        }
        case GET_SYS_CAP: {
            return DeserializeSystemCapabilities(owner, reply);
        }
#ifdef OHOS_DEBUG
        case SET_EXTERNAL_INSTALL_MODE:
        case SET_SIGN_DEBUG_MODE:
        case SET_SIGN_MODE: {
            uint8_t *ret = reinterpret_cast<uint8_t *>(owner);
            ReadUint8(reply, ret);
            break;
        }
#endif
        default: {
            break;
        }
    }
    return ERR_OK;
}

static IClientProxy *GetBmsClient()
{
    IClientProxy *bmsClient = nullptr;
    IUnknown *iUnknown = SAMGR_GetInstance()->GetFeatureApi(BMS_SERVICE, BMS_FEATURE);
    if (iUnknown == nullptr) {
        return nullptr;
    }
    int result = iUnknown->QueryInterface(iUnknown, CLIENT_PROXY_VER, reinterpret_cast<void **>(&bmsClient));
    if (result != 0) {
        return nullptr;
    }

    return bmsClient;
}

static IClientProxy *GetBmsInnerClient()
{
    IClientProxy *bmsClient = nullptr;
    IUnknown *iUnknown = SAMGR_GetInstance()->GetFeatureApi(BMS_SERVICE, BMS_INNER_FEATURE);
    if (iUnknown == nullptr) {
        return nullptr;
    }
    int result = iUnknown->QueryInterface(iUnknown, CLIENT_PROXY_VER, reinterpret_cast<void **>(&bmsClient));
    if (result != 0) {
        return nullptr;
    }

    return bmsClient;
}

bool Install(const char *hapPath, const InstallParam *installParam, InstallerCallback installerCallback)
{
    if ((hapPath == nullptr) || (installerCallback == nullptr) || (installParam == nullptr)) {
        HILOG_ERROR(HILOG_MODULE_APP, "BundleManager install failed due to nullptr parameters");
        return false;
    }
    if (CheckSelfPermission(static_cast<const char *>(PERMISSION_INSTALL_BUNDLE)) != GRANTED) {
        HILOG_ERROR(HILOG_MODULE_APP, "BundleManager install failed due to permission denied");
        return false;
    }
    auto bmsInnerClient = GetBmsInnerClient();
    if (bmsInnerClient == nullptr) {
        HILOG_ERROR(HILOG_MODULE_APP, "BundleManager install failed due to nullptr bms client");
        return false;
    }

    IpcIo ipcIo;
    char data[MAX_IO_SIZE];
    IpcIoInit(&ipcIo, data, MAX_IO_SIZE, OBJECT_NUMBER_IN_INSTALLATION);
    WriteString(&ipcIo, hapPath);
    const SvcIdentity *svc = OHOS::BundleSelfCallback::GetInstance().RegisterBundleSelfCallback(installerCallback);
    if (svc == nullptr) {
        HILOG_ERROR(HILOG_MODULE_APP, "BundleManager Install svc is nullptr");
        return false;
    }
    bool writeRemote = WriteRemoteObject(&ipcIo, svc);
    if (!writeRemote) {
        HILOG_ERROR(HILOG_MODULE_APP, "BundleManager Install ipc failed");
        return false;
    }
    WriteInt32(&ipcIo, installParam->installLocation);
    HILOG_DEBUG(HILOG_MODULE_APP, "BMS client invoke install");
    uint8_t result = 0;
    int32_t ret = bmsInnerClient->Invoke(bmsInnerClient, INSTALL, &ipcIo, &result, Notify);
    if (ret != OHOS_SUCCESS) {
        HILOG_ERROR(HILOG_MODULE_APP, "BundleManager Install invoke failed: %{public}d", ret);
        return false;
    }
    return result == OHOS_SUCCESS;
}

bool Uninstall(const char *bundleName, const InstallParam *installParam, InstallerCallback installerCallback)
{
    // installParam is nullptr at present.
    if ((bundleName == nullptr) || (installerCallback == nullptr) || (strlen(bundleName) >= MAX_BUNDLE_NAME) ||
        (installParam == nullptr)) {
        HILOG_ERROR(HILOG_MODULE_APP, "BundleManager uninstall failed due to nullptr or invalid parameters");
        return false;
    }
    if (CheckSelfPermission(static_cast<const char *>(PERMISSION_INSTALL_BUNDLE)) != GRANTED) {
        HILOG_ERROR(HILOG_MODULE_APP, "BundleManager uninstall failed due to permission denied");
        return false;
    }
    auto bmsInnerClient = GetBmsInnerClient();
    if (bmsInnerClient == nullptr) {
        HILOG_ERROR(HILOG_MODULE_APP, "BundleManager uninstall failed due to nullptr bms client");
        return false;
    }

    const SvcIdentity *svc = OHOS::BundleSelfCallback::GetInstance().RegisterBundleSelfCallback(installerCallback);
    if (svc == nullptr) {
        HILOG_ERROR(HILOG_MODULE_APP, "BundleManager Uninstall svc is nullptr");
        return false;
    }
    IpcIo ipcIo;
    char data[MAX_IO_SIZE];
    IpcIoInit(&ipcIo, data, MAX_IO_SIZE, 1);
    WriteString(&ipcIo, bundleName);
    bool writeRemote = WriteRemoteObject(&ipcIo, svc);
    if (!writeRemote) {
        HILOG_ERROR(HILOG_MODULE_APP, "BundleManager Uninstall ipc failed");
        return false;
    }
    WriteBool(&ipcIo, installParam->keepData);
    HILOG_DEBUG(HILOG_MODULE_APP, "BMS client invoke uninstall");
    uint8_t result = 0;
    int32_t ret = bmsInnerClient->Invoke(bmsInnerClient, UNINSTALL, &ipcIo, &result, Notify);
    if (ret != OHOS_SUCCESS) {
        HILOG_ERROR(HILOG_MODULE_APP, "BundleManager Uninstall invoke failed: %{public}d", ret);
        return false;
    }
    return result == OHOS_SUCCESS;
}

uint8_t QueryAbilityInfo(const Want *want, AbilityInfo *abilityInfo)
{
    if ((want == nullptr) || (abilityInfo == nullptr)) {
        return ERR_APPEXECFWK_OBJECT_NULL;
    }
    if (CheckSelfPermission(static_cast<const char *>(PERMISSION_GET_BUNDLE_INFO)) != GRANTED) {
        HILOG_ERROR(HILOG_MODULE_APP, "BundleManager query AbilityInfo failed due to permission denied");
        return ERR_APPEXECFWK_PERMISSION_DENIED;
    }
    auto bmsClient = GetBmsClient();
    if (bmsClient == nullptr) {
        HILOG_ERROR(HILOG_MODULE_APP, "BundleManager query AbilityInfo failed due to nullptr bms client");
        return ERR_APPEXECFWK_OBJECT_NULL;
    }

    IpcIo ipcIo;
    char data[MAX_IO_SIZE];
    IpcIoInit(&ipcIo, data, MAX_IO_SIZE, OBJECT_NUMBER_IN_WANT);
    if (!SerializeWant(&ipcIo, want)) {
        return ERR_APPEXECFWK_SERIALIZATION_FAILED;
    }
    ResultOfQueryAbilityInfo resultOfQueryAbilityInfo = { 0, nullptr };

    int32_t ret = bmsClient->Invoke(bmsClient, QUERY_ABILITY_INFO, &ipcIo, &resultOfQueryAbilityInfo, Notify);
    if (ret != OHOS_SUCCESS) {
        HILOG_ERROR(HILOG_MODULE_APP, "BundleManager QueryAbilityInfo invoke failed: %{public}d", ret);
        return ERR_APPEXECFWK_INVOKE_ERROR;
    }

    if (resultOfQueryAbilityInfo.abilityInfo == nullptr) {
        HILOG_ERROR(HILOG_MODULE_APP, "BundleManager QueryAbilityInfo failed: %{public}d",
            resultOfQueryAbilityInfo.resultCode);
        return resultOfQueryAbilityInfo.resultCode;
    }
    if (resultOfQueryAbilityInfo.resultCode == ERR_OK) {
        OHOS::AbilityInfoUtils::CopyAbilityInfo(abilityInfo, *(resultOfQueryAbilityInfo.abilityInfo));
        ClearAbilityInfo(resultOfQueryAbilityInfo.abilityInfo);
        AdapterFree(resultOfQueryAbilityInfo.abilityInfo);
    }

    return resultOfQueryAbilityInfo.resultCode;
}

uint8_t GetBundleInfo(const char *bundleName, int32_t flags, BundleInfo *bundleInfo)
{
    if ((bundleName == nullptr) || (bundleInfo == nullptr)) {
        return ERR_APPEXECFWK_OBJECT_NULL;
    }
    if (flags < 0 || flags > 1 || (strlen(bundleName) >= MAX_BUNDLE_NAME)) {
        return ERR_APPEXECFWK_QUERY_PARAMETER_ERROR;
    }
    if (CheckSelfPermission(static_cast<const char *>(PERMISSION_GET_BUNDLE_INFO)) != GRANTED) {
        HILOG_ERROR(HILOG_MODULE_APP, "BundleManager get BundleInfo failed due to permission denied");
        return ERR_APPEXECFWK_PERMISSION_DENIED;
    }
    auto bmsClient = GetBmsClient();
    if (bmsClient == nullptr) {
        HILOG_ERROR(HILOG_MODULE_APP, "BundleManager get BundleInfo failed due to nullptr bms client");
        return ERR_APPEXECFWK_OBJECT_NULL;
    }

    IpcIo ipcIo;
    char data[MAX_IO_SIZE];
    IpcIoInit(&ipcIo, data, MAX_IO_SIZE, 0);
    WriteString(&ipcIo, bundleName);
    WriteInt32(&ipcIo, flags);
    ResultOfGetBundleInfo resultOfGetBundleInfo;
    resultOfGetBundleInfo.bundleInfo = nullptr;
    int32_t ret = bmsClient->Invoke(bmsClient, GET_BUNDLE_INFO, &ipcIo, &resultOfGetBundleInfo, Notify);
    if (ret != OHOS_SUCCESS) {
        HILOG_ERROR(HILOG_MODULE_APP, "BundleManager GetBundleInfo invoke failed: %{public}d", ret);
        return ERR_APPEXECFWK_INVOKE_ERROR;
    }
    if (resultOfGetBundleInfo.bundleInfo == nullptr) {
        HILOG_ERROR(HILOG_MODULE_APP, "BundleManager GetBundleInfo failed: %{public}d",
            resultOfGetBundleInfo.resultCode);
        return resultOfGetBundleInfo.resultCode;
    }
    if (resultOfGetBundleInfo.resultCode == ERR_OK) {
        OHOS::BundleInfoUtils::CopyBundleInfo(flags, bundleInfo, *(resultOfGetBundleInfo.bundleInfo));
        ClearBundleInfo(resultOfGetBundleInfo.bundleInfo);
        AdapterFree(resultOfGetBundleInfo.bundleInfo);
    }
    return resultOfGetBundleInfo.resultCode;
}

static uint8_t ObtainInnerBundleInfos(const int flags, BundleInfo **bundleInfos, int32_t *len,
    uint8_t code, IpcIo *ipcIo)
{
    if ((bundleInfos == nullptr) || (len == nullptr) || (ipcIo == nullptr)) {
        return ERR_APPEXECFWK_OBJECT_NULL;
    }
    if (CheckSelfPermission(static_cast<const char *>(PERMISSION_GET_BUNDLE_INFO)) != GRANTED) {
        HILOG_ERROR(HILOG_MODULE_APP, "BundleManager get BundleInfos failed due to permission denied");
        return ERR_APPEXECFWK_PERMISSION_DENIED;
    }
    auto bmsClient = GetBmsClient();
    if (bmsClient == nullptr) {
        HILOG_ERROR(HILOG_MODULE_APP, "BundleManager get BundleInfos failed due to nullptr bms client");
        return ERR_APPEXECFWK_OBJECT_NULL;
    }

    ResultOfGetBundleInfos resultOfGetBundleInfos;
    resultOfGetBundleInfos.length = 0;
    resultOfGetBundleInfos.bundleInfo = nullptr;
    int32_t ret = bmsClient->Invoke(bmsClient, code, ipcIo, &resultOfGetBundleInfos, Notify);
    if (ret != OHOS_SUCCESS) {
        HILOG_ERROR(HILOG_MODULE_APP, "BundleManager ObtainInnerBundleInfo invoke failed: %{public}d\n", ret);
        return ERR_APPEXECFWK_INVOKE_ERROR;
    }

    if (resultOfGetBundleInfos.length == 0 || resultOfGetBundleInfos.resultCode != ERR_OK) {
        HILOG_ERROR(HILOG_MODULE_APP, "BundleManager ObtainInnerBundleInfo fail");
        *bundleInfos = nullptr;
        return resultOfGetBundleInfos.resultCode;
    }

    *bundleInfos = reinterpret_cast<BundleInfo *>(AdapterMalloc(sizeof(BundleInfo) * resultOfGetBundleInfos.length));
    if (*bundleInfos == nullptr) {
        OHOS::BundleInfoUtils::FreeBundleInfos(resultOfGetBundleInfos.bundleInfo, resultOfGetBundleInfos.length);
        return ERR_APPEXECFWK_OBJECT_NULL;
    }
    if (memset_s(*bundleInfos, sizeof(BundleInfo) * (resultOfGetBundleInfos.length), 0, sizeof(BundleInfo) *
        (resultOfGetBundleInfos.length)) != EOK) {
        AdapterFree(*bundleInfos);
        OHOS::BundleInfoUtils::FreeBundleInfos(resultOfGetBundleInfos.bundleInfo, resultOfGetBundleInfos.length);
        return ERR_APPEXECFWK_SYSTEM_INTERNAL_ERROR;
    }
    for (int32_t i = 0; i < resultOfGetBundleInfos.length; ++i) {
        OHOS::BundleInfoUtils::CopyBundleInfo(flags, *bundleInfos + i, (resultOfGetBundleInfos.bundleInfo)[i]);
    }
    *len = resultOfGetBundleInfos.length;
    OHOS::BundleInfoUtils::FreeBundleInfos(resultOfGetBundleInfos.bundleInfo, resultOfGetBundleInfos.length);
    return resultOfGetBundleInfos.resultCode;
}

static uint8_t ObtainBundleInfosOneByOne(BasicInfo basicInfo, int32_t len, uint8_t code, IClientProxy *bmsClient,
    BundleInfo **bundleInfos)
{
    if (bmsClient == nullptr || bundleInfos == nullptr) {
        HILOG_ERROR(HILOG_MODULE_APP, "BundleManager ObtainBundleInfosOneByOne failed due to nullptr parma");
        return ERR_APPEXECFWK_OBJECT_NULL;
    }
    for (int32_t i = 0; i < len; ++i) {
        IpcIo innerIpcIo;
        char data[MAX_IO_SIZE];
        IpcIoInit(&innerIpcIo, data, MAX_IO_SIZE, 0);
        WriteInt32(&innerIpcIo, static_cast<int32_t>(code));
        if (code == GET_BUNDLE_INFOS) {
            WriteInt32(&innerIpcIo, basicInfo.flags);
        }
        if (code == GET_BUNDLE_INFOS_BY_METADATA) {
            WriteString(&innerIpcIo, basicInfo.metaDataKey);
        }
        WriteInt32(&innerIpcIo, i);
        ResultOfGetBundleInfo resultOfGetBundleInfo;
        resultOfGetBundleInfo.bundleInfo = nullptr;
        int32_t ret = bmsClient->Invoke(bmsClient, GET_BUNDLE_INFO_BY_INDEX, &innerIpcIo,
            &resultOfGetBundleInfo, Notify);
        if (ret != OHOS_SUCCESS) {
            HILOG_ERROR(HILOG_MODULE_APP, "BundleManager ObtainBundleInfosOneByOne invoke failed: %{public}d", ret);
            return ERR_APPEXECFWK_INVOKE_ERROR;
        }
        if (resultOfGetBundleInfo.bundleInfo == nullptr) {
            HILOG_ERROR(HILOG_MODULE_APP, "BundleManager ObtainBundleInfosOneByOne failed: %{public}d",
                resultOfGetBundleInfo.resultCode);
            return resultOfGetBundleInfo.resultCode;
        }
        OHOS::BundleInfoUtils::CopyBundleInfo(basicInfo.flags, *bundleInfos + i, *(resultOfGetBundleInfo.bundleInfo));
        ClearBundleInfo(resultOfGetBundleInfo.bundleInfo);
        AdapterFree(resultOfGetBundleInfo.bundleInfo);
    }
    return OHOS_SUCCESS;
}

static uint8_t ObtainBundleInfos(BasicInfo basicInfo, BundleInfo **bundleInfos, int32_t *len,
    uint8_t code, IpcIo *ipcIo)
{
    if ((bundleInfos == nullptr) || (len == nullptr) || (ipcIo == nullptr)) {
        return ERR_APPEXECFWK_OBJECT_NULL;
    }
    if (CheckSelfPermission(static_cast<const char *>(PERMISSION_GET_BUNDLE_INFO)) != GRANTED) {
        HILOG_ERROR(HILOG_MODULE_APP, "BundleManager ObtainBundleInfos failed due to permission denied");
        return ERR_APPEXECFWK_PERMISSION_DENIED;
    }
    auto bmsClient = GetBmsClient();
    if (bmsClient == nullptr) {
        HILOG_ERROR(HILOG_MODULE_APP, "BundleManager ObtainBundleInfos failed due to nullptr bms client");
        return ERR_APPEXECFWK_OBJECT_NULL;
    }

    ResultOfGetBundleInfos resultOfGetBundleInfos;
    resultOfGetBundleInfos.length = 0;
    resultOfGetBundleInfos.bundleInfo = nullptr;
    int32_t ret = bmsClient->Invoke(bmsClient, GET_BUNDLE_INFO_LENGTH, ipcIo, &resultOfGetBundleInfos, Notify);
    if (ret != OHOS_SUCCESS) {
        HILOG_ERROR(HILOG_MODULE_APP, "BundleManager ObtainBundleInfos invoke failed: %{public}d\n", ret);
        return ERR_APPEXECFWK_INVOKE_ERROR;
    }

    if (resultOfGetBundleInfos.length == 0 || resultOfGetBundleInfos.resultCode != ERR_OK) {
        HILOG_ERROR(HILOG_MODULE_APP, "BundleManager ObtainBundleInfos fail");
        *bundleInfos = nullptr;
        return resultOfGetBundleInfos.resultCode;
    }

    *bundleInfos = reinterpret_cast<BundleInfo *>(AdapterMalloc(sizeof(BundleInfo) * resultOfGetBundleInfos.length));
    if (*bundleInfos == nullptr) {
        OHOS::BundleInfoUtils::FreeBundleInfos(resultOfGetBundleInfos.bundleInfo, resultOfGetBundleInfos.length);
        return ERR_APPEXECFWK_OBJECT_NULL;
    }
    if (memset_s(*bundleInfos, sizeof(BundleInfo) * (resultOfGetBundleInfos.length), 0, sizeof(BundleInfo) *
        (resultOfGetBundleInfos.length)) != EOK) {
        AdapterFree(*bundleInfos);
        OHOS::BundleInfoUtils::FreeBundleInfos(resultOfGetBundleInfos.bundleInfo, resultOfGetBundleInfos.length);
        return ERR_APPEXECFWK_SYSTEM_INTERNAL_ERROR;
    }

    uint8_t res = ObtainBundleInfosOneByOne(basicInfo, resultOfGetBundleInfos.length, code, bmsClient, bundleInfos);
    if (res != OHOS_SUCCESS) {
        HILOG_WARN(HILOG_MODULE_APP, "BundleManager ObtainBundleInfos invoke failed: %{public}d\n", res);
    }
    *len = resultOfGetBundleInfos.length;
    OHOS::BundleInfoUtils::FreeBundleInfos(resultOfGetBundleInfos.bundleInfo, resultOfGetBundleInfos.length);
    return res;
}

uint8_t GetBundleInfos(const int flags, BundleInfo **bundleInfos, int32_t *len)
{
    if ((bundleInfos == nullptr) || (len == nullptr)) {
        return ERR_APPEXECFWK_OBJECT_NULL;
    }

    if (flags < 0 || flags > 1) {
        return ERR_APPEXECFWK_QUERY_PARAMETER_ERROR;
    }
    IpcIo ipcIo;
    char data[MAX_IO_SIZE];
    IpcIoInit(&ipcIo, data, MAX_IO_SIZE, 0);
#ifdef __LINUX__
    WriteInt32(&ipcIo, flags);
    return ObtainInnerBundleInfos(flags, bundleInfos, len, GET_BUNDLE_INFOS, &ipcIo);
#else
    WriteInt32(&ipcIo, GET_BUNDLE_INFOS);
    WriteInt32(&ipcIo, flags);
    BasicInfo basicInfo;
    basicInfo.flags = flags;
    basicInfo.metaDataKey = nullptr;
    return ObtainBundleInfos(basicInfo, bundleInfos, len, GET_BUNDLE_INFOS, &ipcIo);
#endif
}

uint32_t GetBundleSize(const char *bundleName)
{
    if (bundleName == nullptr) {
        return 0;
    }
    if (strlen(bundleName) >= MAX_BUNDLE_NAME) {
        return 0;
    }
    if (CheckSelfPermission(static_cast<const char *>(PERMISSION_GET_BUNDLE_INFO)) != GRANTED) {
        HILOG_ERROR(HILOG_MODULE_APP, "BundleManager get bundle size failed due to permission denied");
        return 0;
    }
    auto bmsClient = GetBmsClient();
    if (bmsClient == nullptr) {
        HILOG_ERROR(HILOG_MODULE_APP, "BundleManager get bundle size failed due to nullptr bms client");
        return 0;
    }

    IpcIo ipcIo;
    char data[MAX_IO_SIZE];
    IpcIoInit(&ipcIo, data, MAX_IO_SIZE, 0);
    WriteString(&ipcIo, bundleName);

    ResultOfGetBundleSize resultOfGetBundleSize;
    int32_t ret = bmsClient->Invoke(bmsClient, GET_BUNDLE_SIZE, &ipcIo, &resultOfGetBundleSize, Notify);
    if (ret != OHOS_SUCCESS) {
        HILOG_ERROR(HILOG_MODULE_APP, "BundleManager GetBundleSize invoke failed: %{public}d", ret);
        return 0;
    }
    uint32_t bundleSize = resultOfGetBundleSize.bundleSize;
    return bundleSize;
}

uint8_t QueryKeepAliveBundleInfos(BundleInfo **bundleInfos, int32_t *len)
{
    if ((bundleInfos == nullptr) || (len == nullptr)) {
        return ERR_APPEXECFWK_OBJECT_NULL;
    }
    IpcIo ipcIo;
    char data[MAX_IO_SIZE];
    IpcIoInit(&ipcIo, data, MAX_IO_SIZE, 0);
    return ObtainInnerBundleInfos(0, bundleInfos, len, QUERY_KEEPALIVE_BUNDLE_INFOS, &ipcIo);
}

uint8_t GetBundleInfosByMetaData(const char *metaDataKey, BundleInfo **bundleInfos, int32_t *len)
{
    if ((metaDataKey == nullptr) || (bundleInfos == nullptr) || (len == nullptr)) {
        return ERR_APPEXECFWK_OBJECT_NULL;
    }
    IpcIo ipcIo;
    char data[MAX_IO_SIZE];
    IpcIoInit(&ipcIo, data, MAX_IO_SIZE, 0);
    WriteString(&ipcIo, metaDataKey);
    return ObtainInnerBundleInfos(0, bundleInfos, len, GET_BUNDLE_INFOS_BY_METADATA, &ipcIo);
}

uint8_t GetBundleNameForUid(int32_t uid, char **bundleName)
{
    if (bundleName == nullptr) {
        return ERR_APPEXECFWK_OBJECT_NULL;
    }
    if (CheckSelfPermission(static_cast<const char *>(PERMISSION_GET_BUNDLE_INFO)) != GRANTED) {
        HILOG_ERROR(HILOG_MODULE_APP, "BundleManager get BundleName for uid failed due to permission denied");
        return ERR_APPEXECFWK_PERMISSION_DENIED;
    }
    auto bmsClient = GetBmsClient();
    if (bmsClient == nullptr) {
        HILOG_ERROR(HILOG_MODULE_APP, "BundleManager get BundleName for uid failed due to nullptr bms client");
        return ERR_APPEXECFWK_OBJECT_NULL;
    }

    IpcIo ipcIo;
    char data[MAX_IO_SIZE];
    IpcIoInit(&ipcIo, data, MAX_IO_SIZE, 0);
    WriteInt32(&ipcIo, uid);

    ResultOfGetBundleNameForUid resultOfGetBundleNameForUid;
    resultOfGetBundleNameForUid.length = 0;
    resultOfGetBundleNameForUid.bundleName = nullptr;
    int32_t ret = bmsClient->Invoke(bmsClient, GET_BUNDLENAME_FOR_UID, &ipcIo, &resultOfGetBundleNameForUid, Notify);
    if (ret != OHOS_SUCCESS) {
        HILOG_ERROR(HILOG_MODULE_APP, "BundleManager GetBundleNameForUid invoke failed: %{public}d\n", ret);
        return ERR_APPEXECFWK_INVOKE_ERROR;
    }
    if (resultOfGetBundleNameForUid.bundleName == nullptr || resultOfGetBundleNameForUid.length == 0) {
        HILOG_ERROR(HILOG_MODULE_APP, "BundleManager GetBundleNameForUid failed: %{public}d\n",
            resultOfGetBundleNameForUid.resultCode);
        return resultOfGetBundleNameForUid.resultCode;
    }

    *bundleName = reinterpret_cast<char *>(AdapterMalloc(resultOfGetBundleNameForUid.length + 1));
    if (*bundleName == nullptr) {
        AdapterFree(resultOfGetBundleNameForUid.bundleName);
        return ERR_APPEXECFWK_OBJECT_NULL;
    }
    errno_t err = strncpy_s(*bundleName, resultOfGetBundleNameForUid.length + 1,
        resultOfGetBundleNameForUid.bundleName, resultOfGetBundleNameForUid.length);
    AdapterFree(resultOfGetBundleNameForUid.bundleName);
    if (err != EOK) {
        return ERR_APPEXECFWK_SYSTEM_INTERNAL_ERROR;
    }
    return resultOfGetBundleNameForUid.resultCode;
}

bool HasSystemCapability(const char *sysCapName)
{
    if (sysCapName == nullptr || strlen(sysCapName) > MAX_SYSCAP_NAME_LEN) {
        HILOG_ERROR(HILOG_MODULE_APP, "BundleManager HasSystemCapability failed due to parameters is invalid");
        return false;
    }
    auto bmsClient = GetBmsClient();
    if (bmsClient == nullptr) {
        HILOG_ERROR(HILOG_MODULE_APP, "BundleManager HasSystemCapability failed due to nullptr bms client");
        return false;
    }
    IpcIo ipcIo;
    char data[MAX_IO_SIZE];
    IpcIoInit(&ipcIo, data, MAX_IO_SIZE, 0);
    WriteString(&ipcIo, sysCapName);
    uint8_t result = 0;
    int32_t ret = bmsClient->Invoke(bmsClient, CHECK_SYS_CAP, &ipcIo, &result, Notify);
    if (ret != OHOS_SUCCESS) {
        HILOG_ERROR(HILOG_MODULE_APP, "BundleManager HasSystemCapability invoke failed: %{public}d", ret);
        return false;
    }
    return result == OHOS_SUCCESS;
}

SystemCapability *GetSystemAvailableCapabilities()
{
    auto bmsClient = GetBmsClient();
    if (bmsClient == nullptr) {
        HILOG_ERROR(HILOG_MODULE_APP, "BundleManager GetSystemAvailableCapabilities failed due to nullptr bms client");
        return nullptr;
    }
    IpcIo ipcIo;
    char data[MAX_IO_SIZE];
    IpcIoInit(&ipcIo, data, MAX_IO_SIZE, 0);
    ResultOfGetSysCap resultOfGetSysCap;
    resultOfGetSysCap.systemCap.systemCapNum = 0;
    resultOfGetSysCap.systemCap.systemCapName = nullptr;
    int32_t ret = bmsClient->Invoke(bmsClient, GET_SYS_CAP, &ipcIo, &resultOfGetSysCap, Notify);
    if (ret != OHOS_SUCCESS) {
        HILOG_ERROR(HILOG_MODULE_APP, "BundleManager GetSystemAvailableCapabilities invoke failed: %{public}d\n", ret);
        return nullptr;
    }
    if (resultOfGetSysCap.systemCap.systemCapNum == 0 || resultOfGetSysCap.resultCode != ERR_OK
        || resultOfGetSysCap.systemCap.systemCapName == nullptr) {
        HILOG_ERROR(HILOG_MODULE_APP, "BundleManager GetSystemAvailableCapabilities fail");
        return nullptr;
    }
    int32_t sysCapNum = resultOfGetSysCap.systemCap.systemCapNum;
    SystemCapability *retSystemCap = reinterpret_cast<SystemCapability *>(AdapterMalloc(sizeof(SystemCapability)));
    if (retSystemCap == nullptr) {
        HILOG_ERROR(HILOG_MODULE_APP, "BundleManager GetSystemAvailableCapabilities retSystemCap is null");
        AdapterFree(resultOfGetSysCap.systemCap.systemCapName);
        return nullptr;
    }
    if (memset_s(retSystemCap, sizeof(SystemCapability), 0, sizeof(SystemCapability)) != EOK) {
        AdapterFree(resultOfGetSysCap.systemCap.systemCapName);
        AdapterFree(retSystemCap);
        return nullptr;
    }
    int32_t sysCapNameMem = sizeof(SystemCapName) * sysCapNum;
    retSystemCap->systemCapName = reinterpret_cast<SystemCapName *>(AdapterMalloc(sysCapNameMem));
    if (retSystemCap->systemCapName == nullptr) {
        AdapterFree(resultOfGetSysCap.systemCap.systemCapName);
        AdapterFree(retSystemCap);
        return nullptr;
    }
    if (memset_s(retSystemCap->systemCapName, sysCapNameMem, 0, sysCapNameMem) != EOK) {
        AdapterFree(resultOfGetSysCap.systemCap.systemCapName);
        AdapterFree(retSystemCap->systemCapName);
        AdapterFree(retSystemCap);
        HILOG_ERROR(HILOG_MODULE_APP, "BundleManager GetSystemAvailableCapabilities memset error");
        return nullptr;
    }
    for (int32_t index = 0; index < sysCapNum; index++) {
        int32_t copyLen = strlen(resultOfGetSysCap.systemCap.systemCapName[index].name);
        errno_t err = strncpy_s(retSystemCap->systemCapName[index].name, MAX_SYSCAP_NAME_LEN,
            resultOfGetSysCap.systemCap.systemCapName[index].name, copyLen);
        if (err != EOK) {
            HILOG_ERROR(HILOG_MODULE_APP, "BundleManager GetSystemAvailableCapabilities strcpy error %{public}d", err);
            AdapterFree(resultOfGetSysCap.systemCap.systemCapName);
            AdapterFree(retSystemCap->systemCapName);
            AdapterFree(retSystemCap);
            return nullptr;
        }
    }
    retSystemCap->systemCapNum = sysCapNum;
    return retSystemCap;
}

void FreeSystemAvailableCapabilitiesInfo(SystemCapability *sysCap)
{
    if (sysCap == nullptr) {
        return;
    }
    if (sysCap->systemCapName != nullptr) {
        AdapterFree(sysCap->systemCapName);
    }
    AdapterFree(sysCap);
}
}
