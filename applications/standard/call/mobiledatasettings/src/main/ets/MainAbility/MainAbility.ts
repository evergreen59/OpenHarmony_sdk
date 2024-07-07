import Ability from '@ohos.app.ability.UIAbility'
import Window from '@ohos.window'
import LogUtils from '../common/utils/LogUtils'

const TAG = "MainAbility";

export default class MainAbility extends Ability {
  onCreate(want, launchParam) {
    LogUtils.i(TAG, "onCreate")
    globalThis.settingsAbilityContext = this.context;
  }

  onDestroy() {
    LogUtils.i(TAG, "onDestroy")
  }

  onWindowStageCreate(windowStage: Window.WindowStage) {
    // Main window is created, set main page for this ability
    LogUtils.i(TAG, "onWindowStageCreate")

    windowStage.loadContent('pages/index', (err, data) => {
      if (err.code) {
        LogUtils.e(TAG, 'Failed to load the content. Cause:' + JSON.stringify(err));
        return;
      }
      LogUtils.e(TAG, 'Succeeded in loading the content. Data: ' + JSON.stringify(data))
    });
  }

  onWindowStageDestroy() {
    // Main window is destroyed, release UI related resources
    LogUtils.i(TAG, "onWindowStageDestroy")
  }

  onForeground() {
    // Ability has brought to foreground
    LogUtils.i(TAG, "onForeground")
  }

  onBackground() {
    // Ability has back to background
    LogUtils.i(TAG, "onBackground")
  }
}
