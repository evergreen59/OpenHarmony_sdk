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

#ifndef OHOS_IDSCREEN_SOURCE_CALLBACK_H
#define OHOS_IDSCREEN_SOURCE_CALLBACK_H

#include "iremote_broker.h"

namespace OHOS {
namespace DistributedHardware {
class IDScreenSourceCallback : public OHOS::IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.distributedhardware.dscreensourcecallback");
    enum {
        NOTIFY_REG_RESULT = 0,
        NOTIFY_UNREG_RESULT,
    };

    virtual ~IDScreenSourceCallback() {}
    virtual int32_t OnNotifyRegResult(const std::string &devId, const std::string &dhId,
        const std::string &reqId, int32_t status, const std::string &data) = 0;
    virtual int32_t OnNotifyUnregResult(const std::string &devId, const std::string &dhId,
        const std::string &reqId, int32_t status, const std::string &data) = 0;
};
}
}
#endif