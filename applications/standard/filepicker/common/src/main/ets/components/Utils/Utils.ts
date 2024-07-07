/*
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

import { MediaName } from "../Data/Constants"
import { TopPathInfo } from '../Data/TopPathInfo'
import { logInfo } from './LogUtils'

var TAG: string = 'Utils'

const TYPE_INDEX = 0
const NAME_INDEX = 1

var strTypeArr = [
	[MediaName.IMAGE, '图片'],
	[MediaName.VIDEO, '视频'],
	[MediaName.AUDIO, '音乐'],
	[MediaName.FILE, '文档'],
]

export function changeTypeToString(mediaType: string): string {
	logInfo(TAG, 'changeTypeToString type = ' + mediaType)
	for (var i = 0; i < strTypeArr.length; i++) {
		if (strTypeArr[i][0] == mediaType) {
			return strTypeArr[i][1]
		}
	}
	return ''
}

export function changeStringToType(name: string): string {
	logInfo(TAG, 'changeStringToType type = ' + name)
	for (var i = 0; i < strTypeArr.length; i++) {
		if (strTypeArr[i][1] == name) {
			return strTypeArr[i][0]
		}
	}
	return ''
}

var fileSuffixAndType = [
	['.doc', $r("app.media.ic_file_doc"), $r("app.string.file_docx")],
	['.html', $r("app.media.ic_file_html"), $r("app.string.file_html")],
	['.pdf', $r("app.media.ic_file_pdf"), $r("app.string.file_pdf")],
	['.pptx', $r("app.media.ic_file_pptx"), $r("app.string.file_pptx")],
	['.ppt', $r("app.media.ic_file_pptx"), $r("app.string.file_pptx")],
	['.rar', $r("app.media.ic_file_rar"), $r("app.string.file_rar")],
	['.txt', $r("app.media.ic_file_txt"), $r("app.string.file_txt")],
	['.xls', $r("app.media.ic_file_xls"), $r("app.string.file_xls")],
	['.xml', $r("app.media.ic_file_xml"), $r("app.string.file_xml")],
	['.zip', $r("app.media.ic_file_zip"), $r("app.string.file_zip")],
]

export function getShowIconBySuffix(suffix: string): string | Resource {
	logInfo(TAG, 'getShowIconBySuffix suffix = ' + suffix)
	for (var i = 0; i < fileSuffixAndType.length; i++) {
		if (isTwoStringEqual(fileSuffixAndType[i][0].toString(), suffix)) {
			return fileSuffixAndType[i][1]
		}
	}
	return $r("app.media.ic_file_unknown")
}

function isTwoStringEqual(src: string, tar: string) {
	if (src.indexOf(tar) == 0 && tar.indexOf(src) == 0) {
		return true;
	}
	return false;
}

export function updateTopPathInfo(currentPathInfo: Array<TopPathInfo>, name: string, path: string) {
	currentPathInfo.forEach((item: TopPathInfo) => {
		item.opacity = 0.4
	})

	currentPathInfo.push(new TopPathInfo(name, path))
	AppStorage.Set('topPathInfo', currentPathInfo)
	return currentPathInfo
}