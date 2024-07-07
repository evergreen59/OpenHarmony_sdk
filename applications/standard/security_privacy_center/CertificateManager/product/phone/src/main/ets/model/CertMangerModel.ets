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

import { CertAbstractVo } from './CertManagerVo/CertAbstractVo'
import { CertInfoVo } from './CertManagerVo/CertInfoVo'
import { CredentialAbstractVo } from './CertManagerVo/CredentialAbstractVo'
import { CredentialVo } from './CertManagerVo/CredentialVo'

import CertManager from '@ohos.security.certManager'

const TAG = "CertManager Model: ";

export enum CMModelErrorCode {
  CM_MODEL_ERROR_SUCCESS = 0,
  CM_MODEL_ERROR_FAILED = -1,
  CM_MODEL_ERROR_EXCEPTION = -2,
  CM_MODEL_ERROR_UNKNOWN_OPT = -3,
  CM_MODEL_ERROR_NOT_SUPPORT = -4,
}

export enum CMModelOptType {
  CM_MODEL_OPT_UNKNOWN = 0,
  CM_MODEL_OPT_SYSTEM_CA = 1,
  CM_MODEL_OPT_USER_CA = 2,
  CM_MODEL_OPT_APP_CRED = 3,
  CM_MODEL_OPT_PRIVATE_CRED = 4,
}

export enum CertManagerStore {
  CERT_MANAGER_CREDENTIAL_STORE = 0, /* credential certificate store for end entity certificates. */
  CERT_MANAGER_SYSTEM_TRUSTED_STORE = 1, /* read only, updated by system only. */
  CERT_MANAGER_USER_TRUSTED_STORE = 2, /* modifiable by applications and user. */
  CERT_MANAGER_APPLICATION_TRUSTED_STORE = 3, /* application specific trusted certificate store; modifiable by the application only. */
}

export class CertMangerModel {
  getCertOrCredList(optType, callback) {
    console.log(TAG + "getCertOrCredList start");

    switch (optType) {
      case CMModelOptType.CM_MODEL_OPT_SYSTEM_CA:
        this.getSystemTrustedCertificateList((errCode, certList) => {
          callback(errCode, certList)
        });
        break;
      case CMModelOptType.CM_MODEL_OPT_USER_CA:
        this.getUserTrustedCertificateList((errCode, certList) => {
          callback(errCode, certList)
        });
        break;
      case CMModelOptType.CM_MODEL_OPT_APP_CRED:
        this.getAppCredList((errCode, credList) => {
          callback(errCode, credList)
        });
        break;
      case CMModelOptType.CM_MODEL_OPT_PRIVATE_CRED:
        this.getPrivateCredList((errCode, credList) => {
          callback(errCode, credList)
        });
        break;
      default:
        callback(CMModelErrorCode.CM_MODEL_ERROR_UNKNOWN_OPT, null)
        break;
    }
  }

  getCertOrCred(optType, uri, callback) {
    console.log(TAG + "getCertOrCred start");

    switch (optType) {
      case CMModelOptType.CM_MODEL_OPT_SYSTEM_CA:
        this.getSystemTrustedCertificate(uri, (errCode, certInfo) => {
          console.info('getSystemTrustedCertificate certInfo is' + JSON.stringify(certInfo))
          callback(errCode, certInfo)
        });
        break;
      case CMModelOptType.CM_MODEL_OPT_USER_CA:
        this.getUserTrustedCertificate(uri, (errCode, certInfo) => {
          callback(errCode, certInfo)
        });
        break;
      case CMModelOptType.CM_MODEL_OPT_APP_CRED:
        this.getAppCredential(uri, (errCode, credInfo) => {
          callback(errCode, credInfo)
        });
        break;
      case CMModelOptType.CM_MODEL_OPT_PRIVATE_CRED:
        this.getPrivateCred(uri, (errCode, credInfo) => {
          callback(errCode, credInfo)
        });
        break;
      default:
        callback(CMModelErrorCode.CM_MODEL_ERROR_UNKNOWN_OPT, null)
        break;
    }
  }

