/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
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

#define private public

#include <sstream>
#include <string>
#include <gtest/gtest.h>

#include "app_log_wrapper.h"
#include "bundle_constants.h"
#include "bundle_extractor.h"
#include "bundle_parser.h"
#include "bundle_profile.h"
#include "common_profile.h"
#include "default_permission_profile.h"
#include "json_constants.h"
#include "module_profile.h"

using namespace testing::ext;
using namespace OHOS::AppExecFwk;
using namespace OHOS::AppExecFwk::Constants;
using namespace OHOS::AppExecFwk::ProfileReader;
namespace OHOS {
namespace {
const std::string RESOURCE_ROOT_PATH = "/data/test/resource/bms/parse_bundle/";
const std::string NEW_APP = "new";
const std::string UNKOWN_PATH = "unknown_path";
const size_t ONE = 1;
const size_t TWO = 2;
const std::string OVER_MAX_PATH_SIZE(260, 'x');
const nlohmann::json CONFIG_JSON = R"(
    {
        "app": {
            "bundleName": "com.example.hiworld.himusic",
            "vendor": "example",
            "version": {
                "code": 2,
                "name": "2.0"
            },
            "apiVersion": {
                "compatible": 3,
                "target": 3,
                "releaseType": "Beta1"
            }
        },
        "deviceConfig": {
            "default": {
                "keepAlive":true
            }
        },
        "module": {
            "package": "com.example.hiworld.himusic.entry",
            "name": ".MainApplication",
            "supportedModes": [
                "drive"
            ],
            "distro": {
                "moduleType": "entry",
                "deliveryWithInstall": true,
                "moduleName": "hap-car"
            },
            "deviceType": [
                "car"
            ],
            "shortcuts": [
                {
                    "shortcutId": "id",
                    "label": "$string:shortcut",
                    "icon": "$media:icon",
                    "intents": [
                      {
                        "targetBundle": "com.demo.hiworld.himusic",
                        "targetClass": "com.demo.hiworld.himusic.entry.MainAbility"
                      }
                    ]
                }
            ],
            "abilities": [
                {
                    "name": ".MainAbility",
                    "description": "himusic main ability",
                    "icon": "$media:ic_launcher",
                    "label": "HiMusic",
                    "launchType": "standard",
                    "orientation": "unspecified",
                    "visible": true,
                    "forms": [
                        {
                        "name": "Form_JS",
                        "description": "It's JS Form",
                        "type": "JS",
                        "colorMode": "auto",
                        "isDefault": false,
                        "jsComponentName": "card4x4",
                        "updateEnabled": true,
                        "scheduledUpdateTime": "21:05",
                        "updateDuration": 1,
                        "defaultDimension": "2*1",
                        "supportDimensions": [
                            "2*1"
                        ],
                        "landscapeLayouts": [
                            "$layout:ability_form"
                        ],
                        "portraitLayouts": [
                            "$layout:ability_form"
                        ],
                        "src": "pages/card/index",
                        "window": {
                            "designWidth": 720,
                            "autoDesignWidth": true
                        },
                        "formVisibleNotify": true,
                        "deepLink": "ability://com.example.myapplication.fa/.MainAbility",
                        "formConfigAbility": "ability://com.example.myapplication.fa/.MainAbility",
                            "metaData": {
                                "customizeData": [
                                    {
                                        "name": "originWidgetName",
                                        "value": "com.weather.testWidget"
                                    }
                                ]
                            }
                        }
                    ],
                    "skills": [
                        {
                            "actions": [
                                "action.system.home"
                            ],
                            "entities": [
                                "entity.system.home"
                            ]
                        }
                    ],
                    "type": "page",
                    "formEnabled": true
                },
                {
                    "name": ".PlayService",
                    "description": "himusic play ability",
                    "icon": "$media:ic_launcher",
                    "label": "HiMusic",
                    "launchType": "standard",
                    "orientation": "unspecified",
                    "visible": false,
                    "skills": [
                        {
                            "actions": [
                                "action.play.music",
                                "action.stop.music"
                            ],
                            "entities": [
                                "entity.audio"
                            ]
                        }
                    ],
                    "type": "service",
                    "backgroundModes": [
                        "audioPlayback"
                    ]
                },
                {
                    "name": ".UserADataAbility",
                    "type": "data",
                    "uri": "dataability://com.example.hiworld.himusic.UserADataAbility",
                    "visible": true
                }
            ],
            "reqPermissions": [
                {
                    "name": "ohos.permission.DISTRIBUTED_DATASYNC",
                    "reason": "",
                    "usedScene": {
                        "ability": [
                            "com.example.hiworld.himusic.entry.MainAbility",
                            "com.example.hiworld.himusic.entry.PlayService"
                        ],
                        "when": "inuse"
                    }
                }
            ]
        }
    }
)"_json;

const nlohmann::json CONFIG_JSON_2 = R"(
{
    "app":{
        "apiVersion":{
            "compatible":8,
            "releaseType":"Release",
            "target":8
        },
        "bundleName":"com.example.myapplication",
        "vendor":"example",
        "version":{
            "code":1000000,
            "name":"1.0.0"
        }
    },
    "deviceConfig":{
        "default":{
            "debug":true
        }
    },
    "module":{
        "abilities":[
            {
                "description":"$string:MainAbility_desc",
                "descriptionId":16777216,
                "formsEnabled":false,
                "icon":"$media:icon",
                "iconId":16777220,
                "label":"$string:MainAbility_label",
                "labelId":16777217,
                "launchType":"standard",
                "name":".MainAbility",
                "orientation":"unspecified",
                "srcLanguage":"ets",
                "srcPath":"MainAbility",
                "type":"page",
                "visible":true
            }
        ],
        "deviceType":[
            "phone"
        ],
        "distro":{
            "deliveryWithInstall":true,
            "installationFree":false,
            "moduleName":"entry",
            "moduleType":"entry",
            "virtualMachine":"ark0.0.0.2"
        },
        "js":[
            {
                "mode":{
                    "syntax":"ets",
                    "type":"pageAbility"
                },
                "name":".MainAbility",
                "pages":[
                    "pages/index"
                ],
                "window":{
                    "autoDesignWidth":false,
                    "designWidth":720
                }
            }
        ],
        "mainAbility":".MainAbility",
        "name":".entry",
        "package":"com.example.myapplication"
    }
}
)"_json;

const nlohmann::json CONFIG_JSON_3 = R"(
{
    "app":{
        "apiVersion":{
            "compatible":8,
            "releaseType":"Release",
            "target":8
        },
        "bundleName":"com.example.myapplication",
        "vendor":"example",
        "version":{
            "code":1000000,
            "name":"1.0.0"
        }
    },
    "deviceConfig":{
        "default":{
            "debug":true
        }
    },
    "module":{
        "abilities":[
            {
                "skills": [
                    {
                        "entities": [
                            "entity.system.home"
                        ],
                        "actions": [
                            "action.system.home"
                        ]
                    }
                ],
                "description":"$string:MainAbility_desc",
                "descriptionId":16777216,
                "formsEnabled":false,
                "icon":"$media:icon",
                "iconId":16777220,
                "label":"$string:MainAbility_label",
                "labelId":16777217,
                "launchType":"standard",
                "name":".MainAbility",
                "orientation":"unspecified",
                "srcLanguage":"ets",
                "srcPath":"MainAbility",
                "type":"page",
                "visible":true
            }
        ],
        "deviceType":[
            "phone"
        ],
        "distro":{
            "deliveryWithInstall":true,
            "installationFree":false,
            "moduleName":"entry",
            "moduleType":"entry",
            "virtualMachine":"ark0.0.0.2"
        },
        "js":[
            {
                "mode":{
                    "syntax":"ets",
                    "type":"pageAbility"
                },
                "name":".MainAbility",
                "pages":[
                    "pages/index"
                ],
                "window":{
                    "autoDesignWidth":false,
                    "designWidth":720
                }
            }
        ],
        "mainAbility":".MainAbility",
        "name":".entry",
        "package":"com.example.myapplication"
    }
}
)"_json;

const nlohmann::json MODULE_JSON = R"(
    {
    "app": {
        "apiReleaseType": "Beta3",
        "bundleName": "com.example.backuptest",
        "debug": true,
        "icon": "$media:app_icon",
        "iconId": 16777220,
        "label": "$string:app_name",
        "labelId": 16777216,
        "minAPIVersion": 9,
        "targetAPIVersion": 9,
        "vendor": "example",
        "versionCode": 1000000,
        "versionName": "1.0.0",
        "deviceConfigs": {
            "car": {
                "accessible": false,
                "keepAlive": false,
                "minAPIVersion": 9,
                "removable": false,
                "singleton": false,
                "userDataClearable": false
            },
            "liteWearable": {
                "accessible": false,
                "keepAlive": false,
                "minAPIVersion": 9,
                "removable": false,
                "singleton": false,
                "userDataClearable": false
            },
            "phone": {
                "accessible": false,
                "keepAlive": false,
                "minAPIVersion": 9,
                "removable": false,
                "singleton": false,
                "userDataClearable": false
            },
            "router": {
                "accessible": false,
                "keepAlive": false,
                "minAPIVersion": 9,
                "removable": false,
                "singleton": false,
                "userDataClearable": false
            },
            "smartVision": {
                "accessible": false,
                "keepAlive": false,
                "minAPIVersion": 9,
                "removable": false,
                "singleton": false,
                "userDataClearable": false
            },
            "tablet": {
                "accessible": false,
                "keepAlive": false,
                "minAPIVersion": 9,
                "removable": false,
                "singleton": false,
                "userDataClearable": false
            },
            "tv": {
                "accessible": false,
                "keepAlive": false,
                "minAPIVersion": 9,
                "removable": false,
                "singleton": false,
                "userDataClearable": false
            },
            "wearable": {
                "accessible": false,
                "keepAlive": false,
                "minAPIVersion": 9,
                "removable": false,
                "singleton": false,
                "userDataClearable": false
            },
            "default": {
                "accessible": false,
                "keepAlive": false,
                "minAPIVersion": 9,
                "removable": false,
                "singleton": false,
                "userDataClearable": false
            }
        }
    },
    "module": {
        "abilities": [
            {
                "description": "$string:MainAbility_desc",
                "descriptionId": 16777217,
                "icon": "$media:icon",
                "iconId": 16777221,
                "label": "$string:MainAbility_label",
                "labelId": 16777218,
                "name": "MainAbility",
                "launchType": "unknowlaunchType",
                "orientation": "unknoworientation",
                "skills": [
                    {
                        "actions": [
                            "action.system.home"
                        ],
                        "entities": [
                            "entity.system.home",
                            "flag.home.intent.from.system"
                        ]
                    }
                ],
                "srcEntrance": "./ets/MainAbility/MainAbility.ts",
                "visible": true
            },
            {
                "description": "$string:MainAbility_desc",
                "descriptionId": 16777217,
                "icon": "$media:icon",
                "iconId": 16777221,
                "label": "$string:MainAbility_label",
                "labelId": 16777218,
                "name": "MainAbility",
                "launchType": "unknowlaunchType",
                "orientation": "unknoworientation",
                "skills": [
                    {
                        "actions": [
                            "action.thirdparty.home"
                        ],
                        "entities": [
                            "entity.thirdparty.home"
                        ]
                    }
                ],
                "srcEntrance": "./ets/MainAbility/MainAbility.ts",
                "visible": true
            },
            {
                "description": "$string:MainAbility_desc",
                "descriptionId": 16777217,
                "icon": "$media:icon",
                "iconId": 16777221,
                "label": "$string:MainAbility_label",
                "labelId": 16777218,
                "name": "MainAbility",
                "launchType": "unknowlaunchType",
                "orientation": "unknoworientation",
                "skills": [
                    {
                        "actions": [
                            "action.system.home"
                        ],
                        "entities": [
                            "entity.thirdparty.home"
                        ]
                    }
                ],
                "srcEntrance": "./ets/MainAbility/MainAbility.ts",
                "visible": true
            },
            {
                "description": "$string:MainAbility_desc",
                "descriptionId": 16777217,
                "icon": "$media:icon",
                "iconId": 16777221,
                "label": "$string:MainAbility_label",
                "labelId": 16777218,
                "name": "MainAbility",
                "launchType": "unknowlaunchType",
                "orientation": "unknoworientation",
                "skills": [
                    {
                        "actions": [
                            "action.thirdparty.home"
                        ],
                        "entities": [
                            "entity.system.home"
                        ]
                    }
                ],
                "srcEntrance": "./ets/MainAbility/MainAbility.ts",
                "visible": true
            }
        ],
        "deliveryWithInstall": true,
        "description": "$string:entry_desc",
        "descriptionId": 16777219,
        "deviceTypes": [
            "unknowtype"
        ],
        "extensionAbilities": [
            {
                "name": "extensionAbility_A",
                "skills": [
                    {
                        "actions": [
                            "action.system.home"
                        ],
                        "entities": [
                            "entity.system.home"
                        ]
                    }
                ],
                "srcEntrance": "",
                "type": "backup"
            }
        ],
        "installationFree": false,
        "mainElement": "MainAbility",
        "name": "entry",
        "pages": "$profile:main_pages",
        "srcEntrance": "./ets/Application/AbilityStage.ts",
        "type": "unknowtype",
        "virtualMachine": "ark0.0.0.3"
    }
}
)"_json;

