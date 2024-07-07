import AbilityStage from "@ohos.app.ability.AbilityStage"
import deviceInfo from '@ohos.deviceInfo';
import LogUtils from '../common/utils/LogUtils'
import notification from '@ohos.notification';

const TAG = "MyAbilityStage";

export default class MyAbilityStage extends AbilityStage {
  onCreate() {
    notification.enableNotification({
      bundle: "com.ohos.callui"
    }, true, (err, data) => {
      if (err) {
        LogUtils.e(TAG, "enableNotification err: " + JSON.stringify(err));
      }
    })
    LogUtils.i(TAG, "MyAbilityStage onCreate")
  }

  onDestroy() {
    LogUtils.i(TAG, "MyAbilityStage onDestroy")
  }
}

