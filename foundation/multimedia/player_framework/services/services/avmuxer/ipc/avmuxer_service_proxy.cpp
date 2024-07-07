/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

#include "avmuxer_service_proxy.h"
#include "media_log.h"
#include "media_errors.h"
#include "avsharedmemory_ipc.h"
#include "media_parcel.h"

namespace {
    constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, LOG_DOMAIN, "AVMuxerServiceProxy"};
}

namespace OHOS {
namespace Media {
AVMuxerServiceProxy::AVMuxerServiceProxy(const sptr<IRemoteObject> &impl)
    : IRemoteProxy<IStandardAVMuxerService>(impl)
{
    MEDIA_LOGD("0x%{public}06" PRIXPTR " Instances create", FAKE_POINTER(this));
}

AVMuxerServiceProxy::~AVMuxerServiceProxy()
{
    MEDIA_LOGD("0x%{public}06" PRIXPTR " Instances destroy", FAKE_POINTER(this));
}

int32_t AVMuxerServiceProxy::DestroyStub()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    bool token = data.WriteInterfaceToken(AVMuxerServiceProxy::GetDescriptor());
    CHECK_AND_RETURN_RET_LOG(token, MSERR_INVALID_OPERATION, "Failed to write descriptor!");

    int error = Remote()->SendRequest(DESTROY, data, reply, option);
    CHECK_AND_RETURN_RET_LOG(error == MSERR_OK, error, "Failed to call DestroyStub, error: %{public}d", error);
    return reply.ReadInt32();
}

std::vector<std::string> AVMuxerServiceProxy::GetAVMuxerFormatList()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    bool token = data.WriteInterfaceToken(AVMuxerServiceProxy::GetDescriptor());
    CHECK_AND_RETURN_RET_LOG(token, std::vector<std::string>(), "Failed to write descriptor!");

    std::vector<std::string> formatList;
    int error = Remote()->SendRequest(GET_MUXER_FORMAT_LIST, data, reply, option);
    CHECK_AND_RETURN_RET_LOG(error == MSERR_OK, formatList,
        "Failed to call GetAVMuxerFormatList, error: %{public}d", error);
    reply.ReadStringVector(&formatList);
    return formatList;
}

int32_t AVMuxerServiceProxy::SetOutput(int32_t fd, const std::string &format)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    bool token = data.WriteInterfaceToken(AVMuxerServiceProxy::GetDescriptor());
    CHECK_AND_RETURN_RET_LOG(token, MSERR_INVALID_OPERATION, "Failed to write descriptor!");

    CHECK_AND_RETURN_RET(data.WriteFileDescriptor(fd), MSERR_UNKNOWN);
    CHECK_AND_RETURN_RET(data.WriteString(format), MSERR_UNKNOWN);
    int error = Remote()->SendRequest(SET_OUTPUT, data, reply, option);
    CHECK_AND_RETURN_RET_LOG(error == MSERR_OK, error, "Failed to call SetOutput, error: %{public}d", error);
    return reply.ReadInt32();
}

int32_t AVMuxerServiceProxy::SetLocation(float latitude, float longitude)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    bool token = data.WriteInterfaceToken(AVMuxerServiceProxy::GetDescriptor());
    CHECK_AND_RETURN_RET_LOG(token, MSERR_INVALID_OPERATION, "Failed to write descriptor!");

    CHECK_AND_RETURN_RET(data.WriteFloat(latitude), MSERR_UNKNOWN);
    CHECK_AND_RETURN_RET(data.WriteFloat(longitude), MSERR_UNKNOWN);
    int error = Remote()->SendRequest(SET_LOCATION, data, reply, option);
    CHECK_AND_RETURN_RET_LOG(error == MSERR_OK, error, "Failed to call SetLocation, error: %{public}d", error);
    return reply.ReadInt32();
}

