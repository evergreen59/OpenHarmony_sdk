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

#include "ipc/installd_host.h"

#include "app_log_wrapper.h"
#include "appexecfwk_errors.h"
#include "bundle_constants.h"
#include "parcel_macro.h"
#include "string_ex.h"

namespace OHOS {
namespace AppExecFwk {
InstalldHost::InstalldHost()
{
    init();
    APP_LOGI("installd host instance is created");
}

InstalldHost::~InstalldHost()
{
    APP_LOGI("installd host instance is destroyed");
}

void InstalldHost::init()
{
    funcMap_.emplace(IInstalld::Message::CREATE_BUNDLE_DIR, &InstalldHost::HandleCreateBundleDir);
    funcMap_.emplace(IInstalld::Message::EXTRACT_MODULE_FILES, &InstalldHost::HandleExtractModuleFiles);
    funcMap_.emplace(IInstalld::Message::RENAME_MODULE_DIR, &InstalldHost::HandleRenameModuleDir);
    funcMap_.emplace(IInstalld::Message::CREATE_BUNDLE_DATA_DIR, &InstalldHost::HandleCreateBundleDataDir);
    funcMap_.emplace(IInstalld::Message::REMOVE_BUNDLE_DATA_DIR, &InstalldHost::HandleRemoveBundleDataDir);
    funcMap_.emplace(IInstalld::Message::REMOVE_MODULE_DATA_DIR, &InstalldHost::HandleRemoveModuleDataDir);
    funcMap_.emplace(IInstalld::Message::CLEAN_BUNDLE_DATA_DIR, &InstalldHost::HandleCleanBundleDataDir);
    funcMap_.emplace(IInstalld::Message::SET_DIR_APL, &InstalldHost::HandleSetDirApl);
    funcMap_.emplace(IInstalld::Message::REMOVE_DIR, &InstalldHost::HandleRemoveDir);
    funcMap_.emplace(IInstalld::Message::GET_BUNDLE_STATS, &InstalldHost::HandleGetBundleStats);
    funcMap_.emplace(IInstalld::Message::GET_BUNDLE_CACHE_PATH, &InstalldHost::HandleGetBundleCachePath);
    funcMap_.emplace(IInstalld::Message::SCAN_DIR, &InstalldHost::HandleScanDir);
    funcMap_.emplace(IInstalld::Message::MOVE_FILE, &InstalldHost::HandleMoveFile);
    funcMap_.emplace(IInstalld::Message::COPY_FILE, &InstalldHost::HandleCopyFile);
    funcMap_.emplace(IInstalld::Message::MKDIR, &InstalldHost::HandleMkdir);
    funcMap_.emplace(IInstalld::Message::GET_FILE_STAT, &InstalldHost::HandleGetFileStat);
    funcMap_.emplace(IInstalld::Message::EXTRACT_DIFF_FILES, &InstalldHost::HandleExtractDiffFiles);
    funcMap_.emplace(IInstalld::Message::APPLY_DIFF_PATCH, &InstalldHost::HandleApplyDiffPatch);
    funcMap_.emplace(IInstalld::Message::IS_EXIST_DIR, &InstalldHost::HandleIsExistDir);
    funcMap_.emplace(IInstalld::Message::IS_DIR_EMPTY, &InstalldHost::HandleIsDirEmpty);
    funcMap_.emplace(IInstalld::Message::OBTAIN_QUICK_FIX_DIR, &InstalldHost::HandObtainQuickFixFileDir);
    funcMap_.emplace(IInstalld::Message::COPY_FILES, &InstalldHost::HandCopyFiles);
    funcMap_.emplace(IInstalld::Message::EXTRACT_FILES, &InstalldHost::HandleExtractFiles);
}

int InstalldHost::OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    APP_LOGD(
        "installd host receives message from client, code = %{public}d, flags = %{public}d", code, option.GetFlags());
    std::u16string descripter = InstalldHost::GetDescriptor();
    std::u16string remoteDescripter = data.ReadInterfaceToken();
    if (descripter != remoteDescripter) {
        APP_LOGE("installd host fail to write reply message due to the reply is nullptr");
        return OHOS::ERR_APPEXECFWK_PARCEL_ERROR;
    }
    bool result = true;
    APP_LOGD("funcMap_ size is %{public}d", static_cast<int32_t>(funcMap_.size()));
    if (funcMap_.find(code) != funcMap_.end() && funcMap_[code] != nullptr) {
        result = (this->*funcMap_[code])(data, reply);
    } else {
        APP_LOGW("installd host receives unknown code, code = %{public}u", code);
        return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
    }
    APP_LOGD("installd host finish to process message from client");
    return result ? NO_ERROR : OHOS::ERR_APPEXECFWK_PARCEL_ERROR;
}

bool InstalldHost::HandleCreateBundleDir(MessageParcel &data, MessageParcel &reply)
{
    std::string bundleDir = Str16ToStr8(data.ReadString16());
    APP_LOGI("bundleName %{public}s", bundleDir.c_str());
    ErrCode result = CreateBundleDir(bundleDir);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, reply, result);
    return true;
}

