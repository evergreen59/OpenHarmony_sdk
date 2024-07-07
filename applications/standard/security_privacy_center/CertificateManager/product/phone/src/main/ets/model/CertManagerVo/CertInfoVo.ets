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

export class CertInfoVo {
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
  subjectNameMap :Map<string, string>;
  issuerNameMap :Map<string, string>;
  dateMap :Map<string, string>;

  constructor(uri: string,
              certAlias: string,
              status: boolean,
              issuerName: string,
              subjectName: string,
              serial: string,
              notBefore: string,
              notAfter: string,
              fingerprintSha256: string,
              cert: Uint8Array,
              subjectNameMap: Map<string, string>,
              issuerNameMap :Map<string, string>,
              dateMap :Map<string, string>) {
    this.uri = uri;
    this.certAlias = certAlias;
    this.status = status;
    this.issuerName = issuerName;
    this.subjectName = subjectName;
    this.serial = serial;
    this.notBefore = notBefore;
    this.notAfter = notAfter;
    this.fingerprintSha256 = fingerprintSha256;
    this.cert = cert;
    this.subjectNameMap = subjectNameMap
    this.issuerNameMap = issuerNameMap
    this.dateMap = dateMap
  }

  clearCertInfoVo() {
    this.uri = "";
    this.certAlias = "";
    this.status = false;
    this.issuerName = "";
    this.subjectName = "";
    this.serial = "";
    this.notBefore = "";
    this.notAfter = "";
    this.fingerprintSha256 = "";
    this.cert = null;
    this.subjectNameMap = null;
    this.issuerNameMap = null;
    this.dateMap = null;
  }

}