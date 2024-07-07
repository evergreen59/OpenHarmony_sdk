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
#ifndef SERVICES_INCLUDE_WALLPAPER_SERVICE_CB_PROXY_H
#define SERVICES_INCLUDE_WALLPAPER_SERVICE_CB_PROXY_H

#include <cstdint>
#include "iremote_proxy.h"
#include "iremote_object.h"
#include "i_wallpaper_callback.h"
#include "refbase.h"

namespace OHOS {
namespace WallpaperMgrService {
class WallpaperServiceCbProxy : public IRemoteProxy<IWallpaperCallback> {
public:
    explicit WallpaperServiceCbProxy(const sptr<IRemoteObject> &object) : IRemoteProxy<IWallpaperCallback>(object) {}
    virtual ~WallpaperServiceCbProxy() {};

    static inline BrokerDelegator<WallpaperServiceCbProxy> delegator_;

    int32_t OnCall(const int32_t num) override;
};
} // namespace WallpaperMgrService
} // namespace OHOS
#endif // SERVICES_INCLUDE_WALLPAPER_SERVICE_CB_PROXY_H
