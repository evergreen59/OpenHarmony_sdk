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

#include "hcapture_session_proxy.h"
#include "camera_log.h"
#include "remote_request_code.h"

namespace OHOS {
namespace CameraStandard {
HCaptureSessionProxy::HCaptureSessionProxy(const sptr<IRemoteObject> &impl)
    : IRemoteProxy<ICaptureSession>(impl) { }

int32_t HCaptureSessionProxy::BeginConfig()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        MEDIA_ERR_LOG("HCaptureSessionProxy BeginConfig Write interface token failed");
        return IPC_PROXY_ERR;
    }
    int error = Remote()->SendRequest(CAMERA_CAPTURE_SESSION_BEGIN_CONFIG, data, reply, option);
    if (error != ERR_NONE) {
        MEDIA_ERR_LOG("HCaptureSessionProxy BeginConfig failed, error: %{public}d", error);
    }

    return error;
}

int32_t HCaptureSessionProxy::AddInput(sptr<ICameraDeviceService> cameraDevice)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (cameraDevice == nullptr) {
        MEDIA_ERR_LOG("HCaptureSessionProxy AddInput cameraDevice is null");
        return IPC_PROXY_ERR;
    }

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        MEDIA_ERR_LOG("HCaptureSessionProxy AddInput Write interface token failed");
        return IPC_PROXY_ERR;
    }
    if (!data.WriteRemoteObject(cameraDevice->AsObject())) {
        MEDIA_ERR_LOG("HCaptureSessionProxy AddInput write cameraDevice obj failed");
        return IPC_PROXY_ERR;
    }

    int error = Remote()->SendRequest(CAMERA_CAPTURE_SESSION_ADD_INPUT, data, reply, option);
    if (error != ERR_NONE) {
        MEDIA_ERR_LOG("HCaptureSessionProxy AddInput failed, error: %{public}d", error);
    }

    return error;
}

int32_t HCaptureSessionProxy::AddOutput(StreamType streamType, sptr<IStreamCommon> stream)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (stream == nullptr) {
        MEDIA_ERR_LOG("HCaptureSessionProxy AddOutput stream is null");
        return IPC_PROXY_ERR;
    }

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        MEDIA_ERR_LOG("HCaptureSessionProxy AddOutput Write interface token failed");
        return IPC_PROXY_ERR;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(streamType))) {
        MEDIA_ERR_LOG("HCameraServiceProxy AddOutput Write stream type failed");
        return IPC_PROXY_ERR;
    }
    if (!data.WriteRemoteObject(stream->AsObject())) {
        MEDIA_ERR_LOG("HCaptureSessionProxy AddOutput write stream obj failed");
        return IPC_PROXY_ERR;
    }

    int error = Remote()->SendRequest(CAMERA_CAPTURE_SESSION_ADD_OUTPUT, data, reply, option);
    if (error != ERR_NONE) {
        MEDIA_ERR_LOG("HCaptureSessionProxy AddOutput failed, error: %{public}d", error);
    }

    return error;
}

int32_t HCaptureSessionProxy::RemoveInput(sptr<ICameraDeviceService> cameraDevice)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (cameraDevice == nullptr) {
        MEDIA_ERR_LOG("HCaptureSessionProxy RemoveInput cameraDevice is null");
        return IPC_PROXY_ERR;
    }

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        MEDIA_ERR_LOG("HCaptureSessionProxy RemoveInput Write interface token failed");
        return IPC_PROXY_ERR;
    }
    if (!data.WriteRemoteObject(cameraDevice->AsObject())) {
        MEDIA_ERR_LOG("HCaptureSessionProxy RemoveInput write cameraDevice obj failed");
        return IPC_PROXY_ERR;
    }

    int error = Remote()->SendRequest(CAMERA_CAPTURE_SESSION_REMOVE_INPUT, data, reply, option);
    if (error != ERR_NONE) {
        MEDIA_ERR_LOG("HCaptureSessionProxy RemoveInput failed, error: %{public}d", error);
    }

    return error;
}

