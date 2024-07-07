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

#include "tfa9879_codec_ops.h"
#include "audio_codec_base.h"
#include "audio_codec_if.h"
#include "audio_driver_log.h"

#define HDF_LOG_TAG HDF_AUDIO_DRIVER

struct CodecData g_tfa9879Data = {
    .Init = Tfa9879DeviceInit,
    .Read = CodecDeviceRegI2cRead,
    .Write = CodecDeviceRegI2cWrite,
};

struct AudioDaiOps g_tfa9879DaiDeviceOps = {
    .Startup = Tfa9879DaiStartup,
    .HwParams = Tfa9879DaiHwParams,
};

struct DaiData g_tfa9879DaiData = {
    .drvDaiName = "tfa9879_codec_dai",
    .DaiInit = Tfa9879DaiDeviceInit,
    .ops = &g_tfa9879DaiDeviceOps,
    .Read = CodecDaiRegI2cRead,
    .Write = CodecDaiRegI2cWrite,
};

/* HdfDriverEntry implementations */
static int32_t Tfa9879DriverBind(struct HdfDeviceObject *device)
{
    (void)device;
    AUDIO_DRIVER_LOG_INFO("success!");
    return HDF_SUCCESS;
}

static int32_t Tfa9879DriverInit(struct HdfDeviceObject *device)
{
    int32_t ret;
    if (device == NULL) {
        AUDIO_DRIVER_LOG_ERR("device is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }

    ret = CodecGetConfigInfo(device, &g_tfa9879Data);
    if (ret !=  HDF_SUCCESS) {
        AUDIO_DRIVER_LOG_ERR("get config info failed.");
        return ret;
    }

    if (CodecSetConfigInfoOfControls(&g_tfa9879Data, &g_tfa9879DaiData) != HDF_SUCCESS) {
        AUDIO_DRIVER_LOG_ERR("set config info failed.");
        return HDF_FAILURE;
    }

    if (CodecGetServiceName(device, &g_tfa9879Data.drvCodecName) != HDF_SUCCESS) {
        AUDIO_DRIVER_LOG_ERR("get codec service name failed.");
        return HDF_FAILURE;
    }

    if (CodecGetDaiName(device, &g_tfa9879DaiData.drvDaiName) != HDF_SUCCESS) {
        AUDIO_DRIVER_LOG_ERR("get codec dai service name failed.");
        return HDF_FAILURE;
    }

    OsalMutexInit(&g_tfa9879Data.mutex);

    ret = AudioRegisterCodec(device, &g_tfa9879Data, &g_tfa9879DaiData);
    if (ret !=  HDF_SUCCESS) {
        AUDIO_DRIVER_LOG_ERR("AudioRegisterCodec failed.");
        return ret;
    }
    AUDIO_DRIVER_LOG_INFO("success!");
    return HDF_SUCCESS;
}

static void Tfa9879DriverRelease(struct HdfDeviceObject *device)
{
    if (device == NULL) {
        AUDIO_DRIVER_LOG_ERR("device is NULL");
        return;
    }

    OsalMutexDestroy(&g_tfa9879Data.mutex);

    if (device->priv != NULL) {
        OsalMemFree(device->priv);
    }
    struct CodecHost *codecHost = (struct CodecHost *)device->service;
    if (codecHost == NULL) {
        HDF_LOGE("CodecDriverRelease: codecHost is NULL");
        return;
    }
    OsalMemFree(codecHost);
}

/* HdfDriverEntry definitions */
struct HdfDriverEntry g_tfa9879DriverEntry = {
    .moduleVersion = 1,
    .moduleName = "CODEC_TFA9879",
    .Bind = Tfa9879DriverBind,
    .Init = Tfa9879DriverInit,
    .Release = Tfa9879DriverRelease,
};
HDF_INIT(g_tfa9879DriverEntry);
