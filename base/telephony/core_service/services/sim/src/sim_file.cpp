/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#include "sim_file.h"

#include <unistd.h>

#include "common_event_manager.h"
#include "common_event_support.h"
#include "radio_event.h"
#include "telephony_common_utils.h"

using namespace std;
using namespace OHOS::AppExecFwk;
using namespace OHOS::EventFwk;

namespace OHOS {
namespace Telephony {
std::mutex IccFile::mtx_;
SimFile::SimFile(
    const std::shared_ptr<AppExecFwk::EventRunner> &runner, std::shared_ptr<SimStateManager> simStateManager)
    : IccFile(runner, simStateManager)
{
    fileQueried_ = false;
    displayConditionOfSpn_ = SPN_INVALID;
    InitMemberFunc();
}

void SimFile::StartLoad()
{
    TELEPHONY_LOGI("SimFile::StartLoad() start");
    LoadSimFiles();
}

std::string SimFile::ObtainSimOperator()
{
    if (operatorNumeric_.empty()) {
        std::string imsi = ObtainIMSI();
        if (imsi.empty()) {
            TELEPHONY_LOGE("SimFile ObtainSimOperator: IMSI is null");
            return "";
        }
        if ((lengthOfMnc_ == UNINITIALIZED_MNC) || (lengthOfMnc_ == UNKNOWN_MNC)) {
            TELEPHONY_LOGE("SimFile ObtainSimOperator:  mncLength invalid");
            return "";
        }
        int length = MCC_LEN + lengthOfMnc_;
        int imsiLen = static_cast<int>(imsi.size());
        operatorNumeric_ = ((imsiLen >= length) ? imsi.substr(0, MCC_LEN + lengthOfMnc_) : "");
    }
    return operatorNumeric_;
}

std::string SimFile::ObtainIsoCountryCode()
{
    std::string numeric = ObtainSimOperator();
    if (numeric.empty()) {
        TELEPHONY_LOGE("SimFile ObtainIsoCountryCode: numeric is null");
        return "";
    }
    int len = static_cast<int>(numeric.length());
    std::string mcc = numeric.substr(0, MCC_LEN);
    if (len >= MCC_LEN && IsValidDecValue(mcc)) {
        std::string iso = MccPool::MccCountryCode(std::stoi(mcc));
        return iso;
    } else {
        return "";
    }
}

int SimFile::ObtainCallForwardStatus()
{
    return callForwardStatus_;
}

void SimFile::UpdateMsisdnNumber(
    const std::string &alphaTag, const std::string &number, const AppExecFwk::InnerEvent::Pointer &onComplete)
{}

void SimFile::ProcessEvent(const AppExecFwk::InnerEvent::Pointer &event)
{
    if (event == nullptr) {
        TELEPHONY_LOGE("event is nullptr!");
        return;
    }
    auto id = event->GetInnerEventId();
    TELEPHONY_LOGI("SimFile::ProcessEvent id %{public}d", id);
    auto itFunc = memberFuncMap_.find(id);
    if (itFunc != memberFuncMap_.end()) {
        auto memberFunc = itFunc->second;
        if (memberFunc != nullptr) {
            bool isFileProcessResponse = (this->*memberFunc)(event);
            ProcessFileLoaded(isFileProcessResponse);
        }
    } else {
        IccFile::ProcessEvent(event);
    }
}

void SimFile::ProcessIccRefresh(int msgId)
{
    switch (msgId) {
        case ELEMENTARY_FILE_MBDN:
            fileToGet_++;
            break;
        case ELEMENTARY_FILE_MAILBOX_CPHS:
            fileToGet_++;
            break;
        case ELEMENTARY_FILE_CSP_CPHS:
            fileToGet_++;
            break;
        case ELEMENTARY_FILE_FDN:
            break;
        case ELEMENTARY_FILE_MSISDN:
            fileToGet_++;
            break;
        case ELEMENTARY_FILE_CFIS:
        case ELEMENTARY_FILE_CFF_CPHS:
            break;
        default:
            LoadSimFiles();
            break;
    }
}

void SimFile::ProcessFileLoaded(bool response)
{
    if (!response) {
        return;
    }
    fileToGet_ -= LOAD_STEP;
    TELEPHONY_LOGI("SimFile ProcessFileLoaded Done: %{public}d requested: %{public}d", fileToGet_, fileQueried_);
    if (ObtainFilesFetched()) {
        OnAllFilesFetched();
    } else if (LockQueriedOrNot()) {
        UpdateSimLanguage();
    } else if (fileToGet_ < 0) {
        fileToGet_ = 0;
    }
}

void SimFile::OnAllFilesFetched()
{
    UpdateSimLanguage();
    UpdateLoaded(true);
    TELEPHONY_LOGI("SimFile SimFile::OnAllFilesFetched: start notify");
    if (filesFetchedObser_ != nullptr) {
        filesFetchedObser_->NotifyObserver(RadioEvent::RADIO_SIM_RECORDS_LOADED, slotId_);
    }
    PublishSimFileEvent(EventFwk::CommonEventSupport::COMMON_EVENT_SIM_STATE_CHANGED, ICC_STATE_LOADED, "");
}

bool SimFile::ProcessIccReady(const AppExecFwk::InnerEvent::Pointer &event)
{
    TELEPHONY_LOGI("SimFile::SIM_STATE_READY received");
    CardType cardType = stateManager_->GetCardType();
    if (cardType == CardType::SINGLE_MODE_USIM_CARD || cardType == CardType::SINGLE_MODE_SIM_CARD) {
        LoadSimFiles();
    } else {
        TELEPHONY_LOGI("invalid SimFile::SIM_STATE_READY received %{public}d", cardType);
    }
    return false;
}

bool SimFile::ProcessIccLocked(const AppExecFwk::InnerEvent::Pointer &event)
{
    TELEPHONY_LOGI("only fetch ELEMENTARY_FILE_LI, ELEMENTARY_FILE_PL and ELEMENTARY_FILE_ICCID in locked state");
    lockQueried_ = true;
    AppExecFwk::InnerEvent::Pointer eventIccId = BuildCallerInfo(MSG_SIM_OBTAIN_ICCID_DONE);
    fileController_->ObtainBinaryFile(ELEMENTARY_FILE_ICCID, eventIccId);
    fileToGet_++;
    return false;
}

void SimFile::ObtainCallForwardFiles()
{
    fileQueried_ = true;

    AppExecFwk::InnerEvent::Pointer eventCFIS = BuildCallerInfo(MSG_SIM_OBTAIN_CFIS_DONE);
    fileController_->ObtainLinearFixedFile(ELEMENTARY_FILE_CFIS, 1, eventCFIS);
    fileToGet_++;

    AppExecFwk::InnerEvent::Pointer eventCFF = BuildCallerInfo(MSG_SIM_OBTAIN_CFF_DONE);
    fileController_->ObtainBinaryFile(ELEMENTARY_FILE_CFF_CPHS, eventCFF);
    fileToGet_++;
}

void SimFile::LoadSimFiles()
{
    TELEPHONY_LOGI("SimFile LoadSimFiles started");
    fileQueried_ = true;

    AppExecFwk::InnerEvent::Pointer eventIMSI = BuildCallerInfo(MSG_SIM_OBTAIN_IMSI_DONE);
    telRilManager_->GetImsi(slotId_, eventIMSI);
    fileToGet_++;

    AppExecFwk::InnerEvent::Pointer eventIccId = BuildCallerInfo(MSG_SIM_OBTAIN_ICCID_DONE);
    fileController_->ObtainBinaryFile(ELEMENTARY_FILE_ICCID, eventIccId);
    fileToGet_++;

    AppExecFwk::InnerEvent::Pointer eventSpn = AppExecFwk::InnerEvent::Pointer(nullptr, nullptr);
    ObtainSpnPhase(true, eventSpn);

    AppExecFwk::InnerEvent::Pointer eventGid1 = BuildCallerInfo(MSG_SIM_OBTAIN_GID1_DONE);
    fileController_->ObtainBinaryFile(ELEMENTARY_FILE_GID1, eventGid1);
    fileToGet_++;

    AppExecFwk::InnerEvent::Pointer eventGid2 = BuildCallerInfo(MSG_SIM_OBTAIN_GID2_DONE);
    fileController_->ObtainBinaryFile(ELEMENTARY_FILE_GID2, eventGid2);
    fileToGet_++;

    AppExecFwk::InnerEvent::Pointer eventPnn = BuildCallerInfo(MSG_SIM_OBTAIN_PNN_DONE);
    fileController_->ObtainAllLinearFixedFile(ELEMENTARY_FILE_PNN, eventPnn);
    fileToGet_++;

    AppExecFwk::InnerEvent::Pointer eventOpl = BuildCallerInfo(MSG_SIM_OBTAIN_OPL_DONE);
    fileController_->ObtainAllLinearFixedFile(ELEMENTARY_FILE_OPL, eventOpl);
    fileToGet_++;

    AppExecFwk::InnerEvent::Pointer phoneNumberEvent =
        CreateDiallingNumberPointer(MSG_SIM_OBTAIN_MSISDN_DONE, 0, 0, nullptr);
    diallingNumberHandler_->GetDiallingNumbers(
        ELEMENTARY_FILE_MSISDN, ObtainExtensionElementaryFile(ELEMENTARY_FILE_MSISDN), 1, phoneNumberEvent);
    fileToGet_++;

    AppExecFwk::InnerEvent::Pointer eventMBI = BuildCallerInfo(MSG_SIM_OBTAIN_MBI_DONE);
    fileController_->ObtainLinearFixedFile(ELEMENTARY_FILE_MBI, 1, eventMBI);
    fileToGet_++;

    AppExecFwk::InnerEvent::Pointer eventAD = BuildCallerInfo(MSG_SIM_OBTAIN_AD_DONE);
    fileController_->ObtainBinaryFile(ELEMENTARY_FILE_AD, eventAD);
    fileToGet_++;
}

void SimFile::ObtainSpnPhase(bool start, const AppExecFwk::InnerEvent::Pointer &event)
{
    SpnStatus curStatus = spnStatus_;
    if (!IsContinueGetSpn(start, curStatus, spnStatus_)) {
        return;
    }

    TELEPHONY_LOGI("SimFile::ObtainSpnPhase state is %{public}d", spnStatus_);
    if (spnStatus_ == OBTAIN_SPN_START) {
        StartObtainSpn();
    } else if (spnStatus_ == OBTAIN_SPN_GENERAL) {
        ProcessSpnGeneral(event);
    } else if (spnStatus_ == OBTAIN_OPERATOR_NAMESTRING) {
        ProcessSpnCphs(event);
    } else if (spnStatus_ == OBTAIN_OPERATOR_NAME_SHORTFORM) {
        ProcessSpnShortCphs(event);
    } else {
        spnStatus_ = SpnStatus::OBTAIN_SPN_NONE;
    }
}

void SimFile::StartObtainSpn()
{
    UpdateSPN(IccFileController::NULLSTR);
    AppExecFwk::InnerEvent::Pointer eventSPN = BuildCallerInfo(MSG_SIM_OBTAIN_SPN_DONE);
    fileController_->ObtainBinaryFile(ELEMENTARY_FILE_SPN, eventSPN);
    fileToGet_++;
    spnStatus_ = SpnStatus::OBTAIN_SPN_GENERAL;
}

void SimFile::ProcessSpnGeneral(const AppExecFwk::InnerEvent::Pointer &event)
{
    if (event == nullptr) {
        TELEPHONY_LOGE("event is nullptr!");
        return;
    }
    std::unique_ptr<ControllerToFileMsg> fd = event->GetUniqueObject<ControllerToFileMsg>();
    if (fd != nullptr && fd->exception == nullptr) {
        std::string iccData = fd->resultData;
        int length = 0;
        std::shared_ptr<unsigned char> hexData = SIMUtils::HexStringConvertToBytes(iccData, length);
        if (hexData != nullptr) {
            unsigned char *byteData = hexData.get();
            unsigned char value = byteData[0];
            displayConditionOfSpn_ = (BYTE_NUM & value);
        }
        std::string str = ParseSpn(iccData, spnStatus_);
        UpdateSPN(str);
        std::string spn = ObtainSPN();
        if (spn.empty() || !spn.size()) {
            spnStatus_ = SpnStatus::OBTAIN_OPERATOR_NAMESTRING;
        } else {
            TELEPHONY_LOGI("SimFile Load Spn3Gpp done");
            spnStatus_ = SpnStatus::OBTAIN_SPN_NONE;
        }
    } else {
        spnStatus_ = SpnStatus::OBTAIN_OPERATOR_NAMESTRING;
    }

    if (spnStatus_ == SpnStatus::OBTAIN_OPERATOR_NAMESTRING) {
        AppExecFwk::InnerEvent::Pointer eventCphs = BuildCallerInfo(MSG_SIM_OBTAIN_SPN_DONE);
        fileController_->ObtainBinaryFile(ELEMENTARY_FILE_SPN_CPHS, eventCphs);
        fileToGet_++;
    }
}

void SimFile::ProcessSpnCphs(const AppExecFwk::InnerEvent::Pointer &event)
{
    if (event == nullptr) {
        TELEPHONY_LOGE("event is nullptr!");
        return;
    }
    std::unique_ptr<ControllerToFileMsg> fd = event->GetUniqueObject<ControllerToFileMsg>();
    if (fd != nullptr && fd->exception == nullptr) {
        std::string iccData = fd->resultData;
        UpdateSPN(ParseSpn(iccData, spnStatus_));
        std::string spn = ObtainSPN();
        if (spn.empty() || !spn.size()) {
            spnStatus_ = SpnStatus::OBTAIN_OPERATOR_NAME_SHORTFORM;
        } else {
            displayConditionOfSpn_ = 0;
            TELEPHONY_LOGI("SimFile Load ELEMENTARY_FILE_SPN_CPHS done: %{public}s", spn.c_str());
            spnStatus_ = SpnStatus::OBTAIN_SPN_NONE;
        }
    } else {
        spnStatus_ = SpnStatus::OBTAIN_OPERATOR_NAME_SHORTFORM;
    }

    if (spnStatus_ == SpnStatus::OBTAIN_OPERATOR_NAME_SHORTFORM) {
        AppExecFwk::InnerEvent::Pointer eventShortCphs = BuildCallerInfo(MSG_SIM_OBTAIN_SPN_DONE);
        fileController_->ObtainBinaryFile(ELEMENTARY_FILE_SPN_SHORT_CPHS, eventShortCphs);
        fileToGet_++;
    }
}

void SimFile::ProcessSpnShortCphs(const AppExecFwk::InnerEvent::Pointer &event)
{
    if (event == nullptr) {
        TELEPHONY_LOGE("event is nullptr!");
        return;
    }
    std::unique_ptr<ControllerToFileMsg> fd = event->GetUniqueObject<ControllerToFileMsg>();
    if (fd != nullptr && fd->exception == nullptr) {
        std::string iccData = fd->resultData;
        UpdateSPN(ParseSpn(iccData, spnStatus_));
        std::string spn = ObtainSPN();
        if (spn.empty() || !spn.size()) {
            TELEPHONY_LOGI("SimFile No SPN loaded");
        } else {
            displayConditionOfSpn_ = 0;
            TELEPHONY_LOGI("SimFile Load ELEMENTARY_FILE_SPN_SHORT_CPHS");
        }
    } else {
        UpdateSPN(IccFileController::NULLSTR);
        TELEPHONY_LOGI("SimFile No SPN get in either CHPS or 3GPP");
    }
    spnStatus_ = SpnStatus::OBTAIN_SPN_NONE;
}

std::string SimFile::ParseSpn(const std::string &rawData, int spnStatus)
{
    int offset = 0;
    int length = 0;
    std::shared_ptr<unsigned char> bytesRaw = SIMUtils::HexStringConvertToBytes(rawData, length);
    std::shared_ptr<unsigned char> bytesNew = nullptr;
    if (bytesRaw == nullptr) {
        TELEPHONY_LOGI("ParseSpn invalid data: %{public}s", rawData.c_str());
        return "";
    }
    if (spnStatus == OBTAIN_SPN_GENERAL) {
        offset = 0;
        length -= INVALID_BYTES_NUM;
        bytesNew = std::shared_ptr<unsigned char>(
            bytesRaw.get() + INVALID_BYTES_NUM, [bytesRaw](unsigned char *) {}); // first is 0, +1
    } else if ((spnStatus == OBTAIN_OPERATOR_NAMESTRING) || (spnStatus == OBTAIN_OPERATOR_NAME_SHORTFORM)) {
        offset = 0;
        bytesNew = bytesRaw;
    } else {
        return "";
    }
    std::string ret = SIMUtils::DiallingNumberStringFieldConvertToString(bytesNew, offset, length, SPN_CHAR_POS);
    TELEPHONY_LOGI("SimFile::ParseSpn success");
    return ret;
}

void SimFile::ParsePnn(const std::vector<std::string> &records)
{
    pnnFiles_.clear();
    if (records.empty()) {
        TELEPHONY_LOGI("ParsePnn records is empty");
        return;
    }
    for (const auto &dataPnn : records) {
        TELEPHONY_LOGI("ParsePnn: %{public}s", dataPnn.c_str());
        int recordLen = 0;
        std::shared_ptr<unsigned char> data = SIMUtils::HexStringConvertToBytes(dataPnn, recordLen);
        if (data == nullptr) {
            TELEPHONY_LOGI("ParsePnn data is nullptr");
            continue;
        }
        unsigned char *tlv = data.get();
        std::shared_ptr<PlmnNetworkName> file = std::make_shared<PlmnNetworkName>();
        int tagAndLength = NETWORK_NAME_LENGTH + 1;
        if (recordLen > tagAndLength) {
            if (recordLen >= (tagAndLength + static_cast<int>(tlv[NETWORK_NAME_LENGTH])) &&
                tlv[NETWORK_NAME_IEI] == (unsigned char)LONG_NAME_FLAG) {
                file->longName =
                    SIMUtils::Gsm7bitConvertToString(tlv + NETWORK_NAME_TEXT_STRING, tlv[NETWORK_NAME_LENGTH] - 1);
            }
            int shortNameOffset = tagAndLength + tlv[NETWORK_NAME_LENGTH];
            if (recordLen > (shortNameOffset + tagAndLength)) {
                if (recordLen >= (shortNameOffset + tagAndLength +
                                     static_cast<int>(tlv[shortNameOffset + NETWORK_NAME_LENGTH])) &&
                    tlv[shortNameOffset + NETWORK_NAME_IEI] == (unsigned char)SHORT_NAME_FLAG) {
                    file->shortName =
                        SIMUtils::Gsm7bitConvertToString(tlv + (shortNameOffset + NETWORK_NAME_TEXT_STRING),
                            tlv[shortNameOffset + NETWORK_NAME_LENGTH] - 1);
                }
            }
        }
        TELEPHONY_LOGI("longName: %{public}s, shortName: %{public}s", file->longName.c_str(), file->shortName.c_str());
        if (!file->longName.empty() || !file->shortName.empty()) {
            pnnFiles_.push_back(file);
        }
    }
}

void SimFile::ParseOpl(const std::vector<std::string> &records)
{
    oplFiles_.clear();
    if (records.empty()) {
        TELEPHONY_LOGI("ParseOpl records is empty");
        return;
    }
    for (const auto &dataOpl : records) {
        TELEPHONY_LOGI("ParseOpl: %{public}s", dataOpl.c_str());
        if (dataOpl.size() != (BYTE_LENGTH + BYTE_LENGTH)) {
            continue;
        }
        std::string plmn = SIMUtils::BcdPlmnConvertToString(dataOpl, 0);
        if (plmn.empty()) {
            continue;
        }
        std::shared_ptr<OperatorPlmnInfo> file = std::make_shared<OperatorPlmnInfo>();
        file->plmnNumeric = plmn;
        int base = 16; // convert to hexadecimal
        file->lacStart = stoi(dataOpl.substr(MCCMNC_LEN, HALF_BYTE_LEN), 0, base);
        file->lacEnd = stoi(dataOpl.substr(MCCMNC_LEN + HALF_BYTE_LEN, HALF_BYTE_LEN), 0, base);
        file->pnnRecordId = stoi(dataOpl.substr(MCCMNC_LEN + BYTE_LENGTH, HALF_LEN), 0, base);
        TELEPHONY_LOGI("plmnNumeric: %{public}s, lacStart: %{public}d, lacEnd: %{public}d, pnnRecordId: %{public}d",
            file->plmnNumeric.c_str(), file->lacStart, file->lacEnd, file->pnnRecordId);
        oplFiles_.push_back(file);
    }
}

std::shared_ptr<UsimFunctionHandle> SimFile::ObtainUsimFunctionHandle()
{
    return UsimFunctionHandle_;
}

void SimFile::UpdateSimLanguage()
{
    AppExecFwk::InnerEvent::Pointer eventLILAN = BuildCallerInfo(MSG_SIM_OBTAIN_LI_LANGUAGE_DONE);
    fileController_->ObtainBinaryFile(ELEMENTARY_FILE_LI, eventLILAN);
}

bool SimFile::ProcessObtainLiLanguage(const AppExecFwk::InnerEvent::Pointer &event)
{
    if (event == nullptr) {
        TELEPHONY_LOGE("event is nullptr!");
        return true;
    }
    std::unique_ptr<ControllerToFileMsg> fd = event->GetUniqueObject<ControllerToFileMsg>();
    if (fd != nullptr && fd->exception == nullptr) {
        efLi_ = fd->resultData;
        if (efLi_.empty() || !efLi_.size()) {
            TELEPHONY_LOGI("efLi_ No language loaded");
            AppExecFwk::InnerEvent::Pointer eventPLLAN = BuildCallerInfo(MSG_SIM_OBTAIN_PL_LANGUAGE_DONE);
            fileController_->ObtainBinaryFile(ELEMENTARY_FILE_PL, eventPLLAN);
        } else {
            TELEPHONY_LOGI("efLi_  language loaded");
            UpdateIccLanguage(efLi_, efPl_);
        }
    }
    return true;
}

bool SimFile::ProcessObtainPlLanguage(const AppExecFwk::InnerEvent::Pointer &event)
{
    if (event == nullptr) {
        TELEPHONY_LOGE("event is nullptr!");
        return true;
    }
    std::unique_ptr<ControllerToFileMsg> fd = event->GetUniqueObject<ControllerToFileMsg>();
    if (fd != nullptr && fd->exception == nullptr) {
        efPl_ = fd->resultData;
        if (efPl_.empty() || !efPl_.size()) {
            TELEPHONY_LOGI("efPl_ No language loaded");
        } else {
            TELEPHONY_LOGI("efPl_  language loaded");
            UpdateIccLanguage(efLi_, efPl_);
        }
    }
    return true;
}

std::string SimFile::AnalysisBcdPlmn(std::string data, std::string description)
{
    return "";
}

void SimFile::ProcessElementaryFileCsp(std::string data) {}

void SimFile::AnalysisElementaryFileSpdi(std::string data) {}

void SimFile::ProcessSmses(std::string messages) {}

void SimFile::ProcessSms(std::string data) {}

bool SimFile::ProcessObtainGid1Done(const AppExecFwk::InnerEvent::Pointer &event)
{
    bool isFileProcessResponse = true;
    if (event == nullptr) {
        TELEPHONY_LOGE("event is nullptr!");
        return isFileProcessResponse;
    }
    std::unique_ptr<ControllerToFileMsg> fd = event->GetUniqueObject<ControllerToFileMsg>();
    if (fd == nullptr) {
        TELEPHONY_LOGE("fd is nullptr!");
        return isFileProcessResponse;
    }
    std::string iccData = fd->resultData;
    char *rawData = const_cast<char *>(iccData.c_str());
    unsigned char *fileData = reinterpret_cast<unsigned char *>(rawData);

    if (fd->exception != nullptr) {
        TELEPHONY_LOGE("SimFile failed in get GID1 ");
        gid1_ = "";
        return isFileProcessResponse;
    }

    gid1_ = iccData;
    TELEPHONY_LOGI("SimFile GID1: %{public}s", fileData);
    return isFileProcessResponse;
}

bool SimFile::ProcessObtainGid2Done(const AppExecFwk::InnerEvent::Pointer &event)
{
    bool isFileProcessResponse = true;
    if (event == nullptr) {
        TELEPHONY_LOGE("event is nullptr!");
        return isFileProcessResponse;
    }
    std::unique_ptr<ControllerToFileMsg> fd = event->GetUniqueObject<ControllerToFileMsg>();
    if (fd == nullptr) {
        TELEPHONY_LOGE("fd is nullptr!");
        return isFileProcessResponse;
    }
    std::string iccData = fd->resultData;
    char *rawData = const_cast<char *>(iccData.c_str());
    unsigned char *fileData = reinterpret_cast<unsigned char *>(rawData);

    if (fd->exception != nullptr) {
        TELEPHONY_LOGE("SimFile failed in get GID2 ");
        gid2_ = "";
        return isFileProcessResponse;
    }

    gid2_ = iccData;
    TELEPHONY_LOGI("SimFile GID2: %{public}s", fileData);
    return isFileProcessResponse;
}

bool SimFile::ProcessGetMsisdnDone(const AppExecFwk::InnerEvent::Pointer &event)
{
    bool isFileProcessResponse = true;
    if (event == nullptr) {
        TELEPHONY_LOGE("event is nullptr!");
        return isFileProcessResponse;
    }
    std::unique_ptr<DiallingNumbersHandlerResult> fd = event->GetUniqueObject<DiallingNumbersHandlerResult>();
    if (fd == nullptr) {
        TELEPHONY_LOGE("fd is nullptr!");
        return isFileProcessResponse;
    }
    std::shared_ptr<DiallingNumbersInfo> diallingNumber = std::static_pointer_cast<DiallingNumbersInfo>(fd->result);
    if (fd->exception != nullptr) {
        TELEPHONY_LOGE("SimFile Invalid or missing EF[MSISDN]");
        return isFileProcessResponse;
    }
    msisdn_ = Str16ToStr8(diallingNumber->GetNumber());
    msisdnTag_ = Str16ToStr8(diallingNumber->GetName());
    return isFileProcessResponse;
}

bool SimFile::ProcessSetMsisdnDone(const AppExecFwk::InnerEvent::Pointer &event)
{
    bool isFileProcessResponse = false;
    if (event == nullptr) {
        TELEPHONY_LOGE("event is nullptr!");
        return isFileProcessResponse;
    }
    std::unique_ptr<ControllerToFileMsg> fd = event->GetUniqueObject<ControllerToFileMsg>();
    if (fd == nullptr) {
        TELEPHONY_LOGE("fd is nullptr!");
        return isFileProcessResponse;
    }
    std::string iccData = fd->resultData;
    if (fd->exception == nullptr) {
        msisdn_ = lastMsisdn_;
        msisdnTag_ = lastMsisdnTag_;
        TELEPHONY_LOGI("SimFile Success to update EF[MSISDN]");
    }
    return isFileProcessResponse;
}

bool SimFile::ProcessGetSpdiDone(const AppExecFwk::InnerEvent::Pointer &event)
{
    bool isFileProcessResponse = true;
    if (event == nullptr) {
        TELEPHONY_LOGE("event is nullptr!");
        return isFileProcessResponse;
    }
    std::unique_ptr<ControllerToFileMsg> fd = event->GetUniqueObject<ControllerToFileMsg>();
    if (fd == nullptr) {
        TELEPHONY_LOGE("fd is nullptr!");
        return isFileProcessResponse;
    }
    std::string iccData = fd->resultData;
    char *rawData = const_cast<char *>(iccData.c_str());
    unsigned char *fileData = reinterpret_cast<unsigned char *>(rawData);

    if (fd->exception != nullptr) {
        return isFileProcessResponse;
    }
    TELEPHONY_LOGI("SimFile MSG_SIM_OBTAIN_SPDI_DONE data:%{public}s", fileData);
    AnalysisElementaryFileSpdi(iccData);
    return isFileProcessResponse;
}

bool SimFile::ProcessGetCfisDone(const AppExecFwk::InnerEvent::Pointer &event)
{
    bool isFileProcessResponse = true;
    if (event == nullptr) {
        TELEPHONY_LOGE("event is nullptr!");
        return isFileProcessResponse;
    }
    std::unique_ptr<ControllerToFileMsg> fd = event->GetUniqueObject<ControllerToFileMsg>();
    if (fd == nullptr) {
        TELEPHONY_LOGE("fd is nullptr!");
        return isFileProcessResponse;
    }
    std::string iccData = fd->resultData;
    char *rawData = const_cast<char *>(iccData.c_str());
    unsigned char *fileData = reinterpret_cast<unsigned char *>(rawData);

    if (fd->exception != nullptr) {
        efCfis_ = nullptr;
    } else {
        TELEPHONY_LOGI("ELEMENTARY_FILE_CFIS: %{public}s", fileData);
        efCfis_ = fileData;
    }
    return isFileProcessResponse;
}

bool SimFile::ProcessGetMbiDone(const AppExecFwk::InnerEvent::Pointer &event)
{
    bool isFileProcessResponse = true;
    if (event == nullptr) {
        TELEPHONY_LOGE("event is nullptr!");
        return isFileProcessResponse;
    }
    std::unique_ptr<ControllerToFileMsg> fd = event->GetUniqueObject<ControllerToFileMsg>();
    if (fd == nullptr) {
        TELEPHONY_LOGE("fd is nullptr!");
        return isFileProcessResponse;
    }
    std::string iccData = fd->resultData;
    char *rawData = const_cast<char *>(iccData.c_str());
    unsigned char *fileData = reinterpret_cast<unsigned char *>(rawData);
    if (fd->exception == nullptr) {
        int dataLen = 0;
        std::shared_ptr<unsigned char> dataByte = SIMUtils::HexStringConvertToBytes(iccData, dataLen);
        int index = (dataByte != nullptr) ? (dataByte.get()[0] & BYTE_NUM) : 0;
        if (index != 0 && index != BYTE_NUM) {
            indexOfMailbox_ = index;
            TELEPHONY_LOGI("fetch valid mailbox number for MBDN");
        }
    }
    TELEPHONY_LOGI("ELEMENTARY_FILE_MBI data is:%{public}s id: %{public}d", fileData, indexOfMailbox_);
    fileToGet_ += LOAD_STEP;
    AppExecFwk::InnerEvent::Pointer mbdnEvent = CreateDiallingNumberPointer(MSG_SIM_OBTAIN_MBDN_DONE, 0, 0, nullptr);
    diallingNumberHandler_->GetDiallingNumbers(ELEMENTARY_FILE_MBDN, ELEMENTARY_FILE_EXT6, indexOfMailbox_, mbdnEvent);
    return isFileProcessResponse;
}

bool SimFile::ProcessGetMbdnDone(const AppExecFwk::InnerEvent::Pointer &event)
{
    bool isFileProcessResponse = true;
    if (event == nullptr) {
        TELEPHONY_LOGE("event is nullptr!");
        return isFileProcessResponse;
    }
    std::unique_ptr<DiallingNumbersHandlerResult> fd = event->GetUniqueObject<DiallingNumbersHandlerResult>();
    if (fd == nullptr) {
        TELEPHONY_LOGE("fd is nullptr!");
        return isFileProcessResponse;
    }
    bool hasException = fd->exception == nullptr;
    TELEPHONY_LOGI("ProcessGetMbdnDone start %{public}d", hasException);
    voiceMailNum_ = IccFileController::NULLSTR;
    voiceMailTag_ = IccFileController::NULLSTR;
    if (fd->exception != nullptr) {
        TELEPHONY_LOGE("SimFile failed missing EF MBDN");
        GetCphsMailBox();
        return isFileProcessResponse;
    }
    std::shared_ptr<DiallingNumbersInfo> diallingNumber = std::static_pointer_cast<DiallingNumbersInfo>(fd->result);
    if (diallingNumber == nullptr) {
        TELEPHONY_LOGE("ProcessGetMbdnDone get null diallingNumber!!");
        return isFileProcessResponse;
    }

    if (diallingNumber->IsEmpty()) {
        GetCphsMailBox();
        return isFileProcessResponse;
    }
    voiceMailNum_ = Str16ToStr8(diallingNumber->GetNumber());
    voiceMailTag_ = Str16ToStr8(diallingNumber->GetName());
    TELEPHONY_LOGI("ProcessGetMbdnDone success");
    return isFileProcessResponse;
}

bool SimFile::ProcessGetCphsMailBoxDone(const AppExecFwk::InnerEvent::Pointer &event)
{
    bool isFileProcessResponse = true;
    if (event == nullptr) {
        TELEPHONY_LOGE("event is nullptr!");
        return isFileProcessResponse;
    }
    std::unique_ptr<DiallingNumbersHandlerResult> fd = event->GetUniqueObject<DiallingNumbersHandlerResult>();
    if (fd == nullptr) {
        TELEPHONY_LOGE("fd is nullptr!");
        return isFileProcessResponse;
    }
    bool hasException = fd->exception == nullptr;
    TELEPHONY_LOGI("ProcessGetCphsMailBoxDone start %{public}d", hasException);
    voiceMailNum_ = IccFileController::NULLSTR;
    voiceMailTag_ = IccFileController::NULLSTR;
    if (fd->exception != nullptr) {
        TELEPHONY_LOGE("SimFile failed missing CPHS MAILBOX");
        return isFileProcessResponse;
    }
    std::shared_ptr<DiallingNumbersInfo> diallingNumber = std::static_pointer_cast<DiallingNumbersInfo>(fd->result);
    if (diallingNumber == nullptr) {
        TELEPHONY_LOGE("GetCphsMailBoxDone get null diallingNumber!!");
        return isFileProcessResponse;
    }

    voiceMailNum_ = Str16ToStr8(diallingNumber->GetNumber());
    voiceMailTag_ = Str16ToStr8(diallingNumber->GetName());
    TELEPHONY_LOGI("GetCphsMailBoxDone success");
    return isFileProcessResponse;
}

void SimFile::GetCphsMailBox()
{
    fileToGet_ += LOAD_STEP;
    AppExecFwk::InnerEvent::Pointer cphsEvent =
        CreateDiallingNumberPointer(MSG_SIM_OBTAIN_CPHS_MAILBOX_DONE, 0, 0, nullptr);
    diallingNumberHandler_->GetDiallingNumbers(ELEMENTARY_FILE_MAILBOX_CPHS, ELEMENTARY_FILE_EXT1, 1, cphsEvent);
}

bool SimFile::ProcessGetMwisDone(const AppExecFwk::InnerEvent::Pointer &event)
{
    bool isFileProcessResponse = true;
    if (event == nullptr) {
        TELEPHONY_LOGE("event is nullptr!");
        return isFileProcessResponse;
    }
    std::unique_ptr<ControllerToFileMsg> fd = event->GetUniqueObject<ControllerToFileMsg>();
    if (fd == nullptr) {
        TELEPHONY_LOGE("fd is nullptr!");
        return isFileProcessResponse;
    }
    std::string iccData = fd->resultData;
    char *rawData = const_cast<char *>(iccData.c_str());
    unsigned char *fileData = reinterpret_cast<unsigned char *>(rawData);
    TELEPHONY_LOGI("SimFile ELEMENTARY_FILE_MWIS : %{public}s", rawData);
    if (fd->exception != nullptr) {
        TELEPHONY_LOGE("MSG_SIM_OBTAIN_MWIS_DONE exception = ");
        return isFileProcessResponse;
    }
    unsigned char value = fileData[0];
    if ((value & BYTE_NUM) == BYTE_NUM) {
        TELEPHONY_LOGI("SimFiles: Uninitialized record MWIS");
        return isFileProcessResponse;
    }
    efMWIS_ = fileData;
    return isFileProcessResponse;
}

bool SimFile::ProcessVoiceMailCphs(const AppExecFwk::InnerEvent::Pointer &event)
{
    bool isFileProcessResponse = true;
    if (event == nullptr) {
        TELEPHONY_LOGE("event is nullptr!");
        return isFileProcessResponse;
    }
    std::unique_ptr<ControllerToFileMsg> fd = event->GetUniqueObject<ControllerToFileMsg>();
    if (fd == nullptr) {
        TELEPHONY_LOGE("fd is nullptr!");
        return isFileProcessResponse;
    }
    std::string iccData = fd->resultData;
    char *rawData = const_cast<char *>(iccData.c_str());
    unsigned char *fileData = reinterpret_cast<unsigned char *>(rawData);
    TELEPHONY_LOGI("SimFile ELEMENTARY_FILE_CPHS_MWI: %{public}s", rawData);
    if (fd->exception != nullptr) {
        TELEPHONY_LOGE("MSG_SIM_OBTAIN_VOICE_MAIL_INDICATOR_CPHS_DONE exception = ");
        return isFileProcessResponse;
    }
    efCphsMwi_ = fileData;
    return isFileProcessResponse;
}

bool SimFile::ProcessGetIccIdDone(const AppExecFwk::InnerEvent::Pointer &event)
{
    bool isFileProcessResponse = true;
    if (event == nullptr) {
        TELEPHONY_LOGE("event is nullptr!");
        return isFileProcessResponse;
    }
    std::unique_ptr<ControllerToFileMsg> fd = event->GetUniqueObject<ControllerToFileMsg>();
    if (fd == nullptr) {
        TELEPHONY_LOGE("fd is nullptr!");
        return isFileProcessResponse;
    }
    if (fd->exception == nullptr) {
        std::string iccData = fd->resultData;
        TELEPHONY_LOGI("SimFile::ProcessEvent ICCID result success");
        iccId_ = iccData;
    }
    return isFileProcessResponse;
}

bool SimFile::ProcessObtainIMSIDone(const AppExecFwk::InnerEvent::Pointer &event)
{
    bool isFileProcessResponse = true;
    if (event == nullptr) {
        TELEPHONY_LOGE("event is nullptr!");
        return isFileProcessResponse;
    }
    std::shared_ptr<std::string> sharedObject = event->GetSharedObject<std::string>();
    if (sharedObject == nullptr) {
        TELEPHONY_LOGE("fd is nullptr!");
        return isFileProcessResponse;
    }
    if (sharedObject != nullptr) {
        imsi_ = *sharedObject;
        TELEPHONY_LOGI("SimFile::ProcessEvent IMSI received success");
        SaveCountryCode();
        TELEPHONY_LOGI("SimFile::ObtainIsoCountryCode result success");
        if (!imsi_.empty()) {
            imsiReadyObser_->NotifyObserver(RadioEvent::RADIO_IMSI_LOADED_READY);
        }
    }
    return isFileProcessResponse;
}

bool SimFile::ProcessGetCffDone(const AppExecFwk::InnerEvent::Pointer &event)
{
    bool isFileProcessResponse = true;
    if (event == nullptr) {
        TELEPHONY_LOGE("event is nullptr!");
        return isFileProcessResponse;
    }
    std::unique_ptr<ControllerToFileMsg> fd = event->GetUniqueObject<ControllerToFileMsg>();
    if (fd == nullptr) {
        TELEPHONY_LOGE("fd is nullptr!");
        return isFileProcessResponse;
    }
    std::string iccData = fd->resultData;
    char *rawData = const_cast<char *>(iccData.c_str());
    unsigned char *fileData = reinterpret_cast<unsigned char *>(rawData);
    if (fd->exception != nullptr) {
        efCff_ = nullptr;
    } else {
        TELEPHONY_LOGI("SimFile ELEMENTARY_FILE_CFF_CPHS: %{public}s", rawData);
        efCff_ = fileData;
    }
    return isFileProcessResponse;
}

bool SimFile::ProcessGetAdDone(const AppExecFwk::InnerEvent::Pointer &event)
{
    bool isFileProcessResponse = true;
    if (event == nullptr) {
        TELEPHONY_LOGE("event is nullptr!");
        return isFileProcessResponse;
    }
    std::unique_ptr<ControllerToFileMsg> fd = event->GetUniqueObject<ControllerToFileMsg>();
    if (fd == nullptr) {
        TELEPHONY_LOGE("fd is nullptr!");
        return isFileProcessResponse;
    }
    std::string iccData = fd->resultData;
    bool doneData = true;
    if (!ObtainIMSI().empty()) {
        std::string imsi = ObtainIMSI();
        int mcc = atoi(imsi.substr(0, MCC_LEN).c_str());
        lengthOfMnc_ = MccPool::ShortestMncLengthFromMcc(mcc);
        TELEPHONY_LOGI("SimFile [TestMode] lengthOfMnc_= %{public}d", lengthOfMnc_);
    } else {
        char *rawData = const_cast<char *>(iccData.c_str());
        unsigned char *fileData = reinterpret_cast<unsigned char *>(rawData);
        if (fd->exception != nullptr) {
            doneData = false;
        }
        TELEPHONY_LOGI("SimFile ELEMENTARY_FILE_AD: %{public}s", rawData);
        int dataSize = static_cast<int>(iccData.size());
        if (dataSize <= MCC_LEN) {
            TELEPHONY_LOGI("SimFile MNC length dataSize = %{public}d", dataSize);
            doneData = false;
        }
        if (doneData) {
            lengthOfMnc_ = fileData[MCC_LEN] & 0xf;
            TELEPHONY_LOGI("setting4 lengthOfMnc_= %{public}d", lengthOfMnc_);
        }
    }

    if (doneData && (lengthOfMnc_ == 0xf)) {
        lengthOfMnc_ = UNKNOWN_MNC;
    } else if (doneData && (lengthOfMnc_ != MNC_LEN) && (lengthOfMnc_ != MCC_LEN)) {
        lengthOfMnc_ = UNINITIALIZED_MNC;
    }
    TELEPHONY_LOGI("update5 length Mnc_= %{public}d", lengthOfMnc_);
    CheckMncLength();
    return isFileProcessResponse;
}

void SimFile::CheckMncLength()
{
    std::string imsi = ObtainIMSI();
    int imsiSize = static_cast<int>(imsi.size());
    if (((lengthOfMnc_ == UNINITIALIZED_MNC) || (lengthOfMnc_ == UNKNOWN_MNC) || (lengthOfMnc_ == MNC_LEN)) &&
        ((!imsi.empty()) && (imsiSize >= MCCMNC_LEN))) {
        std::string mccMncCode = imsi.substr(0, MCCMNC_LEN);
        TELEPHONY_LOGI("SimFile mccMncCode= %{public}s", mccMncCode.c_str());
        if (MccPool::LengthIsThreeMnc(mccMncCode)) {
            lengthOfMnc_ = MCC_LEN;
            TELEPHONY_LOGI("SimFile update6 lengthOfMnc_= %{public}d", lengthOfMnc_);
        }
    }

    if (lengthOfMnc_ == UNKNOWN_MNC || lengthOfMnc_ == UNINITIALIZED_MNC) {
        if (!imsi.empty()) {
            int mcc = atoi(imsi.substr(0, MCC_LEN).c_str());
            lengthOfMnc_ = MccPool::ShortestMncLengthFromMcc(mcc);
            TELEPHONY_LOGI("SimFile update7 lengthOfMnc_= %{public}d", lengthOfMnc_);
        } else {
            lengthOfMnc_ = UNKNOWN_MNC;
            TELEPHONY_LOGI(
                "MNC length not present in ELEMENTARY_FILE_AD setting9 lengthOfMnc_= %{public}d", lengthOfMnc_);
        }
    }
    int lenNum = MCC_LEN + lengthOfMnc_;
    int sz = static_cast<int>(imsi.size());
    bool cond = sz >= lenNum;
    if ((!imsi.empty()) && (lengthOfMnc_ != UNKNOWN_MNC) && cond) {
        operatorNumeric_ = imsi.substr(0, lenNum);
    }
}

bool SimFile::ProcessSmsOnSim(const AppExecFwk::InnerEvent::Pointer &event)
{
    bool isFileProcessResponse = false;
    if (event == nullptr) {
        TELEPHONY_LOGE("event is nullptr!");
        return isFileProcessResponse;
    }
    std::unique_ptr<ControllerToFileMsg> fd = event->GetUniqueObject<ControllerToFileMsg>();
    if (fd == nullptr) {
        TELEPHONY_LOGE("fd is nullptr!");
        return isFileProcessResponse;
    }
    std::string iccData = fd->resultData;
    int index = atoi(iccData.c_str());
    if (fd->exception != nullptr || index == INVALID_VALUE) {
        TELEPHONY_LOGE("exception on SMS_ON_SIM with index: %{public}d", index);
    } else {
        TELEPHONY_LOGI("READ ELEMENTARY_FILE_SMS RECORD index= %{public}d", index);
        AppExecFwk::InnerEvent::Pointer eventSMS = BuildCallerInfo(MSG_SIM_OBTAIN_SMS_DONE);
        fileController_->ObtainLinearFixedFile(ELEMENTARY_FILE_SMS, index, eventSMS);
    }
    return isFileProcessResponse;
}

bool SimFile::ProcessGetAllSmsDone(const AppExecFwk::InnerEvent::Pointer &event)
{
    bool isFileProcessResponse = true;
    if (event == nullptr) {
        TELEPHONY_LOGE("event is nullptr!");
        return isFileProcessResponse;
    }
    std::unique_ptr<ControllerToFileMsg> fd = event->GetUniqueObject<ControllerToFileMsg>();
    if (fd == nullptr) {
        TELEPHONY_LOGE("fd is nullptr!");
        return isFileProcessResponse;
    }
    std::string iccData = fd->resultData;
    if (fd->exception != nullptr) {
        return isFileProcessResponse;
    }
    ProcessSmses(iccData);
    return isFileProcessResponse;
}

bool SimFile::ProcessGetSmsDone(const AppExecFwk::InnerEvent::Pointer &event)
{
    bool isFileProcessResponse = false;
    if (event == nullptr) {
        TELEPHONY_LOGE("event is nullptr!");
        return isFileProcessResponse;
    }
    std::unique_ptr<ControllerToFileMsg> fd = event->GetUniqueObject<ControllerToFileMsg>();
    if (fd == nullptr) {
        TELEPHONY_LOGE("fd is nullptr!");
        return isFileProcessResponse;
    }
    std::string iccData = fd->resultData;
    if (fd->exception == nullptr) {
        ProcessSms(iccData);
    } else {
        TELEPHONY_LOGI("SimFile exception on GET_SMS ");
    }
    return isFileProcessResponse;
}

bool SimFile::ProcessGetPlmnActDone(const AppExecFwk::InnerEvent::Pointer &event)
{
    bool isFileProcessResponse = true;
    if (event == nullptr) {
        TELEPHONY_LOGE("event is nullptr!");
        return isFileProcessResponse;
    }
    std::unique_ptr<ControllerToFileMsg> fd = event->GetUniqueObject<ControllerToFileMsg>();
    if (fd == nullptr) {
        TELEPHONY_LOGE("fd is nullptr!");
        return isFileProcessResponse;
    }
    std::string iccData = fd->resultData;
    char *rawData = const_cast<char *>(iccData.c_str());
    unsigned char *fileData = reinterpret_cast<unsigned char *>(rawData);

    if (fd->exception != nullptr || iccData.empty()) {
        TELEPHONY_LOGE("Failed fetch User PLMN ");
    } else {
        TELEPHONY_LOGI("fetch a PlmnRAT, iccData= %{public}s", fileData);
    }
    return isFileProcessResponse;
}

// Process operator plmn
bool SimFile::ProcessGetOplmnActDone(const AppExecFwk::InnerEvent::Pointer &event)
{
    bool isFileProcessResponse = true;
    if (event == nullptr) {
        TELEPHONY_LOGE("event is nullptr!");
        return isFileProcessResponse;
    }
    std::unique_ptr<ControllerToFileMsg> fd = event->GetUniqueObject<ControllerToFileMsg>();
    if (fd == nullptr) {
        TELEPHONY_LOGE("fd is nullptr!");
        return isFileProcessResponse;
    }
    std::string iccData = fd->resultData;
    char *rawData = const_cast<char *>(iccData.c_str());
    unsigned char *fileData = reinterpret_cast<unsigned char *>(rawData);

    if (fd->exception != nullptr || iccData.empty()) {
        TELEPHONY_LOGE("Failed fetch Operator PLMN");
    } else {
        TELEPHONY_LOGI("fetch a OPlmnRAT, iccData= %{public}s", fileData);
    }
    return isFileProcessResponse;
}

bool SimFile::ProcessGetCspCphs(const AppExecFwk::InnerEvent::Pointer &event)
{
    bool isFileProcessResponse = true;
    if (event == nullptr) {
        TELEPHONY_LOGE("event is nullptr!");
        return isFileProcessResponse;
    }
    std::unique_ptr<ControllerToFileMsg> fd = event->GetUniqueObject<ControllerToFileMsg>();
    if (fd == nullptr) {
        TELEPHONY_LOGE("fd is nullptr!");
        return isFileProcessResponse;
    }
    std::string iccData = fd->resultData;
    char *rawData = const_cast<char *>(iccData.c_str());
    unsigned char *fileData = reinterpret_cast<unsigned char *>(rawData);

    if (fd->exception != nullptr) {
        TELEPHONY_LOGE("Exception to get ELEMENTARY_FILE_CSP data ");
        return isFileProcessResponse;
    }
    TELEPHONY_LOGI("SimFile MSG_SIM_OBTAIN_CSP_CPHS_DONE data:%{public}s", fileData);
    TELEPHONY_LOGI("ELEMENTARY_FILE_CSP: %{public}s", fileData);
    ProcessElementaryFileCsp(iccData);
    return isFileProcessResponse;
}

bool SimFile::ProcessGetInfoCphs(const AppExecFwk::InnerEvent::Pointer &event)
{
    bool isFileProcessResponse = true;
    if (event == nullptr) {
        TELEPHONY_LOGE("event is nullptr!");
        return isFileProcessResponse;
    }
    std::unique_ptr<ControllerToFileMsg> fd = event->GetUniqueObject<ControllerToFileMsg>();
    if (fd == nullptr) {
        TELEPHONY_LOGE("fd is nullptr!");
        return isFileProcessResponse;
    }
    if (fd->exception != nullptr) {
        return isFileProcessResponse;
    }
    cphsInfo_ = fd->resultData;
    TELEPHONY_LOGI("SimFile::ProcessGetInfoCphs success");
    return isFileProcessResponse;
}

bool SimFile::ProcessGetSstDone(const AppExecFwk::InnerEvent::Pointer &event)
{
    bool isFileProcessResponse = true;
    if (event == nullptr) {
        TELEPHONY_LOGE("event is nullptr!");
        return isFileProcessResponse;
    }
    std::unique_ptr<ControllerToFileMsg> fd = event->GetUniqueObject<ControllerToFileMsg>();
    if (fd == nullptr) {
        TELEPHONY_LOGE("fd is nullptr!");
        return isFileProcessResponse;
    }
    std::string iccData = fd->resultData;
    char *rawData = const_cast<char *>(iccData.c_str());
    unsigned char *fileData = reinterpret_cast<unsigned char *>(rawData);

    if (fd->exception != nullptr) {
        return isFileProcessResponse;
    }
    TELEPHONY_LOGI("SimFile MSG_SIM_OBTAIN_SST_DONE data:%{public}s", fileData);
    return isFileProcessResponse;
}

bool SimFile::ProcessGetPnnDone(const AppExecFwk::InnerEvent::Pointer &event)
{
    TELEPHONY_LOGI("ProcessGetPnnDone: start");
    bool isFileProcessResponse = true;
    if (event == nullptr) {
        TELEPHONY_LOGE("event is nullptr!");
        return isFileProcessResponse;
    }
    std::unique_ptr<ControllerToFileMsg> fd = event->GetUniqueObject<ControllerToFileMsg>();
    if (fd != nullptr) {
        if (fd->exception != nullptr) {
            TELEPHONY_LOGE("ProcessGetPnnDone: get error result");
            return isFileProcessResponse;
        }
    } else {
        std::shared_ptr<MultiRecordResult> object = event->GetSharedObject<MultiRecordResult>();
        if (object != nullptr) {
            TELEPHONY_LOGI("ProcessGetPnnDone: %{public}d", object->resultLength);
            if (object->exception == nullptr) {
                ParsePnn(object->fileResults);
            }
            for (std::string str : object->fileResults) {
                TELEPHONY_LOGI("ProcessGetPnnDone: %{public}s", str.c_str());
            }
        } else {
            TELEPHONY_LOGE("ProcessGetPnnDone: get null pointer!!!");
        }
    }
    return isFileProcessResponse;
}

bool SimFile::ProcessGetOplDone(const AppExecFwk::InnerEvent::Pointer &event)
{
    TELEPHONY_LOGI("ProcessGetOplDone: start");
    bool isFileProcessResponse = true;
    if (event == nullptr) {
        TELEPHONY_LOGE("event is nullptr!");
        return isFileProcessResponse;
    }
    std::unique_ptr<ControllerToFileMsg> fd = event->GetUniqueObject<ControllerToFileMsg>();
    if (fd != nullptr) {
        if (fd->exception != nullptr) {
            TELEPHONY_LOGE("ProcessGetOplDone: get error result");
            return isFileProcessResponse;
        }
    } else {
        std::shared_ptr<MultiRecordResult> object = event->GetSharedObject<MultiRecordResult>();
        if (object != nullptr) {
            TELEPHONY_LOGI("ProcessGetOplDone: %{public}d", object->resultLength);
            if (object->exception == nullptr) {
                ParseOpl(object->fileResults);
            }
            for (std::string str : object->fileResults) {
                TELEPHONY_LOGI("ProcessGetOplDone: %{public}s", str.c_str());
            }
        } else {
            TELEPHONY_LOGE("ProcessGetOplDone: get null pointer!!!");
        }
    }
    return isFileProcessResponse;
}

bool SimFile::ProcessUpdateDone(const AppExecFwk::InnerEvent::Pointer &event)
{
    bool isFileProcessResponse = false;
    if (event == nullptr) {
        TELEPHONY_LOGE("event is nullptr!");
        return isFileProcessResponse;
    }
    std::unique_ptr<ControllerToFileMsg> fd = event->GetUniqueObject<ControllerToFileMsg>();
    if (fd == nullptr) {
        TELEPHONY_LOGE("fd is nullptr!");
        return isFileProcessResponse;
    }
    std::string iccData = fd->resultData;
    if (fd->exception != nullptr) {
        TELEPHONY_LOGE("SimFile failed to update");
    }
    return isFileProcessResponse;
}

bool SimFile::ProcessSetCphsMailbox(const AppExecFwk::InnerEvent::Pointer &event)
{
    bool isFileProcessResponse = true;
    if (event == nullptr) {
        TELEPHONY_LOGE("event is nullptr!");
        return isFileProcessResponse;
    }
    std::unique_ptr<DiallingNumbersHandlerResult> fd = event->GetUniqueObject<DiallingNumbersHandlerResult>();
    std::shared_ptr<DiallingNumbersInfo> diallingNumber = std::static_pointer_cast<DiallingNumbersInfo>(fd->result);
    if (fd->exception == nullptr) {
        voiceMailNum_ = Str16ToStr8(diallingNumber->GetNumber());
        voiceMailTag_ = Str16ToStr8(diallingNumber->GetName());
        waitResult_ = true;
        processWait_.notify_all();
        TELEPHONY_LOGI("set cphs voicemail success");
    } else {
        processWait_.notify_all();
        TELEPHONY_LOGE("set cphs voicemail failed with exception!!");
    }
    return isFileProcessResponse;
}

// Process forbidden PLMNs
bool SimFile::ProcessGetHplmActDone(const AppExecFwk::InnerEvent::Pointer &event)
{
    bool isFileProcessResponse = true;
    if (event == nullptr) {
        TELEPHONY_LOGE("event is nullptr!");
        return isFileProcessResponse;
    }
    std::unique_ptr<ControllerToFileMsg> fd = event->GetUniqueObject<ControllerToFileMsg>();
    if (fd == nullptr) {
        TELEPHONY_LOGE("fd is nullptr!");
        return isFileProcessResponse;
    }
    std::string iccData = fd->resultData;
    char *rawData = const_cast<char *>(iccData.c_str());
    unsigned char *fileData = reinterpret_cast<unsigned char *>(rawData);

    if (fd->exception != nullptr || iccData.empty()) {
        TELEPHONY_LOGE("Failed to fetch forbidden PLMN");
        return isFileProcessResponse;
    } else {
        TELEPHONY_LOGI("fetch a FPlmnRAT, iccData=%{public}s", fileData);
    }
    return isFileProcessResponse;
}

// Process Equivalent Home PLMNs
bool SimFile::ProcessGetEhplmnDone(const AppExecFwk::InnerEvent::Pointer &event)
{
    bool isFileProcessResponse = true;
    if (event == nullptr) {
        TELEPHONY_LOGE("event is nullptr!");
        return isFileProcessResponse;
    }
    std::unique_ptr<ControllerToFileMsg> fd = event->GetUniqueObject<ControllerToFileMsg>();
    if (fd == nullptr) {
        TELEPHONY_LOGE("fd is nullptr!");
        return isFileProcessResponse;
    }
    std::string iccData = fd->resultData;
    if (fd->exception != nullptr || iccData.empty()) {
        TELEPHONY_LOGE("Failed fetch Equivalent Home PLMNs");
        return isFileProcessResponse;
    }
    return isFileProcessResponse;
}

// Process forbidden PLMNs
bool SimFile::ProcessGetFplmnDone(const AppExecFwk::InnerEvent::Pointer &event)
{
    bool loadResponse = true;
    if (event == nullptr) {
        TELEPHONY_LOGE("event is nullptr!");
        return loadResponse;
    }
    std::unique_ptr<ControllerToFileMsg> fd = event->GetUniqueObject<ControllerToFileMsg>();
    if (fd == nullptr) {
        TELEPHONY_LOGE("fd is nullptr!");
        return loadResponse;
    }
    std::string iccData = fd->resultData;
    if (fd->exception != nullptr || iccData.empty()) {
        TELEPHONY_LOGE("Failed to get forbidden PLMNs");
        return loadResponse;
    }
    if (fd->arg1 == ICC_CONTROLLER_REQ_SEND_RESPONSE) {
        TELEPHONY_LOGI("getForbiddenPlmns and send result");
        loadResponse = false;
    }
    return loadResponse;
}

bool SimFile::ProcessSetMbdn(const AppExecFwk::InnerEvent::Pointer &event)
{
    bool isFileProcessResponse = true;
    if (event == nullptr) {
        TELEPHONY_LOGE("event is nullptr!");
        return isFileProcessResponse;
    }
    bool hasNotify = false;
    std::unique_ptr<DiallingNumbersHandlerResult> fd = event->GetUniqueObject<DiallingNumbersHandlerResult>();
    std::shared_ptr<DiallingNumbersInfo> diallingNumber = std::static_pointer_cast<DiallingNumbersInfo>(fd->result);
    if (fd->exception == nullptr) {
        voiceMailNum_ = Str16ToStr8(diallingNumber->GetNumber());
        voiceMailTag_ = Str16ToStr8(diallingNumber->GetName());
        waitResult_ = true;
        processWait_.notify_all();
        hasNotify = true;
        TELEPHONY_LOGI("set voicemail name success");
    }

    if (CphsVoiceMailAvailable()) {
        std::shared_ptr<DiallingNumbersInfo> diallingNumberCphs = std::make_shared<DiallingNumbersInfo>();
        diallingNumberCphs->name_ = Str8ToStr16(voiceMailNum_);
        diallingNumberCphs->number_ = Str8ToStr16(voiceMailTag_);
        AppExecFwk::InnerEvent::Pointer eventCphs =
            CreateDiallingNumberPointer(MSG_SIM_SET_CPHS_MAILBOX_DONE, 0, 0, nullptr);
        DiallingNumberUpdateInfor infor;
        infor.diallingNumber = diallingNumberCphs;
        infor.fileId = ELEMENTARY_FILE_MAILBOX_CPHS;
        infor.extFile = ELEMENTARY_FILE_EXT1;
        infor.index = 1;
        diallingNumberHandler_->UpdateDiallingNumbers(infor, eventCphs);
        TELEPHONY_LOGI("set cphs voicemail number as it is available");
    } else {
        if (!hasNotify) {
            processWait_.notify_all();
        }
        TELEPHONY_LOGI("set voicemail number finished");
    }
    return isFileProcessResponse;
}

bool SimFile::ProcessMarkSms(const AppExecFwk::InnerEvent::Pointer &event)
{
    (void)event;
    return false;
}

bool SimFile::ProcessObtainSpnPhase(const AppExecFwk::InnerEvent::Pointer &event)
{
    bool loadResponse = true;
    if (event == nullptr) {
        TELEPHONY_LOGE("event is nullptr!");
        return loadResponse;
    }
    ObtainSpnPhase(false, event);
    return loadResponse;
}

bool SimFile::IsContinueGetSpn(bool start, SpnStatus curStatus, SpnStatus &newStatus)
{
    if (start) {
        switch (curStatus) {
            case OBTAIN_SPN_GENERAL:
            case OBTAIN_OPERATOR_NAMESTRING:
            case OBTAIN_OPERATOR_NAME_SHORTFORM:
            case OBTAIN_SPN_START:
                newStatus = SpnStatus::OBTAIN_SPN_START;
                return false;
            default:
                newStatus = SpnStatus::OBTAIN_SPN_START;
                return true;
        }
    } else {
        return true;
    }
}

void SimFile::InitMemberFunc()
{
    memberFuncMap_[RadioEvent::RADIO_SIM_STATE_READY] = &SimFile::ProcessIccReady;
    memberFuncMap_[RadioEvent::RADIO_SIM_STATE_LOCKED] = &SimFile::ProcessIccLocked;
    memberFuncMap_[RadioEvent::RADIO_SIM_STATE_SIMLOCK] = &SimFile::ProcessIccLocked;
    memberFuncMap_[MSG_SIM_OBTAIN_IMSI_DONE] = &SimFile::ProcessObtainIMSIDone;
    memberFuncMap_[MSG_SIM_OBTAIN_ICCID_DONE] = &SimFile::ProcessGetIccIdDone;
    memberFuncMap_[MSG_SIM_OBTAIN_MBI_DONE] = &SimFile::ProcessGetMbiDone;
    memberFuncMap_[MSG_SIM_OBTAIN_CPHS_MAILBOX_DONE] = &SimFile::ProcessGetCphsMailBoxDone;
    memberFuncMap_[MSG_SIM_OBTAIN_MBDN_DONE] = &SimFile::ProcessGetMbdnDone;
    memberFuncMap_[MSG_SIM_OBTAIN_MSISDN_DONE] = &SimFile::ProcessGetMsisdnDone;
    memberFuncMap_[MSG_SIM_SET_MSISDN_DONE] = &SimFile::ProcessSetMsisdnDone;
    memberFuncMap_[MSG_SIM_OBTAIN_MWIS_DONE] = &SimFile::ProcessGetMwisDone;
    memberFuncMap_[MSG_SIM_OBTAIN_VOICE_MAIL_INDICATOR_CPHS_DONE] = &SimFile::ProcessVoiceMailCphs;
    memberFuncMap_[MSG_SIM_OBTAIN_AD_DONE] = &SimFile::ProcessGetAdDone;
    memberFuncMap_[MSG_SIM_OBTAIN_SPN_DONE] = &SimFile::ProcessObtainSpnPhase;
    memberFuncMap_[MSG_SIM_OBTAIN_LI_LANGUAGE_DONE] = &SimFile::ProcessObtainLiLanguage;
    memberFuncMap_[MSG_SIM_OBTAIN_PL_LANGUAGE_DONE] = &SimFile::ProcessObtainPlLanguage;
    memberFuncMap_[MSG_SIM_OBTAIN_CFF_DONE] = &SimFile::ProcessGetCffDone;
    memberFuncMap_[MSG_SIM_OBTAIN_SPDI_DONE] = &SimFile::ProcessGetSpdiDone;
    memberFuncMap_[MSG_SIM_UPDATE_DONE] = &SimFile::ProcessUpdateDone;
    memberFuncMap_[MSG_SIM_OBTAIN_PNN_DONE] = &SimFile::ProcessGetPnnDone;
    memberFuncMap_[MSG_SIM_OBTAIN_OPL_DONE] = &SimFile::ProcessGetOplDone;
    memberFuncMap_[MSG_SIM_OBTAIN_ALL_SMS_DONE] = &SimFile::ProcessGetAllSmsDone;
    memberFuncMap_[MSG_SIM_MARK_SMS_READ_DONE] = &SimFile::ProcessMarkSms;
    memberFuncMap_[MSG_SIM_SMS_ON_SIM] = &SimFile::ProcessSmsOnSim;
    memberFuncMap_[MSG_SIM_OBTAIN_SMS_DONE] = &SimFile::ProcessGetSmsDone;
    memberFuncMap_[MSG_SIM_OBTAIN_SST_DONE] = &SimFile::ProcessGetSstDone;
    memberFuncMap_[MSG_SIM_OBTAIN_INFO_CPHS_DONE] = &SimFile::ProcessGetInfoCphs;
    memberFuncMap_[MSG_SIM_SET_MBDN_DONE] = &SimFile::ProcessSetMbdn;
    memberFuncMap_[MSG_SIM_SET_CPHS_MAILBOX_DONE] = &SimFile::ProcessSetCphsMailbox;
    memberFuncMap_[MSG_SIM_OBTAIN_CFIS_DONE] = &SimFile::ProcessGetCfisDone;
    memberFuncMap_[MSG_SIM_OBTAIN_CSP_CPHS_DONE] = &SimFile::ProcessGetCspCphs;
    memberFuncMap_[MSG_SIM_OBTAIN_GID1_DONE] = &SimFile::ProcessObtainGid1Done;
    memberFuncMap_[MSG_SIM_OBTAIN_GID2_DONE] = &SimFile::ProcessObtainGid2Done;
    memberFuncMap_[MSG_SIM_OBTAIN_PLMN_W_ACT_DONE] = &SimFile::ProcessGetPlmnActDone;
    memberFuncMap_[MSG_SIM_OBTAIN_OPLMN_W_ACT_DONE] = &SimFile::ProcessGetOplmnActDone;
    memberFuncMap_[MSG_SIM_OBTAIN_HPLMN_W_ACT_DONE] = &SimFile::ProcessGetHplmActDone;
    memberFuncMap_[MSG_SIM_OBTAIN_EHPLMN_DONE] = &SimFile::ProcessGetEhplmnDone;
    memberFuncMap_[MSG_SIM_OBTAIN_FPLMN_DONE] = &SimFile::ProcessGetFplmnDone;
}

int SimFile::ObtainSpnCondition(bool roaming, const std::string &operatorNum)
{
    unsigned int cond = 0;
    if (displayConditionOfSpn_ <= SPN_INVALID) {
        return cond;
    }
    if (roaming) {
        cond = SPN_CONDITION_DISPLAY_PLMN;
        if ((static_cast<unsigned int>(displayConditionOfSpn_) & static_cast<unsigned int>(SPN_COND)) == 0) {
            cond |= static_cast<unsigned int>(SPN_CONDITION_DISPLAY_SPN);
        }
    } else {
        cond = SPN_CONDITION_DISPLAY_SPN;
        if ((static_cast<unsigned int>(displayConditionOfSpn_) & static_cast<unsigned int>(SPN_COND_PLMN)) ==
            SPN_COND_PLMN) {
            cond |= static_cast<unsigned int>(SPN_CONDITION_DISPLAY_PLMN);
        }
    }
    return cond;
}

int SimFile::ObtainExtensionElementaryFile(int ef)
{
    int ext = 0;
    if (ef == ELEMENTARY_FILE_MSISDN) {
        ext = ELEMENTARY_FILE_EXT5; // ELEMENTARY_FILE_EXT1
    } else {
        ext = ELEMENTARY_FILE_EXT1;
    }
    return ext;
}

bool SimFile::UpdateVoiceMail(const std::string &mailName, const std::string &mailNumber)
{
    waitResult_ = false;
    std::shared_ptr<DiallingNumbersInfo> diallingNumber = std::make_shared<DiallingNumbersInfo>();
    diallingNumber->name_ = Str8ToStr16(mailName);
    diallingNumber->number_ = Str8ToStr16(mailNumber);

    if ((indexOfMailbox_) && (indexOfMailbox_ != BYTE_NUM)) {
        std::unique_lock<std::mutex> lock(IccFile::mtx_);
        TELEPHONY_LOGI("UpdateVoiceMail start MBDN");
        AppExecFwk::InnerEvent::Pointer event = CreateDiallingNumberPointer(MSG_SIM_SET_MBDN_DONE, 0, 0, nullptr);
        DiallingNumberUpdateInfor infor;
        infor.diallingNumber = diallingNumber;
        infor.fileId = ELEMENTARY_FILE_MBDN;
        infor.extFile = ELEMENTARY_FILE_EXT6;
        infor.index = indexOfMailbox_;
        diallingNumberHandler_->UpdateDiallingNumbers(infor, event);
        processWait_.wait(lock);
    } else if (CphsVoiceMailAvailable()) {
        std::unique_lock<std::mutex> lock(IccFile::mtx_);
        AppExecFwk::InnerEvent::Pointer event =
            CreateDiallingNumberPointer(MSG_SIM_SET_CPHS_MAILBOX_DONE, 0, 0, nullptr);
        DiallingNumberUpdateInfor infor;
        infor.diallingNumber = diallingNumber;
        infor.fileId = ELEMENTARY_FILE_MAILBOX_CPHS;
        infor.extFile = ELEMENTARY_FILE_EXT1;
        infor.index = 1;
        diallingNumberHandler_->UpdateDiallingNumbers(infor, event);
        processWait_.wait(lock);
    } else {
        TELEPHONY_LOGE("UpdateVoiceMail indexOfMailbox_ %{public}d is invalid!!", indexOfMailbox_);
    }
    TELEPHONY_LOGI("UpdateVoiceMail finished %{public}d", waitResult_);
    return waitResult_;
}

bool SimFile::CphsVoiceMailAvailable()
{
    bool available = false;
    if (!cphsInfo_.empty()) {
        int dataLen = 0;
        std::shared_ptr<unsigned char> dataByte = SIMUtils::HexStringConvertToBytes(cphsInfo_, dataLen);
        available = (dataByte != nullptr) ? (dataByte.get()[1] & CPHS_VOICE_MAIL_MASK) == CPHS_VOICE_MAIL_EXSIT : false;
    }
    return available;
}
} // namespace Telephony
} // namespace OHOS
