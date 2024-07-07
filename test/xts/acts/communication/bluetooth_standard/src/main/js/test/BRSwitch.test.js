/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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

import bluetooth from '@ohos.bluetooth';
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from '@ohos/hypium'

export default function bluetoothhostTest() {
describe('bluetoothhostTest', function() {
    function sleep(delay) {
        return new Promise(resovle => setTimeout(resovle, delay))
    }
    async function tryToEnableBt() {
        let sta = bluetooth.getState();
        switch(sta){
            case 0:
                bluetooth.enableBluetooth();
                await sleep(10000);
                let sta1 = bluetooth.getState();
                console.info('[bluetooth_js] Reacquire bt state:'+ JSON.stringify(sta1));
                break;
            case 1:
                console.info('[bluetooth_js] bt turning on:'+ JSON.stringify(sta));
                await sleep(3000);
                break;
            case 2:
                console.info('[bluetooth_js] bt turn on:'+ JSON.stringify(sta));
                break;
            case 3:
                bluetooth.enableBluetooth();
                await sleep(10000);
                let sta2 = bluetooth.getState();
                console.info('[bluetooth_js] bt turning off:'+ JSON.stringify(sta2));
                break;
            default:
                console.info('[bluetooth_js] enable success');
        }
    }
    beforeAll(function () {
        console.info('beforeAll called')
    })
    beforeEach(async function(done) {
        console.info('beforeEach called')
        await tryToEnableBt()
        done()
    })
    afterEach(function () {
        console.info('afterEach called')
    })
    afterAll(function () {
        console.info('afterAll called')
    })


    /**
     * @tc.number SUB_COMMUNICATION_BLUETOOTH_BR_Switch_0300
     * @tc.name testEnableBluetooth
     * @tc.desc Test EnableBluetooth api by promise.
     * @tc.size MEDIUM
     * @ since 7
     * @tc.type Function
     * @tc.level Level 3
     */
    it('SUB_COMMUNICATION_BLUETOOTH_BR_Switch_0300', 0, async function (done) {
        let enable = bluetooth.enableBluetooth();
        let state = bluetooth.getState();
        console.info('[bluetooth_js] bt open state1 = '+ JSON.stringify(state));
        expect(state).assertEqual(bluetooth.BluetoothState.STATE_ON);
        if(state==bluetooth.BluetoothState.STATE_ON) {
            let enable1=bluetooth.enableBluetooth();
            await sleep(10000);
            console.info('[bluetooth_js]enable1 :'+ JSON.stringify(enable1));
            expect(enable1).assertFalse();
            let state1 = bluetooth.getState();
            console.info('[bluetooth_js] bt open state2='+ JSON.stringify(state1));
            expect(state1).assertEqual(bluetooth.BluetoothState.STATE_ON);
        }else{
            console.info('[bluetooth_js]bluetooth open fail');
        } 
        done();
    })

    /**
     * @tc.number SUB_COMMUNICATION_BLUETOOTH_BR_Switch_0100
     * @tc.name testEnableBluetooth and getState
     * @tc.desc Test EnableBluetooth api by promise.
     * @tc.size MEDIUM
     * @ since 7
     * @tc.type Function
     * @tc.level Level 0
     */
    it('SUB_COMMUNICATION_BLUETOOTH_BR_Switch_0100', 0, async function (done) {
        function onReceiveEvent(data) {
            console.info('bluetooth state001 ='+ JSON.stringify(data));
        }
        await bluetooth.on('stateChange', onReceiveEvent);
        let state = bluetooth.getState();
        console.info('[bluetooth_js] get bluetooth state001'+ JSON.stringify(state));
        if(state!=bluetooth.BluetoothState.STATE_ON)
        {
            let enable = bluetooth.enableBluetooth();
            await sleep(10000);
            console.info('[bluetooth_js] bluetooth enable001'+JSON.stringify(enable));
            expect(enable).assertTrue();
            let state1 = bluetooth.getState();
            console.info('[bluetooth_js] enable state001 '+ JSON.stringify(state1));
            expect(state1).assertEqual(bluetooth.BluetoothState.STATE_ON);
        }
        done()
    })

    /**
     * @tc.number COMMUNICATION_BLUETOOTH_SwitchOff_0444
     * @tc.name testEnableBluetooth and getState
     * @tc.desc Test EnableBluetooth api by promise.
     * @tc.size MEDIUM
     * @tc.type Function
     * @tc.level Level 0
     */
    it('COMMUNICATION_BLUETOOTH_SwitchOff_0444', 0, async function (done) {
        let STATE_ON = bluetooth.BluetoothState.STATE_ON;
        console.info("[bluetooth_js]STATE_ON : " + JSON.stringify(STATE_ON));
        expect(true).assertTrue(STATE_ON == 2);
        let STATE_OFF = bluetooth.BluetoothState.STATE_OFF;
        console.info("[bluetooth_js]STATE_OFF : " + JSON.stringify(STATE_OFF));
        expect(true).assertTrue(STATE_OFF == 0);
        let STATE_TURNING_ON = bluetooth.BluetoothState.STATE_TURNING_ON;
        console.info("[bluetooth_js]STATE_TURNING_ON : " + JSON.stringify(STATE_TURNING_ON));
        expect(true).assertTrue(STATE_TURNING_ON == 1);
        let STATE_TURNING_OFF = bluetooth.BluetoothState.STATE_TURNING_OFF;
        console.info("[bluetooth_js]STATE_TURNING_OFF : " + JSON.stringify(STATE_TURNING_OFF));
        expect(true).assertTrue(STATE_TURNING_OFF == 3);
        let STATE_BLE_TURNING_ON = bluetooth.BluetoothState.STATE_BLE_TURNING_ON;
        console.info("[bluetooth_js]STATE_BLE_TURNING_ON : " + JSON.stringify(STATE_BLE_TURNING_ON));
        expect(true).assertTrue(STATE_BLE_TURNING_ON == 4);
        let STATE_BLE_ON = bluetooth.BluetoothState.STATE_BLE_ON;
        console.info("[bluetooth_js]STATE_BLE_ON : " + JSON.stringify(STATE_BLE_ON));
        expect(true).assertTrue(STATE_BLE_ON == 5);
        let STATE_BLE_TURNING_OFF = bluetooth.BluetoothState.STATE_BLE_TURNING_OFF;
        console.info("[bluetooth_js]STATE_BLE_TURNING_OFF : " + JSON.stringify(STATE_BLE_TURNING_OFF));
        expect(true).assertTrue(STATE_BLE_TURNING_OFF == 6);
        done()
    })
})
}


