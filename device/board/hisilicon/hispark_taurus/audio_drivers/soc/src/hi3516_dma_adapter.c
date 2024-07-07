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

#include "audio_core.h"
#include "audio_driver_log.h"
#include "hi3516_dma_ops.h"

#define HDF_LOG_TAG HDF_AUDIO_DRIVER

struct AudioDmaOps g_dmaDeviceOps = {
    .DmaBufAlloc = Hi3516DmaBufAlloc,
    .DmaBufFree = Hi3516DmaBufFree,
    .DmaRequestChannel = Hi3516DmaRequestChannel,
    .DmaConfigChannel = Hi3516DmaConfigChannel,
    .DmaPrep = Hi3516DmaPrep,
    .DmaSubmit = Hi3516DmaSubmit,
    .DmaPending = Hi3516DmaPending,
    .DmaPause = Hi3516DmaPause,
    .DmaResume = Hi3516DmaResume,
    .DmaPointer = Hi3516DmaPointer,
};

struct PlatformData g_platformData = {
    .PlatformInit = AudioDmaDeviceInit,
    .ops = &g_dmaDeviceOps,
};

/* HdfDriverEntry implementations */
static int32_t Hi3516DmaDriverBind(struct HdfDeviceObject *device)
{
    struct PlatformHost *platformHost = NULL;

    if (device == NULL) {
        AUDIO_DRIVER_LOG_ERR("input para is NULL.");
        return HDF_FAILURE;
    }

    platformHost = (struct PlatformHost *)OsalMemCalloc(sizeof(*platformHost));
    if (platformHost == NULL) {
        AUDIO_DRIVER_LOG_ERR("malloc host fail!");
        return HDF_FAILURE;
    }

    platformHost->device = device;
    device->service = &platformHost->service;

    AUDIO_DRIVER_LOG_DEBUG("success!");
    return HDF_SUCCESS;
}

static int32_t Hi3516DmaGetServiceName(const struct HdfDeviceObject *device)
{
    if (device == NULL) {
        AUDIO_DRIVER_LOG_ERR("para is NULL.");
        return HDF_FAILURE;
    }

    const struct DeviceResourceNode *node = device->property;
    if (node == NULL) {
        AUDIO_DRIVER_LOG_ERR("node is NULL.");
        return HDF_FAILURE;
    }

    struct DeviceResourceIface *drsOps = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (drsOps == NULL || drsOps->GetString == NULL) {
        AUDIO_DRIVER_LOG_ERR("get drsops object instance fail!");
        return HDF_FAILURE;
    }

    int32_t ret = drsOps->GetString(node, "serviceName", &g_platformData.drvPlatformName, 0);
    if (ret != HDF_SUCCESS) {
        AUDIO_DRIVER_LOG_ERR("read serviceName fail!");
        return ret;
    }

    return HDF_SUCCESS;
}

static int32_t Hi3516DmaDriverInit(struct HdfDeviceObject *device)
{
    if (device == NULL) {
        AUDIO_DRIVER_LOG_ERR("device is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }

    int32_t ret = Hi3516DmaGetServiceName(device);
    if (ret !=  HDF_SUCCESS) {
        AUDIO_DRIVER_LOG_ERR("get service name fail.");
        return ret;
    }
    OsalMutexInit(&g_platformData.renderBufInfo.buffMutex);
    OsalMutexInit(&g_platformData.captureBufInfo.buffMutex);
    g_platformData.platformInitFlag = false;
    ret = AudioSocRegisterPlatform(device, &g_platformData);
    if (ret !=  HDF_SUCCESS) {
        AUDIO_DRIVER_LOG_ERR("register dai fail.");
        return ret;
    }

    AUDIO_DRIVER_LOG_INFO("success.\n");
    return HDF_SUCCESS;
}

static void Hi3516DmaDriverRelease(struct HdfDeviceObject *device)
{
    struct PlatformHost *platformHost = NULL;

    if (device == NULL) {
        AUDIO_DRIVER_LOG_ERR("device is NULL");
        return;
    }

    platformHost = (struct PlatformHost *)device->service;
    if (platformHost == NULL) {
        AUDIO_DRIVER_LOG_ERR("platformHost is NULL");
        return;
    }
    OsalMutexDestroy(&g_platformData.renderBufInfo.buffMutex);
    OsalMutexDestroy(&g_platformData.captureBufInfo.buffMutex);
    OsalMemFree(platformHost);
}

/* HdfDriverEntry definitions */
struct HdfDriverEntry g_platformDriverEntry = {
    .moduleVersion = 1,
    .moduleName = "DMA_HI3516",
    .Bind = Hi3516DmaDriverBind,
    .Init = Hi3516DmaDriverInit,
    .Release = Hi3516DmaDriverRelease,
};
HDF_INIT(g_platformDriverEntry);
