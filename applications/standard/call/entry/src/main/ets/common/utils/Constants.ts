/**
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

/**
 *  constants package tool class
 */
export class Constants {
  public APP_KEY_CALLDATAMANGER_MANAGER: string = "CallDataManager";
  public CALL_BUNDLE_NAME = 'com.ohos.callui';
  public CALL_ABILITY_NAME = 'com.ohos.callui.MainAbility';
  public GLOBALTHIS_CALLMANAGER = 'callManager';
  public GLOBALTHIS_ABILITY_WANT = 'abilityWant';
  public GLOBALTHIS_CONTEXT = 'calluiAbilityContext';
  public GLOBALTHIS_APPINACTIVE_STATE = 'appInactiveState';
}

let mConstants = new Constants();

export default mConstants as Constants;