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

#ifndef OHOS_CAMERA_H_CAPTURE_SESSION_H
#define OHOS_CAMERA_H_CAPTURE_SESSION_H

#include "accesstoken_kit.h"
#include "state_customized_cbk.h"
#include "hcamera_device.h"
#include "hcapture_session_stub.h"
#include "hstream_capture.h"
#include "hstream_metadata.h"
#include "hstream_repeat.h"
#include "perm_state_change_callback_customize.h"
#include "privacy_kit.h"
#include "v1_0/istream_operator_callback.h"
#include "v1_0/istream_operator.h"

#include <refbase.h>
#include <iostream>

namespace OHOS {
namespace CameraStandard {
using namespace OHOS::HDI::Camera::V1_0;
class StreamOperatorCallback;
class PermissionStatusChangeCb;
class CameraUseStateChangeCb;

static const int32_t STREAMID_BEGIN = 1;
static const int32_t STREAM_NOT_FOUNT = -1;

class HCaptureSession : public HCaptureSessionStub {
public:
    HCaptureSession(sptr<HCameraHostManager> cameraHostManager,
        sptr<StreamOperatorCallback> streamOperatorCb, const uint32_t callingTokenId);
    ~HCaptureSession();

    int32_t BeginConfig() override;
    int32_t CommitConfig() override;

    int32_t AddInput(sptr<ICameraDeviceService> cameraDevice) override;
    int32_t AddOutput(StreamType streamType, sptr<IStreamCommon> stream) override;

    int32_t RemoveInput(sptr<ICameraDeviceService> cameraDevice) override;
    int32_t RemoveOutput(StreamType streamType, sptr<IStreamCommon> stream) override;

    int32_t Start() override;
    int32_t Stop() override;
    int32_t Release(pid_t pid) override;
    int32_t ReleaseInner();
    static void DestroyStubObjectForPid(pid_t pid);
    int32_t SetCallback(sptr<ICaptureSessionCallback> &callback) override;

    int32_t GetSessionState(CaptureSessionState &sessionState) override;

    friend class StreamOperatorCallback;
    static void dumpSessions(std::string& dumpString);
    void dumpSessionInfo(std::string& dumpString);
    static void CameraSessionSummary(std::string& dumpString);

private:
    int32_t ValidateSessionInputs();
    int32_t ValidateSessionOutputs();
    int32_t AddOutputStream(sptr<HStreamCommon> stream);
    int32_t FindRepeatStream(sptr<HStreamCommon> stream);
    int32_t RemoveOutputStream(sptr<HStreamCommon> stream);
    int32_t GetCameraDevice(sptr<HCameraDevice> &device);
    int32_t HandleCaptureOuputsConfig(sptr<HCameraDevice> &device);
    int32_t CreateAndCommitStreams(sptr<HCameraDevice> &device,
	                               std::shared_ptr<OHOS::Camera::CameraMetadata> &deviceSettings,
                                   std::vector<StreamInfo> &streamInfos);
    int32_t CheckAndCommitStreams(sptr<HCameraDevice> &device,
	                              std::shared_ptr<OHOS::Camera::CameraMetadata> &deviceSettings,
                                  std::vector<StreamInfo> &allStreamInfos,
                                  std::vector<StreamInfo> &newStreamInfos);
    int32_t GetCurrentStreamInfos(sptr<HCameraDevice> &device,
	                              std::shared_ptr<OHOS::Camera::CameraMetadata> &deviceSettings,
                                  std::vector<StreamInfo> &streamInfos);
    void UpdateSessionConfig(sptr<HCameraDevice> &device);
    void DeleteReleasedStream();
    void RestorePreviousState(sptr<HCameraDevice> &device, bool isCreateReleaseStreams);
    void ReleaseStreams();
    void ClearCaptureSession(pid_t pid);
    void RegisterPermissionCallback(const uint32_t callingTokenId, const std::string permissionName);
    void UnregisterPermissionCallback(const uint32_t callingTokenId);
    void StartUsingPermissionCallback(const uint32_t callingTokenId, const std::string permissionName);
    void StopUsingPermissionCallback(const uint32_t callingTokenId, const std::string permissionName);

    std::string GetSessionState();

    CaptureSessionState curState_ = CaptureSessionState::SESSION_INIT;
    CaptureSessionState prevState_ = CaptureSessionState::SESSION_INIT;
    sptr<HCameraDevice> cameraDevice_;
    std::vector<sptr<HStreamCommon>> repeatStreams_;
    std::vector<sptr<HStreamCommon>> captureStreams_;
    std::vector<sptr<HStreamCommon>> metadataStreams_;
    std::vector<sptr<HStreamCommon>> streams_;
    std::vector<sptr<HStreamCommon>> tempStreams_;
    std::vector<sptr<HCameraDevice>> tempCameraDevices_;
    std::vector<int32_t> deletedStreamIds_;
    sptr<HCameraHostManager> cameraHostManager_;
    sptr<StreamOperatorCallback> streamOperatorCallback_;
    sptr<ICaptureSessionCallback> sessionCallback_;
    int32_t streamId_ = STREAMID_BEGIN;
    std::mutex streamsLock_;
    pid_t pid_;
    int32_t uid_;
    uint32_t callerToken_;
    std::shared_ptr<PermissionStatusChangeCb> callbackPtr_;
    std::shared_ptr<CameraUseStateChangeCb> cameraUseCallbackPtr_;
};

class PermissionStatusChangeCb : public Security::AccessToken::PermStateChangeCallbackCustomize {
public:
    explicit PermissionStatusChangeCb(const Security::AccessToken::PermStateChangeScope &scopeInfo)
        : PermStateChangeCallbackCustomize(scopeInfo) {}
    ~PermissionStatusChangeCb();
    void PermStateChangeCallback(Security::AccessToken::PermStateChangeInfo& result) override;
    void SetCaptureSession(sptr<HCaptureSession> captureSession);

private:
    wptr<HCaptureSession> captureSession_;
};

class CameraUseStateChangeCb : public Security::AccessToken::StateCustomizedCbk {
public:
    CameraUseStateChangeCb() {}
    ~CameraUseStateChangeCb();
    void StateChangeNotify(Security::AccessToken::AccessTokenID tokenId, bool isShowing) override;
    void SetCaptureSession(sptr<HCaptureSession> captureSession);

private:
    wptr<HCaptureSession> captureSession_;
};

class StreamOperatorCallback : public IStreamOperatorCallback {
public:
    StreamOperatorCallback() = default;
    explicit StreamOperatorCallback(sptr<HCaptureSession> session);
    virtual ~StreamOperatorCallback();

    int32_t OnCaptureStarted(int32_t captureId, const std::vector<int32_t>& streamIds) override;
    int32_t OnCaptureEnded(int32_t captureId, const std::vector<CaptureEndedInfo>& infos) override;
    int32_t OnCaptureError(int32_t captureId, const std::vector<CaptureErrorInfo>& infos) override;
    int32_t OnFrameShutter(int32_t captureId, const std::vector<int32_t>& streamIds, uint64_t timestamp) override;
    void SetCaptureSession(sptr<HCaptureSession> captureSession);

private:
    sptr<HStreamCommon> GetStreamByStreamID(int32_t streamId);
    sptr<HCaptureSession> captureSession_;
    std::mutex cbMutex_;
};
} // namespace CameraStandard
} // namespace OHOS
#endif // OHOS_CAMERA_H_CAPTURE_SESSION_H
