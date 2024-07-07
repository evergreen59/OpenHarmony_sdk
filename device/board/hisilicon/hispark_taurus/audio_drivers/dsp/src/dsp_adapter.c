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

#include "audio_dsp_base.h"
#include "audio_core.h"
#include "dsp_ops.h"
#include "audio_dsp_if.h"
#include "audio_driver_log.h"
#include "osal_io.h"

#define HDF_LOG_TAG HDF_AUDIO_DRIVER

struct DspData g_dspData = {
    .DspInit = DspDeviceInit,
    .Read = DspDeviceReadReg,
    .Write = DspDeviceWriteReg,
    .Decode = DspDecodeAudioStream,
    .Encode = DspEncodeAudioStream,
    .Equalizer = DspEqualizerActive,
};

struct AudioDaiOps g_dspDaiDeviceOps = {
    .Startup = DspDaiStartup,
    .HwParams = DspDaiHwParams,
};

struct DaiData g_dspDaiData = {
    .DaiInit = DspDaiDeviceInit,
    .ops = &g_dspDaiDeviceOps,
};

static int32_t DspDriverBind(struct HdfDeviceObject *device)
{
    AUDIO_DRIVER_LOG_INFO("entry");

    if (device == NULL) {
        AUDIO_DRIVER_LOG_ERR("device is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }

    AUDIO_DRIVER_LOG_INFO("success");
    return HDF_SUCCESS;
}

static int32_t DspDriverInit(struct HdfDeviceObject *device)
{
    AUDIO_DRIVER_LOG_INFO("entry");

    if (device == NULL) {
        AUDIO_DRIVER_LOG_ERR("device is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }

    int32_t ret = DspGetServiceName(device, &g_dspData.drvDspName);
    if (ret != HDF_SUCCESS) {
        return ret;
    }

    ret = DspGetDaiName(device, &g_dspDaiData.drvDaiName);
    if (ret != HDF_SUCCESS) {
        return ret;
    }

    ret = AudioRegisterDsp(device, &g_dspData, &g_dspDaiData);
    if (ret !=  HDF_SUCCESS) {
        return ret;
    }

    AUDIO_DRIVER_LOG_INFO("success");
    return HDF_SUCCESS;
}


static void DspDriverRelease(struct HdfDeviceObject *device)
{
    AUDIO_DRIVER_LOG_INFO("entry");
    if (device == NULL) {
        AUDIO_DRIVER_LOG_ERR("device is NULL");
        return;
    }

    struct DspHost *dspHost = (struct DspHost *)device->service;
    if (dspHost == NULL) {
        AUDIO_DRIVER_LOG_ERR("DspHost is NULL");
        return;
    }
    if (dspHost->priv != NULL) {
        OsalMemFree(dspHost->priv);
    }
    OsalMemFree(dspHost);
    AUDIO_DRIVER_LOG_INFO("success");
}

/* HdfDriverEntry definitions */
struct HdfDriverEntry g_dspDriverEntry = {
    .moduleVersion = 1,
    .moduleName = "DSP",
    .Bind = DspDriverBind,
    .Init = DspDriverInit,
    .Release = DspDriverRelease,
};
HDF_INIT(g_dspDriverEntry);
