/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "hi3516_common_func.h"

int32_t InitHwParam(struct AudioPcmHwParams *hwParam)
{
    const uint32_t channelNum = 2;
    const uint32_t sampleRate = 48000;
    const uint32_t periodSize = 4096;
    const uint32_t periodCount = 8;
    const int format = 2;
    const uint32_t startThreshold = 1024;
    const uint32_t stopThreshold = 0x7fffffff;
    const uint32_t silenceThreshold = 1024 * 16;

    if (hwParam == NULL) {
        HDF_LOGE("InitHwParam: input param is NULL.");
        return HDF_FAILURE;
    }

    hwParam->channels  = channelNum;
    hwParam->rate = sampleRate;
    hwParam->periodSize = periodSize;
    hwParam->periodCount = periodCount;
    hwParam->format = format;
    hwParam->cardServiceName = "hdf_audio_codec_dev0";
    hwParam->isBigEndian = false;
    hwParam->isSignedData = true;
    hwParam->startThreshold = startThreshold;
    hwParam->stopThreshold = stopThreshold;
    hwParam->silenceThreshold = silenceThreshold;
    hwParam->streamType = 0; // AUDIO_RENDER_STREAM
    return HDF_SUCCESS;
}


int32_t GetAudioCard(struct AudioCard **card)
{
    HDF_LOGI("%s: enter", __func__);
    if (card == NULL) {
        HDF_LOGE("input param is NULL");
        return HDF_FAILURE;
    }
    *card = GetCardInstance("hdf_audio_codec_primary_dev0");

    if (*card == NULL) {
        HDF_LOGE("get card instance failed");
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}
