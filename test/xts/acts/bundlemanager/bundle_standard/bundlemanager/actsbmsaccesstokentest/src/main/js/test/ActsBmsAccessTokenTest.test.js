/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

import bundle from '@ohos.bundle'
import account from '@ohos.account.osAccount'
import { describe,beforeAll, it, expect } from '@ohos/hypium'

const BUNDLE_NAME1 = 'com.example.bmsaccesstoken1';
const BUNDLE_NAME2 = 'com.example.bmsaccesstoken2';
const BUNDLE_NAME3 = 'com.example.bmsaccesstoken3';
let userId = 0;

export default function ActsBmsAccessTokenTest() {
describe('ActsBmsAccessTokenTest', function () {

    beforeAll(async function (done) {
        await account.getAccountManager().getOsAccountLocalIdFromProcess().then(account => {
            console.info("getOsAccountLocalIdFromProcess userid  ==========" + account);
            userId = account;
            done();
          }).catch(err=>{
            console.info("getOsAccountLocalIdFromProcess err ==========" + JSON.stringify(err));
            done();
          })
    });

    /*
     * @tc.number: BMS_SECURITY_INITIALIZE_0004
     * @tc.name: getAccessTokenIdOfEntry
     * @tc.desc: get the accessTokenId
     */
    it('getAccessTokenIdOfEntry', 0, async function (done) {
        await bundle.getApplicationInfo(BUNDLE_NAME1, bundle.BundleFlag.GET_BUNDLE_DEFAULT, userId)
            .then(applicationInfo => {
                console.info('accessTokenId: ' + applicationInfo.accessTokenId);
                expect(applicationInfo.name).assertEqual(BUNDLE_NAME1);
                expect(applicationInfo.accessTokenId).assertLarger(0);
                done();
            }).catch((err) => {
                expect(err).assertFail();
                done();
            });
    });

    /*
     * @tc.number: SUB_BMS_APPINFO_QUERYPERMISSION_0001
     * @tc.name: getReqPermissionOfEntry
     * @tc.desc: get the reqPermissions and reqPermissionStates
     */
    it('getReqPermissionOfEntry', 0, async function (done) {
        await bundle.getBundleInfo(BUNDLE_NAME3, bundle.BundleFlag.GET_BUNDLE_WITH_REQUESTED_PERMISSION)
            .then(bundleInfo => {
                expect(bundleInfo.name).assertEqual(BUNDLE_NAME3);
                expect(bundleInfo.reqPermissions.length).assertEqual(2);
                expect(bundleInfo.reqPermissions[0]).assertEqual("ohos.permission.DISTRIBUTED_DATASYNC");
                expect(bundleInfo.reqPermissions[1]).assertEqual("ohos.permission.KEEP_BACKGROUND_RUNNING");
                expect(bundleInfo.reqPermissionStates.length).assertEqual(2);
                expect(bundleInfo.reqPermissionStates[0]).assertEqual(bundle.GrantStatus.PERMISSION_DENIED);
                expect(bundleInfo.reqPermissionStates[1]).assertEqual(bundle.GrantStatus.PERMISSION_GRANTED);
                done();
            }).catch((err) => {
                expect(err).assertFail();
                done();
            });
    });

    /*
     * @tc.number: SUB_BMS_APPINFO_QUERYPERMISSION_0008
     * @tc.name: getReqPermissionUpdateEntry
     * @tc.desc: get the reqPermissions and reqPermissionStates
     */
    it('getReqPermissionUpdateEntry', 0, async function (done) {
        await bundle.getBundleInfo(BUNDLE_NAME2, bundle.BundleFlag.GET_BUNDLE_WITH_REQUESTED_PERMISSION)
            .then(bundleInfo => {
                expect(bundleInfo.name).assertEqual(BUNDLE_NAME2);
                expect(bundleInfo.reqPermissions.length).assertEqual(4);
                expect(bundleInfo.reqPermissions[0]).assertEqual("ohos.permission.KEEP_BACKGROUND_RUNNING");
                expect(bundleInfo.reqPermissions[1]).assertEqual("ohos.permission.LOCATION_IN_BACKGROUND");
                expect(bundleInfo.reqPermissions[2]).assertEqual("ohos.permission.USE_BLUETOOTH");
                expect(bundleInfo.reqPermissions[3]).assertEqual("ohos.permission.VIBRATE");
                expect(bundleInfo.reqPermissionStates.length).assertEqual(4);
                expect(bundleInfo.reqPermissionStates[0]).assertEqual(bundle.GrantStatus.PERMISSION_GRANTED);
                expect(bundleInfo.reqPermissionStates[1]).assertEqual(bundle.GrantStatus.PERMISSION_DENIED);
                expect(bundleInfo.reqPermissionStates[2]).assertEqual(bundle.GrantStatus.PERMISSION_GRANTED);
                expect(bundleInfo.reqPermissionStates[3]).assertEqual(bundle.GrantStatus.PERMISSION_GRANTED);
                done();
            }).catch((err) => {
                expect(err).assertFail();
                done();
            });
    });

    /*
     * @tc.number: SUB_BMS_APPINFO_QUERYPERMISSION_0003
     * @tc.name: getReqPermissionUpdateEntryAndFeature
     * @tc.desc: get the reqPermissions and reqPermissionStates
     */
    it('getReqPermissionUpdateEntryAndFeature', 0, async function (done) {
        await bundle.getBundleInfo(BUNDLE_NAME1, bundle.BundleFlag.GET_BUNDLE_WITH_REQUESTED_PERMISSION)
            .then(bundleInfo => {
                expect(bundleInfo.name).assertEqual(BUNDLE_NAME1);
                expect(bundleInfo.reqPermissions.length).assertEqual(5);
                expect(bundleInfo.reqPermissions[0]).assertEqual("ohos.permission.DISTRIBUTED_DATASYNC");
                expect(bundleInfo.reqPermissions[1]).assertEqual("ohos.permission.KEEP_BACKGROUND_RUNNING");
                expect(bundleInfo.reqPermissions[2]).assertEqual("ohos.permission.LOCATION_IN_BACKGROUND");
                expect(bundleInfo.reqPermissions[3]).assertEqual("ohos.permission.USE_BLUETOOTH");
                expect(bundleInfo.reqPermissions[4]).assertEqual("ohos.permission.VIBRATE");
                expect(bundleInfo.reqPermissionStates.length).assertEqual(5);
                expect(bundleInfo.reqPermissionStates[0]).assertEqual(bundle.GrantStatus.PERMISSION_DENIED);
                expect(bundleInfo.reqPermissionStates[1]).assertEqual(bundle.GrantStatus.PERMISSION_GRANTED);
                expect(bundleInfo.reqPermissionStates[2]).assertEqual(bundle.GrantStatus.PERMISSION_DENIED);
                expect(bundleInfo.reqPermissionStates[3]).assertEqual(bundle.GrantStatus.PERMISSION_GRANTED);
                expect(bundleInfo.reqPermissionStates[4]).assertEqual(bundle.GrantStatus.PERMISSION_GRANTED);
                done();
            }).catch((err) => {
                expect(err).assertFail();
                done();
            });
    });

    /**
     * @tc.number BMS_SECURITY_INITIALIZE_0013
     * @tc.name getAccessTokenIdWithDefault
     * @tc.desc Test getBundleInfos interfaces with with a flag
     */
    it("getAccessTokenIdWithDefault", 0, async function (done) {
        await bundle.getApplicationInfo(BUNDLE_NAME1, bundle.BundleFlag.GET_BUNDLE_DEFAULT)
            .then((applicationInfo) => {
                console.info("getAccessTokenIdWithDefault accessTokenId: " + applicationInfo.accessTokenId);
                expect(applicationInfo.name).assertEqual(BUNDLE_NAME1);
                expect(applicationInfo.accessTokenId).assertLarger(0);
                done();
            }).catch((err) => {
                expect(err).assertFail();
                done();
            });
    });

    /**
     * @tc.number BMS_SECURITY_INITIALIZE_0014
     * @tc.name getAccessTokenIdWithGetAbilities
     * @tc.desc Test getBundleInfos interfaces with a flag
     */
    it("getAccessTokenIdWithGetAbilities", 0, async function (done) {
        await bundle.getApplicationInfo(BUNDLE_NAME1, bundle.BundleFlag.GET_BUNDLE_WITH_ABILITIES)
            .then((applicationInfo) => {
                console.info("getAccessTokenIdWithGetAbilities accessTokenId: " + applicationInfo.accessTokenId);
                expect(applicationInfo.name).assertEqual(BUNDLE_NAME1);
                expect(applicationInfo.accessTokenId).assertLarger(0);
                done();
            }).catch((err) => {
                expect(err).assertFail();
                done();
            });
    });

    /**
     * @tc.number BMS_SECURITY_INITIALIZE_0015
     * @tc.name getAccessTokenIdWithGetPermission
     * @tc.desc Test getBundleInfos interfaces with a flag
     */
    it("getAccessTokenIdWithGetPermission", 0, async function (done) {
        await bundle.getApplicationInfo(BUNDLE_NAME1, bundle.BundleFlag.GET_APPLICATION_INFO_WITH_PERMISSION)
            .then((applicationInfo) => {
                console.info("getAccessTokenIdWithGetPermission accessTokenId: " + applicationInfo.accessTokenId);
                expect(applicationInfo.name).assertEqual(BUNDLE_NAME1);
                expect(applicationInfo.accessTokenId).assertLarger(0);
                done();
            }).catch((err) => {
                expect(err).assertFail();
                done();
            });
    });

});
}
