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

#ifndef OHOS_SESSION_STACK_H
#define OHOS_SESSION_STACK_H

#include <string>

#include "session_container.h"

namespace OHOS::AVSession {
class SessionStack : public SessionContainer {
public:
    int32_t AddSession(pid_t pid, const std::string& abilityName, sptr<AVSessionItem>& item) override;

    std::vector<sptr<AVSessionItem>> RemoveSession(pid_t pid) override;

    sptr<AVSessionItem> RemoveSession(pid_t pid, const std::string& abilityName) override;

    sptr<AVSessionItem> RemoveSession(const std::string& sessionId) override;

    sptr<AVSessionItem> GetSession(pid_t pid, const std::string& abilityName) override;

    sptr<AVSessionItem> GetSessionById(const std::string& sessionId) override;

    std::vector<sptr<AVSessionItem>> GetAllSessions() override;

private:
    std::map<std::pair<pid_t, std::string>, sptr<AVSessionItem>> sessions_;
    std::list<sptr<AVSessionItem>> stack_;
};
} // namespace OHOS::AVSession
#endif // OHOS_SESSION_STACK_H