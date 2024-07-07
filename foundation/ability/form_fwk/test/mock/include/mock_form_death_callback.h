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

#ifndef OHOS_FORM_FWK_MOCK_FORM_DEATH_CALLBACK_H
#define OHOS_FORM_FWK_MOCK_FORM_DEATH_CALLBACK_H

#include <codecvt>
#include <memory>
#include <singleton.h>
#include <thread_ex.h>
#include <unordered_map>
#include <vector>

#include "hilog_wrapper.h"
#include "event_handler.h"
#include "form_constants.h"
#include "form_callback_interface.h"
#include "form_mgr_stub.h"
#include "form_js_info.h"
#include "form_provider_data.h"
#include "ipc_types.h"
#include "iremote_object.h"

namespace OHOS {
namespace AppExecFwk {
/**
 * @class MockFormDeathCallback.
 * The MockFormDeathCallback for form mgr test.
 */
class MockFormDeathCallback : public FormCallbackInterface {
public:
    MockFormDeathCallback() = default;
    virtual ~MockFormDeathCallback() = default;

    void OnDeathReceived()
    {
        HILOG_INFO("%{public}s called.", __func__);
    }

    /**
     * @brief Update form.
     *
     * @param formJsInfo Indicates the obtained {@code FormJsInfo} instance.
     */
    void ProcessFormUpdate(const FormJsInfo &formJsInfo)
    {
        HILOG_INFO("%{public}s called.", __func__);
    }

    /**
     * @brief Uninstall form.
     *
     * @param formId Indicates the ID of the form to uninstall.
     */
    void ProcessFormUninstall(const int64_t formId)
    {
        HILOG_INFO("%{public}s called.", __func__);
    }

    /**
     * @brief Return error.
     *
     * @param errorCode Indicates error-code of the form.
     * @param errorMsg Indicates error-message of the form.
     */
    void OnError(const int32_t errorCode, const std::string &errorMsg)
    {
        HILOG_INFO("%{public}s called.", __func__);
    }
};
}  // namespace AppExecFwk
}  // namespace OHOS
#endif  // OHOS_FORM_FWK_MOCK_FORM_DEATH_CALLBACK_H
