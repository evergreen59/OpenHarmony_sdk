/*
 * Copyright (c) 2020-2021 Huawei Device Co., Ltd.
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

#include "components/ui_view_group.h"
#include <climits>
#include <gtest/gtest.h>

using namespace testing::ext;
namespace OHOS {
class UIViewGroupTest : public testing::Test {
public:
    UIViewGroupTest() : viewGroup_(nullptr) {}
    ~UIViewGroupTest() {}
    static void SetUpTestCase(void) {}
    static void TearDownTestCase(void) {}
    void SetUp(void);
    void TearDown(void);
    UIViewGroup* viewGroup_;
};

void UIViewGroupTest::SetUp(void)
{
    if (viewGroup_ == nullptr) {
        viewGroup_ = new UIViewGroup();
    }
}

void UIViewGroupTest::TearDown(void)
{
    if (viewGroup_ != nullptr) {
        delete viewGroup_;
        viewGroup_ = nullptr;
    }
}

/**
 * @tc.name: UIViewGroupAdd_001
 * @tc.desc: Verify Add function.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQF
 */
HWTEST_F(UIViewGroupTest, UIViewGroupAdd_001, TestSize.Level0)
{
    if (viewGroup_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    UIView* view = new UIView();
    if (view == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    viewGroup_->Add(view);
    EXPECT_EQ(view, viewGroup_->GetChildrenHead());
    EXPECT_EQ(view->GetParent(), viewGroup_);

    delete view;
}

/**
 * @tc.name: UIViewGroupInsert_001
 * @tc.desc: Verify Insert function.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQF
 */
HWTEST_F(UIViewGroupTest, UIViewGroupInsert_001, TestSize.Level0)
{
    if (viewGroup_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    UIView* preView = new UIView();
    if (preView == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    UIView* view = new UIView();

    viewGroup_->Add(preView);
    viewGroup_->Insert(preView, view);
    EXPECT_EQ(view, preView->GetNextSibling());

    delete preView;
    delete view;
}

/**
 * @tc.name: UIViewGroupRemove_001
 * @tc.desc: Verify Remove function.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQF
 */
HWTEST_F(UIViewGroupTest, UIViewGroupRemove_001, TestSize.Level0)
{
    if (viewGroup_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    UIView* view = new UIView();
    if (view == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    viewGroup_->Add(view);
    viewGroup_->Remove(view);
    EXPECT_EQ(nullptr, viewGroup_->GetChildrenHead());
    EXPECT_EQ(nullptr, view->GetParent());

    delete view;
}

/**
 * @tc.name: UIViewGroupRemoveAll_001
 * @tc.desc: Verify RemoveAll function.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQF
 */
HWTEST_F(UIViewGroupTest, UIViewGroupRemoveAll_001, TestSize.Level0)
{
    if (viewGroup_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    UIView* view = new UIView();
    UIView* view2 = new UIView();

    viewGroup_->Add(view);
    viewGroup_->RemoveAll();
    EXPECT_EQ(nullptr, viewGroup_->GetChildrenHead());

    delete view;
    delete view2;
}

/**
 * @tc.name: UIViewGroupGetViewType_001
 * @tc.desc: Verify GetViewType function.
 * @tc.type: FUNC
 * @tc.require: AR000EEMQF
 */
HWTEST_F(UIViewGroupTest, UIViewGroupGetViewType_001, TestSize.Level1)
{
    if (viewGroup_ == nullptr) {
        EXPECT_EQ(1, 0);
        return;
    }
    EXPECT_EQ(viewGroup_->GetViewType(), UI_VIEW_GROUP);
}

/**
 * @tc.name: Graphic_UIView_Test_GetChildrenRenderHead_001
 * @tc.desc: Check the child render head
 * @tc.type: FUNC
 * @tc.require:issueI5AD8G
 */
HWTEST_F(UIViewGroupTest, Graphic_UIView_Test_GetChildrenRenderHead_001, TestSize.Level0)
{
    UIViewGroup* viewGroup = new UIViewGroup();
    UIView* sibling = viewGroup->GetChildrenRenderHead();
    if (sibling != nullptr) {
        EXPECT_NE(0, 0);
        return;
    }
    EXPECT_EQ(0, 0);
    delete viewGroup;
}

/**
 * @tc.name: Graphic_UIView_Test_GetChildrenRenderHead_001
 * @tc.desc: Check child render head
 * @tc.type: FUNC
 * @tc.require:issueI5AD8G
 */
HWTEST_F(UIViewGroupTest, Graphic_UIView_Test_GetChildrenRenderHead_002, TestSize.Level0)
{
    UIView* view1 = new UIView();
    UIView* view2 = new UIView();
    UIView* view3 = new UIView();
    UIViewGroup* viewGroup = new UIViewGroup();
    viewGroup->Add(view1);
    // check child head
    EXPECT_EQ(viewGroup->GetChildrenRenderHead(), view1);

    viewGroup->Add(view2);
    // check child head with two child
    EXPECT_EQ(viewGroup->GetChildrenRenderHead(), view1);

    viewGroup->Remove(view1);
    // check child head when remove head
    EXPECT_EQ(viewGroup->GetChildrenRenderHead(), view2);

    viewGroup->Remove(view2);
    // check child head when remove all
    EXPECT_EQ(viewGroup->GetChildrenRenderHead(), nullptr);

    viewGroup->Add(view1);
    viewGroup->Add(view2);
    viewGroup->RemoveAll();
    // check child head when remove all
    EXPECT_EQ(viewGroup->GetChildrenRenderHead(), nullptr);

    viewGroup->Add(view1);
    viewGroup->Add(view2);
    viewGroup->Insert(view1, view3);
    viewGroup->Remove(view1);
    // check child head when insert view
    EXPECT_EQ(viewGroup->GetChildrenRenderHead(), view3);

    delete view1;
    delete view2;
    delete view3;
    delete viewGroup;
}

/**
 * @tc.name: Graphic_UIView_Test_GetChildrenRenderHead_003
 * @tc.desc: Check child render head when zIndex change
 * @tc.type: FUNC
 * @tc.require:issueI5AD8G
 */
HWTEST_F(UIViewGroupTest, Graphic_UIView_Test_GetChildrenRenderHead_003, TestSize.Level0)
{
    UIView* view1 = new UIView();
    UIView* view2 = new UIView();
    UIView* view3 = new UIView();
    UIView* view4 = new UIView();
    UIViewGroup* viewGroup = new UIViewGroup();
    viewGroup->Add(view1);
    viewGroup->Add(view2);
    viewGroup->Add(view3);
    viewGroup->Add(view4);
    // check child with zIndex = 0
    EXPECT_EQ(viewGroup->GetChildrenRenderHead(), view1);

    view1->SetZIndex(10); // 10: zindex
    // check child when zIndex change
    EXPECT_EQ(viewGroup->GetChildrenRenderHead(), view2);

    view3->SetZIndex(-10); // -10: zindex
    // check child when zIndex change
    EXPECT_EQ(viewGroup->GetChildrenRenderHead(), view3);

    delete view1;
    delete view2;
    delete view3;
    delete view4;
    delete viewGroup;
}

/**
 * @tc.name: Graphic_UIView_Test_GetChildrenRenderHead_004
 * @tc.desc: Check child render head when zIndex change
 * @tc.type: FUNC
 * @tc.require:issueI5AD8G
 */
HWTEST_F(UIViewGroupTest, Graphic_UIView_Test_GetChildrenRenderHead_004, TestSize.Level0)
{
    UIView* view1 = new UIView();
    UIView* view2 = new UIView();
    UIView* view3 = new UIView();
    UIView* view4 = new UIView();
    UIViewGroup* viewGroup = new UIViewGroup();
    viewGroup->Add(view1);
    viewGroup->Add(view2);
    viewGroup->Add(view3);
    viewGroup->Add(view4);
    // check child with zIndex = 0
    EXPECT_EQ(viewGroup->GetChildrenRenderHead(), view1);
    EXPECT_EQ(view1->GetNextRenderSibling(), view2);
    EXPECT_EQ(view2->GetNextRenderSibling(), view3);
    EXPECT_EQ(view3->GetNextRenderSibling(), view4);

    view1->SetZIndex(10); // 10: zindex
    // check child when zIndex change
    EXPECT_EQ(viewGroup->GetChildrenRenderHead(), view2);
    EXPECT_EQ(view2->GetNextRenderSibling(), view3);
    EXPECT_EQ(view3->GetNextRenderSibling(), view4);
    EXPECT_EQ(view4->GetNextRenderSibling(), view1);

    view3->SetZIndex(-10); // -10: zindex
    // check child when zIndex change
    EXPECT_EQ(viewGroup->GetChildrenRenderHead(), view3);
    EXPECT_EQ(view3->GetNextRenderSibling(), view2);
    EXPECT_EQ(view2->GetNextRenderSibling(), view4);
    EXPECT_EQ(view4->GetNextRenderSibling(), view1);

    viewGroup->Remove(view3);
    // check child when zIndex change
    EXPECT_EQ(viewGroup->GetChildrenRenderHead(), view2);
    EXPECT_EQ(view2->GetNextRenderSibling(), view4);
    EXPECT_EQ(view4->GetNextRenderSibling(), view1);

    delete view1;
    delete view2;
    delete view3;
    delete view4;
    delete viewGroup;
}

/**
 * @tc.name: Graphic_UIView_Test_SetChildrenRenderHead_001
 * @tc.desc: Check child render head
 * @tc.type: FUNC
 * @tc.require:issueI5AD8G
 */
HWTEST_F(UIViewGroupTest, Graphic_UIView_Test_SetChildrenRenderHead_001, TestSize.Level0)
{
    UIView* view1 = new UIView();
    UIViewGroup* viewGroup = new UIViewGroup();

    // check default child head
    EXPECT_EQ(viewGroup->GetChildrenRenderHead(), nullptr);

    viewGroup->SetChildrenRenderHead(view1);
    // can not set child head if it is not child
    EXPECT_EQ(viewGroup->GetChildrenRenderHead(), nullptr);

    viewGroup->Add(view1);
    // check child head
    EXPECT_EQ(viewGroup->GetChildrenRenderHead(), view1);

    delete view1;
    delete viewGroup;
}

/**
 * @tc.name: Graphic_UIView_Test_UpdateRenderView_001
 * @tc.desc: check update render view
 * @tc.type: FUNC
 * @tc.require:issueI5AD8G
 */
HWTEST_F(UIViewGroupTest, Graphic_UIView_Test_UpdateRenderView_001, TestSize.Level0)
{
    UIView* view1 = new UIView();
    UIView* view2 = new UIView();
    UIViewGroup* viewGroup = new UIViewGroup();
    viewGroup->Add(view1);
    viewGroup->Add(view2);
    viewGroup->UpdateRenderView(view2);
    // check child head when remove all
    EXPECT_EQ(viewGroup->GetChildrenRenderHead(), view1);
    // check child head when insert view
    EXPECT_EQ(view1->GetNextRenderSibling(), view2);

    delete view1;
    delete view2;
    delete viewGroup;
}
} // namespace OHOS
