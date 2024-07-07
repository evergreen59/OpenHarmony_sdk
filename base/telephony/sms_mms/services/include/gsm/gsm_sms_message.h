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

#ifndef GSM_SMS_MESSAGE_H
#define GSM_SMS_MESSAGE_H

#include <memory>
#include <stdint.h>
#include <string>
#include <vector>

#include "gsm_pdu_code_type.h"
#include "sms_base_message.h"

namespace OHOS {
namespace Telephony {
class GsmSmsMessage : public SmsBaseMessage {
public:
    GsmSmsMessage() = default;
    virtual ~GsmSmsMessage() = default;
    void SetFullText(const std::string &text);
    void SetDestAddress(const std::string &destAddress);
    void SetDestPort(uint32_t port);

    std::string GetFullText() const;
    std::string GetDestAddress() const;
    uint16_t GetDestPort();
    bool GetIsSmsText() const;
    bool GetGsm() const;
    std::string GetReplyAddress() const;

    int SetHeaderLang(int index, const SmsCodingScheme codingType, const MSG_LANGUAGE_ID_T langId);
    int SetHeaderConcat(int index, const SmsConcat &concat);
    int SetHeaderReply(int index);

    std::shared_ptr<struct SmsTpdu> CreateDefaultSubmitSmsTpdu(const std::string &dest, const std::string &sc,
        const std::string &text, bool bStatusReport, const SmsCodingScheme codingScheme);
    std::shared_ptr<struct SmsTpdu> CreateDataSubmitSmsTpdu(const std::string &desAddr, const std::string &scAddr,
        int32_t port, const uint8_t *data, uint32_t dataLen, uint8_t msgRef8bit, bool bStatusReport);

    std::shared_ptr<struct EncodeInfo> GetSubmitEncodeInfo(const std::string &sc, bool bMore);
    std::shared_ptr<struct SmsTpdu> CreateDeliverSmsTpdu();
    std::shared_ptr<struct SmsTpdu> CreateDeliverReportSmsTpdu();
    std::shared_ptr<struct SmsTpdu> CreateStatusReportSmsTpdu();
    static std::shared_ptr<GsmSmsMessage> CreateMessage(const std::string &pdu);

    bool PduAnalysis(const std::string &pdu);
    void ConvertMessageDcs();
    void ConvertUserData();
    bool GetIsTypeZeroInd() const;
    bool GetIsSIMDataTypeDownload() const;
    void ConvertMsgTimeStamp(const struct SmsTimeStamp &times);
    bool IsSpecialMessage() const;

    static constexpr uint16_t TAPI_NETTEXT_SMDATA_SIZE_MAX = 255;
    static constexpr uint8_t TAPI_SIM_SMSP_ADDRESS_LEN = 20;

private:
    static constexpr uint16_t DEFAULT_PORT = -1;
    static constexpr uint8_t MAX_GSM_7BIT_DATA_LEN = 160;
    static constexpr uint8_t MAX_SMSC_LEN = 20;
    static constexpr uint16_t MAX_TPDU_DATA_LEN = 255;
    static constexpr uint16_t TAPI_TEXT_SIZE_MAX = 520;
    static constexpr uint8_t GSM_BEAR_DATA_LEN = 140;
    static constexpr uint8_t BYTE_BITS = 8;
    static constexpr uint8_t CHARSET_7BIT_BITS = 7;

    std::string fullText_;
    std::string destAddress_;
    std::string replyAddress_;
    uint16_t destPort_ = -1;
    bool bSmsText_ = false;
    std::shared_ptr<struct SmsTpdu> smsTpdu_;

    void AnalysisMsgDeliver(const SmsDeliver &deliver);
    void AnalysisMsgStatusReport(const SmsStatusReport &statusRep);
    void AnalysisMsgSubmit(const SmsSubmit &submit);
    void CreateDefaultSubmit(bool bStatusReport, const SmsCodingScheme codingScheme);
    int SetSmsTpduDestAddress(std::shared_ptr<struct SmsTpdu> &tPdu, const std::string &desAddr);
    int CalcReplyEncodeAddress(const std::string &replyAddress);
    virtual int DecodeMessage(unsigned char *decodeData, unsigned int length, SmsCodingScheme &codingType,
        const std::string &msgText, bool &bAbnormal, MSG_LANGUAGE_ID_T &langId);
};

struct EncodeInfo {
    char tpduData_[GsmSmsMessage::TAPI_NETTEXT_SMDATA_SIZE_MAX + 1];
    char smcaData_[GsmSmsMessage::TAPI_SIM_SMSP_ADDRESS_LEN + 1];
    int tpduLen;
    int smcaLen;
    bool isMore_;
};
} // namespace Telephony
} // namespace OHOS
#endif
