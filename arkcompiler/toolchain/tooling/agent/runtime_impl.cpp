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

#include "agent/runtime_impl.h"

#include <iomanip>

#include "base/pt_returns.h"
#include "protocol_channel.h"

#include "ecmascript/napi/include/dfx_jsnapi.h"

namespace panda::ecmascript::tooling {
void RuntimeImpl::DispatcherImpl::Dispatch(const DispatchRequest &request)
{
    static std::unordered_map<std::string, AgentHandler> dispatcherTable {
        { "enable", &RuntimeImpl::DispatcherImpl::Enable },
        { "getProperties", &RuntimeImpl::DispatcherImpl::GetProperties },
        { "runIfWaitingForDebugger", &RuntimeImpl::DispatcherImpl::RunIfWaitingForDebugger },
        { "callFunctionOn", &RuntimeImpl::DispatcherImpl::CallFunctionOn },
        { "getHeapUsage", &RuntimeImpl::DispatcherImpl::GetHeapUsage }
    };

    const std::string &method = request.GetMethod();
    LOG_DEBUGGER(DEBUG) << "dispatch [" << method << "] to RuntimeImpl";

    auto entry = dispatcherTable.find(method);
    if (entry != dispatcherTable.end()) {
        (this->*(entry->second))(request);
    } else {
        LOG_DEBUGGER(ERROR) << "unknown method: " << method;
        SendResponse(request, DispatchResponse::Fail("unknown method: " + method));
    }
}

void RuntimeImpl::DispatcherImpl::Enable(const DispatchRequest &request)
{
    DispatchResponse response = runtime_->Enable();
    SendResponse(request, response);
}

void RuntimeImpl::DispatcherImpl::Disable(const DispatchRequest &request)
{
    DispatchResponse response = runtime_->Disable();
    SendResponse(request, response);
}

void RuntimeImpl::DispatcherImpl::RunIfWaitingForDebugger(const DispatchRequest &request)
{
    DispatchResponse response = runtime_->RunIfWaitingForDebugger();
    SendResponse(request, response);
}

void RuntimeImpl::DispatcherImpl::GetProperties(const DispatchRequest &request)
{
    std::unique_ptr<GetPropertiesParams> params = GetPropertiesParams::Create(request.GetParams());
    if (params == nullptr) {
        SendResponse(request, DispatchResponse::Fail("wrong params"));
        return;
    }

    std::vector<std::unique_ptr<PropertyDescriptor>> outPropertyDesc;
    std::optional<std::vector<std::unique_ptr<InternalPropertyDescriptor>>> outInternalDescs;
    std::optional<std::vector<std::unique_ptr<PrivatePropertyDescriptor>>> outPrivateProperties;
    std::optional<std::unique_ptr<ExceptionDetails>> outExceptionDetails;
    DispatchResponse response = runtime_->GetProperties(*params, &outPropertyDesc, &outInternalDescs,
        &outPrivateProperties, &outExceptionDetails);
    if (outExceptionDetails) {
        ASSERT(outExceptionDetails.value() != nullptr);
        LOG_DEBUGGER(WARN) << "GetProperties thrown an exception";
    }
    GetPropertiesReturns result(std::move(outPropertyDesc),
        std::move(outInternalDescs),
        std::move(outPrivateProperties),
        std::move(outExceptionDetails));
    SendResponse(request, response, result);
}

void RuntimeImpl::DispatcherImpl::CallFunctionOn(const DispatchRequest &request)
{
    std::unique_ptr<CallFunctionOnParams> params = CallFunctionOnParams::Create(request.GetParams());
    if (params == nullptr) {
        SendResponse(request, DispatchResponse::Fail("wrong params"));
        return;
    }

    std::unique_ptr<RemoteObject> outRemoteObject;
    std::optional<std::unique_ptr<ExceptionDetails>> outExceptionDetails;
    DispatchResponse response = runtime_->CallFunctionOn(*params, &outRemoteObject, &outExceptionDetails);
    if (outExceptionDetails) {
        ASSERT(outExceptionDetails.value() != nullptr);
        LOG_DEBUGGER(WARN) << "CallFunctionOn thrown an exception";
    }
    if (outRemoteObject == nullptr) {
        SendResponse(request, response);
        return;
    }

    CallFunctionOnReturns result(std::move(outRemoteObject), std::move(outExceptionDetails));
    SendResponse(request, response, result);
}

void RuntimeImpl::DispatcherImpl::GetHeapUsage(const DispatchRequest &request)
{
    double usedSize = 0;
    double totalSize = 0;
    DispatchResponse response = runtime_->GetHeapUsage(&usedSize, &totalSize);
    GetHeapUsageReturns result(usedSize, totalSize);
    SendResponse(request, response, result);
}

bool RuntimeImpl::Frontend::AllowNotify() const
{
    return channel_ != nullptr;
}

void RuntimeImpl::Frontend::RunIfWaitingForDebugger()
{
    if (!AllowNotify()) {
        return;
    }

    channel_->RunIfWaitingForDebugger();
}

DispatchResponse RuntimeImpl::Enable()
{
    return DispatchResponse::Ok();
}

DispatchResponse RuntimeImpl::Disable()
{
    return DispatchResponse::Ok();
}

DispatchResponse RuntimeImpl::RunIfWaitingForDebugger()
{
    frontend_.RunIfWaitingForDebugger();
    return DispatchResponse::Ok();
}

DispatchResponse RuntimeImpl::CallFunctionOn([[maybe_unused]] const CallFunctionOnParams &params,
    std::unique_ptr<RemoteObject> *outRemoteObject,
    [[maybe_unused]] std::optional<std::unique_ptr<ExceptionDetails>> *outExceptionDetails)
{
    // Return EvalError temporarily.
    auto error = Exception::EvalError(vm_, StringRef::NewFromUtf8(vm_, "Unsupport eval now"));
    *outRemoteObject = RemoteObject::FromTagged(vm_, error);
    return DispatchResponse::Ok();
}

DispatchResponse RuntimeImpl::GetHeapUsage(double *usedSize, double *totalSize)
{
#ifdef ECMASCRIPT_SUPPORT_HEAPPROFILER
    *totalSize = static_cast<double>(DFXJSNApi::GetHeapTotalSize(vm_));
    *usedSize = static_cast<double>(DFXJSNApi::GetHeapUsedSize(vm_));
#else
    *totalSize = 0;
    *usedSize = 0;
#endif
    return DispatchResponse::Ok();
}

DispatchResponse RuntimeImpl::GetProperties(const GetPropertiesParams &params,
    std::vector<std::unique_ptr<PropertyDescriptor>> *outPropertyDesc,
    [[maybe_unused]] std::optional<std::vector<std::unique_ptr<InternalPropertyDescriptor>>> *outInternalDescs,
    [[maybe_unused]] std::optional<std::vector<std::unique_ptr<PrivatePropertyDescriptor>>> *outPrivateProps,
    [[maybe_unused]] std::optional<std::unique_ptr<ExceptionDetails>> *outExceptionDetails)
{
    RemoteObjectId objectId = params.GetObjectId();
    bool isOwn = params.GetOwnProperties();
    bool isAccessorOnly = params.GetAccessPropertiesOnly();
    auto iter = properties_.find(objectId);
    if (iter == properties_.end()) {
        LOG_DEBUGGER(ERROR) << "RuntimeImpl::GetProperties Unknown object id: " << objectId;
        return DispatchResponse::Fail("Unknown object id");
    }
    Local<JSValueRef> value = Local<JSValueRef>(vm_, iter->second);
    if (value.IsEmpty() || !value->IsObject()) {
        LOG_DEBUGGER(ERROR) << "RuntimeImpl::GetProperties should a js object";
        return DispatchResponse::Fail("Not a object");
    }
    bool skipProto = false;
    if (!internalObjects_.IsEmpty() && internalObjects_->Get(vm_, value)->IsNumber()) {
        if (static_cast<ArkInternalValueType>(internalObjects_->Get(vm_, value)->ToNumber(vm_)->Value()) ==
            ArkInternalValueType::Entry) {
            skipProto = true;
        }
    }
    if (value->IsArrayBuffer()) {
        Local<ArrayBufferRef> arrayBufferRef(value);
        AddTypedArrayRefs(arrayBufferRef, outPropertyDesc);
    } else if (value->IsSharedArrayBuffer()) {
        Local<ArrayBufferRef> arrayBufferRef(value);
        AddSharedArrayBufferRefs(arrayBufferRef, outPropertyDesc);
    } else if (value->IsMapIterator()) {
        GetMapIteratorValue(value, outPropertyDesc);
    } else if (value->IsSetIterator()) {
        GetSetIteratorValue(value, outPropertyDesc);
    } else if (value->IsJSPrimitiveRef() && value->IsJSPrimitiveNumber()) {
        GetPrimitiveNumberValue(value, outPropertyDesc);
    } else if (value->IsJSPrimitiveRef() && value->IsJSPrimitiveString()) {
        GetPrimitiveStringValue(value, outPropertyDesc);
    } else if (value->IsJSPrimitiveRef() && value->IsJSPrimitiveBoolean()) {
        GetPrimitiveBooleanValue(value, outPropertyDesc);
    } else if (value->IsGeneratorFunction()) {
        GetGeneratorFunctionValue(value, outPropertyDesc);
    } else if (value->IsGeneratorObject()) {
        GetGeneratorObjectValue(value, outPropertyDesc);
    } else if (value->IsJSNumberFormat()) {
        GetNumberFormatValue(value, outPropertyDesc);
    } else if (value->IsJSCollator()) {
        GetCollatorValue(value, outPropertyDesc);
    } else if (value->IsJSDateTimeFormat()) {
        GetDateTimeFormatValue(value, outPropertyDesc);
    } else if (value->IsMap()) {
        GetMapValue(value, outPropertyDesc);
    } else if (value->IsRegExp()) {
        GetRegExpValue(value, outPropertyDesc);
    } else if (value->IsSet()) {
        GetSetValue(value, outPropertyDesc);
    }
    Local<ArrayRef> keys = Local<ObjectRef>(value)->GetOwnPropertyNames(vm_);
    int32_t length = keys->Length(vm_);
    Local<JSValueRef> name = JSValueRef::Undefined(vm_);
    for (int32_t i = 0; i < length; ++i) {
        name = keys->Get(vm_, i);
        PropertyAttribute jsProperty = PropertyAttribute::Default();
        if (!Local<ObjectRef>(value)->GetOwnProperty(vm_, name, jsProperty)) {
            continue;
        }
        std::unique_ptr<PropertyDescriptor> debuggerProperty =
            PropertyDescriptor::FromProperty(vm_, name, jsProperty);
        if (isAccessorOnly && !jsProperty.HasGetter() && !jsProperty.HasSetter()) {
            continue;
        }
        if (debuggerProperty->HasGet()) {
            debuggerProperty->GetGet()->SetObjectId(curObjectId_);
            properties_[curObjectId_++] = Global<JSValueRef>(vm_, jsProperty.GetGetter(vm_));
        }
        if (debuggerProperty->HasSet()) {
            debuggerProperty->GetSet()->SetObjectId(curObjectId_);
            properties_[curObjectId_++] = Global<JSValueRef>(vm_, jsProperty.GetSetter(vm_));
        }
        if (debuggerProperty->HasValue()) {
            Local<JSValueRef> vValue = jsProperty.GetValue(vm_);
            if (vValue->IsObject() && !vValue->IsProxy()) {
                debuggerProperty->GetValue()->SetObjectId(curObjectId_);
                properties_[curObjectId_++] = Global<JSValueRef>(vm_, vValue);
            }
        }
        if (debuggerProperty->HasSymbol()) {
            debuggerProperty->GetSymbol()->SetObjectId(curObjectId_);
            properties_[curObjectId_++] = Global<JSValueRef>(vm_, name);
        }
        outPropertyDesc->emplace_back(std::move(debuggerProperty));
    }
    if (!skipProto) {
        GetProtoOrProtoType(value, isOwn, isAccessorOnly, outPropertyDesc);
    }
    GetAdditionalProperties(value, outPropertyDesc);

    return DispatchResponse::Ok();
}

void RuntimeImpl::AddTypedArrayRefs(Local<ArrayBufferRef> arrayBufferRef,
    std::vector<std::unique_ptr<PropertyDescriptor>> *outPropertyDesc)
{
    int32_t arrayBufferByteLength = arrayBufferRef->ByteLength(vm_);
    int32_t typedArrayLength = arrayBufferByteLength;
    AddTypedArrayRef<Int8ArrayRef>(arrayBufferRef, typedArrayLength, "[[Int8Array]]", outPropertyDesc);
    AddTypedArrayRef<Uint8ArrayRef>(arrayBufferRef, typedArrayLength, "[[Uint8Array]]", outPropertyDesc);
    AddTypedArrayRef<Uint8ClampedArrayRef>(arrayBufferRef, typedArrayLength, "[[Uint8ClampedArray]]", outPropertyDesc);

    if ((arrayBufferByteLength % NumberSize::BYTES_OF_16BITS) == 0) {
        typedArrayLength = arrayBufferByteLength / NumberSize::BYTES_OF_16BITS;
        AddTypedArrayRef<Int16ArrayRef>(arrayBufferRef, typedArrayLength, "[[Int16Array]]", outPropertyDesc);
        AddTypedArrayRef<Uint16ArrayRef>(arrayBufferRef, typedArrayLength, "[[Uint16Array]]", outPropertyDesc);
    }

    if ((arrayBufferByteLength % NumberSize::BYTES_OF_32BITS) == 0) {
        typedArrayLength = arrayBufferByteLength / NumberSize::BYTES_OF_32BITS;
        AddTypedArrayRef<Int32ArrayRef>(arrayBufferRef, typedArrayLength, "[[Int32Array]]", outPropertyDesc);
        AddTypedArrayRef<Uint32ArrayRef>(arrayBufferRef, typedArrayLength, "[[Uint32Array]]", outPropertyDesc);
        AddTypedArrayRef<Float32ArrayRef>(arrayBufferRef, typedArrayLength, "[[Float32Array]]", outPropertyDesc);
    }

    if ((arrayBufferByteLength % NumberSize::BYTES_OF_64BITS) == 0) {
        typedArrayLength = arrayBufferByteLength / NumberSize::BYTES_OF_64BITS;
        AddTypedArrayRef<Float64ArrayRef>(arrayBufferRef, typedArrayLength, "[[Float64Array]]", outPropertyDesc);
        AddTypedArrayRef<BigInt64ArrayRef>(arrayBufferRef, typedArrayLength, "[[BigInt64Array]]", outPropertyDesc);
        AddTypedArrayRef<BigUint64ArrayRef>(arrayBufferRef, typedArrayLength, "[[BigUint64Array]]", outPropertyDesc);
    }
}

void RuntimeImpl::AddSharedArrayBufferRefs(Local<ArrayBufferRef> arrayBufferRef,
    std::vector<std::unique_ptr<PropertyDescriptor>> *outPropertyDesc)
{
    int32_t arrayBufferByteLength = arrayBufferRef->ByteLength(vm_);
    int32_t typedArrayLength = arrayBufferByteLength;
    AddTypedArrayRef<Int8ArrayRef>(arrayBufferRef, typedArrayLength, "[[Int8Array]]", outPropertyDesc);
    AddTypedArrayRef<Uint8ArrayRef>(arrayBufferRef, typedArrayLength, "[[Uint8Array]]", outPropertyDesc);

    if ((arrayBufferByteLength % NumberSize::BYTES_OF_16BITS) == 0) {
        typedArrayLength = arrayBufferByteLength / NumberSize::BYTES_OF_16BITS;
        AddTypedArrayRef<Int16ArrayRef>(arrayBufferRef, typedArrayLength, "[[Int16Array]]", outPropertyDesc);
    }

    if ((arrayBufferByteLength % NumberSize::BYTES_OF_32BITS) == 0) {
        typedArrayLength = arrayBufferByteLength / NumberSize::BYTES_OF_32BITS;
        AddTypedArrayRef<Int32ArrayRef>(arrayBufferRef, typedArrayLength, "[[Int32Array]]", outPropertyDesc);
    }
    Local<JSValueRef> jsValueRef;
    jsValueRef = NumberRef::New(vm_, arrayBufferByteLength);
    SetKeyValue(jsValueRef, outPropertyDesc, "[[ArrayBufferByteLength]]");
    SetKeyValue(jsValueRef, outPropertyDesc, "byteLength");
}

template <typename TypedArrayRef>
void RuntimeImpl::AddTypedArrayRef(Local<ArrayBufferRef> arrayBufferRef, int32_t length, const char* name,
    std::vector<std::unique_ptr<PropertyDescriptor>> *outPropertyDesc)
{
    Local<JSValueRef> jsValueRefTypedArray(TypedArrayRef::New(vm_, arrayBufferRef, 0, length));
    std::unique_ptr<RemoteObject> remoteObjectTypedArray = RemoteObject::FromTagged(vm_, jsValueRefTypedArray);
    remoteObjectTypedArray->SetObjectId(curObjectId_);
    properties_[curObjectId_++] = Global<JSValueRef>(vm_, jsValueRefTypedArray);
    std::unique_ptr<PropertyDescriptor> debuggerProperty = std::make_unique<PropertyDescriptor>();
    debuggerProperty->SetName(name)
        .SetWritable(true)
        .SetConfigurable(true)
        .SetEnumerable(false)
        .SetIsOwn(true)
        .SetValue(std::move(remoteObjectTypedArray));
    outPropertyDesc->emplace_back(std::move(debuggerProperty));
}

void RuntimeImpl::CacheObjectIfNeeded(Local<JSValueRef> valRef, RemoteObject *remoteObj)
{
    if (valRef->IsObject() && !valRef->IsProxy()) {
        remoteObj->SetObjectId(curObjectId_);
        properties_[curObjectId_++] = Global<JSValueRef>(vm_, valRef);
    }
}

void RuntimeImpl::GetProtoOrProtoType(Local<JSValueRef> value, bool isOwn, bool isAccessorOnly,
    std::vector<std::unique_ptr<PropertyDescriptor>> *outPropertyDesc)
{
    if (!isAccessorOnly && isOwn && !value->IsProxy()) {
        return;
    }
    // Get Function ProtoOrHClass
    if (value->IsConstructor()) {
        Local<JSValueRef> prototype = Local<FunctionRef>(value)->GetFunctionPrototype(vm_);
        std::unique_ptr<RemoteObject> protoObj = RemoteObject::FromTagged(vm_, prototype);
        CacheObjectIfNeeded(prototype, protoObj.get());
        std::unique_ptr<PropertyDescriptor> debuggerProperty = std::make_unique<PropertyDescriptor>();
        debuggerProperty->SetName("prototype")
            .SetWritable(false)
            .SetConfigurable(false)
            .SetEnumerable(false)
            .SetIsOwn(true)
            .SetValue(std::move(protoObj));
        outPropertyDesc->emplace_back(std::move(debuggerProperty));
    }
    // Get __proto__
    Local<JSValueRef> proto = Local<ObjectRef>(value)->GetPrototype(vm_);
    std::unique_ptr<RemoteObject> protoObj = RemoteObject::FromTagged(vm_, proto);
    CacheObjectIfNeeded(proto, protoObj.get());
    std::unique_ptr<PropertyDescriptor> debuggerProperty = std::make_unique<PropertyDescriptor>();
    debuggerProperty->SetName("__proto__")
        .SetWritable(true)
        .SetConfigurable(true)
        .SetEnumerable(false)
        .SetIsOwn(true)
        .SetValue(std::move(protoObj));
    outPropertyDesc->emplace_back(std::move(debuggerProperty));
}

void RuntimeImpl::GetAdditionalProperties(Local<JSValueRef> value,
    std::vector<std::unique_ptr<PropertyDescriptor>> *outPropertyDesc)
{
    // The length of the TypedArray have to be limited(less than or equal to lengthTypedArrayLimit) until we construct
    // the PropertyPreview class. Let lengthTypedArrayLimit be 10000 temporarily.
    static const uint32_t lengthTypedArrayLimit = 10000;

    // The width of the string-expression for JSTypedArray::MAX_TYPED_ARRAY_INDEX which is euqal to
    // JSObject::MAX_ELEMENT_INDEX which is equal to std::numeric_limits<uint32_t>::max(). (42,9496,7295)
    static const int32_t widthStrExprMaxElementIndex = 10;

    if (value->IsTypedArray()) {
        Local<TypedArrayRef> localTypedArrayRef(value);
        uint32_t lengthTypedArray = localTypedArrayRef->ArrayLength(vm_);
        if (lengthTypedArray > lengthTypedArrayLimit) {
            LOG_DEBUGGER(ERROR) << "The length of the TypedArray is non-compliant or unsupported.";
            return;
        }
        for (uint32_t i = 0; i < lengthTypedArray; i++) {
            Local<JSValueRef> localValRefElement = localTypedArrayRef->Get(vm_, i);
            std::unique_ptr<RemoteObject> remoteObjElement = RemoteObject::FromTagged(vm_, localValRefElement);
            remoteObjElement->SetObjectId(curObjectId_);
            properties_[curObjectId_++] = Global<JSValueRef>(vm_, localValRefElement);
            std::unique_ptr<PropertyDescriptor> debuggerProperty = std::make_unique<PropertyDescriptor>();

            std::ostringstream osNameElement;
            osNameElement << std::right << std::setw(widthStrExprMaxElementIndex) << i;
            std::string cStrNameElement = osNameElement.str();
            debuggerProperty->SetName(cStrNameElement)
                .SetWritable(true)
                .SetConfigurable(true)
                .SetEnumerable(false)
                .SetIsOwn(true)
                .SetValue(std::move(remoteObjElement));
            outPropertyDesc->emplace_back(std::move(debuggerProperty));
        }
    }
}

void RuntimeImpl::SetKeyValue(Local<JSValueRef> &jsValueRef,
    std::vector<std::unique_ptr<PropertyDescriptor>> *outPropertyDesc, const std::string &strProName)
{
    std::unique_ptr<RemoteObject> remoteObj = RemoteObject::FromTagged(vm_, jsValueRef);
    remoteObj->SetObjectId(curObjectId_);
    properties_[curObjectId_++] = Global<JSValueRef>(vm_, jsValueRef);
    std::unique_ptr<PropertyDescriptor> debuggerProperty = std::make_unique<PropertyDescriptor>();
    debuggerProperty->SetName(strProName)
        .SetWritable(false)
        .SetConfigurable(false)
        .SetEnumerable(false)
        .SetIsOwn(false)
        .SetValue(std::move(remoteObj));
    outPropertyDesc->emplace_back(std::move(debuggerProperty));
}

void RuntimeImpl::GetPrimitiveNumberValue(Local<JSValueRef> value,
    std::vector<std::unique_ptr<PropertyDescriptor>> *outPropertyDesc)
{
    Local<JSValueRef> jsValueRef;
    jsValueRef = value->ToNumber(vm_);
    SetKeyValue(jsValueRef, outPropertyDesc, "[[PrimitiveValue]]");
}

void RuntimeImpl::GetPrimitiveStringValue(Local<JSValueRef> value,
    std::vector<std::unique_ptr<PropertyDescriptor>> *outPropertyDesc)
{
    Local<JSValueRef> jsValueRef;
    jsValueRef = value->ToString(vm_);
    SetKeyValue(jsValueRef, outPropertyDesc, "[[PrimitiveValue]]");
}

void RuntimeImpl::GetPrimitiveBooleanValue(Local<JSValueRef> value,
    std::vector<std::unique_ptr<PropertyDescriptor>> *outPropertyDesc)
{
    Local<JSValueRef> jsValueRef;
    Local<PrimitiveRef> primitiveRef(value);
    jsValueRef = primitiveRef->GetValue(vm_);
    SetKeyValue(jsValueRef, outPropertyDesc, "[[PrimitiveValue]]");
}

void RuntimeImpl::GetMapIteratorValue(Local<JSValueRef> value,
    std::vector<std::unique_ptr<PropertyDescriptor>> *outPropertyDesc)
{
    Local<JSValueRef> jsValueRef;
    Local<MapIteratorRef> iterRef = value->ToObject(vm_);
    if (!iterRef.IsEmpty()) {
        jsValueRef = NumberRef::New(vm_, iterRef->GetIndex());
        SetKeyValue(jsValueRef, outPropertyDesc, "[[IteratorIndex]]");
        jsValueRef = iterRef->GetKind(vm_);
        SetKeyValue(jsValueRef, outPropertyDesc, "[[IteratorKind]]");
    }
}

void RuntimeImpl::GetSetIteratorValue(Local<JSValueRef> value,
    std::vector<std::unique_ptr<PropertyDescriptor>> *outPropertyDesc)
{
    Local<JSValueRef> jsValueRef;
    Local<SetIteratorRef> iterRef = value->ToObject(vm_);
    if (!iterRef.IsEmpty()) {
        jsValueRef = NumberRef::New(vm_, iterRef->GetIndex());
        SetKeyValue(jsValueRef, outPropertyDesc, "[[IteratorIndex]]");
        jsValueRef = iterRef->GetKind(vm_);
        SetKeyValue(jsValueRef, outPropertyDesc, "[[IteratorKind]]");
    }
}

void RuntimeImpl::GetGeneratorFunctionValue(Local<JSValueRef> value,
    std::vector<std::unique_ptr<PropertyDescriptor>> *outPropertyDesc)
{
    Local<JSValueRef> jsValueRef;
    Local<GeneratorFunctionRef> genFuncRef = value->ToObject(vm_);
    if (!genFuncRef.IsEmpty()) {
        jsValueRef = BooleanRef::New(vm_, genFuncRef->IsGenerator());
        SetKeyValue(jsValueRef, outPropertyDesc, "[[IsGenerator]]");
    }
}

void RuntimeImpl::GetGeneratorObjectValue(Local<JSValueRef> value,
    std::vector<std::unique_ptr<PropertyDescriptor>> *outPropertyDesc)
{
    Local<JSValueRef> jsValueRef;
    Local<GeneratorObjectRef> genObjRef = value->ToObject(vm_);
    if (!genObjRef.IsEmpty()) {
        jsValueRef = genObjRef->GetGeneratorState(vm_);
        SetKeyValue(jsValueRef, outPropertyDesc, "[[GeneratorState]]");
        jsValueRef = genObjRef->GetGeneratorFunction(vm_);
        SetKeyValue(jsValueRef, outPropertyDesc, "[[GeneratorFunction]]");
        jsValueRef = JSNApi::GetGlobalObject(vm_);
        SetKeyValue(jsValueRef, outPropertyDesc, "[[GeneratorReceiver]]");
    }
}

void RuntimeImpl::GetNumberFormatValue(Local<JSValueRef> value,
    std::vector<std::unique_ptr<PropertyDescriptor>> *outPropertyDesc)
{
    Local<NumberFormatRef> numberFormatRef = value->ToObject(vm_);
    Local<JSValueRef> jsValueRef = numberFormatRef->GetFormatFunction(vm_);
    SetKeyValue(jsValueRef, outPropertyDesc, "format");
}

void RuntimeImpl::GetCollatorValue(Local<JSValueRef> value,
    std::vector<std::unique_ptr<PropertyDescriptor>> *outPropertyDesc)
{
    Local<CollatorRef> collatorRef = value->ToObject(vm_);
    Local<JSValueRef> jsValueRef = collatorRef->GetCompareFunction(vm_);
    SetKeyValue(jsValueRef, outPropertyDesc, "compare");
}

void RuntimeImpl::GetDateTimeFormatValue(Local<JSValueRef> value,
    std::vector<std::unique_ptr<PropertyDescriptor>> *outPropertyDesc)
{
    Local<DataTimeFormatRef> dtFormatRef = value->ToObject(vm_);
    Local<JSValueRef> jsValueRef = dtFormatRef->GetFormatFunction(vm_);
    SetKeyValue(jsValueRef, outPropertyDesc, "format");
}

void RuntimeImpl::AddInternalProperties(Local<ObjectRef> objRef, ArkInternalValueType type)
{
    if (internalObjects_.IsEmpty()) {
        internalObjects_ = Global<MapRef>(vm_, MapRef::New(vm_));
    }
    internalObjects_->Set(vm_, objRef, NumberRef::New(vm_, static_cast<int32_t>(type)));
}

void RuntimeImpl::GetMapValue(Local<JSValueRef> value,
    std::vector<std::unique_ptr<PropertyDescriptor>> *outPropertyDesc)
{
    Local<MapRef> mapRef = value->ToObject(vm_);
    int32_t size = mapRef->GetSize();
    int32_t len = mapRef->GetTotalElements();
    int32_t index = 0;
    Local<JSValueRef> jsValueRef = NumberRef::New(vm_, size);
    SetKeyValue(jsValueRef, outPropertyDesc, "size");
    jsValueRef = ArrayRef::New(vm_, size);
    for (int32_t i = 0; i < len; i++) {
        Local<JSValueRef> jsKey = mapRef->GetKey(vm_, i);
        if (jsKey->IsHole()) {
            continue;
        }
        Local<JSValueRef> jsValue = mapRef->GetValue(vm_, i);
        Local<ObjectRef> objRef = ObjectRef::New(vm_);
        objRef->Set(vm_, StringRef::NewFromUtf8(vm_, "key"), jsKey);
        objRef->Set(vm_, StringRef::NewFromUtf8(vm_, "value"), jsValue);
        AddInternalProperties(objRef, ArkInternalValueType::Entry);
        ArrayRef::SetValueAt(vm_, jsValueRef, index++, objRef);
    }
    AddInternalProperties(jsValueRef, ArkInternalValueType::Entry);
    SetKeyValue(jsValueRef, outPropertyDesc, "[[Entries]]");
}

void RuntimeImpl::GetSetValue(Local<JSValueRef> value,
    std::vector<std::unique_ptr<PropertyDescriptor>> *outPropertyDesc)
{
    Local<SetRef> setRef = value->ToObject(vm_);
    int32_t size = setRef->GetSize();
    int32_t len = setRef->GetTotalElements();
    int32_t index = 0;
    Local<JSValueRef> jsValueRef = NumberRef::New(vm_, size);
    SetKeyValue(jsValueRef, outPropertyDesc, "size");
    jsValueRef = ArrayRef::New(vm_, size);
    for (int32_t i = 0; i < len; i++) {
        Local<JSValueRef> elementRef = setRef->GetValue(vm_, i);
        if (elementRef->IsHole()) {
            continue;
        } else if (elementRef->IsObject()) {
            Local<ObjectRef> objRef = ObjectRef::New(vm_);
            objRef->Set(vm_, StringRef::NewFromUtf8(vm_, "value"), elementRef);
            AddInternalProperties(objRef, ArkInternalValueType::Entry);
            ArrayRef::SetValueAt(vm_, jsValueRef, index++, objRef);
        } else {
            ArrayRef::SetValueAt(vm_, jsValueRef, index++, elementRef);
        }
    }
    AddInternalProperties(jsValueRef, ArkInternalValueType::Entry);
    SetKeyValue(jsValueRef, outPropertyDesc, "[[Entries]]");
}

void RuntimeImpl::GetRegExpValue(Local<JSValueRef> value,
    std::vector<std::unique_ptr<PropertyDescriptor>> *outPropertyDesc)
{
    Local<RegExpRef> regExpRef = value->ToObject(vm_);
    Local<JSValueRef> jsValueRef = regExpRef->IsGlobal(vm_);
    SetKeyValue(jsValueRef, outPropertyDesc, "global");
    jsValueRef = regExpRef->IsIgnoreCase(vm_);
    SetKeyValue(jsValueRef, outPropertyDesc, "ignoreCase");
    jsValueRef = regExpRef->IsMultiline(vm_);
    SetKeyValue(jsValueRef, outPropertyDesc, "multiline");
    jsValueRef = regExpRef->IsDotAll(vm_);
    SetKeyValue(jsValueRef, outPropertyDesc, "dotAll");
    SetKeyValue(jsValueRef, outPropertyDesc, "hasIndices");
    jsValueRef = regExpRef->IsUtf16(vm_);
    SetKeyValue(jsValueRef, outPropertyDesc, "unicode");
    jsValueRef = regExpRef->IsStick(vm_);
    SetKeyValue(jsValueRef, outPropertyDesc, "sticky");
    std::string strFlags = regExpRef->GetOriginalFlags();
    jsValueRef = StringRef::NewFromUtf8(vm_, strFlags.c_str());
    SetKeyValue(jsValueRef, outPropertyDesc, "flags");
    std::string strSource = regExpRef->GetOriginalSource(vm_)->ToString();
    jsValueRef = StringRef::NewFromUtf8(vm_, strSource.c_str());
    SetKeyValue(jsValueRef, outPropertyDesc, "source");
}
}  // namespace panda::ecmascript::tooling