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
#ifndef HIDUMPER_UTIL_ZIP_COMMON_TYPE_H
#define HIDUMPER_UTIL_ZIP_COMMON_TYPE_H
#include <functional>
namespace OHOS {
namespace HiviewDFX {
const int UNSET_PROGRESS = -1;
using ZipTickNotify = std::function<bool(int progress, int subprogress)>;
} // namespace HiviewDFX
} // namespace OHOS
#endif // HIDUMPER_UTIL_ZIP_COMMON_TYPE_H