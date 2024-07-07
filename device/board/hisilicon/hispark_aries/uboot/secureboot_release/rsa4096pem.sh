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

#! /bin/sh


echo "======================================================================================"
echo "|                                     rsa4096pem                                      |"
echo "======================================================================================"
# generate rsa pub key and private key
if [ -f rsa4096pem/rsa_pub_4096_sha256.txt ]; then
	echo "use old rsa4096pem/rsa_pub.bin rsa4096pem/rsa_pub_4096_sha256.txt"
	./HASH rsa4096pem/rsa_pub_4096_sha256.txt
else
	echo "gen new rsa4096pem/rsa_pub.bin rsa4096pem/rsa_pub_4096_sha256.txt"
	openssl genrsa -out rsa4096pem/rsa_priv_4096.pem  4096
	openssl rsa -in rsa4096pem/rsa_priv_4096.pem -pubout -out rsa4096pem/rsa_pub_4096.pem
fi
