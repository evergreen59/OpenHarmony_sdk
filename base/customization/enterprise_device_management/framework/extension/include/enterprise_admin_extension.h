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

#ifndef FRAMEWORK_EXTENSION_INCLUDE_ENTERPRISE_ADMIN_EXTENSION_H
#define FRAMEWORK_EXTENSION_INCLUDE_ENTERPRISE_ADMIN_EXTENSION_H

#include "enterprise_admin_extension_context.h"
#include "extension_base.h"
#include "extension_context.h"

namespace OHOS {
namespace EDM {
class EnterpriseAdminExtension : public AbilityRuntime::ExtensionBase<EnterpriseAdminExtensionContext> {
public:
    EnterpriseAdminExtension() = default;
    virtual ~EnterpriseAdminExtension() = default;

    /**
     * @brief Create and init context.
     *
     * @param record The record info.
     * @param application The application info.
     * @param handler The handler.
     * @param token The remote object token.
     * @return The context
     */
    std::shared_ptr<EnterpriseAdminExtensionContext> CreateAndInitContext(
        const std::shared_ptr<AppExecFwk::AbilityLocalRecord>& record,
        const std::shared_ptr<AppExecFwk::OHOSApplication>& application,
        std::shared_ptr<AppExecFwk::AbilityHandler>& handler,
        const sptr<IRemoteObject>& token) override;

    /**
     * @brief The Init.
     *
     * @param record The record.
     * @param application The application.
     * @param handler The handler.
     * @param token The remmote object token.
     */
    void Init(const std::shared_ptr<AppExecFwk::AbilityLocalRecord>& record,
        const std::shared_ptr<AppExecFwk::OHOSApplication>& application,
        std::shared_ptr<AppExecFwk::AbilityHandler>& handler,
        const sptr<IRemoteObject>& token) override;

    static EnterpriseAdminExtension* Create(const std::unique_ptr<AbilityRuntime::Runtime>& runtime);
};
}  // namespace EDM
}  // namespace OHOS
#endif  // FRAMEWORK_EXTENSION_INCLUDE_ENTERPRISE_ADMIN_EXTENSION_H