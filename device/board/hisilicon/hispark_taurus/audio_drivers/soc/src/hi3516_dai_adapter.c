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

#include "hi3516_dai_ops.h"
#include "audio_core.h"
#include "audio_dai_base.h"
#include "audio_driver_log.h"
#include "osal_io.h"

#define HDF_LOG_TAG HDF_AUDIO_DRIVER

struct AudioDaiOps g_hi3516DaiDeviceOps = {
    .HwParams = Hi3516DaiHwParams,
    .Trigger = Hi3516DaiTrigger,
    .Startup = Hi3516DaiStartup,
};

struct DaiData g_hi3516DaiData = {
    .DaiInit = Hi3516DaiDeviceInit,
    .Read = DaiDeviceReadReg,
    .Write = DaiDeviceWriteReg,
    .ops = &g_hi3516DaiDeviceOps,
};

/* HdfDriverEntry implementations */
static int32_t DaiDriverBind(struct HdfDeviceObject *device)
{
    if (device == NULL) {
        AUDIO_DRIVER_LOG_ERR("input para is NULL.");
        return HDF_FAILURE;
    }

    struct DaiHost *daiHost = (struct DaiHost *)OsalMemCalloc(sizeof(*daiHost));
    if (daiHost == NULL) {
        AUDIO_DRIVER_LOG_ERR("malloc host fail!");
        return HDF_FAILURE;
    }

    daiHost->device = device;
    device->service = &daiHost->service;
    g_hi3516DaiData.daiInitFlag = false;

    AUDIO_DRIVER_LOG_DEBUG("success!");
    return HDF_SUCCESS;
}

static int32_t DaiGetServiceName(const struct HdfDeviceObject *device)
{
    if (device == NULL) {
        AUDIO_DRIVER_LOG_ERR("input para is nullptr.");
        return HDF_FAILURE;
    }

    const struct DeviceResourceNode *node = device->property;
    if (node == NULL) {
        AUDIO_DRIVER_LOG_ERR("drs node is nullptr.");
        return HDF_FAILURE;
    }
    struct DeviceResourceIface *drsOps = DeviceResourceGetIfaceInstance(HDF_CONFIG_SOURCE);
    if (drsOps == NULL || drsOps->GetString == NULL) {
        AUDIO_DRIVER_LOG_ERR("invalid drs ops fail!");
        return HDF_FAILURE;
    }

    int32_t ret = drsOps->GetString(node, "serviceName", &g_hi3516DaiData.drvDaiName, 0);
    if (ret != HDF_SUCCESS) {
        AUDIO_DRIVER_LOG_ERR("read serviceName fail!");
        return ret;
    }

    return HDF_SUCCESS;
}

static int32_t DaiDriverInit(struct HdfDeviceObject *device)
{
    if (device == NULL) {
        AUDIO_DRIVER_LOG_ERR("device is nullptr.");
        return HDF_ERR_INVALID_OBJECT;
    }

    if (DaiGetConfigInfo(device, &g_hi3516DaiData) !=  HDF_SUCCESS) {
        AUDIO_DRIVER_LOG_ERR("get dai data fail.");
        return HDF_FAILURE;
    }

    if (DaiGetServiceName(device) !=  HDF_SUCCESS) {
        AUDIO_DRIVER_LOG_ERR("get service name fail.");
        return HDF_FAILURE;
    }

    OsalMutexInit(&g_hi3516DaiData.mutex);

    int32_t ret = AudioSocRegisterDai(device, &g_hi3516DaiData);
    if (ret !=  HDF_SUCCESS) {
        AUDIO_DRIVER_LOG_ERR("register dai fail.");
        return ret;
    }

    AUDIO_DRIVER_LOG_DEBUG("success.\n");
    return HDF_SUCCESS;
}

static void DaiDriverRelease(struct HdfDeviceObject *device)
{
    if (device == NULL) {
        AUDIO_DRIVER_LOG_ERR("device is NULL");
        return;
    }

    OsalMutexDestroy(&g_hi3516DaiData.mutex);

    struct DaiHost *daiHost = (struct DaiHost *)device->service;
    if (daiHost == NULL) {
        AUDIO_DRIVER_LOG_ERR("daiHost is NULL");
        return;
    }
    OsalMemFree(daiHost);
}

/* HdfDriverEntry definitions */
struct HdfDriverEntry g_daiDriverEntry = {
    .moduleVersion = 1,
    .moduleName = "DAI_HI3516",
    .Bind = DaiDriverBind,
    .Init = DaiDriverInit,
    .Release = DaiDriverRelease,
};
HDF_INIT(g_daiDriverEntry);
