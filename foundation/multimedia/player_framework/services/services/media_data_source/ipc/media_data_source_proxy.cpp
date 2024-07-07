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

#include "media_data_source_proxy.h"
#include "media_log.h"
#include "media_errors.h"
#include "avsharedmemory_ipc.h"

namespace {
constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, LOG_DOMAIN, "MediaDataSourceProxy"};
}

namespace OHOS {
namespace Media {
MediaDataCallback::MediaDataCallback(const sptr<IStandardMediaDataSource> &ipcProxy)
    : callbackProxy_(ipcProxy)
{
    MEDIA_LOGD("0x%{public}06" PRIXPTR " Instances create", FAKE_POINTER(this));
}

MediaDataCallback::~MediaDataCallback()
{
    MEDIA_LOGD("0x%{public}06" PRIXPTR " Instances create", FAKE_POINTER(this));
}

int32_t MediaDataCallback::ReadAt(uint32_t length, const std::shared_ptr<AVSharedMemory> &mem)
{
    CHECK_AND_RETURN_RET_LOG(callbackProxy_ != nullptr, SOURCE_ERROR_IO, "callbackProxy_ is nullptr");
    return callbackProxy_->ReadAt(length, mem);
}

int32_t MediaDataCallback::ReadAt(int64_t pos, uint32_t length, const std::shared_ptr<AVSharedMemory> &mem)
{
    CHECK_AND_RETURN_RET_LOG(callbackProxy_ != nullptr, SOURCE_ERROR_IO, "callbackProxy_ is nullptr");
    return callbackProxy_->ReadAt(pos, length, mem);
}

int32_t MediaDataCallback::GetSize(int64_t &size)
{
    CHECK_AND_RETURN_RET_LOG(callbackProxy_ != nullptr, MSERR_INVALID_OPERATION, "callbackProxy_ is nullptr");
    return callbackProxy_->GetSize(size);
}

MediaDataSourceProxy::MediaDataSourceProxy(const sptr<IRemoteObject> &impl)
    : IRemoteProxy<IStandardMediaDataSource>(impl)
{
    MEDIA_LOGD("0x%{public}06" PRIXPTR " Instances create", FAKE_POINTER(this));
}

MediaDataSourceProxy::~MediaDataSourceProxy()
{
    MEDIA_LOGD("0x%{public}06" PRIXPTR " Instances destroy", FAKE_POINTER(this));
}

int32_t MediaDataSourceProxy::ReadAt(int64_t pos, uint32_t length, const std::shared_ptr<AVSharedMemory> &mem)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    bool token = data.WriteInterfaceToken(MediaDataSourceProxy::GetDescriptor());
    CHECK_AND_RETURN_RET_LOG(token, MSERR_INVALID_OPERATION, "Failed to write descriptor!");

    data.WriteInt64(pos);
    data.WriteUint32(length);
    CHECK_AND_RETURN_RET_LOG(WriteAVSharedMemoryToParcel(mem, data) == MSERR_OK, 0, "write parcel failed");
    int error = Remote()->SendRequest(ListenerMsg::READ_AT_POS, data, reply, option);
    CHECK_AND_RETURN_RET_LOG(error == MSERR_OK, 0, "ReadAt failed, error: %{public}d", error);

    return reply.ReadInt32();
}

int32_t MediaDataSourceProxy::ReadAt(uint32_t length, const std::shared_ptr<AVSharedMemory> &mem)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    bool token = data.WriteInterfaceToken(MediaDataSourceProxy::GetDescriptor());
    CHECK_AND_RETURN_RET_LOG(token, MSERR_INVALID_OPERATION, "Failed to write descriptor!");

    data.WriteUint32(length);
    CHECK_AND_RETURN_RET_LOG(WriteAVSharedMemoryToParcel(mem, data) == MSERR_OK, 0, "write parcel failed");
    int error = Remote()->SendRequest(ListenerMsg::READ_AT, data, reply, option);
    CHECK_AND_RETURN_RET_LOG(error == MSERR_OK, 0, "ReadAt failed, error: %{public}d", error);

    return reply.ReadInt32();
}

int32_t MediaDataSourceProxy::GetSize(int64_t &size)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);

    bool token = data.WriteInterfaceToken(MediaDataSourceProxy::GetDescriptor());
    CHECK_AND_RETURN_RET_LOG(token, MSERR_INVALID_OPERATION, "Failed to write descriptor!");

    int error = Remote()->SendRequest(ListenerMsg::GET_SIZE, data, reply, option);
    CHECK_AND_RETURN_RET_LOG(error == MSERR_OK, -1, "GetSize failed, error: %{public}d", error);

    size = reply.ReadInt64();
    return reply.ReadInt32();
}
} // namespace Media
} // namespace OHOS