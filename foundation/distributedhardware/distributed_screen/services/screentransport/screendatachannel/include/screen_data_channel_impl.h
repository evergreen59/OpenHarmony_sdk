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

#ifndef OHOS_SCREEN_DATA_CHANNEL_IMPL_H
#define OHOS_SCREEN_DATA_CHANNEL_IMPL_H

#include <memory>
#include <string>

#include "data_buffer.h"
#include "iscreen_channel.h"
#include "softbus_adapter.h"

namespace OHOS {
namespace DistributedHardware {
class ScreenDataChannelImpl : public IScreenChannel,
    public ISoftbusListener,
    public std::enable_shared_from_this<ScreenDataChannelImpl> {
public:
    ScreenDataChannelImpl(std::string peerDevId) : peerDevId_(peerDevId) {};
    ~ScreenDataChannelImpl() = default;

    int32_t CreateSession(const std::shared_ptr<IScreenChannelListener> &listener) override;
    int32_t ReleaseSession() override;
    int32_t OpenSession() override;
    int32_t CloseSession() override;
    int32_t SendData(const std::shared_ptr<DataBuffer> &screenData) override;

    void OnSessionOpened(int32_t sessionId, int32_t result) override;
    void OnSessionClosed(int32_t sessionId) override;
    void OnBytesReceived(int32_t sessionId, const void *data, uint32_t dataLen) override;
    void OnStreamReceived(int32_t sessionId, const StreamData *data, const StreamData *ext,
        const StreamFrameInfo *param) override;

private:
    static const constexpr char *LOG_TAG = "ScreenDataChannel";

    const std::string peerDevId_;
    int32_t sessionId_ = 0;
    std::weak_ptr<IScreenChannelListener> channelListener_;
};
} // namespace DistributedHardware
} // namespace OHOS
#endif