bool InstalldHost::HandleExtractModuleFiles(MessageParcel &data, MessageParcel &reply)
{
    std::string srcModulePath = Str16ToStr8(data.ReadString16());
    std::string targetPath = Str16ToStr8(data.ReadString16());
    std::string targetSoPath = Str16ToStr8(data.ReadString16());
    std::string cpuAbi = Str16ToStr8(data.ReadString16());
    APP_LOGI("extract module %{private}s", targetPath.c_str());
    ErrCode result = ExtractModuleFiles(srcModulePath, targetPath, targetSoPath, cpuAbi);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, reply, result);
    return true;
}

bool InstalldHost::HandleExtractFiles(MessageParcel &data, MessageParcel &reply)
{
    std::unique_ptr<ExtractParam> info(data.ReadParcelable<ExtractParam>());
    if (info == nullptr) {
        APP_LOGE("readParcelableInfo failed");
        return ERR_APPEXECFWK_INSTALL_INSTALLD_SERVICE_ERROR;
    }

    ErrCode result = ExtractFiles(*info);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, reply, result);
    return true;
}

bool InstalldHost::HandleRenameModuleDir(MessageParcel &data, MessageParcel &reply)
{
    std::string oldPath = Str16ToStr8(data.ReadString16());
    std::string newPath = Str16ToStr8(data.ReadString16());
    APP_LOGI("rename moduleDir %{private}s", oldPath.c_str());
    ErrCode result = RenameModuleDir(oldPath, newPath);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, reply, result);
    return true;
}

bool InstalldHost::HandleCreateBundleDataDir(MessageParcel &data, MessageParcel &reply)
{
    std::string bundleName = Str16ToStr8(data.ReadString16());
    int userid = data.ReadInt32();
    int uid = data.ReadInt32();
    int gid = data.ReadInt32();
    std::string apl = Str16ToStr8(data.ReadString16());
    ErrCode result = CreateBundleDataDir(bundleName, userid, uid, gid, apl);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, reply, result);
    return true;
}

bool InstalldHost::HandleRemoveBundleDataDir(MessageParcel &data, MessageParcel &reply)
{
    std::string bundleName = Str16ToStr8(data.ReadString16());
    int userid = data.ReadInt32();
    ErrCode result = RemoveBundleDataDir(bundleName, userid);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, reply, result);
    return true;
}

bool InstalldHost::HandleRemoveModuleDataDir(MessageParcel &data, MessageParcel &reply)
{
    std::string moduleNmae = Str16ToStr8(data.ReadString16());
    int userid = data.ReadInt32();
    ErrCode result = RemoveModuleDataDir(moduleNmae, userid);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, reply, result);
    return true;
}

bool InstalldHost::HandleRemoveDir(MessageParcel &data, MessageParcel &reply)
{
    std::string removedDir = Str16ToStr8(data.ReadString16());
    ErrCode result = RemoveDir(removedDir);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, reply, result);
    return true;
}

bool InstalldHost::HandleCleanBundleDataDir(MessageParcel &data, MessageParcel &reply)
{
    std::string bundleDir = Str16ToStr8(data.ReadString16());
    ErrCode result = CleanBundleDataDir(bundleDir);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, reply, result);
    return true;
}

bool InstalldHost::HandleGetBundleStats(MessageParcel &data, MessageParcel &reply)
{
    std::string bundleName = Str16ToStr8(data.ReadString16());
    int32_t userId = data.ReadInt32();
    std::vector<int64_t> bundleStats;
    ErrCode result = GetBundleStats(bundleName, userId, bundleStats);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, reply, result);
    if (!reply.WriteInt64Vector(bundleStats)) {
        APP_LOGE("HandleGetBundleStats write failed");
        return false;
    }
    return true;
}

bool InstalldHost::HandleSetDirApl(MessageParcel &data, MessageParcel &reply)
{
    std::string dataDir = Str16ToStr8(data.ReadString16());
    std::string bundleName = Str16ToStr8(data.ReadString16());
    std::string apl = Str16ToStr8(data.ReadString16());
    ErrCode result = SetDirApl(dataDir, bundleName, apl);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, reply, result);
    return true;
}

bool InstalldHost::HandleGetBundleCachePath(MessageParcel &data, MessageParcel &reply)
{
    std::string dir = Str16ToStr8(data.ReadString16());
    std::vector<std::string> cachePath;
    ErrCode result = GetBundleCachePath(dir, cachePath);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, reply, result);
    if (!reply.WriteStringVector(cachePath)) {
        APP_LOGE("fail to GetBundleCachePath from reply");
        return false;
    }
    return true;
}