  deleteCertOrCred(optType, uri, callback) {
    console.log(TAG + "deleteCertOrCred start");

    switch (optType) {
      case CMModelOptType.CM_MODEL_OPT_USER_CA:
        this.deleteUserTrustedCertificate(uri, (errCode) => {
          callback(errCode)
        });
        break;
      case CMModelOptType.CM_MODEL_OPT_APP_CRED:
        this.deleteAppCredential(uri, (errCode) => {
          callback(errCode)
        });
        break;
      case CMModelOptType.CM_MODEL_OPT_SYSTEM_CA:
      case CMModelOptType.CM_MODEL_OPT_PRIVATE_CRED:
        callback(CMModelErrorCode.CM_MODEL_ERROR_NOT_SUPPORT)
        break;
      default:
        callback(CMModelErrorCode.CM_MODEL_ERROR_UNKNOWN_OPT)
        break;
    }
  }

  setCertStatus(optType, uri, status, callback) {
    console.log(TAG + "setCertStatus start");

    switch (optType) {
      case CMModelOptType.CM_MODEL_OPT_USER_CA:
        this.setCertificateStatus(uri, CertManagerStore.CERT_MANAGER_USER_TRUSTED_STORE, status, (errCode) => {
          callback(errCode)
        });
        break;
      case CMModelOptType.CM_MODEL_OPT_SYSTEM_CA:
        this.setCertificateStatus(uri, CertManagerStore.CERT_MANAGER_SYSTEM_TRUSTED_STORE, status, (errCode) => {
          callback(errCode)
        });
        break;
      case CMModelOptType.CM_MODEL_OPT_APP_CRED:
      case CMModelOptType.CM_MODEL_OPT_PRIVATE_CRED:
        callback(CMModelErrorCode.CM_MODEL_ERROR_NOT_SUPPORT)
        break;
      default:
        callback(CMModelErrorCode.CM_MODEL_ERROR_UNKNOWN_OPT)
        break;
    }
  }

  delAllCertOrCred(optType, callback) {
    console.log(TAG + "delAllCertOrCred start");

    switch (optType) {
      case CMModelOptType.CM_MODEL_OPT_USER_CA:
        this.delAllUserCertificate((errCode) => {
          callback(errCode)
        });
        break;
      case CMModelOptType.CM_MODEL_OPT_APP_CRED:
        this.delAllAppCredential((errCode) => {
          callback(errCode)
        });
        break;
      case CMModelOptType.CM_MODEL_OPT_SYSTEM_CA:
      case CMModelOptType.CM_MODEL_OPT_PRIVATE_CRED:
        callback(CMModelErrorCode.CM_MODEL_ERROR_NOT_SUPPORT)
        break;
      default:
        callback(CMModelErrorCode.CM_MODEL_ERROR_UNKNOWN_OPT)
        break;
    }
  }

  getAuthAppList(optType, uri, callback) {
    console.log(TAG + "getAuthAppList start");

    switch (optType) {
      case CMModelOptType.CM_MODEL_OPT_APP_CRED:
        this.getAuthorizedAppList(uri, (errCode, appUidList) => {
          callback(errCode, appUidList)
        });
        break;
      case CMModelOptType.CM_MODEL_OPT_USER_CA:
      case CMModelOptType.CM_MODEL_OPT_SYSTEM_CA:
      case CMModelOptType.CM_MODEL_OPT_PRIVATE_CRED:
        callback(CMModelErrorCode.CM_MODEL_ERROR_NOT_SUPPORT)
        break;
      default:
        callback(CMModelErrorCode.CM_MODEL_ERROR_UNKNOWN_OPT)
        break;
    }
  }

