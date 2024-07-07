/*
 * Copyright (C) 2021-2022 Huawei Device Co., Ltd.
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

#include "test.h"

#include <list>
#include <securec.h>
#include <sys/time.h>
#include <unistd.h>

#include "core_service_client.h"
#include "core_service_proxy.h"
#include "ims_reg_info_callback_test.h"
#include "iservice_registry.h"
#include "network_search_test_callback_stub.h"
#include "singleton.h"
#include "system_ability_definition.h"
#include "telephony_errors.h"
#include "telephony_log_wrapper.h"
#include "test_broadcast.h"
#include "time_service_client.h"

namespace OHOS {
namespace Telephony {
using namespace OHOS::EventFwk;
sptr<ICoreService> g_telephonyService = nullptr;
const int32_t INVALID_VALUE = -1;
const int32_t INPUT_GET_PS_RADIO_TECH = 0;
const int32_t INPUT_GET_CS_RADIO_TECH = 1;
const int32_t INPUT_GET_OPERATOR_NUMERIC = 2;
const int32_t INPUT_GET_NETWORK_STATE = 3;
const int32_t INPUT_GET_OPERATOR_NAME = 4;
const int32_t INPUT_GET_SIGNAL_INFO_LIST = 5;
const int32_t INPUT_SET_RADIO_STATE = 6;
const int32_t INPUT_GET_RADIO_STATE = 7;
const int32_t INPUT_NOTIFY_NETWORK_CHANGE = 8;
const int32_t INPUT_GET_NETWORK_SEARCH_RESULT = 9;
const int32_t INPUT_GET_NETWORK_SELECTION_MODE = 10;
const int32_t INPUT_SET_NETWORK_SELECTION_MODE = 11;
const int32_t INPUT_GET_IOS_COUNTRY_CODE = 14;
const int32_t INPUT_GET_PREFERRED_NETWORK_MODE = 15;
const int32_t INPUT_SET_PREFERRED_NETWORK_MODE = 16;
const int32_t INPUT_SET_TIME_AND_TIMEZONE = 17;
const int32_t INPUT_GET_IMEI = 18;
const int32_t INPUT_GET_MEID = 19;
const int32_t INPUT_GET_UNIQUE_DEVICE_ID = 20;
const int32_t INPUT_SET_GET_IMS_REG_STATUS = 22;
const int32_t INPUT_GET_CELL_INFO_LIST = 23;
const int32_t INPUT_REQUEST_CELL_LOCATION = 24;
const int32_t INPUT_IS_NR_SUPPORTED = 25;
const int32_t INPUT_GET_NR_OPTION_MODE = 26;
const int32_t REG_IMS_ST_CALLBACK = 27;
const int32_t UN_REG_IMS_ST_CALLBACK = 28;
const int32_t INPUT_NOTIFY_SPN_CHANGE = 29;
const int32_t INPUT_INIT_TIME = 99;
const int32_t INPUT_QUIT = 100;
const int32_t SLEEP_TIME = 5;
using NsTestFunc = void (*)();
std::map<int32_t, NsTestFunc> memberFuncMap_;
std::shared_ptr<TestBroadCast> subscriber = nullptr;
struct ImsRegStateCallback {
    int32_t slotId;
    int32_t imsSrvType;
    sptr<ImsRegInfoCallback> imsCallback = nullptr;
};
std::list<ImsRegStateCallback> imsRegStateCallbackList_;
sptr<ICoreService> GetProxy()
{
    TELEPHONY_LOGI("TelephonyTestService GetProxy ... ");
    sptr<ISystemAbilityManager> systemAbilityMgr = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
    if (systemAbilityMgr == nullptr) {
        TELEPHONY_LOGE("TelephonyTestService Get ISystemAbilityManager failed ... ");
        return nullptr;
    }

    sptr<IRemoteObject> remote = systemAbilityMgr->CheckSystemAbility(TELEPHONY_CORE_SERVICE_SYS_ABILITY_ID);
    if (remote) {
        sptr<ICoreService> telephonyService = iface_cast<ICoreService>(remote);
        TELEPHONY_LOGI("TelephonyTestService Get TELEPHONY_CORE_SERVICE_SYS_ABILITY_ID success ... ");
        return telephonyService;
    } else {
        TELEPHONY_LOGE("TelephonyTestService Get TELEPHONY_CORE_SERVICE_SYS_ABILITY_ID fail ... ");
        return nullptr;
    }
}

static int32_t InputSlotId()
{
    printf("please input slotid(0,1):\n");
    int32_t slotId = 0;
    std::cin >> slotId;
    while (std::cin.fail()) {
        std::cin.clear();
        std::cin.ignore();
        TELEPHONY_LOGI("Input error, please input slotid again\n");
        std::cin >> slotId;
    }
    TELEPHONY_LOGI("slotId is:[%{public}d]", slotId);
    return slotId;
}

void TestGetNetworkState()
{
    AccessToken token;
    TELEPHONY_LOGI("TelephonyTestService Remote GetNetworkState");
    sptr<NetworkState> result;
    g_telephonyService->GetNetworkState(InputSlotId(), result);
    if (result == nullptr) {
        TELEPHONY_LOGE("result is null");
    } else {
        TELEPHONY_LOGI("CoreServiceProxy::GetNetworkState return");
        TELEPHONY_LOGI("GetRegStatus():%{public}d", result->GetRegStatus());
        TELEPHONY_LOGI("GetLongOperatorName():%{public}s", result->GetLongOperatorName().c_str());
        TELEPHONY_LOGI("GetShortOperatorName():%{public}s", result->GetShortOperatorName().c_str());
        TELEPHONY_LOGI("GetPlmnNumeric():%{public}s", result->GetPlmnNumeric().c_str());
        TELEPHONY_LOGI("GetPsRoamingStatus():%{public}d", result->GetPsRoamingStatus());
        TELEPHONY_LOGI("GetCsRoamingStatus():%{public}d", result->GetCsRoamingStatus());
        TELEPHONY_LOGI("IsEmergency():%{public}d", result->IsEmergency());
        TELEPHONY_LOGI("GetCfgTech():%{public}d", result->GetCfgTech());
        TELEPHONY_LOGI("GetNrState():%{public}d", result->GetNrState());
    }
}

void TestGetPsRadioTech()
{
    AccessToken token;
    int32_t radioTech = INVALID_VALUE;
    g_telephonyService->GetPsRadioTech(InputSlotId(), radioTech);
    TELEPHONY_LOGI("TelephonyTestService Remote GetPsRadioTech radioTech:%{public}d", radioTech);
}

void TestGetCsRadioTech()
{
    AccessToken token;
    int32_t radioTech = INVALID_VALUE;
    g_telephonyService->GetCsRadioTech(InputSlotId(), radioTech);
    TELEPHONY_LOGI("TelephonyTestService Remote GetCsRadioTech radioTech:%{public}d", radioTech);
}

void TestGetOperatorNumeric()
{
    std::u16string result = g_telephonyService->GetOperatorNumeric(InputSlotId());
    std::string str = Str16ToStr8(result);
    TELEPHONY_LOGI("TelephonyTestService Remote GetOperatorNumeric result:%{public}s", str.c_str());
}

void TestGetOperatorName()
{
    std::u16string operatorName = u"";
    g_telephonyService->GetOperatorName(InputSlotId(), operatorName);
    std::string str = Str16ToStr8(operatorName);
    TELEPHONY_LOGI("TelephonyTestService Remote GetOperatorName result:%{public}s", str.c_str());
}

void TestGsmSignalInformation(const GsmSignalInformation &gsm)
{
    TELEPHONY_LOGI("TelephonyTestService Remote GsmSignalInformation ToString:%{public}s", gsm.ToString().c_str());
    TELEPHONY_LOGI("TelephonyTestService Remote GsmSignalInformation GetRssi:%{public}d", gsm.GetRssi());
    TELEPHONY_LOGI("TelephonyTestService Remote GsmSignalInformation GetSignalLevel:%{public}d", gsm.GetSignalLevel());
    TELEPHONY_LOGI("TelephonyTestService Remote GsmSignalInformation GetBitErrorRate:%{public}d", gsm.GetGsmBer());
    TELEPHONY_LOGI("TelephonyTestService Remote GsmSignalInformation GetNetworkType[GSM] \n");
}

void TestCdmaSignalInformation(const CdmaSignalInformation &cdma)
{
    TELEPHONY_LOGI("TelephonyTestService Remote CdmaSignalInformation ToString:%{public}s", cdma.ToString().c_str());
    TELEPHONY_LOGI("TelephonyTestService Remote CdmaSignalInformation GetCdmaRssi:%{public}d", cdma.GetCdmaRssi());
    TELEPHONY_LOGI(
        "TelephonyTestService Remote CdmaSignalInformation GetSignalLevel:%{public}d", cdma.GetSignalLevel());
    TELEPHONY_LOGI("TelephonyTestService Remote CdmaSignalInformation GetNetworkType[CDMA] \n");
}

void TestLteSignalInformation(const LteSignalInformation &lte)
{
    TELEPHONY_LOGI("TelephonyTestService Remote LteSignalInformation ToString:%{public}s", lte.ToString().c_str());
    TELEPHONY_LOGI("TelephonyTestService Remote LteSignalInformation GetLteRsrq:%{public}d", lte.GetRsrq());
    TELEPHONY_LOGI("TelephonyTestService Remote LteSignalInformation GetLteRsrp:%{public}d", lte.GetRsrp());
    TELEPHONY_LOGI("TelephonyTestService Remote LteSignalInformation GetLteSnr:%{public}d", lte.GetSnr());
    TELEPHONY_LOGI("TelephonyTestService Remote LteSignalInformation GetLteRxlev:%{public}d", lte.GetRxlev());
    TELEPHONY_LOGI("TelephonyTestService Remote LteSignalInformation GetSignalLevel:%{public}d", lte.GetSignalLevel());
    TELEPHONY_LOGI("TelephonyTestService Remote LteSignalInformation GetNetworkType[LTE] \n");
}

void TestWcdmaSignalInformation(const WcdmaSignalInformation &wcdma)
{
    TELEPHONY_LOGI("TelephonyTestService Remote WcdmaSignalInformation ToString:%{public}s", wcdma.ToString().c_str());
    TELEPHONY_LOGI("TelephonyTestService Remote WcdmaSignalInformation GetwcdmaRscp:%{public}d", wcdma.GetRscp());
    TELEPHONY_LOGI("TelephonyTestService Remote WcdmaSignalInformation GetRxlev:%{public}d", wcdma.GetRxlev());
    TELEPHONY_LOGI("TelephonyTestService Remote WcdmaSignalInformation GetwcdmaEcio:%{public}d", wcdma.GetEcno());
    TELEPHONY_LOGI("TelephonyTestService Remote WcdmaSignalInformation GetwcdmaBer:%{public}d", wcdma.GetBer());
    TELEPHONY_LOGI(
        "TelephonyTestService Remote WcdmaSignalInformation GetSignalLevel:%{public}d", wcdma.GetSignalLevel());
    TELEPHONY_LOGI("TelephonyTestService Remote WcdmaSignalInformation GetNetworkType[WCDMA] \n");
}

void TestTdScdmaSignalInformation(const TdScdmaSignalInformation &tdScdma)
{
    TELEPHONY_LOGI(
        "TelephonyTestService Remote TdScdmaSignalInformation ToString:%{public}s", tdScdma.ToString().c_str());
    TELEPHONY_LOGI("TelephonyTestService Remote TdScdmaSignalInformation GetRssi:%{public}d", tdScdma.GetRscp());
    TELEPHONY_LOGI(
        "TelephonyTestService Remote TdScdmaSignalInformation GetSignalLevel:%{public}d", tdScdma.GetSignalLevel());
    TELEPHONY_LOGI("TelephonyTestService Remote TdScdmaSignalInformation GetNetworkType[TDSCDMA] \n");
}

void TestNrSignalInformation(const NrSignalInformation &nr)
{
    TELEPHONY_LOGI("TelephonyTestService Remote NrSignalInformation ToString:%{public}s", nr.ToString().c_str());
    TELEPHONY_LOGI("TelephonyTestService Remote NrSignalInformation NrRsrp:%{public}d", nr.GetRsrp());
    TELEPHONY_LOGI("TelephonyTestService Remote NrSignalInformation NrRsrq:%{public}d", nr.GetRsrq());
    TELEPHONY_LOGI("TelephonyTestService Remote NrSignalInformation NrSinr:%{public}d", nr.GetSinr());
    TELEPHONY_LOGI("TelephonyTestService Remote NrSignalInformation GetSignalLevel:%{public}d", nr.GetSignalLevel());
    TELEPHONY_LOGI("TelephonyTestService Remote NrSignalInformation GetNetworkType[NR] \n");
}

void TestGetSignalInfoList()
{
    std::vector<sptr<SignalInformation>> signals;
    g_telephonyService->GetSignalInfoList(InputSlotId(), signals);
    SignalInformation::NetworkType type;
    for (const auto &v : signals) {
        type = v->GetNetworkType();
        TELEPHONY_LOGI("TelephonyTestService Remote SignalInformation result NetworkTypeId:%{public}d",
            static_cast<int32_t>(type));
        if (type == SignalInformation::NetworkType::GSM) {
            GsmSignalInformation *gsm = reinterpret_cast<GsmSignalInformation *>(v.GetRefPtr());
            TestGsmSignalInformation(*gsm);
        } else if (type == SignalInformation::NetworkType::CDMA) {
            CdmaSignalInformation *cdma = reinterpret_cast<CdmaSignalInformation *>(v.GetRefPtr());
            TestCdmaSignalInformation(*cdma);
        } else if (type == SignalInformation::NetworkType::LTE) {
            LteSignalInformation *lte = reinterpret_cast<LteSignalInformation *>(v.GetRefPtr());
            TestLteSignalInformation(*lte);
        } else if (type == SignalInformation::NetworkType::WCDMA) {
            WcdmaSignalInformation *wcdma = reinterpret_cast<WcdmaSignalInformation *>(v.GetRefPtr());
            TestWcdmaSignalInformation(*wcdma);
        } else if (type == SignalInformation::NetworkType::TDSCDMA) {
            TdScdmaSignalInformation *tdScdma = reinterpret_cast<TdScdmaSignalInformation *>(v.GetRefPtr());
            TestTdScdmaSignalInformation(*tdScdma);
        } else if (type == SignalInformation::NetworkType::NR) {
            NrSignalInformation *nr = reinterpret_cast<NrSignalInformation *>(v.GetRefPtr());
            TestNrSignalInformation(*nr);
        }
    }
    TELEPHONY_LOGI("TelephonyTestService Remote TestGetSignalInfoList size:%{public}zu", signals.size());
}

void TestSetRadioState()
{
    AccessToken token;
    std::cout << "please input radio state off(0) or on(1)" << std::endl;
    int32_t inputState = 0;
    std::cin >> inputState;
    bool isOn = true;
    if (!inputState) {
        isOn = false;
    }
    TELEPHONY_LOGI("radio off(N) or on(Y) :%{public}d", isOn);
    OHOS::sptr<NetworkSearchTestCallbackStub> callback(new NetworkSearchTestCallbackStub());
    bool result = g_telephonyService->SetRadioState(InputSlotId(), isOn, callback);
    TELEPHONY_LOGI("TelephonyTestService Remote SetRadioState result:%{public}d", inputState);
    TELEPHONY_LOGI("TelephonyTestService::TestSetRadioState result:%{public}d", result);
}

void TestGetRadioState()
{
    AccessToken token;
    OHOS::sptr<NetworkSearchTestCallbackStub> callback(new NetworkSearchTestCallbackStub());
    int32_t result = g_telephonyService->GetRadioState(InputSlotId(), callback);
    TELEPHONY_LOGI("TelephonyTestService Remote GetRadioState result:%{public}d", result);
}

void TestNotifySpnChanged()
{
    TestSetRadioState();
    std::cout << "please wait for SPN_INFO_CHANGED, or input any key to exit" << std::endl;
    int32_t inputToExit = 0;
    std::cin >> inputToExit;
}

void TestNotifyNetworkStateChange()
{
    AccessToken token;
    TestGetCsRadioTech();
    sleep(SLEEP_TIME);
    TestGetOperatorNumeric();
    bool isOn = true;
    OHOS::sptr<NetworkSearchTestCallbackStub> callback(new NetworkSearchTestCallbackStub());
    g_telephonyService->SetRadioState(InputSlotId(), isOn, callback);
}

void TestGetNetworkSearchInformation()
{
    AccessToken token;
    if (g_telephonyService != nullptr) {
        OHOS::sptr<NetworkSearchTestCallbackStub> callback(new NetworkSearchTestCallbackStub());
        int32_t result = g_telephonyService->GetNetworkSearchInformation(InputSlotId(), callback);
        TELEPHONY_LOGI("TelephonyTestService::TestGetNetworkSearchInformation result:%{public}d", result);
    }
}

void TestGetNetworkSelectionMode()
{
    AccessToken token;
    if (g_telephonyService != nullptr) {
        OHOS::sptr<NetworkSearchTestCallbackStub> callback(new NetworkSearchTestCallbackStub());
        bool result = g_telephonyService->GetNetworkSelectionMode(InputSlotId(), callback);
        TELEPHONY_LOGI("TelephonyTestService::TestGetNetworkSelectionMode result:%{public}d", result);
    }
}

void TestSetNetworkSelectionMode()
{
    AccessToken token;
    if (g_telephonyService == nullptr) {
        TELEPHONY_LOGE("g_telephonyService is nullptr");
        return;
    }
    sptr<NetworkState> networkState;
    g_telephonyService->GetNetworkState(InputSlotId(), networkState);
    if (networkState == nullptr) {
        TELEPHONY_LOGE("networkState is nullptr");
        return;
    }
    sptr<NetworkInformation> networkInfo = new (std::nothrow) NetworkInformation();
    int32_t selectionMode = 1;
    std::cout << "please enter the selectionMode (0:Automatic mode , 1:Manual mode) :";
    std::cin >> selectionMode;
    networkInfo->SetOperateInformation("CHINA MOBILE", "CMCC", networkState->GetPlmnNumeric().c_str(),
        static_cast<int32_t>(NetworkPlmnState::NETWORK_PLMN_STATE_AVAILABLE),
        static_cast<int32_t>(NetworkRat::NETWORK_LTE));
    sptr<NetworkSearchTestCallbackStub> callback(new NetworkSearchTestCallbackStub());
    bool result = g_telephonyService->SetNetworkSelectionMode(
        InputSlotId(), selectionMode, networkInfo, true, callback);
    TELEPHONY_LOGI("TelephonyTestService::TestSetNetworkSelectionMode result:%{public}d", result);
}

void TestGetIsoCountryCodeForNetwork()
{
    if (g_telephonyService != nullptr) {
        std::u16string result;
        g_telephonyService->GetIsoCountryCodeForNetwork(InputSlotId(), result);
        std::string str = Str16ToStr8(result);
        TELEPHONY_LOGI("TestGetIsoCountryCodeForNetwork result:%{public}s", str.c_str());
    }
}

void TestGetPreferredNetwork()
{
    AccessToken token;
    if (g_telephonyService != nullptr) {
        OHOS::sptr<NetworkSearchTestCallbackStub> callback(new NetworkSearchTestCallbackStub());
        int32_t result = g_telephonyService->GetPreferredNetwork(InputSlotId(), callback);
        TELEPHONY_LOGI("TelephonyTestService::TestGetPreferredNetwork result:%{public}d", result);
    }
}

void TestSetPreferredNetwork()
{
    AccessToken token;
    int32_t networkMode = 0;
    std::cout << "please input networkmode:" << std::endl;
    std::cin >> networkMode;
    if (g_telephonyService != nullptr) {
        OHOS::sptr<NetworkSearchTestCallbackStub> callback(new NetworkSearchTestCallbackStub());
        int32_t result = g_telephonyService->SetPreferredNetwork(InputSlotId(), networkMode, callback);
        TELEPHONY_LOGI("TelephonyTestService::TestSetPreferredNetwork result:%{public}d", result);
    }
}

void TestIsNrSupported()
{
    if (g_telephonyService != nullptr) {
        bool result = g_telephonyService->IsNrSupported(InputSlotId());
        TELEPHONY_LOGI("TelephonyTestService::TestIsNrSupported result:%{public}s", result ? "true" : "false");
    }
}

void TestGetNrOptionMode()
{
    if (g_telephonyService == nullptr) {
        std::cout << "TestGetNrOptionMode g_telephonyService is nullptr." << std::endl;
        return;
    }
    NrMode mode = NrMode::NR_MODE_UNKNOWN;
    int32_t result = g_telephonyService->GetNrOptionMode(InputSlotId(), mode);
    if (result != TELEPHONY_ERR_SUCCESS) {
        std::cout << "error:" << result << std::endl;
    }
    std::cout << "mode:" << static_cast<int32_t>(mode) << std::endl;
}

void TestRegisterImsRegStateCallback()
{
    if (g_telephonyService == nullptr) {
        std::cout << "g_telephonyService is nullptr" << std::endl;
        return;
    }
    ImsRegStateCallback imsRegStateCallback;
    std::cout << "please input slotId:" << std::endl;
    std::cin >> imsRegStateCallback.slotId;
    std::cout << "please input imsSrvType(TYPE_VOICE = 0,TYPE_VIDEO = 1,"
                 "TYPE_UT = 2,TYPE_SMS = 3):"
              << std::endl;
    std::cin >> imsRegStateCallback.imsSrvType;
    imsRegStateCallback.imsCallback = new ImsRegInfoCallbackTest();
    int32_t ret =
        DelayedRefSingleton<CoreServiceClient>::GetInstance().RegisterImsRegInfoCallback(imsRegStateCallback.slotId,
            static_cast<ImsServiceType>(imsRegStateCallback.imsSrvType), imsRegStateCallback.imsCallback);
    if (ret == TELEPHONY_SUCCESS) {
        imsRegStateCallbackList_.push_back(imsRegStateCallback);
    } else {
        if (imsRegStateCallback.imsCallback != nullptr) {
            delete imsRegStateCallback.imsCallback;
            imsRegStateCallback.imsCallback = nullptr;
        }
    }
    std::cout << " ret:" << ret << std::endl;
}

void TestUnregisterImsRegStateCallback()
{
    if (g_telephonyService == nullptr) {
        std::cout << "g_telephonyService is nullptr" << std::endl;
        return;
    }
    int32_t slotId = 0;
    int32_t imsSrvType = 0;
    std::cout << "please input slotId:" << std::endl;
    std::cin >> slotId;
    std::cout << "please input imsSrvType(TYPE_VOICE = 0,TYPE_VIDEO = 1,"
                 "TYPE_UT = 2,TYPE_SMS = 3):"
              << std::endl;
    std::cin >> imsSrvType;
    int32_t ret = DelayedRefSingleton<CoreServiceClient>::GetInstance().UnregisterImsRegInfoCallback(
        slotId, static_cast<ImsServiceType>(imsSrvType));
    if (ret != TELEPHONY_SUCCESS) {
        std::cout << " ret:" << ret << std::endl;
        return;
    }
    auto itor = imsRegStateCallbackList_.begin();
    for (; itor != imsRegStateCallbackList_.end(); ++itor) {
        if (itor->slotId == slotId && itor->imsSrvType == imsSrvType) {
            if (itor->imsCallback != nullptr) {
                delete itor->imsCallback;
                itor->imsCallback = nullptr;
            }
            imsRegStateCallbackList_.erase(itor);
            break;
        }
    }
    std::cout << " ret:" << ret << std::endl;
}

void TestGetTimeZone()
{
    std::string timeZoneRes = OHOS::MiscServices::TimeServiceClient::GetInstance()->GetTimeZone();
    std::cout << " timezone:" << timeZoneRes << std::endl;
}

void TestInitTimeAndTimeZone()
{
    static const int32_t MILLI_TO_BASE = 1000;
    static const int32_t TEST_NITZ = 100;
    struct tm t;
    (void)memset_s(&t, sizeof(t), 0x00, sizeof(t));
    t.tm_year = TEST_NITZ;
    t.tm_mon = 1;
    t.tm_mday = 1;
    time_t retTime = mktime(&t);
    if (retTime == -1) {
        return;
    }
    int64_t time = static_cast<int64_t>(retTime);
    bool result = OHOS::MiscServices::TimeServiceClient::GetInstance()->SetTime(time * MILLI_TO_BASE);
    std::string ret = result ? "ture" : "false";
    TELEPHONY_LOGI("TelephonyTestService::TestInitTimeAndTimeZone result : %{public}s", ret.c_str());

    std::string timeZoneSet("America/Santiago");
    result = OHOS::MiscServices::TimeServiceClient::GetInstance()->SetTimeZone(timeZoneSet);
    ret = result ? "ture" : "false";
    TELEPHONY_LOGI("TelephonyTestService::TestInitTimeAndTimeZone SetTimeZone result : %{public}d", ret.c_str());

    std::string timeZoneRes = OHOS::MiscServices::TimeServiceClient::GetInstance()->GetTimeZone();
    std::cout << " timezone:" << timeZoneRes << std::endl;
}

void TestGetImsRegStatus()
{
    AccessToken token;
    if (g_telephonyService != nullptr) {
        int32_t slotId = 0;
        int32_t imsSrvType = 0;
        std::cout << "please input slotId:" << std::endl;
        std::cin >> slotId;
        std::cout << "please input imsSrvType(TYPE_VOICE = 0,TYPE_VIDEO = 1,"
            "TYPE_UT = 2,TYPE_SMS = 3):" << std::endl;
        std::cin >> imsSrvType;
        ImsRegInfo info;
        int32_t ret = g_telephonyService->GetImsRegStatus(slotId, static_cast<ImsServiceType>(imsSrvType), info);
        if (ret != TELEPHONY_SUCCESS) {
            std::cout << "get ims register info failed! error code is" << ret << std::endl;
        } else {
            std::cout << "imsRegState" << info.imsRegState << "imsRegTech" << info.imsRegTech << std::endl;
        }
    }
}

void TestGetImei()
{
    AccessToken token;
    if (g_telephonyService == nullptr) {
        std::cout << "TestGetImei g_telephonyService is nullptr." << std::endl;
        return;
    }
    std::u16string imei = u"";
    int32_t result = g_telephonyService->GetMeid(InputSlotId(), imei);
    if (result != TELEPHONY_ERR_SUCCESS) {
        std::cout << "error:" << result << std::endl;
    }
    std::string str = Str16ToStr8(imei);
    std::cout << "imei:" << str << std::endl;
}

void TestGetMeid()
{
    AccessToken token;
    if (g_telephonyService == nullptr) {
        std::cout << "TestGetMeid g_telephonyService is nullptr." << std::endl;
        return;
    }
    std::u16string meid = u"";
    int32_t result = g_telephonyService->GetMeid(InputSlotId(), meid);
    if (result != TELEPHONY_ERR_SUCCESS) {
        std::cout << "error:" << result << std::endl;
    }
    std::string str = Str16ToStr8(meid);
    std::cout << "meid:" << str << std::endl;
}

void TestGetUniqueDeviceId()
{
    AccessToken token;
    if (g_telephonyService == nullptr) {
        std::cout << "TestGetUniqueDeviceId g_telephonyService is nullptr." << std::endl;
        return;
    }
    std::u16string deviceId = u"";
    int32_t result = g_telephonyService->GetUniqueDeviceId(InputSlotId(), deviceId);
    if (result != TELEPHONY_ERR_SUCCESS) {
        std::cout << "error:" << result << std::endl;
    }
    std::string str = Str16ToStr8(deviceId);
    std::cout << "deviceId:" << str << std::endl;
}

void TestGetCellInfoList()
{
    AccessToken token;
    if (g_telephonyService != nullptr) {
        sptr<NetworkState> networkState = new (std::nothrow) NetworkState();
        std::vector<sptr<CellInformation>> cellList;
        if (g_telephonyService->GetCellInfoList(InputSlotId(), cellList) != TELEPHONY_ERR_SUCCESS) {
            TELEPHONY_LOGE("TestGetCellInfoList GetCellInfoList failed.");
            return;
        }
        CellInformation::CellType type;
        for (const auto &v : cellList) {
            type = v->GetNetworkType();
            TELEPHONY_LOGI(
                "TelephonyTestService Remote CellInfoList result NetworkTypeId:%{public}d", static_cast<int32_t>(type));
            if (type == CellInformation::CellType::CELL_TYPE_GSM) {
                GsmCellInformation *gsm = reinterpret_cast<GsmCellInformation *>(v.GetRefPtr());
                TELEPHONY_LOGI("result:%{public}s", gsm->ToString().c_str());
            } else if (type == CellInformation::CellType::CELL_TYPE_LTE) {
                LteCellInformation *lte = reinterpret_cast<LteCellInformation *>(v.GetRefPtr());
                TELEPHONY_LOGI("result:%{public}s", lte->ToString().c_str());
            } else if (type == CellInformation::CellType::CELL_TYPE_WCDMA) {
                WcdmaCellInformation *wcdma = reinterpret_cast<WcdmaCellInformation *>(v.GetRefPtr());
                TELEPHONY_LOGI("result:%{public}s", wcdma->ToString().c_str());
            } else if (type == CellInformation::CellType::CELL_TYPE_CDMA) {
                CdmaCellInformation *cdma = reinterpret_cast<CdmaCellInformation *>(v.GetRefPtr());
                TELEPHONY_LOGI("result:%{public}s", cdma->ToString().c_str());
            } else if (type == CellInformation::CellType::CELL_TYPE_TDSCDMA) {
                TdscdmaCellInformation *tdscdma = reinterpret_cast<TdscdmaCellInformation *>(v.GetRefPtr());
                TELEPHONY_LOGI("result:%{public}s", tdscdma->ToString().c_str());
            } else if (type == CellInformation::CellType::CELL_TYPE_NR) {
                NrCellInformation *nr = reinterpret_cast<NrCellInformation *>(v.GetRefPtr());
                TELEPHONY_LOGI("result:%{public}s", nr->ToString().c_str());
            }
        }
    }
}

void TestSendUpdateCellLocationRequest()
{
    AccessToken token;
    if (g_telephonyService != nullptr) {
        int32_t result = g_telephonyService->SendUpdateCellLocationRequest(InputSlotId());
        TELEPHONY_LOGI("TelephonyTestService::SendUpdateCellLocationRequest result:%{public}d", result);
    }
}
void Prompt()
{
    printf(
        "\n-----------start test remote api--------------\n"
        "usage:please input a cmd num:\n"
        "0:GetPsRadioTech\n"
        "1:GetCsRadioTech\n"
        "2:GetOperatorNumeric\n"
        "3:GetNetworkState\n"
        "4:GetOperatorName\n"
        "5:GetSignalInfoList\n"
        "6:SetRadioState\n"
        "7:GetRadioState\n"
        "8:NotifyNetworkStateChange\n"
        "9:GetNetworkSearchInformation\n"
        "10:GetNetworkSelectionMode\n"
        "11:SetNetworkSelectionMode\n"
        "14:GetIsoCountryCodeForNetwork\n"
        "15:GetPreferredNetwork\n"
        "16:SetPreferredNetwork\n"
        "17:GetTimeZone\n"
        "18:GetImei\n"
        "19:GetMeid\n"
        "20:GetUniqueDeviceId\n"
        "22:GetImsRegStatus\n"
        "23:GetCellInfoList\n"
        "24:SendUpdateCellLocationRequest\n"
        "25:IsNrSupported\n"
        "26:GetNrOptionMode\n"
        "27:RegisterImsRegStateCallback\n"
        "28:UnregisterImsRegStateCallback\n"
        "29:NotifySpnChange\n"
        "99:InitTimeAndTimeZone\n"
        "100:exit \n");
}

void ProcessInput(bool &loopFlag)
{
    int32_t inputCMD = 0;
    std::cin >> inputCMD;
    while (std::cin.fail()) {
        std::cin.clear();
        std::cin.ignore();
        TELEPHONY_LOGI("Input error, please input again\n");
        std::cin >> inputCMD;
    }
    auto itFunc = memberFuncMap_.find(inputCMD);
    if (itFunc != memberFuncMap_.end()) {
        auto memberFunc = itFunc->second;
        if (memberFunc != nullptr) {
            (*memberFunc)();
            return;
        }
    }
    TELEPHONY_LOGI("inputCMD is:[%{public}d]", inputCMD);
    switch (inputCMD) {
        case INPUT_QUIT: {
            loopFlag = false;
            TELEPHONY_LOGI("exit...");
            break;
        }
        default:
            TELEPHONY_LOGI("please input correct number...");
            break;
    }
}

void TestInputQuit(bool &loopFlag)
{
    loopFlag = false;
}

void Init()
{
    memberFuncMap_[INPUT_GET_PS_RADIO_TECH] = TestGetPsRadioTech;
    memberFuncMap_[INPUT_GET_CS_RADIO_TECH] = TestGetCsRadioTech;
    memberFuncMap_[INPUT_GET_OPERATOR_NUMERIC] = TestGetOperatorNumeric;
    memberFuncMap_[INPUT_GET_NETWORK_STATE] = TestGetNetworkState;
    memberFuncMap_[INPUT_GET_OPERATOR_NAME] = TestGetOperatorName;
    memberFuncMap_[INPUT_GET_SIGNAL_INFO_LIST] = TestGetSignalInfoList;
    memberFuncMap_[INPUT_SET_RADIO_STATE] = TestSetRadioState;
    memberFuncMap_[INPUT_GET_RADIO_STATE] = TestGetRadioState;
    memberFuncMap_[INPUT_NOTIFY_NETWORK_CHANGE] = TestNotifyNetworkStateChange;
    memberFuncMap_[INPUT_GET_NETWORK_SEARCH_RESULT] = TestGetNetworkSearchInformation;
    memberFuncMap_[INPUT_GET_NETWORK_SELECTION_MODE] = TestGetNetworkSelectionMode;
    memberFuncMap_[INPUT_SET_NETWORK_SELECTION_MODE] = TestSetNetworkSelectionMode;
    memberFuncMap_[INPUT_GET_IOS_COUNTRY_CODE] = TestGetIsoCountryCodeForNetwork;
    memberFuncMap_[INPUT_GET_PREFERRED_NETWORK_MODE] = TestGetPreferredNetwork;
    memberFuncMap_[INPUT_SET_PREFERRED_NETWORK_MODE] = TestSetPreferredNetwork;
    memberFuncMap_[INPUT_SET_TIME_AND_TIMEZONE] = TestGetTimeZone;
    memberFuncMap_[INPUT_GET_IMEI] = TestGetImei;
    memberFuncMap_[INPUT_GET_MEID] = TestGetMeid;
    memberFuncMap_[INPUT_GET_UNIQUE_DEVICE_ID] = TestGetUniqueDeviceId;
    memberFuncMap_[INPUT_SET_GET_IMS_REG_STATUS] = TestGetImsRegStatus;
    memberFuncMap_[INPUT_GET_CELL_INFO_LIST] = TestGetCellInfoList;
    memberFuncMap_[INPUT_REQUEST_CELL_LOCATION] = TestSendUpdateCellLocationRequest;
    memberFuncMap_[INPUT_INIT_TIME] = TestInitTimeAndTimeZone;
    memberFuncMap_[INPUT_IS_NR_SUPPORTED] = TestIsNrSupported;
    memberFuncMap_[INPUT_GET_NR_OPTION_MODE] = TestGetNrOptionMode;
    memberFuncMap_[REG_IMS_ST_CALLBACK] = TestRegisterImsRegStateCallback;
    memberFuncMap_[UN_REG_IMS_ST_CALLBACK] = TestUnregisterImsRegStateCallback;
    memberFuncMap_[INPUT_NOTIFY_SPN_CHANGE] = TestNotifySpnChanged;
}

void InitBroadCast()
{
    MatchingSkills matchingSkills;
    matchingSkills.AddEvent(CommonEventSupport::COMMON_EVENT_SPN_INFO_CHANGED);
    CommonEventSubscribeInfo subscribeInfo(matchingSkills);
    subscribeInfo.SetPriority(1);
    subscriber = std::make_shared<TestBroadCast>(subscribeInfo);
    CommonEventManager::SubscribeCommonEvent(subscriber);
}
} // namespace Telephony
} // namespace OHOS

using namespace OHOS::Telephony;
using namespace OHOS::EventFwk;
int32_t main()
{
    Init();
    g_telephonyService = GetProxy();
    if (g_telephonyService == nullptr) {
        return 1;
    }
    InitBroadCast();
    bool loopFlag = true;
    while (loopFlag) {
        Prompt();
        ProcessInput(loopFlag);
    }
    TELEPHONY_LOGI("...exit test...");
    return 0;
}
