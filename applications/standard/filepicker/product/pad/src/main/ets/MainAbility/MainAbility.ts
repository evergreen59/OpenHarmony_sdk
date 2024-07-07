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

import UIAbility from '@ohos.app.ability.UIAbility';
import { Callback } from 'basic';
import { logInfo, logError } from '../../../../../../common/src/main/ets/components/Utils/LogUtils';

const TAG: string = 'MainAbility'

export default class MainAbility extends UIAbility {
	onCreate(want, launchParam) {
		logInfo(TAG, "onCreate")
		globalThis.abilityWant = want;

		globalThis.startMode = want.parameters.startMode
		globalThis.saveFile = want.parameters.saveFile
		globalThis.debugMode = want.parameters.debugMode
		logInfo(TAG, `parameters ${JSON.stringify(want.parameters)}`)
	}

	onDestroy() {
		logInfo(TAG, "onDestroy")
	}

	onWindowStageCreate(windowStage) {
		// Main window is created, set main page for this ability
		logInfo(TAG, "onWindowStageCreate")

		globalThis.context = this.context
		this.requestPermissions(() => this.displayWindow(windowStage))
	}

	onWindowStageDestroy() {
		// Main window is destroyed, release UI related resources
		logInfo(TAG, "onWindowStageDestroy")
	}

	onForeground() {
		// Ability has brought to foreground
		logInfo(TAG, "onForeground")
	}

	onBackground() {
		// Ability has back to background
		logInfo(TAG, "onBackground")
	}

	private requestPermissions(callback: Callback<void>) {
		let permissionList: Array<string> = [
			"ohos.permission.MEDIA_LOCATION",
			"ohos.permission.READ_MEDIA",
			"ohos.permission.WRITE_MEDIA"
		]
		globalThis.context.requestPermissionsFromUser(permissionList).then(function (data) {
			logInfo(TAG, `request permission data result = ${data.authResults}`)
			callback()
		}, (error) => {
			logError(TAG, `fail to request permission error code = ${error.code}`)
		})
	}

	private displayWindow(windowStage) {
		logInfo(TAG, "displayWindow()")
		windowStage.setUIContent(this.context, "pages/index", null)
		windowStage.getMainWindow().then(win => {
			logInfo(TAG, "windowStage.getMainWindow()")
			win.resetSize(vp2px(752), vp2px(450))
		})
	}
};
