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

import certManagerModel from '../model/CertMangerModel'
import { CMModelErrorCode, CMModelOptType } from '../model/CertMangerModel'
import { CredentialAbstractVo } from '../model/CertManagerVo/CredentialAbstractVo'
import { CredentialVo } from '../model/CertManagerVo/CredentialVo'

export default class CMShowPrivateCredPresenter {
  private static sInstance: CMShowPrivateCredPresenter;
  credList: CredentialAbstractVo[] = [];
  credInfo: CredentialVo = new CredentialVo("", "", "", 0, 0, null);

  public static getInstance(): CMShowPrivateCredPresenter {
    if (CMShowPrivateCredPresenter.sInstance == null) {
      CMShowPrivateCredPresenter.sInstance = new CMShowPrivateCredPresenter();
    }
    return CMShowPrivateCredPresenter.sInstance;
  }

  aboutToDisappear() {
    this.credList = [];
    this.credInfo = new CredentialVo("", "", "", 0, 0, null);
  }

  updatePrivateCredList() {
    certManagerModel.getCertOrCredList(CMModelOptType.CM_MODEL_OPT_PRIVATE_CRED, (errCode, credList) => {
      if (errCode === CMModelErrorCode.CM_MODEL_ERROR_SUCCESS) {
        credList.sort((certAbs, certAbsOther) => {
          let certAlias = certAbs.alias
          let certAliasOther = certAbsOther.alias
          if (certAlias <= certAliasOther) {
            return -1
          } else {
            return 1
          }
        })
        this.credList = credList;
      } else {
        console.log("updatePrivateCredList error :" + JSON.stringify(errCode));
        this.credList = [];
      }
    });
  }

  getPrivateCred(uri) {
    certManagerModel.getCertOrCred(CMModelOptType.CM_MODEL_OPT_PRIVATE_CRED, uri, (errCode, credInfo) => {
      if (errCode === CMModelErrorCode.CM_MODEL_ERROR_SUCCESS) {
        this.credInfo = credInfo
      } else {
        console.log("getPrivateCred error :" + JSON.stringify(errCode));
        this.credInfo.clearCredentialVo();
      }
    });
  }
}