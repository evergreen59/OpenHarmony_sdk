/*
* Copyright (C) 2023 Huawei Device Co., Ltd.
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

#include "wallpaper_extension_ability_death_recipient.h"

#include "hilog_wrapper.h"
namespace OHOS {
namespace WallpaperMgrService {
void OHOS::WallpaperMgrService::WallpaperExtensionAbilityDeathRecipient::OnRemoteDied(
    const OHOS::wptr<OHOS::IRemoteObject> &remote)
{
    HILOG_INFO("On remote died");
    wallpaperService_.StartWallpaperExtension();
}

} // namespace WallpaperMgrService
} // namespace OHOS