/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#ifndef OHOS_ABILITY_RUNTIME_APP_SPAWN_MSG_WRAPPER_H
#define OHOS_ABILITY_RUNTIME_APP_SPAWN_MSG_WRAPPER_H

#include <string>
#include <vector>
#include <unistd.h>

#include "nocopyable.h"
#include "client_socket.h"

namespace OHOS {
namespace AppExecFwk {
struct AppSpawnStartMsg {
    int32_t uid;
    int32_t gid;
    std::vector<int32_t> gids;
    std::string procName;
    std::string soPath;
    uint32_t accessTokenId;
    std::string apl;
    std::string bundleName;
    std::string renderParam; // only nweb spawn need this param.
    int32_t pid;
    int32_t code = 0; // 0: DEFAULT; 1: GET_RENDER_TERMINATION_STATUS
    uint32_t flags;
    int32_t bundleIndex;   // when dlp launch another app used, default is 0
    uint8_t setAllowInternet;
    uint8_t allowInternet; // hap socket allowed
    uint8_t reserved1;
    uint8_t reserved2;
};

using AppSpawnMsg = AppSpawn::ClientSocket::AppProperty;

constexpr auto LEN_PID = sizeof(pid_t);
struct StartFlags {
    static const int COLD_START = 0;
    static const int BACKUP_EXTENSION = 1;
    static const int DLP_MANAGER = 2;
    static const int DEBUGGABLE = 3;
    static const int ASANENABLED = 4;
};

union AppSpawnPidMsg {
    pid_t pid = 0;
    char pidBuf[LEN_PID];
};

class AppSpawnMsgWrapper {
public:
    /**
     * Constructor.
     */
    AppSpawnMsgWrapper() = default;

    /**
     * Destructor
     */
    ~AppSpawnMsgWrapper();

    /**
     * Disable copy.
     */
    DISALLOW_COPY_AND_MOVE(AppSpawnMsgWrapper);

    /**
     * Verify message and assign to member variable.
     *
     * @param startMsg, request message.
     */
    bool AssembleMsg(const AppSpawnStartMsg &startMsg);

    /**
     * Get function, return isValid_.
     */
    bool IsValid() const
    {
        return isValid_;
    }

    /**
     * Get function, return member variable message.
     */
    const void *GetMsgBuf() const
    {
        return reinterpret_cast<void *>(msg_);
    }

    /**
     * Get function, return message length.
     */
    int32_t GetMsgLength() const
    {
        return isValid_ ? sizeof(AppSpawnMsg) : 0;
    }

private:
    /**
     * Verify message.
     *
     * @param startMsg, request message.
     */
    bool VerifyMsg(const AppSpawnStartMsg &startMsg) const;

    /**
     * Print message.
     *
     * @param startMsg, request message.
     */
    void DumpMsg() const;

    /**
     * Release message.
     */
    void FreeMsg();

private:
    bool isValid_ = false;
    // because AppSpawnMsg's size is uncertain, so should use raw pointer.
    AppSpawnMsg *msg_ = nullptr;
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // OHOS_ABILITY_RUNTIME_APP_SPAWN_MSG_WRAPPER_H
