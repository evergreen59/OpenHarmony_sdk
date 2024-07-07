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

#ifndef TELEPHONY_NAPI_HRIL_ERROR_CODE_H
#define TELEPHONY_NAPI_HRIL_ERROR_CODE_H

namespace OHOS {
namespace Telephony {
enum RilErrorCode {
    HRIL_ERR_NULL_POINT = -1,
    HRIL_ERR_SUCCESS = 0,
    HRIL_ERR_GENERIC_FAILURE = 1,
    HRIL_ERR_INVALID_PARAMETER = 2,
    HRIL_ERR_MEMORY_FULL = 3,
    HRIL_ERR_CMD_SEND_FAILURE = 4,
    HRIL_ERR_CMD_NO_CARRIER = 5,
    HRIL_ERR_INVALID_RESPONSE = 6,
    HRIL_ERR_REPEAT_STATUS = 7,
};
} // namespace Telephony
} // namespace OHOS
#endif // TELEPHONY_NAPI_HRIL_ERROR_CODE_H