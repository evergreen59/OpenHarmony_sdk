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

#include "setpreviewwindow_fuzzer.h"

#include <cstddef>
#include <cstdint>
#include <string_ex.h>
#define private public

#include "addcellularcalltoken_fuzzer.h"
#include "cellular_call_service.h"
#include "securec.h"
#include "system_ability_definition.h"

using namespace OHOS::Telephony;
namespace OHOS {
static bool g_isInited = false;
constexpr int32_t INT_NUM = 6;
constexpr int32_t BOOL_NUM = 2;
constexpr int32_t VEDIO_STATE_NUM = 2;
constexpr size_t MAX_NUMBER_LEN = 99;

bool IsServiceInited()
{
    DelayedSingleton<CellularCallService>::GetInstance()->OnStart();
    if (!g_isInited && (static_cast<int32_t>(DelayedSingleton<CellularCallService>::GetInstance()->state_) ==
                           static_cast<int32_t>(ServiceRunningState::STATE_RUNNING))) {
        g_isInited = true;
    }
    return g_isInited;
}

void OnRemoteRequest(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    MessageParcel dataMessageParcel;
    if (!dataMessageParcel.WriteInterfaceToken(CellularCallStub::GetDescriptor())) {
        return;
    }
    size_t dataSize = size - sizeof(uint32_t);
    dataMessageParcel.WriteBuffer(data + sizeof(uint32_t), dataSize);
    dataMessageParcel.RewindRead(0);
    uint32_t code = static_cast<uint32_t>(size);
    MessageParcel reply;
    MessageOption option;
    DelayedSingleton<CellularCallService>::GetInstance()->OnRemoteRequest(code, dataMessageParcel, reply, option);
}

void SetPreviewWindow(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    int32_t maxSize = static_cast<int32_t>(size);
    int32_t x = static_cast<int32_t>(size);
    int32_t y = static_cast<int32_t>(size);
    int32_t z = static_cast<int32_t>(size);
    int32_t width = static_cast<int32_t>(size);
    int32_t height = static_cast<int32_t>(size);
    MessageParcel dataMessageParcel;
    dataMessageParcel.WriteInt32(maxSize);
    dataMessageParcel.WriteInt32(x);
    dataMessageParcel.WriteInt32(y);
    dataMessageParcel.WriteInt32(z);
    dataMessageParcel.WriteInt32(width);
    dataMessageParcel.WriteInt32(height);
    size_t dataSize = size - sizeof(int32_t) * INT_NUM;
    dataMessageParcel.WriteBuffer(data + sizeof(int32_t) * INT_NUM, dataSize);
    dataMessageParcel.RewindRead(0);
    MessageParcel reply;
    DelayedSingleton<CellularCallService>::GetInstance()->OnSetPreviewWindowInner(dataMessageParcel, reply);
}

void SetDisplayWindow(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    int32_t maxSize = static_cast<int32_t>(size);
    int32_t x = static_cast<int32_t>(size);
    int32_t y = static_cast<int32_t>(size);
    int32_t z = static_cast<int32_t>(size);
    int32_t width = static_cast<int32_t>(size);
    int32_t height = static_cast<int32_t>(size);
    MessageParcel dataMessageParcel;
    dataMessageParcel.WriteInt32(maxSize);
    dataMessageParcel.WriteInt32(x);
    dataMessageParcel.WriteInt32(y);
    dataMessageParcel.WriteInt32(z);
    dataMessageParcel.WriteInt32(width);
    dataMessageParcel.WriteInt32(height);
    size_t dataSize = size - sizeof(int32_t) * INT_NUM;
    dataMessageParcel.WriteBuffer(data + sizeof(int32_t) * INT_NUM, dataSize);
    dataMessageParcel.RewindRead(0);
    MessageParcel reply;
    DelayedSingleton<CellularCallService>::GetInstance()->OnSetDisplayWindowInner(dataMessageParcel, reply);
}

void SetCameraZoom(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    int32_t maxSize = static_cast<int32_t>(size);
    float zoomRatio = static_cast<float>(size);
    MessageParcel dataMessageParcel;
    dataMessageParcel.WriteInt32(maxSize);
    dataMessageParcel.WriteFloat(zoomRatio);
    dataMessageParcel.RewindRead(0);
    MessageParcel reply;
    DelayedSingleton<CellularCallService>::GetInstance()->OnSetCameraZoomInner(dataMessageParcel, reply);
}

void SetMute(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    int32_t mute = static_cast<int32_t>(size);
    MessageParcel dataMessageParcel;
    dataMessageParcel.WriteInt32(mute);
    size_t dataSize = size - sizeof(int32_t);
    dataMessageParcel.WriteBuffer(data + sizeof(int32_t), dataSize);
    dataMessageParcel.RewindRead(0);
    MessageParcel reply;
    DelayedSingleton<CellularCallService>::GetInstance()->OnSetMuteInner(dataMessageParcel, reply);
}

void GetMute(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    MessageParcel dataMessageParcel;
    dataMessageParcel.WriteBuffer(data, size);
    dataMessageParcel.RewindRead(0);
    MessageParcel reply;
    DelayedSingleton<CellularCallService>::GetInstance()->OnGetMuteInner(dataMessageParcel, reply);
}

void StartDtmf(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    int32_t maxSize = static_cast<int32_t>(size);
    int32_t slotId = static_cast<int32_t>(size % BOOL_NUM);
    int32_t callId = static_cast<int32_t>(size);
    int32_t accountId = static_cast<int32_t>(size);
    int32_t videoState = static_cast<int32_t>(size % VEDIO_STATE_NUM);
    int32_t index = static_cast<int32_t>(size);
    char cDtmfCode = static_cast<char>(size);
    std::string telNum = "000000000";
    std::string tempNum(reinterpret_cast<const char *>(data), size);
    if (strlen(tempNum.c_str()) <= MAX_NUMBER_LEN) {
        telNum = tempNum;
    }
    size_t length = strlen(telNum.c_str()) + 1;
    CellularCallInfo callInfo;
    callInfo.slotId = slotId;
    callInfo.callId = callId;
    callInfo.accountId = accountId;
    callInfo.videoState = videoState;
    callInfo.index = index;
    if (strcpy_s(callInfo.phoneNum, length, telNum.c_str()) != EOK) {
        return;
    }
    MessageParcel dataMessageParcel;
    dataMessageParcel.WriteInt32(maxSize);
    dataMessageParcel.WriteInt8(cDtmfCode);
    dataMessageParcel.WriteRawData(static_cast<const void *>(&callInfo), sizeof(CellularCallInfo));
    dataMessageParcel.RewindRead(0);
    MessageParcel reply;
    DelayedSingleton<CellularCallService>::GetInstance()->OnStartDtmfInner(dataMessageParcel, reply);
}

void CtrlCamera(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    int32_t maxSize = static_cast<int32_t>(size);
    int32_t callingUid = static_cast<int32_t>(size);
    int32_t callingPid = static_cast<int32_t>(size);
    std::string cameraId(reinterpret_cast<const char *>(data), size);
    auto cameraIdU16 = Str8ToStr16(cameraId);
    MessageParcel dataMessageParcel;
    dataMessageParcel.WriteInt32(maxSize);
    dataMessageParcel.WriteInt32(callingUid);
    dataMessageParcel.WriteInt32(callingPid);
    dataMessageParcel.WriteString16(cameraIdU16);
    dataMessageParcel.RewindRead(0);
    MessageParcel reply;
    DelayedSingleton<CellularCallService>::GetInstance()->OnCtrlCameraInner(dataMessageParcel, reply);
}

void SetPauseImage(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }

    int32_t maxSize = static_cast<int32_t>(size);
    std::string path(reinterpret_cast<const char *>(data), size);
    auto pathU16 = Str8ToStr16(path);
    MessageParcel dataMessageParcel;
    dataMessageParcel.WriteInt32(maxSize);
    dataMessageParcel.WriteString16(pathU16);
    dataMessageParcel.RewindRead(0);
    MessageParcel reply;
    DelayedSingleton<CellularCallService>::GetInstance()->OnSetPauseImageInner(dataMessageParcel, reply);
}

void DoSomethingInterestingWithMyAPI(const uint8_t *data, size_t size)
{
    if (data == nullptr || size == 0) {
        return;
    }

    OnRemoteRequest(data, size);
    SetPreviewWindow(data, size);
    SetDisplayWindow(data, size);
    SetCameraZoom(data, size);
    SetMute(data, size);
    GetMute(data, size);
    StartDtmf(data, size);
    CtrlCamera(data, size);
    SetPauseImage(data, size);
    return;
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    OHOS::AddCellularCallTokenFuzzer token;
    /* Run your code on data */
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}
