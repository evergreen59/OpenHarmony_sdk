#!/bin/bash
# Copyright 2021 Huawei Technologies Co., Ltd
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
# ============================================================================

ARCH=$1
OHOS_TOOLCHAIN_FILE=$2
BASEPATH=$(cd "$(dirname $0)" || exit; pwd)

MODEL_DOWNLOAD_URL="https://download.mindspore.cn/model_zoo/official/lite/quick_start/mobilenetv2.ms"

mkdir -p build
mkdir -p model
if [ ! -e ${BASEPATH}/model/mobilenetv2.ms ]; then
    wget -c -O ${BASEPATH}/model/mobilenetv2.ms --no-check-certificate ${MODEL_DOWNLOAD_URL}
fi

cd ${BASEPATH}/build || exit

CMakeToolChainFile=$OHOS_TOOLCHAIN_FILE

if [ $ARCH == "arm32" ];then
  cmake ${BASEPATH} -G "Kate - Ninja" -DCMAKE_TOOLCHAIN_FILE=$CMakeToolChainFile -DOHOS_ARCH=armeabi-v7a #32
elif [ $ARCH == "arm64" ];then
  cmake ${BASEPATH} -G "Kate - Ninja" -DCMAKE_TOOLCHAIN_FILE=$CMakeToolChainFile -DOHOS_ARCH=arm64-v8a #64
else
  echo "Error arch, the first input must be 'arm32' or 'arm64' "
  exit
fi
ninja
