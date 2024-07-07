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

.PHONY:all
#export MACH

BIT_WIDTH:=$(shell getconf LONG_BIT)

ifeq ($(BIT_WIDTH),64)
MACH:=m64
else
MACH:=m32
endif

all: ddrinit rsa2048pem_gen rsa4096pem_gen
ddrinit:
	pushd ddr_init;make;./mkddrinit.sh u-boot-original.bin;popd
	cp ./ddr_init/ddr_init_reg_info.bin ./

rsa2048pem_gen: ddrinit
	./rsa2048pem.sh
	sh create_secure_boot.sh rsa2048pem_gen

rsa4096pem_gen: ddrinit
	./rsa4096pem.sh
	sh create_secure_boot.sh rsa4096pem_gen

clean:
	pushd ddr_init;make clean;popd
	find . -name "*.bin" -exec rm -rf {} \;
distclean:clean
	rm rsa2048pem/*
	rm rsa4096pem/*
	rm -rf aes_otp_cfg.txt
