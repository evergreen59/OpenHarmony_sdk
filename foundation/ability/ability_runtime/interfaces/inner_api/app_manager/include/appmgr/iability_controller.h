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

#ifndef OHOS_ABILITY_RUNTIME_IABILITY_CONTROLLER_H
#define OHOS_ABILITY_RUNTIME_IABILITY_CONTROLLER_H

#include "iremote_broker.h"
#include "iremote_object.h"
#include "want.h"

namespace OHOS {
namespace AppExecFwk {
using OHOS::AAFwk::Want;
/**
 * @brief Testing interface to monitor what is happening in ability manager while tests are running.
 */
class IAbilityController : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.appexecfwk.IAbilityController");

    /**
     * The system is trying to start an ability.
     *
     * @param want The want of ability to start.
     * @param bundleName The bundleName of ability to start.
     * @return Return true to allow ability to start, or false to reject.
     */
    virtual bool AllowAbilityStart(const Want &want, const std::string &bundleName) = 0;

    /**
     * The system is scheduling Ability to background.
     *
     * @param bundleName The bundleName of ability to return.
     * @return Return true to allow ability to background, or false to reject.
     */
    virtual bool AllowAbilityBackground(const std::string &bundleName) = 0;

    enum class Message {
        TRANSACT_ON_ALLOW_ABILITY_START = 0,
        TRANSACT_ON_ALLOW_ABILITY_BACKGROUND,
    };
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // OHOS_ABILITY_RUNTIME_IABILITY_CONTROLLER_H
