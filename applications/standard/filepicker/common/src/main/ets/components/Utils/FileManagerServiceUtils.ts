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

import { MediaType, MediaName } from '../Data/Constants';
import { FileInfo } from '../Data/FileInfo'
import { logInfo } from './LogUtils'

var TAG = 'FileManagerServiceUtils'
var isDebug: boolean = true;

export function getFirstMenu(deviceName: string): Array<FileInfo> {
	let fileList: Array<FileInfo> = [];
	if (isDebug) {
		let file1: FileInfo = new FileInfo('image_album');
		file1.type = MediaType.MEDIA_TYPE_ALBUM;
		file1.path = 'dataability:///album';
		file1.size = '';
		file1.addedTime = 'xxx';
		file1.modifiedTime = 'xxx';
		fileList.push(file1);

		let file2: FileInfo = new FileInfo('video_album');
		file2.type = MediaType.MEDIA_TYPE_ALBUM;
		file2.path = 'dataability:///album';
		file2.size = '';
		file2.addedTime = 'xxx';
		file2.modifiedTime = 'xxx';
		fileList.push(file2);

		let file4: FileInfo = new FileInfo('file_folder');
		file4.type = MediaType.MEDIA_TYPE_ALBUM;
		file4.path = 'dataability:///album';
		file4.size = '';
		file4.addedTime = 'xxx';
		file4.modifiedTime = 'xxx';
		fileList.push(file4);

		let file3: FileInfo = new FileInfo('audio_album');
		file3.type = MediaType.MEDIA_TYPE_ALBUM;
		file3.path = 'dataability:///album';
		file3.size = '';
		file3.addedTime = 'xxx';
		file3.modifiedTime = 'xxx';
		fileList.push(file3);
	} else {
		//    var fileInfo = filemanager.getRoot(deviceName);
	}

	fileList.forEach(item => {
		if (item.name == 'image_album') {
			item.name = "图片";
			item.mediaType = 'image';
			item.thumbnail = $r("app.media.ic_type_image")
		} else if (item.name == 'video_album') {
			item.name = "视频";
			item.mediaType = 'video';
			item.thumbnail = $r("app.media.ic_type_video")
		} else if (item.name == 'audio_album') {
			item.name = "音乐";
			item.mediaType = 'audio';
			item.thumbnail = $r("app.media.ic_type_audio")
		} else if (item.name == 'file_folder') {
			item.name = "文档";
			item.mediaType = 'file';
			item.thumbnail = $r("app.media.ic_public_file")
		}
	});

	return fileList;
}

