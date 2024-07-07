/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#include "package/package.h"
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <memory>
#include "log/log.h"
#include "package/pkg_manager.h"
#include "securec.h"

using namespace Updater;
using namespace Hpackage;
constexpr uint32_t VERIFY_FINSH_PERCENT = 100;

namespace {
int32_t GetUpgradePkgInfo(UpgradePkgInfo *upgradePackageInfo,
    std::vector<std::pair<std::string, ComponentInfo>> &files,
    const UpgradePkgInfoExt *pkgInfoExt,
    ComponentInfoExt compInfo[])
{
    upgradePackageInfo->updateFileVersion = pkgInfoExt->updateFileVersion;
    if (pkgInfoExt->softwareVersion != nullptr) {
        upgradePackageInfo->softwareVersion = pkgInfoExt->softwareVersion;
    }
    if (pkgInfoExt->productUpdateId != nullptr) {
        upgradePackageInfo->productUpdateId = pkgInfoExt->productUpdateId;
    }
    if (pkgInfoExt->descriptPackageId != nullptr) {
        upgradePackageInfo->descriptPackageId = pkgInfoExt->descriptPackageId;
    }
    if (pkgInfoExt->time != nullptr) {
        upgradePackageInfo->time = pkgInfoExt->time;
    }
    if (pkgInfoExt->date != nullptr) {
        upgradePackageInfo->date = pkgInfoExt->date;
    }
    upgradePackageInfo->pkgInfo.digestMethod = pkgInfoExt->digestMethod;
    upgradePackageInfo->pkgInfo.signMethod = pkgInfoExt->signMethod;
    upgradePackageInfo->pkgInfo.entryCount = pkgInfoExt->entryCount;
    upgradePackageInfo->pkgInfo.pkgType = PKG_PACK_TYPE_UPGRADE;
    files.resize(pkgInfoExt->entryCount);
    for (uint32_t i = 0; i < pkgInfoExt->entryCount; i++) {
        files[i].first.assign(compInfo[i].filePath);
        ComponentInfo* info = &files[i].second;
        if (memcpy_s(info->digest, sizeof(info->digest), compInfo[i].digest, sizeof(info->digest)) != EOK) {
            LOG(ERROR) << "GetUpgradePkgInfo memcpy failed";
            return PKG_NONE_MEMORY;
        }
        info->fileInfo.identity.assign(compInfo[i].componentAddr);
        info->fileInfo.unpackedSize = compInfo[i].size;
        info->fileInfo.packedSize = compInfo[i].size;
        info->fileInfo.packMethod = PKG_COMPRESS_METHOD_NONE;
        info->fileInfo.digestMethod = pkgInfoExt->digestMethod;
        info->version.assign(compInfo[i].version);
        info->id = compInfo[i].id;
        info->resType = compInfo[i].resType;
        info->type = compInfo[i].type;
        info->originalSize = compInfo[i].originalSize;
        info->compFlags = compInfo[i].flags;
    }
    return PKG_SUCCESS;
}

int32_t GetZipPkgInfo(PkgManager::PkgInfoPtr pkgInfo,
    std::vector<std::pair<std::string, ZipFileInfo>> &files,
    const UpgradePkgInfoExt *pkgInfoExt,
    ComponentInfoExt compInfo[])
{
    pkgInfo->signMethod = pkgInfoExt->signMethod;
    pkgInfo->digestMethod  = pkgInfoExt->digestMethod;
    pkgInfo->entryCount = pkgInfoExt->entryCount;
    pkgInfo->pkgType = pkgInfoExt->pkgType;
    files.resize(pkgInfoExt->entryCount);
    for (uint32_t i = 0; i < pkgInfo->entryCount; i++) {
        files[i].first.assign(compInfo[i].filePath);
        ZipFileInfo* info = &files[i].second;
        info->fileInfo.identity.assign(compInfo[i].componentAddr);
        info->fileInfo.packMethod = PKG_COMPRESS_METHOD_ZIP;
        info->fileInfo.digestMethod = PKG_DIGEST_TYPE_CRC;
    }
    return PKG_SUCCESS;
}

int32_t GetLz4PkgInfo(PkgManager::PkgInfoPtr pkgInfo,
    std::vector<std::pair<std::string, Lz4FileInfo>> &files,
    const UpgradePkgInfoExt *pkgInfoExt,
    ComponentInfoExt compInfo[])
{
    pkgInfo->signMethod = pkgInfoExt->signMethod;
    pkgInfo->digestMethod  = pkgInfoExt->digestMethod;
    pkgInfo->entryCount = pkgInfoExt->entryCount;
    pkgInfo->pkgType = PKG_PACK_TYPE_LZ4;
    files.resize(pkgInfoExt->entryCount);
    for (uint32_t i = 0; i < pkgInfoExt->entryCount; i++) {
        files[i].first.assign(compInfo[i].filePath);
        Lz4FileInfo* info = &files[i].second;
        info->fileInfo.identity.assign(compInfo[i].componentAddr);
        info->fileInfo.packMethod = PKG_COMPRESS_METHOD_LZ4;
        info->fileInfo.digestMethod = PKG_DIGEST_TYPE_CRC;
        info->compressionLevel = MID_COMPRESS_LEVEL;
        info->blockSizeID = 0;
        info->contentChecksumFlag = 0;
        info->blockIndependence = 0;
    }
    return PKG_SUCCESS;
}
}

