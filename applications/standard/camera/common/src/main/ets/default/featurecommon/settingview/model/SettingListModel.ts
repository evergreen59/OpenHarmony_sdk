
/**
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

import AspectRatio from '../../../setting/settingitem/AspectRatio'
import AssistiveGrid from '../../../setting/settingitem/AssistiveGrid'
import SaveGeoLocation from '../../../setting/settingitem/SaveGeoLocation'
import SelfMirror from '../../../setting/settingitem/SelfMirror'
import Resolution from '../../../setting/settingitem/Resolution'
import Timer from '../../../setting/settingitem/Timer'
import VideoCodec from '../../../setting/settingitem/VideoCodec'
import { Voice } from '../../../setting/settingitem/Voice'
import type { RadioItem, SettingGroupItem, SettingItem } from './SettingData'

/**
 * app setting homepage service class
 */
export class SettingListModel {
    private settingsList: SettingGroupItem[] = []
    constructor() {
        this.buildSettingsList()
    }

    getSettingList() {
        return this.settingsList;
    }

    private buildSettingsList() {
        this.settingsList[0] = this.buildPhotoModeSettings()
        this.settingsList[1] = this.buildVideoModeSettings()
        this.settingsList[2] = this.buildGeneralSettings()
    }

    private buildPhotoModeSettings() {
        let result: SettingGroupItem = {}
        result.settingTitle = $r('app.string.photo_mode')
        result.settingChildren = []
        result.settingChildren[0] = this.buildPhotoResolutionSettingItem()
        return result;
    }

    private buildPhotoResolutionSettingItem() {
        let result: SettingItem = {}
        result.imagePath = $r("app.media.ic_camera_setting_resolution_photo")
        result.settingName = $r('app.string.aspect_ratio')
        result.settingAlias = AspectRatio.ALIAS
        result.selectType = "radio"
        result.radio = this.buildPhotoResolutionRadio()
        result.checkedName = result.radio[0].itemValue
        return result;
    }

    private buildPhotoResolutionRadio() {
        let result: RadioItem[] = []
        result[0] = { "itemValue": AspectRatio.RESOURCE_RATIO_4_3}
        result[1] = { "itemValue": AspectRatio.RESOURCE_RATIO_1_1}
        result[2] = { "itemValue": AspectRatio.RESOURCE_RATIO_16_9}
        return result;
    }

    private buildVideoModeSettings() {
        let result: SettingGroupItem = {}
        result.settingTitle = $r('app.string.video_mode')
        result.settingChildren = []
        result.settingChildren[0] = this.buildVideoResolutionSettingItem()
//        result.settingChildren[1] = this.buildVideoCodecSettingItem()
        return result;
    }

    private buildVideoResolutionSettingItem() {
        let result :SettingItem = {}
        result.imagePath = $r("app.media.ic_camera_setting_resolution_video")
        result.settingName = $r('app.string.video_resolution')
        result.settingAlias = Resolution.ALIAS
        result.selectType = "radio"
        result.radio = this.buildVideoResolutionRadio()
        result.checkedName = result.radio[0].itemValue
        return result;
    }

    private buildVideoResolutionRadio() {
        let result: RadioItem[] = []
        result[0] = { "itemValue": Resolution.RESOURCE_16_9_720P}
        result[1] = { "itemValue": Resolution.RESOURCE_16_9_1080P}
        return result;
    }

    private buildVideoCodecSettingItem() {
        let result: SettingItem = {}
        result.imagePath = $r("app.media.ic_camera_setting_efficient_video_format")
        result.settingName = $r('app.string.video_codec')
        result.description = $r("app.string.video_codec_desp")
        result.settingAlias = VideoCodec.ALIAS
        result.selectType = "toggle"
        result.checkedName = false
        result.toggle = true
        return result;
    }

    private buildGeneralSettings() {
        let result: SettingGroupItem = {}
        result.settingTitle = $r('app.string.general')
        result.settingChildren = []
        result.settingChildren[0] = this.buildAssistiveGridSettingItem()
        result.settingChildren[1] = this.buildTimerSettingItem()
        result.settingChildren[2] = this.buildSaveGeoLocationSettingItem()
        result.settingChildren[3] = this.buildSoundMuteSettingItem()
        result.settingChildren[4] = this.buildSelfMirrorSettingItem()
        return result;
    }

    private buildAssistiveGridSettingItem() {
        let result: SettingItem = {}
        result.imagePath = $r("app.media.ic_camera_setting_assistive_grid")
        result.settingName = $r('app.string.assistive_grid')
        result.settingAlias = AssistiveGrid.ALIAS
        result.selectType = "toggle"
        result.checkedName = false
        result.toggle = true
        return result;
    }

    private buildTimerSettingItem() {
        let result :SettingItem = {}
        result.imagePath = $r("app.media.ic_camera_rersolution")
        result.settingName = $r('app.string.timer')
        result.settingAlias = Timer.ALIAS
        result.selectType = "radio"
        result.radio = this.buildTimerRadio()
        result.checkedName = result.radio[0].itemValue
        return result;
    }

    private buildTimerRadio() {
        let result: RadioItem[] = []
        result[0] = { "itemValue": Timer.RESOURCE_OFF }
        result[1] = { "itemValue": Timer.RESOURCE_TWO_SECONDS }
        result[2] = { "itemValue": Timer.RESOURCE_FIVE_SECONDS }
        result[3] = { "itemValue": Timer.RESOURCE_TEN_SECONDS }
        return result;
    }

    private buildSaveGeoLocationSettingItem() {
        let result: SettingItem = {}
        result.imagePath = $r("app.media.ic_camera_setting_location")
        result.settingName = $r('app.string.save_geo_location')
        result.description = $r("app.string.geo_location_desp")
        result.settingAlias = SaveGeoLocation.ALIAS
        result.selectType = "toggle"
        result.checkedName = false
        result.toggle = true
        return result;
    }

    private buildSoundMuteSettingItem() {
        let result: SettingItem = {}
        result.imagePath = $r("app.media.ic_camera_sound_mute")
        result.settingName = $r('app.string.sound_mute')
        result.settingAlias = Voice.ALIAS
        result.selectType = "toggle"
        result.checkedName = false
        result.toggle = true
        return result;
    }

    private buildSelfMirrorSettingItem() {
        let result: SettingItem = {}
        result.imagePath = $r("app.media.ic_camera_mirrow_reflection")
        result.settingName = $r('app.string.self_mirror')
        result.description = $r("app.string.self_mirror_desp")
        result.settingAlias = SelfMirror.ALIAS
        result.selectType = "toggle"
        result.checkedName = false
        result.toggle = false
        return result;
    }
}