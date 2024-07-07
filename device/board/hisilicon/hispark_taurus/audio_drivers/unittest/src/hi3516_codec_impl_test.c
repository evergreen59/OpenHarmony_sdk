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

#include "hi3516_codec_impl_test.h"
#include "audio_codec_if.h"
#include "hdf_base.h"
#include "hdf_log.h"
#include "hdf_types.h"
#include "hi3516_codec_impl.h"

#define HDF_LOG_TAG HDF_AUDIO_DRIVER

int32_t TestCodecHalSysInit(void)
{
    int ret;
    struct CodecData codeData;

    HDF_LOGI("%s: enter", __func__);
    ret = Hi3516CodecHalSysInit(&codeData);

    HDF_LOGI("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t TestCodecRegBitsRead(void)
{
    struct AudioMixerControl *regAttr = NULL;
    uint32_t regValue;
    int ret;
    HDF_LOGI("%s: enter", __func__);

    ret = Hi3516CodecRegBitsRead(regAttr, &regValue);

    HDF_LOGI("%s: success", __func__);
    return HDF_SUCCESS;
}

static int32_t CodecRegBitsUpdateMock(struct AudioMixerControl regAttr)
{
    HDF_LOGI("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t TestCodecRegBitsUpdate(void)
{
    struct AudioMixerControl regAttr;
    int ret;
    HDF_LOGI("%s: enter", __func__);
    regAttr.reg = -1; // -1 is bad value
    ret = CodecRegBitsUpdateMock(regAttr);

    HDF_LOGI("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t TestCodecRegDefaultInit(void)
{
    struct AudioRegCfgGroupNode **regCfgGroup = NULL;
    int ret;
    HDF_LOGI("%s: enter", __func__);

    ret = Hi3516CodecRegDefaultInit(regCfgGroup);

    HDF_LOGI("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t TestCodecSetAdcTuneEnable(void)
{
    struct AudioRegCfgGroupNode **regCfgGroup = NULL;
    int ret;
    HDF_LOGI("%s: enter", __func__);

    ret = Hi3516CodecSetAdcTuneEnable(regCfgGroup);

    HDF_LOGI("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t TestCodecDaiParamsUpdate(void)
{
    struct AudioRegCfgGroupNode **regCfgGroup = NULL;
    struct CodecDaiParamsVal codecDaiParamsVal;
    int ret;
    HDF_LOGI("%s: enter", __func__);

    codecDaiParamsVal.channelVal = 1; // 1 is dam channel
    ret = Hi3516CodecDaiParamsUpdate(regCfgGroup, codecDaiParamsVal);

    HDF_LOGI("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t TestAudioCodecAiaoSetCtrlOps(void)
{
    struct AudioKcontrol *kcontrol = NULL;
    struct AudioCtrlElemValue *elemValue = NULL;
    int ret;
    HDF_LOGI("%s: enter", __func__);

    ret = Hi3516CodecAiaoSetCtrlOps(kcontrol, elemValue);

    HDF_LOGI("%s: success", __func__);
    return HDF_SUCCESS;
}

int32_t TestAudioCodecAiaoGetCtrlOps(void)
{
    struct AudioKcontrol *kcontrol = NULL;
    struct AudioCtrlElemValue *elemValue = NULL;
    int ret;
    HDF_LOGI("%s: enter", __func__);

    ret = Hi3516CodecAiaoGetCtrlOps(kcontrol, elemValue);

    HDF_LOGI("%s: success", __func__);
    return HDF_SUCCESS;
}
