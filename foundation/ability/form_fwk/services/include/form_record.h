/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License")_;
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

#ifndef OHOS_FORM_FWK_FORM_RECORD_H
#define OHOS_FORM_FWK_FORM_RECORD_H

#include <vector>
#include "form_info_base.h"
#include "form_provider_info.h"

namespace OHOS {
namespace AppExecFwk {
/**
 * @class FormRecord
 * Form data.
 */
class FormRecord {
public:
    int64_t formId = 0;
    bool isInited = false;
    bool needFreeInstall = false;
    bool versionUpgrade = false;
    bool needRefresh = false;
    bool isCountTimerRefresh = false;
    std::string packageName = "";
    std::string bundleName = "";
    std::string moduleName = "";
    std::string abilityName = "";
    std::string formName = "";
    int32_t specification;
    bool isEnableUpdate = false;
    bool isVisible = false;
    int64_t updateDuration = 0;
    int updateAtHour = -1;
    int updateAtMin = -1;
    FormProviderInfo formProviderInfo;
    std::vector<std::string> hapSourceDirs;
    std::string jsFormCodePath = "";
    bool formTempFlag = false;
    std::vector<int> formUserUids;
    bool formVisibleNotify = false;
    int formVisibleNotifyState = 0;
    int32_t userId = 0;
    std::string formSrc = "";
    FormWindow formWindow;
    uint32_t versionCode = 0;
    std::string versionName = "";
    uint32_t compatibleVersion = 0;
    FormType type = FormType::JS;
    FormType uiSyntax = FormType::JS;
    bool isTimerRefresh = false;
};
}  // namespace AppExecFwk
}  // namespace OHOS

#endif // OHOS_FORM_FWK_FORM_RECORD_H
