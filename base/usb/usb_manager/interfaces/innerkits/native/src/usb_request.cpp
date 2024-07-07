/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "usb_request.h"
#include "usb_common.h"
#include "usb_srv_client.h"

namespace OHOS {
namespace USB {
int32_t UsbRequest::Initialize(const USBDevicePipe &pipe, const USBEndpoint &endpoint)
{
    this->pipe_ = pipe;
    this->endpoint_ = endpoint;
    int32_t ret = UsbSrvClient::GetInstance().RequestInitialize(*this);
    if (ret != ERR_OK) {
        USB_HILOGE(MODULE_USB_INNERKIT, "RequestInitialize failed with ret = %{public}d.", ret);
    }
    return ret;
}

int32_t UsbRequest::Queue()
{
    int32_t ret = UsbSrvClient::GetInstance().RequestQueue(*this);
    if (ret != ERR_OK) {
        USB_HILOGE(MODULE_USB_INNERKIT, "RequestQueue failed with ret = %{public}d.", ret);
    }
    return ret;
}

int32_t UsbRequest::Free()
{
    int32_t ret = UsbSrvClient::GetInstance().RequestFree(*this);
    if (ret != ERR_OK) {
        USB_HILOGE(MODULE_USB_INNERKIT, "RequestFree failed with ret = %{public}d.", ret);
    }
    return ret;
}

int32_t UsbRequest::Abort()
{
    int32_t ret = UsbSrvClient::GetInstance().RequestAbort(*this);
    if (ret != ERR_OK) {
        USB_HILOGE(MODULE_USB_INNERKIT, "RequestAbort failed with ret = %{public}d.", ret);
    }
    return ret;
}
} // namespace USB
} // namespace OHOS