export function getListFile(deviceName: string, mediaType: string, path: string): Array<FileInfo> {
	logInfo(TAG, "getListFile mediaType = " + mediaType + ';path = ' + path);
	let fileList: Array<FileInfo> = [];
	if (isDebug) {
		if (path == 'dataability:///album') {
			if (mediaType == 'image') {
				let fileInfo1: FileInfo = new FileInfo("微信的图片");
				fileInfo1.type = MediaType.MEDIA_TYPE_ALBUM;
				fileInfo1.path = 'dataability:///com.ohos.medialibrary.MediaLibraryDataAbility/file/01';
				fileInfo1.addedTime = 'xxx';
				fileInfo1.modifiedTime = 'xxx';
				fileList.push(fileInfo1);
				let fileInfo2: FileInfo = new FileInfo("图片1");
				fileInfo2.type = MediaType.MEDIA_TYPE_ALBUM;
				fileInfo2.path = 'dataability:///com.ohos.medialibrary.MediaLibraryDataAbility/file/02';
				fileInfo2.addedTime = 'xxx';
				fileInfo2.modifiedTime = 'xxx';
				fileList.push(fileInfo2);
				let fileInfo3: FileInfo = new FileInfo("图片2");
				fileInfo3.type = MediaType.MEDIA_TYPE_ALBUM;
				fileInfo3.path = 'dataability:///com.ohos.medialibrary.MediaLibraryDataAbility/file/03';
				fileInfo3.addedTime = 'xxx';
				fileInfo3.modifiedTime = 'xxx';
				fileList.push(fileInfo3);
				let fileInfo4: FileInfo = new FileInfo("图片3");
				fileInfo4.type = MediaType.MEDIA_TYPE_ALBUM;
				fileInfo4.path = 'dataability:///com.ohos.medialibrary.MediaLibraryDataAbility/file/04';
				fileInfo4.addedTime = 'xxx';
				fileInfo4.modifiedTime = 'xxx';
				fileList.push(fileInfo4);
				let fileInfo5: FileInfo = new FileInfo("图片4");
				fileInfo5.type = MediaType.MEDIA_TYPE_ALBUM;
				fileInfo5.path = 'dataability:///com.ohos.medialibrary.MediaLibraryDataAbility/file/05';
				fileInfo5.addedTime = 'xxx';
				fileInfo5.modifiedTime = 'xxx';
				fileList.push(fileInfo5);
				let fileInfo6: FileInfo = new FileInfo("图片5");
				fileInfo6.type = MediaType.MEDIA_TYPE_ALBUM;
				fileInfo6.path = 'dataability:///com.ohos.medialibrary.MediaLibraryDataAbility/file/06';
				fileInfo6.addedTime = 'xxx';
				fileInfo6.modifiedTime = 'xxx';
				fileList.push(fileInfo6);
				let fileInfo7: FileInfo = new FileInfo("图片6");
				fileInfo7.type = MediaType.MEDIA_TYPE_ALBUM;
				fileInfo7.path = 'dataability:///com.ohos.medialibrary.MediaLibraryDataAbility/file/07';
				fileInfo7.addedTime = 'xxx';
				fileInfo7.modifiedTime = 'xxx';
				fileList.push(fileInfo7);
				let fileInfo8: FileInfo = new FileInfo("图片7");
				fileInfo8.type = MediaType.MEDIA_TYPE_ALBUM;
				fileInfo8.path = 'dataability:///com.ohos.medialibrary.MediaLibraryDataAbility/file/08';
				fileInfo8.addedTime = 'xxx';
				fileInfo8.modifiedTime = 'xxx';
				fileList.push(fileInfo8);
				let fileInfo9: FileInfo = new FileInfo("图片8");
				fileInfo1.type = MediaType.MEDIA_TYPE_ALBUM;
				fileInfo1.path = 'dataability:///com.ohos.medialibrary.MediaLibraryDataAbility/file/09';
				fileInfo1.addedTime = 'xxx';
				fileInfo1.modifiedTime = 'xxx';
				fileList.push(fileInfo9);
				let fileInfo10: FileInfo = new FileInfo("图片9");
				fileInfo10.type = MediaType.MEDIA_TYPE_ALBUM;
				fileInfo10.path = 'dataability:///com.ohos.medialibrary.MediaLibraryDataAbility/file/10';
				fileInfo10.addedTime = 'xxx';
				fileInfo10.modifiedTime = 'xxx';
				fileList.push(fileInfo10);
				let fileInfo11: FileInfo = new FileInfo("图片10");
				fileInfo11.type = MediaType.MEDIA_TYPE_ALBUM;
				fileInfo11.path = 'dataability:///com.ohos.medialibrary.MediaLibraryDataAbility/file/11';
				fileInfo11.addedTime = 'xxx';
				fileInfo11.modifiedTime = 'xxx';
				fileList.push(fileInfo11);
			} else if (mediaType == 'video') {
				let fileInfo1: FileInfo = new FileInfo("视频0");
				fileInfo1.type = MediaType.MEDIA_TYPE_ALBUM;
				fileInfo1.path = 'dataability:///com.ohos.medialibrary.MediaLibraryDataAbility/file/12';
				fileInfo1.addedTime = 'xxx';
				fileInfo1.modifiedTime = 'xxx';
				fileList.push(fileInfo1);
				let fileInfo2: FileInfo = new FileInfo("视频1");
				fileInfo2.type = MediaType.MEDIA_TYPE_ALBUM;
				fileInfo2.path = 'dataability:///com.ohos.medialibrary.MediaLibraryDataAbility/file/13';
				fileInfo2.addedTime = 'xxx';
				fileInfo2.modifiedTime = 'xxx';
				fileList.push(fileInfo2);
				let fileInfo3: FileInfo = new FileInfo("视频2");
				fileInfo3.type = MediaType.MEDIA_TYPE_ALBUM;
				fileInfo3.path = 'dataability:///com.ohos.medialibrary.MediaLibraryDataAbility/file/14';
				fileInfo3.addedTime = 'xxx';
				fileInfo3.modifiedTime = 'xxx';
				fileList.push(fileInfo3);
				let fileInfo4: FileInfo = new FileInfo("视频3");
				fileInfo4.type = MediaType.MEDIA_TYPE_ALBUM;
				fileInfo4.path = 'dataability:///com.ohos.medialibrary.MediaLibraryDataAbility/file/15';
				fileInfo4.addedTime = 'xxx';
				fileInfo4.modifiedTime = 'xxx';
				fileList.push(fileInfo4);
				let fileInfo5: FileInfo = new FileInfo("视频4");
				fileInfo5.type = MediaType.MEDIA_TYPE_ALBUM;
				fileInfo5.path = 'dataability:///com.ohos.medialibrary.MediaLibraryDataAbility/file/16';
				fileInfo5.addedTime = 'xxx';
				fileInfo5.modifiedTime = 'xxx';
				fileList.push(fileInfo5);
			} else if (mediaType == 'audio') {
				let fileInfo1: FileInfo = new FileInfo("音乐0");
				fileInfo1.type = MediaType.MEDIA_TYPE_ALBUM;
				fileInfo1.path = 'dataability:///com.ohos.medialibrary.MediaLibraryDataAbility/file/17';
				fileInfo1.addedTime = 'xxx';
				fileInfo1.modifiedTime = 'xxx';
				fileList.push(fileInfo1);
				let fileInfo2: FileInfo = new FileInfo("音乐1");
				fileInfo2.type = MediaType.MEDIA_TYPE_ALBUM;
				fileInfo2.path = 'dataability:///com.ohos.medialibrary.MediaLibraryDataAbility/file/18';
				fileInfo2.addedTime = 'xxx';
				fileInfo2.modifiedTime = 'xxx';
				fileList.push(fileInfo2);
				let fileInfo3: FileInfo = new FileInfo("音乐2");
				fileInfo3.type = MediaType.MEDIA_TYPE_ALBUM;
				fileInfo3.path = 'dataability:///com.ohos.medialibrary.MediaLibraryDataAbility/file/19';
				fileInfo3.addedTime = 'xxx';
				fileInfo3.modifiedTime = 'xxx';
				fileList.push(fileInfo3);
				let fileInfo4: FileInfo = new FileInfo("音乐3");
				fileInfo4.type = MediaType.MEDIA_TYPE_ALBUM;
				fileInfo4.path = 'dataability:///com.ohos.medialibrary.MediaLibraryDataAbility/file/20';
				fileInfo4.addedTime = 'xxx';
				fileInfo4.modifiedTime = 'xxx';
				fileList.push(fileInfo4);
				let fileInfo5: FileInfo = new FileInfo("音乐4");
				fileInfo5.type = MediaType.MEDIA_TYPE_ALBUM;
				fileInfo5.path = 'dataability:///com.ohos.medialibrary.MediaLibraryDataAbility/file/21';
				fileInfo5.addedTime = 'xxx';
				fileInfo5.modifiedTime = 'xxx';
				fileList.push(fileInfo5);
			} else if (mediaType == 'file') {
				let fileInfo1: FileInfo = new FileInfo("文件夹0");
				fileInfo1.type = MediaType.MEDIA_TYPE_ALBUM;
				fileInfo1.mediaType = mediaType;
				fileInfo1.path = 'dataability:///com.ohos.medialibrary.MediaLibraryDataAbility/file/22';
				fileInfo1.addedTime = 'xxx';
				fileInfo1.modifiedTime = 'xxx';
				fileList.push(fileInfo1);
				let fileInfo2: FileInfo = new FileInfo("文件夹1");
				fileInfo2.type = MediaType.MEDIA_TYPE_ALBUM;
				fileInfo2.mediaType = mediaType;
				fileInfo2.path = 'dataability:///com.ohos.medialibrary.MediaLibraryDataAbility/file/23';
				fileInfo2.addedTime = 'xxx';
				fileInfo2.modifiedTime = 'xxx';
				fileList.push(fileInfo2);
				let fileInfo3: FileInfo = new FileInfo("文件夹2");
				fileInfo3.type = MediaType.MEDIA_TYPE_ALBUM;
				fileInfo3.mediaType = mediaType;
				fileInfo3.path = 'dataability:///com.ohos.medialibrary.MediaLibraryDataAbility/file/24';
				fileInfo3.addedTime = 'xxx';
				fileInfo3.modifiedTime = 'xxx';
				fileList.push(fileInfo3);
				let fileInfo4: FileInfo = new FileInfo("文件夹3");
				fileInfo4.type = MediaType.MEDIA_TYPE_ALBUM;
				fileInfo4.mediaType = mediaType;
				fileInfo4.path = 'dataability:///com.ohos.medialibrary.MediaLibraryDataAbility/file/25';
				fileInfo4.addedTime = 'xxx';
				fileInfo4.modifiedTime = 'xxx';
				fileList.push(fileInfo4);
				let fileInfo5: FileInfo = new FileInfo("文件夹4");
				fileInfo5.type = MediaType.MEDIA_TYPE_ALBUM;
				fileInfo5.mediaType = mediaType;
				fileInfo5.path = 'dataability:///com.ohos.medialibrary.MediaLibraryDataAbility/file/26';
				fileInfo5.addedTime = 'xxx';
				fileInfo5.modifiedTime = 'xxx';
				fileList.push(fileInfo5);
				let fileInfo6: FileInfo = new FileInfo("文本文档.txt");
				fileInfo6.type = MediaType.MEDIA_TYPE_FILE;
				fileInfo6.mediaType = mediaType;
				fileInfo6.path = 'dataability:///com.ohos.medialibrary.MediaLibraryDataAbility/file/27';
				fileInfo6.addedTime = 'xxx';
				fileInfo6.modifiedTime = 'xxx';
				fileInfo6.size = '1,024 KB'
				fileList.push(fileInfo6);
				let fileInfo7: FileInfo = new FileInfo("验收文稿.ppt");
				fileInfo7.type = MediaType.MEDIA_TYPE_FILE;
				fileInfo7.mediaType = mediaType;
				fileInfo7.path = 'dataability:///com.ohos.medialibrary.MediaLibraryDataAbility/file/28';
				fileInfo7.addedTime = 'xxx';
				fileInfo7.modifiedTime = 'xxx';
				fileInfo7.size = '4,562,000 KB'
				fileList.push(fileInfo7);
			}
		} else {
			if (mediaType == 'image') {
				for (var index = 0; index < 10; index++) {
					let file: FileInfo = new FileInfo("" + index + ".png");
					file.type = MediaType.MEDIA_TYPE_IMAGE;
					file.path = path + index;
					file.addedTime = 'xxx';
					file.modifiedTime = 'xxx';
					fileList.push(file);
				}
			} else if (mediaType == 'video') {
				for (var index = 0; index < 15; index++) {
					let file: FileInfo = new FileInfo("" + index + ".wmv");
					file.type = MediaType.MEDIA_TYPE_VIDEO;
					file.path = path + index;
					file.addedTime = 'xxx';
					file.modifiedTime = 'xxx';
					fileList.push(file);
				}
			} else if (mediaType == 'audio') {
				for (var index = 0; index < 18; index++) {
					let file: FileInfo = new FileInfo("" + index + ".mp3");
					file.type = MediaType.MEDIA_TYPE_AUDIO;
					file.path = path + index;
					file.addedTime = 'xxx';
					file.modifiedTime = 'xxx';
					fileList.push(file);
				}
			} else if (mediaType == 'file') {
				let fileInfo4: FileInfo = new FileInfo("文件夹3");
				fileInfo4.type = MediaType.MEDIA_TYPE_ALBUM;
				fileInfo4.path = path + '5';
				fileInfo4.addedTime = 'xxx';
				fileInfo4.modifiedTime = 'xxx';
				fileList.push(fileInfo4);
				let fileInfo5: FileInfo = new FileInfo("文件夹4");
				fileInfo5.type = MediaType.MEDIA_TYPE_ALBUM;
				fileInfo5.path = path + '6';
				fileInfo5.addedTime = 'xxx';
				fileInfo5.modifiedTime = 'xxx';
				fileList.push(fileInfo5);
				let fileInfo6: FileInfo = new FileInfo("文本文档.txt");
				fileInfo6.type = MediaType.MEDIA_TYPE_FILE;
				fileInfo6.path = path + '7';
				fileInfo6.size = '1,000 KB'
				fileInfo6.addedTime = 'xxx';
				fileInfo6.modifiedTime = 'xxx';
				fileList.push(fileInfo6);
				let fileInfo7: FileInfo = new FileInfo("验收文稿.ppt");
				fileInfo7.type = MediaType.MEDIA_TYPE_FILE;
				fileInfo7.path = path + '8';
				fileInfo7.size = '520,000 KB'
				fileInfo7.addedTime = 'xxx';
				fileInfo7.modifiedTime = 'xxx';
				fileList.push(fileInfo7);
			}
		}
	} else {
		//    fileList = filemanager.listfile(deviceName, mediaType, path);
	}
	logInfo(TAG, "get file list is " + fileList.length)
	return fileList;
}