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

#ifndef HI3516_CODEC_OPS_TEST_H
#define HI3516_CODEC_OPS_TEST_H

#include "hdf_types.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

int32_t TestCodecDeviceInit(void);
int32_t TestCodecDeviceInitFail(void);
int32_t TestCodecDaiDeviceInit(void);
int32_t TestCodecDaiDeviceInitFail(void);
int32_t TestCodecDaiStartup(void);
int32_t TestCodecDaiHwParams(void);
int32_t TestCodecDaiInvalidBitWidthParam(void);
int32_t TestCodecDaiInvalidRateParam(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif
