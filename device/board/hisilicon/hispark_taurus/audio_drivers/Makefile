#
# Copyright (c) 2022 Huawei Device Co., Ltd.
#
# This software is licensed under the terms of the GNU General Public
# License version 2, as published by the Free Software Foundation, and
# may be copied, distributed, and modified under those terms.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
#

KHDF_AUDIO_KHDF_ROOT_DIR = drivers/hdf/khdf
KHDF_FRAMEWORK_ROOT_DIR = drivers/hdf/framework
KHDF_AUDIO_HI3516DV300_DIR = $(KHDF_AUDIO_BASE_ROOT_DIR)/device/board/hisilicon/hispark_taurus/audio_drivers
KHDF_AUDIO_HI3516DV300_INC_DIR = drivers/hdf/framework/../../../device/board/hisilicon/hispark_taurus/audio_drivers

obj-$(CONFIG_DRIVERS_HDF_AUDIO_HI3516CODEC) += \
        codec/tfa9879/src/tfa9879_codec_adapter.o \
        codec/tfa9879/src/tfa9879_codec_ops.o \
        codec/hi3516/src/hi3516_codec_adapter.o \
        codec/hi3516/src/hi3516_codec_impl.o \
        codec/hi3516/src/hi3516_codec_ops.o \
        dsp/src/dsp_adapter.o \
        dsp/src/dsp_ops.o \
        soc/src/hi3516_dai_adapter.o \
        soc/src/hi3516_dai_ops.o \
        soc/src/hi3516_aiao_impl.o \
        soc/src/hi3516_dma_ops.o \
        soc/src/hi3516_dma_adapter.o

ccflags-$(CONFIG_DRIVERS_HDF_AUDIO_HI3516CODEC) += \
        -I$(srctree)/$(KHDF_AUDIO_KHDF_ROOT_DIR)/osal/include \
        -I$(srctree)/$(KHDF_FRAMEWORK_ROOT_DIR)/include/core \
        -I$(srctree)/$(KHDF_FRAMEWORK_ROOT_DIR)/include/utils \
        -I$(srctree)/$(KHDF_FRAMEWORK_ROOT_DIR)/include/osal \
        -I$(srctree)/$(KHDF_FRAMEWORK_ROOT_DIR)/include/platform \
        -I$(srctree)/$(KHDF_FRAMEWORK_ROOT_DIR)/include/config \
        -I$(srctree)/$(KHDF_FRAMEWORK_ROOT_DIR)/include/audio \
        -I$(srctree)/$(KHDF_FRAMEWORK_ROOT_DIR)/ability/sbuf/include \
        -I$(srctree)/$(KHDF_FRAMEWORK_ROOT_DIR)/core/common/include/host \
        -I$(srctree)/$(KHDF_FRAMEWORK_ROOT_DIR)/core/host/include \
        -I$(srctree)/$(KHDF_FRAMEWORK_ROOT_DIR)/model/audio/core/include \
        -I$(srctree)/$(KHDF_FRAMEWORK_ROOT_DIR)/model/audio/sapm/include \
        -I$(srctree)/$(KHDF_FRAMEWORK_ROOT_DIR)/model/audio/dispatch/include \
        -I$(srctree)/$(KHDF_FRAMEWORK_ROOT_DIR)/model/audio/common/include \
        -I$(srctree)/bounds_checking_function/include \
        -I$(srctree)/$(KHDF_AUDIO_HI3516DV300_INC_DIR)/codec/hi3516/include \
        -I$(srctree)/$(KHDF_AUDIO_HI3516DV300_INC_DIR)/soc/include \
        -I$(srctree)/$(KHDF_AUDIO_HI3516DV300_INC_DIR)/dsp/include \
        -I$(srctree)/$(KHDF_AUDIO_HI3516DV300_INC_DIR)/codec/tfa9879/include
