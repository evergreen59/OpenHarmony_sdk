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

#ifndef NWEB_TEST_LOG_H
#define NWEB_TEST_LOG_H

#include <hilog/log.h>

namespace OHOS {
constexpr OHOS::HiviewDFX::HiLogLabel LOG_LABEL = { LOG_CORE, 0, "NWEB_TEST" };

#define TESTLOG(func, fmt, ...) \
    (void)func(LOG_LABEL, "%{public}s<%{public}d>: " fmt, __func__, __LINE__, ##__VA_ARGS__)

#define TESTLOG_F(fmt, ...) TESTLOG(HiviewDFX::HiLog::Fatal, fmt, ##__VA_ARGS__)
#define TESTLOG_E(fmt, ...) TESTLOG(HiviewDFX::HiLog::Error, fmt, ##__VA_ARGS__)
#define TESTLOG_W(fmt, ...) TESTLOG(HiviewDFX::HiLog::Warn, fmt, ##__VA_ARGS__)
#define TESTLOG_I(fmt, ...) TESTLOG(HiviewDFX::HiLog::Info, fmt, ##__VA_ARGS__)
#define TESTLOG_D(fmt, ...) TESTLOG(HiviewDFX::HiLog::Debug, fmt, ##__VA_ARGS__)
} // namespace OHOS

#endif // NWEB_TEST_LOG_H