  setAppAuthPromise(optType, uri, appUid, status) {
    console.log(TAG + "setAppAuth start");
    switch (optType) {
      case CMModelOptType.CM_MODEL_OPT_APP_CRED:
        return new Promise((resolve,reject)=>{
          this.setAuthorizedAppStatus(uri, appUid, status, (errCode, data) => {
            if (errCode == CMModelErrorCode.CM_MODEL_ERROR_SUCCESS) {
              resolve(data)
            } else {
              reject(errCode)
            }
          });
        })
      default:
        return new Promise((resolve,reject) => {
          reject(CMModelErrorCode.CM_MODEL_ERROR_UNKNOWN_OPT)
        })
    }
  }

  setAppAuth(optType, uri, appUid, status, callback) {
    console.log(TAG + "setAppAuth start");

    switch (optType) {
      case CMModelOptType.CM_MODEL_OPT_APP_CRED:
        this.setAuthorizedAppStatus(uri, appUid, status, (errCode, data) => {
          callback(errCode, data)
        });
        break;
      case CMModelOptType.CM_MODEL_OPT_USER_CA:
      case CMModelOptType.CM_MODEL_OPT_SYSTEM_CA:
      case CMModelOptType.CM_MODEL_OPT_PRIVATE_CRED:
        callback(CMModelErrorCode.CM_MODEL_ERROR_NOT_SUPPORT)
        break;
      default:
        callback(CMModelErrorCode.CM_MODEL_ERROR_UNKNOWN_OPT)
        break;
    }
  }

  installCertOrCred(optType, alias, data, pwd, callback) {
    console.log(TAG + "installCertOrCred start");
    console.info(TAG + 'installCert optType: ' + optType + 'alias: ' + alias + ' pwd: ' + pwd)
    switch (optType) {
      case CMModelOptType.CM_MODEL_OPT_USER_CA:
        this.installUserCertificate(data, alias, (errCode) => {
          callback(errCode)
        });
        break;
      case CMModelOptType.CM_MODEL_OPT_APP_CRED:
        this.installAppCertificate(data, alias, pwd, (errCode) => {
          callback(errCode)
        });
        break;
      case CMModelOptType.CM_MODEL_OPT_PRIVATE_CRED:
      case CMModelOptType.CM_MODEL_OPT_SYSTEM_CA:
        callback(CMModelErrorCode.CM_MODEL_ERROR_NOT_SUPPORT)
        break;
      default:
        callback(CMModelErrorCode.CM_MODEL_ERROR_UNKNOWN_OPT)
        break;
    }
  }

  private async getSystemTrustedCertificateList(callback) {
    console.log(TAG + "getSystemTrustedCertificateList start");
    try {
      let SubjectNameCN;
      var context = {
        userId: "123",
        uid: "123",
        packageName: "123"
      }
      let result = await CertManager.getSystemTrustedCertificateList(context);
      let certList = new Array();
      console.log(TAG + "certManager::getSystemTrustedCertificateList result: " + JSON.stringify(result));
      for (let i = 0; i < result.certList.length; i++) {
        if (result.certList[i].subjectName.length != 0) {
          SubjectNameCN = result.certList[i].subjectName.match(/(?<=CN=).*?(?=,)/g).toString()
          console.info('SubjectNameCN is:' + SubjectNameCN)
        }
        certList.push(new CertAbstractVo(result.certList[i].uri, result.certList[i].certAlias,
          result.certList[i].status, result.certList[i].subjectName, SubjectNameCN))
      }
      console.log(TAG + "getSystemTrustedCertificateList end");
      callback(CMModelErrorCode.CM_MODEL_ERROR_SUCCESS, certList)

    } catch (err) {
      console.log(TAG + "getSystemTrustedCertificateList err: " + JSON.stringify(err));
      callback(CMModelErrorCode.CM_MODEL_ERROR_EXCEPTION, null)
    }
  }

