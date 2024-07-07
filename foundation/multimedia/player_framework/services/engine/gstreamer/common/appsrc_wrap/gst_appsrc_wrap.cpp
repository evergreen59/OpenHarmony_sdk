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

#include "gst_appsrc_wrap.h"
#include "avsharedmemorybase.h"
#include "media_log.h"
#include "media_errors.h"
#include "securec.h"

namespace {
    constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, LOG_DOMAIN, "GstAppsrcWrap"};
    constexpr int32_t BUFFERS_NUM = 5;
    constexpr int32_t BUFFER_SIZE = 81920;
    constexpr int64_t INVALID_SIZE = -1;
}

namespace OHOS {
namespace Media {
std::shared_ptr<GstAppsrcWrap> GstAppsrcWrap::Create(const std::shared_ptr<IMediaDataSource> &dataSrc)
{
    CHECK_AND_RETURN_RET_LOG(dataSrc != nullptr, nullptr, "input dataSrc is empty!");
    int64_t size = 0;
    int32_t ret = dataSrc->GetSize(size);
    CHECK_AND_RETURN_RET_LOG(ret == MSERR_OK, nullptr, "media data source get size failed!");
    CHECK_AND_RETURN_RET_LOG(size >= INVALID_SIZE, nullptr, "size cannot less than -1");
    std::shared_ptr<GstAppsrcWrap> wrap = std::make_shared<GstAppsrcWrap>(dataSrc, size);
    CHECK_AND_RETURN_RET_LOG(wrap->Init() == MSERR_OK, nullptr, "init failed");
    return wrap;
}

GstAppsrcWrap::GstAppsrcWrap(const std::shared_ptr<IMediaDataSource> &dataSrc, const int64_t size)
    : dataSrc_(dataSrc),
      size_(size),
      fillTaskQue_("fillbufferTask"),
      emptyTaskQue_("emptybufferTask"),
      bufferSize_(BUFFER_SIZE),
      buffersNum_(BUFFERS_NUM)
{
    MEDIA_LOGD("0x%{public}06" PRIXPTR " Instances create and size %{public}" PRId64 "", FAKE_POINTER(this), size);
    streamType_ = size == INVALID_SIZE ? GST_APP_STREAM_TYPE_STREAM : GST_APP_STREAM_TYPE_RANDOM_ACCESS;
}

GstAppsrcWrap::~GstAppsrcWrap()
{
    MEDIA_LOGD("0x%{public}06" PRIXPTR " Instances destroy", FAKE_POINTER(this));
    Stop();
    ClearAppsrc();
}

int32_t GstAppsrcWrap::Init()
{
    for (int i = 0; i < buffersNum_; ++i) {
        std::shared_ptr<AppsrcMemWrap> appSrcMem = std::make_shared<AppsrcMemWrap>();
        CHECK_AND_RETURN_RET_LOG(appSrcMem != nullptr, MSERR_NO_MEMORY, "init AppsrcMemWrap failed");
        appSrcMem->mem = AVSharedMemoryBase::CreateFromLocal(
            bufferSize_, AVSharedMemory::Flags::FLAGS_READ_WRITE, "appsrc");
        CHECK_AND_RETURN_RET_LOG(appSrcMem->mem != nullptr, MSERR_NO_MEMORY, "init AVSharedMemory failed");
        (void)emptyBuffers_.emplace(appSrcMem);
    }
    return MSERR_OK;
}

int32_t GstAppsrcWrap::Prepare()
{
    std::unique_lock<std::mutex> lock(mutex_);
    MEDIA_LOGD("Prepare in");
    if (!isExit_) {
        MEDIA_LOGD("Prepared");
        return MSERR_OK;
    }
    isExit_ = false;
    needData_ = false;
    filledBufferSize_ = 0;
    needDataSize_ = 0;
    atEos_ = false;
    curPos_ = 0;
    while (!filledBuffers_.empty()) {
        std::shared_ptr<AppsrcMemWrap> appSrcMem = filledBuffers_.front();
        filledBuffers_.pop();
        emptyBuffers_.push(appSrcMem);
    }
    CHECK_AND_RETURN_RET_LOG(fillTaskQue_.Start() == MSERR_OK, MSERR_INVALID_OPERATION, "init task failed");
    CHECK_AND_RETURN_RET_LOG(emptyTaskQue_.Start() == MSERR_OK, MSERR_INVALID_OPERATION, "init task failed");
    auto task = std::make_shared<TaskHandler<void>>([this] {
        FillTask();
    });
    CHECK_AND_RETURN_RET_LOG(fillTaskQue_.EnqueueTask(task) == MSERR_OK,
        MSERR_INVALID_OPERATION, "enque task failed");
    task = std::make_shared<TaskHandler<void>>([this] {
        EmptyTask();
    });
    CHECK_AND_RETURN_RET_LOG(emptyTaskQue_.EnqueueTask(task) == MSERR_OK,
        MSERR_INVALID_OPERATION, "enque task failed");
    MEDIA_LOGD("Prepare out");
    return MSERR_OK;
}

void GstAppsrcWrap::Stop()
{
    {
        std::unique_lock<std::mutex> lock(mutex_);
        isExit_ = true;
        if (bufferWrap_ != nullptr) {
            gst_buffer_unref(bufferWrap_->buffer);
            bufferWrap_ = nullptr;
        }
        fillCond_.notify_all();
        emptyCond_.notify_all();
    }
    (void)fillTaskQue_.Stop();
    (void)emptyTaskQue_.Stop();
}

void GstAppsrcWrap::ClearAppsrc()
{
    if (appSrc_ != nullptr) {
        for (auto &id : callbackIds_) {
            g_signal_handler_disconnect(appSrc_, id);
        }
        callbackIds_.clear();
        gst_object_unref(appSrc_);
        appSrc_ = nullptr;
    }
}

int32_t GstAppsrcWrap::SetAppsrc(GstElement *appSrc)
{
    MEDIA_LOGD("set Appsrc");
    ClearAppsrc();
    appSrc_ = static_cast<GstElement *>(gst_object_ref(appSrc));
    CHECK_AND_RETURN_RET_LOG(appSrc_ != nullptr, MSERR_INVALID_VAL, "set appsrc failed");
    SetCallBackForAppSrc();
    return MSERR_OK;
}

void GstAppsrcWrap::SetCallBackForAppSrc()
{
    MEDIA_LOGD("SetCallBackForAppSrc");
    CHECK_AND_RETURN_LOG(appSrc_ != nullptr, "appSrc_ is nullptr");
    int64_t size = static_cast<int64_t>(size_);
    g_object_set(appSrc_, "stream-type", streamType_, nullptr);
    g_object_set(appSrc_, "format", GST_FORMAT_BYTES, nullptr);
    g_object_set(appSrc_, "size", size, nullptr);
    callbackIds_.push_back(g_signal_connect(appSrc_, "need-data", G_CALLBACK(NeedData), this));
    if (streamType_ == GST_APP_STREAM_TYPE_RANDOM_ACCESS) {
        callbackIds_.push_back(g_signal_connect(appSrc_, "seek-data", G_CALLBACK(SeekData), this));
    }
    MEDIA_LOGD("setcall back end");
}

bool GstAppsrcWrap::IsLiveMode() const
{
    return streamType_ == GST_APP_STREAM_TYPE_STREAM;
}

int32_t GstAppsrcWrap::SetErrorCallback(AppsrcErrorNotifier notifier)
{
    std::unique_lock<std::mutex> lock(mutex_);
    notifier_ = notifier;
    return MSERR_OK;
}

void GstAppsrcWrap::NeedData(const GstElement *appSrc, uint32_t size, gpointer self)
{
    (void)appSrc;
    CHECK_AND_RETURN_LOG(self != nullptr, "self is nullptr");
    auto wrap = static_cast<GstAppsrcWrap *>(self);
    wrap->NeedDataInner(size);
}

void GstAppsrcWrap::NeedDataInner(uint32_t size)
{
    std::unique_lock<std::mutex> lock(mutex_);
    needDataSize_ = static_cast<int32_t>(size);
    if (!filledBuffers_.empty() && (needDataSize_ <= filledBufferSize_ || atEos_ ||
        streamType_ == GST_APP_STREAM_TYPE_STREAM) && !isExit_) {
        int32_t ret = GetAndPushMem();
        if (ret != MSERR_OK) {
            OnError(ret);
        }
    } else {
        needData_ = true;
        if (!filledBuffers_.empty()) {
            emptyCond_.notify_all();
        }
    }
}

void GstAppsrcWrap::FillTask()
{
    int32_t ret = ReadAndGetMem();
    if (ret != MSERR_OK) {
        OnError(ret);
    }
}

void GstAppsrcWrap::EmptyTask()
{
    int32_t ret = MSERR_OK;
    while (ret == MSERR_OK) {
        std::unique_lock<std::mutex> lock(mutex_);
        emptyCond_.wait(lock, [this] {
            return (!filledBuffers_.empty() && needData_) || isExit_;
        });
        if (isExit_) {
            break;
        }
        ret = GetAndPushMem();
    }
    if (ret != MSERR_OK) {
        OnError(ret);
    }
}

gboolean GstAppsrcWrap::SeekData(const GstElement *appSrc, uint64_t seekPos, gpointer self)
{
    MEDIA_LOGD("SeekData pos: %{public}" PRIu64 "", seekPos);
    (void)appSrc;
    CHECK_AND_RETURN_RET_LOG(self != nullptr, FALSE, "self is nullptr");
    auto wrap = static_cast<GstAppsrcWrap *>(self);
    return wrap->SeekDataInner(seekPos);
}

void GstAppsrcWrap::SeekAndFreeBuffers(uint64_t pos)
{
    std::unique_lock<std::mutex> lock(mutex_);
    while (!filledBuffers_.empty()) {
        std::shared_ptr<AppsrcMemWrap> appSrcMem = filledBuffers_.front();
        CHECK_AND_RETURN_LOG(appSrcMem != nullptr, "appSrcMem is nullptr");
        if (appSrcMem->size < 0) {
            filledBuffers_.pop();
            emptyBuffers_.push(appSrcMem);
            continue;
        }
        if (appSrcMem->pos <= pos && appSrcMem->pos + static_cast<uint64_t>(appSrcMem->size) > pos) {
            int32_t len = static_cast<int32_t>(pos - appSrcMem->pos);
            filledBufferSize_ += appSrcMem->offset;
            appSrcMem->offset = len;
            filledBufferSize_ -= appSrcMem->offset;
            break;
        }
        filledBufferSize_ = filledBufferSize_ - (appSrcMem->size - appSrcMem->offset);
        filledBuffers_.pop();
        emptyBuffers_.push(appSrcMem);
    }
    if (filledBuffers_.empty()) {
        curPos_ = pos;
        atEos_ = false;
    }
    fillCond_.notify_all();
}

gboolean GstAppsrcWrap::SeekDataInner(uint64_t pos)
{
    SeekAndFreeBuffers(pos);
    return TRUE;
}

int32_t GstAppsrcWrap::ReadAndGetMem()
{
    int32_t ret = MSERR_OK;
    while (ret == MSERR_OK) {
        int32_t size = 0;
        std::shared_ptr<AppsrcMemWrap> appSrcMem = nullptr;
        {
            std::unique_lock<std::mutex> lock(mutex_);
            fillCond_.wait(lock, [this] { return (!emptyBuffers_.empty() && !atEos_) || isExit_; });
            if (isExit_) {
                break;
            }
            appSrcMem = emptyBuffers_.front();
            CHECK_AND_RETURN_RET_LOG(appSrcMem != nullptr && appSrcMem->mem != nullptr, MSERR_NO_MEMORY, "no mem");
            appSrcMem->pos = curPos_;
            emptyBuffers_.pop();
        }
        if (size_ == INVALID_SIZE) {
            size = dataSrc_->ReadAt(bufferSize_, appSrcMem->mem);
        } else {
            size = dataSrc_->ReadAt(static_cast<int64_t>(appSrcMem->pos), bufferSize_, appSrcMem->mem);
        }
        if (size > appSrcMem->mem->GetSize()) {
            ret = MSERR_INVALID_VAL;
        }
        {
            std::unique_lock<std::mutex> lock(mutex_);
            if (size == 0 || curPos_ != appSrcMem->pos) {
                emptyBuffers_.push(appSrcMem);
            } else if (size < 0) {
                appSrcMem->size = size;
                atEos_ = true;
                filledBuffers_.push(appSrcMem);
            } else {
                size = std::min(size, appSrcMem->mem->GetSize());
                appSrcMem->size = size;
                filledBufferSize_ += size;
                appSrcMem->pos = curPos_;
                appSrcMem->offset = 0;
                curPos_ = curPos_ + static_cast<uint64_t>(size);
                filledBuffers_.push(appSrcMem);
            }
            emptyCond_.notify_all();
        }
    }
    return ret;
}

void GstAppsrcWrap::EosAndCheckSize(int32_t size)
{
    MEDIA_LOGD("%{public}d", size);
    PushEos();
    switch (size) {
        case SOURCE_ERROR_IO:
            OnError(MSERR_DATA_SOURCE_IO_ERROR);
            MEDIA_LOGW("IO ERROR %{public}d", size);
            break;
        case SOURCE_ERROR_EOF:
            break;
        default:
            OnError(MSERR_DATA_SOURCE_ERROR_UNKNOWN);
            MEDIA_LOGE("unknown error %{public}d", size);
            break;
    }
}

int32_t GstAppsrcWrap::GetAndPushMem()
{
    int32_t size = needDataSize_ > filledBufferSize_ ? filledBufferSize_ : needDataSize_;
    std::shared_ptr<AppsrcMemWrap> appSrcMem = filledBuffers_.front();
    CHECK_AND_RETURN_RET_LOG(appSrcMem != nullptr && appSrcMem->mem != nullptr, MSERR_NO_MEMORY, "no mem");
    if (size == 0) {
        EosAndCheckSize(appSrcMem->size);
        filledBuffers_.pop();
        emptyBuffers_.push(appSrcMem);
        needData_ = false;
        return MSERR_OK;
    }
    GstBuffer *buffer = nullptr;
    if (bufferWrap_ != nullptr && bufferWrap_->buffer != nullptr) {
        buffer = bufferWrap_->buffer;
    } else {
        bufferWrap_ = std::make_shared<AppsrcBufferWrap>();
        int32_t allocSize = streamType_ == GST_APP_STREAM_TYPE_STREAM ? size : needDataSize_;
        buffer = gst_buffer_new_allocate(nullptr, static_cast<gsize>(allocSize), nullptr);
        CHECK_AND_RETURN_RET_LOG(buffer != nullptr, MSERR_NO_MEMORY, "no mem");
        GST_BUFFER_OFFSET(buffer) = appSrcMem->pos + static_cast<uint64_t>(appSrcMem->offset);
        bufferWrap_->buffer = buffer;
        bufferWrap_->offset = 0;
        bufferWrap_->size = allocSize;
    }
    GstMapInfo info = GST_MAP_INFO_INIT;
    if (gst_buffer_map(buffer, &info, GST_MAP_WRITE) == FALSE) {
        gst_buffer_unref(buffer);
        MEDIA_LOGE("map buffer failed");
        return MSERR_NO_MEMORY;
    }
    bool copyRet = CopyToGstBuffer(info);
    gst_buffer_unmap(buffer, &info);
    if (!copyRet) {
        MEDIA_LOGE("copy buffer failed");
        gst_buffer_unref(buffer);
        return MSERR_NO_MEMORY;
    }
    if (bufferWrap_->size == bufferWrap_->offset) {
        bufferWrap_ = nullptr;
        PushData(buffer);
        needDataSize_ = 0;
        needData_ = false;
        gst_buffer_unref(buffer);
    } else {
        needDataSize_ = bufferWrap_->size - bufferWrap_->offset;
    }
    filledBufferSize_ -= size;
    return MSERR_OK;
}

bool GstAppsrcWrap::CopyToGstBuffer(const GstMapInfo &info)
{
    guint8 *data = info.data + bufferWrap_->offset;
    int32_t size = static_cast<int32_t>(info.size) - bufferWrap_->offset;
    while (size > 0 && !filledBuffers_.empty()) {
        std::shared_ptr<AppsrcMemWrap> appSrcMem = filledBuffers_.front();
        CHECK_AND_BREAK_LOG(appSrcMem != nullptr && appSrcMem->mem != nullptr
            && appSrcMem->mem->GetBase() != nullptr
            && (appSrcMem->size - appSrcMem->offset) > 0,
            "get mem is nullptr");
        int32_t lastSize = appSrcMem->size - appSrcMem->offset;
        int32_t copySize = std::min(lastSize, size);
        CHECK_AND_BREAK_LOG(memcpy_s(data, static_cast<size_t>(size),
            appSrcMem->mem->GetBase() + appSrcMem->offset, static_cast<size_t>(copySize)) == EOK,
            "get mem is nullptr");
        if (lastSize <= size) {
            filledBuffers_.pop();
            emptyBuffers_.push(appSrcMem);
            fillCond_.notify_all();
        } else {
            appSrcMem->offset += copySize;
        }
        data = data + copySize;
        bufferWrap_->offset += copySize;
        size -= copySize;
    }
    if (size != 0 && !filledBuffers_.empty()) {
        return false;
    }
    return true;
}

void GstAppsrcWrap::OnError(int32_t errorCode)
{
    if (notifier_ != nullptr) {
        notifier_(errorCode);
    }
}

void GstAppsrcWrap::PushData(const GstBuffer *buffer) const
{
    int32_t ret = GST_FLOW_OK;
    if (appSrc_ != nullptr) {
        g_signal_emit_by_name(appSrc_, "push-buffer", buffer, &ret);
    }
}

void GstAppsrcWrap::PushEos()
{
    int32_t ret = GST_FLOW_OK;
    if (appSrc_ != nullptr) {
        g_signal_emit_by_name(appSrc_, "end-of-stream", &ret);
    }
    MEDIA_LOGD("appsrcPushEos ret:%{public}d", ret);
}
} // namespace Media
} // namespace OHOS
