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

#include "agent/heapprofiler_impl.h"
#include "ecmascript/tests/test_helper.h"

using namespace panda::ecmascript;
using namespace panda::ecmascript::tooling;


namespace panda::test {
class HeapProfilerImplTest : public testing::Test {
public:
    static void SetUpTestCase()
    {
        GTEST_LOG_(INFO) << "SetUpTestCase";
    }

    static void TearDownTestCase()
    {
        GTEST_LOG_(INFO) << "TearDownCase";
    }

    void SetUp() override
    {
        TestHelper::CreateEcmaVMWithScope(ecmaVm, thread, scope);
    }

    void TearDown() override
    {
        TestHelper::DestroyEcmaVMWithScope(ecmaVm, scope);
    }

protected:
    EcmaVM *ecmaVm {nullptr};
    EcmaHandleScope *scope {nullptr};
    JSThread *thread {nullptr};
};

HWTEST_F_L0(HeapProfilerImplTest, AddInspectedHeapObject)
{
    ProtocolChannel *channel = nullptr;
    AddInspectedHeapObjectParams param;
    auto heapProfiler = std::make_unique<HeapProfilerImpl>(ecmaVm, channel);
    DispatchResponse response = heapProfiler->AddInspectedHeapObject(param);
    ASSERT_TRUE(response.GetMessage() == "AddInspectedHeapObject not support now");
    ASSERT_TRUE(!response.IsOk());
}

HWTEST_F_L0(HeapProfilerImplTest, CollectGarbage)
{
    ProtocolChannel *channel = nullptr;
    auto heapProfiler = std::make_unique<HeapProfilerImpl>(ecmaVm, channel);
    DispatchResponse response = heapProfiler->CollectGarbage();
    ASSERT_TRUE(response.GetMessage() == "");
    ASSERT_TRUE(response.IsOk());
}

HWTEST_F_L0(HeapProfilerImplTest, Enable)
{
    ProtocolChannel *channel = nullptr;
    auto heapProfiler = std::make_unique<HeapProfilerImpl>(ecmaVm, channel);
    DispatchResponse response = heapProfiler->Enable();
    ASSERT_TRUE(response.GetMessage() == "");
    ASSERT_TRUE(response.IsOk());
}

HWTEST_F_L0(HeapProfilerImplTest, Disable)
{
    ProtocolChannel *channel = nullptr;
    auto heapProfiler = std::make_unique<HeapProfilerImpl>(ecmaVm, channel);
    DispatchResponse response = heapProfiler->Disable();
    ASSERT_TRUE(response.GetMessage() == "");
    ASSERT_TRUE(response.IsOk());
}

HWTEST_F_L0(HeapProfilerImplTest, GetHeapObjectId)
{
    ProtocolChannel *channel = nullptr;
    auto heapProfiler = std::make_unique<HeapProfilerImpl>(ecmaVm, channel);
    GetHeapObjectIdParams params;
    HeapSnapshotObjectId objectId;
    DispatchResponse response = heapProfiler->GetHeapObjectId(params, &objectId);
    ASSERT_TRUE(response.GetMessage() == "GetHeapObjectId not support now");
    ASSERT_TRUE(!response.IsOk());
}

HWTEST_F_L0(HeapProfilerImplTest, GetObjectByHeapObjectId)
{
    ProtocolChannel *channel = nullptr;
    auto heapProfiler = std::make_unique<HeapProfilerImpl>(ecmaVm, channel);
    std::unique_ptr<GetObjectByHeapObjectIdParams> params;
    std::unique_ptr<RemoteObject> remoteObjectResult;
    DispatchResponse response = heapProfiler->GetObjectByHeapObjectId(*params, &remoteObjectResult);
    ASSERT_TRUE(response.GetMessage() == "GetObjectByHeapObjectId not support now");
    ASSERT_TRUE(!response.IsOk());
}

HWTEST_F_L0(HeapProfilerImplTest, GetSamplingProfile)
{
    ProtocolChannel *channel = nullptr;
    auto heapProfiler = std::make_unique<HeapProfilerImpl>(ecmaVm, channel);
    std::unique_ptr<SamplingHeapProfile> profile;
    DispatchResponse response = heapProfiler->GetSamplingProfile(&profile);
    ASSERT_TRUE(response.GetMessage() == "GetSamplingProfile not support now");
    ASSERT_TRUE(!response.IsOk());
}

HWTEST_F_L0(HeapProfilerImplTest, StartSampling)
{
    ProtocolChannel *channel = nullptr;
    auto heapProfiler = std::make_unique<HeapProfilerImpl>(ecmaVm, channel);
    StartSamplingParams params;
    DispatchResponse response = heapProfiler->StartSampling(params);
    ASSERT_TRUE(response.GetMessage() == "StartSampling not support now");
    ASSERT_TRUE(!response.IsOk());
}

HWTEST_F_L0(HeapProfilerImplTest, StopSampling)
{
    ProtocolChannel *channel = nullptr;
    auto heapProfiler = std::make_unique<HeapProfilerImpl>(ecmaVm, channel);
    std::unique_ptr<SamplingHeapProfile> profile;
    DispatchResponse response = heapProfiler->StopSampling(&profile);
    ASSERT_TRUE(response.GetMessage() == "StopSampling not support now.");
    ASSERT_TRUE(!response.IsOk());
}

HWTEST_F_L0(HeapProfilerImplTest, TakeHeapSnapshot)
{
    ProtocolChannel *channel = nullptr;
    auto heapProfiler = std::make_unique<HeapProfilerImpl>(ecmaVm, channel);
    StopTrackingHeapObjectsParams params;
    DispatchResponse response = heapProfiler->TakeHeapSnapshot(params);
    ASSERT_TRUE(response.GetMessage() == "");
    ASSERT_TRUE(response.IsOk());
}

HWTEST_F_L0(HeapProfilerImplTest, DispatcherImplDispatch)
{
    std::string result = "";
    std::function<void(const void*, const std::string &)> callback =
        [&result]([[maybe_unused]]const void* ptr, const std::string &temp) {result = temp;};
    ProtocolChannel *channel = new ProtocolHandler(callback, ecmaVm);
    auto tracing = std::make_unique<HeapProfilerImpl>(ecmaVm, channel);
    auto dispatcherImpl = std::make_unique<HeapProfilerImpl::DispatcherImpl>(channel, std::move(tracing));
    std::string msg = std::string() + R"({"id":0,"method":"Debugger.Test","params":{}})";
    DispatchRequest request(msg);
    dispatcherImpl->Dispatch(request);
    ASSERT_TRUE(result.find("Unknown method: Test") != std::string::npos);
    msg = std::string() + R"({"id":0,"method":"Debugger.disable","params":{}})";
    DispatchRequest request1 = DispatchRequest(msg);
    dispatcherImpl->Dispatch(request1);
    if (channel) {
        delete channel;
        channel = nullptr;
    }
    ASSERT_TRUE(result == "{\"id\":0,\"result\":{}}");
}

HWTEST_F_L0(HeapProfilerImplTest, DispatcherImplAddInspectedHeapObject)
{
    std::string result = "";
    std::function<void(const void*, const std::string &)> callback =
        [&result]([[maybe_unused]]const void* ptr, const std::string &temp) {result = temp;};
    ProtocolChannel *channel = new ProtocolHandler(callback, ecmaVm);
    auto tracing = std::make_unique<HeapProfilerImpl>(ecmaVm, channel);
    auto dispatcherImpl = std::make_unique<HeapProfilerImpl::DispatcherImpl>(channel, std::move(tracing));
    std::string msg = std::string() + R"({"id":0,"method":"HeapProfiler.disable","params":{}})";
    DispatchRequest request(msg);
    dispatcherImpl->AddInspectedHeapObject(request);
    ASSERT_TRUE(result.find("wrong params") != std::string::npos);
    msg = std::string() + R"({"id":0,"method":"HeapProfiler.disable","params":{"heapObjectId":"0"}})";
    DispatchRequest request1 = DispatchRequest(msg);
    dispatcherImpl->AddInspectedHeapObject(request1);
    if (channel) {
        delete channel;
        channel = nullptr;
    }
    ASSERT_TRUE(result.find("AddInspectedHeapObject not support now") != std::string::npos);
}

HWTEST_F_L0(HeapProfilerImplTest, DispatcherImplCollectGarbage)
{
    std::string result = "";
    std::function<void(const void*, const std::string &)> callback =
        [&result]([[maybe_unused]]const void* ptr, const std::string &temp) {result = temp;};
    ProtocolChannel *channel = new ProtocolHandler(callback, ecmaVm);
    auto tracing = std::make_unique<HeapProfilerImpl>(ecmaVm, channel);
    auto dispatcherImpl = std::make_unique<HeapProfilerImpl::DispatcherImpl>(channel, std::move(tracing));
    std::string msg = std::string() + R"({"id":0,"method":"HeapProfiler.collectGarbage","params":{}})";
    DispatchRequest request(msg);
    dispatcherImpl->CollectGarbage(request);
    ASSERT_TRUE(result == "{\"id\":0,\"result\":{}}");
    if (channel) {
        delete channel;
        channel = nullptr;
    }
}

HWTEST_F_L0(HeapProfilerImplTest, DispatcherImplEnable)
{
    std::string result = "";
    std::function<void(const void*, const std::string &)> callback =
        [&result]([[maybe_unused]]const void* ptr, const std::string &temp) {result = temp;};
    ProtocolChannel *channel = new ProtocolHandler(callback, ecmaVm);
    auto tracing = std::make_unique<HeapProfilerImpl>(ecmaVm, channel);
    auto dispatcherImpl = std::make_unique<HeapProfilerImpl::DispatcherImpl>(channel, std::move(tracing));
    std::string msg = std::string() + R"({"id":0,"method":"HeapProfiler.enable","params":{}})";
    DispatchRequest request(msg);
    dispatcherImpl->Enable(request);
    ASSERT_TRUE(result == "{\"id\":0,\"result\":{}}");
    if (channel) {
        delete channel;
        channel = nullptr;
    }
}

HWTEST_F_L0(HeapProfilerImplTest, DispatcherImplDisable)
{
    std::string result = "";
    std::function<void(const void*, const std::string &)> callback =
        [&result]([[maybe_unused]]const void* ptr, const std::string &temp) {result = temp;};
    ProtocolChannel *channel = new ProtocolHandler(callback, ecmaVm);
    auto tracing = std::make_unique<HeapProfilerImpl>(ecmaVm, channel);
    auto dispatcherImpl = std::make_unique<HeapProfilerImpl::DispatcherImpl>(channel, std::move(tracing));
    std::string msg = std::string() + R"({"id":0,"method":"HeapProfiler.disable","params":{}})";
    DispatchRequest request(msg);
    dispatcherImpl->Disable(request);
    ASSERT_TRUE(result == "{\"id\":0,\"result\":{}}");
    if (channel) {
        delete channel;
        channel = nullptr;
    }
}

HWTEST_F_L0(HeapProfilerImplTest, DispatcherImplGetHeapObjectId)
{
    std::string result = "";
    std::function<void(const void*, const std::string &)> callback =
        [&result]([[maybe_unused]]const void* ptr, const std::string &temp) {result = temp;};
    ProtocolChannel *channel = new ProtocolHandler(callback, ecmaVm);
    auto tracing = std::make_unique<HeapProfilerImpl>(ecmaVm, channel);
    auto dispatcherImpl = std::make_unique<HeapProfilerImpl::DispatcherImpl>(channel, std::move(tracing));
    std::string msg = std::string() + R"({"id":0,"method":"HeapProfiler.getHeapObjectId","params":{"objectId":true}})";
    DispatchRequest request(msg);
    dispatcherImpl->GetHeapObjectId(request);
    ASSERT_TRUE(result.find("wrong params") != std::string::npos);
    msg = std::string() + R"({"id":0,"method":"HeapProfiler.getHeapObjectId","params":{"objectId":"0"}})";
    DispatchRequest request1(msg);
    dispatcherImpl->GetHeapObjectId(request1);
    ASSERT_TRUE(result.find("GetHeapObjectId not support now") != std::string::npos);
    if (channel) {
        delete channel;
        channel = nullptr;
    }
}

HWTEST_F_L0(HeapProfilerImplTest, DispatcherImplGetObjectByHeapObjectId)
{
    std::string result = "";
    std::function<void(const void*, const std::string &)> callback =
        [&result]([[maybe_unused]]const void* ptr, const std::string &temp) {result = temp;};
    ProtocolChannel *channel = new ProtocolHandler(callback, ecmaVm);
    auto tracing = std::make_unique<HeapProfilerImpl>(ecmaVm, channel);
    auto dispatcherImpl = std::make_unique<HeapProfilerImpl::DispatcherImpl>(channel, std::move(tracing));
    std::string msg = std::string() + R"({"id":0,"method":"HeapProfiler.getObjectByHeapObjectId","params":{
        "objectId":001}})";
    DispatchRequest request(msg);
    dispatcherImpl->GetObjectByHeapObjectId(request);
    ASSERT_TRUE(result.find("wrong params") != std::string::npos);
    msg = std::string() + R"({"id":0,"method":"HeapProfiler.getObjectByHeapObjectId","params":{"objectId":"001",
        "objectGroup":"000"}})";
    DispatchRequest request1(msg);
    dispatcherImpl->GetObjectByHeapObjectId(request1);
    ASSERT_TRUE(result.find("GetObjectByHeapObjectId not support now") != std::string::npos);
    if (channel) {
        delete channel;
        channel = nullptr;
    }
}

