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

#include <memory>

#include "gtest/gtest.h"
#define private public
#include "ans_inner_errors.h"
#include "distributed_notification_manager.h"

using namespace testing::ext;
extern void MockGetLocalDevice(int32_t mockRet);

namespace OHOS {
namespace Notification {
class DistributedNotificationManagerTest : public testing::Test {
public:
    void SetUp() override;
    void TearDown() override;

public:
    virtual void OnPublish(
        const std::string &deviceId, const std::string &bundleName, sptr<NotificationRequest> &request) {};
    virtual void OnUpdate(
        const std::string &deviceId, const std::string &bundleName, sptr<NotificationRequest> &request) {};
    virtual void OnDelete(
        const std::string &deviceId, const std::string &bundleName, const std::string &label, int32_t id) {};

protected:
    std::shared_ptr<DistributedNotificationManager> distributedManager_;
};

void DistributedNotificationManagerTest::SetUp()
{
    distributedManager_ = DistributedNotificationManager::GetInstance();
    distributedManager_->OnDeviceConnected("test");
    MockGetLocalDevice(0);
}

void DistributedNotificationManagerTest::TearDown()
{
    distributedManager_ = nullptr;
    DistributedNotificationManager::DestroyInstance();
}

/**
 * @tc.name      : Distributed_Publish_00100
 * @tc.number    : Distributed_Publish_00100
 * @tc.desc      : Publish a local notification to remote used distributed.
 */
HWTEST_F(DistributedNotificationManagerTest, Distributed_Publish_00100, Function | SmallTest | Level1)
{
    sptr<NotificationRequest> request = new NotificationRequest(1000);
    request->SetLabel("<label>");

    std::string bundleName = "<bundleName>";
    std::string label = request->GetLabel();
    int32_t id = request->GetNotificationId();

    EXPECT_EQ(distributedManager_->Publish(bundleName, label, id, request), ERR_OK);
}

/**
 * @tc.name      : Distributed_Publish_00200
 * @tc.number    : Distributed_Publish_00200
 * @tc.desc      : Publish a local notification to remote used distributed.
 */
HWTEST_F(DistributedNotificationManagerTest, Distributed_Publish_00200, Function | SmallTest | Level1)
{
    sptr<NotificationRequest> request = new NotificationRequest(1000);
    request->SetLabel("<label>");

    std::string bundleName = "<bundleName>";
    std::string label = request->GetLabel();
    int32_t id = request->GetNotificationId();
    MockGetLocalDevice(-1);

    EXPECT_EQ(distributedManager_->Publish(bundleName, label, id, request), ERR_ANS_DISTRIBUTED_GET_INFO_FAILED);
}

/**
 * @tc.name      : Distributed_Update_00100
 * @tc.number    : Distributed_Update_00100
 * @tc.desc      : Update a local notification to remote used distributed.
 */
HWTEST_F(DistributedNotificationManagerTest, Distributed_Update_00100, Function | SmallTest | Level1)
{
    sptr<NotificationRequest> request = new NotificationRequest(1000);
    request->SetLabel("<label>");

    std::string bundleName = "<bundleName>";
    std::string label = request->GetLabel();
    int32_t id = request->GetNotificationId();

    EXPECT_EQ(distributedManager_->Update(bundleName, label, id, request), ERR_OK);
}

/**
 * @tc.name      : Distributed_Update_00200
 * @tc.number    : Distributed_Update_00200
 * @tc.desc      : Update a local notification to remote used distributed.
 */
HWTEST_F(DistributedNotificationManagerTest, Distributed_Update_00200, Function | SmallTest | Level1)
{
    sptr<NotificationRequest> request = new NotificationRequest(1000);
    request->SetLabel("<label>");

    std::string bundleName = "<bundleName>";
    std::string label = request->GetLabel();
    int32_t id = request->GetNotificationId();
    MockGetLocalDevice(-1);

    EXPECT_EQ(distributedManager_->Update(bundleName, label, id, request), ERR_ANS_DISTRIBUTED_GET_INFO_FAILED);
}

/**
 * @tc.name      : Distributed_Delete_00100
 * @tc.number    : Distributed_Delete_00100
 * @tc.desc      : Delete a local notification to remote used distributed.
 */
HWTEST_F(DistributedNotificationManagerTest, Distributed_Delete_00100, Function | SmallTest | Level1)
{
    sptr<NotificationRequest> request = new NotificationRequest(1000);
    request->SetLabel("<label>");

    std::string bundleName = "<bundleName>";
    std::string label = request->GetLabel();
    int32_t id = request->GetNotificationId();

    EXPECT_EQ(distributedManager_->Delete(bundleName, label, id), ERR_OK);
}

/**
 * @tc.name      : Distributed_Delete_00200
 * @tc.number    : Distributed_Delete_00200
 * @tc.desc      : Delete a remote notification to remote used distributed.
 */
HWTEST_F(DistributedNotificationManagerTest, Distributed_Delete_00200, Function | SmallTest | Level1)
{
    sptr<NotificationRequest> request = new NotificationRequest(1000);
    request->SetLabel("<label>");

    std::string deviceId = "<remoteDeviceId>";
    std::string bundleName = "<bundleName>";
    std::string label = request->GetLabel();
    int32_t id = request->GetNotificationId();
    MockGetLocalDevice(-1);

    EXPECT_EQ(distributedManager_->DeleteRemoteNotification(deviceId, bundleName, label, id), ERR_OK);
}

/**
 * @tc.name      : Distributed_Delete_00300
 * @tc.number    : Distributed_Delete_00300
 * @tc.desc      : Delete a remote notification to remote used distributed.
 */
HWTEST_F(DistributedNotificationManagerTest, Distributed_Delete_00300, Function | SmallTest | Level1)
{
    sptr<NotificationRequest> request = new NotificationRequest(1000);
    request->SetLabel("<label>");

    std::string bundleName = "<bundleName>";
    std::string label = request->GetLabel();
    int32_t id = request->GetNotificationId();

    EXPECT_EQ(distributedManager_->Delete(bundleName, label, id), ERR_OK);
}

/**
 * @tc.name      : Distributed_Register_Manager_callback_00100
 * @tc.number    : Distributed_Register_Manager_callback_00100
 * @tc.desc      : Register callback to watch distributed database change.
 */
HWTEST_F(DistributedNotificationManagerTest, Distributed_Register_Manager_callback_00100, Function | SmallTest | Level1)
{
    DistributedNotificationManager::IDistributedCallback callback = {
        .OnPublish = std::bind(&DistributedNotificationManagerTest::OnPublish,
            this,
            std::placeholders::_1,
            std::placeholders::_2,
            std::placeholders::_3),
        .OnUpdate = std::bind(&DistributedNotificationManagerTest::OnUpdate,
            this,
            std::placeholders::_1,
            std::placeholders::_2,
            std::placeholders::_3),
        .OnDelete = std::bind(&DistributedNotificationManagerTest::OnDelete,
            this,
            std::placeholders::_1,
            std::placeholders::_2,
            std::placeholders::_3,
            std::placeholders::_4),
    };

    EXPECT_EQ(distributedManager_->RegisterCallback(callback), ERR_OK);
}

/**
 * @tc.name      : Distributed_Unregister_Manager_callback_00100
 * @tc.number    : Distributed_Unregister_Manager_callback_00100
 * @tc.desc      : Unregister callback to watch distributed database change.
 */
HWTEST_F(
    DistributedNotificationManagerTest, Distributed_Unregister_Manager_callback_00100, Function | SmallTest | Level1)
{
    EXPECT_EQ(distributedManager_->UngegisterCallback(), ERR_OK);
}

/**
 * @tc.name      : Distributed_Get_Current_Notification_00100
 * @tc.number    : Distributed_Get_Current_Notification_00100
 * @tc.desc      : Get current notification in distributed database.
 */
HWTEST_F(DistributedNotificationManagerTest, Distributed_Get_Current_Notification_00100, Function | SmallTest | Level1)
{
    std::vector<sptr<NotificationRequest>> requestList;
    EXPECT_EQ(distributedManager_->GetCurrentDistributedNotification(requestList), ERR_OK);
}

/**
 * @tc.name      : Distributed_Get_Local_DeviceInfo_00100
 * @tc.number    : Distributed_Get_Local_DeviceInfo_00100
 * @tc.desc      : Get local distributed device information.
 */
HWTEST_F(DistributedNotificationManagerTest, Distributed_Get_Local_DeviceInfo_00100, Function | SmallTest | Level1)
{
    DistributedDatabase::DeviceInfo deviceInfo;
    EXPECT_EQ(distributedManager_->GetLocalDeviceInfo(deviceInfo), ERR_OK);
}

/**
 * @tc.name      : Distributed_ResolveDistributedKey_00100
 * @tc.number    : Distributed_ResolveDistributedKey_00100
 * @tc.desc      : text ResolveDistributedKey function.
 */
HWTEST_F(DistributedNotificationManagerTest, Distributed_ResolveDistributedKey_00100, Function | SmallTest | Level1)
{
    std::string key("<key>");
    DistributedNotificationManager::ResolveKey resolveKey;
    EXPECT_EQ(distributedManager_->ResolveDistributedKey(key, resolveKey), false);
}

/**
 * @tc.name      : Distributed_ResolveDistributedKey_00200
 * @tc.number    : Distributed_ResolveDistributedKey_00200
 * @tc.desc      : text ResolveDistributedKey function.
 */
HWTEST_F(DistributedNotificationManagerTest, Distributed_ResolveDistributedKey_00200, Function | SmallTest | Level1)
{
    std::string key("deviceId|bundleName");
    DistributedNotificationManager::ResolveKey resolveKey;
    EXPECT_EQ(distributedManager_->ResolveDistributedKey(key, resolveKey), false);
}

/**
 * @tc.name      : Distributed_ResolveDistributedKey_00300
 * @tc.number    : Distributed_ResolveDistributedKey_00300
 * @tc.desc      : text ResolveDistributedKey function.
 */
HWTEST_F(DistributedNotificationManagerTest, Distributed_ResolveDistributedKey_00300, Function | SmallTest | Level1)
{
    std::string key("deviceId|bundleName|label");
    DistributedNotificationManager::ResolveKey resolveKey;
    EXPECT_EQ(distributedManager_->ResolveDistributedKey(key, resolveKey), false);
}

/**
 * @tc.name      : Distributed_ResolveDistributedKey_00400
 * @tc.number    : Distributed_ResolveDistributedKey_00400
 * @tc.desc      : text ResolveDistributedKey function.
 */
HWTEST_F(DistributedNotificationManagerTest, Distributed_ResolveDistributedKey_00400, Function | SmallTest | Level1)
{
    std::string key("deviceId|bundleName|label|0");
    DistributedNotificationManager::ResolveKey resolveKey;
    EXPECT_EQ(distributedManager_->ResolveDistributedKey(key, resolveKey), true);
}

/**
 * @tc.name      : Distributed_CheckDeviceId_00100
 * @tc.number    : Distributed_CheckDeviceId_00100
 * @tc.desc      : text CheckDeviceId function.
 */
HWTEST_F(DistributedNotificationManagerTest, Distributed_CheckDeviceId_00100, Function | SmallTest | Level1)
{
    std::string deviceId = "<remoteDeviceId>";
    std::string key("<key>");
    std::string value("<value>");
    // text OnDatabaseInsert function.
    distributedManager_->OnDatabaseInsert(deviceId, key, value);
    // text OnDatabaseUpdate function.
    distributedManager_->OnDatabaseUpdate(deviceId, key, value);
    // text OnDatabaseDelete function.
    distributedManager_->OnDatabaseDelete(deviceId, key, value);
    // text CheckDeviceId function.
    EXPECT_EQ(distributedManager_->CheckDeviceId(deviceId, key), false);
}

/**
 * @tc.name      : Distributed_CheckDeviceId_00200
 * @tc.number    : Distributed_CheckDeviceId_00200
 * @tc.desc      : text CheckDeviceId function.
 */
HWTEST_F(DistributedNotificationManagerTest, Distributed_CheckDeviceId_00200, Function | SmallTest | Level1)
{
    std::string deviceId = "deviceId";
    std::string key("deviceId|bundleName|label|0");
    std::string value("");
    // text OnDatabaseInsert function.
    distributedManager_->OnDatabaseInsert(deviceId, key, value);
    // text OnDatabaseUpdate function.
    distributedManager_->OnDatabaseUpdate(deviceId, key, value);
    // text OnDatabaseDelete function.
    distributedManager_->OnDatabaseDelete(deviceId, key, value);
    // text CheckDeviceId function.
    EXPECT_EQ(distributedManager_->CheckDeviceId(deviceId, key), true);
}

/**
 * @tc.name      : Distributed_OnDeviceDisconnected_00100
 * @tc.number    : Distributed_OnDeviceDisconnected_00100
 * @tc.desc      : text OnDeviceDisconnected function.
 */
HWTEST_F(DistributedNotificationManagerTest, Distributed_OnDeviceDisconnected_00100, Function | SmallTest | Level1)
{
    const std::string deviceId = "<remoteDeviceId>";
    // text OnDeviceDisconnected function.
    distributedManager_->OnDeviceDisconnected(deviceId);
    // text PublishCallback function.
    const std::string bundleName = "<bundleName>";
    sptr<NotificationRequest> request = new NotificationRequest(1);
    EXPECT_EQ(distributedManager_->PublishCallback(deviceId, bundleName, request), true);
}

/**
 * @tc.name      : Distributed_UpdateCallback_00100
 * @tc.number    : Distributed_UpdateCallback_00100
 * @tc.desc      : text UpdateCallback function.
 */
HWTEST_F(DistributedNotificationManagerTest, Distributed_UpdateCallback_00100, Function | SmallTest | Level1)
{
    std::string deviceId = "<remoteDeviceId>";
    std::string bundleName = "<bundleName>";
    sptr<NotificationRequest> request = new NotificationRequest(1000);
    EXPECT_EQ(distributedManager_->UpdateCallback(deviceId, bundleName, request), true);
}

/**
 * @tc.name      : Distributed_DeleteCallback_00100
 * @tc.number    : Distributed_DeleteCallback_00100
 * @tc.desc      : text DeleteCallback function.
 */
HWTEST_F(DistributedNotificationManagerTest, Distributed_DeleteCallback_00100, Function | SmallTest | Level1)
{
    sptr<NotificationRequest> request = new NotificationRequest(1000);
    request->SetLabel("<label>");

    std::string deviceId = "<remoteDeviceId>";
    std::string bundleName = "<bundleName>";
    std::string label = request->GetLabel();
    int32_t id = request->GetNotificationId();
    EXPECT_EQ(distributedManager_->DeleteCallback(deviceId, bundleName, label, id), true);
}

/**
 * @tc.name      : Distributed_OnDistributedKvStoreDeathRecipient_00100
 * @tc.number    : Distributed_OnDistributedKvStoreDeathRecipient_00100
 * @tc.desc      : text OnDistributedKvStoreDeathRecipient function.
 */
HWTEST_F(DistributedNotificationManagerTest, Distributed_OnDistributedKvStoreDeathRecipient_00100,
         Function | SmallTest | Level1)
{
    EXPECT_EQ(distributedManager_->OnDistributedKvStoreDeathRecipient(), ERR_OK);
}
}  // namespace Notification
}  // namespace OHOS