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

import certManagerModel from '../model/CertMangerModel';
import BundleModel from '../model/BundleModel';
import { CMModelErrorCode, CMModelOptType } from '../model/CertMangerModel';
import { CredentialAbstractVo } from '../model/CertManagerVo/CredentialAbstractVo';
import router from '@system.router';

const TAG = "CMAppCredAuthPresenter Presenter: ";
const SUCCESS = 0;
const FAIL = -1;

export default class CMAppCredAuthPresenter {
  private static sInstance: CMAppCredAuthPresenter;
  optType: CMModelOptType = CMModelOptType.CM_MODEL_OPT_UNKNOWN;
  credList: CredentialAbstractVo[] = [];
  appName: string = '';

  public static getInstance(): CMAppCredAuthPresenter {
    if (CMAppCredAuthPresenter.sInstance == null) {
      CMAppCredAuthPresenter.sInstance = new CMAppCredAuthPresenter();
    }
    return CMAppCredAuthPresenter.sInstance;
  }

  onAboutToAppear() {

  }

  aboutToDisappear() {
    this.optType = CMModelOptType.CM_MODEL_OPT_UNKNOWN;
    this.credList = [];
  }

  UpdateAppNameFromUid(uid) {
    console.log("getAppNameFromUid start uid = " + uid);
    BundleModel.getAppInfoList(uid, (errCode, appInfo) => {
      console.log("getAppNameFromUid success, appInfo = " + appInfo);
      if (!errCode) {
        this.appName = appInfo.AppName;
      } else {
        console.log("getAppNameFromUid fail, uid = " + uid);
      }
    })
  }

  updateAppCredList() {
    certManagerModel.getCertOrCredList(CMModelOptType.CM_MODEL_OPT_APP_CRED, (errCode, credList) => {
      if (errCode === CMModelErrorCode.CM_MODEL_ERROR_SUCCESS) {
        this.credList = credList;
        console.log("updateSystemTrustedCertificateList inin :" + JSON.stringify(credList));
      } else {
        console.log("updateSystemTrustedCertificateList fail");
      }
    });
  }

  requestAuthorize(uri, appUid) {
    console.log(TAG + "requestAuthorize start uri :" + uri + "appUid: " + appUid);
    let want = globalThis.abilityWant;

    certManagerModel.setAppAuth(CMModelOptType.CM_MODEL_OPT_APP_CRED, uri, appUid,
      true, (errCode, data) => {
        router.clear();
        if (errCode === CMModelErrorCode.CM_MODEL_ERROR_SUCCESS) {
          console.log("requestAuthorize success data: " + data);
          want.parameters.authUri = data;
          globalThis.certManagerAbilityContext.terminateSelfWithResult({
            resultCode: SUCCESS,
            want: want
          });
        } else {
          console.log("requestAuthorize fail");
          globalThis.certManagerAbilityContext.terminateSelfWithResult({
            resultCode: FAIL,
            want: want
          });
        }
      });
  }

  cancelProcess() {
    console.log("cancelProcess start");
    router.clear();
    globalThis.certManagerAbilityContext.terminateSelfWithResult({
      resultCode: FAIL,
      want: globalThis.abilityWant
    });
  }
}