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
import screenManager from '@ohos/base/src/main/ets/manager/ScreenManager';
import { computeSampleSize } from '@ohos/base/src/main/ets/utils/ImageUtil';
import { MediaDataItem } from '@ohos/base/src/main/ets/data/MediaDataItem';
import { MediaConstants } from '@ohos/base/src/main/ets/constants/MediaConstants';

export async function getThumbnail(mediaItem: MediaDataItem, isCurrent: boolean): Promise<string> {
    await mediaItem.load(true);
    let imgWidth = mediaItem.width;
    let imgHeight = mediaItem.height;
    let scale = generateSampleSize(imgWidth, imgHeight, isCurrent);
    mediaItem.imgWidth = Math.ceil(mediaItem.width / scale);
    mediaItem.imgHeight = Math.ceil(mediaItem.height / scale);
    imgWidth = Math.ceil(imgWidth / scale);
    imgHeight = Math.ceil(imgHeight / scale);
    return mediaItem.getThumbnail(imgWidth, imgHeight);
}

function generateSampleSize(imageWidth: number, imageHeight: number, isCurrent: boolean): number {
    let width = vp2px(screenManager.getWinWidth());
    let height = vp2px(screenManager.getWinHeight());
    width = width == 0 ? screenManager.DEFAULT_WIDTH : width;
    height = height == 0 ? screenManager.DEFAULT_HEIGHT : height;
    let maxNumOfPixels;
    if (isCurrent) {
        maxNumOfPixels = 2 * width * height;
    } else {
        maxNumOfPixels = width * height;
    }
    let minSide = Math.min(width, height);
    return computeSampleSize(imageWidth, imageHeight, minSide, maxNumOfPixels);
}