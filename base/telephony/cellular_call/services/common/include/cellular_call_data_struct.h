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

#ifndef CELLULAR_CALL_DATA_STRUCT_H
#define CELLULAR_CALL_DATA_STRUCT_H

#include "call_manager_inner_type.h"

namespace OHOS {
namespace Telephony {
enum CLIRMode {
    DEFAULT = 0,
    TRANSFER = 1,
    INHIBITION = 2,
};

struct DialRequestStruct {
    std::string phoneNum;
    CLIRMode clirMode;
};

// GSM Association Non-confidential Official Document IR.92 - IMS Profile for Voice and SMS
struct ImsDialInfoStruct : public DialRequestStruct {
    int32_t videoState; // 0: audio 1:video
    bool bEmergencyCall;
    bool bImsCallFirst;
    bool bRoaming;
};

enum MMIHandlerId {
    EVENT_MMI_Id = 300,
    EVENT_INQUIRE_MMI,
    EVENT_SET_MMI,
    EVENT_INQUIRE_CLIR_MMI,
    EVENT_INQUIRE_CALL_FORWARD_MMI,
    EVENT_SET_CALL_FORWARD_MMI,
    EVENT_DEAL_USSD_MMI,
    EVENT_SET_UNLOCK_PIN_PUK_ID,
};

/**
 * 3GPP TS 22.030 V4.0.0 (2001-03)  6.5.2 Structure of the MMI
 * The following sequence of functions shall be used for the control of Supplementary Services:
 *  SELECT:	Entry of the procedure information (may be a digit or a sequence of characters).
 *  SEND:	Transmission of the information to the network.
 *  INDICATION:	Call progress indications.
 */
struct MMIData {
    std::string fullString;
    std::string actionString;
    std::string serviceCode;
    // 3GPP TS 22.030 V4.0.0 (2001-03)  6.5.2 Structure of the MMI
    // This structure consists of the following parts:
    // Service Code, SC( (2 or 3 digits);
    // Supplementary Information, SI (variable length).
    std::string serviceInfoA;
    std::string serviceInfoB;
    std::string serviceInfoC;
    std::string pwdString;
    std::string dialString;
};

/**
 * 3GPP TS 22.030 V4.0.0 (2001-03)
 * Tele- and Bearer Service Supplementary Information codes (SIb).
Alternate and speech/data services are included with the equivalent data service.
Basic Service
group number (note)	Telecommunication Service	MMI Service Code
1 to 12	        All tele and bearer services	no code required
                Teleservices
1 to 6, 12	    All teleservices	            10
1	            Telephony 	                    11
2 to 6	        All data teleservices	        12
6	            Facsimile services	            13
2	            Short Message Services	        16
1, 3 to 6, 12	All teleservices except SMS	    19
12	            Voice group services
                Voice Group Call Service (VGCS)	17
                Voice Broadcast Service (VBS)	18
                Bearer Service
7 to 11	        All bearer services	            20
7	            All async services	            21
8	            All sync services	            22
8	            All data circuit sync	        24
7	            All data circuit async	        25
13	            All GPRS bearer services	    99
*/
enum CFServiceCode {
    ALL_TELE_SERVICES = 10,
    TELE_SERVICES = 11,
    ALL_DATA_TELE_SERVICES = 12,
    FACSIMILE_SERVICES = 13,
    SHORT_MESSAGE_SERVICES = 16,
    ALL_TELE_SERVICES_EXCEPT_SMS = 19,
    ALL_BEARER_SERVICES = 20,
    ALL_ASYNC_SERVICES = 21,
    ALL_SYNC_SERVICES = 22,
    ALL_DATA_CIRCUIT_SYNC = 24,
    ALL_DATA_CIRCUIT_ASYNC = 25,
    ALL_GPRS_BEARER_SERVICES = 99,
};

/**
* 27007-430_2001 7.11 Used for various supplementary services apis
*
* 3GPP TS 22.082 [4] Call forwarding number and conditions +CCFC
* 3GPP TS 22.083 [5] Call Waiting +CCWA
* 3GPP TS 22.088 [6] Facility lock +CLCK(for call barring)
*
* <classx> is a sum of integers each representing a class of information (default 7):
* 1	voice (telephony)
* 2	data (refers to all bearer services; with <mode>=2 this may refer only to some bearer service if TA does
not support values 16, 32, 64 and 128)
* 4	fax (facsimile services)
* 8	short message service
* 16	data circuit sync
* 32	data circuit async
* 64	dedicated packet access
* 128	dedicated PAD access
*/
enum ServiceClassType {
    NONE = 0,
    VOICE = 1,
    FAX = 4,
    SHORT_MESSAGE_SERVICE = 8,
    DATA_CIRCUIT_SYNC = 16,
    DATA_CIRCUIT_ASYNC = 32,
    DEDICATED_PACKET_ACCESS = 64,
    DEDICATED_PAD_ACCESS = 128,
};

/**
 * 3GPP TS 27.007 Vh.1.0 (2021-03)  8.74	List of current calls +CLCCS
 * <neg_status>: integer type as defined in the +CCMMD command.
 * 0	The parameter <neg_status> has no valid content. Parameter <SDP_md> is set to an empty string ("").
 * 1	The <SDP_md> parameter describes the active media in the call.
 * 2	The <SDP_md> parameter describes a proposed but not yet active new set of media for the call.
 * 3	A proposed new set of media for the call was accepted by the remote party. The <SDP_md> parameter describes
 * the active media in the call (if any).
 * 4	A proposed new set of media for the call was rejected by the remote
 * party. The <SDP_md> parameter describes the active media in the call (if any)
 */
enum NegStatus {
    NEG_INVALID = 0,
    NEG_ACTIVE = 1,
    NEG_PROPOSE = 2,
    NEG_ACCEPT = 3,
    NEG_REJECT = 4,
};

/**
 * 3GPP TS 27.007 Vh.1.0 (2021-03)  8.74	List of current calls +CLCCS
 * <cs_mode>: integer type (bearer/teleservice)
0	no relevant information about bearer/teleservice
1	voice
2	data
3	fax
4	voice followed by data, voice mode
5	alternating voice/data, voice mode
6	alternating voice/fax, voice mode
7	voice followed by data, data mode
8	alternating voice/data, data mode
9	alternating voice/fax, fax mode
255	unknown
 */
enum CsMode {
    NO_INFORMATION = 0,
    MODE_VOICE = 1,
    MODE_DATA = 2,
    MODE_FAX = 3,
    VOICE_FOLLOWED_DATA = 4,
    VOICE_DATA_ALTERNATE = 5,
    VOICE_FAX_ALTERNATE = 6,
    VOICE_FOLLOWED_DATA_MODE_DATA = 7,
    VOICE_DATA_ALTERNATE_MODE_DATA = 8,
    VOICE_FAX_ALTERNATE_MODE_FAX = 9,
    UNKNOWN = 255,
};

/**
 * 3GPP TS 27.007 Vh.1.0 (2021-03)  8.74	List of current calls +CLCCS
 * <ccstatus>: integer type. Indicating the state of the call.
1	Idle
2	Calling (MO); the call setup has been started
3	Connecting (MO); the call is in progress
4	Alerting (MO): an alert indication has been received
5	Alerting (MT); an alert indication has been sent
6	Active; the connection is established
7	Released; an outgoing (MO) call is released.
8	Released; an incoming (MT) call is released
9	User Busy
10	User Determined User Busy
11	Call Waiting (MO)
12	Call Waiting (MT)
13	Call Hold (MO)
14	Call Hold (MT)
 */
enum CCstatus {
    IDLE = 1,
    DIALING = 2,
    CONNECTING = 3,
    MO_ALERTING = 4,
    MT_ALERTING = 5,
    ACTIVE = 6,
    MO_RELEASED = 7,
    MT_RELEASED = 8,
    USER_BUSY = 9,
    USER_DETERMINED_USER_BUSY = 10,
    MO_CALL_WAITING = 11,
    MT_CALL_WAITING = 12,
    MO_CALL_HOLD = 13,
    MT_CALL_HOLD = 14,
};

/**
 * 3GPP TS 27.007 Vh.1.0 (2021-03)  8.74	List of current calls +CLCCS
 * <dir>: integer type
0	mobile originated (MO) call
1	mobile terminated (MT) call

 <neg_status_present>: integer type. Indicates whether parameter <neg_status> has any valid information.
0	No valid information in parameter <neg_status>. Parameter <neg_status> is set to zero.
1	Valid information in parameter <neg_status>

 <SDP_md>: string type represented with IRA characters. SDP media description as per the +CDEFMP command. This
parameter shall not be subject to conventional character conversion as per +CSCS. This parameter will be an empty
string ("") if the call has no multimedia content

 <mpty>: integer type
0	call is not one of multiparty (conference) call parties
1	call is one of multiparty (conference) call parties

 <numbertype>: integer type. Indicating type of information in parameter <number>.
0	No valid information in parameter <number>
1	Number in <number> according to URI including the prefix specifying the URI type (see command +CDU). Parameter
 <ton> has no relevant information and is set to zero. 2	Number in <number> according to one of the formats
supported by 3GPP TS 24.008 [8] subclause 10.5.4.7)

 <ton>: type of number in integer format (refer 3GPP TS 24.008[8] subclause 10.5.4.7). The parameter is also set to
zero when it has no meaningful content, e.g. when <numbertype>=1.

 <number>: string type phone number in format specified by <numbertype>. This parameter shall not be subject to
 conventional character conversion as per +CSCS.

 <priority_present>: integer type. Indicates whether parameter <priority> has any valid information.
 0	No valid information in parameter <priority>. Parameter
<priority> is set to zero.
 1	Valid information in parameter <priority>.

 <priority>: integer type parameter
indicating the eMLPP priority level of the call, values specified in 3GPP TS 22.067 [54].

 <CLI_validity_present>: integer type. Indicates whether parameter <CLI_validity> has any valid information.
0	No valid information in parameter <CLI_validity>. Parameter <priority> is set to zero.
1	Valid information in parameter <CLI_validity>
 */
struct ImsCallInfoResponse {
    int32_t index;
    bool mt;
    bool negStatusPresent;
    NegStatus negStatus;
    char sdpMd[kMaxNumberLen];
    CsMode csMode;
    TelCallState state;
    bool mpty;
    int32_t numberType;
    int32_t ton;
    char number[kMaxNumberLen];
    bool priorityPresent;
    int32_t priority;
    bool cliValidityPresent;
    CallType callType; // call type: CS、IMS
    int32_t videoState; // 0: audio 1:video
};

struct PinPukResponse {
    int32_t result;
    int32_t remain;  // It can be repeated operator N times
};

enum ImsHandleId {
    IMS_RADIO_CALL_STATE = 2001,
    IMS_RADIO_AVAIL,
    IMS_RADIO_NOT_AVAIL,
    IMS_RADIO_CURRENT_CALLS,
};
} // namespace Telephony
} // namespace OHOS

#endif // CELLULAR_CALL_DATA_STRUCT_H