int32_t AVMuxerServiceProxy::SetRotation(int32_t rotation)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    bool token = data.WriteInterfaceToken(AVMuxerServiceProxy::GetDescriptor());
    CHECK_AND_RETURN_RET_LOG(token, MSERR_INVALID_OPERATION, "Failed to write descriptor!");

    CHECK_AND_RETURN_RET(data.WriteInt32(rotation), MSERR_UNKNOWN);
    int error = Remote()->SendRequest(SET_ORIENTATION_HINT, data, reply, option);
    CHECK_AND_RETURN_RET_LOG(error == MSERR_OK, error, "Failed to call SetRotation, error: %{public}d", error);
    return reply.ReadInt32();
}

int32_t AVMuxerServiceProxy::AddTrack(const MediaDescription &trackDesc, int32_t &trackId)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    bool token = data.WriteInterfaceToken(AVMuxerServiceProxy::GetDescriptor());
    CHECK_AND_RETURN_RET_LOG(token, MSERR_INVALID_OPERATION, "Failed to write descriptor!");

    CHECK_AND_RETURN_RET(MediaParcel::Marshalling(data, trackDesc), MSERR_UNKNOWN);
    int error = Remote()->SendRequest(ADD_TRACK, data, reply, option);
    CHECK_AND_RETURN_RET_LOG(error == MSERR_OK, error, "Failed to call AddTrack, error: %{public}d", error);
    trackId = reply.ReadInt32();
    return reply.ReadInt32();
}

int32_t AVMuxerServiceProxy::Start()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    bool token = data.WriteInterfaceToken(AVMuxerServiceProxy::GetDescriptor());
    CHECK_AND_RETURN_RET_LOG(token, MSERR_INVALID_OPERATION, "Failed to write descriptor!");

    int error = Remote()->SendRequest(START, data, reply, option);
    CHECK_AND_RETURN_RET_LOG(error == MSERR_OK, error, "Failed to call Start, error: %{public}d", error);
    return reply.ReadInt32();
}

int32_t AVMuxerServiceProxy::WriteTrackSample(std::shared_ptr<AVSharedMemory> sampleData,
    const TrackSampleInfo &sampleInfo)
{
    CHECK_AND_RETURN_RET_LOG(sampleData != nullptr, MSERR_INVALID_VAL, "sampleData is nullptr");
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    bool token = data.WriteInterfaceToken(AVMuxerServiceProxy::GetDescriptor());
    CHECK_AND_RETURN_RET_LOG(token, MSERR_INVALID_OPERATION, "Failed to write descriptor!");

    WriteAVSharedMemoryToParcel(sampleData, data);
    CHECK_AND_RETURN_RET(data.WriteInt32(sampleInfo.trackIdx), MSERR_UNKNOWN);
    CHECK_AND_RETURN_RET(data.WriteInt64(sampleInfo.timeUs), MSERR_UNKNOWN);
    CHECK_AND_RETURN_RET(data.WriteInt32(sampleInfo.size), MSERR_UNKNOWN);
    CHECK_AND_RETURN_RET(data.WriteInt32(sampleInfo.flags), MSERR_UNKNOWN);
    int error = Remote()->SendRequest(WRITE_TRACK_SAMPLE, data, reply, option);
    CHECK_AND_RETURN_RET_LOG(error == MSERR_OK, error, "Failed to call WriteTrackSample, error: %{public}d", error);
    return reply.ReadInt32();
}

int32_t AVMuxerServiceProxy::Stop()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    bool token = data.WriteInterfaceToken(AVMuxerServiceProxy::GetDescriptor());
    CHECK_AND_RETURN_RET_LOG(token, MSERR_INVALID_OPERATION, "Failed to write descriptor!");

    int error = Remote()->SendRequest(STOP, data, reply, option);
    CHECK_AND_RETURN_RET_LOG(error == MSERR_OK, error, "Failed to call Stop, error: %{public}d", error);
    return reply.ReadInt32();
}

void AVMuxerServiceProxy::Release()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    bool token = data.WriteInterfaceToken(AVMuxerServiceProxy::GetDescriptor());
    CHECK_AND_RETUR_LOG(token, "Failed to write descriptor!");

    int error = Remote()->SendRequest(RELEASE, data, reply, option);
    CHECK_AND_RETURN_LOG(error == MSERR_OK, "Failed to call Release, error: %{public}d", error);
}
}  // namespace Media
}  // namespace OHOS