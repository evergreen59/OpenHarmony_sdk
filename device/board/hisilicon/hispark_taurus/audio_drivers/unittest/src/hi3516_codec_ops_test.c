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

#include "hi3516_codec_ops_test.h"
#include "audio_codec_base.h"
#include "devsvc_manager_clnt.h"
#include "hdf_base.h"
#include "hdf_log.h"
#include "hi3516_codec_ops.h"
#include "hi3516_common_func.h"

#define HDF_LOG_TAG HDF_AUDIO_DRIVER

int32_t TestCodecDeviceInit(void)
{
    int32_t ret;
    struct AudioCard *audioCard = NULL;
    struct CodecDevice *codec = NULL;
    HDF_LOGI("%s: enter", __func__);

    ret = Hi3516CodecDeviceInit(audioCard, codec);

    HDF_LOGI("TestCodecDeviceInit:%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t TestCodecDaiDeviceInit(void)
{
    int ret;
    struct AudioCard *card = NULL;
    struct DaiDevice *device = NULL;
    HDF_LOGI("%s: enter", __func__);

    ret = Hi3516CodecDaiDeviceInit(card, device);

    HDF_LOGI("TestCodecDaiDeviceInit: success");
    OsalMemFree(device);
    return HDF_SUCCESS;
}

int32_t TestCodecDaiStartup(void)
{
    int ret;
    struct AudioCard *card = NULL;
    struct DaiDevice *device = NULL;
    HDF_LOGI("%s: enter", __func__);

    ret = Hi3516CodecDaiStartup(card, device);

    HDF_LOGI("TestCodecDaiStartup: success");
    return HDF_SUCCESS;
}

int32_t TestCodecDaiHwParams(void)
{
    struct AudioCard *card = NULL;
    struct AudioPcmHwParams *param = NULL;
    int ret;
    HDF_LOGI("TestCodecDaiHwParams: enter");

    ret = Hi3516CodecDaiHwParams(card, param);

    HDF_LOGI("TestCodecDaiHwParams: success");
    return HDF_SUCCESS;
}
