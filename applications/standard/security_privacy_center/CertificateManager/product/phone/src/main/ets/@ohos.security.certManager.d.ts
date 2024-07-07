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

export interface AsyncCallback<T, E = void> {
    (err: BusinessError<E>, data: T): void;
}

export interface BusinessError<T = void> extends Error {
    code: number;
    data?: T;
}

declare namespace CertManagerFunc {
    function getSystemTrustedCertificateList(context: CMContext, callback: AsyncCallback<CMResult>) : void;
    function getSystemTrustedCertificateList(context: CMContext) : Promise<CMResult>;

    function getSystemTrustedCertificate(context: CMContext, certUri: string, callback: AsyncCallback<CMResult>) : void;
    function getSystemTrustedCertificate(context: CMContext, certUri: string) : Promise<CMResult>;

    function setCertificateStatus(context: CMContext, certUri: string, store: number, status: boolean, callback: AsyncCallback<boolean>) : void;
    function setCertificateStatus(context: CMContext, certUri: string, store: number, status: boolean) : Promise<boolean>;

    function installUserTrustedCertificate(certificate: CertBlob, callback: AsyncCallback<CMResult>) : void;
    function installUserTrustedCertificate(certificate: CertBlob,) : Promise<CMResult>;

    function uninstallAllUserTrustedCertificate(callback: AsyncCallback<boolean>) : void;
    function uninstallAllUserTrustedCertificate() : Promise<boolean>;

    function uninstallUserTrustedCertificate(certUri: string, callback: AsyncCallback<boolean>) : void;
    function uninstallUserTrustedCertificate(certUri: string) : Promise<boolean>;

    function getUserTrustedCertificateList(callback: AsyncCallback<CMResult>) : void;
    function getUserTrustedCertificateList() : Promise<CMResult>;

    function getUserTrustedCertificate(certUri: string, callback: AsyncCallback<CMResult>) : void;
    function getUserTrustedCertificate(certUri: string) : Promise<CMResult>;

    function installAppCertificate(keystore: Uint8Array, keystorePwd: string, certAlias: string, callback: AsyncCallback<CMResult>) : void;
    function installAppCertificate(keystore: Uint8Array, keystorePwd: string, certAlias: string) : Promise<CMResult>;

    function installPrivateCertificate(keystore: Uint8Array, keystorePwd: string, certAlias: string, callback: AsyncCallback<CMResult>) : void;
    function installPrivateCertificate(keystore: Uint8Array, keystorePwd: string, certAlias: string) : Promise<CMResult>;

    function generatePrivateCertificate(keyAlias: string, keyProperties: CMKeyProperties, callback: AsyncCallback<CMResult>) : void;
    function generatePrivateCertificate(keyAlias: string, keyProperties: CMKeyProperties) : Promise<CMResult>;

    function updatePrivateCertificate(type: string, keyUri: string, certificate: CertBlob, callback: AsyncCallback<boolean>) : void;
    function updatePrivateCertificate(type: string, keyUri: string, certificate: CertBlob) : Promise<boolean>;

    function uninstallAllAppCertificate(callback: AsyncCallback<boolean>) : void;
    function uninstallAllAppCertificate() : Promise<boolean>;

    function uninstallAppCertificate(keyUri: string, callback: AsyncCallback<boolean>) : void;
    function uninstallAppCertificate(keyUri: string) : Promise<boolean>;

    function uninstallPrivateCertificate(keyUri: string, callback: AsyncCallback<boolean>) : void;
    function uninstallPrivateCertificate(keyUri: string) : Promise<boolean>;

    function getAppCertificateList(callback: AsyncCallback<CMResult>) : void;
    function getAppCertificateList() : Promise<CMResult>;

    function getPrivateCertificateList(callback: AsyncCallback<CMResult>) : void;
    function getPrivateCertificateList() : Promise<CMResult>;