const nlohmann::json MODULE_JSON_2 = R"(
{
        "app": {
            "bundleName": "com.example.backuptest",
            "debug": true,
            "icon": "$media:app_icon",
            "iconId": 16777220,
            "label": "$string:app_name",
            "labelId": 16777216,
            "minAPIVersion": 9,
            "targetAPIVersion": 9,
            "vendor": "example",
            "versionCode": 1000000,
            "versionName": "1.0.0"
        },
        "module": {
            "deliveryWithInstall": true,
            "description": "$string:entry_desc",
            "descriptionId": 16777219,
            "deviceTypes": [
                "default"
            ],
            "abilities": [
                {
                    "description": "$string:MainAbility_desc",
                    "descriptionId": 16777217,
                    "icon": "$media:icon",
                    "iconId": 16777221,
                    "label": "$string:MainAbility_label",
                    "labelId": 16777218,
                    "name": "MainAbility",
                    "launchType": "unknowlaunchType",
                    "orientation": "unknoworientation",
                    "srcEntrance": "./ets/MainAbility/MainAbility.ts",
                    "visible": true
                }
            ],
            "name": "entry",
            "installationFree": false,
            "mainElement": "MainAbility",
            "pages": "$profile:main_pages",
            "srcEntrance": "./ets/Application/AbilityStage.ts",
            "type": "entry",
            "virtualMachine": "ark0.0.0.3"
        }
    }
)"_json;

const nlohmann::json MODULE_JSON_3 = R"(
{
    "app": {
            "bundleName": "com.example.backuptest",
            "debug": true,
            "icon": "$media:app_icon",
            "iconId": 16777220,
            "label": "$string:app_name",
            "labelId": 16777216,
            "minAPIVersion": 9,
            "targetAPIVersion": 9,
            "vendor": "example",
            "versionCode": 1000000,
            "versionName": "1.0.0"
        },
        "module": {
            "deliveryWithInstall": true,
            "description": "$string:entry_desc",
            "descriptionId": 16777219,
            "deviceTypes": [
                "default"
            ],
            "abilities": [
                {
                    "description": "$string:MainAbility_desc",
                    "descriptionId": 16777217,
                    "icon": "$media:icon",
                    "iconId": 16777221,
                    "label": "$string:MainAbility_label",
                    "labelId": 16777218,
                    "name": "MainAbility",
                    "launchType": "unknowlaunchType",
                    "orientation": "unknoworientation",
                    "srcEntrance": "./ets/MainAbility/MainAbility.ts",
                    "visible": true,
                    "skills": [
                        {
                            "actions": [
                                "action.system.home"
                            ],
                            "entities": [
                                "entity.system.home"
                            ]
                        }
                    ]
                }
            ],
            "name": "entry",
            "installationFree": false,
            "mainElement": "MainAbility",
            "pages": "$profile:main_pages",
            "srcEntrance": "./ets/Application/AbilityStage.ts",
            "type": "entry",
            "virtualMachine": "ark0.0.0.3"
        }
    }
)"_json;
}  // namespace

class BmsBundleParserTest : public testing::Test {
public:
    static void SetUpTestCase();
    static void TearDownTestCase();
    void SetUp();
    void TearDown();

protected:
    void GetProfileTypeErrorProps(nlohmann::json &typeErrorProps) const;
    void CheckNoPropProfileParseApp(const std::string &propKey, const ErrCode expectCode) const;
    void CheckNoPropProfileParseDeviceConfig(const std::string &propKey, const ErrCode expectCode) const;
    void CheckNoPropProfileParseModule(const std::string &propKey, const ErrCode expectCode) const;
    void CheckProfilePermission(const nlohmann::json &checkedProfileJson) const;
    void CheckProfileForms(const nlohmann::json &checkedProfileJson) const;
    void CheckProfileShortcut(const nlohmann::json &checkedProfileJson, const ErrCode expectCode) const;
    void CheckProfileModule(const nlohmann::json &checkedProfileJson, const ErrCode code) const;
    ErrCode CheckProfileDefaultPermission(const nlohmann::json &checkedProfileJson,
        std::set<DefaultPermission> &defaultPermissions) const;
protected:
    std::ostringstream pathStream_;
};

void BmsBundleParserTest::SetUpTestCase()
{}

void BmsBundleParserTest::TearDownTestCase()
{}

void BmsBundleParserTest::SetUp()
{}

void BmsBundleParserTest::TearDown()
{
    pathStream_.clear();
}

void BmsBundleParserTest::GetProfileTypeErrorProps(nlohmann::json &typeErrorProps) const
{
    typeErrorProps[PROFILE_KEY_NAME] = JsonConstants::NOT_STRING_TYPE;
    typeErrorProps[PROFILE_KEY_LABEL] = JsonConstants::NOT_STRING_TYPE;
    typeErrorProps[PROFILE_KEY_DESCRIPTION] = JsonConstants::NOT_STRING_TYPE;
    typeErrorProps[PROFILE_KEY_TYPE] = JsonConstants::NOT_STRING_TYPE;
    // bundle profile tag
    typeErrorProps[BUNDLE_PROFILE_KEY_APP] = JsonConstants::NOT_OBJECT_TYPE;
    typeErrorProps[BUNDLE_PROFILE_KEY_DEVICE_CONFIG] = JsonConstants::NOT_STRING_TYPE;
    typeErrorProps[BUNDLE_PROFILE_KEY_MODULE] = JsonConstants::NOT_OBJECT_TYPE;
    // sub BUNDLE_PROFILE_KEY_APP
    typeErrorProps[BUNDLE_APP_PROFILE_KEY_BUNDLE_NAME] = JsonConstants::NOT_STRING_TYPE;
    typeErrorProps[BUNDLE_APP_PROFILE_KEY_VENDOR] = JsonConstants::NOT_STRING_TYPE;
    typeErrorProps[BUNDLE_APP_PROFILE_KEY_VERSION] = JsonConstants::NOT_OBJECT_TYPE;
    typeErrorProps[BUNDLE_APP_PROFILE_KEY_API_VERSION] = JsonConstants::NOT_OBJECT_TYPE;
    // BUNDLE_APP_PROFILE_KEY_VERSION
    typeErrorProps[BUNDLE_APP_PROFILE_KEY_CODE] = JsonConstants::NOT_NUMBER_TYPE;
    // BUNDLE_APP_PROFILE_KEY_API_VERSION
    typeErrorProps[BUNDLE_APP_PROFILE_KEY_COMPATIBLE] = JsonConstants::NOT_NUMBER_TYPE;
    typeErrorProps[BUNDLE_APP_PROFILE_KEY_TARGET] = JsonConstants::NOT_NUMBER_TYPE;
    typeErrorProps[BUNDLE_APP_PROFILE_KEY_RELEASE_TYPE] = JsonConstants::NOT_STRING_TYPE;
    // sub BUNDLE_PROFILE_KEY_DEVICE_CONFIG
    typeErrorProps[BUNDLE_DEVICE_CONFIG_PROFILE_KEY_DEFAULT] = JsonConstants::NOT_OBJECT_TYPE;
    typeErrorProps[BUNDLE_DEVICE_CONFIG_PROFILE_KEY_PHONE] = JsonConstants::NOT_OBJECT_TYPE;
    typeErrorProps[BUNDLE_DEVICE_CONFIG_PROFILE_KEY_TABLET] = JsonConstants::NOT_OBJECT_TYPE;
    typeErrorProps[BUNDLE_DEVICE_CONFIG_PROFILE_KEY_TV] = JsonConstants::NOT_OBJECT_TYPE;
    typeErrorProps[BUNDLE_DEVICE_CONFIG_PROFILE_KEY_CAR] = JsonConstants::NOT_OBJECT_TYPE;
    typeErrorProps[BUNDLE_DEVICE_CONFIG_PROFILE_KEY_WEARABLE] = JsonConstants::NOT_OBJECT_TYPE;
    typeErrorProps[BUNDLE_DEVICE_CONFIG_PROFILE_KEY_LITE_WEARABLE] = JsonConstants::NOT_OBJECT_TYPE;
    typeErrorProps[BUNDLE_DEVICE_CONFIG_PROFILE_KEY_SMART_VISION] = JsonConstants::NOT_OBJECT_TYPE;
    // BUNDLE_DEVICE_CONFIG_PROFILE_KEY_DEFAULT
    typeErrorProps[BUNDLE_DEVICE_CONFIG_PROFILE_KEY_PROCESS] = JsonConstants::NOT_STRING_TYPE;
    typeErrorProps[BUNDLE_DEVICE_CONFIG_PROFILE_KEY_DIRECT_LAUNCH] = JsonConstants::NOT_BOOL_TYPE;
    typeErrorProps[BUNDLE_DEVICE_CONFIG_PROFILE_KEY_SUPPORT_BACKUP] = JsonConstants::NOT_BOOL_TYPE;
    typeErrorProps[BUNDLE_DEVICE_CONFIG_PROFILE_KEY_COMPRESS_NATIVE_LIBS] = JsonConstants::NOT_BOOL_TYPE;
    typeErrorProps[BUNDLE_DEVICE_CONFIG_PROFILE_KEY_NETWORK] = JsonConstants::NOT_OBJECT_TYPE;
    // BUNDLE_DEVICE_CONFIG_PROFILE_KEY_NETWORK
    typeErrorProps[BUNDLE_DEVICE_CONFIG_PROFILE_KEY_USES_CLEAR_TEXT] = JsonConstants::NOT_BOOL_TYPE;
    typeErrorProps[BUNDLE_DEVICE_CONFIG_PROFILE_KEY_SECURITY_CONFIG] = JsonConstants::NOT_OBJECT_TYPE;
    // BUNDLE_DEVICE_CONFIG_PROFILE_KEY_SECURITY_CONFIG
    typeErrorProps[BUNDLE_DEVICE_CONFIG_PROFILE_KEY_DOMAIN_SETTINGS] = JsonConstants::NOT_OBJECT_TYPE;
    // BUNDLE_DEVICE_CONFIG_PROFILE_KEY_DOMAIN_SETTINGS
    typeErrorProps[BUNDLE_DEVICE_CONFIG_PROFILE_KEY_CLEAR_TEXT_PERMITTED] = JsonConstants::NOT_BOOL_TYPE;
    typeErrorProps[BUNDLE_DEVICE_CONFIG_PROFILE_KEY_DOMAINS] = JsonConstants::NOT_ARRAY_TYPE;
    // BUNDLE_DEVICE_CONFIG_PROFILE_KEY_DOMAINS
    typeErrorProps[BUNDLE_DEVICE_CONFIG_PROFILE_KEY_SUB_DOMAINS] = JsonConstants::NOT_ARRAY_TYPE;
    // sub BUNDLE_PROFILE_KEY_MODULE
    typeErrorProps[BUNDLE_MODULE_PROFILE_KEY_PACKAGE] = JsonConstants::NOT_STRING_TYPE;
    typeErrorProps[BUNDLE_MODULE_PROFILE_KEY_SUPPORTED_MODES] = JsonConstants::NOT_ARRAY_TYPE;
    typeErrorProps[BUNDLE_MODULE_PROFILE_KEY_DEVICE_TYPE] = JsonConstants::NOT_ARRAY_TYPE;
    typeErrorProps[BUNDLE_MODULE_PROFILE_KEY_DISTRO] = JsonConstants::NOT_OBJECT_TYPE;
    typeErrorProps[BUNDLE_MODULE_PROFILE_KEY_ABILITIES] = JsonConstants::NOT_ARRAY_TYPE;
    typeErrorProps[BUNDLE_MODULE_PROFILE_KEY_JS] = JsonConstants::NOT_OBJECT_TYPE;
    typeErrorProps[BUNDLE_MODULE_PROFILE_KEY_SHORTCUTS] = JsonConstants::NOT_ARRAY_TYPE;
    typeErrorProps[BUNDLE_MODULE_PROFILE_KEY_REQ_PERMISSIONS] = JsonConstants::NOT_ARRAY_TYPE;
    typeErrorProps[BUNDLE_MODULE_PROFILE_KEY_COLOR_MODE] = JsonConstants::NOT_STRING_TYPE;
    // BUNDLE_MODULE_PROFILE_KEY_DISTRO
    typeErrorProps[BUNDLE_MODULE_PROFILE_KEY_DELIVERY_WITH_INSTALL] = JsonConstants::NOT_BOOL_TYPE;
    typeErrorProps[BUNDLE_MODULE_PROFILE_KEY_MODULE_NAME] = JsonConstants::NOT_STRING_TYPE;
    typeErrorProps[BUNDLE_MODULE_PROFILE_KEY_MODULE_TYPE] = JsonConstants::NOT_STRING_TYPE;
    // BUNDLE_MODULE_PROFILE_KEY_ABILITIES
    typeErrorProps[BUNDLE_MODULE_PROFILE_KEY_ICON] = JsonConstants::NOT_STRING_TYPE;
    typeErrorProps[BUNDLE_MODULE_PROFILE_KEY_URI] = JsonConstants::NOT_STRING_TYPE;
    typeErrorProps[BUNDLE_MODULE_PROFILE_KEY_LAUNCH_TYPE] = JsonConstants::NOT_STRING_TYPE;
    typeErrorProps[BUNDLE_MODULE_PROFILE_KEY_VISIBLE] = JsonConstants::NOT_BOOL_TYPE;
    typeErrorProps[BUNDLE_MODULE_PROFILE_KEY_PERMISSIONS] = JsonConstants::NOT_ARRAY_TYPE;
    typeErrorProps[BUNDLE_MODULE_PROFILE_KEY_SKILLS] = JsonConstants::NOT_ARRAY_TYPE;
    typeErrorProps[BUNDLE_MODULE_PROFILE_KEY_DEVICE_CAP_ABILITY] = JsonConstants::NOT_ARRAY_TYPE;
    typeErrorProps[BUNDLE_MODULE_PROFILE_KEY_ORIENTATION] = JsonConstants::NOT_STRING_TYPE;
    typeErrorProps[BUNDLE_MODULE_PROFILE_KEY_BACKGROUND_MODES] = JsonConstants::NOT_ARRAY_TYPE;
    typeErrorProps[BUNDLE_MODULE_PROFILE_KEY_READ_PERMISSION] = JsonConstants::NOT_STRING_TYPE;
    typeErrorProps[BUNDLE_MODULE_PROFILE_KEY_WRITE_PERMISSION] = JsonConstants::NOT_STRING_TYPE;
    typeErrorProps[BUNDLE_MODULE_PROFILE_KEY_DIRECT_LAUNCH] = JsonConstants::NOT_BOOL_TYPE;
    typeErrorProps[BUNDLE_MODULE_PROFILE_KEY_CONFIG_CHANGES] = JsonConstants::NOT_ARRAY_TYPE;
    typeErrorProps[BUNDLE_MODULE_PROFILE_KEY_MISSION] = JsonConstants::NOT_STRING_TYPE;
    typeErrorProps[BUNDLE_MODULE_PROFILE_KEY_TARGET_ABILITY] = JsonConstants::NOT_STRING_TYPE;
    typeErrorProps[BUNDLE_MODULE_PROFILE_KEY_MULTIUSER_SHARED] = JsonConstants::NOT_BOOL_TYPE;
    typeErrorProps[BUNDLE_MODULE_PROFILE_KEY_SUPPORT_PIP_MODE] = JsonConstants::NOT_BOOL_TYPE;
    // BUNDLE_MODULE_PROFILE_KEY_SKILLS
    typeErrorProps[BUNDLE_MODULE_PROFILE_KEY_ACTIONS] = JsonConstants::NOT_ARRAY_TYPE;
    typeErrorProps[BUNDLE_MODULE_PROFILE_KEY_ENTITIES] = JsonConstants::NOT_ARRAY_TYPE;
    typeErrorProps[BUNDLE_MODULE_PROFILE_KEY_URIS] = JsonConstants::NOT_ARRAY_TYPE;
    // BUNDLE_MODULE_PROFILE_KEY_URIS
    typeErrorProps[BUNDLE_MODULE_PROFILE_KEY_SCHEME] = JsonConstants::NOT_STRING_TYPE;
    typeErrorProps[BUNDLE_MODULE_PROFILE_KEY_HOST] = JsonConstants::NOT_STRING_TYPE;
    typeErrorProps[BUNDLE_MODULE_PROFILE_KEY_PORT] = JsonConstants::NOT_STRING_TYPE;
    typeErrorProps[BUNDLE_MODULE_PROFILE_KEY_PATH] = JsonConstants::NOT_STRING_TYPE;
    // BUNDLE_MODULE_PROFILE_KEY_JS
    typeErrorProps[BUNDLE_MODULE_PROFILE_KEY_PAGES] = JsonConstants::NOT_ARRAY_TYPE;
    typeErrorProps[BUNDLE_MODULE_PROFILE_KEY_WINDOW] = JsonConstants::NOT_OBJECT_TYPE;
    // BUNDLE_MODULE_PROFILE_KEY_WINDOW
    typeErrorProps[BUNDLE_MODULE_PROFILE_KEY_DESIGN_WIDTH] = JsonConstants::NOT_NUMBER_TYPE;
    typeErrorProps[BUNDLE_MODULE_PROFILE_KEY_AUTO_DESIGN_WIDTH] = JsonConstants::NOT_BOOL_TYPE;
    // BUNDLE_MODULE_PROFILE_KEY_SHORTCUTS
    typeErrorProps[BUNDLE_MODULE_PROFILE_KEY_SHORTCUT_ID] = JsonConstants::NOT_STRING_TYPE;
    typeErrorProps[BUNDLE_MODULE_PROFILE_KEY_SHORTCUT_WANTS] = JsonConstants::NOT_STRING_TYPE;
    // BUNDLE_MODULE_PROFILE_KEY_SHORTCUT_WANTS
    typeErrorProps[BUNDLE_MODULE_PROFILE_KEY_TARGET_CLASS] = JsonConstants::NOT_STRING_TYPE;
    typeErrorProps[BUNDLE_MODULE_PROFILE_KEY_TARGET_BUNDLE] = JsonConstants::NOT_STRING_TYPE;
    typeErrorProps[BUNDLE_DEVICE_CONFIG_PROFILE_KEY_COMPATIBLE] = JsonConstants::NOT_NUMBER_TYPE;
    typeErrorProps[BUNDLE_DEVICE_CONFIG_PROFILE_KEY_TARGET] = JsonConstants::NOT_NUMBER_TYPE;
    typeErrorProps[BUNDLE_DEVICE_CONFIG_PROFILE_KEY_REQ_VERSION] = JsonConstants::NOT_OBJECT_TYPE;
    typeErrorProps[BUNDLE_DEVICE_CONFIG_PROFILE_KEY_FLAG] = JsonConstants::NOT_STRING_TYPE;
    typeErrorProps[BUNDLE_MODULE_PROFILE_KEY_MODE] = JsonConstants::NOT_STRING_TYPE;
}

