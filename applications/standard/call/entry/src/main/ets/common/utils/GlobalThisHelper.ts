import LogUtils from '../utils/LogUtils'

const TAG = "GlobalThisHelper"

export default class GlobalThisHelper {
  private static registerKeys = [
    "calluiAbilityContext",
    "callManager",
    "aaa",
    'abilityWant',
    'appInactiveState'
  ];

  public static set<T>(key: string, value: T) : T {
    const element = GlobalThisHelper.registerKeys.find((ele) => ele === key);
    if (element === undefined) {
      LogUtils.i(TAG, "Cant't find register key: " + JSON.stringify(key))
      return undefined;
    }

    if (!globalThis[key]) {
      globalThis[key] = value;
    }
    LogUtils.i(TAG, "GlobalThisHelper.set succeed, key:" + JSON.stringify(key));
    return globalThis[key];
  }

  public static get<T>(key: string): T{
    if (!globalThis[key]) {
      LogUtils.i(TAG, "the key is not exist, key" + JSON.stringify(key));
      return undefined;
    }
    return (globalThis[key] as T);
  }
}