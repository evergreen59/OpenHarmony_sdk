/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef FILEMGMT_LIBHILOG_H
#define FILEMGMT_LIBHILOG_H

#include "hilog/log.h"

#include <string>

namespace OHOS {
#ifndef LOG_DOMAIN
#define LOG_DOMAIN 0xD001600
#endif

#ifndef LOG_TAG
#define LOG_TAG "FileManagement"
#endif

static constexpr HiviewDFX::HiLogLabel FILEMGMT_LOG_LABEL = {LOG_CORE, LOG_DOMAIN, LOG_TAG};

#if defined __FILE_NAME__
#define FILEMGMT_FILE_NAME __FILE_NAME__
#else
#include <cstring>
#define FILEMGMT_FILE_NAME (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#endif

#define FILEMGMT_PRINT_LOG(Level, fmt, ...)                                                                     \
    HiviewDFX::HiLog::Level(FILEMGMT_LOG_LABEL, "[%{public}s:%{public}d->%{public}s] " fmt, FILEMGMT_FILE_NAME, \
                            __LINE__, __FUNCTION__, ##__VA_ARGS__)

#define HILOGD(fmt, ...) FILEMGMT_PRINT_LOG(Debug, fmt, ##__VA_ARGS__)
#define HILOGI(fmt, ...) FILEMGMT_PRINT_LOG(Info, fmt, ##__VA_ARGS__)
#define HILOGW(fmt, ...) FILEMGMT_PRINT_LOG(Warn, fmt, ##__VA_ARGS__)
#define HILOGE(fmt, ...) FILEMGMT_PRINT_LOG(Error, fmt, ##__VA_ARGS__)
#define HILOGF(fmt, ...) FILEMGMT_PRINT_LOG(Fatal, fmt, ##__VA_ARGS__)
} // namespace OHOS

#endif // FILEMGMT_LIBHILOG_H