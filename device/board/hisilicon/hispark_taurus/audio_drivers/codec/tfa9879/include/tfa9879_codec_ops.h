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

#ifndef TFA9879_CODEC_OPS_H
#define TFA9879_CODEC_OPS_H

#include <linux/types.h>
#include "osal_io.h"
#include "osal_mem.h"
#include "osal_time.h"
#include "securec.h"
#include "audio_codec_if.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

enum Tfa9879I2sFormatRegVal {
    I2S_SAMPLE_FORMAT_REG_VAL_MSB_24    = 0x2,    /*  MSB-justified data up to 24 bits */
    I2S_SAMPLE_FORMAT_REG_VAL_24        = 0x3,    /*  I2S data up to 24 bits */
    I2S_SAMPLE_FORMAT_REG_VAL_LSB_16    = 0x4,    /*  LSB-justified 16-bit data */
    I2S_SAMPLE_FORMAT_REG_VAL_LSB_18    = 0x5,    /*  LSB-justified 18-bit data */
    I2S_SAMPLE_FORMAT_REG_VAL_LSB_20    = 0x6,    /*  LSB-justified 20-bit data */
    I2S_SAMPLE_FORMAT_REG_VAL_LSB_24    = 0x7,    /*  LSB-justified 24-bit data */
};

int32_t Tfa9879DeviceInit(struct AudioCard *audioCard, const struct CodecDevice *device);
int32_t Tfa9879DaiDeviceInit(struct AudioCard *card, const struct DaiDevice *device);
int32_t Tfa9879DaiStartup(const struct AudioCard *card, const struct DaiDevice *device);
int32_t Tfa9879DaiHwParams(const struct AudioCard *card, const struct AudioPcmHwParams *param);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif
