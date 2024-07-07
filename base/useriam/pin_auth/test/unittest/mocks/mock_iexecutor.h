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

#ifndef MOCK_IEXECUTOR_H
#define MOCK_IEXECUTOR_H

#include "gmock/gmock.h"

#include "v1_0/iexecutor.h"

namespace OHOS {
namespace HDI {
namespace PinAuth {
namespace V1_0 {
using namespace OHOS;
using namespace OHOS::HDI;
class MockIExecutor : public IExecutor {
public:
    virtual ~MockIExecutor() = default;
    MOCK_METHOD1(GetExecutorInfo, int32_t(ExecutorInfo &executorInfo));
    MOCK_METHOD2(GetTemplateInfo, int32_t(uint64_t templateId, TemplateInfo &templateInfo));
    MOCK_METHOD3(OnRegisterFinish, int32_t(const std::vector<uint64_t> &templateIdList,
        const std::vector<uint8_t> &frameworkPublicKey, const std::vector<uint8_t> &extraInfo));
    MOCK_METHOD3(Enroll, int32_t(uint64_t scheduleId, const std::vector<uint8_t> &extraInfo,
        const sptr<IExecutorCallback> &callbackObj));
    MOCK_METHOD4(Authenticate, int32_t(uint64_t scheduleId, uint64_t templateId, const std::vector<uint8_t> &extraInfo,
        const sptr<IExecutorCallback> &callbackObj));
    MOCK_METHOD3(Identify, int32_t(uint64_t scheduleId, const std::vector<uint8_t> &extraInfo,
        const sptr<IExecutorCallback> &callbackObj));
    MOCK_METHOD1(Delete, int32_t(uint64_t templateId));
    MOCK_METHOD1(Cancel, int32_t(uint64_t scheduleId));
    MOCK_METHOD3(SendCommand, int32_t(int32_t commandId, const std::vector<uint8_t> &extraInfo,
        const sptr<IExecutorCallback> &callbackObj));
    MOCK_METHOD3(OnSetData, int32_t(uint64_t scheduleId, uint64_t authSubType, const std::vector<uint8_t> &data));
};
} // namespace V1_0
} // namespace PinAuth
} // namespace HDI
} // namespace OHOS

#endif // MOCK_IEXECUTOR_H