  private async getSystemTrustedCertificate(certUri, callback) {
    console.log(TAG + "getSystemTrustedCertificate start");
    try {
      var context = {
        userId: "123",
        uid: "123",
        packageName: "123"
      }
      let result = await CertManager.getSystemTrustedCertificate(context, certUri);
      let subjectNameMap: Map<string, string> = new Map()
      let issuerNameMap: Map<string, string> = new Map()
      let dateMap: Map<string, string> = new Map()
      if (result.certInfo.subjectName.length != 0) {
        let SubjectNameCN = result.certInfo.subjectName.match(/(?<=CN=).*?(?=,)/g)
        console.info('SubjectNameCN is:' + SubjectNameCN)
        let SubjectNameOU = result.certInfo.subjectName.match(/(?<=OU=).*?(?=,)/g)
        console.info('SubjectNameOU is:' + SubjectNameOU)
        let SubjectNameO = result.certInfo.subjectName.match(/(?<=O=).*/g)
        console.info('SubjectNameO is:' + SubjectNameO)
        subjectNameMap.set('常用名称:', String(SubjectNameCN))
        subjectNameMap.set('组织:', String(SubjectNameO))
        subjectNameMap.set('组织单位:', String(SubjectNameOU))
        subjectNameMap.set('序列号:', result.certInfo.serial)
      }
      if (result.certInfo.issuerName.length != 0) {
        let IssuerNameCN = result.certInfo.issuerName.match(/(?<=CN=).*?(?=,)/g)
        console.info('issuerNameCN is:' + IssuerNameCN)
        let IssuerNameOU = result.certInfo.issuerName.match(/(?<=OU=).*?(?=,)/g)
        console.info('issuerNameOU is:' + IssuerNameOU)
        let IssuerNameO = result.certInfo.issuerName.match(/(?<=O=).*/g)
        console.info('issuerNameO is:' + IssuerNameO)
        issuerNameMap.set('常用名称:', String(IssuerNameCN))
        issuerNameMap.set('组织:', String(IssuerNameO))
        issuerNameMap.set('组织单位:', String(IssuerNameOU))
      }
      dateMap.set('颁发时间:', result.certInfo.notBefore)
      dateMap.set('有效期至:', result.certInfo.notAfter)

      let certInfo = new CertInfoVo(result.certInfo.uri, result.certInfo.certAlias,
        result.certInfo.status, result.certInfo.issuerName, result.certInfo.subjectName,
        result.certInfo.serial, result.certInfo.notBefore,
        result.certInfo.notAfter, result.certInfo.fingerprintSha256, result.certInfo.cert,
        subjectNameMap, issuerNameMap, dateMap);

      console.log(TAG + "getSystemTrustedCertificate end");
      callback(CMModelErrorCode.CM_MODEL_ERROR_SUCCESS, certInfo)

    } catch (err) {
      console.log(TAG + "getSystemTrustedCertificate err: " + JSON.stringify(err));
      callback(CMModelErrorCode.CM_MODEL_ERROR_EXCEPTION, null)
    }
  }

  private async getUserTrustedCertificateList(callback) {
    console.log(TAG + "getUserTrustedCertificateList start");
    try {
      let SubjectNameCN
      let result = await CertManager.getUserTrustedCertificateList();
      let certList = new Array();
      console.log(TAG + "getUserTrustedCertificateList result: " + JSON.stringify(result));
      for (let i = 0; i < result.certList.length; i++) {
          if (result.certList[i].subjectName.length != 0) {
            SubjectNameCN = result.certList[i].subjectName.match(/(?<=CN=).*?(?=,)/g).toString()
            console.info('SubjectNameCN is:' + SubjectNameCN)
          }
        certList.push(new CertAbstractVo(result.certList[i].uri, result.certList[i].certAlias,
            result.certList[i].status, result.certList[i].subjectName, SubjectNameCN))
      }
      console.log(TAG + "getUserTrustedCertificateList end");
      callback(CMModelErrorCode.CM_MODEL_ERROR_SUCCESS, certList)
    } catch (err) {
      console.log(TAG + "getUserTrustedCertificateList err: " + JSON.stringify(err));
      callback(CMModelErrorCode.CM_MODEL_ERROR_EXCEPTION)
    }
  }

