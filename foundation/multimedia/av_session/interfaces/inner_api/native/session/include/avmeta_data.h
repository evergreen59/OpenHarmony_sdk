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
#ifndef OHOS_AVMETA_DATA_H
#define OHOS_AVMETA_DATA_H

#include <bitset>
#include <memory>
#include <string>
#include <map>

#include "parcel.h"
#include "avsession_pixel_map.h"

#if !defined(WINDOWS_PLATFORM) and !defined(MAC_PLATFORM) and !defined(IOS_PLATFORM)
#include <malloc.h>
#endif

namespace OHOS::AVSession {
class AVMetaData : public Parcelable {
public:
    static constexpr std::int64_t DURATION_ALWAYS_PLAY = -1;
    enum {
        META_KEY_ASSET_ID = 0,
        META_KEY_TITLE = 1,
        META_KEY_ARTIST = 2,
        META_KEY_AUTHOR = 3,
        META_KEY_ALBUM = 4,
        META_KEY_WRITER = 5,
        META_KEY_COMPOSER = 6,
        META_KEY_DURATION = 7,
        META_KEY_MEDIA_IMAGE = 8,
        META_KEY_MEDIA_IMAGE_URI = 9,
        META_KEY_PUBLISH_DATE = 10,
        META_KEY_SUBTITLE = 11,
        META_KEY_DESCRIPTION = 12,
        META_KEY_LYRIC = 13,
        META_KEY_PREVIOUS_ASSET_ID = 14,
        META_KEY_NEXT_ASSET_ID = 15,
        META_KEY_MAX = 16
    };

    using MetaMaskType = std::bitset<META_KEY_MAX>;

    AVMetaData() = default;
    ~AVMetaData() = default;

    static AVMetaData* Unmarshalling(Parcel& data);
    bool Marshalling(Parcel& parcel) const override;

    void SetAssetId(const std::string& assetId);
    std::string GetAssetId() const;

    void SetTitle(const std::string& title);
    std::string GetTitle() const;

    void SetArtist(const std::string& artist);
    std::string GetArtist() const;

    void SetAuthor(const std::string& author);
    std::string GetAuthor() const;

    void SetAlbum(const std::string& album);
    std::string GetAlbum() const;

    void SetWriter(const std::string& writer);
    std::string GetWriter() const;

    void SetComposer(const std::string& composer);
    std::string GetComposer() const;

    void SetDuration(int64_t duration);
    int64_t GetDuration() const;

    void SetMediaImage(const std::shared_ptr<AVSessionPixelMap>& mediaImage);
    std::shared_ptr<AVSessionPixelMap> GetMediaImage() const;

    void SetMediaImageUri(const std::string& mediaImageUri);
    std::string GetMediaImageUri() const;

    void SetPublishDate(double date);
    double GetPublishDate() const;

    void SetSubTitle(const std::string& subTitle);
    std::string GetSubTitle() const;

    void SetDescription(const std::string& description);
    std::string GetDescription() const;

    void SetLyric(const std::string& lyric);
    std::string GetLyric() const;

    void SetPreviousAssetId(const std::string& assetId);
    std::string GetPreviousAssetId() const;

    void SetNextAssetId(const std::string& assetId);
    std::string GetNextAssetId() const;

    void Reset();

    MetaMaskType GetMetaMask() const;

    bool CopyToByMask(MetaMaskType& mask, AVMetaData& metaOut) const;
    bool CopyFrom(const AVMetaData& metaIn);

    bool IsValid() const;

    const static inline std::vector<int32_t> localCapability {
        META_KEY_ASSET_ID,
        META_KEY_TITLE,
        META_KEY_ARTIST,
        META_KEY_AUTHOR,
        META_KEY_ALBUM,
        META_KEY_WRITER,
        META_KEY_COMPOSER,
        META_KEY_DURATION,
        META_KEY_MEDIA_IMAGE,
        META_KEY_MEDIA_IMAGE_URI,
        META_KEY_PUBLISH_DATE,
        META_KEY_SUBTITLE,
        META_KEY_DESCRIPTION,
        META_KEY_LYRIC,
        META_KEY_PREVIOUS_ASSET_ID,
        META_KEY_NEXT_ASSET_ID,
    };

private:
    MetaMaskType metaMask_;

