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

import Log from '../utils/log'
import CommonController from '../controller/commonController'
import account_osAccount from '@ohos.account.osAccount';

class UserAuthModel {
  protected TAG: string = "UserAuthModel"
  protected pinAuthManger : any
  protected userAuthManager : any
  private CMD_SET_SURFACE_ID = 100

  constructor() {
    Log.info(this.TAG, "constructor+")
    this.pinAuthManger = new account_osAccount.PINAuth()
    this.userAuthManager = new account_osAccount.UserAuth()
    Log.info(this.TAG, "constructor-")
  }

  async authPin(challenge: Uint8Array):Promise<Uint8Array> {
    Log.info(this.TAG, "auth Pin+")
    this.pinAuthManger.registerInputer({
      onGetData: (AuthSubType, IInputData) => {
        Log.info(this.TAG, 'FaceEnroll pin.registerInputer start')
        IInputData.onSetData(10000, new Uint8Array([ 49, 50, 51, 52, 53, 54 ]))
      }
    });

    let ret = new Promise<Uint8Array>((resolve)=> {
      this.userAuthManager.auth(challenge, 1, 10000, {
        onResult: (result, extraInfo) => {
          Log.info(this.TAG, 'authPin onResult: ' + result);
          this.pinAuthManger.unregisterInputer();
          if ((result == 0) && extraInfo.token && (Object.keys(extraInfo.token).length > 0) ) {
            resolve(extraInfo.token);
            Log.info(this.TAG, 'authPin success');
            setTimeout(()=>{CommonController.terminateAbility()}, 10 * 60 * 1000)
          } else {
            Log.error(this.TAG, 'authPin fail');
            resolve(new Uint8Array([]));
          }
        }
      })
    })
    Log.info(this.TAG, "auth Pin-")
    return ret
  }
}

let userAuthModel = new UserAuthModel();
export default userAuthModel as UserAuthModel