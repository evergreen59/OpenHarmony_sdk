import AbilityStage from "@ohos.app.ability.AbilityStage"
import LogUtils from '../common/utils/LogUtils'

const TAG = "MyAbilityStage";

export default class MyAbilityStage extends AbilityStage {
  onCreate() {
    LogUtils.i(TAG, "onWindowStageDestroy")
  }
}