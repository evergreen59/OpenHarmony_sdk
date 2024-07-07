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
#ifndef IAM_MOCK_AUTHENTICATION_H
#define IAM_MOCK_AUTHENTICATION_H

#include <gmock/gmock.h>

#include "authentication.h"

namespace OHOS {
namespace UserIam {
namespace UserAuth {
class MockAuthentication final : public Authentication {
public:
    virtual ~MockAuthentication() = default;
    MOCK_METHOD1(SetExecutor, void(uint32_t executorIndex));
    MOCK_METHOD1(SetChallenge, void(const std::vector<uint8_t> &challenge));
    MOCK_METHOD1(SetAccessTokenId, void(uint32_t tokenId));
    MOCK_METHOD2(Start,
        bool(std::vector<std::shared_ptr<ScheduleNode>> &scheduleList, std::shared_ptr<ScheduleNodeCallback> callback));
    MOCK_METHOD2(Update, bool(const std::vector<uint8_t> &scheduleResult, AuthResultInfo &resultInfo));
    MOCK_METHOD0(Cancel, bool());
    MOCK_CONST_METHOD0(GetLatestError, int32_t());

protected:
    MOCK_METHOD1(SetLatestError, void(int32_t error));
};
} // namespace UserAuth
} // namespace UserIam
} // namespace OHOS
#endif // IAM_MOCK_AUTHENTICATION_H