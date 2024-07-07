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

#include "recorder_pipeline.h"
#include <gst/gst.h>
#include "string_ex.h"
#include "media_errors.h"
#include "media_log.h"
#include "i_recorder_engine.h"
#include "recorder_private_param.h"
#include "scope_guard.h"

namespace {
    constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, LOG_DOMAIN, "RecorderPipeline"};
}

namespace OHOS {
namespace Media {
RecorderPipeline::RecorderPipeline(std::shared_ptr<RecorderPipelineDesc> desc)
    : desc_(desc)
{
    MEDIA_LOGD("enter, ctor");
}

RecorderPipeline::~RecorderPipeline()
{
    MEDIA_LOGD("enter, dtor");
    (void)Reset();
}

void RecorderPipeline::SetNotifier(RecorderMsgNotifier notifier)
{
    std::unique_lock<std::mutex> lock(gstPipeMutex_);
    notifier_ = notifier;
}

void RecorderPipeline::SetExecuteInCmdQ(RecorderExecuteInCmdQ executeInCmdQ)
{
    std::unique_lock<std::mutex> lock(cmdQMutex_);
    executeInCmdQ_ = executeInCmdQ;
}

int32_t RecorderPipeline::Init()
{
    if (desc_ == nullptr) {
        MEDIA_LOGE("pipeline desc is nullptr");
        return MSERR_INVALID_OPERATION;
    }

    gstPipeline_ = reinterpret_cast<GstPipeline *>(gst_pipeline_new("recorder-pipeline"));
    if (gstPipeline_ == nullptr) {
        MEDIA_LOGE("Create gst pipeline failed !");
        return MSERR_NO_MEMORY;
    }

    GstBus *bus = gst_pipeline_get_bus(gstPipeline_);
    CHECK_AND_RETURN_RET(bus != nullptr, MSERR_INVALID_OPERATION);

    auto msgResCb = std::bind(&RecorderPipeline::OnNotifyMsgProcResult, this, std::placeholders::_1);
    msgProcessor_ = std::make_unique<RecorderMsgProcessor>(*bus, msgResCb);
    gst_object_unref(bus);

    int32_t ret = msgProcessor_->Init();
    if (ret != MSERR_OK) {
        MEDIA_LOGE("Init RecorderMsgProcessor Failed !,  ret = %{publiuc}d", ret);
        ClearResource();
        return ret;
    }

    for (auto &elem : desc_->allElems) {
        msgProcessor_->AddMsgHandler(elem);
    }

    return MSERR_OK;
}

int32_t RecorderPipeline::Prepare()
{
    MEDIA_LOGD("enter Prepare");

    CHECK_AND_RETURN_RET(!errorState_.load(), MSERR_INVALID_STATE);

    int32_t ret = DoElemAction(&RecorderElement::Prepare);
    CHECK_AND_RETURN_RET(ret == MSERR_OK, ret);

    ret = SyncWaitChangeState(GST_STATE_PAUSED);
    CHECK_AND_RETURN_RET(ret == MSERR_OK, ret);

    return MSERR_OK;
}

int32_t RecorderPipeline::Start()
{
    MEDIA_LOGD("enter Start");

    CHECK_AND_RETURN_RET(!errorState_.load(), MSERR_INVALID_STATE);

    int32_t ret = DoElemAction(&RecorderElement::Start);
    CHECK_AND_RETURN_RET(ret == MSERR_OK, ret);

    ret = SyncWaitChangeState(GST_STATE_PLAYING);
    CHECK_AND_RETURN_RET(ret == MSERR_OK, ret);

    isStarted_ = true;
    return MSERR_OK;
}

int32_t RecorderPipeline::Pause()
{
    MEDIA_LOGD("enter Pause");

    CHECK_AND_RETURN_RET(!errorState_.load(), MSERR_INVALID_STATE);

    int32_t ret = DoElemAction(&RecorderElement::Pause);
    CHECK_AND_RETURN_RET(ret == MSERR_OK, ret);

    return SyncWaitChangeState(GST_STATE_PAUSED);
}

int32_t RecorderPipeline::Resume()
{
    MEDIA_LOGD("enter Resume");

    CHECK_AND_RETURN_RET(!errorState_.load(), MSERR_INVALID_STATE);

    int32_t ret = DoElemAction(&RecorderElement::Resume);
    CHECK_AND_RETURN_RET(ret == MSERR_OK, ret);

    return SyncWaitChangeState(GST_STATE_PLAYING);
}

int32_t RecorderPipeline::Stop(bool isDrainAll)
{
    if (errorState_.load()) {
        return MSERR_INVALID_STATE;
    }

    if (currState_ == GST_STATE_NULL) {
        return MSERR_OK;
    }

    (void)DoElemAction(&RecorderElement::Stop, false);

    if (currState_ != GST_STATE_READY) {
        MEDIA_LOGI("enter Stop, isDrainAll = %{public}d", isDrainAll);
        DrainBuffer(isDrainAll);
    }

    int32_t ret = SyncWaitChangeState(GST_STATE_NULL);
    CHECK_AND_RETURN_RET_LOG(ret == MSERR_OK, ret, "Stop failed !");

    isStarted_ = false;
    return MSERR_OK;
}

int32_t RecorderPipeline::SetParameter(int32_t sourceId, const RecorderParam &recParam)
{
    CHECK_AND_RETURN_RET(!errorState_.load(), MSERR_INVALID_STATE);

    int32_t ret = MSERR_OK;
    for (auto &elem : desc_->allElems) {
        if (elem->GetSourceId() == sourceId)  {
            ret = elem->SetParameter(recParam);
            CHECK_AND_RETURN_RET(ret == MSERR_OK, ret);
        }
    }

    return ret;
}

int32_t RecorderPipeline::GetParameter(int32_t sourceId, RecorderParam &recParam)
{
    CHECK_AND_RETURN_RET(!errorState_.load(), MSERR_INVALID_STATE);
    CHECK_AND_RETURN_RET(desc_ != nullptr, MSERR_INVALID_STATE);

    if (desc_->srcElems.find(sourceId) == desc_->srcElems.end()) {
        MEDIA_LOGE("invalid sourceId %{public}d", sourceId);
        return MSERR_INVALID_VAL;
    }
    return desc_->srcElems[sourceId]->GetParameter(recParam);
}

int32_t RecorderPipeline::SyncWaitChangeState(GstState targetState)
{
    MEDIA_LOGI("change state to %{public}d", targetState);

    GstStateChangeReturn stateRet = gst_element_set_state((GstElement *)gstPipeline_, targetState);
    if (stateRet == GST_STATE_CHANGE_FAILURE) {
        errorState_.store(true);
        return MSERR_INVALID_STATE;
    }

    if (stateRet != GST_STATE_CHANGE_ASYNC) {
        MEDIA_LOGI("finish change gstpipeline state to %{public}d.", targetState);
        currState_ = targetState;
        return MSERR_OK;
    }

    MEDIA_LOGI("begin sync wait gstpipeline state change to %{public}d..........", targetState);
    std::unique_lock<std::mutex> lock(gstPipeMutex_);
    gstPipeCond_.wait(lock, [this, targetState] { return currState_ == targetState || errorState_.load(); });
    if (errorState_.load()) {
        MEDIA_LOGE("error happened, change state to %{public}d failed !", targetState);
        return MSERR_INVALID_STATE;
    }
    MEDIA_LOGI("finish change gstpipeline state to %{public}d..........", targetState);
    return MSERR_OK;
}

void RecorderPipeline::DrainBuffer(bool isDrainAll)
{
    if (currState_ == GST_STATE_PAUSED) {
        if (isStarted_) {
            (void)SyncWaitChangeState(GST_STATE_PLAYING);
        } else {
            return;
        }
    }

    int32_t ret = MSERR_OK;
    auto iter = desc_->allElems.begin();
    for (size_t index = 0; index < desc_->srcElems.size(); index++, iter = std::next(iter)) {
        ret = (*iter)->DrainAll(isDrainAll);
        if (ret != MSERR_OK) {
            MEDIA_LOGE("drain [%{public}d] failed for %{public}s", isDrainAll, (*iter)->GetName().c_str());
            break;
        }
    }

    if (ret == MSERR_OK) {
        (void)SyncWaitEOS();
    }
}

bool RecorderPipeline::SyncWaitEOS()
{
    MEDIA_LOGI("Wait EOS finished........................");
    std::unique_lock<std::mutex> lock(gstPipeMutex_);
    if (errorState_.load()) {
        static constexpr int32_t timeout = 1; // wait 1s for eos finished
        gstPipeCond_.wait_for(lock, std::chrono::seconds(timeout), [this] { return eosDone_; });
    } else {
        gstPipeCond_.wait(lock, [this] { return eosDone_ || errorState_.load(); });
    }
    
    if (!eosDone_) {
        MEDIA_LOGE("error happened, wait eos done failed !");
        return false;
    }
    eosDone_ = false;
    MEDIA_LOGI("EOS finished........................");
    return true;
}

int32_t RecorderPipeline::Reset()
{
    (void)Stop(false);
    (void)DoElemAction(&RecorderElement::Reset, false);
    ClearResource();
    return MSERR_OK;
}

int32_t RecorderPipeline::DoElemAction(const ElemAction &action, bool needAllSucc)
{
    if (desc_ == nullptr)  {
        return MSERR_INVALID_OPERATION;
    }

    bool allSucc = true;
    for (auto &elem : desc_->allElems) {
        int32_t ret = action(*elem);
        if (ret == MSERR_OK) {
            continue;
        }
        allSucc = false;
        // if one element execute action fail, exit immediately.
        if (needAllSucc) {
            MEDIA_LOGE("element %{public}s execute action failed", elem->GetName().c_str());
            return ret;
        }
    }

    return allSucc ? MSERR_OK : MSERR_INVALID_OPERATION;
}

void RecorderPipeline::ClearResource()
{
    if (msgProcessor_ != nullptr) {
        (void)msgProcessor_->Reset();
        msgProcessor_ = nullptr;
    }

    if (gstPipeline_ != nullptr) {
        gst_object_unref(gstPipeline_);
        gstPipeline_ = nullptr;
    }

    desc_ = nullptr;
}

void RecorderPipeline::Dump()
{
    MEDIA_LOGI("==========================Dump Recorder Parameters Begin=========================");
    for (auto &elem : desc_->allElems) {
        elem->Dump();
    }
    MEDIA_LOGI("==========================Dump Recorder Parameters End===========================");
}

void RecorderPipeline::OnNotifyMsgProcResult(const RecorderMessage &msg)
{
    if (msg.type == RecorderMessageType::REC_MSG_INFO) {
        return ProcessInfoMessage(msg);
    }

    if (msg.type == RecorderMessageType::REC_MSG_ERROR) {
        return ProcessErrorMessage(msg);
    }

    if (msg.type == RecorderMessageType::REC_MSG_FEATURE) {
        return ProcessFeatureMessage(msg);
    }
}

void RecorderPipeline::ProcessInfoMessage(const RecorderMessage &msg)
{
    NotifyMessage(msg);
}

void RecorderPipeline::ProcessErrorMessage(const RecorderMessage &msg)
{
    // ignore the error msg
    if (errorState_.load() || (errorSources_.count(msg.sourceId) != 0)) {
        return;
    }

    if (CheckStopForError(msg)) {
        StopForError(msg);
        return;
    }

    int ret = BypassOneSource(msg.sourceId);
    if (ret != MSERR_OK) {
        MEDIA_LOGE("bypass source[0x%{public}x] failed, stop recording.", msg.sourceId);
        StopForError(msg);
        return;
    }
    NotifyMessage(msg);
}

void RecorderPipeline::ProcessFeatureMessage(const RecorderMessage &msg)
{
    switch (msg.code) {
        case REC_MSG_FEATURE_ASYNC_DONE: {
            {
                std::unique_lock<std::mutex> lock(gstPipeMutex_);
                asyncDone_ = true;
                MEDIA_LOGI("Accept message GST_MESSAGE_ASYNC_DONE");
            }
            gstPipeCond_.notify_one();
            break;
        }
        case REC_MSG_FEATURE_EOS_DONE: {
            {
                std::unique_lock<std::mutex> lock(gstPipeMutex_);
                eosDone_ = true;
                MEDIA_LOGI("Accept message GST_MESSAGE_EOS");
            }
            gstPipeCond_.notify_one();
            break;
        }
        case REC_MSG_FEATURE_STATE_CHANGE_DONE: {
            {
                std::unique_lock<std::mutex> lock(gstPipeMutex_);
                currState_ = static_cast<GstState>(msg.detail);
                MEDIA_LOGI("Accept message REC_MSG_FEATURE_STATE_CHANGE_DONE, currState = %{public}d", currState_);
            }
            gstPipeCond_.notify_one();
            break;
        }
        default:
            MEDIA_LOGW("unknown feature message: %{public}d", msg.code);
            break;
    }
}

void RecorderPipeline::NotifyMessage(const RecorderMessage &msg)
{
    std::unique_lock<std::mutex> lock(gstPipeMutex_);
    if (notifier_ != nullptr) {
        notifier_(msg);
    }
}

bool RecorderPipeline::CheckStopForError(const RecorderMessage &msg)
{
    // Not meaningful sourceId, means the error is related to all sources, and the recording must be stopped.
    if (msg.sourceId == INVALID_SOURCE_ID || msg.sourceId == DUMMY_SOURCE_ID) {
        return true;
    }

    (void)errorSources_.emplace(msg.sourceId);
    return errorSources_.size() == desc_->srcElems.size();
}

void RecorderPipeline::StopForError(const RecorderMessage &msg)
{
    MEDIA_LOGE("Fatal error happened, stop recording. Error code: %{public}d, detail: %{public}d",
               msg.code, msg.detail);
    NotifyMessage(msg);

    errorState_.store(true);
    gstPipeCond_.notify_all();

    auto stopforErrorTask = std::make_shared<TaskHandler<int32_t>>([this] {
        (void)DoElemAction(&RecorderElement::Stop, false);
        DrainBuffer(false);
        (void)SyncWaitChangeState(GST_STATE_NULL);
        return MSERR_OK;
    });

    std::unique_lock<std::mutex> lock(cmdQMutex_);
    CHECK_AND_RETURN(executeInCmdQ_ != nullptr);
    (void)executeInCmdQ_(stopforErrorTask, true);
    
    isStarted_ = false;
}

int32_t RecorderPipeline::BypassOneSource(int32_t sourceId)
{
    MEDIA_LOGE("recorder source[0x%{public}x] has error happened, bypass it", sourceId);

    auto srcElemIter = desc_->srcElems.find(sourceId);
    if (srcElemIter == desc_->srcElems.end() || srcElemIter->second == nullptr) {
        MEDIA_LOGE("The sourceId 0x%{public}x is unrecognizable, ignored !", sourceId);
        std::string srcIdStr;
        for (auto &srcElemItem : desc_->srcElems) {
            srcIdStr += DexToHexString(srcElemItem.first) + " ";
        }
        MEDIA_LOGE("Valid source id: %{public}s", srcIdStr.c_str());
        return MSERR_INVALID_VAL;
    }

    bool ret = srcElemIter->second->DrainAll(true);
    CHECK_AND_RETURN_RET(ret, MSERR_INVALID_OPERATION);

    return MSERR_OK;
}
} // namespace Media
} // namespace OHOS
