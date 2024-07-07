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

#ifndef FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_CORE_INCLUDE_BUNDLEMGR_BUNDLE_MGR_HOST_H
#define FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_CORE_INCLUDE_BUNDLEMGR_BUNDLE_MGR_HOST_H

#include <mutex>

#include "iremote_stub.h"
#include "nocopyable.h"

#include "appexecfwk_errors.h"
#include "bundle_mgr_interface.h"
#include "bundle_mgr_proxy.h"

namespace OHOS {
namespace AppExecFwk {
class BundleMgrHost : public IRemoteStub<IBundleMgr> {
public:
    BundleMgrHost();
    virtual ~BundleMgrHost() = default;

    int OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;

private:
    /**
     * @brief Handles the GetApplicationInfo function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleGetApplicationInfo(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handles the GetApplicationInfo function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleGetApplicationInfoWithIntFlags(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handles the GetApplicationInfoV9 function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleGetApplicationInfoWithIntFlagsV9(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handles the GetApplicationInfos function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleGetApplicationInfos(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handles the GetApplicationInfos function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleGetApplicationInfosWithIntFlags(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handles the GetApplicationsInfoV9 function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleGetApplicationInfosWithIntFlagsV9(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handles the GetBundleInfo function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleGetBundleInfo(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handles the GetBundleInfo function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleGetBundleInfoForSelf(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handles the GetBundlePackInfo function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleGetBundlePackInfo(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handles the GetBundlePackInfo function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleGetBundlePackInfoWithIntFlags(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handles the GetBundleInfo function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleGetBundleInfoWithIntFlags(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handles the GetBundleInfo function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleGetBundleInfoWithIntFlagsV9(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handles the GetBundleInfos function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleGetBundleInfos(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handles the GetBundleInfos function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleGetBundleInfosWithIntFlags(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handles the GetBundleInfo function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleGetBundleInfosWithIntFlagsV9(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handles the GetBundleNameForUid function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleGetBundleNameForUid(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handles the GetBundlesForUid function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleGetBundlesForUid(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handles the GetNameForUid function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleGetNameForUid(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handles the GetBundleGids function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleGetBundleGids(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handles the GetBundleGidsByUid function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleGetBundleGidsByUid(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handles the GetBundleInfosByMetaData function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleGetBundleInfosByMetaData(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handles the QueryAbilityInfo function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleQueryAbilityInfo(MessageParcel &data, MessageParcel &reply);

     /**
     * @brief Handles the QueryAbilityInfo function called from a IBundleMgr proxy object with callback.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleQueryAbilityInfoWithCallback(MessageParcel &data, MessageParcel &reply);

    /**
     * @brief Handles the SilentInstall function called from a IBundleMgr proxy.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleSilentInstall(MessageParcel &data, MessageParcel &reply);

    /**
     * @brief Handles the UpgradeAtomicService function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleUpgradeAtomicService(MessageParcel &data, MessageParcel &reply);

    /**
     * @brief Handles the QueryAbilityInfo function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleQueryAbilityInfoMutiparam(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handles the QueryAbilityInfos function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleQueryAbilityInfos(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handles the QueryAbilityInfoByUri function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleQueryAbilityInfoByUri(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handles the QueryAbilityInfoByUri function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleQueryAbilityInfosByUri(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handles the QueryAbilityInfosMutiparam function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleQueryAbilityInfosMutiparam(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handles the QueryAbilityInfosV9 function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleQueryAbilityInfosV9(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handles the HandleQueryAllAbilityInfos function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleQueryAllAbilityInfos(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handles the QueryAbilityInfoByUri function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleQueryAbilityInfoByUriForUserId(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handles the QueryKeepAliveBundleInfos function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleQueryKeepAliveBundleInfos(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handles the GetAbilityLabel function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleGetAbilityLabel(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handles the GetAbilityLabel function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleGetAbilityLabelWithModuleName(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handles the CheckIsSystemAppByUid function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleCheckIsSystemAppByUid(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handles the GetBundleArchiveInfo function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleGetBundleArchiveInfo(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handles the GetBundleArchiveInfo function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleGetBundleArchiveInfoWithIntFlags(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handles the GetBundleArchiveInfo function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleGetBundleArchiveInfoWithIntFlagsV9(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handles the GetHapModuleInfo function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleGetHapModuleInfo(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handles the GetHapModuleInfo function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleGetHapModuleInfoWithUserId(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handles the GetLaunchWantForBundle function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleGetLaunchWantForBundle(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handles the CheckPublicKeys function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleCheckPublicKeys(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handles the GetPermissionDef function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleGetPermissionDef(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handles the HasSystemCapability function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleHasSystemCapability(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handles the GetSystemAvailableCapabilities function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleGetSystemAvailableCapabilities(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handles the IsSafeMode function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleIsSafeMode(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handles the CleanBundleCacheFiles function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleCleanBundleCacheFiles(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handles the CleanBundleDataFiles function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleCleanBundleDataFiles(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handles the RegisterBundleStatusCallback function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleRegisterBundleStatusCallback(MessageParcel &data, MessageParcel &reply);

    ErrCode HandleRegisterBundleEventCallback(MessageParcel &data, MessageParcel &reply);

    ErrCode HandleUnregisterBundleEventCallback(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handles the ClearBundleStatusCallback function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleClearBundleStatusCallback(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handles the UnregisterBundleStatusCallback function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleUnregisterBundleStatusCallback(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handles the DumpInfos function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleDumpInfos(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handles the GetBundleInstaller function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleGetBundleInstaller(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handles the GetBundleUserMgr function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleGetBundleUserMgr(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handles the IsApplicationEnabled function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleIsApplicationEnabled(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handles the SetApplicationEnabled function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleSetApplicationEnabled(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handles the IsAbilityEnabled function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleIsAbilityEnabled(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handles the SetAbilityEnabled function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleSetAbilityEnabled(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handles the GetAllFormsInfo function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleGetAllFormsInfo(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handles the GetFormsInfoByApp function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleGetFormsInfoByApp(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handles the HandleGetFormsInfoByModule function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleGetFormsInfoByModule(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handles the HandleGetShortcutInfo function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleGetShortcutInfos(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handles the HandleGetShortcutInfo function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns err_code of result.
     */
    ErrCode HandleGetShortcutInfoV9(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handles the HandleGetAllCommonEventInfo function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleGetAllCommonEventInfo(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handles the GetDistributedBundleInfo function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleGetDistributedBundleInfo(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handles the HandleGetAppPrivilegeLevel function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleGetAppPrivilegeLevel(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handles the HandleQueryExtAbilityInfosWithoutType function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleQueryExtAbilityInfosWithoutType(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handles the HandleQueryExtAbilityInfosWithoutTypeV9 function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleQueryExtAbilityInfosWithoutTypeV9(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handles the HandleQueryExtensionInfo function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleQueryExtAbilityInfos(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handles the HandleQueryExtAbilityInfosV9 function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleQueryExtAbilityInfosV9(MessageParcel &data, MessageParcel &reply);

    ErrCode HandleQueryExtAbilityInfosByType(MessageParcel &data, MessageParcel &reply);

    ErrCode HandleVerifyCallingPermission(MessageParcel &data, MessageParcel &reply);

    ErrCode HandleGetAccessibleAppCodePaths(MessageParcel &data, MessageParcel &reply);

    ErrCode HandleQueryExtensionAbilityInfoByUri(MessageParcel &data, MessageParcel &reply);

    ErrCode HandleGetAppIdByBundleName(MessageParcel &data, MessageParcel &reply);

    ErrCode HandleGetAppType(MessageParcel &data, MessageParcel &reply);

    ErrCode HandleGetUidByBundleName(MessageParcel &data, MessageParcel &reply);

    ErrCode HandleGetUidByDebugBundleName(MessageParcel &data, MessageParcel &reply);

    ErrCode HandleGetAbilityInfo(MessageParcel &data, MessageParcel &reply);

    ErrCode HandleGetAbilityInfoWithModuleName(MessageParcel &data, MessageParcel &reply);

    /**
     * @brief Handles the HandleGetModuleUpgradeFlag function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleGetModuleUpgradeFlag(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handles the HandleSetModuleUpgradeFlag function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleSetModuleUpgradeFlag(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handles the HandleIsModuleRemovable function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleIsModuleRemovable(MessageParcel &data, MessageParcel &reply);
    /**
     * @brief Handles the HandleSetModuleRemovable function called from a IBundleMgr proxy object.
     * @param data Indicates the data to be read.
     * @param reply Indicates the reply to be sent;
     * @return Returns ERR_OK if called successfully; returns error code otherwise.
     */
    ErrCode HandleSetModuleRemovable(MessageParcel &data, MessageParcel &reply);

    ErrCode HandleImplicitQueryInfoByPriority(MessageParcel &data, MessageParcel &reply);

    ErrCode HandleImplicitQueryInfos(MessageParcel &data, MessageParcel &reply);

    ErrCode HandleGetAllDependentModuleNames(MessageParcel &data, MessageParcel &reply);

    ErrCode HandleGetSandboxBundleInfo(MessageParcel &data, MessageParcel &reply);

    ErrCode HandleSetDisposedStatus(MessageParcel &data, MessageParcel &reply);

    ErrCode HandleGetDisposedStatus(MessageParcel &data, MessageParcel &reply);

    ErrCode HandleObtainCallingBundleName(MessageParcel &data, MessageParcel &reply);

    ErrCode HandleGetBundleStats(MessageParcel &data, MessageParcel &reply);

    ErrCode HandleCheckAbilityEnableInstall(MessageParcel &data, MessageParcel &reply);

    ErrCode HandleGetSandboxAbilityInfo(MessageParcel &data, MessageParcel &reply);

    ErrCode HandleGetSandboxExtAbilityInfos(MessageParcel &data, MessageParcel &reply);

    ErrCode HandleGetSandboxHapModuleInfo(MessageParcel &data, MessageParcel &reply);

    ErrCode HandleGetMediaData(MessageParcel &data, MessageParcel &reply);

    ErrCode HandleGetStringById(MessageParcel &data, MessageParcel &reply);

    ErrCode HandleGetIconById(MessageParcel &data, MessageParcel &reply);

    ErrCode HandleGetUdidByNetworkId(MessageParcel &data, MessageParcel &reply);

#ifdef BUNDLE_FRAMEWORK_DEFAULT_APP
    ErrCode HandleGetDefaultAppProxy(MessageParcel &data, MessageParcel &reply);
#endif

#ifdef BUNDLE_FRAMEWORK_APP_CONTROL
    ErrCode HandleGetAppControlProxy(MessageParcel &data, MessageParcel &reply);
#endif

    ErrCode HandleGetQuickFixManagerProxy(MessageParcel &data, MessageParcel &reply);

    ErrCode HandleSetDebugMode(MessageParcel &data, MessageParcel &reply);

    ErrCode HandleVerifySystemApi(MessageParcel &data, MessageParcel &reply);

    ErrCode HandleProcessPreload(MessageParcel &data, MessageParcel &reply);

    ErrCode HandleGetProvisionMetadata(MessageParcel &data, MessageParcel &reply);

private:
    /**
     * @brief Write a parcelabe vector objects to the proxy node.
     * @param parcelableVector Indicates the objects to be write.
     * @param reply Indicates the reply to be sent;
     * @return Returns true if objects send successfully; returns false otherwise.
     */
    template<typename T>
    bool WriteParcelableVector(std::vector<T> &parcelableVector, MessageParcel &reply);
    /**
     * @brief Write a parcelabe vector objects to ashmem.
     * @param parcelableVector Indicates the objects to be write.
     * @param ashmemName Indicates the ashmem name;
     * @param reply Indicates the reply to be sent;
     * @return Returns true if objects send successfully; returns false otherwise.
     */
    template<typename T>
    bool WriteParcelableVectorIntoAshmem(
        std::vector<T> &parcelableVector, const char *ashmemName, MessageParcel &reply);
    /**
     * @brief Allocat ashmem num.
     * @return Returns ashmem num.
     */
    int32_t AllocatAshmemNum();
    void init();

    using BundleMgrHostFunc = ErrCode (BundleMgrHost::*)(MessageParcel &, MessageParcel &);
    std::unordered_map<uint32_t, BundleMgrHostFunc> funcMap_;

    std::mutex bundleAshmemMutex_;
    int32_t ashmemNum_ = 0;
    DISALLOW_COPY_AND_MOVE(BundleMgrHost);
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // FOUNDATION_APPEXECFWK_INTERFACES_INNERKITS_APPEXECFWK_CORE_INCLUDE_BUNDLEMGR_BUNDLE_MGR_HOST_H
