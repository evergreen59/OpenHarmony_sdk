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

#ifndef NET_CONNECT_ADAPTER_H
#define NET_CONNECT_ADAPTER_H

#include <memory>
#include <string>

namespace OHOS::NWeb {
enum class NetConnectType {
    CONNECTION_UNKNOWN = 0,
    CONNECTION_ETHERNET = 1,
    CONNECTION_WIFI = 2,
    CONNECTION_2G = 3,
    CONNECTION_3G = 4,
    CONNECTION_4G = 5,
    CONNECTION_NONE = 6,
    CONNECTION_BLUETOOTH = 7,
    CONNECTION_5G = 8,
    CONNECTION_LAST = CONNECTION_5G
};

enum class NetConnectSubtype {
    SUBTYPE_UNKNOWN = 0,
    SUBTYPE_NONE,
    SUBTYPE_OTHER,
    SUBTYPE_GSM,
    SUBTYPE_IDEN,
    SUBTYPE_CDMA,
    SUBTYPE_1XRTT,
    SUBTYPE_GPRS,
    SUBTYPE_EDGE,
    SUBTYPE_UMTS,
    SUBTYPE_EVDO_REV_0,
    SUBTYPE_EVDO_REV_A,
    SUBTYPE_HSPA,
    SUBTYPE_EVDO_REV_B,
    SUBTYPE_HSDPA,
    SUBTYPE_HSUPA,
    SUBTYPE_EHRPD,
    SUBTYPE_HSPAP,
    SUBTYPE_LTE,
    SUBTYPE_LTE_ADVANCED,
    SUBTYPE_BLUETOOTH_1_2,
    SUBTYPE_BLUETOOTH_2_1,
    SUBTYPE_BLUETOOTH_3_0,
    SUBTYPE_BLUETOOTH_4_0,
    SUBTYPE_ETHERNET,
    SUBTYPE_FAST_ETHERNET,
    SUBTYPE_GIGABIT_ETHERNET,
    SUBTYPE_10_GIGABIT_ETHERNET,
    SUBTYPE_WIFI_B,
    SUBTYPE_WIFI_G,
    SUBTYPE_WIFI_N,
    SUBTYPE_WIFI_AC,
    SUBTYPE_WIFI_AD,
    SUBTYPE_LAST = SUBTYPE_WIFI_AD
};

class NetConnCallback {
public:
    virtual int32_t NetAvailable() = 0;
    virtual int32_t NetCapabilitiesChange(const NetConnectType &netConnectType,
                                          const NetConnectSubtype &netConnectSubtype) = 0;
    virtual int32_t NetConnectionPropertiesChange() = 0;
    virtual int32_t NetUnavailable() = 0;
};

class NetConnectAdapter {
public:
    NetConnectAdapter() = default;

    virtual ~NetConnectAdapter() = default;

    virtual int32_t RegisterNetConnCallback(std::shared_ptr<NetConnCallback> cb) = 0;

    virtual int32_t UnregisterNetConnCallback(std::shared_ptr<NetConnCallback> cb) = 0;

    virtual int32_t GetDefaultNetConnect(NetConnectType &type, NetConnectSubtype &netConnectSubtype) = 0;
};
}  // namespace OHOS::NWeb

#endif  // NET_CONNECT_ADAPTER_H