/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

import router from '@system.router'
import featureAbility from '@ohos.ability.featureAbility';

export default {
    data: {
        title: ""
    },

    onInit() {
        this.grantPermission();
    },
    onclick: function () {
        router.replace({
            uri: "pages/second/second"
        })
    },
    grantPermission() {
        console.info('Calc[IndexPage] grantPermission')
        let context = featureAbility.getContext()
        context.requestPermissionsFromUser(['ohos.permission.DISTRIBUTED_DATASYNC'], 666, function (result) {
            console.info('Calc[IndexPage] grantPermission,requestPermissionsFromUser,'+result.requestCode)

        })
    },

}


