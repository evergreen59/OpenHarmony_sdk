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

#ifndef DATASHARE_BLOCK_WRITER_IMPL_H
#define DATASHARE_BLOCK_WRITER_IMPL_H

#include "shared_block.h"
#include "result_set_bridge.h"
#include "datashare_errno.h"

namespace OHOS {
namespace DataShare {
/**
 * This class stores a set of rows from a database in a buffer，
 * which is used as the set of query result.
 */
class DataShareBlockWriterImpl : public virtual ResultSetBridge::Writer {
public:
    /**
     * SharedBlock constructor.
     */
    DataShareBlockWriterImpl();

    /**
     * SharedBlock constructor.
     */
    DataShareBlockWriterImpl(const std::string &name, size_t size);

    /**
     * SharedBlock Deconstruction.
     */
    virtual ~DataShareBlockWriterImpl();

    /**
     * Allocate a row unit and its directory.
     */
    int AllocRow() override;

    /**
     * Write Null data to the shared block.
     */
    virtual int Write(uint32_t column) override;

    /**
     * Write long data to the shared block.
     */
    virtual int Write(uint32_t column, int64_t value) override;

    /**
     * Write Double data to the shared block.
     */
    virtual int Write(uint32_t column, double value) override;

    /**
     * Write blob data to the shared block.
     */
    virtual int Write(uint32_t column, const uint8_t *value, size_t Size) override;

    /**
     * Write string data to the shared block.
     */
    virtual int Write(uint32_t column, const char *value, size_t sizeIncludingNull) override;

    /**
     * Get Block
     */
    AppDataFwk::SharedBlock *GetBlock() const;

private:
    /**
     * The fd of shared memory
     */
    bool GetCurrentRowIndex(uint32_t &rowIndex);

    /**
     * Convert ShareBlock error code to DataShare format
     */
    int ConvertErrorCode(int shareBlockErr)
    {
        return shareBlockErr == AppDataFwk::SharedBlock::SHARED_BLOCK_OK ? E_OK : E_ERROR;
    }

private:
    AppDataFwk::SharedBlock *shareBlock_;
};
} // namespace DataShare
} // namespace OHOS
#endif
