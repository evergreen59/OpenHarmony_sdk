/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#include "statement.h"

#include "accesstoken_log.h"

namespace OHOS {
namespace Security {
namespace AccessToken {
namespace {
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, SECURITY_DOMAIN_ACCESSTOKEN, "Statement"};
}

Statement::Statement(sqlite3* db, const std::string& sql) : db_(db), sql_(sql)
{
    if (sqlite3_prepare_v2(db, sql.c_str(), sql.size(), &statement_, nullptr) != SQLITE_OK) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "Cannot prepare, errorMsg: %{public}s", sqlite3_errmsg(db_));
    }
}

Statement::~Statement()
{
    sqlite3_finalize(statement_);
    statement_ = nullptr;
}

void Statement::Bind(const int32_t index, const std::string& text)
{
    if (sqlite3_bind_text(statement_, index, text.c_str(), text.size(), SQLITE_TRANSIENT) != SQLITE_OK) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "Cannot bind string, errorMsg: %{public}s", sqlite3_errmsg(db_));
    }
}

void Statement::Bind(const int32_t index, int32_t value)
{
    if (sqlite3_bind_int(statement_, index, value) != SQLITE_OK) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "Cannot bind int32_t, errorMsg: %{public}s", sqlite3_errmsg(db_));
    }
}

void Statement::Bind(const int32_t index, int64_t value)
{
    if (sqlite3_bind_int64(statement_, index, value) != SQLITE_OK) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "Cannot bind int64_t, errorMsg: %{public}s", sqlite3_errmsg(db_));
    }
}

int32_t Statement::GetColumnInt(const int32_t column) const
{
    return sqlite3_column_int(statement_, column);
}

int64_t Statement::GetColumnInt64(const int32_t column) const
{
    return sqlite3_column_int64(statement_, column);
}

std::string Statement::GetColumnString(const int32_t column) const
{
    return std::string(reinterpret_cast<const char*>(sqlite3_column_text(statement_, column)));
}

std::string Statement::GetColumnName(const int32_t column) const
{
    return sqlite3_column_name(statement_, column);
}

Statement::State Statement::Step()
{
    int32_t ret = sqlite3_step(statement_);
    switch (ret) {
        case SQLITE_ROW:
            return Statement::State::ROW;
        case SQLITE_DONE:
            return Statement::State::DONE;
        case SQLITE_BUSY:
            return Statement::State::BUSY;
        case SQLITE_MISUSE:
            return Statement::State::MISUSE;
        default:
            return Statement::State::UNKNOWN;
    }
}

int32_t Statement::GetParameterIndex(const std::string& name) const
{
    return sqlite3_bind_parameter_index(statement_, name.c_str());
}

void Statement::Bind(const std::string& tableColumnName, const VariantValue& value)
{
    int32_t index = GetParameterIndex(":" + tableColumnName);
    if (value.GetType() == ValueType::TYPE_STRING) {
        Bind(index, value.GetString());
    } else if (value.GetType() == ValueType::TYPE_INT) {
        Bind(index, value.GetInt());
    } else if (value.GetType() == ValueType::TYPE_INT64) {
        Bind(index, value.GetInt64());
    }
}

int32_t Statement::Reset()
{
    return sqlite3_reset(statement_);
}

int32_t Statement::GetColumnCount() const
{
    return sqlite3_column_count(statement_);
}

VariantValue Statement::GetValue(const int32_t column, const bool flagInt64) const
{
    int32_t type = sqlite3_column_type(statement_, column);
    switch (type) {
        case SQLITE_INTEGER:
            if (flagInt64) {
                return VariantValue(GetColumnInt64(column));
            } else {
                return VariantValue(GetColumnInt(column));
            }
        case SQLITE_TEXT:
            return VariantValue(GetColumnString(column));
        default:
            return VariantValue();
    }
}
} // namespace AccessToken
} // namespace Security
} // namespace OHOS
