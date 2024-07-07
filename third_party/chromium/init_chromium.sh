#!/bin/bash
# Copyright (c) 2022 Huawei Device Co., Ltd.
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

set -e

# 下载google工具集depot_tools
git clone https://chromium.googlesource.com/chromium/tools/depot_tools.git

# 配置环境变量
export CR_BRANCH="99.0.4844.88"
export PATH="$PATH:${PWD}/depot_tools"

# 下载代码
fetch --nohooks android
cd src
git fetch origin $CR_BRANCH
git checkout -b $CR_BRANCH FETCH_HEAD
gclient sync --force --nohooks --with_branch_heads -D -v

# 下载依赖
./build/install-build-deps-android.sh
gclient runhooks -v

# 使用Openharmony nweb的patch
git apply --whitespace=nowarn --ignore-whitespace -p2 ../patch/*.patch

