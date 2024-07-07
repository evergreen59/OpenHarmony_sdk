/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from '@ohos/hypium'
import accessibility from '@ohos.accessibility'

const bundleName = 'com.sample.testfora11y';
const triggerAction = 'accessibilityFocus';
const eventType = 'accessibilityFocus';

export default function AccessibleRegisterState() {
describe('AccessibleRegisterState', function () {

    beforeEach(async function (done) {
        console.info(`AccessibleRegisterState: beforeEach starts`);
        done();
    })

    afterEach(async function (done) {
        console.info(`AccessibleRegisterState: afterEach starts`);
        setTimeout(done, 1000);
    })

    /******************************************************************************** */
    /* Cases RegisterStateOnOff_0330-0360 are for accessibility.on() API test         */
    /******************************************************************************** */

    /*
    * @tc.number  RegisterStateOnOff_0330
    * @tc.name    RegisterStateOnOff_0330
    * @tc.desc    The parameter input is 'accessibilityStateChange', test the accessibility.on() function,
    *             and return void.
    * @tc.size    SmallTest
    * @tc.type    User
    */
    it('RegisterStateOnOff_0330', 0, async function (done) {
        console.info('RegisterStateOnOff_0330');
        let stateEventType = 'accessibilityStateChange';
        let ret = accessibility.on(stateEventType, (data) => {});
        expect(ret).assertEqual(undefined);
        done();
    })

    /*
    * @tc.number  RegisterStateOnOff_0340
    * @tc.name    RegisterStateOnOff_0340
    * @tc.desc    The parameter input is 'touchGuideStateChange', test the accessibility.on() function,
    *             and return void.
    * @tc.size    SmallTest
    * @tc.type    User
    */
    it('RegisterStateOnOff_0340', 0, async function (done) {
        console.info('RegisterStateOnOff_0340');
        let stateEventType = 'touchGuideStateChange';
        let ret = accessibility.on(stateEventType, (data) => {});
        expect(ret).assertEqual(undefined);
        done();
    })

    /******************************************************************************** */
    /* Cases RegisterStateOnOff_0370-0410 are for accessibility.off() API test        */
    /******************************************************************************** */

    /*
    * @tc.number  RegisterStateOnOff_0370
    * @tc.name    RegisterStateOnOff_0370
    * @tc.desc    The parameter input is 'accessibilityStateChange', test the accessibility.off() function,
    *             and return void.
    * @tc.size    SmallTest
    * @tc.type    User
    */
    it('RegisterStateOnOff_0370', 0, async function (done) {
        console.info('RegisterStateOnOff_0370');
        let stateEventType = 'accessibilityStateChange';
        let ret = accessibility.off(stateEventType, (data) => {});
        expect(ret).assertEqual(undefined);
        done();
    })

    /*
    * @tc.number  RegisterStateOnOff_0380
    * @tc.name    RegisterStateOnOff_0380
    * @tc.desc    The parameter input is 'touchGuideStateChange', test the accessibility.off() function,
    *             and return void.
    * @tc.size    SmallTest
    * @tc.type    User
    */
    it('RegisterStateOnOff_0380', 0, async function (done) {
        console.info('RegisterStateOnOff_0380');
        let stateEventType = 'touchGuideStateChange';
        let ret = accessibility.off(stateEventType, (data) => {});
        expect(ret).assertEqual(undefined);
        done();
    })
})

}