int32_t CreatePackage(const UpgradePkgInfoExt *pkgInfoExt,
    ComponentInfoExt compInfo[],
    const char *path,
    const char *keyPath)
{
    PkgManager::PkgManagerPtr manager = PkgManager::GetPackageInstance();
    if (pkgInfoExt == nullptr || path == nullptr || keyPath == nullptr || manager == nullptr) {
        LOG(ERROR) << "Check param fail ";
        return PKG_INVALID_PARAM;
    }

    int32_t ret = PKG_SUCCESS;
    switch (pkgInfoExt->pkgType) {
        case PKG_PACK_TYPE_UPGRADE: {
            UpgradePkgInfo upgradePackageInfo;
            std::vector<std::pair<std::string, ComponentInfo>> files;
            ret = GetUpgradePkgInfo(&upgradePackageInfo, files, pkgInfoExt, compInfo);
            if (ret == PKG_SUCCESS) {
                ret = manager->CreatePackage(path, keyPath, &upgradePackageInfo.pkgInfo, files);
            }
            break;
        }
        case PKG_PACK_TYPE_ZIP:
        case PKG_PACK_TYPE_GZIP: {
            PkgInfo info;
            std::vector<std::pair<std::string, ZipFileInfo>> files;
            ret = GetZipPkgInfo(&info, files, pkgInfoExt, compInfo);
            if (ret == PKG_SUCCESS) {
                ret = manager->CreatePackage(path, keyPath, &info, files);
            }
            break;
        }
        case PKG_PACK_TYPE_LZ4: {
            PkgInfo info;
            std::vector<std::pair<std::string, Lz4FileInfo>> files;
            ret = GetLz4PkgInfo(&info, files, pkgInfoExt, compInfo);
            if (ret == PKG_SUCCESS) {
                ret = manager->CreatePackage(path, keyPath, &info, files);
            }
            break;
        }
        default:
            ret = PKG_INVALID_PARAM;
            break;
    }
    PkgManager::ReleasePackageInstance(manager);
    return ret;
}

