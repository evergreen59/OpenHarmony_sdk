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

#ifndef MOCK_OHOS_ABILITY_RUNTIME_MOCK_LIFECYCLE_OBSERVER_H
#define MOCK_OHOS_ABILITY_RUNTIME_MOCK_LIFECYCLE_OBSERVER_H

#include "want.h"
#include "ability_lifecycle_observer_interface.h"
#include <gtest/gtest.h>

namespace OHOS {
namespace AppExecFwk {
using Want = OHOS::AAFwk::Want;

class MockLifecycleObserver : public ILifecycleObserver {
public:
    MockLifecycleObserver() = default;
    virtual ~MockLifecycleObserver() = default;

    /**
     * Called back in response to an ON_ACTIVE event.
     * When an ON_ACTIVE event is received, the ability or ability slice is in the foreground and is interactive.
     */
    virtual void OnActive()
    {
        GTEST_LOG_(INFO) << "MockLifecycleObserver::OnActive called";
    }

    /**
     * Called back in response to an ON_INACTIVE event.
     * When an ON_INACTIVE event is received, the ability or ability slice is in the INACTIVE state. INACTIVE is an
     * intermediate state before the state changes to ACTIVE or BACKGROUND. In this state, the UI may be visible but is
     * not interactive. You are advised not to use this method to invoke complex service logic.
     *
     */
    virtual void OnInactive()
    {
        GTEST_LOG_(INFO) << "MockLifecycleObserver::OnInactive called";
    }

    /**
     * Called back in response to an ON_START event, where the startup information
     * is carried in the want parameter.
     * This method initializes an Ability or AbilitySlice and is called back only once during the entire lifecycle.
     * You are advised to implement some initialization logic using this method, for example, you can initialize a
     * timer or define some global objects.
     *
     * @param want Indicates the startup information.
     */
    virtual void OnStart(const Want &want)
    {
        (void)want;
        GTEST_LOG_(INFO) << "MockLifecycleObserver::OnStart called";
    }

    /**
     * Called back in response to an ON_STOP event.
     * This method is called back when the lifecycle of the ability or ability slice is destroyed. You can reclaim
     * resources using this method.
     *
     */
    virtual void OnStop()
    {
        GTEST_LOG_(INFO) << "MockLifecycleObserver::OnStop called";
    }

    /**
     * @brief Called back in response to a lifecycle change. This method is triggered by a registered LifecycleObserver
     * each time the lifecycle state changes.
     *
     * @param event Indicates the lifecycle event.
     * @param want Indicates the state change information.
     */
    virtual void OnStateChanged(LifeCycle::Event event, const Want &want)
    {
        (void)event;
        (void)want;
        GTEST_LOG_(INFO) << "MockLifecycleObserver::OnStateChanged called";
    }

    /**
     * @brief Called back in response to a lifecycle change. This method is triggered by a registered LifecycleObserver
     * each time the lifecycle state changes.
     *
     * @param event Indicates the lifecycle event.
     */
    virtual void OnStateChanged(LifeCycle::Event event)
    {
        (void)event;
        GTEST_LOG_(INFO) << "MockLifecycleObserver::OnStateChanged called";
    }

#ifdef SUPPORT_GRAPHICS
    /**
     * Called back in response to an ON_BACKGROUND event.
     * When an ON_BACKGROUND event is received, the ability or ability slice is invisible. You are advised to
     * suspend the threads related to this ability or ability slice and clear resources for more system memory.
     *
     */

    virtual void OnBackground()
    {
        GTEST_LOG_(INFO) << "MockLifecycleObserver::OnBackground called";
    }

    /**
     * Called back in response to an ON_FOREGROUND event, where information for the
     * ability or ability slice to go back to the ACTIVE state is carried in the want parameter.
     * When an ON_FOREGROUND event is received, the ability or ability slice returns to the foreground. You can use
     * this method to implement re-initialization or adjust the UI display by using the want parameter.
     *
     * @param want Indicates the information for the ability or ability slice to go back to the ACTIVE state.
     */
    virtual void OnForeground(const Want &want)
    {
        (void)want;
        GTEST_LOG_(INFO) << "MockLifecycleObserver::OnForeground called";
    }
#endif
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // MOCK_OHOS_ABILITY_RUNTIME_MOCK_LIFECYCLE_OBSERVER_H
