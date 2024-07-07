import UIAbility from '@ohos.app.ability.UIAbility'
import display from '@ohos.display'
import { Callback } from 'basic'
import { logInfo, logError, logWarn } from '../../../../../../common/src/main/ets/components/Utils/LogUtils'

let displayWidth: number = 0
let displayHeight: number = 0

const TAG = "MainAbility"

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
			logInfo(TAG, 'request permission data result = ' + data.authResults)
			callback()
		}, (error) => {
			logError(TAG, 'fail to request permission error code = ' + error.code)
		})
	}

	private displayWindow(windowStage) {
		display.getDefaultDisplay().then(dis => {
			displayWidth = dis.width
			displayHeight = dis.height

			globalThis.width = dis.width
			globalThis.height = dis.height
			globalThis.mainDialogWidth = dis.width
			globalThis.mainDialogHeight = (((displayHeight) - 180) * 0.7) / 1.3

			logInfo(TAG, "displayWidth = " + displayWidth + " displayHeight = " + displayHeight)

			windowStage.getMainWindow().then(win => {
				logInfo(TAG, "windowStage.getMainWindow()")

				win.resetSize(displayWidth, displayHeight - 120)

				win.moveTo(0, 0)

				win.setBackgroundColor("#00FFFFFF", (err, data) => {
					if (err.code) {
						logWarn(TAG, "Fail to set the background color" + JSON.stringify(err))
					} else {
						logInfo(TAG, "Success to set the background color" + JSON.stringify(data))
					}
				})

				win.disableWindowDecor((err, data) => {
					if (err.code) {
						logWarn(TAG, "Fail to disable Window Decor" + JSON.stringify(err))
					} else {
						logInfo(TAG, "Success to disable Window Decor" + JSON.stringify(data))
					}
				})

				win.setWindowMode(102, (err, data) => {
					if (err.code) {
						logWarn(TAG, 'Failed to set the setWindowMode. Data: ' + JSON.stringify(data))
					} else {
						logInfo(TAG, 'Succeeded in setting the setWindowMode. Data: ' + JSON.stringify(data))
					}
				})

				windowStage.setUIContent(this.context, "pages/index", null)
			})
		})
	}
};
