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

#include "ability_impl.h"
#include <gtest/gtest.h>
#include "ability_local_record.h"
#include "hilog_wrapper.h"
#include "ohos_application.h"

namespace OHOS {
namespace AppExecFwk {
void AbilityImpl::Init(std::shared_ptr<OHOSApplication> &application, const std::shared_ptr<AbilityLocalRecord> &record,
    std::shared_ptr<Ability> &ability, std::shared_ptr<AbilityHandler> &handler, const sptr<IRemoteObject> &token,
    std::shared_ptr<ContextDeal> &contextDeal)
{
    HILOG_INFO("AbilityImpl::init begin");

    if (token == nullptr) {
        HILOG_ERROR("AbilityImpl::init failed, token is nullptr");
        return;
    }

    if (application == nullptr) {
        HILOG_ERROR("AbilityImpl::init failed, application is nullptr");
        return;
    }

    if (record == nullptr) {
        HILOG_ERROR("AbilityImpl::init failed, record is nullptr");
        return;
    }

    if (ability == nullptr) {
        HILOG_ERROR("AbilityImpl::init failed, ability is nullptr");
        return;
    }

    if (token == nullptr) {
        HILOG_ERROR("AbilityImpl::init failed, token is nullptr");
        return;
    }

    if (contextDeal == nullptr) {
        HILOG_ERROR("AbilityImpl::init failed, contextDeal is nullptr");
        return;
    }

    token_ = record->GetToken();
    record->SetAbilityImpl(shared_from_this());
    ability_ = ability;
    ability_->Init(record->GetAbilityInfo(), application, handler, token);
    lifecycleState_ = AAFwk::ABILITY_STATE_INITIAL;
    abilityLifecycleCallbacks_ = application;
    contextDeal_ = contextDeal;
    HILOG_INFO("AbilityImpl::init end");
}

void AbilityImpl::Start(const Want &want)
{
    GTEST_LOG_(INFO) << "Mock AbilityImpl::Start called";
}

void AbilityImpl::Stop()
{
    GTEST_LOG_(INFO) << "Mock AbilityImpl::Stop called";
}

void AbilityImpl::Active()
{
    GTEST_LOG_(INFO) << "Mock AbilityImpl::Active called";
}

void AbilityImpl::Inactive()
{
    GTEST_LOG_(INFO) << "Mock AbilityImpl::Inactive called";
}

void AbilityImpl::DispatchSaveAbilityState()
{
    GTEST_LOG_(INFO) << "Mock AbilityImpl::DispatchSaveAbilityState called";
}

void AbilityImpl::DispatchRestoreAbilityState(const PacMap &inState)
{
    GTEST_LOG_(INFO) << "Mock AbilityImpl::DispatchRestoreAbilityState called";
}

void AbilityImpl::HandleAbilityTransaction(const Want &want, const AAFwk::LifeCycleStateInfo &targetState)
{
    GTEST_LOG_(INFO) << "Mock AbilityImpl::HandleAbilityTransaction called";
}

sptr<IRemoteObject> AbilityImpl::ConnectAbility(const Want &want)
{
    GTEST_LOG_(INFO) << "Mock AbilityImpl::ConnectAbility called";
    return nullptr;
}

void AbilityImpl::DisconnectAbility(const Want &want)
{
    GTEST_LOG_(INFO) << "Mock AbilityImpl::DisconnectAbility called";
}

void AbilityImpl::CommandAbility(const Want &want, bool restart, int startId)
{
    GTEST_LOG_(INFO) << "Mock AbilityImpl::CommandAbility called";
}

int AbilityImpl::GetCurrentState()
{
    return lifecycleState_;
}

void AbilityImpl::SendResult(int requestCode, int resultCode, const Want &resultData)
{
    GTEST_LOG_(INFO) << "Mock AbilityImpl::SendResult called";
}

void AbilityImpl::NewWant(const Want &want)
{
    GTEST_LOG_(INFO) << "Mock AbilityImpl::NewWant called";
    if (ability_ == nullptr) {
        HILOG_ERROR("AbilityImpl::NewWant ability_ is nullptr");
        return;
    }
    ability_->SetWant(want);
    ability_->OnNewWant(want);
#ifdef SUPPORT_GRAPHICS
    ability_->ContinuationRestore(want);
#endif
}

std::vector<std::string> AbilityImpl::GetFileTypes(const Uri &uri, const std::string &mimeTypeFilter)
{
    std::vector<std::string> value;
    value.push_back(mimeTypeFilter);
    GTEST_LOG_(INFO) << "Mock AbilityImpl::GetFileTypes called";
    return value;
}

int AbilityImpl::OpenFile(const Uri &uri, const std::string &mode)
{
    GTEST_LOG_(INFO) << "Mock AbilityImpl::OpenFile called";
    return 1;
}

int AbilityImpl::Insert(const Uri &uri, const NativeRdb::ValuesBucket &value)
{
    GTEST_LOG_(INFO) << "Mock AbilityImpl::Insert called";
    return 1;
}

int AbilityImpl::Update(const Uri &uri, const NativeRdb::ValuesBucket &value,
    const NativeRdb::DataAbilityPredicates &predicates)
{
    GTEST_LOG_(INFO) << "Mock AbilityImpl::Update called";
    return 1;
}

int AbilityImpl::Delete(const Uri &uri, const NativeRdb::DataAbilityPredicates &predicates)
{
    GTEST_LOG_(INFO) << "Mock AbilityImpl::Delete called";
    return 1;
}

std::shared_ptr<NativeRdb::AbsSharedResultSet> AbilityImpl::Query(
    const Uri &uri, std::vector<std::string> &columns, const NativeRdb::DataAbilityPredicates &predicates)
{
    GTEST_LOG_(INFO) << "Mock AbilityImpl::Query called";
    return nullptr;
}

std::string AbilityImpl::GetType(const Uri &uri)
{
    GTEST_LOG_(INFO) << "Mock AbilityImpl::GetType called";
    return "";
}

#ifdef SUPPORT_GRAPHICS
void AbilityImpl::Foreground(const Want &want)
{
    GTEST_LOG_(INFO) << "Mock AbilityImpl::Foreground called";
}

void AbilityImpl::Background()
{
    GTEST_LOG_(INFO) << "Mock AbilityImpl::Background called";
}

void AbilityImpl::DoKeyDown(const std::shared_ptr<MMI::KeyEvent>& keyEvent) {}

void AbilityImpl::DoKeyUp(const std::shared_ptr<MMI::KeyEvent>& keyEvent) {}

void AbilityImpl::DoPointerEvent(std::shared_ptr<MMI::PointerEvent>& pointerEvent) {}
#endif
}  // namespace AppExecFwk
}  // namespace OHOS
