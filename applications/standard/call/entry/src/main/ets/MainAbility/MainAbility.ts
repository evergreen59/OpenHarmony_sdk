import Ability from '@ohos.app.ability.UIAbility'
import LogUtils from '../common/utils/LogUtils'
import CallManager from '../model/CallManager'
import GlobalThisHelper from '../common/utils/GlobalThisHelper'
import Constants from '../common/utils/Constants'

const TAG = "MainAbility";

export default class MainAbility extends Ability {
  onCreate(want, launchParam) {
    LogUtils.i(TAG, "onCreate")
    GlobalThisHelper.set<any>(Constants.GLOBALTHIS_ABILITY_WANT, want);
    GlobalThisHelper.set<any>(Constants.GLOBALTHIS_CONTEXT, this.context);
  }

  onWindowStageCreate(windowStage) {
    // Main window is created, set main page for this ability
    LogUtils.i(TAG, "onWindowStageCreate")
    windowStage.setShowOnLockScreen(true);
    windowStage.loadContent("pages/index", (err, data) => {
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

  onDestroy() {
    LogUtils.i(TAG, "onDestroy")
    CallManager.getInstance()?.clearTimer();
  }
};
