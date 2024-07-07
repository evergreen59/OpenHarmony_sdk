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

#include "nweb_pre_dns_adapter.h"

#include <securec.h>
#include <vector>

#include "nweb_log.h"
#include "ohos_adapter_helper.h"

namespace OHOS::NWeb {
    std::map<std::string, addrinfo *> g_AddrInfoMap;

    void PreDnsInThread()
    {
        WVLOG_I("PreDns start");
        struct addrinfo hints = {0};
        hints.ai_family = AF_UNSPEC;
        hints.ai_flags = AI_ADDRCONFIG;
        hints.ai_socktype = SOCK_STREAM;
        std::vector<std::string> hostInfo;

        OHOS::NWeb::OhosWebDnsDataBaseAdapter &dnsDatabaseAdapter =
            OHOS::NWeb::OhosAdapterHelper::GetInstance().GetWebDnsDataBaseInstance();
        dnsDatabaseAdapter.GetHostnames(hostInfo);
        dnsDatabaseAdapter.ClearAllHostname();
        if (hostInfo.empty()) {
            WVLOG_I("PreDns no hostinfo.");
            return;
        }

        for (std::vector<std::string>::iterator it = hostInfo.begin(); it != hostInfo.end(); it++) {
            WVLOG_I("PreDns hostname: %{public}s", (*it).c_str());
            addrinfo *addrInfo;
            int error = ::getaddrinfo((*it).c_str(), nullptr, &hints, &addrInfo);
            if (!addrInfo) {
                WVLOG_E("PreDns get address failed, error: %{public}d", error);
                continue;
            }
            g_AddrInfoMap.emplace(*it, addrInfo);
        }
        WVLOG_I("PreDns end");
    }

} // namespace OHOS::NWeb
