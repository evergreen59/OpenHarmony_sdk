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

#ifndef PLAYER_CALLBACK_NAPI_H
#define PLAYER_CALLBACK_NAPI_H

#include "audio_player_napi.h"
#include "player.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "common_napi.h"

namespace OHOS {
namespace Media {
struct AudioPlayerAsyncContext : public MediaAsyncContext {
    explicit AudioPlayerAsyncContext(napi_env env) : MediaAsyncContext(env) {}
    ~AudioPlayerAsyncContext() = default;
    AudioPlayerNapi *jsPlayer = nullptr;
};

class PlayerCallbackNapi : public PlayerCallback {
public:
    explicit PlayerCallbackNapi(napi_env env);
    virtual ~PlayerCallbackNapi();
    void SaveCallbackReference(const std::string &name, std::weak_ptr<AutoRef> ref);
    void ClearCallbackReference();
    void SendErrorCallback(MediaServiceExtErrCode errCode, const std::string &info = "error");
    virtual PlayerStates GetCurrentState() const;
    void OnError(int32_t errorCode, const std::string &errorMsg) override;
    void OnInfo(PlayerOnInfoType type, int32_t extra, const Format &infoBody) override;

protected:
    struct PlayerJsCallback {
        std::weak_ptr<AutoRef> callback;
        std::string callbackName = "unknown";
        std::string errorMsg = "unknown";
        MediaServiceExtErrCode errorCode = MSERR_EXT_UNKNOWN;
        std::vector<int32_t> valueVec;
        OHOS::AudioStandard::InterruptEvent interruptEvent;
    };
    void OnJsCallBack(PlayerJsCallback *jsCb) const;
    void OnJsCallBackError(PlayerJsCallback *jsCb) const;
    void OnJsCallBackInt(PlayerJsCallback *jsCb) const;
    void OnJsCallBackIntVec(PlayerJsCallback *jsCb) const;
    void OnJsCallBackIntArray(PlayerJsCallback *jsCb) const;
    void OnJsCallBackInterrupt(PlayerJsCallback *jsCb) const;
    std::map<std::string, std::weak_ptr<AutoRef>> refMap_;

private:
    void OnSeekDoneCb(int32_t currentPositon) const;
    void OnEosCb(int32_t isLooping) const;
    void OnStateChangeCb(PlayerStates state);
    void OnPositionUpdateCb(int32_t position) const;
    void OnMessageCb(int32_t type) const;
    void OnVolumeChangeCb();
    void OnBufferingUpdateCb(const Format &infoBody) const;
    void OnAudioInterruptCb(const Format &infoBody) const;
    std::mutex mutex_;
    napi_env env_ = nullptr;
    PlayerStates currentState_ = PLAYER_IDLE;
};
} // namespace Media
} // namespace OHOS
#endif // PLAYER_CALLBACK_NAPI_H
