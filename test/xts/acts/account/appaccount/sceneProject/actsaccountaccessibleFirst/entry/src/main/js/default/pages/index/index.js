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
import account from '@ohos.account.appAccount'
import featureAbility from '@ohos.ability.featureAbility'

const injectRef = Object.getPrototypeOf(global) || global
injectRef.regeneratorRuntime = require('@babel/runtime/regenerator')

export default {
    data: {
    },
    onInit() {
        this.title = "scene accessible first application";
    },
    onShow() {
        console.debug('====>scene accessible first application start====');
        var appAccountManager = account.createAppAccountManager();
        console.debug("====>creat first scene manager finish====");
        var enableBundle = "com.example.actsgetallaccessiblemultiple";
        var enableBundle_2 = "com.example.getmultipleaccountstest"
        console.debug("====>add first account start====");
        appAccountManager.createAccount("account_name_scene_first_first", (err)=>{
            console.debug("====>add first account err:" + JSON.stringify(err));
            appAccountManager.setAppAccess("account_name_scene_first_first", enableBundle, true, (err)=>{
                console.debug("====>enableAppAccess first account err:" + JSON.stringify(err));
                appAccountManager.setAppAccess("account_name_scene_first_first", enableBundle_2, true, (err) => {
                    console.debug("====>enableAppAccess first account for enableBundle_2 err:" + JSON.stringify(err));
                    appAccountManager.createAccount("account_name_scene_first_second", (err)=>{
                        console.debug("====>add second account err:" + JSON.stringify(err));
                        appAccountManager.setAppAccess("account_name_scene_first_second", enableBundle, true, (err)=>{
                            appAccountManager.setAppAccess("account_name_scene_first_second", enableBundle_2, true, (err)=>{
                                console.debug("====>enableAppAccess second account err:" + JSON.stringify(err));
                                console.debug("====>startAbility second start====");
                                featureAbility.startAbility(
                                    {
                                        want:
                                        {
                                            deviceId: "",
                                            bundleName: "com.example.actsaccountaccessiblesecond",
                                            abilityName: "com.example.actsaccountaccessiblesecond.MainAbility",
                                            action: "action1",
                                            parameters:
                                            {},
                                        },
                                    }
                                );
                            })
                        })
                    })
                })
            })  
        })
    },
    onReady() {
    },
}