void BmsBundleParserTest::CheckNoPropProfileParseApp(const std::string &propKey, const ErrCode expectCode) const
{
    BundleProfile bundleProfile;
    InnerBundleInfo innerBundleInfo;
    std::ostringstream profileFileBuffer;

    nlohmann::json errorProfileJson = CONFIG_JSON;
    errorProfileJson[BUNDLE_PROFILE_KEY_APP].erase(propKey);
    profileFileBuffer << errorProfileJson.dump();

    BundleExtractor bundleExtractor("");
    ErrCode result = bundleProfile.TransformTo(
        profileFileBuffer, bundleExtractor, innerBundleInfo);
    EXPECT_EQ(result, expectCode);
}

void BmsBundleParserTest::CheckNoPropProfileParseDeviceConfig(
    const std::string &propKey, const ErrCode expectCode) const
{
    BundleProfile bundleProfile;
    InnerBundleInfo innerBundleInfo;
    std::ostringstream profileFileBuffer;

    nlohmann::json errorProfileJson = CONFIG_JSON;
    errorProfileJson[BUNDLE_PROFILE_KEY_DEVICE_CONFIG].erase(propKey);
    profileFileBuffer << errorProfileJson.dump();

    BundleExtractor bundleExtractor("");
    ErrCode result = bundleProfile.TransformTo(
        profileFileBuffer, bundleExtractor, innerBundleInfo);
    EXPECT_EQ(result, expectCode);
}

void BmsBundleParserTest::CheckNoPropProfileParseModule(const std::string &propKey, const ErrCode expectCode) const
{
    BundleProfile bundleProfile;
    InnerBundleInfo innerBundleInfo;
    std::ostringstream profileFileBuffer;

    nlohmann::json errorProfileJson = CONFIG_JSON;
    errorProfileJson[BUNDLE_PROFILE_KEY_MODULE].erase(propKey);
    profileFileBuffer << errorProfileJson.dump();

    BundleExtractor bundleExtractor("");
    ErrCode result = bundleProfile.TransformTo(
        profileFileBuffer, bundleExtractor, innerBundleInfo);
    EXPECT_EQ(result, expectCode);
}

void BmsBundleParserTest::CheckProfilePermission(const nlohmann::json &checkedProfileJson) const
{
    BundleProfile bundleProfile;
    InnerBundleInfo innerBundleInfo;
    std::ostringstream profileFileBuffer;

    profileFileBuffer << checkedProfileJson.dump();
    
    BundleExtractor bundleExtractor("");
    ErrCode result = bundleProfile.TransformTo(
        profileFileBuffer, bundleExtractor, innerBundleInfo);
    EXPECT_EQ(result, ERR_OK) << profileFileBuffer.str();
}

void BmsBundleParserTest::CheckProfileForms(const nlohmann::json &checkedProfileJson) const
{
    BundleProfile bundleProfile;
    InnerBundleInfo innerBundleInfo;
    std::ostringstream profileFileBuffer;

    profileFileBuffer << checkedProfileJson.dump();

    BundleExtractor bundleExtractor("");
    ErrCode result = bundleProfile.TransformTo(
        profileFileBuffer, bundleExtractor, innerBundleInfo);
    EXPECT_EQ(result, ERR_APPEXECFWK_PARSE_PROFILE_MISSING_PROP) << profileFileBuffer.str();
}

void BmsBundleParserTest::CheckProfileShortcut(const nlohmann::json &checkedProfileJson, const ErrCode expectCode) const
{
    BundleProfile bundleProfile;
    InnerBundleInfo innerBundleInfo;
    std::ostringstream profileFileBuffer;

    profileFileBuffer << checkedProfileJson.dump();

    BundleExtractor bundleExtractor("");
    ErrCode result = bundleProfile.TransformTo(
        profileFileBuffer, bundleExtractor, innerBundleInfo);
    EXPECT_EQ(result, expectCode) << profileFileBuffer.str();
}

void BmsBundleParserTest::CheckProfileModule(const nlohmann::json &checkedProfileJson, const ErrCode code) const
{
    ModuleProfile moduleProfile;
    InnerBundleInfo innerBundleInfo;
    innerBundleInfo.baseBundleInfo_->isPreInstallApp = true;
    std::ostringstream profileFileBuffer;

    profileFileBuffer << checkedProfileJson.dump();

    BundleExtractor bundleExtractor("");
    ErrCode result = moduleProfile.TransformTo(
        profileFileBuffer, bundleExtractor, innerBundleInfo);
    EXPECT_EQ(result, code) << profileFileBuffer.str();
}

ErrCode BmsBundleParserTest::CheckProfileDefaultPermission(const nlohmann::json &checkedProfileJson,
    std::set<DefaultPermission> &defaultPermissions) const
{
    DefaultPermissionProfile profile;
    std::ostringstream profileFileBuffer;
    profileFileBuffer << checkedProfileJson.dump();
    nlohmann::json jsonObject =  nlohmann::json::parse(profileFileBuffer.str(), nullptr, false);
    if (jsonObject.is_discarded()) {
        return ERR_APPEXECFWK_PARSE_BAD_PROFILE;
    }

    return profile.TransformTo(jsonObject, defaultPermissions);
}

/**
 * @tc.number: TestParse_0200
 * @tc.name: parse bundle package by config.json
 * @tc.desc: 1. system running normally
 *           2. test parse bundle failed when file is not exist by the input pathName
 */