int32_t HCaptureSessionProxy::RemoveOutput(StreamType streamType, sptr<IStreamCommon> stream)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (stream == nullptr) {
        MEDIA_ERR_LOG("HCaptureSessionProxy RemoveOutput stream is null");
        return IPC_PROXY_ERR;
    }

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        MEDIA_ERR_LOG("HCaptureSessionProxy RemoveOutput Write interface token failed");
        return IPC_PROXY_ERR;
    }
    if (!data.WriteUint32(static_cast<uint32_t>(streamType))) {
        MEDIA_ERR_LOG("HCameraServiceProxy RemoveOutput Write stream type failed");
        return IPC_PROXY_ERR;
    }
    if (!data.WriteRemoteObject(stream->AsObject())) {
        MEDIA_ERR_LOG("HCaptureSessionProxy RemoveOutput write stream obj failed");
        return IPC_PROXY_ERR;
    }

    int error = Remote()->SendRequest(CAMERA_CAPTURE_SESSION_REMOVE_OUTPUT, data, reply, option);
    if (error != ERR_NONE) {
        MEDIA_ERR_LOG("HCaptureSessionProxy RemoveOutput failed, error: %{public}d", error);
    }

    return error;
}

int32_t HCaptureSessionProxy::CommitConfig()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        MEDIA_ERR_LOG("HCaptureSessionProxy CommitConfig Write interface token failed");
        return IPC_PROXY_ERR;
    }
    int error = Remote()->SendRequest(CAMERA_CAPTURE_SESSION_COMMIT_CONFIG, data, reply, option);
    if (error != ERR_NONE) {
        MEDIA_ERR_LOG("HCaptureSessionProxy CommitConfig failed, error: %{public}d", error);
    }

    return error;
}

int32_t HCaptureSessionProxy::Start()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        MEDIA_ERR_LOG("HCaptureSessionProxy Start Write interface token failed");
        return IPC_PROXY_ERR;
    }
    int error = Remote()->SendRequest(CAMERA_CAPTURE_SESSION_START, data, reply, option);
    if (error != ERR_NONE) {
        MEDIA_ERR_LOG("HCaptureSessionProxy Start failed, error: %{public}d", error);
    }

    return error;
}

int32_t HCaptureSessionProxy::Stop()
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        MEDIA_ERR_LOG("HCaptureSessionProxy Stop Write interface token failed");
        return IPC_PROXY_ERR;
    }
    int error = Remote()->SendRequest(CAMERA_CAPTURE_SESSION_STOP, data, reply, option);
    if (error != ERR_NONE) {
        MEDIA_ERR_LOG("HCaptureSessionProxy Stop failed, error: %{public}d", error);
    }

    return error;
}

int32_t HCaptureSessionProxy::Release(pid_t pid)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        MEDIA_ERR_LOG("HCaptureSessionProxy Release Write interface token failed");
        return IPC_PROXY_ERR;
    }
    int error = Remote()->SendRequest(CAMERA_CAPTURE_SESSION_RELEASE, data, reply, option);
    if (error != ERR_NONE) {
        MEDIA_ERR_LOG("HCaptureSessionProxy Release failed, error: %{public}d", error);
    }

    return error;
}

int32_t HCaptureSessionProxy::SetCallback(sptr<ICaptureSessionCallback> &callback)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (callback == nullptr) {
        MEDIA_ERR_LOG("HCaptureSessionProxy SetCallback callback is null");
        return IPC_PROXY_ERR;
    }

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        MEDIA_ERR_LOG("HCaptureSessionProxy SetCallback Write interface token failed");
        return IPC_PROXY_ERR;
    }
    if (!data.WriteRemoteObject(callback->AsObject())) {
        MEDIA_ERR_LOG("HCaptureSessionProxy SetCallback write CaptureSessionCallback obj failed");
        return IPC_PROXY_ERR;
    }

    int error = Remote()->SendRequest(CAMERA_CAPTURE_SESSION_SET_CALLBACK, data, reply, option);
    if (error != ERR_NONE) {
        MEDIA_ERR_LOG("HCaptureSessionProxy SetCallback failed, error: %{public}d", error);
    }

    return error;
}

int32_t HCaptureSessionProxy::GetSessionState(CaptureSessionState &sessionState)
{
    MessageParcel data;
    MessageParcel reply;
    MessageOption option;

    if (!data.WriteInterfaceToken(GetDescriptor())) {
        MEDIA_ERR_LOG("HCaptureSessionProxy GetSessionState Write interface token failed");
        return IPC_PROXY_ERR;
    }

    int error = Remote()->SendRequest(CAMERA_CAPTURE_GET_SESSION_STATE, data, reply, option);
    if (error != ERR_NONE) {
        MEDIA_ERR_LOG("HCaptureSessionProxy GetSessionState failed, error: %{public}d", error);
    }
    sessionState = static_cast<CaptureSessionState>(reply.ReadUint32());
    return error;
}
} // namespace CameraStandard
} // namespace OHOS
