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

import { logInfo, logWarn } from './LogUtils'

var TAG: string = 'AbilityUtils'

export function terminateSelfWithResult(resultCode: number, result: string): void {
	logInfo(TAG, 'terminateSelfWithResult')
	let startMode: string = AppStorage.Get<string>('startMode')
	var abilityResult = {
		resultCode: resultCode,
		want: {
			parameters: {
				'startMode': startMode,
				'result': result
			}
		}
	}
	globalThis.context.terminateSelfWithResult(abilityResult, (error) => {
		if (error.code) {
			logWarn(TAG, 'Operation failed. Cause: ' + JSON.stringify(error))
			return
		}
		logInfo(TAG, 'Operation succeeded')
	})
}