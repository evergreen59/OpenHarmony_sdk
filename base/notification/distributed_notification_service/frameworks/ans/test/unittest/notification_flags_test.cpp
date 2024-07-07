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

#include <gtest/gtest.h>

#define private public
#define protected public
#include "notification_flags.h"
#undef private
#undef protected

using namespace testing::ext;
namespace OHOS {
namespace Notification {
class NotificationFlagsTest : public testing::Test {
public:
    static void SetUpTestCase() {}
    static void TearDownTestCase() {}
    void SetUp() {}
    void TearDown() {}
};

/**
 * @tc.name: SetSoundEnabled_00001
 * @tc.desc: Test SetSoundEnabled parameters.
 * @tc.type: FUNC
 * @tc.require: issueI5WBBH
 */
HWTEST_F(NotificationFlagsTest, SetSoundEnabled_00001, Function | SmallTest | Level1)
{
    NotificationConstant::FlagStatus sound =NotificationConstant::FlagStatus(1);
    auto rrc = std::make_shared<NotificationFlags>();
    rrc->SetSoundEnabled(sound);
    EXPECT_EQ(rrc->IsSoundEnabled(), sound);
}

/**
 * @tc.name: SetVibrationEnabled_00001
 * @tc.desc: Test SetVibrationEnabled parameters.
 * @tc.type: FUNC
 * @tc.require: issueI5WBBH
 */
HWTEST_F(NotificationFlagsTest, SetVibrationEnabled_00001, Function | SmallTest | Level1)
{
    NotificationConstant::FlagStatus vibrationEnabled =NotificationConstant::FlagStatus(1);
    auto rrc = std::make_shared<NotificationFlags>();
    rrc->SetVibrationEnabled(vibrationEnabled);
    EXPECT_EQ(rrc->IsVibrationEnabled(), vibrationEnabled);
}

/**
 * @tc.name: Dump_00001
 * @tc.desc: Test Dump parameters.
 * @tc.type: FUNC
 * @tc.require: issueI5WBBH
 */
HWTEST_F(NotificationFlagsTest, Dump_00001, Function | SmallTest | Level1)
{
    auto rrc = std::make_shared<NotificationFlags>();
    std::string ret = "soundEnabled = 0, vibrationEnabled = 0";
    EXPECT_EQ(rrc->Dump(), ret);
}

/**
 * @tc.name: ToJson_00001
 * @tc.desc: Test ToJson parameters.
 * @tc.type: FUNC
 * @tc.require: issueI5WBBH
 */
HWTEST_F(NotificationFlagsTest, ToJson_00001, Function | SmallTest | Level1)
{
    nlohmann::json jsonObject;
    auto rrc = std::make_shared<NotificationFlags>();
    rrc->FromJson(jsonObject);
    EXPECT_EQ(rrc->ToJson(jsonObject), true);
}

/**
 * @tc.name: Marshalling_00001
 * @tc.desc: Test Marshalling parameters.
 * @tc.type: FUNC
 * @tc.require: issueI5WBBH
 */
HWTEST_F(NotificationFlagsTest, Marshalling_00001, Function | SmallTest | Level1)
{
    Parcel parcel;
    auto rrc = std::make_shared<NotificationFlags>();
    EXPECT_EQ(rrc->Marshalling(parcel), true);
}

/**
 * @tc.name: Unmarshalling_00001
 * @tc.desc: Test Unmarshalling parameters.
 * @tc.type: FUNC
 * @tc.require: issueI5WBBH
 */
HWTEST_F(NotificationFlagsTest, Unmarshalling_001, Function | SmallTest | Level1)
{
    bool unmarshalling = true;
    Parcel parcel;
    std::shared_ptr<NotificationFlags> result =
    std::make_shared<NotificationFlags>();

    if (nullptr != result) {
        if (nullptr == result->Unmarshalling(parcel)) {
            unmarshalling = false;
        }
    }
    EXPECT_EQ(unmarshalling, true);
}

/**
 * @tc.name: ReadFromParcel_00001
 * @tc.desc: Test ReadFromParcel parameters.
 * @tc.type: FUNC
 * @tc.require: issueI5WBBH
 */
HWTEST_F(NotificationFlagsTest, ReadFromParcel_00001, Function | SmallTest | Level1)
{
    Parcel parcel;
    auto rrc = std::make_shared<NotificationFlags>();
    EXPECT_EQ(rrc->ReadFromParcel(parcel), true);
}
}
}