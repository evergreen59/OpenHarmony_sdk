/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#ifndef DATASHARE_ABS_RESULT_SET_H
#define DATASHARE_ABS_RESULT_SET_H

#include <map>
#include <string>
#include "basic/result_set.h"

namespace OHOS {
namespace DataShare {
class DataShareAbsResultSet : public ResultSet {
public:
    DataShareAbsResultSet();
    virtual ~DataShareAbsResultSet();
    virtual int GetRowCount(int &count) override;
    virtual int GetAllColumnNames(std::vector<std::string> &columnNames) override;
    virtual int GetBlob(int columnIndex, std::vector<uint8_t> &blob) override;
    virtual int GetString(int columnIndex, std::string &value) override;
    virtual int GetInt(int columnIndex, int &value) override;
    virtual int GetLong(int columnIndex, int64_t &value) override;
    virtual int GetDouble(int columnIndex, double &value) override;
    virtual int IsColumnNull(int columnIndex, bool &isNull) override;
    virtual int GoToRow(int position) override;
    virtual int GetDataType(int columnIndex, DataType &dataType) override;
    int GetRowIndex(int &position) const override;
    int GoTo(int offset) override;
    int GoToFirstRow() override;
    int GoToLastRow() override;
    int GoToNextRow() override;
    int GoToPreviousRow() override;
    int IsAtFirstRow(bool &result) const override;
    int IsAtLastRow(bool &result) override;
    int IsStarted(bool &result) const override;
    int IsEnded(bool &result) override;
    int GetColumnCount(int &count) override;
    int GetColumnIndex(const std::string &columnName, int &columnIndex) override;
    int GetColumnName(int columnIndex, std::string &columnName) override;
    bool IsClosed() const override;
    int Close() override;

protected:
    // The default position of the result set
    static const int INIT_POS = -1;
    // Current row position
    int rowPos_;
    int count_;
    // Indicates whether the result set is closed
    bool isClosed_;
    std::map<std::string, int> indexCache_;
};
} // namespace DataShare
} // namespace OHOS

#endif