  private async getUserTrustedCertificate(certUri, callback) {
    console.log(TAG + "getUserTrustedCertificate start");
    try {
      let result = await CertManager.getUserTrustedCertificate(certUri);
      let subjectNameMap: Map<string, string> = new Map()
      let issuerNameMap: Map<string, string> = new Map()
      let dateMap: Map<string, string> = new Map()

      if (result.certInfo.subjectName.length != 0) {
        let SubjectNameCN = result.certInfo.subjectName.match(/(?<=CN=).*?(?=,)/g)
        console.info('SubjectNameCN is:' + SubjectNameCN)
        let SubjectNameOU = result.certInfo.subjectName.match(/(?<=OU=).*?(?=,)/g)
        console.info('SubjectNameOU is:' + SubjectNameOU)
        let SubjectNameO = result.certInfo.subjectName.match(/(?<=O=).*/g)
        console.info('SubjectNameO is:' + SubjectNameO)
        subjectNameMap.set('常用名称:', String(SubjectNameCN))
        subjectNameMap.set('组织:', String(SubjectNameO))
        subjectNameMap.set('组织单位:', String(SubjectNameOU))
        subjectNameMap.set('序列号:', result.certInfo.serial)
      }
      if (result.certInfo.issuerName.length != 0) {
        let IssuerNameCN = result.certInfo.issuerName.match(/(?<=CN=).*?(?=,)/g)
        console.info('issuerNameCN is:' + IssuerNameCN)
        let IssuerNameOU = result.certInfo.issuerName.match(/(?<=OU=).*?(?=,)/g)
        console.info('issuerNameOU is:' + IssuerNameOU)
        let IssuerNameO = result.certInfo.issuerName.match(/(?<=O=).*/g)
        console.info('issuerNameO is:' + IssuerNameO)
        issuerNameMap.set('常用名称:', String(IssuerNameCN))
        issuerNameMap.set('组织:', String(IssuerNameO))
        issuerNameMap.set('组织单位:', String(IssuerNameOU))
      }
      dateMap.set('颁发时间:', result.certInfo.notBefore)
      dateMap.set('有效期至:', result.certInfo.notAfter)
      let certInfo = new CertInfoVo(result.certInfo.uri, result.certInfo.certAlias,
        result.certInfo.status, result.certInfo.issuerName, result.certInfo.subjectName,
        result.certInfo.serial, result.certInfo.notBefore,
        result.certInfo.notAfter, result.certInfo.fingerprintSha256, result.certInfo.cert,
        subjectNameMap, issuerNameMap, dateMap);
      console.log(TAG + "getUserTrustedCertificate end");
      callback(CMModelErrorCode.CM_MODEL_ERROR_SUCCESS, certInfo)
    } catch (err) {
      console.log(TAG + "getUserTrustedCertificate err: " + JSON.stringify(err));
      callback(CMModelErrorCode.CM_MODEL_ERROR_EXCEPTION, null)
    }
  }

  private async deleteUserTrustedCertificate(certUri, callback) {
    console.log(TAG + "deleteUserTrustedCertificate start");
    try {
      await CertManager.uninstallUserTrustedCertificate(certUri);
      console.log(TAG + "deleteUserTrustedCertificate end");
      callback(CMModelErrorCode.CM_MODEL_ERROR_SUCCESS)
    } catch (err) {
      console.log(TAG + "deleteUserTrustedCertificate err: " + JSON.stringify(err));
      callback(CMModelErrorCode.CM_MODEL_ERROR_EXCEPTION)
    }
  }

