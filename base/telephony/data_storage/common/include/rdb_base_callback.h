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

#ifndef DATA_STORAGE_RDB_BASE_CALLBACK_H
#define DATA_STORAGE_RDB_BASE_CALLBACK_H

#include "iosfwd"
#include "rdb_open_callback.h"
#include "string"
#include "vector"

namespace OHOS {
namespace NativeRdb {
class RdbStore;
}
namespace Telephony {
class RdbBaseCallBack : public NativeRdb::RdbOpenCallback {
public:
    explicit RdbBaseCallBack(const std::vector<std::string> &createTableVec) : createTableVec_(createTableVec) {}
    ~RdbBaseCallBack() = default;

    int OnCreate(NativeRdb::RdbStore &rdbStore) override;

private:
    std::vector<std::string> createTableVec_;
};
} // namespace Telephony
} // namespace OHOS
#endif // DATA_STORAGE_RDB_BASE_CALLBACK_H
