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

#ifndef FOUNDATION_ABILITY_FORM_FWK_SERVICES_INCLUDE_FREE_INSTALL_STATUS_CALL_BACK_INTERFACE_H
#define FOUNDATION_ABILITY_FORM_FWK_SERVICES_INCLUDE_FREE_INSTALL_STATUS_CALL_BACK_INTERFACE_H

#include <iremote_broker.h>
#include "want.h"

namespace OHOS {
namespace AppExecFwk {
using OHOS::AAFwk::Want;
/**
 * @class IFreeInstallStatusCallBack
 * IFreeInstallStatusCallBack is used to notify caller ability that free install is complete.
 */
class IFreeInstallStatusCallBack : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"ohos.aafwk.IAtomicServiceStatusCallback");

    /**
     * OnInstallFinished, FreeInstall is complete.
     *
     * @param resultCode, ERR_OK on success, others on failure.
     * @param want, installed ability.
     * @param userId, user`s id.
     */
    virtual void OnInstallFinished(int32_t resultCode, const Want &want, int32_t userId) = 0;

protected:
    enum IFreeInstallStatusCallBackCmd {
        ON_FREE_INSTALL_DONE = 0,
        CMD_MAX,
    };
};
} // namespace AppExecFwk
} // namespace OHOS
#endif // FOUNDATION_ABILITY_FORM_FWK_SERVICES_INCLUDE_FREE_INSTALL_STATUS_CALL_BACK_INTERFACE_H
