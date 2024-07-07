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
#include "audio_sapm.h"
#include "audio_platform_base.h"
#include "hi3516_codec_impl.h"
#include "audio_driver_log.h"
#include "audio_codec_base.h"

#define HDF_LOG_TAG HDF_AUDIO_DRIVER

static const struct AudioSapmRoute g_audioRoutes[] = {
    { "SPKL", NULL, "SPKL PGA"},
    { "SPKR", NULL, "SPKR PGA"},
    { "SPKL PGA", "Dacl enable", "DACL"},
    { "SPKR PGA", "Dacr enable", "DACR"},

    { "ADCL", NULL, "LPGA"},
    { "LPGA", "LPGA MIC Switch", "MIC1"},

    { "ADCR", NULL, "RPGA"},
    { "RPGA", "RPGA MIC Switch", "MIC2"},
};

int32_t Hi3516CodecDeviceInit(struct AudioCard *audioCard, const struct CodecDevice *codec)
{
    if (audioCard == NULL || codec == NULL || codec->devData == NULL ||
        codec->devData->sapmComponents == NULL || codec->devData->controls == NULL) {
        AUDIO_DRIVER_LOG_ERR("input para is NULL.");
        return HDF_ERR_INVALID_OBJECT;
    }

    if (CodecSetCtlFunc(codec->devData, AUDIO_CONTROL_MIXER, Hi3516CodecAiaoGetCtrlOps,
        Hi3516CodecAiaoSetCtrlOps) != HDF_SUCCESS) {
        AUDIO_DRIVER_LOG_ERR("AudioCodecSetCtlFunc failed.");
        return HDF_FAILURE;
    }

    if (Hi3516CodecHalSysInit(codec->devData) != HDF_SUCCESS) {
        AUDIO_DRIVER_LOG_ERR("Hi3516CodecHalSysInit failed.");
        return HDF_FAILURE;
    }

    if (Hi3516CodecRegDefaultInit(codec->devData->regCfgGroup) != HDF_SUCCESS) {
        AUDIO_DRIVER_LOG_ERR("Hi3516CodecRegDefaultInit failed.");
        return HDF_FAILURE;
    }

    if (AudioAddControls(audioCard, codec->devData->controls, codec->devData->numControls) != HDF_SUCCESS) {
        AUDIO_DRIVER_LOG_ERR("add controls failed.");
        return HDF_FAILURE;
    }

    if (AudioSapmNewComponents(audioCard, codec->devData->sapmComponents,
        codec->devData->numSapmComponent) != HDF_SUCCESS) {
        AUDIO_DRIVER_LOG_ERR("new components failed.");
        return HDF_FAILURE;
    }

    if (AudioSapmAddRoutes(audioCard, g_audioRoutes, HDF_ARRAY_SIZE(g_audioRoutes)) != HDF_SUCCESS) {
        AUDIO_DRIVER_LOG_ERR("add route failed.");
        return HDF_FAILURE;
    }

    if (AudioSapmNewControls(audioCard) != HDF_SUCCESS) {
        AUDIO_DRIVER_LOG_ERR("add sapm controls failed.");
        return HDF_FAILURE;
    }

    if (AudioSapmSleep(audioCard) != HDF_SUCCESS) {
        AUDIO_DRIVER_LOG_ERR("add sapm sleep modular failed.");
        return HDF_FAILURE;
    }

    AUDIO_DRIVER_LOG_DEBUG("success.");
    return HDF_SUCCESS;
}

int32_t Hi3516CodecDaiDeviceInit(struct AudioCard *card, const struct DaiDevice *device)
{
    if (device == NULL || device->devDaiName == NULL) {
        AUDIO_DRIVER_LOG_ERR("input para is NULL.");
        return HDF_FAILURE;
    }

    AUDIO_DRIVER_LOG_DEBUG("codec dai device name: %s\n", device->devDaiName);
    (void)card;
    return HDF_SUCCESS;
}

int32_t Hi3516CodecDaiHwParams(const struct AudioCard *card, const struct AudioPcmHwParams *param)
{
    unsigned int bitWidth;
    struct CodecDaiParamsVal codecDaiParamsVal;

    if (param == NULL || param->cardServiceName == NULL || card == NULL ||
        card->rtd == NULL || card->rtd->codecDai == NULL || card->rtd->codecDai->devData == NULL ||
        card->rtd->codecDai->devData->regCfgGroup == NULL) {
        AUDIO_DRIVER_LOG_ERR("input para is NULL.");
        return HDF_FAILURE;
    }

    (void)memset_s(&bitWidth, sizeof(unsigned int), 0, sizeof(unsigned int));
    int ret = AudioFormatToBitWidth(param->format, &bitWidth);
    if (ret != HDF_SUCCESS) {
        return HDF_FAILURE;
    }

    AUDIO_DRIVER_LOG_DEBUG("channels count : %d .", param->channels);
    (void)memset_s(&codecDaiParamsVal, sizeof(struct CodecDaiParamsVal), 0, sizeof(struct CodecDaiParamsVal));
    codecDaiParamsVal.frequencyVal = param->rate;
    codecDaiParamsVal.formatVal = bitWidth;
    ret = Hi3516CodecDaiParamsUpdate(card->rtd->codecDai->devData->regCfgGroup, codecDaiParamsVal);
    if (ret != HDF_SUCCESS) {
        AUDIO_DRIVER_LOG_ERR("Hi3516CodecDaiParamsUpdate failed.");
        return HDF_FAILURE;
    }
    return HDF_SUCCESS;
}

int32_t Hi3516CodecDaiStartup(const struct AudioCard *card, const struct DaiDevice *device)
{
    int32_t ret;
    if (device == NULL || device->devData == NULL ||
        device->devData->regCfgGroup == NULL) {
        AUDIO_DRIVER_LOG_ERR("input para is NULL.");
        return HDF_FAILURE;
    }
    (void)card;
    ret = Hi3516CodecSetAdcTuneEnable(device->devData->regCfgGroup);
    if (ret != HDF_SUCCESS) {
        AUDIO_DRIVER_LOG_ERR("Hi3516CodecSetAdcTuneEnable failed.");
        return HDF_FAILURE;
    }
    AUDIO_DRIVER_LOG_DEBUG("success.");
    return HDF_SUCCESS;
}

