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

#include "agent/debugger_impl.h"

#include "base/pt_base64.h"
#include "base/pt_events.h"
#include "base/pt_params.h"
#include "base/pt_returns.h"
#include "base/pt_types.h"
#include "backend/debugger_executor.h"
#include "dispatcher.h"
#include "protocol_handler.h"

#include "ecmascript/jspandafile/js_pandafile_manager.h"
#include "ecmascript/napi/jsnapi_helper.h"
namespace panda::ecmascript::tooling {
using namespace std::placeholders;

using ObjectType = RemoteObject::TypeName;
using ObjectSubType = RemoteObject::SubTypeName;
using ObjectClassName = RemoteObject::ClassName;
using StepperType = SingleStepper::Type;

#ifdef DEBUGGER_TEST
const std::string DATA_APP_PATH = "/";
#else
const std::string DATA_APP_PATH = "/data/";
#endif

DebuggerImpl::DebuggerImpl(const EcmaVM *vm, ProtocolChannel *channel, RuntimeImpl *runtime)
    : vm_(vm), frontend_(channel), runtime_(runtime)
{
    hooks_ = std::make_unique<JSPtHooks>(this);

    jsDebugger_ = DebuggerApi::CreateJSDebugger(vm_);
    DebuggerApi::RegisterHooks(jsDebugger_, hooks_.get());

    DebuggerExecutor::Initialize(vm_);
    updaterFunc_ = std::bind(&DebuggerImpl::UpdateScopeObject, this, _1, _2, _3);
    stepperFunc_ = std::bind(&DebuggerImpl::ClearSingleStepper, this);
    vm_->GetJsDebuggerManager()->SetLocalScopeUpdater(&updaterFunc_);
    vm_->GetJsDebuggerManager()->SetStepperFunc(&stepperFunc_);
}

DebuggerImpl::~DebuggerImpl()
{
    DebuggerApi::DestroyJSDebugger(jsDebugger_);
}

bool DebuggerImpl::NotifyScriptParsed(ScriptId scriptId, const std::string &fileName, std::string_view entryPoint)
{
#if !defined(PANDA_TARGET_WINDOWS) && !defined(PANDA_TARGET_MACOS) \
    && !defined(PANDA_TARGET_ANDROID) && !defined(PANDA_TARGET_IOS)
    if (fileName.substr(0, DATA_APP_PATH.length()) != DATA_APP_PATH) {
        LOG_DEBUGGER(DEBUG) << "NotifyScriptParsed: unsupport file: " << fileName;
        return false;
    }
#endif

    const JSPandaFile *jsPandaFile = nullptr;
    JSPandaFileManager::GetInstance()->EnumerateJSPandaFiles([&jsPandaFile, &fileName](
        const panda::ecmascript::JSPandaFile *pf) {
        if (fileName == pf->GetJSPandaFileDesc().c_str()) {
            jsPandaFile = pf;
            return false;
        }
        return true;
    });
    if (jsPandaFile == nullptr) {
        LOG_DEBUGGER(ERROR) << "NotifyScriptParsed: unknown file: " << fileName;
        return false;
    }

    DebugInfoExtractor *extractor = GetExtractor(jsPandaFile);
    if (extractor == nullptr) {
        LOG_DEBUGGER(ERROR) << "NotifyScriptParsed: Unsupported file: " << fileName;
        return false;
    }

    auto mainMethodIndex = panda_file::File::EntityId(jsPandaFile->GetMainMethodIndex(entryPoint.data()));
    const std::string &source = extractor->GetSourceCode(mainMethodIndex);
    const std::string &url = extractor->GetSourceFile(mainMethodIndex);
    const uint32_t MIN_SOURCE_CODE_LENGTH = 5;  // maybe return 'ANDA' when source code is empty
    if (source.size() < MIN_SOURCE_CODE_LENGTH) {
        LOG_DEBUGGER(ERROR) << "NotifyScriptParsed: invalid file: " << fileName;
        return false;
    }
    // store here for performance of get extractor from url
    extractors_[url] = extractor;

    auto scriptFunc = [this](PtScript *script) -> bool {
        frontend_.ScriptParsed(vm_, *script);
        return true;
    };
    if (MatchScripts(scriptFunc, url, ScriptMatchType::URL)) {
        LOG_DEBUGGER(WARN) << "NotifyScriptParsed: already loaded: " << url;
        return false;
    }

    // Notify script parsed event
    std::unique_ptr<PtScript> script = std::make_unique<PtScript>(scriptId, fileName, url, source);

    frontend_.ScriptParsed(vm_, *script);

    // Store parsed script in map
    scripts_[script->GetScriptId()] = std::move(script);
    return true;
}

bool DebuggerImpl::NotifySingleStep(const JSPtLocation &location)
{
    if (UNLIKELY(pauseOnNextByteCode_)) {
        if (IsSkipLine(location)) {
            return false;
        }
        pauseOnNextByteCode_ = false;
        LOG_DEBUGGER(INFO) << "StepComplete: pause on next bytecode";
        return true;
    }

    if (LIKELY(singleStepper_ == nullptr)) {
        return false;
    }

    // step not complete
    if (!singleStepper_->StepComplete(location.GetBytecodeOffset())) {
        return false;
    }

    // skip unknown file or special line -1
    if (IsSkipLine(location)) {
        return false;
    }

    singleStepper_.reset();
    LOG_DEBUGGER(INFO) << "StepComplete: pause on current byte_code";
    return true;
}

bool DebuggerImpl::IsSkipLine(const JSPtLocation &location)
{
    DebugInfoExtractor *extractor = nullptr;
    const auto *jsPandaFile = location.GetJsPandaFile();
    auto scriptFunc = [this, &extractor, jsPandaFile](PtScript *) -> bool {
        extractor = GetExtractor(jsPandaFile);
        return true;
    };

    // In hot reload scenario, use the base js panda file instead
    const auto &fileName = DebuggerApi::GetBaseJSPandaFile(vm_, jsPandaFile)->GetJSPandaFileDesc();
    if (!MatchScripts(scriptFunc, fileName.c_str(), ScriptMatchType::FILE_NAME) || extractor == nullptr) {
        LOG_DEBUGGER(INFO) << "StepComplete: skip unknown file " << fileName.c_str();
        return true;
    }

    auto callbackFunc = [](int32_t line) -> bool {
        return line == DebugInfoExtractor::SPECIAL_LINE_MARK;
    };
    File::EntityId methodId = location.GetMethodId();
    uint32_t offset = location.GetBytecodeOffset();
    if (extractor->MatchLineWithOffset(callbackFunc, methodId, offset)) {
        LOG_DEBUGGER(INFO) << "StepComplete: skip -1";
        return true;
    }

    return false;
}

bool DebuggerImpl::CheckPauseOnException()
{
    if (pauseOnException_ == PauseOnExceptionsState::NONE) {
        return false;
    }
    if (pauseOnException_ == PauseOnExceptionsState::UNCAUGHT) {
        if (DebuggerApi::IsExceptionCaught(vm_)) {
            return false;
        }
    }
    return true;
}

void DebuggerImpl::NotifyPaused(std::optional<JSPtLocation> location, PauseReason reason)
{
    if (reason == EXCEPTION && !CheckPauseOnException()) {
        return;
    }

    Local<JSValueRef> exception = DebuggerApi::GetAndClearException(vm_);

    std::vector<std::string> hitBreakpoints;
    if (location.has_value()) {
        BreakpointDetails detail;
        DebugInfoExtractor *extractor = nullptr;
        auto scriptFunc = [this, &location, &detail, &extractor](PtScript *script) -> bool {
            detail.url_ = script->GetUrl();
            extractor = GetExtractor(location->GetJsPandaFile());
            return true;
        };
        auto callbackLineFunc = [&detail](int32_t line) -> bool {
            detail.line_ = line;
            return true;
        };
        auto callbackColumnFunc = [&detail](int32_t column) -> bool {
            detail.column_ = column;
            return true;
        };
        File::EntityId methodId = location->GetMethodId();
        uint32_t offset = location->GetBytecodeOffset();
        // In merge abc scenario, need to use the source file to match to get right url
        if (!MatchScripts(scriptFunc, location->GetSourceFile(), ScriptMatchType::URL) ||
            extractor == nullptr || !extractor->MatchLineWithOffset(callbackLineFunc, methodId, offset) ||
            !extractor->MatchColumnWithOffset(callbackColumnFunc, methodId, offset)) {
            LOG_DEBUGGER(ERROR) << "NotifyPaused: unknown file " << location->GetSourceFile();
            return;
        }
        hitBreakpoints.emplace_back(BreakpointDetails::ToString(detail));
    }

    // Do something cleaning on paused
    CleanUpOnPaused();

    // Notify paused event
    std::vector<std::unique_ptr<CallFrame>> callFrames;
    if (!GenerateCallFrames(&callFrames)) {
        LOG_DEBUGGER(ERROR) << "NotifyPaused: GenerateCallFrames failed";
        return;
    }
    tooling::Paused paused;
    paused.SetCallFrames(std::move(callFrames)).SetReason(reason).SetHitBreakpoints(std::move(hitBreakpoints));
    if (reason == EXCEPTION && exception->IsError()) {
        std::unique_ptr<RemoteObject> tmpException = RemoteObject::FromTagged(vm_, exception);
        paused.SetData(std::move(tmpException));
    }
    frontend_.Paused(vm_, paused);

    frontend_.WaitForDebugger(vm_);
    DebuggerApi::SetException(vm_, exception);
}

void DebuggerImpl::NotifyNativeCalling(const void *nativeAddress)
{
    // native calling only after step into should be reported
    if (singleStepper_ != nullptr &&
        singleStepper_->GetStepperType() == StepperType::STEP_INTO) {
        tooling::NativeCalling nativeCalling;
        nativeCalling.SetNativeAddress(nativeAddress);
        frontend_.NativeCalling(vm_, nativeCalling);
        frontend_.WaitForDebugger(vm_);
    }
}

void DebuggerImpl::NotifyPendingJobEntry()
{
    if (singleStepper_ != nullptr) {
        singleStepper_.reset();
        pauseOnNextByteCode_ = true;
    }
}

void DebuggerImpl::NotifyHandleProtocolCommand()
{
    auto *handler = vm_->GetJsDebuggerManager()->GetDebuggerHandler();
    handler->ProcessCommand();
}

void DebuggerImpl::DispatcherImpl::Dispatch(const DispatchRequest &request)
{
    static std::unordered_map<std::string, AgentHandler> dispatcherTable {
        { "enable", &DebuggerImpl::DispatcherImpl::Enable },
        { "disable", &DebuggerImpl::DispatcherImpl::Disable },
        { "evaluateOnCallFrame", &DebuggerImpl::DispatcherImpl::EvaluateOnCallFrame },
        { "getPossibleBreakpoints", &DebuggerImpl::DispatcherImpl::GetPossibleBreakpoints },
        { "getScriptSource", &DebuggerImpl::DispatcherImpl::GetScriptSource },
        { "pause", &DebuggerImpl::DispatcherImpl::Pause },
        { "removeBreakpoint", &DebuggerImpl::DispatcherImpl::RemoveBreakpoint },
        { "resume", &DebuggerImpl::DispatcherImpl::Resume },
        { "setAsyncCallStackDepth", &DebuggerImpl::DispatcherImpl::SetAsyncCallStackDepth },
        { "setBreakpointByUrl", &DebuggerImpl::DispatcherImpl::SetBreakpointByUrl },
        { "setPauseOnExceptions", &DebuggerImpl::DispatcherImpl::SetPauseOnExceptions },
        { "stepInto", &DebuggerImpl::DispatcherImpl::StepInto },
        { "stepOut", &DebuggerImpl::DispatcherImpl::StepOut },
        { "stepOver", &DebuggerImpl::DispatcherImpl::StepOver },
        { "setMixedDebugEnabled", &DebuggerImpl::DispatcherImpl::SetMixedDebugEnabled },
        { "replyNativeCalling", &DebuggerImpl::DispatcherImpl::ReplyNativeCalling }
    };

    const std::string &method = request.GetMethod();
    LOG_DEBUGGER(DEBUG) << "dispatch [" << method << "] to DebuggerImpl";
    auto entry = dispatcherTable.find(method);
    if (entry != dispatcherTable.end() && entry->second != nullptr) {
        (this->*(entry->second))(request);
    } else {
        SendResponse(request, DispatchResponse::Fail("Unknown method: " + method));
    }
}

void DebuggerImpl::DispatcherImpl::Enable(const DispatchRequest &request)
{
    std::unique_ptr<EnableParams> params = EnableParams::Create(request.GetParams());
    if (params == nullptr) {
        SendResponse(request, DispatchResponse::Fail("wrong params"));
        return;
    }

    UniqueDebuggerId id;
    DispatchResponse response = debugger_->Enable(*params, &id);

    EnableReturns result(id);
    SendResponse(request, response, result);
}

void DebuggerImpl::DispatcherImpl::Disable(const DispatchRequest &request)
{
    DispatchResponse response = debugger_->Disable();
    SendResponse(request, response);
}

void DebuggerImpl::DispatcherImpl::EvaluateOnCallFrame(const DispatchRequest &request)
{
    std::unique_ptr<EvaluateOnCallFrameParams> params = EvaluateOnCallFrameParams::Create(request.GetParams());
    if (params == nullptr) {
        SendResponse(request, DispatchResponse::Fail("wrong params"));
        return;
    }
    std::unique_ptr<RemoteObject> result1;
    DispatchResponse response = debugger_->EvaluateOnCallFrame(*params, &result1);
    if (result1 == nullptr) {
        SendResponse(request, response);
        return;
    }

    EvaluateOnCallFrameReturns result(std::move(result1));
    SendResponse(request, response, result);
}

void DebuggerImpl::DispatcherImpl::GetPossibleBreakpoints(const DispatchRequest &request)
{
    std::unique_ptr<GetPossibleBreakpointsParams> params = GetPossibleBreakpointsParams::Create(request.GetParams());
    if (params == nullptr) {
        SendResponse(request, DispatchResponse::Fail("wrong params"));
        return;
    }
    std::vector<std::unique_ptr<BreakLocation>> locations;
    DispatchResponse response = debugger_->GetPossibleBreakpoints(*params, &locations);
    GetPossibleBreakpointsReturns result(std::move(locations));
    SendResponse(request, response, result);
}

void DebuggerImpl::DispatcherImpl::GetScriptSource(const DispatchRequest &request)
{
    std::unique_ptr<GetScriptSourceParams> params = GetScriptSourceParams::Create(request.GetParams());
    if (params == nullptr) {
        SendResponse(request, DispatchResponse::Fail("wrong params"));
        return;
    }
    std::string source;
    DispatchResponse response = debugger_->GetScriptSource(*params, &source);
    GetScriptSourceReturns result(source);
    SendResponse(request, response, result);
}

void DebuggerImpl::DispatcherImpl::Pause(const DispatchRequest &request)
{
    DispatchResponse response = debugger_->Pause();
    SendResponse(request, response);
}

void DebuggerImpl::DispatcherImpl::RemoveBreakpoint(const DispatchRequest &request)
{
    std::unique_ptr<RemoveBreakpointParams> params = RemoveBreakpointParams::Create(request.GetParams());
    if (params == nullptr) {
        SendResponse(request, DispatchResponse::Fail("wrong params"));
        return;
    }
    DispatchResponse response = debugger_->RemoveBreakpoint(*params);
    SendResponse(request, response);
}

void DebuggerImpl::DispatcherImpl::Resume(const DispatchRequest &request)
{
    std::unique_ptr<ResumeParams> params = ResumeParams::Create(request.GetParams());
    if (params == nullptr) {
        SendResponse(request, DispatchResponse::Fail("wrong params"));
        return;
    }
    DispatchResponse response = debugger_->Resume(*params);
    SendResponse(request, response);
}

void DebuggerImpl::DispatcherImpl::SetAsyncCallStackDepth(const DispatchRequest &request)
{
    DispatchResponse response = debugger_->SetAsyncCallStackDepth();
    SendResponse(request, response);
}

void DebuggerImpl::DispatcherImpl::SetBreakpointByUrl(const DispatchRequest &request)
{
    std::unique_ptr<SetBreakpointByUrlParams> params = SetBreakpointByUrlParams::Create(request.GetParams());
    if (params == nullptr) {
        SendResponse(request, DispatchResponse::Fail("wrong params"));
        return;
    }

    std::string out_id;
    std::vector<std::unique_ptr<Location>> outLocations;
    DispatchResponse response = debugger_->SetBreakpointByUrl(*params, &out_id, &outLocations);
    SetBreakpointByUrlReturns result(out_id, std::move(outLocations));
    SendResponse(request, response, result);
}

void DebuggerImpl::DispatcherImpl::SetPauseOnExceptions(const DispatchRequest &request)
{
    std::unique_ptr<SetPauseOnExceptionsParams> params = SetPauseOnExceptionsParams::Create(request.GetParams());
    if (params == nullptr) {
        SendResponse(request, DispatchResponse::Fail("wrong params"));
        return;
    }

    DispatchResponse response = debugger_->SetPauseOnExceptions(*params);
    SendResponse(request, response);
}

void DebuggerImpl::DispatcherImpl::StepInto(const DispatchRequest &request)
{
    std::unique_ptr<StepIntoParams> params = StepIntoParams::Create(request.GetParams());
    if (params == nullptr) {
        SendResponse(request, DispatchResponse::Fail("wrong params"));
        return;
    }
    DispatchResponse response = debugger_->StepInto(*params);
    SendResponse(request, response);
}

void DebuggerImpl::DispatcherImpl::StepOut(const DispatchRequest &request)
{
    DispatchResponse response = debugger_->StepOut();
    SendResponse(request, response);
}

void DebuggerImpl::DispatcherImpl::StepOver(const DispatchRequest &request)
{
    std::unique_ptr<StepOverParams> params = StepOverParams::Create(request.GetParams());
    if (params == nullptr) {
        SendResponse(request, DispatchResponse::Fail("wrong params"));
        return;
    }
    DispatchResponse response = debugger_->StepOver(*params);
    SendResponse(request, response);
}

void DebuggerImpl::DispatcherImpl::SetMixedDebugEnabled(const DispatchRequest &request)
{
    std::unique_ptr<SetMixedDebugParams> params = SetMixedDebugParams::Create(request.GetParams());
    DispatchResponse response = debugger_->SetMixedDebugEnabled(*params);
    SendResponse(request, response);
}

void DebuggerImpl::DispatcherImpl::ReplyNativeCalling(const DispatchRequest &request)
{
    std::unique_ptr<ReplyNativeCallingParams> params = ReplyNativeCallingParams::Create(request.GetParams());
    DispatchResponse response = debugger_->ReplyNativeCalling(*params);
    SendResponse(request, response);
}

void DebuggerImpl::DispatcherImpl::SetBlackboxPatterns(const DispatchRequest &request)
{
    DispatchResponse response = debugger_->SetBlackboxPatterns();
    SendResponse(request, response);
}

bool DebuggerImpl::Frontend::AllowNotify(const EcmaVM *vm) const
{
    return vm->GetJsDebuggerManager()->IsDebugMode() && channel_ != nullptr;
}

void DebuggerImpl::Frontend::BreakpointResolved(const EcmaVM *vm)
{
    if (!AllowNotify(vm)) {
        return;
    }

    tooling::BreakpointResolved breakpointResolved;
    channel_->SendNotification(breakpointResolved);
}

void DebuggerImpl::Frontend::Paused(const EcmaVM *vm, const tooling::Paused &paused)
{
    if (!AllowNotify(vm)) {
        return;
    }

    channel_->SendNotification(paused);
}

void DebuggerImpl::Frontend::NativeCalling(const EcmaVM *vm, const tooling::NativeCalling &nativeCalling)
{
    if (!AllowNotify(vm)) {
        return;
    }

    channel_->SendNotification(nativeCalling);
}

void DebuggerImpl::Frontend::Resumed(const EcmaVM *vm)
{
    if (!AllowNotify(vm)) {
        return;
    }

    channel_->RunIfWaitingForDebugger();
    tooling::Resumed resumed;
    channel_->SendNotification(resumed);
}

void DebuggerImpl::Frontend::ScriptFailedToParse(const EcmaVM *vm)
{
    if (!AllowNotify(vm)) {
        return;
    }

    tooling::ScriptFailedToParse scriptFailedToParse;
    channel_->SendNotification(scriptFailedToParse);
}

void DebuggerImpl::Frontend::ScriptParsed(const EcmaVM *vm, const PtScript &script)
{
    if (!AllowNotify(vm)) {
        return;
    }

    tooling::ScriptParsed scriptParsed;
    scriptParsed.SetScriptId(script.GetScriptId())
        .SetUrl(script.GetUrl())
        .SetStartLine(0)
        .SetStartColumn(0)
        .SetEndLine(script.GetEndLine())
        .SetEndColumn(0)
        .SetExecutionContextId(0)
        .SetHash(script.GetHash());

    channel_->SendNotification(scriptParsed);
}

void DebuggerImpl::Frontend::WaitForDebugger(const EcmaVM *vm)
{
    if (!AllowNotify(vm)) {
        return;
    }

    channel_->WaitForDebugger();
}

void DebuggerImpl::Frontend::RunIfWaitingForDebugger(const EcmaVM *vm)
{
    if (!AllowNotify(vm)) {
        return;
    }

    channel_->RunIfWaitingForDebugger();
}

DispatchResponse DebuggerImpl::Enable([[maybe_unused]] const EnableParams &params, UniqueDebuggerId *id)
{
    ASSERT(id != nullptr);
    *id = 0;
    vm_->GetJsDebuggerManager()->SetDebugMode(true);
    for (auto &script : scripts_) {
        frontend_.ScriptParsed(vm_, *script.second);
    }
    return DispatchResponse::Ok();
}

DispatchResponse DebuggerImpl::Disable()
{
    DebuggerApi::RemoveAllBreakpoints(jsDebugger_);
    frontend_.RunIfWaitingForDebugger(vm_);
    vm_->GetJsDebuggerManager()->SetDebugMode(false);
    return DispatchResponse::Ok();
}

DispatchResponse DebuggerImpl::EvaluateOnCallFrame(const EvaluateOnCallFrameParams &params,
                                                   std::unique_ptr<RemoteObject> *result)
{
    CallFrameId callFrameId = params.GetCallFrameId();
    const std::string &expression = params.GetExpression();
    if (callFrameId < 0 || callFrameId >= static_cast<CallFrameId>(callFrameHandlers_.size())) {
        return DispatchResponse::Fail("Invalid callFrameId.");
    }

    std::string dest;
    if (!DecodeAndCheckBase64(expression, dest)) {
        LOG_DEBUGGER(ERROR) << "EvaluateValue: base64 decode failed";
        auto ret = CmptEvaluateValue(callFrameId, expression, result);
        if (ret.has_value()) {
            LOG_DEBUGGER(ERROR) << "Evaluate fail, expression: " << expression;
        }
        return DispatchResponse::Create(ret);
    }

    auto funcRef = DebuggerApi::GenerateFuncFromBuffer(vm_, dest.data(), dest.size(),
        JSPandaFile::ENTRY_FUNCTION_NAME);
    auto res = DebuggerApi::EvaluateViaFuncCall(const_cast<EcmaVM *>(vm_), funcRef,
        callFrameHandlers_[callFrameId]);
    if (vm_->GetJSThread()->HasPendingException()) {
        LOG_DEBUGGER(ERROR) << "EvaluateValue: has pending exception";
        std::string msg;
        DebuggerApi::HandleUncaughtException(vm_, msg);
        *result = RemoteObject::FromTagged(vm_,
            Exception::EvalError(vm_, StringRef::NewFromUtf8(vm_, msg.data())));
        return DispatchResponse::Fail(msg);
    }

    *result = RemoteObject::FromTagged(vm_, res);
    runtime_->CacheObjectIfNeeded(res, (*result).get());
    return DispatchResponse::Ok();
}

DispatchResponse DebuggerImpl::GetPossibleBreakpoints(const GetPossibleBreakpointsParams &params,
                                                      std::vector<std::unique_ptr<BreakLocation>> *locations)
{
    Location *start = params.GetStart();
    auto iter = scripts_.find(start->GetScriptId());
    if (iter == scripts_.end()) {
        return DispatchResponse::Fail("Unknown file name.");
    }
    const std::string &url = iter->second->GetUrl();
    DebugInfoExtractor *extractor = GetExtractor(url);
    if (extractor == nullptr) {
        LOG_DEBUGGER(ERROR) << "GetPossibleBreakpoints: extractor is null";
        return DispatchResponse::Fail("Unknown file name.");
    }

    int32_t line = start->GetLine();
    int32_t column = start->GetColumn();
    auto callbackFunc = [](const JSPtLocation &) -> bool {
        return true;
    };
    if (extractor->MatchWithLocation(callbackFunc, line, column, url)) {
        std::unique_ptr<BreakLocation> location = std::make_unique<BreakLocation>();
        location->SetScriptId(start->GetScriptId()).SetLine(line).SetColumn(column);
        locations->emplace_back(std::move(location));
    }
    return DispatchResponse::Ok();
}

DispatchResponse DebuggerImpl::GetScriptSource(const GetScriptSourceParams &params, std::string *source)
{
    ScriptId scriptId = params.GetScriptId();
    auto iter = scripts_.find(scriptId);
    if (iter == scripts_.end()) {
        *source = "";
        return DispatchResponse::Fail("unknown script id: " + std::to_string(scriptId));
    }
    *source = iter->second->GetScriptSource();

    return DispatchResponse::Ok();
}

DispatchResponse DebuggerImpl::Pause()
{
    pauseOnNextByteCode_ = true;
    return DispatchResponse::Ok();
}

DispatchResponse DebuggerImpl::RemoveBreakpoint(const RemoveBreakpointParams &params)
{
    std::string id = params.GetBreakpointId();
    LOG_DEBUGGER(INFO) << "RemoveBreakpoint: " << id;
    BreakpointDetails metaData{};
    if (!BreakpointDetails::ParseBreakpointId(id, &metaData)) {
        return DispatchResponse::Fail("Parse breakpoint id failed");
    }
    DebugInfoExtractor *extractor = GetExtractor(metaData.url_);
    if (extractor == nullptr) {
        LOG_DEBUGGER(ERROR) << "RemoveBreakpoint: extractor is null";
        return DispatchResponse::Fail("Unknown file name.");
    }

    auto scriptFunc = [](PtScript *) -> bool {
        return true;
    };
    if (!MatchScripts(scriptFunc, metaData.url_, ScriptMatchType::URL)) {
        LOG_DEBUGGER(ERROR) << "RemoveBreakpoint: Unknown url: " << metaData.url_;
        return DispatchResponse::Fail("Unknown file name.");
    }

    auto callbackFunc = [this](const JSPtLocation &location) -> bool {
        LOG_DEBUGGER(INFO) << "remove breakpoint location: " << location.ToString();
        return DebuggerApi::RemoveBreakpoint(jsDebugger_, location);
    };
    if (!extractor->MatchWithLocation(callbackFunc, metaData.line_, metaData.column_, metaData.url_)) {
        LOG_DEBUGGER(ERROR) << "failed to remove breakpoint location number: "
            << metaData.line_ << ":" << metaData.column_;
        return DispatchResponse::Fail("Breakpoint not found.");
    }

    LOG_DEBUGGER(INFO) << "remove breakpoint line number:" << metaData.line_;
    return DispatchResponse::Ok();
}

DispatchResponse DebuggerImpl::Resume([[maybe_unused]] const ResumeParams &params)
{
    frontend_.Resumed(vm_);
    singleStepper_.reset();
    return DispatchResponse::Ok();
}

DispatchResponse DebuggerImpl::SetAsyncCallStackDepth()
{
    return DispatchResponse::Fail("SetAsyncCallStackDepth not support now");
}

DispatchResponse DebuggerImpl::SetBreakpointByUrl(const SetBreakpointByUrlParams &params,
                                                  std::string *outId,
                                                  std::vector<std::unique_ptr<Location>> *outLocations)
{
    if (!vm_->GetJsDebuggerManager()->IsDebugMode()) {
        return DispatchResponse::Fail("SetBreakpointByUrl: debugger agent is not enabled");
    }
    const std::string &url = params.GetUrl();
    int32_t lineNumber = params.GetLine();
    int32_t columnNumber = params.GetColumn();
    auto condition = params.HasCondition() ? params.GetCondition() : std::optional<std::string> {};

    DebugInfoExtractor *extractor = GetExtractor(url);
    if (extractor == nullptr) {
        LOG_DEBUGGER(ERROR) << "SetBreakpointByUrl: extractor is null";
        return DispatchResponse::Fail("Unknown file name.");
    }

    ScriptId scriptId;
    std::string fileName;
    auto scriptFunc = [&scriptId, &fileName](PtScript *script) -> bool {
        scriptId = script->GetScriptId();
        fileName = script->GetFileName();
        return true;
    };
    if (!MatchScripts(scriptFunc, url, ScriptMatchType::URL)) {
        LOG_DEBUGGER(ERROR) << "SetBreakpointByUrl: Unknown url: " << url;
        return DispatchResponse::Fail("Unknown file name.");
    }

    auto callbackFunc = [this, &condition](const JSPtLocation &location) -> bool {
        LOG_DEBUGGER(INFO) << "set breakpoint location: " << location.ToString();
        Local<FunctionRef> condFuncRef = FunctionRef::Undefined(vm_);
        if (condition.has_value() && !condition.value().empty()) {
            std::string dest;
            if (!DecodeAndCheckBase64(condition.value(), dest)) {
                LOG_DEBUGGER(ERROR) << "SetBreakpointByUrl: base64 decode failed";
                return false;
            }
            condFuncRef = DebuggerApi::GenerateFuncFromBuffer(vm_, dest.data(), dest.size(),
                JSPandaFile::ENTRY_FUNCTION_NAME);
            if (condFuncRef->IsUndefined()) {
                LOG_DEBUGGER(ERROR) << "SetBreakpointByUrl: generate function failed";
                return false;
            }
        }
        return DebuggerApi::SetBreakpoint(jsDebugger_, location, condFuncRef);
    };
    if (!extractor->MatchWithLocation(callbackFunc, lineNumber, columnNumber, url)) {
        LOG_DEBUGGER(ERROR) << "failed to set breakpoint location number: " << lineNumber << ":" << columnNumber;
        return DispatchResponse::Fail("Breakpoint not found.");
    }

    BreakpointDetails metaData{lineNumber, 0, url};
    *outId = BreakpointDetails::ToString(metaData);
    *outLocations = std::vector<std::unique_ptr<Location>>();
    std::unique_ptr<Location> location = std::make_unique<Location>();
    location->SetScriptId(scriptId).SetLine(lineNumber).SetColumn(0);
    outLocations->emplace_back(std::move(location));

    return DispatchResponse::Ok();
}

DispatchResponse DebuggerImpl::SetPauseOnExceptions(const SetPauseOnExceptionsParams &params)
{
    pauseOnException_ = params.GetState();
    return DispatchResponse::Ok();
}

DispatchResponse DebuggerImpl::StepInto([[maybe_unused]] const StepIntoParams &params)
{
    if (!vm_->GetJsDebuggerManager()->IsDebugMode()) {
        return DispatchResponse::Fail("Can only perform operation while paused");
    }
    singleStepper_ = SingleStepper::GetStepIntoStepper(vm_);
    if (singleStepper_ == nullptr) {
        LOG_DEBUGGER(ERROR) << "StepInto: singleStepper is null";
        return DispatchResponse::Fail("Failed to StepInto");
    }
    frontend_.Resumed(vm_);
    return DispatchResponse::Ok();
}

DispatchResponse DebuggerImpl::StepOut()
{
    if (!vm_->GetJsDebuggerManager()->IsDebugMode()) {
        return DispatchResponse::Fail("Can only perform operation while paused");
    }
    singleStepper_ = SingleStepper::GetStepOutStepper(vm_);
    if (singleStepper_ == nullptr) {
        LOG_DEBUGGER(ERROR) << "StepOut: singleStepper is null";
        return DispatchResponse::Fail("Failed to StepOut");
    }
    frontend_.Resumed(vm_);
    return DispatchResponse::Ok();
}

DispatchResponse DebuggerImpl::StepOver([[maybe_unused]] const StepOverParams &params)
{
    if (!vm_->GetJsDebuggerManager()->IsDebugMode()) {
        return DispatchResponse::Fail("Can only perform operation while paused");
    }
    singleStepper_ = SingleStepper::GetStepOverStepper(vm_);
    if (singleStepper_ == nullptr) {
        LOG_DEBUGGER(ERROR) << "StepOver: singleStepper is null";
        return DispatchResponse::Fail("Failed to StepOver");
    }
    frontend_.Resumed(vm_);
    return DispatchResponse::Ok();
}

DispatchResponse DebuggerImpl::SetBlackboxPatterns()
{
    return DispatchResponse::Fail("SetBlackboxPatterns not support now");
}

DispatchResponse DebuggerImpl::SetMixedDebugEnabled([[maybe_unused]] const SetMixedDebugParams &params)
{
    vm_->GetJsDebuggerManager()->SetMixedDebugEnabled(params.GetEnabled());
    return DispatchResponse::Ok();
}

DispatchResponse DebuggerImpl::ReplyNativeCalling([[maybe_unused]] const ReplyNativeCallingParams &params)
{
    frontend_.Resumed(vm_);
    if (params.GetUserCode()) {
        singleStepper_.reset();
    }
    return DispatchResponse::Ok();
}

void DebuggerImpl::CleanUpOnPaused()
{
    runtime_->curObjectId_ = 0;
    runtime_->properties_.clear();

    callFrameHandlers_.clear();
    scopeObjects_.clear();
}

std::string DebuggerImpl::Trim(const std::string &str)
{
    std::string ret = str;
    // If ret has only ' ', remove all charactors.
    ret.erase(ret.find_last_not_of(' ') + 1);
    // If ret has only ' ', remove all charactors.
    ret.erase(0, ret.find_first_not_of(' '));
    return ret;
}

DebugInfoExtractor *DebuggerImpl::GetExtractor(const JSPandaFile *jsPandaFile)
{
    return JSPandaFileManager::GetInstance()->GetJSPtExtractor(jsPandaFile);
}

// mainly used for breakpoints to match location
DebugInfoExtractor *DebuggerImpl::GetExtractor(const std::string &url)
{
    // match patch file first if it contains diff for the url, and currently only support the file
    // specified by the url change as a whole
    auto *extractor = DebuggerApi::GetPatchExtractor(vm_, url);
    if (extractor != nullptr) {
        return extractor;
    }

    auto iter = extractors_.find(url);
    if (iter == extractors_.end()) {
        return nullptr;
    }

    return iter->second;
}

bool DebuggerImpl::GenerateCallFrames(std::vector<std::unique_ptr<CallFrame>> *callFrames)
{
    CallFrameId callFrameId = 0;
    auto walkerFunc = [this, &callFrameId, &callFrames](const FrameHandler *frameHandler) -> StackState {
        if (DebuggerApi::IsNativeMethod(frameHandler)) {
            LOG_DEBUGGER(INFO) << "GenerateCallFrames: Skip CFrame and Native method";
            return StackState::CONTINUE;
        }
        std::unique_ptr<CallFrame> callFrame = std::make_unique<CallFrame>();
        if (!GenerateCallFrame(callFrame.get(), frameHandler, callFrameId)) {
            if (callFrameId == 0) {
                return StackState::FAILED;
            }
        } else {
            SaveCallFrameHandler(frameHandler);
            callFrames->emplace_back(std::move(callFrame));
            callFrameId++;
        }
        return StackState::CONTINUE;
    };
    return DebuggerApi::StackWalker(vm_, walkerFunc);
}

void DebuggerImpl::SaveCallFrameHandler(const FrameHandler *frameHandler)
{
    auto handlerPtr = DebuggerApi::NewFrameHandler(vm_);
    *handlerPtr = *frameHandler;
    callFrameHandlers_.emplace_back(handlerPtr);
}

bool DebuggerImpl::GenerateCallFrame(CallFrame *callFrame,
    const FrameHandler *frameHandler, CallFrameId callFrameId)
{
    if (!frameHandler->HasFrame()) {
        return false;
    }
    Method *method = DebuggerApi::GetMethod(frameHandler);
    auto methodId = method->GetMethodId();
    const JSPandaFile *jsPandaFile = method->GetJSPandaFile();
    DebugInfoExtractor *extractor = GetExtractor(jsPandaFile);
    if (extractor == nullptr) {
        LOG_DEBUGGER(ERROR) << "GenerateCallFrame: extractor is null";
        return false;
    }

    // functionName
    std::string functionName = method->ParseFunctionName();

    // location
    std::unique_ptr<Location> location = std::make_unique<Location>();
    std::string url = extractor->GetSourceFile(methodId);
    auto scriptFunc = [&location](PtScript *script) -> bool {
        location->SetScriptId(script->GetScriptId());
        return true;
    };
    if (!MatchScripts(scriptFunc, url, ScriptMatchType::URL)) {
        LOG_DEBUGGER(ERROR) << "GenerateCallFrame: Unknown url: " << url;
        return false;
    }
    auto callbackLineFunc = [&location](int32_t line) -> bool {
        location->SetLine(line);
        return true;
    };
    auto callbackColumnFunc = [&location](int32_t column) -> bool {
        location->SetColumn(column);
        return true;
    };
    if (!extractor->MatchLineWithOffset(callbackLineFunc, methodId, DebuggerApi::GetBytecodeOffset(frameHandler)) ||
        !extractor->MatchColumnWithOffset(callbackColumnFunc, methodId, DebuggerApi::GetBytecodeOffset(frameHandler))) {
        LOG_DEBUGGER(ERROR) << "GenerateCallFrame: unknown offset: " << DebuggerApi::GetBytecodeOffset(frameHandler);
        return false;
    }

    // scopeChain & this
    std::unique_ptr<RemoteObject> thisObj = std::make_unique<RemoteObject>();
    thisObj->SetType(ObjectType::Undefined);

    JSThread *thread = vm_->GetJSThread();
    std::vector<std::unique_ptr<Scope>> scopeChain;
    scopeChain.emplace_back(GetLocalScopeChain(frameHandler, &thisObj));
    if (jsPandaFile != nullptr && !jsPandaFile->IsBundlePack() && jsPandaFile->IsNewVersion()) {
        JSHandle<JSTaggedValue> currentModule(thread, DebuggerApi::GetCurrentModule(vm_));
        if (currentModule->IsSourceTextModule()) { // CJS module is string
            scopeChain.emplace_back(GetModuleScopeChain());
        }
    }
    scopeChain.emplace_back(GetGlobalScopeChain());

    callFrame->SetCallFrameId(callFrameId)
        .SetFunctionName(functionName)
        .SetLocation(std::move(location))
        .SetUrl(url)
        .SetScopeChain(std::move(scopeChain))
        .SetThis(std::move(thisObj));
    return true;
}

std::unique_ptr<Scope> DebuggerImpl::GetLocalScopeChain(const FrameHandler *frameHandler,
    std::unique_ptr<RemoteObject> *thisObj)
{
    auto localScope = std::make_unique<Scope>();

    Method *method = DebuggerApi::GetMethod(frameHandler);
    auto methodId = method->GetMethodId();
    const JSPandaFile *jsPandaFile = method->GetJSPandaFile();
    DebugInfoExtractor *extractor = GetExtractor(jsPandaFile);
    if (extractor == nullptr) {
        LOG_DEBUGGER(ERROR) << "GetScopeChain: extractor is null";
        return localScope;
    }

    std::unique_ptr<RemoteObject> local = std::make_unique<RemoteObject>();
    Local<ObjectRef> localObj = ObjectRef::New(vm_);
    local->SetType(ObjectType::Object)
        .SetObjectId(runtime_->curObjectId_)
        .SetClassName(ObjectClassName::Object)
        .SetDescription(RemoteObject::ObjectDescription);
    auto *sp = DebuggerApi::GetSp(frameHandler);
    scopeObjects_[sp] = runtime_->curObjectId_;
    runtime_->properties_[runtime_->curObjectId_++] = Global<JSValueRef>(vm_, localObj);

    Local<JSValueRef> thisVal = JSValueRef::Undefined(vm_);
    GetLocalVariables(frameHandler, methodId, jsPandaFile, thisVal, localObj);
    *thisObj = RemoteObject::FromTagged(vm_, thisVal);
    runtime_->CacheObjectIfNeeded(thisVal, (*thisObj).get());

    const LineNumberTable &lines = extractor->GetLineNumberTable(methodId);
    std::unique_ptr<Location> startLoc = std::make_unique<Location>();
    std::unique_ptr<Location> endLoc = std::make_unique<Location>();
    auto scriptFunc = [&startLoc, &endLoc, lines](PtScript *script) -> bool {
        startLoc->SetScriptId(script->GetScriptId())
            .SetLine(lines.front().line)
            .SetColumn(0);
        endLoc->SetScriptId(script->GetScriptId())
            .SetLine(lines.back().line + 1)
            .SetColumn(0);
        return true;
    };
    if (MatchScripts(scriptFunc, extractor->GetSourceFile(methodId), ScriptMatchType::URL)) {
        localScope->SetType(Scope::Type::Local())
            .SetObject(std::move(local))
            .SetStartLocation(std::move(startLoc))
            .SetEndLocation(std::move(endLoc));
    }

    return localScope;
}

std::unique_ptr<Scope> DebuggerImpl::GetModuleScopeChain()
{
    auto moduleScope = std::make_unique<Scope>();

    std::unique_ptr<RemoteObject> module = std::make_unique<RemoteObject>();
    Local<ObjectRef> moduleObj = ObjectRef::New(vm_);
    module->SetType(ObjectType::Object)
        .SetObjectId(runtime_->curObjectId_)
        .SetClassName(ObjectClassName::Object)
        .SetDescription(RemoteObject::ObjectDescription);
    moduleScope->SetType(Scope::Type::Module()).SetObject(std::move(module));
    runtime_->properties_[runtime_->curObjectId_++] = Global<JSValueRef>(vm_, moduleObj);
    JSThread *thread = vm_->GetJSThread();
    JSHandle<JSTaggedValue> currentModule(thread, DebuggerApi::GetCurrentModule(vm_));
    DebuggerApi::GetLocalExportVariables(vm_, moduleObj, currentModule, false);
    DebuggerApi::GetIndirectExportVariables(vm_, moduleObj, currentModule);
    DebuggerApi::GetImportVariables(vm_, moduleObj, currentModule);
    return moduleScope;
}

void DebuggerImpl::GetLocalVariables(const FrameHandler *frameHandler, panda_file::File::EntityId methodId,
    const JSPandaFile *jsPandaFile, Local<JSValueRef> &thisVal, Local<ObjectRef> &localObj)
{
    auto *extractor = GetExtractor(jsPandaFile);
    Local<JSValueRef> value = JSValueRef::Undefined(vm_);
    // in case of arrow function, which doesn't have this in local variable table
    bool hasThis = false;
    for (const auto &[varName, regIndex] : extractor->GetLocalVariableTable(methodId)) {
        value = DebuggerApi::GetVRegValue(vm_, frameHandler, regIndex);
        if (varName == "4newTarget" || varName == "0this" || varName == "0newTarget" || varName == "0funcObj") {
            continue;
        }

        if (varName == "this") {
            thisVal = value;
            hasThis = true;
            continue;
        }
        Local<JSValueRef> name = JSValueRef::Undefined(vm_);
        if (varName == "4funcObj") {
            if (value->IsFunction()) {
                auto funcName = Local<FunctionRef>(value)->GetName(vm_)->ToString();
                name = StringRef::NewFromUtf8(vm_, funcName.c_str());
            } else {
                continue;
            }
        } else {
            name = StringRef::NewFromUtf8(vm_, varName.c_str());
        }
        PropertyAttribute descriptor(value, true, true, true);
        localObj->DefineProperty(vm_, name, descriptor);
    }

    // closure variables are stored in env
    JSTaggedValue env = DebuggerApi::GetEnv(frameHandler);
    if (env.IsTaggedArray() && DebuggerApi::GetBytecodeOffset(frameHandler) != 0) {
        LexicalEnv *lexEnv = LexicalEnv::Cast(env.GetTaggedObject());
        if (lexEnv->GetScopeInfo().IsHole()) {
            return;
        }
        auto ptr = JSNativePointer::Cast(lexEnv->GetScopeInfo().GetTaggedObject())->GetExternalPointer();
        auto *scopeDebugInfo = reinterpret_cast<ScopeDebugInfo *>(ptr);
        JSThread *thread = vm_->GetJSThread();
        for (const auto &[varName, slot] : scopeDebugInfo->scopeInfo) {
            // skip possible duplicate variables both in local variable table and env
            if (varName == "4newTarget") {
                continue;
            }
            value = JSNApiHelper::ToLocal<JSValueRef>(
                JSHandle<JSTaggedValue>(thread, lexEnv->GetProperties(slot)));
            if (varName == "this") {
                if (!hasThis) {
                    thisVal = value;
                }
                continue;
            }
            Local<JSValueRef> name = StringRef::NewFromUtf8(vm_, varName.c_str());
            PropertyAttribute descriptor(value, true, true, true);
            localObj->DefineProperty(vm_, name, descriptor);
        }
    }
}

std::unique_ptr<Scope> DebuggerImpl::GetGlobalScopeChain()
{
    auto globalScope = std::make_unique<Scope>();

    std::unique_ptr<RemoteObject> global = std::make_unique<RemoteObject>();
    global->SetType(ObjectType::Object)
        .SetObjectId(runtime_->curObjectId_)
        .SetClassName(ObjectClassName::Global)
        .SetDescription(RemoteObject::GlobalDescription);
    globalScope->SetType(Scope::Type::Global()).SetObject(std::move(global));
    runtime_->properties_[runtime_->curObjectId_++] = Global<JSValueRef>(vm_, JSNApi::GetGlobalObject(vm_));
    return globalScope;
}

void DebuggerImpl::UpdateScopeObject(const FrameHandler *frameHandler,
    std::string_view varName, Local<JSValueRef> newVal)
{
    auto *sp = DebuggerApi::GetSp(frameHandler);
    auto iter = scopeObjects_.find(sp);
    if (iter == scopeObjects_.end()) {
        LOG_DEBUGGER(ERROR) << "UpdateScopeObject: object not found";
        return;
    }

    auto objectId = iter->second;
    Local<ObjectRef> localObj = runtime_->properties_[objectId].ToLocal(vm_);
    Local<JSValueRef> name = StringRef::NewFromUtf8(vm_, varName.data());
    if (localObj->Has(vm_, name)) {
        LOG_DEBUGGER(DEBUG) << "UpdateScopeObject: set new value";
        PropertyAttribute descriptor(newVal, true, true, true);
        localObj->DefineProperty(vm_, name, descriptor);
    } else {
        LOG_DEBUGGER(ERROR) << "UpdateScopeObject: not found " << varName;
    }
}

void DebuggerImpl::ClearSingleStepper()
{
    // ClearSingleStepper is originally called from Function::Call, if current depth is 0, then it is safe to reset
    if (singleStepper_ != nullptr && DebuggerApi::GetStackDepth(vm_) == 0) {
        singleStepper_.reset();
    }
}

std::optional<std::string> DebuggerImpl::CmptEvaluateValue(CallFrameId callFrameId, const std::string &expression,
    std::unique_ptr<RemoteObject> *result)
{
    if (DebuggerApi::IsNativeMethod(vm_)) {
        *result = RemoteObject::FromTagged(vm_,
            Exception::EvalError(vm_, StringRef::NewFromUtf8(vm_, "Native Frame not support.")));
        return "Native Frame not support.";
    }
    DebugInfoExtractor *extractor = GetExtractor(DebuggerApi::GetJSPandaFile(vm_));
    if (extractor == nullptr) {
        *result = RemoteObject::FromTagged(vm_,
            Exception::EvalError(vm_, StringRef::NewFromUtf8(vm_, "Internal error.")));
        return "Internal error.";
    }
    std::string varName = expression;
    std::string varValue;
    std::string::size_type indexEqual = expression.find_first_of('=', 0);
    if (indexEqual != std::string::npos) {
        varName = Trim(expression.substr(0, indexEqual));
        varValue = Trim(expression.substr(indexEqual + 1, expression.length()));
    }

    Local<StringRef> name = StringRef::NewFromUtf8(vm_, varName.c_str());
    FrameHandler *frameHandler = callFrameHandlers_[callFrameId].get();
    if (varValue.empty()) {
        Local<JSValueRef> ret = DebuggerExecutor::GetValue(vm_, frameHandler, name);
        if (!ret.IsEmpty()) {
            *result = RemoteObject::FromTagged(vm_, ret);
            runtime_->CacheObjectIfNeeded(ret, (*result).get());
            return {};
        }
    } else {
        Local<JSValueRef> value = ConvertToLocal(varValue);
        if (value.IsEmpty()) {
            return "Unsupported expression.";
        }
        JsDebuggerManager *mgr = vm_->GetJsDebuggerManager();
        mgr->SetEvalFrameHandler(callFrameHandlers_[callFrameId]);
        bool ret = DebuggerExecutor::SetValue(vm_, frameHandler, name, value);
        mgr->SetEvalFrameHandler(nullptr);
        if (ret) {
            *result = RemoteObject::FromTagged(vm_, value);
            return {};
        }
    }

    *result = RemoteObject::FromTagged(vm_,
        Exception::EvalError(vm_, StringRef::NewFromUtf8(vm_, "Unsupported expression.")));
    return "Unsupported expression.";
}

Local<JSValueRef> DebuggerImpl::ConvertToLocal(const std::string &varValue)
{
    Local<JSValueRef> taggedValue;
    if (varValue == "false") {
        taggedValue = JSValueRef::False(vm_);
    } else if (varValue == "true") {
        taggedValue = JSValueRef::True(vm_);
    } else if (varValue == "undefined") {
        taggedValue = JSValueRef::Undefined(vm_);
    } else if (varValue[0] == '\"' && varValue[varValue.length() - 1] == '\"') {
        // 2 : 2 means length
        taggedValue = StringRef::NewFromUtf8(vm_, varValue.substr(1, varValue.length() - 2).c_str());
    } else {
        auto begin = reinterpret_cast<const uint8_t *>((varValue.c_str()));
        auto end = begin + varValue.length();  // NOLINT(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        double d = DebuggerApi::StringToDouble(begin, end, 0);
        if (!std::isnan(d)) {
            taggedValue = NumberRef::New(vm_, d);
        }
    }
    return taggedValue;
}

bool DebuggerImpl::DecodeAndCheckBase64(const std::string &src, std::string &dest)
{
    uint32_t numOctets = PtBase64::Decode(src, dest);
    if (numOctets > File::MAGIC_SIZE &&
        memcmp(dest.data(), File::MAGIC.data(), File::MAGIC_SIZE) == 0) {
        return true;
    }
    return false;
}
}  // namespace panda::ecmascript::tooling