HWTEST_F_L0(HeapProfilerImplTest, DispatcherImplGetSamplingProfile)
{
    std::string result = "";
    std::function<void(const void*, const std::string &)> callback =
        [&result]([[maybe_unused]]const void* ptr, const std::string &temp) {result = temp;};
    ProtocolChannel *channel = new ProtocolHandler(callback, ecmaVm);
    auto tracing = std::make_unique<HeapProfilerImpl>(ecmaVm, channel);
    auto dispatcherImpl = std::make_unique<HeapProfilerImpl::DispatcherImpl>(channel, std::move(tracing));
    std::string msg = std::string() + R"({"id":0,"method":"HeapProfiler.getSamplingProfile","params":{}})";
    DispatchRequest request(msg);
    dispatcherImpl->GetSamplingProfile(request);
    ASSERT_TRUE(result.find("GetSamplingProfile not support now") != std::string::npos);
    if (channel) {
        delete channel;
        channel = nullptr;
    }
}

HWTEST_F_L0(HeapProfilerImplTest, DispatcherImplStartSampling)
{
    std::string result = "";
    std::function<void(const void*, const std::string &)> callback =
        [&result]([[maybe_unused]]const void* ptr, const std::string &temp) {result = temp;};
    ProtocolChannel *channel = new ProtocolHandler(callback, ecmaVm);
    auto tracing = std::make_unique<HeapProfilerImpl>(ecmaVm, channel);
    auto dispatcherImpl = std::make_unique<HeapProfilerImpl::DispatcherImpl>(channel, std::move(tracing));
    std::string msg = std::string() + R"({"id":0,"method":"HeapProfiler.startSampling","params":{
        "samplingInterval":"Test"}})";
    DispatchRequest request(msg);
    dispatcherImpl->StartSampling(request);
    ASSERT_TRUE(result.find("wrong params") != std::string::npos);
    msg = std::string() + R"({"id":0,"method":"HeapProfiler.startSampling","params":{"samplingInterval":1000}})";
    DispatchRequest request1(msg);
    dispatcherImpl->StartSampling(request1);
    ASSERT_TRUE(result.find("StartSampling not support now") != std::string::npos);
    if (channel) {
        delete channel;
        channel = nullptr;
    }
}

