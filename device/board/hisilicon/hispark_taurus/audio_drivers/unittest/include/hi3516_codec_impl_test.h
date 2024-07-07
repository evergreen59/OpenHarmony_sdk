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

#ifndef HI3516_CODEC_IMPL_TEST_H
#define HI3516_CODEC_IMPL_TEST_H

#include "hdf_types.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* __cplusplus */

int32_t TestCodecHalSysInit(void);
int32_t TestCodecRegBitsRead(void);
int32_t TestCodecRegBitsUpdate(void);
int32_t TestCodecRegDefaultInit(void);
int32_t TestCodecSetAdcTuneEnable(void);
int32_t TestCodecDaiParamsUpdate(void);
int32_t TestAudioCodecAiaoSetCtrlOps(void);
int32_t TestAudioCodecAiaoGetCtrlOps(void);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* __cplusplus */

#endif
