/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Copyright (C) 2014 Samsung Electronics Co., Ltd. All rights reserved
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

#ifndef GSM_SMS_PARAMCODEC_H
#define GSM_SMS_PARAMCODEC_H

#include "gsm_pdu_code_type.h"

namespace OHOS {
namespace Telephony {
class GsmSmsParamCodec {
public:
    GsmSmsParamCodec() = default;
    virtual ~GsmSmsParamCodec() = default;

    static int EncodeAddress(const struct SmsAddress *pAddress, char **ppParam);
    static int EncodeTime(const struct SmsTimeStamp *pTimeStamp, char **ppParam);
    static int EncodeDCS(const struct SmsDcs *pDCS, char **ppParam);
    static int EncodeSMSC(const char *pAddress, unsigned char *pEncodeAddr);
    static int EncodeSMSC(const struct SmsAddress *pAddress, unsigned char *pSMSC, int smscLen);

    static int DecodeAddress(const unsigned char *pTpdu, struct SmsAddress *pAddress);
    static int DecodeTime(const unsigned char *pTpdu, struct SmsTimeStamp *pTimeStamp);
    static int DecodeDCS(const unsigned char *pTpdu, struct SmsDcs *pDCS);
    static void DecodeSMSC(unsigned char *pAddress, int AddrLen, enum SmsTon ton, char *pDecodeAddr);
    static int DecodeSMSC(const unsigned char *pTpdu, int pduLen, struct SmsAddress &pAddress);
    static bool CheckCphsVmiMsg(const unsigned char *pTpdu, int *setType, int *indType);

private:
    constexpr static unsigned char NUMBER_TEN = 10;
    static constexpr uint8_t MAX_TIME_LEN = 32;
    static constexpr uint8_t MAX_ADDRESS_LEN = 21;
    static constexpr uint8_t MAX_ADD_PARAM_LEN = 12;
    static constexpr uint8_t MAX_REL_TIME_PARAM_LEN = 1;
    static constexpr uint8_t MAX_SMSC_LEN = 20;
    static constexpr uint8_t MAX_DCS_PARAM_LEN = 1;
    static constexpr uint8_t MAX_ABS_TIME_PARAM_LEN = 7;
    static constexpr uint8_t YEARY_OF_MON = 12;
    static constexpr uint8_t HEX_BYTE_STEP = 2;
};
} // namespace Telephony
} // namespace OHOS
#endif /* SMS_PLUGIN_PARAMCODEC_H */