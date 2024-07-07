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
 */

#ifndef GNSS_ABILITY_PROXY_H
#define GNSS_ABILITY_PROXY_H

#include <vector>

#include "iremote_object.h"
#include "iremote_proxy.h"

#include "constant_definition.h"
#include "gnss_ability_skeleton.h"

namespace OHOS {
namespace Location {
class GnssAbilityProxy : public IRemoteProxy<IGnssAbility> {
public:
    explicit GnssAbilityProxy(const sptr<IRemoteObject> &impl);
    ~GnssAbilityProxy() = default;
    LocationErrCode SendLocationRequest(WorkRecord &workrecord) override;
    LocationErrCode SetEnable(bool state) override;
    LocationErrCode RefrashRequirements() override;
    LocationErrCode RegisterGnssStatusCallback(const sptr<IRemoteObject>& callback, pid_t uid) override;
    LocationErrCode UnregisterGnssStatusCallback(const sptr<IRemoteObject>& callback) override;
    LocationErrCode RegisterNmeaMessageCallback(const sptr<IRemoteObject>& callback, pid_t uid) override;
    LocationErrCode UnregisterNmeaMessageCallback(const sptr<IRemoteObject>& callback) override;
    LocationErrCode RegisterCachedCallback(const std::unique_ptr<CachedGnssLocationsRequest>& request,
        const sptr<IRemoteObject>& callback) override;
    LocationErrCode UnregisterCachedCallback(const sptr<IRemoteObject>& callback) override;
    LocationErrCode GetCachedGnssLocationsSize(int &size) override;
    LocationErrCode FlushCachedGnssLocations() override;
    LocationErrCode SendCommand(std::unique_ptr<LocationCommand>& commands) override;
    LocationErrCode AddFence(std::unique_ptr<GeofenceRequest>& request) override;
    LocationErrCode RemoveFence(std::unique_ptr<GeofenceRequest>& request) override;
    LocationErrCode EnableMock() override;
    LocationErrCode DisableMock() override;
    LocationErrCode SetMocked(const int timeInterval, const std::vector<std::shared_ptr<Location>> &location) override;
private:
    static inline BrokerDelegator<GnssAbilityProxy> delegator_;
};
} // namespace Location
} // namespace OHOS
#endif // GNSS_ABILITY_PROXY_H