HWTEST_F(BmsBundleParserTest, TestParse_0200, Function | SmallTest | Level0)
{
    BundleParser bundleParser;
    InnerBundleInfo innerBundleInfo;
    pathStream_ << RESOURCE_ROOT_PATH << UNKOWN_PATH << INSTALL_FILE_SUFFIX;
    ErrCode result = bundleParser.Parse(pathStream_.str(), innerBundleInfo);
    EXPECT_EQ(result, ERR_APPEXECFWK_PARSE_UNEXPECTED);
}

/**
 * @tc.number: TestParse_0600
 * @tc.name: parse bundle package by config.json
 * @tc.desc: 1. system running normally
 *           2. test parse bundle failed when prop(APP_notMustPropKeys) is not exist in the config.json
 */
HWTEST_F(BmsBundleParserTest, TestParse_0600, Function | SmallTest | Level0)
{
    std::vector<std::string> notMustPropKeys = {
        PROFILE_KEY_DESCRIPTION,
        PROFILE_KEY_LABEL,
        // sub BUNDLE_APP_PROFILE_KEY_API_VERSION
        BUNDLE_APP_PROFILE_KEY_VENDOR,
        BUNDLE_APP_PROFILE_KEY_TARGET,
        BUNDLE_APP_PROFILE_KEY_RELEASE_TYPE,
    };

    for (const auto &propKey : notMustPropKeys) {
        APP_LOGD("test not must prop %{public}s not exist", propKey.c_str());
        CheckNoPropProfileParseApp(propKey, ERR_OK);
    }
}

/**
 * @tc.number: TestParse_0700
 * @tc.name: parse bundle package by config.json
 * @tc.desc: 1. system running normally
 *           2. test parse bundle failed when prop(deviceConfig_notMustPropKeys) is not exist in the config.json
 */
HWTEST_F(BmsBundleParserTest, TestParse_0700, Function | SmallTest | Level0)
{
    std::vector<std::string> notMustPropKeys = {
        PROFILE_KEY_DESCRIPTION,
        PROFILE_KEY_LABEL,
        // sub BUNDLE_PROFILE_KEY_DEVICE_CONFIG
        BUNDLE_DEVICE_CONFIG_PROFILE_KEY_PHONE,
        BUNDLE_DEVICE_CONFIG_PROFILE_KEY_TABLET,
        BUNDLE_DEVICE_CONFIG_PROFILE_KEY_TV,
        BUNDLE_DEVICE_CONFIG_PROFILE_KEY_CAR,
        BUNDLE_DEVICE_CONFIG_PROFILE_KEY_WEARABLE,
        BUNDLE_DEVICE_CONFIG_PROFILE_KEY_LITE_WEARABLE,
        BUNDLE_DEVICE_CONFIG_PROFILE_KEY_SMART_VISION,
        // sub BUNDLE_DEVICE_CONFIG_PROFILE_KEY_DEFAULT
        BUNDLE_DEVICE_CONFIG_PROFILE_KEY_PROCESS,
        BUNDLE_DEVICE_CONFIG_PROFILE_KEY_DIRECT_LAUNCH,
        BUNDLE_DEVICE_CONFIG_PROFILE_KEY_SUPPORT_BACKUP,
        BUNDLE_DEVICE_CONFIG_PROFILE_KEY_COMPRESS_NATIVE_LIBS,
        BUNDLE_DEVICE_CONFIG_PROFILE_KEY_NETWORK,
        // sub BUNDLE_DEVICE_CONFIG_PROFILE_KEY_NETWORK
        BUNDLE_DEVICE_CONFIG_PROFILE_KEY_USES_CLEAR_TEXT,
        BUNDLE_DEVICE_CONFIG_PROFILE_KEY_SECURITY_CONFIG,
        // sub BUNDLE_DEVICE_CONFIG_PROFILE_KEY_SECURITY_CONFIG
        BUNDLE_DEVICE_CONFIG_PROFILE_KEY_DOMAIN_SETTINGS,
    };

    for (const auto &propKey : notMustPropKeys) {
        APP_LOGD("test not must prop %{public}s not exist", propKey.c_str());
        CheckNoPropProfileParseDeviceConfig(propKey, ERR_OK);
    }
}

/**
 * @tc.number: TestParse_0800
 * @tc.name: parse bundle package by config.json
 * @tc.desc: 1. system running normally
 *           2. test parse bundle failed when prop(module_notMustPropKeys) is not exist in the config.json
 */
HWTEST_F(BmsBundleParserTest, TestParse_0800, Function | SmallTest | Level0)
{
    std::vector<std::string> notMustPropKeys = {
        PROFILE_KEY_DESCRIPTION,
        PROFILE_KEY_LABEL,
        // sub BUNDLE_PROFILE_KEY_MODULE
        BUNDLE_MODULE_PROFILE_KEY_SUPPORTED_MODES,
        BUNDLE_MODULE_PROFILE_KEY_ABILITIES,
        BUNDLE_MODULE_PROFILE_KEY_JS,
        BUNDLE_MODULE_PROFILE_KEY_SHORTCUTS,
        BUNDLE_MODULE_PROFILE_KEY_DEFINE_PERMISSIONS,
        BUNDLE_MODULE_PROFILE_KEY_REQ_PERMISSIONS,
        BUNDLE_MODULE_PROFILE_KEY_COLOR_MODE,
        // sub BUNDLE_MODULE_PROFILE_KEY_ABILITIES
        BUNDLE_MODULE_PROFILE_KEY_PROCESS,
        BUNDLE_MODULE_PROFILE_KEY_ICON,
        BUNDLE_MODULE_PROFILE_KEY_URI,
        BUNDLE_MODULE_PROFILE_KEY_LAUNCH_TYPE,
        BUNDLE_MODULE_PROFILE_KEY_VISIBLE,
        BUNDLE_MODULE_PROFILE_KEY_PERMISSIONS,
        BUNDLE_MODULE_PROFILE_KEY_SKILLS,
        BUNDLE_MODULE_PROFILE_KEY_DEVICE_CAP_ABILITY,
        BUNDLE_MODULE_PROFILE_KEY_ORIENTATION,
        BUNDLE_MODULE_PROFILE_KEY_BACKGROUND_MODES,
        BUNDLE_MODULE_PROFILE_KEY_READ_PERMISSION,
        BUNDLE_MODULE_PROFILE_KEY_WRITE_PERMISSION,
        BUNDLE_MODULE_PROFILE_KEY_DIRECT_LAUNCH,
        BUNDLE_MODULE_PROFILE_KEY_CONFIG_CHANGES,
        BUNDLE_MODULE_PROFILE_KEY_MISSION,
        BUNDLE_MODULE_PROFILE_KEY_TARGET_ABILITY,
        BUNDLE_MODULE_PROFILE_KEY_MULTIUSER_SHARED,
        BUNDLE_MODULE_PROFILE_KEY_SUPPORT_PIP_MODE,
        // sub BUNDLE_MODULE_PROFILE_KEY_JS
        BUNDLE_MODULE_PROFILE_KEY_WINDOW,
        // sub BUNDLE_MODULE_PROFILE_KEY_WINDOW
        BUNDLE_MODULE_PROFILE_KEY_DESIGN_WIDTH,
        BUNDLE_MODULE_PROFILE_KEY_AUTO_DESIGN_WIDTH,
        // sub BUNDLE_MODULE_PROFILE_KEY_SHORTCUTS
        BUNDLE_MODULE_PROFILE_KEY_SHORTCUT_WANTS,
        // sub BUNDLE_MODULE_PROFILE_KEY_SHORTCUT_WANTS
        BUNDLE_MODULE_PROFILE_KEY_TARGET_CLASS,
        BUNDLE_MODULE_PROFILE_KEY_TARGET_BUNDLE,
        BUNDLE_DEVICE_CONFIG_PROFILE_KEY_COMPATIBLE,
        BUNDLE_DEVICE_CONFIG_PROFILE_KEY_TARGET,
        BUNDLE_DEVICE_CONFIG_PROFILE_KEY_REQ_VERSION,
        BUNDLE_DEVICE_CONFIG_PROFILE_KEY_FLAG,
        BUNDLE_MODULE_PROFILE_KEY_MODE,
        BUNDLE_MODULE_PROFILE_KEY_PATH,
    };

    for (const auto &propKey : notMustPropKeys) {
        APP_LOGD("test not must prop %{public}s not exist", propKey.c_str());
        CheckNoPropProfileParseModule(propKey, ERR_OK);
    }
}

/**
 * @tc.number: TestParse_0900
 * @tc.name: parse bundle package by config.json
 * @tc.desc: 1. system running normally
 *           2. test parse bundle failed when prop(configJson.app.bundleName) is not exist in the config.json
 */
HWTEST_F(BmsBundleParserTest, TestParse_0900, Function | SmallTest | Level0)
{
    std::vector<std::string> mustPropKeys = {
        BUNDLE_APP_PROFILE_KEY_BUNDLE_NAME,
    };

    for (const auto &propKey : mustPropKeys) {
        APP_LOGD("test must prop %{public}s not exist", propKey.c_str());
        CheckNoPropProfileParseApp(propKey, ERR_APPEXECFWK_PARSE_PROFILE_MISSING_PROP);
    }
}

/**
 * @tc.number: TestParse_1000
 * @tc.name: parse bundle deviceType by config.json
 * @tc.desc: 1. system running normally
 *           2. test parse bundle failed when prop(configJson.module.deviceType) is not exist in the
 *              config.json
 */
HWTEST_F(BmsBundleParserTest, TestParse_1000, Function | SmallTest | Level0)
{
    std::vector<std::string> mustPropKeys = {
        BUNDLE_MODULE_PROFILE_KEY_DEVICE_TYPE,
    };

    for (const auto &propKey : mustPropKeys) {
        APP_LOGD("test must prop %{public}s not exist", propKey.c_str());
        CheckNoPropProfileParseModule(propKey, ERR_APPEXECFWK_PARSE_PROFILE_MISSING_PROP);
    }
}

/**
 * @tc.number: TestParse_1100
 * @tc.name: parse bundle package by config.json
 * @tc.desc: 1. system running normally
 *           2. test parse bundle failed when prop(configJson.module.package,distro.moduleName) is not exist in the
 *           config.json
 */
HWTEST_F(BmsBundleParserTest, TestParse_1100, Function | SmallTest | Level0)
{
    std::vector<std::string> mustPropKeys = {
        BUNDLE_MODULE_PROFILE_KEY_PACKAGE,
        BUNDLE_MODULE_PROFILE_KEY_DISTRO,
    };

    for (const auto &propKey : mustPropKeys) {
        APP_LOGD("test must prop %{public}s not exist", propKey.c_str());
        CheckNoPropProfileParseModule(propKey, ERR_APPEXECFWK_PARSE_PROFILE_PROP_CHECK_ERROR);
    }
}

/**
 * @tc.number: TestParse_1200
 * @tc.name: parse bundle package by config.json
 * @tc.desc: 1. system running normally
 *           2. test parse bundle failed when prop(module.abilities.name) is not exist in the config.json
 */
HWTEST_F(BmsBundleParserTest, TestParse_1200, Function | SmallTest | Level0)
{
    std::vector<std::string> mustPropKeys = {
        PROFILE_KEY_NAME,
    };

    for (const auto &propKey : mustPropKeys) {
        APP_LOGD("test must prop %{public}s not exist", propKey.c_str());
        CheckNoPropProfileParseModule(propKey, ERR_OK);
    }
}

/**
 * @tc.number: TestParse_1600
 * @tc.name: parse bundle package by config.json
 * @tc.desc: 1. system running normally
 *           2. test parsing failed when an ability packet with an incorrect type in the file path
 */
HWTEST_F(BmsBundleParserTest, TestParse_1600, Function | SmallTest | Level0)
{
    BundleParser bundleParser;
    InnerBundleInfo innerBundleInfo;
    pathStream_ << RESOURCE_ROOT_PATH << "demo.error_type";
    ErrCode result = bundleParser.Parse(pathStream_.str(), innerBundleInfo);
    EXPECT_EQ(result, ERR_APPEXECFWK_PARSE_UNEXPECTED);

    pathStream_.str("");
    pathStream_ << RESOURCE_ROOT_PATH << "demo.";
    result = bundleParser.Parse(pathStream_.str(), innerBundleInfo);
    EXPECT_EQ(result, ERR_APPEXECFWK_PARSE_UNEXPECTED);

    pathStream_.str("");
    pathStream_ << RESOURCE_ROOT_PATH << "bundle_suffix_test.BUNDLE";
    result = bundleParser.Parse(pathStream_.str(), innerBundleInfo);
    EXPECT_EQ(result, ERR_APPEXECFWK_PARSE_UNEXPECTED);
}

