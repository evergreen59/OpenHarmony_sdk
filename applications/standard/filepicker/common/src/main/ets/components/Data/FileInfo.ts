/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

import { MediaType } from './Constants'

export class FileInfo {
	name: string
	path: string
	type: number = MediaType.MEDIA_TYPE_ALBUM
	size: string
	addedTime: string
	modifiedTime: string
	mediaType: string
	thumbnail: Resource
	fontColor: Color | string | Resource
	opacity: number
	listBackground: string | Resource
	textBackground: string | Resource

	constructor(name: string) {
		this.name = name
	}
}