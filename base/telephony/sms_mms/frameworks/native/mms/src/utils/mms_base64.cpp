﻿/*
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

#include "mms_base64.h"

#include "sms_service_manager_client.h"
#include "string"

namespace OHOS {
namespace Telephony {
std::string MmsBase64::Encode(const std::string src)
{
    std::string dest;
    DelayedSingleton<SmsServiceManagerClient>::GetInstance()->GetBase64Encode(src, dest);
    return dest;
}

std::string MmsBase64::Decode(const std::string src)
{
    std::string dest;
    DelayedSingleton<SmsServiceManagerClient>::GetInstance()->GetBase64Decode(src, dest);
    return dest;
}
} // namespace Telephony
} // namespace OHOS