/**
 * @tc.number: TestParse_1700
 * @tc.name: parse bundle package by config.json
 * @tc.desc: 1. system running normally
 *           2. test parsing failed when an bundle packet with a deep file path depth
 */
HWTEST_F(BmsBundleParserTest, TestParse_1700, Function | SmallTest | Level1)
{
    BundleParser bundleParser;
    InnerBundleInfo innerBundleInfo;
    pathStream_ << RESOURCE_ROOT_PATH;
    int maxDeep = 100;
    for (int i = 0; i < maxDeep; i++) {
        pathStream_ << "test/";
    }
    pathStream_ << NEW_APP << INSTALL_FILE_SUFFIX;
    ErrCode result = bundleParser.Parse(pathStream_.str(), innerBundleInfo);
    EXPECT_EQ(result, ERR_APPEXECFWK_PARSE_UNEXPECTED) << pathStream_.str();
}

/**
 * @tc.number: TestParse_1800
 * @tc.name: parse bundle package by config.json
 * @tc.desc: 1. system running normally
 *           2. test parsing failed when an bundle packet with a long path
 */
HWTEST_F(BmsBundleParserTest, TestParse_1800, Function | SmallTest | Level1)
{
    BundleParser bundleParser;
    InnerBundleInfo innerBundleInfo;
    pathStream_ << RESOURCE_ROOT_PATH;
    int maxLength = 256;
    for (int i = 0; i < maxLength; i++) {
        pathStream_ << "test/";
    }
    pathStream_ << NEW_APP << INSTALL_FILE_SUFFIX;
    ErrCode result = bundleParser.Parse(pathStream_.str(), innerBundleInfo);
    EXPECT_EQ(result, ERR_APPEXECFWK_PARSE_UNEXPECTED);
}

/**
 * @tc.number: TestParse_1900
 * @tc.name: parse bundle package by config.json
 * @tc.desc: 1. system running normally
 *           2. test parsing failed when an bundle packet with special character in the file path
 */
HWTEST_F(BmsBundleParserTest, TestParse_1900, Function | SmallTest | Level1)
{
    BundleParser bundleParser;
    InnerBundleInfo innerBundleInfo;
    pathStream_ << RESOURCE_ROOT_PATH;
    std::string specialChars = "~!@#$%^&*(){}[]:;'?<>,.|`/./+_-";
    pathStream_ << specialChars << "new" << INSTALL_FILE_SUFFIX;
    ErrCode result = bundleParser.Parse(pathStream_.str(), innerBundleInfo);
    EXPECT_EQ(result, ERR_APPEXECFWK_PARSE_UNEXPECTED);
}

/**
 * @tc.number: TestParse_2000
 * @tc.name: parse bundle package by config.json
 * @tc.desc: 1. system running normally
 *           2. test parsing failed when def-permission prop has error in the config.json
 */
HWTEST_F(BmsBundleParserTest, TestParse_2000, Function | SmallTest | Level1)
{
    nlohmann::json errorDefPermJson = CONFIG_JSON;
    errorDefPermJson[BUNDLE_PROFILE_KEY_MODULE][BUNDLE_MODULE_PROFILE_KEY_DEFINE_PERMISSIONS] = R"(
        [{
            "name": "~!@#$%^&*(){}[]:;'?<>,.|`/./+_-",
            "reason": "~!@#$%^&*(){}[]:;'?<>,.|`/./+_-",
            "when": "~!@#$%^&*(){}[]:;'?<>,.|`/./+_-"
        }]
    )"_json;
    CheckProfilePermission(errorDefPermJson);
}

/**
 * @tc.number: TestParse_2100
 * @tc.name: parse bundle package by config.json
 * @tc.desc: 1. system running normally
 *           2. test parsing failed when req-permission prop has error in the config.json
 */
HWTEST_F(BmsBundleParserTest, TestParse_2100, Function | SmallTest | Level1)
{
    nlohmann::json errorReqPermJson = CONFIG_JSON;
    errorReqPermJson[BUNDLE_PROFILE_KEY_MODULE][BUNDLE_MODULE_PROFILE_KEY_REQ_PERMISSIONS] = R"(
        [{
            "reason": "~!@#$%^&*(){}[]:;'?<>,.|`/./+_-",
            "when": "~!@#$%^&*(){}[]:;'?<>,.|`/./+_-"
        }]
    )"_json;
    CheckProfilePermission(errorReqPermJson);
}

/**
 * @tc.number: TestParse_2200
 * @tc.name: parse bundle package by config.json
 * @tc.desc: 1. system running normally
 *           2. test parsing failed when def-permission prop has empty in the config.json
 */
HWTEST_F(BmsBundleParserTest, TestParse_2200, Function | SmallTest | Level1)
{
    nlohmann::json errorDefPermJson = CONFIG_JSON;
    errorDefPermJson[BUNDLE_PROFILE_KEY_MODULE][BUNDLE_MODULE_PROFILE_KEY_DEFINE_PERMISSIONS] = R"(
        [{

        }]
    )"_json;
    CheckProfilePermission(errorDefPermJson);
}

/**
 * @tc.number: TestParse_2300
 * @tc.name: parse bundle package by config.json
 * @tc.desc: 1. system running normally
 *           2. test parsing failed when req-permission prop has empty in the config.json
 */
HWTEST_F(BmsBundleParserTest, TestParse_2300, Function | SmallTest | Level1)
{
    nlohmann::json errorReqPermJson = CONFIG_JSON;
    errorReqPermJson[BUNDLE_PROFILE_KEY_MODULE][BUNDLE_MODULE_PROFILE_KEY_REQ_PERMISSIONS] = R"(
        [{

        }]
    )"_json;
    CheckProfilePermission(errorReqPermJson);
}

/**
 * @tc.number: TestParse_2400
 * @tc.name: parse bundle package by config.json
 * @tc.desc: 1. system running normally
 *           2. test parsing failed when forms prop has error in the config.json
 */
HWTEST_F(BmsBundleParserTest, TestParse_2400, Function | SmallTest | Level1)
{
    nlohmann::json errorFormsJson = CONFIG_JSON;
    errorFormsJson[BUNDLE_PROFILE_KEY_MODULE][BUNDLE_MODULE_PROFILE_KEY_ABILITIES]= R"(
    [{
        "skills": [
          {
            "entities": [
              "entity.system.home",
              "flag.home.intent.from.system"
            ],
            "actions": [
              "action.system.home"
            ]
          }
        ],
        "forms": [
            {
                "description": "~!@#$%^&*(){}[]:;'?<>,.|`/./+_-",
                "type": "~!@#$%^&*(){}[]:;'?<>,.|`/./+_-",
                "colorMode": "~!@#$%^&*(){}[]:;'?<>,.|`/./+_-",
                "isDefault": "~!@#$%^&*(){}[]:;'?<>,.|`/./+_-",
                "updateEnabled": "~!@#$%^&*(){}[]:;'?<>,.|`/./+_-",
                "scheduledUpdateTime": "~!@#$%^&*(){}[]:;'?<>,.|`/./+_-",
                "updateDuration": "~!@#$%^&*(){}[]:;'?<>,.|`/./+_-",
                "defaultDimension": "~!@#$%^&*(){}[]:;'?<>,.|`/./+_-",
                "supportDimensions": [
                    "~!@#$%^&*(){}[]:;'?<>,.|`/./+_-",
                    "~!@#$%^&*(){}[]:;'?<>,.|`/./+_-"
                ],
                "jsComponentName": "~!@#$%^&*(){}[]:;'?<>,.|`/./+_-",
                "deepLink": "~!@#$%^&*(){}[]:;'?<>,.|`/./+_-",
                "metaData": {
                    "customizeData": [
                        {
                            "name": "~!@#$%^&*(){}[]:;'?<>,.|`/./+_-",
                            "value": "~!@#$%^&*(){}[]:;'?<>,.|`/./+_-"
                        }
                    ]
                }
            }
        ],
        "name": "com.example.napi_test_suite.MainAbility",
        "icon": "$media:icon",
        "description": "$string:mainability_description",
        "label": "MyApplication11",
        "type": "page",
        "launchType": "standard"
    }]

    )"_json;
    CheckProfileForms(errorFormsJson);
}

/**
 * @tc.number: TestParse_2500
 * @tc.name: parse bundle package by config.json
 * @tc.desc: 1. system running normally
 *           2. test parsing failed when forms prop has empty in the config.json
 */
HWTEST_F(BmsBundleParserTest, TestParse_2500, Function | SmallTest | Level1)
{
    nlohmann::json errorFormsJson = CONFIG_JSON;
    errorFormsJson[BUNDLE_PROFILE_KEY_MODULE][BUNDLE_MODULE_PROFILE_KEY_ABILITIES] = R"(
        [{
        "skills": [
          {
            "entities": [
              "entity.system.home",
              "flag.home.intent.from.system"
            ],
            "actions": [
              "action.system.home"
            ]
          }
        ],
        "forms": [{
                 }],
        "name": "com.example.napi_test_suite.MainAbility",
        "icon": "$media:icon",
        "description": "$string:mainability_description",
        "label": "MyApplication11",
        "type": "page",
        "launchType": "standard"
        }]
    )"_json;
    CheckProfileForms(errorFormsJson);
}

/**
 * @tc.number: TestParse_2600
 * @tc.name: parse bundle package by config.json
 * @tc.desc: 1. system running normally
 *           2. test parsing failed when shortcuts prop has empty in the config.json
 */
HWTEST_F(BmsBundleParserTest, TestParse_2600, Function | SmallTest | Level1)
{
    nlohmann::json errorShortcutJson = CONFIG_JSON;
    errorShortcutJson[BUNDLE_PROFILE_KEY_MODULE][BUNDLE_MODULE_PROFILE_KEY_SHORTCUTS] = R"(
        [{

        }]
    )"_json;
    CheckProfileShortcut(errorShortcutJson, ERR_APPEXECFWK_PARSE_PROFILE_MISSING_PROP);
}

/**
 * @tc.number: TestParse_2700
 * @tc.name: parse bundle package by config.json
 * @tc.desc: 1. system running normally
 *           2. test parsing failed when shortcuts prop has error in the config.json
 */
HWTEST_F(BmsBundleParserTest, TestParse_2700, Function | SmallTest | Level1)
{
    nlohmann::json errorShortcutJson = CONFIG_JSON;
    errorShortcutJson[BUNDLE_PROFILE_KEY_MODULE][BUNDLE_MODULE_PROFILE_KEY_SHORTCUTS] = R"(
        [{
            "label": "~!@#$%^&*(){}[]:;'?<>,.|`/./+_-",
            "icon": "~!@#$%^&*(){}[]:;'?<>,.|`/./+_-",
            "intents": [
              {
                "targetBundle": "~!@#$%^&*(){}[]:;'?<>,.|`/./+_-",
                "targetClass": "~!@#$%^&*(){}[]:;'?<>,.|`/./+_-"
              }
            ]
        }]
    )"_json;
    CheckProfileShortcut(errorShortcutJson, ERR_APPEXECFWK_PARSE_PROFILE_MISSING_PROP);
}

/**
 * @tc.name: TestParse_2800
 * @tc.desc: 1. system running normally
 *           2. test parsing failed when forms prop has empty in the config.json
 * @tc.type: FUNC
 * @tc.require: issueI5MZ3F
 */
HWTEST_F(BmsBundleParserTest, TestParse_2800, Function | SmallTest | Level1)
{
    nlohmann::json formsJson = CONFIG_JSON;

    BundleProfile bundleProfile;
    InnerBundleInfo innerBundleInfo;
    std::vector<FormInfo> formInfos;
    std::ostringstream profileFileBuffer;

    profileFileBuffer << formsJson.dump();
    
    BundleExtractor bundleExtractor("");
    ErrCode result = bundleProfile.TransformTo(
        profileFileBuffer, bundleExtractor, innerBundleInfo);
    EXPECT_EQ(result, ERR_OK) << profileFileBuffer.str();
    innerBundleInfo.GetFormsInfoByApp(formInfos);
    int newSupportDimension = 5;
    EXPECT_EQ(formInfos[0].supportDimensions[0], newSupportDimension);
}

/**
 * @tc.name: TestParse_2900
 * @tc.desc: 1. system running normally
 *           2. test parsing failed
 * @tc.type: FUNC
 * @tc.require: issueI5MZ3F
 */
