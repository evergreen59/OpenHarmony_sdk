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

#include "hstream_capture_callback_stub.h"
#include "camera_log.h"
#include "remote_request_code.h"

namespace OHOS {
namespace CameraStandard {
int HStreamCaptureCallbackStub::OnRemoteRequest(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    int errCode = -1;

    if (data.ReadInterfaceToken() != GetDescriptor()) {
        return errCode;
    }
    switch (code) {
        case CAMERA_STREAM_CAPTURE_ON_CAPTURE_STARTED:
            errCode = HandleOnCaptureStarted(data);
            break;
        case CAMERA_STREAM_CAPTURE_ON_CAPTURE_ENDED:
            errCode = HandleOnCaptureEnded(data);
            break;
        case CAMERA_STREAM_CAPTURE_ON_CAPTURE_ERROR:
            errCode = HandleOnCaptureError(data);
            break;
        case CAMERA_STREAM_CAPTURE_ON_FRAME_SHUTTER:
            errCode = HandleOnFrameShutter(data);
            break;
        default:
            MEDIA_ERR_LOG("HStreamCaptureCallbackStub request code %{public}u not handled", code);
            errCode = IPCObjectStub::OnRemoteRequest(code, data, reply, option);
            break;
    }

    return errCode;
}

int HStreamCaptureCallbackStub::HandleOnCaptureStarted(MessageParcel& data)
{
    int32_t captureId = data.ReadInt32();

    return OnCaptureStarted(captureId);
}

int HStreamCaptureCallbackStub::HandleOnCaptureEnded(MessageParcel& data)
{
    int32_t captureId = data.ReadInt32();
    int32_t frameCount = data.ReadInt32();

    return OnCaptureEnded(captureId, frameCount);
}

int HStreamCaptureCallbackStub::HandleOnCaptureError(MessageParcel& data)
{
    int32_t captureId = data.ReadInt32();
    int32_t errorCode = data.ReadInt32();

    return OnCaptureError(captureId, errorCode);
}

int HStreamCaptureCallbackStub::HandleOnFrameShutter(MessageParcel& data)
{
    int32_t captureId = data.ReadInt32();
    uint64_t timestamp = data.ReadUint64();

    return OnFrameShutter(captureId, timestamp);
}
} // namespace CameraStandard
} // namespace OHOS

