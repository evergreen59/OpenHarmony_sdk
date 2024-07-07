import Audio from '@ohos.multimedia.audio';
import abilityAccessCtrl from '@ohos.abilityAccessCtrl';
import bluetooth from '@ohos.bluetooth';
import DataShareExtensionAbility from '@ohos.application.DataShareExtensionAbility';
import rpc from '@ohos.rpc';
import settings from '@ohos.settings';
import SettingsDataConfig from '../Utils/SettingsDataConfig';
import SettingsDBHelper from '../Utils/SettingsDBHelper';
import { Log } from '../Utils/Log';
import process from '@ohos.process';

let rdbStore;
let requests:any[] = [];

function DoSystemSetting(settingsKey: string, settingsValue: string) {
    switch (settingsKey) {
        case SettingsDataConfig.SettingsKey.SETTINGS_AUDIO_RINGTONE: {
            try {
                let volumeType = Audio.AudioVolumeType.RINGTONE;
                Log.I('settings RINGTONE start')
                Audio.getAudioManager().setVolume(volumeType, parseInt(settingsValue)).then(() => {
                    Log.I('settings Promise returned to indicate a successful RINGTONE setting.')
                });
            } catch (err) {
                Log.I('settings RINGTONE failed error = ' + JSON.stringify(err));
            }
        }
            break
        case SettingsDataConfig.SettingsKey.SETTINGS_AUDIO_MEDIA: {
            try {
                let volumeType = Audio.AudioVolumeType.MEDIA;
                Log.I('settings MEDIA start')
                Audio.getAudioManager().setVolume(volumeType, parseInt(settingsValue)).then(() => {
                    Log.I('settings Promise returned to indicate a successful MEDIA setting.')
                });
            } catch (err) {
                Log.I('settings MEDIA failed error = ' + JSON.stringify(err));
            }
        }
            break
        case SettingsDataConfig.SettingsKey.SETTINGS_AUDIO_VOICE_CALL: {
            try {
                let volumeType = Audio.AudioVolumeType.VOICE_CALL;
                Log.I('settings VOICE_CALL start')
                Audio.getAudioManager().setVolume(volumeType, parseInt(settingsValue)).then(() => {
                    Log.I('settings Promise returned to indicate a successful VOICE_CALL setting.')
                });
            } catch (err) {
                Log.I('settings VOICE_CALL failed error = ' + JSON.stringify(err));
            }
        }
            break
        case settings.general.DEVICE_NAME: {
            try {
                let result = bluetooth.setLocalName(settingsValue);
                Log.I('settings bluetooth_name res = ' + result)
            } catch (err) {
                Log.I('settings bluetooth_name failed, error = ' + JSON.stringify(err));
            }
        }
            break
        default: {
            Log.I(settingsKey + ' key is not brightness or audio or deviceName');
        }
    }
}

function verifyPermission(callBack) {
    if (process.uid == rpc.IPCSkeleton.getCallingUid()) {
        callBack(true);
        return;
    }
    try {
        let tokenID = rpc.IPCSkeleton.getCallingTokenId();
        Log.I('tokenID = ' + tokenID);
        let grantStatus = abilityAccessCtrl.createAtManager().verifyAccessToken(tokenID, "ohos.permission.MANAGE_SECURE_SETTINGS");
        grantStatus.then(data => {
            if (data == abilityAccessCtrl.GrantStatus.PERMISSION_GRANTED) {
                Log.I('MANAGE_SECURE_SETTINGS active');
                callBack(true);
            } else {
                Log.W('MANAGE_SECURE_SETTINGS grantStatus= ' + JSON.stringify(data));
                callBack(false);
            }
        }).catch((err) => {
            Log.E('tokenID = ' + tokenID + ' verifyAccessToken is failed: ' + JSON.stringify(err));
            callBack(false);
        })
    } catch (err) {
        Log.E('err = ' + JSON.stringify(err));
        callBack(false);
    }
}

function isTrustList(keyWord: String) {
    let ret = -1;
    let trustList: Array<String> = [
        settings.display.SCREEN_BRIGHTNESS_STATUS,
        settings.display.AUTO_SCREEN_BRIGHTNESS,
        settings.display.SCREEN_OFF_TIMEOUT
    ];
    ret = trustList.findIndex((trustItem) => {
        return trustItem === keyWord
    })
    return ret != -1;
}

export default class UserDsExtAbility extends DataShareExtensionAbility {
    onCreate(want) {
        console.info('[ttt] [DataShareTest] <<Extension>> DataShareExtAbility onCreate, want:' + want.abilityName);
        console.info('[ttt] [DataShareTest] <<Extension>> this.context:' + this.context);
        globalThis.abilityContext = this.context;
        this.onInitialized(null);
    }