    function getAppCertificate(keyUri: string, callback: AsyncCallback<CMResult>) : void;
    function getAppCertificate(keyUri: string, ) : Promise<CMResult>;

    function getPrivateCertificate(keyUri: string, callback: AsyncCallback<CMResult>) : void;
    function getPrivateCertificate(keyUri: string) : Promise<CMResult>;

    function grantAppCertificate(keyUri: string, clientAppUid: string, callback: AsyncCallback<CMResult>) : void;
    function grantAppCertificate(keyUri: string, clientAppUid: string) : Promise<CMResult>;

    function isAuthorizedApp(keyUri: string, callback: AsyncCallback<boolean>) : void;
    function isAuthorizedApp(keyUri: string) : Promise<boolean>;

    function getAuthorizedAppList(keyUri: string, callback: AsyncCallback<CMResult>) : void;
    function getAuthorizedAppList(keyUri: string) : Promise<CMResult>;

    function removeGrantedAppCertificate(keyUri: string, clientAppUid: string, callback: AsyncCallback<boolean>) : void;
    function removeGrantedAppCertificate(keyUri: string, clientAppUid: string) : Promise<boolean>;

    function init(authUri: string, spec: CMSignatureSpec, callback: AsyncCallback<CMHandle>) : void;
    function init(authUri: string, spec: CMSignatureSpec) : Promise<CMHandle>;

    function update(handle: Uint8Array, data: Uint8Array, callback: AsyncCallback<boolean>) : void;
    function update(handle: Uint8Array, data: Uint8Array) : Promise<boolean>;

    function finish(handle: Uint8Array, callback: AsyncCallback<CMResult>) : void;
    function finish(handle: Uint8Array, signature: Uint8Array, callback: AsyncCallback<CMResult>) : void;
    function finish(handle: Uint8Array, signature?: Uint8Array) : Promise<CMResult>;

    function abort(handle: Uint8Array, callback: AsyncCallback<boolean>) : void;
    function abort(handle: Uint8Array) : Promise<boolean>;

    export interface CMContext {
        userId: string;
        uid: string;
        packageName: string;
    }

    export interface CertInfo {
        uri: string;
        certAlias: string;
        status: boolean;
        issuerName: string;
        subjectName: string;
        serial: string;
        notBefore: string;
        notAfter: string;
        fingerprintSha256: string;
        cert: Uint8Array;
    }

    export interface CertAbstract {
        uri: string;
        certAlias: string;
        status: boolean;
        subjectName: string;
    }

    export interface Credential {
        type: string;
        alias: string;
        keyUri: string;
        certNum: number;
        keyNum: number;
        credData:Uint8Array;
    }

    export interface CredentialAbstract {
        type: string;
        alias: string;
        keyUri: string;
    }

    export interface CertBlob {
        inData: Uint8Array;
        alias: string;
    }

    export interface CMResult {
        certList?: Array<CertAbstract>;
        certInfo?: CertInfo;
        credentialList?: Array<CredentialAbstract>;
        credential?: Credential;
        appUidList?: Array<string>;
        uri?: string;
        outData?: Uint8Array;
        isAuth?: boolean;
    }

    export interface CMKeyProperties {
        type: string;
        alg: string;
        size: number;
        padding: string;
        purpose: string;
        digest: string;
        authType: string;
        authTimeout: string;
    }

    export enum CmKeyPurpose {
        CM_KEY_PURPOSE_SIGN = 4,
        CM_KEY_PURPOSE_VERIFY = 8,
    }

    export interface CMSignatureSpec {
        purpose: CmKeyPurpose;
    }

    export interface CMHandle {
        handle: Uint8Array;
    }

    export enum CMErrorCode {
        CM_SUCCESS = 0,
        CM_ERROR_INNER_ERROR = 17500001,
        CM_ERROR_NO_PERMISSION = 17500002,
        CM_ERROR_NO_FOUND = 17500003,
        CM_ERROR_X509_FORMATE = 17500004,
    }
}

export default CertManagerFunc;