HWTEST_F(BmsBundleParserTest, TestParse_2900, Function | SmallTest | Level1)
{
    nlohmann::json errorShortcutJson = CONFIG_JSON;
    errorShortcutJson[BUNDLE_PROFILE_KEY_APP][BUNDLE_APP_PROFILE_KEY_BUNDLE_NAME] = "";
    CheckProfileShortcut(errorShortcutJson, ERR_APPEXECFWK_PARSE_PROFILE_PROP_CHECK_ERROR);

    errorShortcutJson[BUNDLE_PROFILE_KEY_APP][BUNDLE_APP_PROFILE_KEY_BUNDLE_NAME] = "xxx";
    CheckProfileShortcut(errorShortcutJson, ERR_APPEXECFWK_PARSE_PROFILE_PROP_CHECK_ERROR);

    errorShortcutJson[BUNDLE_PROFILE_KEY_APP][BUNDLE_APP_PROFILE_KEY_BUNDLE_NAME] = OVER_MAX_PATH_SIZE;
    CheckProfileShortcut(errorShortcutJson, ERR_APPEXECFWK_PARSE_PROFILE_PROP_SIZE_CHECK_ERROR);
    
    errorShortcutJson[BUNDLE_PROFILE_KEY_APP][BUNDLE_APP_PROFILE_KEY_BUNDLE_NAME] = "bundleName&";
    CheckProfileShortcut(errorShortcutJson, ERR_APPEXECFWK_PARSE_PROFILE_PROP_CHECK_ERROR);

    errorShortcutJson[BUNDLE_PROFILE_KEY_APP][BUNDLE_APP_PROFILE_KEY_BUNDLE_NAME] = "bundleName/";
    CheckProfileShortcut(errorShortcutJson, ERR_APPEXECFWK_PARSE_PROFILE_PROP_CHECK_ERROR);

    errorShortcutJson[BUNDLE_PROFILE_KEY_APP][BUNDLE_APP_PROFILE_KEY_BUNDLE_NAME] = "bundleName?";
    CheckProfileShortcut(errorShortcutJson, ERR_APPEXECFWK_PARSE_PROFILE_PROP_CHECK_ERROR);

    errorShortcutJson[BUNDLE_PROFILE_KEY_APP][BUNDLE_APP_PROFILE_KEY_BUNDLE_NAME] = "bundleName]";
    CheckProfileShortcut(errorShortcutJson, ERR_APPEXECFWK_PARSE_PROFILE_PROP_CHECK_ERROR);

    errorShortcutJson[BUNDLE_PROFILE_KEY_APP][BUNDLE_APP_PROFILE_KEY_BUNDLE_NAME] = "bundleName`";
    CheckProfileShortcut(errorShortcutJson, ERR_APPEXECFWK_PARSE_PROFILE_PROP_CHECK_ERROR);

    errorShortcutJson[BUNDLE_PROFILE_KEY_APP][BUNDLE_APP_PROFILE_KEY_BUNDLE_NAME] = "bundleName|";
    CheckProfileShortcut(errorShortcutJson, ERR_APPEXECFWK_PARSE_PROFILE_PROP_CHECK_ERROR);
}


/**
 * @tc.name: TestParse_3000
 * @tc.desc: 1. system running normally
 *           2. test parsing failed
 * @tc.type: FUNC
 * @tc.require: issueI5MZ3F
 */
HWTEST_F(BmsBundleParserTest, TestParse_3000, Function | SmallTest | Level1)
{
    nlohmann::json errorShortcutJson = CONFIG_JSON;
    errorShortcutJson[BUNDLE_PROFILE_KEY_MODULE][BUNDLE_MODULE_PROFILE_KEY_PACKAGE] = "";
    CheckProfileShortcut(errorShortcutJson, ERR_APPEXECFWK_PARSE_PROFILE_PROP_CHECK_ERROR);

    errorShortcutJson[BUNDLE_PROFILE_KEY_MODULE][BUNDLE_MODULE_PROFILE_KEY_DISTRO][
        BUNDLE_MODULE_PROFILE_KEY_MODULE_NAME] = "com../hiworld../";
    CheckProfileShortcut(errorShortcutJson, ERR_APPEXECFWK_PARSE_PROFILE_PROP_CHECK_ERROR);

    errorShortcutJson[BUNDLE_PROFILE_KEY_MODULE][BUNDLE_MODULE_PROFILE_KEY_DISTRO][
        BUNDLE_MODULE_PROFILE_KEY_MODULE_NAME] = "";
    CheckProfileShortcut(errorShortcutJson, ERR_APPEXECFWK_PARSE_PROFILE_PROP_CHECK_ERROR);

    errorShortcutJson[BUNDLE_PROFILE_KEY_MODULE][BUNDLE_MODULE_PROFILE_KEY_DEVICE_TYPE] = "";
    CheckProfileShortcut(errorShortcutJson, ERR_APPEXECFWK_PARSE_PROFILE_PROP_TYPE_ERROR);
}

/**
 * @tc.name: TestParse_3100
 * @tc.desc: 1. system running normally
 *           2. test parsing info in the module.json
 * @tc.type: FUNC
 */
HWTEST_F(BmsBundleParserTest, TestParse_3100, Function | SmallTest | Level1)
{
    nlohmann::json moduleJson = MODULE_JSON;
    CheckProfileModule(moduleJson, ERR_OK);

    moduleJson[BUNDLE_PROFILE_KEY_MODULE][PROFILE_KEY_NAME] = "";
    CheckProfileModule(moduleJson, ERR_APPEXECFWK_PARSE_PROFILE_PROP_CHECK_ERROR);

    moduleJson[BUNDLE_PROFILE_KEY_APP][BUNDLE_APP_PROFILE_KEY_BUNDLE_NAME] = "";
    CheckProfileModule(moduleJson, ERR_APPEXECFWK_PARSE_PROFILE_PROP_CHECK_ERROR);

    moduleJson[BUNDLE_PROFILE_KEY_MODULE][PROFILE_KEY_NAME] = "../entry";
    moduleJson[BUNDLE_PROFILE_KEY_APP][BUNDLE_APP_PROFILE_KEY_BUNDLE_NAME] = "com.ex";
    CheckProfileModule(moduleJson, ERR_APPEXECFWK_PARSE_PROFILE_PROP_CHECK_ERROR);

    moduleJson[BUNDLE_PROFILE_KEY_APP][BUNDLE_APP_PROFILE_KEY_BUNDLE_NAME] =
        "doe8m8mMt2DicXm3fZ7Nz0xaVaw4R2in5Gm1gJVvzRKmh3SM7Jf5gmkaDGFzRsriDtLRioSvg07wokZtmUDE4XKplv6pIMqF5aVIdaff";
    CheckProfileModule(moduleJson, ERR_APPEXECFWK_PARSE_PROFILE_PROP_CHECK_ERROR);

    moduleJson[BUNDLE_PROFILE_KEY_APP][BUNDLE_APP_PROFILE_KEY_BUNDLE_NAME] = "1com.example.backuptest";
    moduleJson[BUNDLE_PROFILE_KEY_MODULE][PROFILE_KEY_NAME] = "entry";
    CheckProfileModule(moduleJson, ERR_APPEXECFWK_PARSE_PROFILE_PROP_CHECK_ERROR);

    moduleJson[BUNDLE_PROFILE_KEY_APP][BUNDLE_APP_PROFILE_KEY_BUNDLE_NAME] = "com.example/.backuptest";
    CheckProfileModule(moduleJson, ERR_APPEXECFWK_PARSE_PROFILE_PROP_CHECK_ERROR);
}


/**
 * @tc.name: TestParse_3200
 * @tc.desc: 1. system running normally
 *           2. test parsing info in the module.json
 * @tc.type: FUNC
 */
HWTEST_F(BmsBundleParserTest, TestParse_3200, Function | SmallTest | Level1)
{
    nlohmann::json formsJson = R"(
     {
      "app": {
          "bundleName": "com.example.backuptest",
         "debug": true,
         "icon": "$media:app_icon",
         "iconId": 16777220,
         "label": "$string:app_name",
         "labelId": 16777216,
         "minAPIVersion": 9,
         "targetAPIVersion": 9,
         "vendor": "example",
         "versionCode": 1000000,
         "versionName": "1.0.0"
      },
      "module": {
          "name": "entry",
          "installationFree": false,
          "mainElement": "MainAbility",
          "pages": "$profile:main_pages",
          "srcEntrance": "./ets/Application/AbilityStage.ts",
          "type": "entry",
          "virtualMachine": "ark0.0.0.3"
      }
  }
)"_json;
//         )"_json;
    ModuleProfile moduleProfile;
    InnerBundleInfo innerBundleInfo;
    std::vector<FormInfo> formInfos;
    std::ostringstream profileFileBuffer;

    profileFileBuffer << formsJson.dump();

    BundleExtractor bundleExtractor("");
    ErrCode result = moduleProfile.TransformTo(
        profileFileBuffer, bundleExtractor, innerBundleInfo);
    EXPECT_EQ(result, ERR_APPEXECFWK_PARSE_PROFILE_MISSING_PROP) << profileFileBuffer.str();
}

/**
 * @tc.name: TestParse_3300
 * @tc.desc: 1. system running normally
 *           2. test parsing info in the module.json
 * @tc.type: FUNC
 */
HWTEST_F(BmsBundleParserTest, TestParse_3300, Function | SmallTest | Level1)
{
    nlohmann::json formsJson = R"(
     {
      "app": {
          "bundleName": "com.example.backuptest",
         "debug": true,
         "icon": "$media:app_icon",
         "iconId": 16777220,
         "label": "$string:app_name",
         "labelId": 16777216,
         "minAPIVersion": 9,
         "targetAPIVersion": 9,
         "vendor": "example",
         "versionCode": 1000000,
         "versionName": "1.0.0"
      },
      "module": {
        "deliveryWithInstall": true,
        "description": "$string:entry_desc",
        "descriptionId": 16777219,
        "deviceTypes": [
            "default"
        ],
        "abilities": [
            {
                "description": "$string:MainAbility_desc",
                "descriptionId": 16777217,
                "icon": "$media:icon",
                "iconId": 16777221,
                "label": "$string:MainAbility_label",
                "labelId": 16777218,
                "name": "MainAbility",
                "launchType": "unknowlaunchType",
                "orientation": "unknoworientation",
                "skills": [
                    {
                        "actions": [
                            "action.thirdparty.home"
                        ],
                        "entities": [
                            "entity.system.home",
                            "flag.home.intent.from.system"
                        ]
                    }
                ],
                "srcEntrance": "./ets/MainAbility/MainAbility.ts",
                "visible": true
            }
        ],
          "name": "entry",
          "installationFree": false,
          "mainElement": "MainAbility",
          "pages": "$profile:main_pages",
          "srcEntrance": "./ets/Application/AbilityStage.ts",
          "type": "entry",
          "virtualMachine": "ark0.0.0.3"
      }
  }
)"_json;
    ModuleProfile moduleProfile;
    InnerBundleInfo innerBundleInfo;
    std::vector<FormInfo> formInfos;
    std::ostringstream profileFileBuffer;

    profileFileBuffer << formsJson.dump();

    BundleExtractor bundleExtractor("");
    ErrCode result = moduleProfile.TransformTo(
        profileFileBuffer, bundleExtractor, innerBundleInfo);
    EXPECT_EQ(result, ERR_OK) << profileFileBuffer.str();
}

/**
 * @tc.number: TestExtractByName_0100
 * @tc.name: extract file stream by file name from package
 * @tc.desc: 1. system running normally
 *           2. test extract file from is not exist bundle or ability package
 */
HWTEST_F(BmsBundleParserTest, TestExtractByName_0100, Function | SmallTest | Level0)
{
    pathStream_ << RESOURCE_ROOT_PATH << UNKOWN_PATH << INSTALL_FILE_SUFFIX;
    std::string fileInBundle = "";
    std::ostringstream fileBuffer;

    BundleExtractor bundleExtractor(pathStream_.str());
    bool result = bundleExtractor.ExtractByName(fileInBundle, fileBuffer);
    EXPECT_FALSE(result);
}

/**
 * @tc.number: TestExtractByName_0200
 * @tc.name: extract file stream by file name from package
 * @tc.desc: 1. system running normally
 *           2. test extract is not exist file from bundle or ability package
 */
HWTEST_F(BmsBundleParserTest, TestExtractByName_0200, Function | SmallTest | Level0)
{
    pathStream_ << RESOURCE_ROOT_PATH << NEW_APP << INSTALL_FILE_SUFFIX;
    std::string fileInBundle = "unknown";
    std::ostringstream fileBuffer;

    BundleExtractor bundleExtractor(pathStream_.str());
    bool result = bundleExtractor.ExtractByName(fileInBundle, fileBuffer);
    EXPECT_FALSE(result);
}