    std::string assetId_ = "";
    std::string title_ = "";
    std::string artist_ = "";
    std::string author_ = "";
    std::string album_ = "";
    std::string writer_ = "";
    std::string composer_ = "";
    int64_t duration_ = 0;
    std::shared_ptr<AVSessionPixelMap> mediaImage_ = nullptr;
    std::string mediaImageUri_ = "";
    double publishDate_ = 0;
    std::string subTitle_ = "";
    std::string description_ = "";
    std::string lyric_ = "";
    std::string previousAssetId_ = "";
    std::string nextAssetId_ = "";

    static void CloneAssetId(const AVMetaData& from, AVMetaData& to);
    static void CloneTitle(const AVMetaData& from, AVMetaData& to);
    static void CloneArtist(const AVMetaData& from, AVMetaData& to);
    static void CloneAuthor(const AVMetaData& from, AVMetaData& to);
    static void CloneAlbum(const AVMetaData& from, AVMetaData& to);
    static void CloneWriter(const AVMetaData& from, AVMetaData& to);
    static void CloneComposer(const AVMetaData& from, AVMetaData& to);
    static void CloneDuration(const AVMetaData& from, AVMetaData& to);
    static void CloneMediaImage(const AVMetaData& from, AVMetaData& to);
    static void CloneMediaImageUri(const AVMetaData& from, AVMetaData& to);
    static void ClonePublishData(const AVMetaData& from, AVMetaData& to);
    static void CloneSubTitle(const AVMetaData& from, AVMetaData& to);
    static void CloneDescription(const AVMetaData& from, AVMetaData& to);
    static void CloneLyric(const AVMetaData& from, AVMetaData& to);
    static void ClonePreviousAssetId(const AVMetaData& from, AVMetaData& to);
    static void CloneNextAssetId(const AVMetaData& from, AVMetaData& to);

    using CloneActionType = void(*)(const AVMetaData& from, AVMetaData& to);
    static inline CloneActionType cloneActions[META_KEY_MAX] = {
        [META_KEY_ASSET_ID] = &AVMetaData::CloneAssetId,
        [META_KEY_TITLE] = &AVMetaData::CloneTitle,
        [META_KEY_ARTIST] = &AVMetaData::CloneArtist,
        [META_KEY_AUTHOR] = &AVMetaData::CloneAuthor,
        [META_KEY_ALBUM] = &AVMetaData::CloneAlbum,
        [META_KEY_WRITER] = &AVMetaData::CloneWriter,
        [META_KEY_COMPOSER] = &AVMetaData::CloneComposer,
        [META_KEY_DURATION] = &AVMetaData::CloneDuration,
        [META_KEY_MEDIA_IMAGE] = &AVMetaData::CloneMediaImage,
        [META_KEY_MEDIA_IMAGE_URI] = &AVMetaData::CloneMediaImageUri,
        [META_KEY_PUBLISH_DATE] = &AVMetaData::ClonePublishData,
        [META_KEY_SUBTITLE] = &AVMetaData::CloneSubTitle,
        [META_KEY_DESCRIPTION] = &AVMetaData::CloneDescription,
        [META_KEY_LYRIC] = &AVMetaData::CloneLyric,
        [META_KEY_PREVIOUS_ASSET_ID] = &AVMetaData::ClonePreviousAssetId,
        [META_KEY_NEXT_ASSET_ID] = &AVMetaData::CloneNextAssetId,
    };
};
} // namespace OHOS::AVSession
#endif // OHOS_AVMETA_DATA_H