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

#ifndef NWEB_HILOG_H
#define NWEB_HILOG_H

#include <cstdio>
#include <hilog/log.h>

namespace OHOS {
constexpr HiviewDFX::HiLogLabel LOG_LABEL = { LOG_CORE, 0xD004500, "NWEB" };

#define FUNC_LINE_FMT " %{public}s<%{public}d>: "

#define WVLOG_D(fmt, ...) \
    HiviewDFX::HiLog::Debug(LOG_LABEL, FUNC_LINE_FMT fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define WVLOG_I(fmt, ...) \
    HiviewDFX::HiLog::Info(LOG_LABEL, FUNC_LINE_FMT fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define WVLOG_W(fmt, ...) \
    HiviewDFX::HiLog::Warn(LOG_LABEL, FUNC_LINE_FMT fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define WVLOG_E(fmt, ...) \
    HiviewDFX::HiLog::Error(LOG_LABEL, FUNC_LINE_FMT fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define WVLOG_F(fmt, ...) \
    HiviewDFX::HiLog::Fatal(LOG_LABEL, FUNC_LINE_FMT fmt, __FUNCTION__, __LINE__, ##__VA_ARGS__)
} // namespace OHOS

#endif // NWEB_HILOG_H