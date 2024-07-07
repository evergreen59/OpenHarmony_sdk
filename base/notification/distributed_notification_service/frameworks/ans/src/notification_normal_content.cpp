/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "notification_normal_content.h"

#include <string>                         // for operator+, basic_string

#include "ans_log_wrapper.h"
#include "nlohmann/json.hpp"              // for json
#include "notification_basic_content.h"   // for NotificationBasicContent

namespace OHOS {
namespace Notification {
std::string NotificationNormalContent::Dump()
{
    return "NotificationNormalContent{ " + NotificationBasicContent::Dump() + " }";
}

bool NotificationNormalContent::ToJson(nlohmann::json &jsonObject) const
{
    return NotificationBasicContent::ToJson(jsonObject);
}

NotificationNormalContent *NotificationNormalContent::FromJson(const nlohmann::json &jsonObject)
{
    if (jsonObject.is_null() or !jsonObject.is_object()) {
        ANS_LOGE("Invalid JSON object");
        return nullptr;
    }

    auto pContent = new (std::nothrow) NotificationNormalContent();
    if (pContent == nullptr) {
        ANS_LOGE("Failed to create normalContent instance");
        return nullptr;
    }

    pContent->ReadFromJson(jsonObject);

    return pContent;
}

bool NotificationNormalContent::Marshalling(Parcel &parcel) const
{
    return NotificationBasicContent::Marshalling(parcel);
}

NotificationNormalContent *NotificationNormalContent::Unmarshalling(Parcel &parcel)
{
    auto pContent = new (std::nothrow) NotificationNormalContent();
    if ((pContent != nullptr) && !pContent->ReadFromParcel(parcel)) {
        delete pContent;
        pContent = nullptr;
    }

    return pContent;
}
}  // namespace Notification
}  // namespace OHOS
