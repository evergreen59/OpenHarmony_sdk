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
export SOC


SOC := hi3518ev300

all:prepare ddrinit rsa2048pem_gen rsa4096pem_gen u-boot-rsa.bin aeskey2reg
#all:prepare rsa2048pem_gen rsa4096pem_gen u-boot-rsa.bin aeskey2reg
prepare:
	gcc hash_modify.c -o HASH 
	gcc aeskey2reg.c -o AES 
ddrinit:
	pushd ddr_init;make;./mkddrinit.sh u-boot-original.bin;popd
	cp ./ddr_init/ddr_init_reg_info.bin ./

rsa2048pem_gen: prepare ddrinit
#rsa2048pem_gen: prepare 
	./rsa2048pem.sh

rsa4096pem_gen: prepare ddrinit
#rsa4096pem_gen: prepare
	./rsa4096pem.sh

u-boot-rsa.bin: prepare ddrinit rsa2048pem_gen rsa4096pem_gen
#u-boot-rsa.bin: prepare rsa2048pem_gen rsa4096pem_gen
	./create_secure_boot.sh

#aeskey2reg:
	./AES ./create_secure_boot.sh > aes_otp_cfg.txt
clean:
	pushd ddr_init;make clean;popd
	find . -name "*.bin" -exec rm -rf {} \;
distclean:clean
	rm rsa2048pem/*
	rm rsa4096pem/*
	rm -rf aes_otp_cfg.txt
