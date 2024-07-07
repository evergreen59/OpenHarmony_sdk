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

#ifndef ECMASCRIPT_TOOLING_AGENT_RUNTIME_IMPL_H
#define ECMASCRIPT_TOOLING_AGENT_RUNTIME_IMPL_H


#include "base/pt_params.h"
#include "dispatcher.h"

#include "libpandabase/macros.h"

namespace panda::ecmascript::tooling {
enum class ArkInternalValueType {None, Entry, Scope, ScopeList};
class RuntimeImpl final {
public:
    RuntimeImpl(const EcmaVM *vm, ProtocolChannel *channel)
        : vm_(vm), frontend_(channel) {}
    ~RuntimeImpl() = default;

    DispatchResponse Enable();
    DispatchResponse Disable();
    DispatchResponse RunIfWaitingForDebugger();
    DispatchResponse CallFunctionOn(
        const CallFunctionOnParams &params,
        std::unique_ptr<RemoteObject> *outRemoteObject,
        std::optional<std::unique_ptr<ExceptionDetails>> *outExceptionDetails);
    DispatchResponse GetHeapUsage(double *usedSize, double *totalSize);
    DispatchResponse GetProperties(
        const GetPropertiesParams &params,
        std::vector<std::unique_ptr<PropertyDescriptor>> *outPropertyDesc,
        std::optional<std::vector<std::unique_ptr<InternalPropertyDescriptor>>> *outInternalDescs,
        std::optional<std::vector<std::unique_ptr<PrivatePropertyDescriptor>>> *outPrivateProps,
        std::optional<std::unique_ptr<ExceptionDetails>> *outExceptionDetails);

    class DispatcherImpl final : public DispatcherBase {
    public:
        DispatcherImpl(ProtocolChannel *channel, std::unique_ptr<RuntimeImpl> runtime)
            : DispatcherBase(channel), runtime_(std::move(runtime)) {}
        ~DispatcherImpl() override = default;

        void Dispatch(const DispatchRequest &request) override;
        void Disable(const DispatchRequest &request);
        void Enable(const DispatchRequest &request);
        void RunIfWaitingForDebugger(const DispatchRequest &request);
        void GetProperties(const DispatchRequest &request);
        void CallFunctionOn(const DispatchRequest &request);
        void GetHeapUsage(const DispatchRequest &request);

    private:
        using AgentHandler = void (RuntimeImpl::DispatcherImpl::*)(const DispatchRequest &request);
        std::unique_ptr<RuntimeImpl> runtime_ {};

        NO_COPY_SEMANTIC(DispatcherImpl);
        NO_MOVE_SEMANTIC(DispatcherImpl);
    };

private:
    NO_COPY_SEMANTIC(RuntimeImpl);
    NO_MOVE_SEMANTIC(RuntimeImpl);
    enum NumberSize : uint8_t { BYTES_OF_16BITS = 2, BYTES_OF_32BITS = 4, BYTES_OF_64BITS = 8 };

    void CacheObjectIfNeeded(Local<JSValueRef> valRef, RemoteObject *remoteObj);

    template <typename TypedArrayRef>
    void AddTypedArrayRef(Local<ArrayBufferRef> arrayBufferRef, int32_t length,
        const char* name, std::vector<std::unique_ptr<PropertyDescriptor>> *outPropertyDesc);
    void AddTypedArrayRefs(Local<ArrayBufferRef> arrayBufferRef,
        std::vector<std::unique_ptr<PropertyDescriptor>> *outPropertyDesc);
    void AddSharedArrayBufferRefs(Local<ArrayBufferRef> arrayBufferRef,
        std::vector<std::unique_ptr<PropertyDescriptor>> *outPropertyDesc);
    void GetProtoOrProtoType(Local<JSValueRef> value, bool isOwn, bool isAccessorOnly,
                             std::vector<std::unique_ptr<PropertyDescriptor>> *outPropertyDesc);
    void GetAdditionalProperties(Local<JSValueRef> value,
        std::vector<std::unique_ptr<PropertyDescriptor>> *outPropertyDesc);
    void SetKeyValue(Local<JSValueRef> &jsValueRef,
        std::vector<std::unique_ptr<PropertyDescriptor>> *outPropertyDesc, const std::string &cstrProName);
    void GetPrimitiveNumberValue(Local<JSValueRef> value,
        std::vector<std::unique_ptr<PropertyDescriptor>> *outPropertyDesc);
    void GetPrimitiveStringValue(Local<JSValueRef> value,
        std::vector<std::unique_ptr<PropertyDescriptor>> *outPropertyDesc);
    void GetPrimitiveBooleanValue(Local<JSValueRef> value,
        std::vector<std::unique_ptr<PropertyDescriptor>> *outPropertyDesc);
    void GetMapIteratorValue(Local<JSValueRef> value,
        std::vector<std::unique_ptr<PropertyDescriptor>> *outPropertyDesc);
    void GetSetIteratorValue(Local<JSValueRef> value,
        std::vector<std::unique_ptr<PropertyDescriptor>> *outPropertyDesc);
    void GetGeneratorFunctionValue(Local<JSValueRef> value,
        std::vector<std::unique_ptr<PropertyDescriptor>> *outPropertyDesc);
    void GetGeneratorObjectValue(Local<JSValueRef> value,
        std::vector<std::unique_ptr<PropertyDescriptor>> *outPropertyDesc);
    void GetNumberFormatValue(Local<JSValueRef> value,
        std::vector<std::unique_ptr<PropertyDescriptor>> *outPropertyDesc);
    void GetCollatorValue(Local<JSValueRef> value,
        std::vector<std::unique_ptr<PropertyDescriptor>> *outPropertyDesc);
    void GetDateTimeFormatValue(Local<JSValueRef> value,
        std::vector<std::unique_ptr<PropertyDescriptor>> *outPropertyDesc);
    void AddInternalProperties(Local<ObjectRef> object, ArkInternalValueType type);
    void GetMapValue(Local<JSValueRef> value,
        std::vector<std::unique_ptr<PropertyDescriptor>> *outPropertyDesc);
    void GetSetValue(Local<JSValueRef> value,
        std::vector<std::unique_ptr<PropertyDescriptor>> *outPropertyDesc);
    void GetRegExpValue(Local<JSValueRef> value,
        std::vector<std::unique_ptr<PropertyDescriptor>> *outPropertyDesc);
    class Frontend {
    public:
        explicit Frontend(ProtocolChannel *channel) : channel_(channel) {}
        ~Frontend() = default;

        void RunIfWaitingForDebugger();

    private:
        bool AllowNotify() const;

        ProtocolChannel *channel_ {nullptr};
    };

    const EcmaVM *vm_ {nullptr};
    Frontend frontend_;

    RemoteObjectId curObjectId_ {0};
    std::unordered_map<RemoteObjectId, Global<JSValueRef>> properties_ {};
    Global<MapRef> internalObjects_;

    friend class DebuggerImpl;
};
}  // namespace panda::ecmascript::tooling
#endif