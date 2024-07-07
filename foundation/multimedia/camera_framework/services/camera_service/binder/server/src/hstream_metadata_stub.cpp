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

#include "hstream_metadata_stub.h"
#include "camera_log.h"
#include "camera_util.h"
#include "remote_request_code.h"

namespace OHOS {
namespace CameraStandard {
int HStreamMetadataStub::OnRemoteRequest(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    DisableJeMalloc();
    int errCode = -1;

    if (data.ReadInterfaceToken() != GetDescriptor()) {
        return errCode;
    }
    switch (code) {
        case CAMERA_STREAM_META_START:
            errCode = Start();
            break;
        case CAMERA_STREAM_META_STOP:
            errCode = Stop();
            break;
        case CAMERA_STREAM_META_RELEASE:
            errCode = Release();
            break;
        default:
            MEDIA_ERR_LOG("HStreamMetadataStub request code %{public}u not handled", code);
            errCode = IPCObjectStub::OnRemoteRequest(code, data, reply, option);
            break;
    }

    return errCode;
}
} // namespace CameraStandard
} // namespace OHOS
