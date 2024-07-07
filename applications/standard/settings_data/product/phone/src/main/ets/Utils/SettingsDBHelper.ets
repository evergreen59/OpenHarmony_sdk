/**
 * @file Describe the file
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

import data_storage from '@ohos.data.storage';
import deviceInfo from '@ohos.deviceInfo';
import ohosDataRdb from '@ohos.data.rdb';
import resourceManager from '@ohos.resourceManager';
import i18n from '@ohos.i18n';
import settings from '@ohos.settings';
import systemParameter from '@ohos.systemparameter';
import SettingsDataConfig from '../Utils/SettingsDataConfig';
import { Log } from '../Utils/Log';

let DDL_TABLE_CREATE = `CREATE TABLE IF NOT EXISTS ${SettingsDataConfig.TABLE_NAME} (`
+ `${SettingsDataConfig.FIELD_ID} INTEGER PRIMARY KEY AUTOINCREMENT, `
+ `KEYWORD TEXT, `
+ `VALUE TEXT CHECK (LENGTH(VALUE)<=1000))`;

const DEFAULT_JSON_FILE_NAME: string = "default_settings.json";
const SETTINGSDATA_PREFERENCE: string = "SettingsDataPreference";

let DefaultKey = SettingsDataConfig.DefaultKey;
let SettingsKey = SettingsDataConfig.SettingsKey;

const DefalutKVGlobal = {
  "settings.screen.brightness.default": "50",
  "settings.time.format.default": "24",
  "settings.audio.ringtone.default": "5",
  "settings.audio.media.default": "5",
  "settings.audio.voicecall.default": "5",
  "settings.display.navigationbar_status.default": "1",
};

const TIME_FORMAT = {
  TIME_FORMAT_24: "24",
  TIME_FORMAT_12: "12",
}

class SettingsDBHelper {
  private firstStartupFlag: boolean;
  private rdbStore_;
  private context_;
  private static instance_: SettingsDBHelper;

  private constructor() {
    this.firstStartupFlag = false;
    this.rdbStore_ = undefined;
    this.context_ = globalThis.abilityContext;
    Log.I('call construstor');
  }

  private async firstStartupConfig(): Promise<void> {
    Log.I("firstStartupConfig start");
    let path: string = await this.context_.getFilesDir();
    let storage = await data_storage.getStorage(path + SETTINGSDATA_PREFERENCE)
    Log.I("firstStartupConfig getStorage = " + storage);
    let isExist = await storage.has('isFirstStartUp')
    Log.I("firstStartupConfig has isFirstStartUp = " + isExist);
    if (!isExist) {
      Log.I("The key of isFirstStartUp is not contained.")
      Log.I("firstStartupConfig put start");
      await storage.put('isFirstStartUp', 'true');
      Log.I("firstStartupConfig flush start");
      await storage.flush();
      Log.I("firstStartupConfig flush end");
      this.firstStartupFlag = true;
    } else {
      let startup = await storage.get('isFirstStartUp', 'null');
      if (startup != 'true') {
        Log.I("firstStartupConfig get startup = " + startup);
        this.firstStartupFlag = true;
      }
    }
    Log.I("firstStartupConfig end");
    return;
  }

  public async initRdbStore() {
    Log.I('call initRdbStore start');
    this.rdbStore_ = await ohosDataRdb.getRdbStore(this.context_, { name: SettingsDataConfig.DB_NAME }, 1);
    Log.I('call initRdbStore, rdbStore_ = ' + this.rdbStore_);

    Log.I("initRdbStore executeSql");
    await this.rdbStore_.executeSql(DDL_TABLE_CREATE, []);

    Log.I('getRdbStore firstStartupConfig');
    await this.firstStartupConfig();

    if (this.firstStartupFlag) {
      Log.I('initRdbStore getDefaultSettingsKV');
      let defaultKV = await this.getDefaultSettingsKV();
      Log.I('initRdbStore loadDefaultSettings');
      await this.loadDefaultSettings(defaultKV);
    } else {
      Log.I('initRdbStore not need loadDefaultSettings');
    }
    Log.I('call initRdbStore end');
    return this.rdbStore_;
  }

  public static getInstance() {
    if (!SettingsDBHelper.instance_) {
      Log.I('call getInstance start');
      SettingsDBHelper.instance_ = new SettingsDBHelper();
      Log.I('call getInstance end');
    }
    return SettingsDBHelper.instance_;
  }

  public async getRdbStore() {
    Log.I('call getRdbStore start');
    return await SettingsDBHelper.instance_.initRdbStore();
  }

  private async loadSettings(rdbStore_, key, value): Promise<void> {
    Log.I("key=" + key + " value " + value);
    await this.rdbStore_.insert(
      SettingsDataConfig.TABLE_NAME,
      { "KEYWORD": key, "VALUE": value },
      (err, ret) => {
        if (err) {
          Log.E("loadSettings insert error:" + JSON.stringify(err));
        }
        Log.I("loadSettings insert ret = " + ret);
      });
  }

  private async loadDefaultSettings(defaultKV) {
    Log.I("loadDefaultSettings start");

    await this.loadSettings(this.rdbStore_,
      settings.display.SCREEN_BRIGHTNESS_STATUS, defaultKV[DefaultKey.SETTINGS_SCREEN_BRIGHTNESS_DEFAULT]);

    await this.loadSettings(this.rdbStore_,
      settings.date.TIME_FORMAT, defaultKV[DefaultKey.SETTINGS_TIME_FORMAT_DEFAULT]);

    await this.loadSettings(this.rdbStore_,
      SettingsKey.SETTINGS_AUDIO_RINGTONE, defaultKV[DefaultKey.SETTINGS_AUDIO_RINGTONE_DEFAULT]);

    await this.loadSettings(this.rdbStore_,
      SettingsKey.SETTINGS_AUDIO_MEDIA, defaultKV[DefaultKey.SETTINGS_AUDIO_MEDIA_DEFAULT]);

    await this.loadSettings(this.rdbStore_,
      SettingsKey.SETTINGS_AUDIO_VOICE_CALL, defaultKV[DefaultKey.SETTINGS_AUDIO_VOICE_CALL_DEFAULT]);

    await this.loadSettings(this.rdbStore_,
      settings.general.DEVICE_NAME, defaultKV[DefaultKey.SETTINGS_GENERAL_DEVICE_NAME_DEFAULT]);

    await this.loadSettings(this.rdbStore_,
      SettingsKey.SETTINGS_DISPALY_NAVBAR_STATUS, defaultKV[DefaultKey.SETTINGS_DISPALY_NAVBAR_STATUS_DEFAULT]);

    Log.I("loadDefaultSettings end");
    return;
  }

  private async readDefaultFile(): Promise<string> {
    Log.I("readDefaultFile start");
    let rawStr: string = "";
    try {
      let resManager = await resourceManager.getResourceManager();
      Log.I("readDefaultFile resManager=" + resManager);
      let content = await resManager.getRawFile(DEFAULT_JSON_FILE_NAME);
      rawStr = String.fromCharCode.apply(null, content);
    } catch (err) {
      Log.E("readDefaultFile readRawFile err" + err);
    }
    Log.I("readDefaultFile end");
    return rawStr;
  }

  private async getDefaultSettingsKV() {
    Log.I("getDefaultSettingsKV start");
    let defaultKV = {};
    try {
      let content: string = await this.readDefaultFile();
      if (!content) {
        Log.I("getDefaultSettingsKV readDefaultFile err! content = " + content);
        throw new Error("readfile error!")
      }
      let defaultJson = JSON.parse(content);
      Log.I("getDefaultSettingsKV parse = " + JSON.stringify(defaultJson));
      for (var index = 0; index < defaultJson.settings.length; index++) {
        defaultKV[defaultJson.settings[index].name] = defaultJson.settings[index].value;
      }
    } catch (err) {
      Log.E("getDefaultSettingsKV catch err! err = " + err);
      defaultKV = DefalutKVGlobal;
    }
    let deviceName: string = deviceInfo.marketName;
    if (deviceName.startsWith('"') && deviceName.endsWith('"')) {
      deviceName = JSON.parse(deviceName);
    }
    defaultKV[DefaultKey.SETTINGS_GENERAL_DEVICE_NAME_DEFAULT] = deviceName;

    if (i18n.is24HourClock()) {
      defaultKV[DefaultKey.SETTINGS_TIME_FORMAT_DEFAULT] = TIME_FORMAT.TIME_FORMAT_24;
    } else {
      defaultKV[DefaultKey.SETTINGS_TIME_FORMAT_DEFAULT] = TIME_FORMAT.TIME_FORMAT_12;
    }

    let defaultBrightness = systemParameter.getSync('const.display.brightness.default');
    if (defaultBrightness.length > 0) {
      defaultKV[DefaultKey.SETTINGS_SCREEN_BRIGHTNESS_DEFAULT] = defaultBrightness;
    }
    return defaultKV;
  }
}

export default SettingsDBHelper;