    onInitialized (abilityInfo) {
        Log.I('onInitialize getContext start');
        let context = globalThis.abilityContext;
        if(context != null) {
            SettingsDBHelper.getInstance().getRdbStore().then((rdb: any) => {
                Log.I(" onInitialized rdb:" + rdb);
                rdbStore = rdb;
                Log.I(" onInitialized requests:" + JSON.stringify(requests));
                for (let i = 0; i < requests.length; i++) {
                    let opt: string = requests[i]["operation"];
                    let columns = requests[i]["columns"];
                    let predicates = requests[i]["predicates"];
                    let value = requests[i]["value"];
                    if (opt == "insert") {
                        rdbStore.insert(SettingsDataConfig.TABLE_NAME, value, function (err, ret) {
                            Log.I('onInitialized insert ret: ' + ret);
                        });
                    } else if (opt == "query") {
                        rdbStore.query(predicates, columns, function (err, resultSet) {
                            Log.I('onInitialized query ret: ' + JSON.stringify(resultSet));
                        });
                    } else if (opt == "update") {
                        rdbStore.update(value, predicates, function (err, ret) {
                            Log.I('onInitialized update ret: ' + ret);
                        });
                    }
                }
            }).catch(err => {
                Log.E('onInitialize failed:'+JSON.stringify(err));
            })
            Log.I('onInitialize end');
        } else {
            Log.I('onInitialize context error!');
        }
    }

    insert(uri, value, callback) {
        Log.I('insert keyword = ' + value["KEYWORD"] + ' start:' + uri);
        let rdbInsert = (GrantStatus) => {
            if (!GrantStatus) {
                callback(-1, 0);
                return;
            }
            DoSystemSetting(value["KEYWORD"], value["VALUE"]);
            if (rdbStore == null) {
                let request = {
                    "operation": "insert", "columns": null, "predicates": null, value: value
                };
                Log.I('insert request = ' + JSON.stringify(request));
                requests.push(request);
                callback(-1, 0);
            } else {
                rdbStore.insert(SettingsDataConfig.TABLE_NAME, value, function (err, ret) {
                    Log.I('insert before callback ');
                    callback(err, ret);
                    Log.I('insert after callback ');
                    Log.I('insert result: ' + ret);
                });
            }
        }

        try {
            if (isTrustList(value["KEYWORD"])) {
                Log.I('trustList data exists.');
                rdbInsert(true);
            } else {
                Log.I('Start to verify permissions.');
                verifyPermission(rdbInsert);
            }
        } catch (err) {
            Log.E('Insert Data error:' + JSON.stringify(err));
            callback(-1, 0);
        }
    }

    update(uri: string, predicates, value, callback) {
        Log.I('update keyword = ' + value["KEYWORD"] + ' start:' + uri);
        let rdbUpData = (GrantStatus) => {
            if (!GrantStatus) {
                callback(-1, 0);
                return;
            }
            DoSystemSetting(value["KEYWORD"], value["VALUE"]);
            if (rdbStore == null) {
                let request = {"operation":"update", "columns" : null, "predicates" : predicates, value : value};
                Log.I('update request = '+ JSON.stringify(request));
                requests.push(request);
                callback(-1, 0);
            } else {
                rdbStore.update(SettingsDataConfig.TABLE_NAME, value, predicates, function (err, ret) {
                    Log.I('update before callback ' + JSON.stringify(err));
                    callback(err, ret);
                    Log.I('update after callback ' + JSON.stringify(ret));
                    Log.I('update result: ' + ret);
                });
            }
        }
        try {
            if (isTrustList(value["KEYWORD"])) {
                Log.I('trustList data exists.');
                rdbUpData(true);
            } else {
                Log.I('Start to verify permissions.');
                verifyPermission(rdbUpData);
            }
        } catch (err) {
            Log.E('upData error:' + JSON.stringify(err));
            callback(-1, 0);
        }
    }

    delete(uri: string, predicates, callback) {
        Log.I('nothing to do');
    }

    query(uri: string, predicates, columns: Array<string>, callback) {
        Log.I( 'query start uri:' + uri);
        if (rdbStore == null) {
            let request= {"operation":"query", "columns" : columns, "predicates" : predicates, value:""};
            Log.I('query request = '+ JSON.stringify(request));
            requests.push(request);
            callback(-1, {"_napiwrapper":{}});
        } else {
            rdbStore.query(SettingsDataConfig.TABLE_NAME, predicates, columns, function (err, resultSet) {
                Log.I('query before callback err:' + JSON.stringify(err) + " ResultSet" + JSON.stringify(resultSet));
                callback(err, resultSet);
                Log.I('query after callback ');
                Log.I('query result: '+ JSON.stringify(resultSet.rowCount) +'columnNames'+ JSON.stringify(resultSet.columnNames));
            });
        }
    }
}