/**
 * @tc.number: TestExtractByName_0300
 * @tc.name: extract file stream by file name from package
 * @tc.desc: 1. system running normally
 *           2. test failed to extract files from a package with a deep file path depth
 */
HWTEST_F(BmsBundleParserTest, TestExtractByName_0300, Function | SmallTest | Level1)
{
    pathStream_ << RESOURCE_ROOT_PATH;
    int maxDeep = 100;
    for (int i = 0; i < maxDeep; i++) {
        pathStream_ << "test/";
    }
    pathStream_ << "app" << INSTALL_FILE_SUFFIX;

    std::string fileInBundle = "config.json";
    std::ostringstream fileBuffer;

    BundleExtractor bundleExtractor(pathStream_.str());
    bool result = bundleExtractor.ExtractByName(fileInBundle, fileBuffer);
    EXPECT_FALSE(result);
}

/**
 * @tc.number: TestExtractByName_0400
 * @tc.name: extract file stream by file name from package
 * @tc.desc: 1. system running normally
 *           2. test failed to extract files from a file with a long path
 */
HWTEST_F(BmsBundleParserTest, TestExtractByName_0400, Function | SmallTest | Level1)
{
    pathStream_ << RESOURCE_ROOT_PATH;
    int maxLength = 256;
    for (int i = 0; i < maxLength; i++) {
        pathStream_ << "test";
    }
    pathStream_ << "new" << INSTALL_FILE_SUFFIX;

    std::string fileInBundle = "config.json";
    std::ostringstream fileBuffer;

    BundleExtractor bundleExtractor(pathStream_.str());
    bool result = bundleExtractor.ExtractByName(fileInBundle, fileBuffer);
    EXPECT_FALSE(result);
}

/**
 * @tc.number: TestExtractByName_0500
 * @tc.name: extract file stream by file name from package
 * @tc.desc: 1. system running normally
 *           2. test failed to extract files from a package with special character in the file path
 */
HWTEST_F(BmsBundleParserTest, TestExtractByName_0500, Function | SmallTest | Level1)
{
    pathStream_ << RESOURCE_ROOT_PATH;
    std::string specialChars = "~!@#$%^&*(){}[]:;'?<>,.|`/./+_-";
    pathStream_ << specialChars << "new" << INSTALL_FILE_SUFFIX;

    std::string fileInBundle = "config.json";
    std::ostringstream fileBuffer;

    BundleExtractor bundleExtractor(pathStream_.str());
    bool result = bundleExtractor.ExtractByName(fileInBundle, fileBuffer);
    EXPECT_FALSE(result);
}

/**
 * @tc.number: TestDefaultPermissionProfile_0100
 * @tc.name: test default permission profile
 * @tc.desc: 1. system running normally
 *           2. test success
 */
HWTEST_F(BmsBundleParserTest, TestDefaultPermissionProfile_0100, Function | SmallTest | Level1)
{
    std::set<DefaultPermission> defaultPermissions;
    nlohmann::json profileJson = R"(
        [
            {
                "bundleName": "com.ohos.test"
            }
        ]
        )"_json;
    ErrCode result = CheckProfileDefaultPermission(profileJson, defaultPermissions);
    EXPECT_EQ(result, ERR_OK);
    EXPECT_EQ(defaultPermissions.size(), ONE);
}

/**
 * @tc.number: TestDefaultPermissionProfile_0200
 * @tc.name: test default permission profile
 * @tc.desc: 1. system running normally
 *           2. test success
 */
HWTEST_F(BmsBundleParserTest, TestDefaultPermissionProfile_0200, Function | SmallTest | Level1)
{
    std::set<DefaultPermission> defaultPermissions;
    nlohmann::json profileJson = R"(
        [
            {
                "bundleName": "com.ohos.test1",
                "permissions":[
                    {
                        "name": "ohos.permission.test1",
                        "userCancellable":true
                    },
                    {
                        "name": "ohos.permission.test2",
                        "userCancellable":false
                    }
                ]
            }
        ]
        )"_json;
    ErrCode result = CheckProfileDefaultPermission(profileJson, defaultPermissions);
    EXPECT_EQ(result, ERR_OK);
    EXPECT_EQ(defaultPermissions.size(), ONE);
    if (defaultPermissions.size() == ONE) {
        auto defaultPermission = *defaultPermissions.begin();
        EXPECT_EQ(defaultPermission.bundleName, "com.ohos.test1");
        EXPECT_EQ(defaultPermission.grantPermission[0].name, "ohos.permission.test1");
        EXPECT_TRUE(defaultPermission.grantPermission[0].userCancellable);
        EXPECT_EQ(defaultPermission.grantPermission[1].name, "ohos.permission.test2");
        EXPECT_FALSE(defaultPermission.grantPermission[1].userCancellable);
    }
}

/**
 * @tc.number: TestDefaultPermissionProfile_0300
 * @tc.name: test default permission profile
 * @tc.desc: 1. system running normally
 *           2. test success
 */
HWTEST_F(BmsBundleParserTest, TestDefaultPermissionProfile_0300, Function | SmallTest | Level1)
{
    std::set<DefaultPermission> defaultPermissions;
    nlohmann::json profileJson = R"(
        [
            {
                "bundleName": "com.ohos.test1",
                "permissions":[
                    {
                        "name": "ohos.permission.test1",
                        "userCancellable":true
                    },
                    {
                        "name": "ohos.permission.test2",
                        "userCancellable":false
                    }
                ]
            },
            {
                "bundleName": "com.ohos.test2",
                "permissions":[
                    {
                        "name": "ohos.permission.test1",
                        "userCancellable":true
                    },
                    {
                        "name": "ohos.permission.test2",
                        "userCancellable":false
                    }
                ]
            }
        ]
        )"_json;
    ErrCode result = CheckProfileDefaultPermission(profileJson, defaultPermissions);
    EXPECT_EQ(result, ERR_OK);
    EXPECT_EQ(defaultPermissions.size(), TWO);
    if (defaultPermissions.size() == TWO) {
        DefaultPermission firstDefaultPermission;
        firstDefaultPermission.bundleName = "com.ohos.test1";
        auto firstDefaultPermissionIter = defaultPermissions.find(firstDefaultPermission);
        EXPECT_TRUE(firstDefaultPermissionIter != defaultPermissions.end());
        firstDefaultPermission = *firstDefaultPermissionIter;
        EXPECT_EQ(firstDefaultPermission.grantPermission[0].name, "ohos.permission.test1");
        EXPECT_TRUE(firstDefaultPermission.grantPermission[0].userCancellable);
        EXPECT_EQ(firstDefaultPermission.grantPermission[1].name, "ohos.permission.test2");
        EXPECT_FALSE(firstDefaultPermission.grantPermission[1].userCancellable);
        DefaultPermission secondDefaultPermission;
        secondDefaultPermission.bundleName = "com.ohos.test2";
        EXPECT_TRUE(defaultPermissions.find(secondDefaultPermission) != defaultPermissions.end());
    }
}

/**
 * @tc.number: TestDefaultPermissionProfile_0400
 * @tc.name: test default permission profile
 * @tc.desc: 1. system running normally
 *           2. test failed
 */
HWTEST_F(BmsBundleParserTest, TestDefaultPermissionProfile_0400, Function | SmallTest | Level1)
{
    std::set<DefaultPermission> defaultPermissions;
    nlohmann::json errorProfileJson = R"(
        [
            {
                "bundleName": "com.ohos.test",
                "permissions": [
                    {
                        "name": "ohos.permission.test1"
                    }
                ]
            }
        ]
        )"_json;
    ErrCode result = CheckProfileDefaultPermission(errorProfileJson, defaultPermissions);
    EXPECT_EQ(result, ERR_APPEXECFWK_PARSE_PROFILE_MISSING_PROP);
}

/**
 * @tc.number: TestDefaultPermissionProfile_0500
 * @tc.name: test default permission profile
 * @tc.desc: 1. system running normally
 *           2. test failed
 */
HWTEST_F(BmsBundleParserTest, TestDefaultPermissionProfile_0500, Function | SmallTest | Level1)
{
    std::set<DefaultPermission> defaultPermissions;
    nlohmann::json errorProfileJson = R"(
        [
            {
            }
        ]
        )"_json;
    ErrCode result = CheckProfileDefaultPermission(errorProfileJson, defaultPermissions);
    EXPECT_EQ(result, ERR_APPEXECFWK_PARSE_PROFILE_MISSING_PROP);
}

/**
 * @tc.name: TestParse_3400
 * @tc.desc: 1. system running normally
 *           2. test parsing info in the module.json
 * @tc.type: FUNC
 */
HWTEST_F(BmsBundleParserTest, TestParse_3400, Function | SmallTest | Level1)
{
    ModuleProfile moduleProfile;
    InnerBundleInfo innerBundleInfo;
    std::vector<FormInfo> formInfos;
    std::ostringstream profileFileBuffer;

    nlohmann::json profileJson = MODULE_JSON_2;
    profileFileBuffer << profileJson.dump();

    BundleExtractor bundleExtractor("");
    ErrCode result = moduleProfile.TransformTo(
        profileFileBuffer, bundleExtractor, innerBundleInfo);
    EXPECT_EQ(result, ERR_OK) << profileFileBuffer.str();
}

/**
 * @tc.name: TestParse_3500
 * @tc.desc: 1. system running normally
 *           2. test parsing info in the module.json
 * @tc.type: FUNC
 */
HWTEST_F(BmsBundleParserTest, TestParse_3500, Function | SmallTest | Level1)
{
    ModuleProfile moduleProfile;
    InnerBundleInfo innerBundleInfo;
    innerBundleInfo.SetIsPreInstallApp(true);
    std::vector<FormInfo> formInfos;
    std::ostringstream profileFileBuffer;

    nlohmann::json profileJson = MODULE_JSON_2;
    profileFileBuffer << profileJson.dump();

    BundleExtractor bundleExtractor("");
    ErrCode result = moduleProfile.TransformTo(
        profileFileBuffer, bundleExtractor, innerBundleInfo);
    EXPECT_EQ(result, ERR_OK) << profileFileBuffer.str();
}

/**
 * @tc.name: TestParse_3600
 * @tc.desc: 1. system running normally
 *           2. test parsing info in the module.json
 * @tc.type: FUNC
 */
HWTEST_F(BmsBundleParserTest, TestParse_3600, Function | SmallTest | Level1)
{
    ModuleProfile moduleProfile;
    InnerBundleInfo innerBundleInfo;
    std::vector<FormInfo> formInfos;
    std::ostringstream profileFileBuffer;

    nlohmann::json profileJson = MODULE_JSON_2;
    profileJson["module"]["type"] = "shared";
    profileFileBuffer << profileJson.dump();

    BundleExtractor bundleExtractor("");
    ErrCode result = moduleProfile.TransformTo(
        profileFileBuffer, bundleExtractor, innerBundleInfo);
    EXPECT_EQ(result, ERR_OK) << profileFileBuffer.str();
}

/**
 * @tc.name: TestParse_3700
 * @tc.desc: 1. system running normally
 *           2. test parsing info in the module.json
 * @tc.type: FUNC
 */
HWTEST_F(BmsBundleParserTest, TestParse_3700, Function | SmallTest | Level1)
{
    ModuleProfile moduleProfile;
    InnerBundleInfo innerBundleInfo;
    innerBundleInfo.SetIsPreInstallApp(true);
    std::vector<FormInfo> formInfos;
    std::ostringstream profileFileBuffer;

    nlohmann::json profileJson = MODULE_JSON_2;
    profileJson["module"]["type"] = "shared";
    profileFileBuffer << profileJson.dump();

    BundleExtractor bundleExtractor("");
    ErrCode result = moduleProfile.TransformTo(
        profileFileBuffer, bundleExtractor, innerBundleInfo);
    EXPECT_EQ(result, ERR_OK) << profileFileBuffer.str();
}

/**
 * @tc.name: TestParse_3800
 * @tc.desc: 1. system running normally
 *           2. test parsing info in the module.json
 * @tc.type: FUNC
 */
HWTEST_F(BmsBundleParserTest, TestParse_3800, Function | SmallTest | Level1)
{
    ModuleProfile moduleProfile;
    InnerBundleInfo innerBundleInfo;
    std::vector<FormInfo> formInfos;
    std::ostringstream profileFileBuffer;

    nlohmann::json profileJson = MODULE_JSON_3;
    profileFileBuffer << profileJson.dump();

    BundleExtractor bundleExtractor("");
    ErrCode result = moduleProfile.TransformTo(
        profileFileBuffer, bundleExtractor, innerBundleInfo);
    EXPECT_EQ(result, ERR_OK) << profileFileBuffer.str();
}