HWTEST_F_L0(HeapProfilerImplTest, DispatcherImplStopSampling)
{
    std::string result = "";
    std::function<void(const void*, const std::string &)> callback =
        [&result]([[maybe_unused]]const void* ptr, const std::string &temp) {result = temp;};
    ProtocolChannel *channel = new ProtocolHandler(callback, ecmaVm);
    auto tracing = std::make_unique<HeapProfilerImpl>(ecmaVm, channel);
    auto dispatcherImpl = std::make_unique<HeapProfilerImpl::DispatcherImpl>(channel, std::move(tracing));
    std::string msg = std::string() + R"({"id":0,"method":"HeapProfiler.stopSampling","params":{}})";
    DispatchRequest request(msg);
    dispatcherImpl->StopSampling(request);
    ASSERT_TRUE(result.find("StopSampling not support now") != std::string::npos);
    if (channel) {
        delete channel;
        channel = nullptr;
    }
}

HWTEST_F_L0(HeapProfilerImplTest, DispatcherImplTakeHeapSnapshot)
{
    std::string result = "";
    std::function<void(const void*, const std::string &)> callback =
        [&result]([[maybe_unused]]const void* ptr, const std::string &temp) {result = temp;};
    ProtocolChannel *channel = new ProtocolHandler(callback, ecmaVm);
    auto tracing = std::make_unique<HeapProfilerImpl>(ecmaVm, channel);
    auto dispatcherImpl = std::make_unique<HeapProfilerImpl::DispatcherImpl>(channel, std::move(tracing));
    std::string msg = std::string() + R"({"id":0,"method":"HeapProfiler.takeHeapSnapshot","params":{
        "reportProgress":10,
        "treatGlobalObjectsAsRoots":10,
        "captureNumericValue":10}})";
    DispatchRequest request(msg);
    dispatcherImpl->TakeHeapSnapshot(request);
    ASSERT_TRUE(result.find("wrong params") != std::string::npos);
    msg = std::string() + R"({"id":0,"method":"HeapProfiler.takeHeapSnapshot","params":{
        "reportProgress":true,
        "treatGlobalObjectsAsRoots":true,
        "captureNumericValue":true}})";
    DispatchRequest request1(msg);
    dispatcherImpl->TakeHeapSnapshot(request1);
    ASSERT_TRUE(result == "{\"id\":0,\"result\":{}}");
    if (channel) {
        delete channel;
        channel = nullptr;
    }
}
}  // namespace panda::test
