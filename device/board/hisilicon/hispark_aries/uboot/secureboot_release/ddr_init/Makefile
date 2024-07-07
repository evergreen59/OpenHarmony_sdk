# Copyright (C) 2021 HiSilicon (Shanghai) Technologies CO., LIMITED.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

include cfg.mk

export CROSS_COMPILE := $(shell pwd)/../../../../../../../prebuilts/gcc-arm-none-eabi-7-2017-q4-major/bin/arm-none-eabi-

export CC := $(CROSS_COMPILE)gcc
export AR := $(CROSS_COMPILE)ar
export RANLIB := $(CROSS_COMPILE)ranlib
export LD := $(CROSS_COMPILE)ld
export OBJCOPY := $(CROSS_COMPILE)objcopy
export OBJDUMP := $(CROSS_COMPILE)objdump

CSRC  = $(wildcard *.c) 
SSRC  = $(wildcard *.S) 

OBJS := $(patsubst %.c,%.o,$(CSRC) )
OBJS += $(patsubst %.S,%.o,$(SSRC) )

export CFLAGS := -fno-builtin -fno-common
CFLAGS += -Wall -D__KERNEL__ -DTEXT_BASE=$(TEXT_BASE) 
CFLAGS += -I$(PWD)/include/

CFLAGS += -march=armv7-a
CFLAGS += -mno-unaligned-access

CFLAGS += -c

CFLAGS += -O2

TARGET = ddr_init

LDSCRIPT := linker.lds
LDFLAGS := -Bstatic -T $(LDSCRIPT) -Ttext $(TEXT_BASE)
LDFLAGS += drv/libdrv.a

.SILENT:

default: 
	make -C drv/
	make -C boot/
	sed -e 's/RAM_BASE/$(RAM_BASE)/' \
		$(LDSCRIPT).mk > $(LDSCRIPT)

	echo "  LD    $(OBJS) -Map $(TARGET).map -o $(TARGET)"
	$(LD) $(OBJS) $(LDFLAGS) -Map $(TARGET).map -o $(TARGET)
	$(OBJCOPY) -O binary $(TARGET) $(TARGET).bin
	
	mkdir -p ./out
	$(OBJDUMP) -D $(TARGET) > deasm.s
	cp $(TARGET).map ./out
	mv deasm.s ./out

clean:
	echo "  rm    boot/*.o"
	make -C boot/ clean
	echo "  rm    drv/*.o libdrv.a"
	make -C drv/ clean
	echo "  rm    $(TARGET) $(TARGET).bin* ./out"
	rm -rf $(TARGET) $(TARGET).map $(TARGET).bin* ./out

