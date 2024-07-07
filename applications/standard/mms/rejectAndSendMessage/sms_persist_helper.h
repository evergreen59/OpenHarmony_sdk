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

#ifndef SMS_PERSIST_HELPER_H
#define SMS_PERSIST_HELPER_H

#include "singleton.h"

#include "datashare_helper.h"
#include "datashare_predicates.h"

#include "sms_receive_indexer.h"
#include "sms_mms_data.h"

namespace OHOS {
namespace Telephony {
class SmsPersistHelper {
    DECLARE_DELAYED_SINGLETON(SmsPersistHelper)
public:
    bool Insert(DataShare::DataShareValuesBucket &values);
    bool Insert(std::string tableUri, DataShare::DataShareValuesBucket &values);
    bool Query(DataShare::DataSharePredicates &predicates, std::vector<SmsReceiveIndexer> &indexers);
    bool QueryMaxGroupId(DataShare::DataSharePredicates &predicates, uint16_t &maxGroupId);
    bool QuerySession(DataShare::DataSharePredicates &predicates, uint16_t &sectionId, uint16_t &messageCount);
    bool Delete(DataShare::DataSharePredicates &predicates);
    bool QueryBlockPhoneNumber(const std::string &phoneNum);
    bool QueryParamBoolean(const std::string key, bool defValue);
    bool Update(DataShare::DataSharePredicates &predicates, DataShare::DataShareValuesBucket &values);

    inline static const std::string SMS_CAPABLE_KEY = "sms_config_capable";
    inline static const std::string SMS_ENCODING_KEY = "sms_config_force_7bit_encoding";
    inline static const std::string SMS_CAPABLE_PARAM_KEY = "const.telephony.sms.capable";
    inline static const std::string SMS_ENCODING_PARAM_KEY = "persist.sys.sms.config.7bitforce";

private:
    const std::string SMS_URI = "datashare:///com.ohos.smsmmsability";
    const std::string SMS_SUBSECTION = "datashare:///com.ohos.smsmmsability/sms_mms/sms_subsection";
    const std::string SMS_MMS_INFO = "datashare:///com.ohos.smsmmsability/sms_mms/sms_mms_info";
    const std::string SESSION = "datashare:///com.ohos.smsmmsability/sms_mms/session";
    const std::string CONTACT_URI = "datashare:///com.ohos.contactsdataability";
    const std::string CONTACT_BLOCK =
        "datashare:///com.ohos.contactsdataability/contacts/contact_blocklist";

    std::shared_ptr<DataShare::DataShareHelper> CreateDataShareHelper(std::string uri);
    void ResultSetConvertToIndexer(
        SmsReceiveIndexer &info, const std::shared_ptr<DataShare::DataShareResultSet> &resultSet);
    void ConvertIntToIndexer(
        SmsReceiveIndexer &info, const std::shared_ptr<DataShare::DataShareResultSet> &resultSet);
    void ConvertStringToIndexer(
        SmsReceiveIndexer &info, const std::shared_ptr<DataShare::DataShareResultSet> &resultSet);
};
} // namespace Telephony
} // namespace OHOS
#endif