  private async setCertificateStatus(certUri, store, status, callback) {
    console.log(TAG + "setCertificateStatus start");
    try {
      var context = {
        userId: "123",
        uid: "123",
        packageName: "123"
      }
      await CertManager.setCertificateStatus(context, certUri, store, status);
      console.log(TAG + "setCertificateStatus end");
      callback(CMModelErrorCode.CM_MODEL_ERROR_SUCCESS)
    } catch (err) {
      console.info(TAG + 'setCertificateStatus fail, err is' + err)
      callback(CMModelErrorCode.CM_MODEL_ERROR_EXCEPTION)
    }
  }

  private async getAppCredList(callback) {
    console.log(TAG + "getAppCredList start");

    try {
      let result = await CertManager.getAppCertificateList();
      let credList = new Array();
      for (let i = 0; i < result.credentialList.length; i++) {
        credList.push(new CredentialAbstractVo(result.credentialList[i].type, result.credentialList[i].alias,
          result.credentialList[i].keyUri))
      }
      console.log(TAG + "getAppCredList end credList: " + JSON.stringify(credList));
      callback(CMModelErrorCode.CM_MODEL_ERROR_SUCCESS, credList)
    } catch (err) {
      console.info(TAG + 'getAppCredList fail, err is ' + JSON.stringify(err))
      callback(CMModelErrorCode.CM_MODEL_ERROR_EXCEPTION)
    }
  }

  private async getAppCredential(certUri, callback) {
    console.log(TAG + "getAppCredential start");
    try {
      let result = await CertManager.getAppCertificate(certUri);
      let credInfo = new CredentialVo(result.credential.type, result.credential.alias,
        result.credential.keyUri, result.credential.certNum, result.credential.keyNum, result.credential.credData);
      console.log(TAG + "getAppCredential end");
      callback(CMModelErrorCode.CM_MODEL_ERROR_SUCCESS, credInfo)
    } catch (err) {
      console.info(TAG + 'getAppCredential fail, err is ' + JSON.stringify(err))
      callback(CMModelErrorCode.CM_MODEL_ERROR_EXCEPTION, null)
    }
  }

  private async deleteAppCredential(certUri, callback) {
    console.log(TAG + "deleteAppCredential start");
    try {
      await CertManager.uninstallAppCertificate(certUri);
      console.log(TAG + "deleteAppCredential end");
      callback(CMModelErrorCode.CM_MODEL_ERROR_SUCCESS)
    } catch (err) {
      console.info(TAG + 'deleteAppCredential fail, err is' + err)
      callback(CMModelErrorCode.CM_MODEL_ERROR_EXCEPTION)
    }
  }

  private async getPrivateCredList(callback) {
    console.log(TAG + "getPrivateCredList start");

    try {
      let result = await CertManager.getPrivateCertificateList();
      let credList = new Array();
      for (let i = 0; i < result.credentialList.length; i++) {
        credList.push(new CredentialAbstractVo(result.credentialList[i].type, result.credentialList[i].alias,
          result.credentialList[i].keyUri))
      }
      console.log(TAG + "getPrivateCredList end");
      callback(CMModelErrorCode.CM_MODEL_ERROR_SUCCESS, credList)
    } catch (err) {
      console.info(TAG + 'getPrivateCredList fail, err is' + err)
      callback(CMModelErrorCode.CM_MODEL_ERROR_EXCEPTION)
    }
  }

  private async getPrivateCred(certUri, callback) {
    console.log(TAG + "getPrivateCred start");
    try {
      let result = await CertManager.getPrivateCertificate(certUri);
      let credInfo = new CredentialVo(result.credential.type, result.credential.alias,
        result.credential.keyUri, result.credential.certNum, result.credential.keyNum, result.credential.credData);
      console.log(TAG + "getPrivateCred end");
      callback(CMModelErrorCode.CM_MODEL_ERROR_SUCCESS, credInfo)
    } catch (err) {
      console.info(TAG + 'getPrivateCred fail, err is' + err)
      callback(CMModelErrorCode.CM_MODEL_ERROR_EXCEPTION, null)
    }
  }

