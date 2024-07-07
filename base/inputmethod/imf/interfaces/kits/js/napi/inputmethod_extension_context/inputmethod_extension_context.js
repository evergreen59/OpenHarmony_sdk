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

let ExtensionContext = requireNapi('application.ExtensionContext');

class InputMethodExtensionContext extends ExtensionContext {
  constructor(obj) {
    super(obj);
    this.extensionAbilityInfo = obj.extensionAbilityInfo;
  }

  startAbility(want, options, callback) {
    console.log('startAbility');
    return this.__context_impl__.startAbility(want, options, callback);
  }

  connectAbility(want, options) {
    console.log('connectAbility');
    return this.__context_impl__.connectAbility(want, options);
  }

  startAbilityWithAccount(want, accountId, options, callback) {
    console.log('startAbilityWithAccount');
    return this.__context_impl__.startAbilityWithAccount(want, accountId, options, callback);
  }

  connectAbilityWithAccount(want, accountId, options) {
    console.log('connectAbilityWithAccount');
    return this.__context_impl__.connectAbilityWithAccount(want, accountId, options);
  }

  disconnectAbility(connection, callback) {
    console.log('disconnectAbility');
    return this.__context_impl__.disconnectAbility(connection, callback);
  }

  terminateSelf(callback) {
    console.log('terminateSelf');
    return this.__context_impl__.terminateSelf(callback);
  }

  destroy(callback) {
    console.log('destroy');
    return this.__context_impl__.destroy(callback);
  }
}

export default InputMethodExtensionContext;