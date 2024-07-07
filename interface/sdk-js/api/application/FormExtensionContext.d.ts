/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License"),
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

import { AsyncCallback } from "../basic";
import ExtensionContext from "./ExtensionContext";
import formBindingData from '../@ohos.app.form.formBindingData';
import Want from '../@ohos.app.ability.Want';

/**
 * The context of form extension. It allows access to
 * formExtension-specific resources.
 * @syscap SystemCapability.Ability.Form
 * @StageModelOnly
 * @since 9
 */
export default class FormExtensionContext extends ExtensionContext {
    /**
     * Start an ability within the same bundle.
     * @param { Want } want - includes ability name, parameters and relative info sending to an ability.
     * @param { AsyncCallback<void> } callback - The callback of startAbility.
     * @throws { BusinessError } 202 - The application is not a system application.
     * @throws { BusinessError } 401 - If the input parameter is not valid parameter.
     * @throws { BusinessError } 16500050 - An IPC connection error happened.
     * @throws { BusinessError } 16500100 - Failed to obtain the configuration information.
     * @throws { BusinessError } 16500101 - The application is not a system application.
     * @throws { BusinessError } 16501000 - An internal functional error occurred.
     * @syscap SystemCapability.Ability.Form
     * @systemapi
     * @StageModelOnly
     * @since 9
     */
    startAbility(want: Want, callback: AsyncCallback<void>): void;

    /**
     * Start an ability within the same bundle.
     * @param { Want } want - includes ability name, parameters and relative info sending to an ability.
     * @returns { Promise<void> } The promise returned by the function.
     * @throws { BusinessError } 202 - The application is not a system application.
     * @throws { BusinessError } 401 - If the input parameter is not valid parameter.
     * @throws { BusinessError } 16500050 - An IPC connection error happened.
     * @throws { BusinessError } 16500100 - Failed to obtain the configuration information.
     * @throws { BusinessError } 16500101 - The application is not a system application.
     * @throws { BusinessError } 16501000 - An internal functional error occurred.
     * @syscap SystemCapability.Ability.Form
     * @systemapi
     * @StageModelOnly
     * @since 9
     */
    startAbility(want: Want): Promise<void>;
}
