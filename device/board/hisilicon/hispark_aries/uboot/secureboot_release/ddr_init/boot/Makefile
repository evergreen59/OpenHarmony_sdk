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

CSRC  = $(wildcard *.c) 
SSRC  = $(wildcard *.S) 

CFLAGS += -nostdinc
SFLAGS += $(CFLAGS)

CSRC += $(SOC)/lowlevel_init_v300.c

#.SILENT:

default: 
	echo "  CC    $(CSRC)"
	echo "$(CC) $(CFLAGS) $(CSRC)"
	$(CC) $(CFLAGS) $(CSRC) -I ../drv
	echo "  CC    $(SSRC)"
	$(CC) $(SFLAGS) $(SSRC)

clean:
	rm *.o *.a -f