/**
 * @tc.name: TestParse_3900
 * @tc.desc: 1. system running normally
 *           2. test parsing info in the module.json
 * @tc.type: FUNC
 */
HWTEST_F(BmsBundleParserTest, TestParse_3900, Function | SmallTest | Level1)
{
    ModuleProfile moduleProfile;
    InnerBundleInfo innerBundleInfo;
    innerBundleInfo.SetIsPreInstallApp(true);
    std::vector<FormInfo> formInfos;
    std::ostringstream profileFileBuffer;

    nlohmann::json profileJson = MODULE_JSON_3;
    profileFileBuffer << profileJson.dump();

    BundleExtractor bundleExtractor("");
    ErrCode result = moduleProfile.TransformTo(
        profileFileBuffer, bundleExtractor, innerBundleInfo);
    EXPECT_EQ(result, ERR_OK) << profileFileBuffer.str();
}

/**
 * @tc.name: TestParse_4000
 * @tc.desc: 1. system running normally
 *           2. test parsing info in the module.json
 * @tc.type: FUNC
 */
HWTEST_F(BmsBundleParserTest, TestParse_4000, Function | SmallTest | Level1)
{
    ModuleProfile moduleProfile;
    InnerBundleInfo innerBundleInfo;
    std::vector<FormInfo> formInfos;
    std::ostringstream profileFileBuffer;

    nlohmann::json profileJson = MODULE_JSON_3;
    profileJson["module"]["type"] = "shared";
    profileFileBuffer << profileJson.dump();

    BundleExtractor bundleExtractor("");
    ErrCode result = moduleProfile.TransformTo(
        profileFileBuffer, bundleExtractor, innerBundleInfo);
    EXPECT_EQ(result, ERR_OK) << profileFileBuffer.str();
}

/**
 * @tc.name: TestParse_4100
 * @tc.desc: 1. system running normally
 *           2. test parsing info in the module.json
 * @tc.type: FUNC
 */
HWTEST_F(BmsBundleParserTest, TestParse_4100, Function | SmallTest | Level1)
{
    ModuleProfile moduleProfile;
    InnerBundleInfo innerBundleInfo;
    innerBundleInfo.SetIsPreInstallApp(true);
    std::vector<FormInfo> formInfos;
    std::ostringstream profileFileBuffer;

    nlohmann::json profileJson = MODULE_JSON_3;
    profileJson["module"]["type"] = "shared";
    profileFileBuffer << profileJson.dump();

    BundleExtractor bundleExtractor("");
    ErrCode result = moduleProfile.TransformTo(
        profileFileBuffer, bundleExtractor, innerBundleInfo);
    EXPECT_EQ(result, ERR_OK) << profileFileBuffer.str();
}

/**
 * @tc.name: TestParse_4200
 * @tc.desc: 1. system running normally
 *           2. test parsing info in the module.json
 * @tc.type: FUNC
 */
HWTEST_F(BmsBundleParserTest, TestParse_4200, Function | SmallTest | Level1)
{
    ModuleProfile moduleProfile;
    InnerBundleInfo innerBundleInfo;
    std::vector<FormInfo> formInfos;
    std::ostringstream profileFileBuffer;

    nlohmann::json profileJson = MODULE_JSON_2;
    profileJson["app"]["labelId"] = 0;
    profileFileBuffer << profileJson.dump();

    BundleExtractor bundleExtractor("");
    ErrCode result = moduleProfile.TransformTo(
        profileFileBuffer, bundleExtractor, innerBundleInfo);
    EXPECT_EQ(result, ERR_OK) << profileFileBuffer.str();
}

/**
 * @tc.name: TestParse_4300
 * @tc.desc: 1. system running normally
 *           2. test parsing info in the module.json
 * @tc.type: FUNC
 */
HWTEST_F(BmsBundleParserTest, TestParse_4300, Function | SmallTest | Level1)
{
    ModuleProfile moduleProfile;
    InnerBundleInfo innerBundleInfo;
    std::vector<FormInfo> formInfos;
    std::ostringstream profileFileBuffer;

    nlohmann::json profileJson = MODULE_JSON_2;
    profileJson["app"]["labelId"] = 0;
    profileJson["app"]["label"] = "";
    profileFileBuffer << profileJson.dump();

    BundleExtractor bundleExtractor("");
    ErrCode result = moduleProfile.TransformTo(
        profileFileBuffer, bundleExtractor, innerBundleInfo);
    EXPECT_EQ(result, ERR_OK) << profileFileBuffer.str();
}

/**
 * @tc.name: TestParse_4400
 * @tc.desc: 1. system running normally
 *           2. test parsing info in the module.json
 * @tc.type: FUNC
 */
HWTEST_F(BmsBundleParserTest, TestParse_4400, Function | SmallTest | Level1)
{
    ModuleProfile moduleProfile;
    InnerBundleInfo innerBundleInfo;
    std::vector<FormInfo> formInfos;
    std::ostringstream profileFileBuffer;

    nlohmann::json profileJson = MODULE_JSON_2;
    profileJson["app"]["label"] = "";
    profileFileBuffer << profileJson.dump();

    BundleExtractor bundleExtractor("");
    ErrCode result = moduleProfile.TransformTo(
        profileFileBuffer, bundleExtractor, innerBundleInfo);
    EXPECT_EQ(result, ERR_OK) << profileFileBuffer.str();
}

/**
 * @tc.name: TestParse_4500
 * @tc.desc: 1. system running normally
 *           2. test parsing info in the config.json
 * @tc.type: FUNC
 */
HWTEST_F(BmsBundleParserTest, TestParse_4500, Function | SmallTest | Level1)
{
    BundleProfile bundleProfile;
    InnerBundleInfo innerBundleInfo;
    std::vector<FormInfo> formInfos;
    std::ostringstream profileFileBuffer;

    nlohmann::json profileJson = CONFIG_JSON_2;
    profileFileBuffer << profileJson.dump();

    BundleExtractor bundleExtractor("");
    ErrCode result = bundleProfile.TransformTo(
        profileFileBuffer, bundleExtractor, innerBundleInfo);
    EXPECT_EQ(result, ERR_OK) << profileFileBuffer.str();
}

/**
 * @tc.name: TestParse_4600
 * @tc.desc: 1. system running normally
 *           2. test parsing info in the config.json
 * @tc.type: FUNC
 */
HWTEST_F(BmsBundleParserTest, TestParse_4600, Function | SmallTest | Level1)
{
    BundleProfile bundleProfile;
    InnerBundleInfo innerBundleInfo;
    innerBundleInfo.SetIsPreInstallApp(true);
    std::vector<FormInfo> formInfos;
    std::ostringstream profileFileBuffer;

    nlohmann::json profileJson = CONFIG_JSON_2;
    profileFileBuffer << profileJson.dump();

    BundleExtractor bundleExtractor("");
    ErrCode result = bundleProfile.TransformTo(
        profileFileBuffer, bundleExtractor, innerBundleInfo);
    EXPECT_EQ(result, ERR_OK) << profileFileBuffer.str();
}

/**
 * @tc.name: TestParse_4700
 * @tc.desc: 1. system running normally
 *           2. test parsing info in the config.json
 * @tc.type: FUNC
 */
HWTEST_F(BmsBundleParserTest, TestParse_4700, Function | SmallTest | Level1)
{
    BundleProfile bundleProfile;
    InnerBundleInfo innerBundleInfo;
    std::vector<FormInfo> formInfos;
    std::ostringstream profileFileBuffer;
    nlohmann::json profileJson = CONFIG_JSON_2;
    profileJson["module"]["distro"]["moduleType"] = "shared";
    profileFileBuffer << profileJson.dump();

    BundleExtractor bundleExtractor("");
    ErrCode result = bundleProfile.TransformTo(
        profileFileBuffer, bundleExtractor, innerBundleInfo);
    EXPECT_EQ(result, ERR_OK) << profileFileBuffer.str();
}

/**
 * @tc.name: TestParse_4800
 * @tc.desc: 1. system running normally
 *           2. test parsing info in the config.json
 * @tc.type: FUNC
 */
HWTEST_F(BmsBundleParserTest, TestParse_4800, Function | SmallTest | Level1)
{
    BundleProfile bundleProfile;
    InnerBundleInfo innerBundleInfo;
    innerBundleInfo.SetIsPreInstallApp(true);
    std::vector<FormInfo> formInfos;
    std::ostringstream profileFileBuffer;

    nlohmann::json profileJson = CONFIG_JSON_2;
    profileJson["module"]["distro"]["moduleType"] = "shared";
    profileFileBuffer << profileJson.dump();

    BundleExtractor bundleExtractor("");
    ErrCode result = bundleProfile.TransformTo(
        profileFileBuffer, bundleExtractor, innerBundleInfo);
    EXPECT_EQ(result, ERR_OK) << profileFileBuffer.str();
}

/**
 * @tc.name: TestParse_4900
 * @tc.desc: 1. system running normally
 *           2. test parsing info in the config.json
 * @tc.type: FUNC
 */
HWTEST_F(BmsBundleParserTest, TestParse_4900, Function | SmallTest | Level1)
{
    BundleProfile bundleProfile;
    InnerBundleInfo innerBundleInfo;
    std::vector<FormInfo> formInfos;
    std::ostringstream profileFileBuffer;

    nlohmann::json profileJson = CONFIG_JSON_3;
    profileFileBuffer << profileJson.dump();

    BundleExtractor bundleExtractor("");
    ErrCode result = bundleProfile.TransformTo(
        profileFileBuffer, bundleExtractor, innerBundleInfo);
    EXPECT_EQ(result, ERR_OK) << profileFileBuffer.str();
}

/**
 * @tc.name: TestParse_5000
 * @tc.desc: 1. system running normally
 *           2. test parsing info in the config.json
 * @tc.type: FUNC
 */
HWTEST_F(BmsBundleParserTest, TestParse_5000, Function | SmallTest | Level1)
{
    BundleProfile bundleProfile;
    InnerBundleInfo innerBundleInfo;
    innerBundleInfo.SetIsPreInstallApp(true);
    std::vector<FormInfo> formInfos;
    std::ostringstream profileFileBuffer;

    nlohmann::json profileJson = CONFIG_JSON_3;
    profileFileBuffer << profileJson.dump();

    BundleExtractor bundleExtractor("");
    ErrCode result = bundleProfile.TransformTo(
        profileFileBuffer, bundleExtractor, innerBundleInfo);
    EXPECT_EQ(result, ERR_OK) << profileFileBuffer.str();
}

/**
 * @tc.name: TestParse_5100
 * @tc.desc: 1. system running normally
 *           2. test parsing info in the config.json
 * @tc.type: FUNC
 */
HWTEST_F(BmsBundleParserTest, TestParse_5100, Function | SmallTest | Level1)
{
    BundleProfile bundleProfile;
    InnerBundleInfo innerBundleInfo;
    std::vector<FormInfo> formInfos;
    std::ostringstream profileFileBuffer;

    nlohmann::json profileJson = CONFIG_JSON_3;
    profileJson["module"]["distro"]["moduleType"] = "shared";
    profileFileBuffer << profileJson.dump();

    BundleExtractor bundleExtractor("");
    ErrCode result = bundleProfile.TransformTo(
        profileFileBuffer, bundleExtractor, innerBundleInfo);
    EXPECT_EQ(result, ERR_OK) << profileFileBuffer.str();
}

/**
 * @tc.name: TestParse_5200
 * @tc.desc: 1. system running normally
 *           2. test parsing info in the config.json
 * @tc.type: FUNC
 */
HWTEST_F(BmsBundleParserTest, TestParse_5200, Function | SmallTest | Level1)
{
    BundleProfile bundleProfile;
    InnerBundleInfo innerBundleInfo;
    innerBundleInfo.SetIsPreInstallApp(true);
    std::vector<FormInfo> formInfos;
    std::ostringstream profileFileBuffer;

    nlohmann::json profileJson = CONFIG_JSON_3;
    profileJson["module"]["distro"]["moduleType"] = "shared";
    profileFileBuffer << profileJson.dump();

    BundleExtractor bundleExtractor("");
    ErrCode result = bundleProfile.TransformTo(
        profileFileBuffer, bundleExtractor, innerBundleInfo);
    EXPECT_EQ(result, ERR_OK) << profileFileBuffer.str();
}
} // OHOS