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

#ifndef TELEPHONY_IMS_CALL_TYPES_H
#define TELEPHONY_IMS_CALL_TYPES_H

#include "ims_core_service_types.h"
#include "call_manager_errors.h"
#include "call_manager_inner_type.h"

namespace OHOS {
namespace Telephony {
const int32_t kMaxNumberLength = 30;

enum ImsRejectReason {
    USER_IS_BUSY = 0,
    USER_DECLINE = 1,
};

// service class used in IMS Set Call Waiting interface
enum ImsServiceClass {
    SERVICE_CLASS_VOICE = 1,
    SERVICE_CLASS_VIDEO = 2,
};

enum SrvccState {
    SRVCC_NONE = -1,
    STARTED = 0,
    COMPLETED = 1,
    FAILED = 2,
    CANCELED = 3
};

enum ImsSrvccAction {
    ACTION_DIAL,
    ACTION_HANGUP,
    ACTION_ANSWER,
    ACTION_REJECT,
    ACTION_CONFERENCE,
    ACTION_SWITCH_CALL,
    ACTION_SEND_DTMF,
    ACTION_START_DTMF,
    ACTION_STOP_DTMF,
};

enum ImsCallType {
    TEL_IMS_CALL_TYPE_VOICE,
    TEL_IMS_CALL_TYPE_VT_TX,
    TEL_IMS_CALL_TYPE_VT_RX,
    TEL_IMS_CALL_TYPE_VT,
};

struct ImsSrvccActionInfo {
    CellularCallInfo callInfo;
    char dtmfCode;
};

enum TransferState {
    VT_TRANSFER_TO_WFC = 0,
    WFC_TRANSFER_TO_VT = 1,
};

struct ImsCallInfo {
    char phoneNum[kMaxNumberLength]; // call phone number
    int32_t slotId;
    int32_t videoState; // 0: audio 1:video
    int32_t index; // call index
};

struct ImsCurrentCall {
    int32_t index;
    int32_t dir;
    int32_t state;
    int32_t mode;
    int32_t mpty;
    int32_t voiceDomain;
    ImsCallType callType;
    std::string number;
    int32_t type;
    std::string alpha;
};

struct ImsCurrentCallList {
    int32_t callSize;
    int32_t flag;
    std::vector<ImsCurrentCall> calls;
};
} // namespace Telephony
} // namespace OHOS

#endif // TELEPHONY_IMS_CALL_TYPES_H
