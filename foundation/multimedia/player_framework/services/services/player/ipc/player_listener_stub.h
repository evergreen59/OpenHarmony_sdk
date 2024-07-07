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

#ifndef PLAYER_LISTENER_STUB_H
#define PLAYER_LISTENER_STUB_H

#include "i_standard_player_listener.h"
#include "player.h"
#include "format.h"

namespace OHOS {
namespace Media {
class PlayerListenerStub : public IRemoteStub<IStandardPlayerListener> {
public:
    PlayerListenerStub();
    virtual ~PlayerListenerStub();
    // IStandardPlayerListener override
    int OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;
    void OnError(PlayerErrorType errorType, int32_t errorCode) override;
    void OnError(int32_t errorCode, const std::string &errorMsg) override;
    void OnInfo(PlayerOnInfoType type, int32_t extra, const Format &infoBody = {}) override;

    // PlayerListenerStub
    void SetPlayerCallback(const std::weak_ptr<PlayerCallback> &callback);

private:
    std::weak_ptr<PlayerCallback> callback_;
};
} // namespace Media
} // namespace OHOS
#endif // PLAYER_LISTENER_STUB_H