bool InstalldHost::HandleScanDir(MessageParcel &data, MessageParcel &reply)
{
    std::string dir = Str16ToStr8(data.ReadString16());
    ScanMode scanMode = static_cast<ScanMode>(data.ReadInt32());
    ResultMode resultMode = static_cast<ResultMode>(data.ReadInt32());
    std::vector<std::string> paths;
    ErrCode result = ScanDir(dir, scanMode, resultMode, paths);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, reply, result);
    if (!reply.WriteStringVector(paths)) {
        APP_LOGE("fail to Scan from reply");
        return false;
    }

    return true;
}

bool InstalldHost::HandleMoveFile(MessageParcel &data, MessageParcel &reply)
{
    std::string oldPath = Str16ToStr8(data.ReadString16());
    std::string newPath = Str16ToStr8(data.ReadString16());
    ErrCode result = MoveFile(oldPath, newPath);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, reply, result);
    return true;
}

bool InstalldHost::HandleCopyFile(MessageParcel &data, MessageParcel &reply)
{
    std::string oldPath = Str16ToStr8(data.ReadString16());
    std::string newPath = Str16ToStr8(data.ReadString16());
    ErrCode result = CopyFile(oldPath, newPath);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, reply, result);
    return true;
}

bool InstalldHost::HandleMkdir(MessageParcel &data, MessageParcel &reply)
{
    std::string dir = Str16ToStr8(data.ReadString16());
    int32_t mode = data.ReadInt32();
    int32_t uid = data.ReadInt32();
    int32_t gid = data.ReadInt32();
    ErrCode result = Mkdir(dir, mode, uid, gid);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, reply, result);
    return true;
}

bool InstalldHost::HandleGetFileStat(MessageParcel &data, MessageParcel &reply)
{
    std::string file = Str16ToStr8(data.ReadString16());
    FileStat fileStat;
    ErrCode result = GetFileStat(file, fileStat);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, reply, result);
    if (!reply.WriteParcelable(&fileStat)) {
        APP_LOGE("fail to GetFileStat from reply");
        return false;
    }

    return true;
}

bool InstalldHost::HandleExtractDiffFiles(MessageParcel &data, MessageParcel &reply)
{
    std::string filePath = Str16ToStr8(data.ReadString16());
    std::string targetPath = Str16ToStr8(data.ReadString16());
    std::string cpuAbi = Str16ToStr8(data.ReadString16());
    ErrCode result = ExtractDiffFiles(filePath, targetPath, cpuAbi);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, reply, result);
    return true;
}

bool InstalldHost::HandleApplyDiffPatch(MessageParcel &data, MessageParcel &reply)
{
    std::string oldSoPath = Str16ToStr8(data.ReadString16());
    std::string diffFilePath = Str16ToStr8(data.ReadString16());
    std::string newSoPath = Str16ToStr8(data.ReadString16());
    ErrCode result = ApplyDiffPatch(oldSoPath, diffFilePath, newSoPath);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, reply, result);
    return true;
}

bool InstalldHost::HandleIsExistDir(MessageParcel &data, MessageParcel &reply)
{
    std::string path = Str16ToStr8(data.ReadString16());
    bool isExist = false;
    ErrCode result = IsExistDir(path, isExist);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, reply, result);
    if (!reply.WriteBool(isExist)) {
        APP_LOGE("fail to IsExistDir from reply");
        return false;
    }
    return true;
}

bool InstalldHost::HandleIsDirEmpty(MessageParcel &data, MessageParcel &reply)
{
    std::string dir = Str16ToStr8(data.ReadString16());
    bool isDirEmpty = false;
    ErrCode result = IsDirEmpty(dir, isDirEmpty);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, reply, result);
    if (!reply.WriteBool(isDirEmpty)) {
        APP_LOGE("write isDirEmpty failed");
        return false;
    }
    return true;
}

bool InstalldHost::HandObtainQuickFixFileDir(MessageParcel &data, MessageParcel &reply)
{
    std::string dir = Str16ToStr8(data.ReadString16());
    std::vector<std::string> dirVec;
    ErrCode result = ObtainQuickFixFileDir(dir, dirVec);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, reply, result);
    if ((result == ERR_OK) && !reply.WriteStringVector(dirVec)) {
        APP_LOGE("fail to obtain quick fix file dir from reply");
        return false;
    }
    return true;
}

bool InstalldHost::HandCopyFiles(MessageParcel &data, MessageParcel &reply)
{
    std::string sourceDir = Str16ToStr8(data.ReadString16());
    std::string destinationDir = Str16ToStr8(data.ReadString16());

    ErrCode result = CopyFiles(sourceDir, destinationDir);
    WRITE_PARCEL_AND_RETURN_FALSE_IF_FAIL(Int32, reply, result);
    return true;
}
}  // namespace AppExecFwk
}  // namespace OHOS
