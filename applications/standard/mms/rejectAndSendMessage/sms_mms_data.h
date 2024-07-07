/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef SMS_MMS_DATA_H
#define SMS_MMS_DATA_H

namespace OHOS {
namespace Telephony {
class SmsMmsData {
public:
    static constexpr const char *ID = "id";
    static constexpr const char *SLOT_ID = "slot_id";
    static constexpr const char *SMS_SUBSECTION_ID = "sms_subsection_id";
    static constexpr const char *RECEIVER_NUMBER = "receiver_number";
    static constexpr const char *SENDER_NUMBER = "sender_number";
    static constexpr const char *IS_SENDER = "is_sender";
    static constexpr const char *START_TIME = "start_time";
    static constexpr const char *END_TIME = "end_time";
    static constexpr const char *RAW_PUD = "raw_pdu";
    static constexpr const char *FORMAT = "format";
    static constexpr const char *DEST_PORT = "dest_port";
    static constexpr const char *SUBSECTION_INDEX = "subsection_index";
    static constexpr const char *SIZE = "size";
};

class SmsMmsInfo {
public:
    static constexpr const char *MSG_ID = "msg_id";
    static constexpr const char *SLOT_ID = "slot_id";
    static constexpr const char *RECEIVER_NUMBER = "receiver_number";
    static constexpr const char *SENDER_NUMBER = "sender_number";
    static constexpr const char *IS_SENDER = "is_sender";
    static constexpr const char *START_TIME = "start_time";
    static constexpr const char *END_TIME = "end_time";
    static constexpr const char *MSG_TYPE = "msg_type";
    static constexpr const char *SMS_TYPE = "sms_type";
    static constexpr const char *MSG_TITLE = "msg_title";
    static constexpr const char *MSG_CONTENT = "msg_content";
    static constexpr const char *MSG_STATE = "msg_state";
    static constexpr const char *MSG_CODE = "msg_code";
    static constexpr const char *IS_LOCK = "is_lock";
    static constexpr const char *IS_READ = "is_read";
    static constexpr const char *IS_COLLECT = "is_collect";
    static constexpr const char *SESSION_TYPE = "session_type";
    static constexpr const char *RETRY_NUMBER = "retry_number";
    static constexpr const char *OPERATOR_SERVICE_NUMBER = "operator_service_number";
    static constexpr const char *SESSION_ID = "session_id";
    static constexpr const char *GROUP_ID = "group_id";
    static constexpr const char *DEVICE_ID = "device_id";
    static constexpr const char *IS_SUBSECTION = "is_subsection";
    static constexpr const char *IS_SEND_REPORT = "is_send_report";
};

class Session {
public:
    static constexpr const char *ID = "id";
    static constexpr const char *TIME = "time";
    static constexpr const char *TELEPHONE = "telephone";
    static constexpr const char *CONTENT = "content";
    static constexpr const char *CONTACTS_NUM = "contacts_num";
    static constexpr const char *SMS_TYPE = "sms_type";
    static constexpr const char *UNREAD_COUNT = "unread_count";
    static constexpr const char *SENDING_STATUS = "sending_status";
    static constexpr const char *HAS_DRAFT = "has_draft";
    static constexpr const char *HAS_LOCK = "has_lock";
    static constexpr const char *MESSAGE_COUNT = "message_count";
    static constexpr const char *HAS_MMS = "has_mms";
    static constexpr const char *HAS_ATTACHMENT = "has_attachment";
};
} // namespace Telephony
} // namespace OHOS
#endif // SMS_MMS_DATA_H

