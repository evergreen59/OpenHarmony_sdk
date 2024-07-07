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

#ifndef DATA_STORAGE_RDB_PDP_OPERATOR_HELPER_H
#define DATA_STORAGE_RDB_PDP_OPERATOR_HELPER_H

#include "iosfwd"
#include "rdb_base_helper.h"
#include "string"

namespace OHOS {
namespace Telephony {
class RdbPdpProfileHelper : public RdbBaseHelper {
public:
    RdbPdpProfileHelper();
    ~RdbPdpProfileHelper() = default;

    /**
     * Init dataBase
     *
     * @return 0 is succeed else failed
     */
    int Init();

    /**
     * Update dataBase path
     *
     * @param path path
     */
    void UpdateDbPath(const std::string &path);

    /**
     * Reload pdp_profile table data form config json file
     *
     * @return 0 is succeed else failed
     */
    int ResetApn();

private:
    /**
     * Create pdp_profile table
     *
     * @param createTableStr Create table statement
     * @param tableName tableName
     */
    void CreatePdpProfileTableStr(std::string &createTableStr, const std::string &tableName);

    /**
     * End the transaction action
     *
     * @return 0 is succeed else failed
     */
    int EndTransactionAction();

    /**
     * Commit the transaction action
     *
     * @return 0 is succeed else failed
     */
    int CommitTransactionAction();

private:
    const std::string DB_NAME = "net.db";
    std::string dbPath_ = FOLDER_PATH + DB_NAME;
    const int VERSION = 1;
};
} // namespace Telephony
} // namespace OHOS
#endif // DATA_STORAGE_RDB_PDP_OPERATOR_HELPER_H
