/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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
 *
 * epfs.h, keep same with fs/epfs/epfs.h
 *
 */

#ifndef OHOS_FILEMANAGEMENT_EPFS_H
#define OHOS_FILEMANAGEMENT_EPFS_H

#include <cstdint>
#include <sys/ioctl.h>

namespace OHOS {
namespace Media {
constexpr uint64_t PRIVACY_MAX_RANGES = 127;

struct __attribute__((packed)) FilterProxyRanges {
    uint64_t size;
    uint64_t reserved; // reserve more space
    struct {
        uint64_t begin;
        uint64_t end;
    } range[0];
};

#define EPFS_IOCTL_MAGIC                  0x71
#define IOC_SET_ORIGIN_FD                 _IOW(EPFS_IOCTL_MAGIC, 1, int32_t)
#define IOC_SET_FILTER_PROXY_RANGE        _IOW(EPFS_IOCTL_MAGIC, 2, FilterProxyRanges)
#define EPFS_IOCTL_MAXNR 3
} // namespace Media
} // namespace OHOS
#endif // OHOS_FILEMANAGEMENT_EPFS_H
