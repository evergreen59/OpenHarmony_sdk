/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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
#ifndef PKG_STREAM_H
#define PKG_STREAM_H
#ifndef __WIN32
#include <sys/mman.h>
#endif
#include <atomic>
#include "pkg_manager.h"
#include "pkg_utils.h"

namespace Hpackage {
class PkgStreamImpl;
using PkgStreamPtr = PkgStreamImpl *;
class PkgStreamImpl : public PkgStream {
public:
    explicit PkgStreamImpl(PkgManager::PkgManagerPtr pkgManager, const std::string fileName)
        : fileName_(fileName), refCount_(0), pkgManager_(pkgManager) {}

    virtual ~PkgStreamImpl() {}

    int32_t Read(const PkgBuffer &data, size_t start, size_t needRead, size_t &readLen) override
    {
        UNUSED(data);
        UNUSED(start);
        UNUSED(readLen);
        UNUSED(needRead);
        return PKG_SUCCESS;
    }

    int32_t GetBuffer(PkgBuffer &buffer) const override
    {
        buffer.length = 0;
        buffer.buffer = nullptr;
        return PKG_SUCCESS;
    }

    virtual int32_t Write(const PkgBuffer &data, size_t size, size_t start) = 0;

    virtual int32_t Seek(long int offset, int whence) = 0;

    virtual int32_t Flush(size_t size) = 0;

    const std::string GetFileName() const override;

    int32_t GetStreamType() const override
    {
        return PkgStreamType_Read;
    };

    void AddRef();

    void DelRef();

    bool IsRef() const;

    static PkgStreamPtr ConvertPkgStream(PkgManager::StreamPtr stream);

protected:
    void PostDecodeProgress(int type, size_t writeDataLen, const void *context) const;
    std::string fileName_;

private:
    std::atomic_int refCount_;
    PkgManager::PkgManagerPtr pkgManager_ = nullptr;
};

class FileStream : public PkgStreamImpl {
public:
    FileStream(PkgManager::PkgManagerPtr pkgManager, const std::string fileName, FILE *stream, int32_t streamType)
        : PkgStreamImpl(pkgManager, fileName), stream_(stream), fileLength_(0), streamType_(streamType) {}

    ~FileStream() override;

    int32_t Read(const PkgBuffer &data, size_t start, size_t needRead, size_t &readLen) override;

    int32_t Write(const PkgBuffer &data, size_t size, size_t start) override;

    int32_t Seek(long int offset, int whence) override;

    int32_t Flush(size_t size) override;

    size_t GetFileLength() override;

    int32_t GetStreamType() const override
    {
        return streamType_;
    }

private:
    FILE *stream_;
    size_t fileLength_;
    int32_t streamType_;
};

class MemoryMapStream : public PkgStreamImpl {
public:
    MemoryMapStream(PkgManager::PkgManagerPtr pkgManager, const std::string fileName, const PkgBuffer &buffer,
        int32_t streamType = PkgStreamType_MemoryMap) : PkgStreamImpl(pkgManager, fileName), memMap_(buffer.buffer),
        memSize_(buffer.length), currOffset_(0), streamType_(streamType) {}
    ~MemoryMapStream() override;

    int32_t Read(const PkgBuffer &data, size_t start, size_t needRead, size_t &readLen) override;

    int32_t Write(const PkgBuffer &data, size_t size, size_t start) override;

    int32_t Seek(long int offset, int whence) override;

    int32_t GetStreamType() const override
    {
        return streamType_;
    }

    size_t GetFileLength() override
    {
        return memSize_;
    }

    int32_t Flush(size_t size) override
    {
        if (size != memSize_) {
            PKG_LOGE("Flush size %zu local size:%zu", size, memSize_);
        }
        if (streamType_ == PkgStreamType_MemoryMap) {
            msync(static_cast<void *>(memMap_), memSize_, MS_ASYNC);
        }
        currOffset_ = size;
        return PKG_SUCCESS;
    }

    int32_t GetBuffer(PkgBuffer &buffer) const override
    {
        buffer.buffer = memMap_;
        buffer.length = memSize_;
        return PKG_SUCCESS;
    }

private:
    uint8_t *memMap_;
    size_t memSize_;
    size_t currOffset_;
    int32_t streamType_;
};

class ProcessorStream : public PkgStreamImpl {
public:
    ProcessorStream(PkgManager::PkgManagerPtr pkgManager, const std::string fileName,
        ExtractFileProcessor processor, const void *context)
        : PkgStreamImpl(pkgManager, fileName), processor_(processor), context_(context) {}

    ~ProcessorStream() override {}

    int32_t Read(const PkgBuffer &data, size_t start, size_t needRead, size_t &readLen) override
    {
        UNUSED(data);
        UNUSED(start);
        UNUSED(readLen);
        UNUSED(needRead);
        return PKG_INVALID_STREAM;
    }

    int32_t Write(const PkgBuffer &data, size_t size, size_t start) override
    {
        if (processor_ == nullptr) {
            PKG_LOGE("processor not exist");
            return PKG_INVALID_STREAM;
        }
        int ret = processor_(data, size, start, false, context_);
        PostDecodeProgress(POST_TYPE_DECODE_PKG, size, nullptr);
        return ret;
    }

    int32_t Seek(long int size, int whence) override
    {
        UNUSED(size);
        UNUSED(whence);
        return PKG_SUCCESS;
    }

    int32_t GetStreamType() const override
    {
        return PkgStreamType_Process;
    }

    size_t GetFileLength() override
    {
        return 0;
    }

    int32_t Flush(size_t size) override
    {
        UNUSED(size);
        if (processor_ == nullptr) {
            PKG_LOGE("processor not exist");
            return PKG_INVALID_STREAM;
        }
        PkgBuffer data = {};
        return processor_(data, 0, 0, true, context_);
    }

private:
    ExtractFileProcessor processor_ = nullptr;
    const void *context_;
};
} // namespace Hpackage
#endif // PKG_STREAM_H