int32_t VerifyPackage(const char *packagePath,
    const char *keyPath,
    const char *version,
    const uint8_t *digest,
    size_t size)
{
    PkgManager::PkgManagerPtr manager = PkgManager::GetPackageInstance();
    if (packagePath == nullptr || keyPath == nullptr || version == nullptr || manager == nullptr) {
        return PKG_INVALID_PARAM;
    }

    PkgBuffer digestBuffer(const_cast<uint8_t*>(digest), size);
    int32_t ret = manager->VerifyPackage(packagePath, keyPath, version, digestBuffer,
        [](int32_t result, uint32_t percent) {});
    PkgManager::ReleasePackageInstance(manager);
    return ret;
}

int32_t VerifyPackageWithCallback(const std::string &packagePath,
    const std::string &keyPath, std::function<void(int32_t result, uint32_t percent)> cb)
{
    if (packagePath.empty() || keyPath.empty() || cb == nullptr) {
        return PKG_INVALID_PARAM;
    }

    PkgManager *manager = PkgManager::GetPackageInstance();
    PkgBuffer digestBuffer {};
    std::string version {};
    int32_t ret = manager->VerifyPackage(packagePath, keyPath, version, digestBuffer, cb);
    if (ret != 0) {
        cb(ret, VERIFY_FINSH_PERCENT);
    }
    PkgManager::ReleasePackageInstance(manager);
    return ret;
}

int32_t ExtraPackageDir(const char *packagePath, [[maybe_unused]] const char *keyPath, const char *dir,
    const char *outPath)
{
    PkgManager::PkgManagerPtr manager = PkgManager::GetPackageInstance();
    if (packagePath == nullptr || outPath == nullptr || manager == nullptr) {
        LOG(ERROR) << "Check param fail ";
        return PKG_INVALID_PARAM;
    }

    std::vector<std::string> components;
    int32_t ret = manager->LoadPackageWithoutUnPack(std::string(packagePath), components);
    if (ret != PKG_SUCCESS) {
        LOG(ERROR) << "LoadPackageWithoutUnPack fail";
        PkgManager::ReleasePackageInstance(manager);
        return ret;
    }

    for (size_t i = 0; i < components.size(); i++) {
        if (dir != nullptr && components[i].compare(0, strlen(dir), dir) != 0) {
            continue;
        }
        PkgManager::StreamPtr outStream = nullptr;
        manager->CreatePkgStream(outStream, std::string(outPath) + components[i], 0, PkgStream::PkgStreamType_Write);
        if (outStream == nullptr) {
            LOG(ERROR) << "CreatePkgStream fail";
            PkgManager::ReleasePackageInstance(manager);
            return PKG_INVALID_STREAM;
        }
        manager->ExtractFile(components[i], outStream);
        manager->ClosePkgStream(outStream);
    }
    PkgManager::ReleasePackageInstance(manager);
    return PKG_SUCCESS;
}

int32_t ExtraPackageFile(const char *packagePath, [[maybe_unused]] const char *keyPath, const char *file,
    const char *outPath)
{
    PkgManager::PkgManagerPtr manager = PkgManager::GetPackageInstance();
    if (packagePath == nullptr || outPath == nullptr || file == nullptr || manager == nullptr) {
        LOG(ERROR) << "Check param fail ";
        return PKG_INVALID_PARAM;
    }

    std::vector<std::string> components;
    int32_t ret = manager->LoadPackageWithoutUnPack(std::string(packagePath), components);
    if (ret != PKG_SUCCESS) {
        LOG(ERROR) << "LoadPackageWithoutUnPack fail";
        PkgManager::ReleasePackageInstance(manager);
        return ret;
    }

    PkgManager::StreamPtr outStream = nullptr;
    manager->CreatePkgStream(outStream, std::string(outPath) + file, 0, PkgStream::PkgStreamType_Write);
    if (outStream == nullptr) {
        LOG(ERROR) << "CreatePkgStream fail";
        PkgManager::ReleasePackageInstance(manager);
        return PKG_INVALID_STREAM;
    }
    manager->ExtractFile(file, outStream);

    manager->ClosePkgStream(outStream);
    PkgManager::ReleasePackageInstance(manager);
    return PKG_SUCCESS;
}