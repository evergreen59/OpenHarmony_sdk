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

#include "core_service_dump_helper.h"

#include "core_service.h"
#include "enum_convert.h"
#include "signal_info.h"
#include "signal_information.h"

namespace OHOS {
namespace Telephony {
bool CoreServiceDumpHelper::Dump(const std::vector<std::string> &args, std::string &result) const
{
    result.clear();
    ShowHelp(result);
    ShowCoreServiceTimeInfo(result);
    ShowCoreServiceInfo(result);
    return true;
}

void CoreServiceDumpHelper::ShowHelp(std::string &result) const
{
    result.append("CoreService:\n")
        .append("Usage:dump <command> [options]\n")
        .append("Description:\n")
        .append("-core_service_info          ")
        .append("dump all core_service information in the system\n")
        .append("-input_simulate <event>    ")
        .append("simulate event from ohos core_service, supported events: login/logout/token_invalid\n")
        .append("-output_simulate <event>    ")
        .append("simulate event output\n")
        .append("-show_log_level        ")
        .append("show core_service SA's log level\n")
        .append("-set_log_level <level>     ")
        .append("set core_service SA's log level\n")
        .append("-perf_dump         ")
        .append("dump performance statistics\n");
}

static std::string to_utf8(std::u16string str16)
{
    return std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> {}.to_bytes(str16);
}

void CoreServiceDumpHelper::ShowCoreServiceTimeInfo(std::string &result) const
{
    result.append("Ohos core_service service:\n");
    result.append("BindTime = ");
    result.append(std::to_string(DelayedSingleton<CoreService>::GetInstance()->GetBindTime()));
    result.append("\nEndTime = ");
    result.append(std::to_string(DelayedSingleton<CoreService>::GetInstance()->GetEndTime()));
    result.append("\nSpendTime = ");
    result.append(std::to_string(DelayedSingleton<CoreService>::GetInstance()->GetSpendTime()));
    result.append("\n");
}

void CoreServiceDumpHelper::ShowCoreServiceInfo(std::string &result) const
{
    for (int32_t i = 0; i < SIM_SLOT_COUNT; i++) {
        bool hasSimCard = false;
        DelayedSingleton<CoreService>::GetInstance()->HasSimCard(i, hasSimCard);
        if (hasSimCard) {
            result.append("SlotId = ");
            result.append(std::to_string(i));
            result.append("\nIsSimActive = ");
            result.append(GetBoolValue(DelayedSingleton<CoreService>::GetInstance()->IsSimActive(i)));
            result.append("\nIsNrSupported = ");
            result.append(GetBoolValue(DelayedSingleton<CoreService>::GetInstance()->IsNrSupported(i)));
            result.append("\nSignalLevel = ");
            std::vector<sptr<SignalInformation>> signals;
            DelayedSingleton<CoreService>::GetInstance()->GetSignalInfoList(i, signals);
            if (signals.size() != 0 && signals[0] != nullptr) {
                result.append(std::to_string(signals[0]->GetSignalLevel()));
            }
            result.append("\nCardType = ");
            CardType cardType = CardType::UNKNOWN_CARD;
            DelayedSingleton<CoreService>::GetInstance()->GetCardType(i, cardType);
            result.append(GetCardType(static_cast<int32_t>(cardType)));
            result.append("\nSimState = ");
            SimState simState = SimState::SIM_STATE_UNKNOWN;
            DelayedSingleton<CoreService>::GetInstance()->GetSimState(i, simState);
            result.append(GetSimState(static_cast<int32_t>(simState)));
            result.append("\nSpn = ");
            std::u16string spn;
            DelayedSingleton<CoreService>::GetInstance()->GetSimSpn(i, spn);
            result.append(to_utf8(spn));
            result.append("\nOperatorName = ");
            std::u16string operatorName;
            DelayedSingleton<CoreService>::GetInstance()->GetOperatorName(i, operatorName);
            result.append(to_utf8(operatorName));
            int32_t csRadioTech = 0;
            int32_t psRadioTech = 0;
            DelayedSingleton<CoreService>::GetInstance()->GetPsRadioTech(i, psRadioTech);
            DelayedSingleton<CoreService>::GetInstance()->GetCsRadioTech(i, csRadioTech);
            result.append("\nPsRadioTech = ");
            result.append(GetCellularDataConnectionNetworkType(psRadioTech));
            result.append("\nCsRadioTech = ");
            result.append(GetCellularDataConnectionNetworkType(csRadioTech));
            result.append("\n");
        }
    }
    result.append("\n");
}
} // namespace Telephony
} // namespace OHOS