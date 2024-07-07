/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include "metadata.h"
#include "medialibrary_db_const.h"
#include "fetch_result.h"

namespace OHOS {
namespace Media {
using namespace std;

Metadata::Metadata()
    : id_(FILE_ID_DEFAULT),
    uri_(URI_DEFAULT),
    filePath_(FILE_PATH_DEFAULT),
    relativePath_(FILE_RELATIVE_PATH_DEFAULT),
    mimeType_(DEFAULT_FILE_MIME_TYPE),
    mediaType_(FILE_MEDIA_TYPE_DEFAULT),
    name_(FILE_NAME_DEFAULT),
    size_(FILE_SIZE_DEFAULT),
    dateModified_(FILE_DATE_MODIFIED_DEFAULT),
    dateAdded_(FILE_DATE_ADDED_DEFAULT),
    fileExt_(FILE_EXTENSION_DEFAULT),
    parentId_(FILE_ID_DEFAULT),
    title_(FILE_TITLE_DEFAULT),
    artist_(FILE_ARTIST_DEFAULT),
    album_(FILE_ALBUM_NAME_DEFAULT),
    height_(FILE_HEIGHT_DEFAULT),
    width_(FILE_WIDTH_DEFAULT),
    duration_(FILE_DURATION_DEFAULT),
    orientation_(FILE_ORIENTATION_DEFAULT),
    dateTaken_(FILE_DATE_TAKEN_DEFAULT),
    longitude_(FILE_LONGITUDE_DEFAULT),
    latitude_(FILE_LATITUDE_DEFAULT),
    albumId_(FILE_ALBUM_ID_DEFAULT),
    albumName_(FILE_ALBUM_NAME_DEFAULT),
    recyclePath_(FILE_RECYCLE_PATH_DEFAULT)
{
    Init();
}

void Metadata::Init()
{
    memberFuncMap_[MEDIA_DATA_DB_ID] = make_pair(ResultSetDataType::TYPE_INT32, &Metadata::SetFileId);
    memberFuncMap_[MEDIA_DATA_DB_URI] = make_pair(ResultSetDataType::TYPE_STRING, &Metadata::SetUri);
    memberFuncMap_[MEDIA_DATA_DB_FILE_PATH] = make_pair(ResultSetDataType::TYPE_STRING, &Metadata::SetFilePath);
    memberFuncMap_[MEDIA_DATA_DB_RELATIVE_PATH] = make_pair(ResultSetDataType::TYPE_STRING, &Metadata::SetRelativePath);
    memberFuncMap_[MEDIA_DATA_DB_MEDIA_TYPE] = make_pair(ResultSetDataType::TYPE_INT32, &Metadata::SetFileMediaType);
    memberFuncMap_[MEDIA_DATA_DB_MIME_TYPE] = make_pair(ResultSetDataType::TYPE_STRING, &Metadata::SetFileMimeType);
    memberFuncMap_[MEDIA_DATA_DB_NAME] = make_pair(ResultSetDataType::TYPE_STRING, &Metadata::SetFileName);
    memberFuncMap_[MEDIA_DATA_DB_SIZE] = make_pair(ResultSetDataType::TYPE_INT64, &Metadata::SetFileSize);
    memberFuncMap_[MEDIA_DATA_DB_DATE_ADDED] = make_pair(ResultSetDataType::TYPE_INT64, &Metadata::SetFileDateAdded);
    memberFuncMap_[MEDIA_DATA_DB_TITLE] = make_pair(ResultSetDataType::TYPE_STRING, &Metadata::SetFileTitle);
    memberFuncMap_[MEDIA_DATA_DB_ARTIST] = make_pair(ResultSetDataType::TYPE_STRING, &Metadata::SetFileArtist);
    memberFuncMap_[MEDIA_DATA_DB_AUDIO_ALBUM] = make_pair(ResultSetDataType::TYPE_STRING, &Metadata::SetAlbum);
    memberFuncMap_[MEDIA_DATA_DB_HEIGHT] = make_pair(ResultSetDataType::TYPE_INT32, &Metadata::SetFileHeight);
    memberFuncMap_[MEDIA_DATA_DB_WIDTH] = make_pair(ResultSetDataType::TYPE_INT32, &Metadata::SetFileWidth);
    memberFuncMap_[MEDIA_DATA_DB_ORIENTATION] = make_pair(ResultSetDataType::TYPE_INT32, &Metadata::SetOrientation);
    memberFuncMap_[MEDIA_DATA_DB_DURATION] = make_pair(ResultSetDataType::TYPE_INT32, &Metadata::SetFileDuration);
    memberFuncMap_[MEDIA_DATA_DB_LONGITUDE] = make_pair(ResultSetDataType::TYPE_DOUBLE, &Metadata::SetLongitude);
    memberFuncMap_[MEDIA_DATA_DB_LATITUDE] = make_pair(ResultSetDataType::TYPE_DOUBLE, &Metadata::SetLatitude);
    memberFuncMap_[MEDIA_DATA_DB_BUCKET_NAME] = make_pair(ResultSetDataType::TYPE_STRING, &Metadata::SetAlbumName);
    memberFuncMap_[MEDIA_DATA_DB_PARENT_ID] = make_pair(ResultSetDataType::TYPE_INT32, &Metadata::SetParentId);
    memberFuncMap_[MEDIA_DATA_DB_RECYCLE_PATH] = make_pair(ResultSetDataType::TYPE_STRING, &Metadata::SetRecyclePath);
    memberFuncMap_[MEDIA_DATA_DB_DATE_TAKEN] = make_pair(ResultSetDataType::TYPE_INT64, &Metadata::SetDateTaken);
    memberFuncMap_[MEDIA_DATA_DB_DATE_MODIFIED] = make_pair(ResultSetDataType::TYPE_INT64,
        &Metadata::SetFileDateModified);
}

void Metadata::SetFileId(const VariantData &id)
{
    id_ = std::get<int32_t>(id);
}

int32_t Metadata::GetFileId() const
{
    return id_;
}

void Metadata::SetUri(const VariantData &uri)
{
    uri_ = std::get<string>(uri);
}

const std::string &Metadata::GetUri() const
{
    return uri_;
}

void Metadata::SetFilePath(const VariantData &filePath)
{
    filePath_ = std::get<string>(filePath);
}

const std::string &Metadata::GetFilePath() const
{
    return filePath_;
}

void Metadata::SetRelativePath(const VariantData &relativePath)
{
    relativePath_ = std::get<string>(relativePath);
}

const std::string &Metadata::GetRelativePath() const
{
    return relativePath_;
}

void Metadata::SetFileMimeType(const VariantData &mimeType)
{
    mimeType_ = std::get<string>(mimeType);
}

const std::string &Metadata::GetFileMimeType() const
{
    return mimeType_;
}

void Metadata::SetFileMediaType(const VariantData &mediaType)
{
    mediaType_ = static_cast<MediaType>(std::get<int32_t>(mediaType));
}

MediaType Metadata::GetFileMediaType() const
{
    return mediaType_;
}

void Metadata::SetFileName(const VariantData &name)
{
    name_ = std::get<string>(name);
}

const std::string &Metadata::GetFileName() const
{
    return name_;
}

void Metadata::SetFileSize(const VariantData &size)
{
    size_ = std::get<int64_t>(size);
}

int64_t Metadata::GetFileSize() const
{
    return size_;
}

void Metadata::SetFileDateAdded(const VariantData &dateAdded)
{
    dateAdded_ = std::get<int64_t>(dateAdded);
}

int64_t Metadata::GetFileDateAdded() const
{
    return dateAdded_;
}

void Metadata::SetFileDateModified(const VariantData &dateModified)
{
    dateModified_ = std::get<int64_t>(dateModified);
}

int64_t Metadata::GetFileDateModified() const
{
    return dateModified_;
}

void Metadata::SetFileExtension(const VariantData &fileExt)
{
    fileExt_ = std::get<string>(fileExt);
}

const std::string &Metadata::GetFileExtension() const
{
    return fileExt_;
}

void Metadata::SetFileTitle(const VariantData &title)
{
    title_ = std::get<string>(title);
}

const std::string &Metadata::GetFileTitle() const
{
    return title_;
}

void Metadata::SetFileArtist(const VariantData &artist)
{
    artist_ = std::get<string>(artist);
}

const std::string &Metadata::GetFileArtist() const
{
    return artist_;
}

void Metadata::SetAlbum(const VariantData &album)
{
    album_ = std::get<string>(album);
}

const std::string &Metadata::GetAlbum() const
{
    return album_;
}

void Metadata::SetFileHeight(const VariantData &height)
{
    height_ = std::get<int32_t>(height);
}

int32_t Metadata::GetFileHeight() const
{
    return height_;
}

void Metadata::SetFileWidth(const VariantData &width)
{
    width_ = std::get<int32_t>(width);
}

int32_t Metadata::GetFileWidth() const
{
    return width_;
}

void Metadata::SetFileDuration(const VariantData &duration)
{
    duration_ = std::get<int32_t>(duration);
}

int32_t Metadata::GetFileDuration() const
{
    return duration_;
}

void Metadata::SetOrientation(const VariantData &orientation)
{
    orientation_ = std::get<int32_t>(orientation);
}

int32_t Metadata::GetOrientation() const
{
    return orientation_;
}

void Metadata::SetAlbumId(const VariantData &albumId)
{
    albumId_ = std::get<int32_t>(albumId);
}

int32_t Metadata::GetAlbumId() const
{
    return albumId_;
}

void Metadata::SetAlbumName(const VariantData &albumName)
{
    albumName_ = std::get<string>(albumName);
}

const std::string &Metadata::GetAlbumName() const
{
    return albumName_;
}

void Metadata::SetParentId(const VariantData &parentId)
{
    parentId_ = std::get<int32_t>(parentId);
}

int32_t Metadata::GetParentId() const
{
    return parentId_;
}

void Metadata::SetRecyclePath(const VariantData &recyclePath)
{
    recyclePath_ = std::get<string>(recyclePath);
}

const std::string &Metadata::GetRecyclePath() const
{
    return recyclePath_;
}

void Metadata::SetDateTaken(const VariantData &dateTaken)
{
    dateTaken_ = std::get<int64_t>(dateTaken);
}

int64_t Metadata::GetDateTaken() const
{
    return dateTaken_;
}

void Metadata::SetLongitude(const VariantData &longitude)
{
    longitude_ = std::get<double>(longitude);
}

double Metadata::GetLongitude() const
{
    return longitude_;
}

void Metadata::SetLatitude(const VariantData &latitude)
{
    latitude_ = std::get<double>(latitude);
}

double Metadata::GetLatitude() const
{
    return latitude_;
}
} // namespace Media
} // namespace OHOS
