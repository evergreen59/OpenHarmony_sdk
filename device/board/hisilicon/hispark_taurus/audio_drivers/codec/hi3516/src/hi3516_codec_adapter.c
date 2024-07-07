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

#include "hi3516_codec_ops.h"
#include "audio_codec_base.h"
#include "audio_core.h"
#include "audio_driver_log.h"

#define HDF_LOG_TAG HDF_AUDIO_DRIVER

struct CodecData g_hi3516CodecData = {
    .Init = Hi3516CodecDeviceInit,
    .Read = CodecDeviceReadReg,
    .Write = CodecDeviceWriteReg,
};

struct AudioDaiOps g_hi3516CodecDaiDeviceOps = {
    .Startup = Hi3516CodecDaiStartup,
    .HwParams = Hi3516CodecDaiHwParams,
};

struct DaiData g_codecDaiData = {
    .DaiInit = Hi3516CodecDaiDeviceInit,
    .ops = &g_hi3516CodecDaiDeviceOps,
};

/* HdfDriverEntry implementations */
static int32_t CodecDriverBind(struct HdfDeviceObject *device)
{
    if (device == NULL) {
        AUDIO_DRIVER_LOG_ERR("input para is NULL.");
        return HDF_FAILURE;
    }

    struct CodecHost *codecHost = (struct CodecHost *)OsalMemCalloc(sizeof(*codecHost));
    if (codecHost == NULL) {
        AUDIO_DRIVER_LOG_ERR("malloc codecHost fail!");
        return HDF_FAILURE;
    }
    codecHost->device = device;
    device->service = &codecHost->service;

    return HDF_SUCCESS;
}

static int32_t CodecDriverInit(struct HdfDeviceObject *device)
{
    if (device == NULL) {
        AUDIO_DRIVER_LOG_ERR("device is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }

    if (CodecGetConfigInfo(device, &g_hi3516CodecData) != HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    if (CodecSetConfigInfoOfControls(&g_hi3516CodecData, &g_codecDaiData) != HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    if (CodecGetServiceName(device, &g_hi3516CodecData.drvCodecName) != HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    if (CodecGetDaiName(device, &g_codecDaiData.drvDaiName) != HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    OsalMutexInit(&g_hi3516CodecData.mutex);

    if (AudioRegisterCodec(device, &g_hi3516CodecData, &g_codecDaiData) != HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    return HDF_SUCCESS;
}

static void CodecDriverRelease(struct HdfDeviceObject *device)
{
    if (device == NULL) {
        AUDIO_DRIVER_LOG_ERR("device is NULL");
        return;
    }

    OsalMutexDestroy(&g_hi3516CodecData.mutex);

    if (g_hi3516CodecData.sapmComponents != NULL) {
        OsalMemFree(g_hi3516CodecData.sapmComponents);
    }

    if (g_hi3516CodecData.regConfig != NULL) {
        OsalMemFree(g_hi3516CodecData.regConfig);
    }

    if (g_hi3516CodecData.controls != NULL) {
        OsalMemFree(g_hi3516CodecData.controls);
    }

    if (g_hi3516CodecData.regCfgGroup != NULL) {
        OsalMemFree(g_hi3516CodecData.regCfgGroup);
    }

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
struct HdfDriverEntry g_codecDriverEntry = {
    .moduleVersion = 1,
    .moduleName = "CODEC_HI3516",
    .Bind = CodecDriverBind,
    .Init = CodecDriverInit,
    .Release = CodecDriverRelease,
};
HDF_INIT(g_codecDriverEntry);
