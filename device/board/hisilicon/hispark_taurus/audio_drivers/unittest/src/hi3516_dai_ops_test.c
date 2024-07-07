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

#include "hi3516_dai_ops_test.h"
#include "audio_host.h"
#include "audio_dai_if.h"
#include "audio_platform_if.h"
#include "hi3516_dai_ops.h"
#include "hdf_base.h"
#include "hdf_log.h"
#include "hi3516_common_func.h"

#define HDF_LOG_TAG HDF_AUDIO_DRIVER


int32_t TestDaiDeviceInit(void)
{
    struct AudioCard *audioCard = NULL;
    struct DaiDevice *dai = NULL;
    int ret;

    ret = Hi3516DaiDeviceInit(audioCard, dai);
 
    HDF_LOGI("TestDaiHwParams: success");
    return HDF_SUCCESS;
}

int32_t TestDaiStartup(void)
{
    struct AudioCard *card = NULL;
    struct DaiDevice *device = NULL;
    int ret;

    ret = Hi3516DaiDeviceInit(card, device);
    HDF_LOGI("TestDaiHwParams: success");
    return HDF_SUCCESS;
}

int32_t TestDaiHwParams(void)
{
    int ret;
    struct AudioCard *card = NULL;
    struct AudioPcmHwParams *param = NULL;

    ret = Hi3516DaiHwParams(card, param);

    HDF_LOGI("TestDaiHwParams: success");
    return HDF_SUCCESS;
}

int32_t TestDaiTrigger(void)
{
    int ret;
    struct AudioCard *card = NULL;
    int cmd;
    struct DaiDevice *cpuDai = NULL;

    ret = GetAudioCard(&card);
    if (ret != HDF_SUCCESS) {
        HDF_LOGE("TestDaiTrigger: get card instance failed.");
        return HDF_FAILURE;
    }

    if (card->rtd == NULL || card->rtd->cpuDai == NULL) {
        HDF_LOGE("TestDaiTrigger: card rtd is NULL.");
        return HDF_FAILURE;
    }
    cpuDai = card->rtd->cpuDai;
    if (cpuDai->devData == NULL || cpuDai->devData->ops == NULL) {
        HDF_LOGE("TestDaiTrigger: cpuDai param is NULL.");
        return HDF_FAILURE;
    }
    cmd = 1;

    if (ret != HDF_SUCCESS) {
        HDF_LOGE("TestDaiTrigger: Trigger fail ret = %d", ret);
        return HDF_FAILURE;
    }

    HDF_LOGI("TestDaiTrigger: success");
    return HDF_SUCCESS;
}