  private async delAllUserCertificate(callback) {
    console.log(TAG + "delAllUserCertificate start");
    try {
      await CertManager.uninstallAllUserTrustedCertificate();
      console.log(TAG + "delAllUserCertificate end");
      callback(CMModelErrorCode.CM_MODEL_ERROR_SUCCESS)
    } catch (err) {
      console.info(TAG + 'delAllUserCertificate fail, err is' + err)
      callback(CMModelErrorCode.CM_MODEL_ERROR_EXCEPTION)
    }
  }

  private async delAllAppCredential(callback) {
    console.log(TAG + "delAllAppCredential start");
    try {
      await CertManager.uninstallAllAppCertificate();
      console.log(TAG + "delAllAppCredential end");
      callback(CMModelErrorCode.CM_MODEL_ERROR_SUCCESS)
    } catch (err) {
      console.info(TAG + 'delAllAppCredential fail, err is' + err)
      callback(CMModelErrorCode.CM_MODEL_ERROR_EXCEPTION)
    }
  }

  private async getAuthorizedAppList(uri, callback) {
    console.log(TAG + "getAuthorizedAppList start");
    try {
      let result = await CertManager.getAuthorizedAppList(uri);
      console.log(TAG + "getAuthorizedAppList end");
      callback(CMModelErrorCode.CM_MODEL_ERROR_SUCCESS, result.appUidList)
    } catch (err) {
      console.info(TAG + 'getAuthorizedAppList fail, err is' + err)
      callback(CMModelErrorCode.CM_MODEL_ERROR_EXCEPTION, null)
    }
  }

  private async setAuthorizedAppStatus(uri, appUid, status, callback) {
    console.log(TAG + "setAuthorizedAppStatus start");
    try {
      if (status) {
        let result = await CertManager.grantAppCertificate(uri, appUid);
        console.log(TAG + "setAuthorizedAppStatus true end");
        callback(CMModelErrorCode.CM_MODEL_ERROR_SUCCESS, result.uri)
      } else {
        console.log(TAG + "appId:" + appUid + "uri:" + uri);
        await CertManager.removeGrantedAppCertificate(uri, appUid);
        console.log(TAG + "setAuthorizedAppStatus false end");
        callback(CMModelErrorCode.CM_MODEL_ERROR_SUCCESS)
      }
    } catch (err) {
      console.info(TAG + 'setAuthorizedAppStatus fail, err is' + JSON.stringify(err))
      callback(CMModelErrorCode.CM_MODEL_ERROR_EXCEPTION)
    }
  }

  private async installUserCertificate(data, alias, callback) {
    console.info(TAG + 'installUserCertificate start alias: ' + alias + 'data' + JSON.stringify(data))
    try {
      await CertManager.installUserTrustedCertificate({
        inData: data,
        alias: alias
      });

      console.info(TAG + 'installUserCertificate end')
      callback(CMModelErrorCode.CM_MODEL_ERROR_SUCCESS)
    } catch (err) {
      console.info(TAG + 'installUserCertificate fail, err is' + err)
      callback(CMModelErrorCode.CM_MODEL_ERROR_EXCEPTION)
    }
  }

  private async installAppCertificate(data, alias, pwd, callback) {
    console.info(TAG + 'installAppCertificate start')
    try {
      await CertManager.installAppCertificate(data, pwd, alias);
      console.info(TAG + 'installAppCertificate end')
      callback(CMModelErrorCode.CM_MODEL_ERROR_SUCCESS)
    } catch (err) {
      console.info(TAG + 'installAppCertificate fail, err is' + err)
      callback(CMModelErrorCode.CM_MODEL_ERROR_EXCEPTION)
    }
  }

}

let certMangerModel = new CertMangerModel();

export default certMangerModel as CertMangerModel;