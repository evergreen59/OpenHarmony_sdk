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
import { CertAbstractVo } from '../model/CertManagerVo/CertAbstractVo'
import { CertInfoVo } from '../model/CertManagerVo/CertInfoVo'

const TAG = "CMFaShowUserCa Presenter: ";

export default class CMShowUserCaPresenter {
  private static sInstance: CMShowUserCaPresenter;
  certList: CertAbstractVo[] = [];
  certInfo: CertInfoVo = new CertInfoVo("", "", false, "", "", "", "", "", "", null, null, null, null);

  public static getInstance(): CMShowUserCaPresenter {
    if (CMShowUserCaPresenter.sInstance == null) {
      CMShowUserCaPresenter.sInstance = new CMShowUserCaPresenter();
    }
    return CMShowUserCaPresenter.sInstance;
  }

  onAboutToAppear() {

  }

  aboutToDisappear() {
    this.certList = [];
    this.certInfo = new CertInfoVo("", "", false, "", "", "", "", "", "", null, null, null, null);
  }

  updateUserTrustedCertificateList() {
    certManagerModel.getCertOrCredList(CMModelOptType.CM_MODEL_OPT_USER_CA, (errCode, certList) => {
      if (errCode === CMModelErrorCode.CM_MODEL_ERROR_SUCCESS) {
        certList.sort((certAbs, certAbsOther) => {
          let certAlias = certAbs.certAlias
          let certAliasOther = certAbsOther.certAlias
          if (certAlias <= certAliasOther) {
            return -1
          } else {
            return 1
          }
        })
        this.certList = certList;
        console.log("updateUserTrustedCertificateList inin :" + JSON.stringify(certList));
      } else {
        console.info(TAG + 'updateUserTrustedCertificateList fail,errCode is ' + errCode);
        this.certList = [];
      }
    });
  }

  getUserTrustedCertificate(uri, callback) {
    certManagerModel.getCertOrCred(CMModelOptType.CM_MODEL_OPT_USER_CA, uri, (errCode, certInfo) => {
      if (errCode === CMModelErrorCode.CM_MODEL_ERROR_SUCCESS) {
        console.info(TAG + 'getUserTrustedCertificate success, errCode is ' + errCode + ' certInfo is' +
        JSON.stringify(certInfo));
        this.certInfo = certInfo;
        callback()
      } else {
        console.info(TAG + 'getUserTrustedCertificate fail, errCode is ' + errCode);
        this.certInfo.clearCertInfoVo();
        callback()
      }
    });
  }

  setUserCertificateStatus(uri, status) {
    certManagerModel.setCertStatus(CMModelOptType.CM_MODEL_OPT_USER_CA, uri, status, (errCode) => {
      if (errCode === CMModelErrorCode.CM_MODEL_ERROR_SUCCESS) {
        this.getUserTrustedCertificate(uri, () => {
          console.info(TAG + 'setCerStatus then getUserTrustedCertificate,errCode is' + errCode);
        });
      } else {
        console.info(TAG + 'setUserCertificateStatus fail,errCode is ' + errCode)
      }
    })
  }

  deleteUserCertificate(uri) {
    certManagerModel.deleteCertOrCred(CMModelOptType.CM_MODEL_OPT_USER_CA, uri, (errCode) => {
      if (errCode === CMModelErrorCode.CM_MODEL_ERROR_SUCCESS) {
        this.updateUserTrustedCertificateList();
      } else {
        console.info(TAG + 'deleteUserCertificate fail,errCode is ' + errCode);
      }
    })
  }
}