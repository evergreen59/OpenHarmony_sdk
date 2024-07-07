#! /bin/bash

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

input_file=$1
output_file=$2

if [ -f $input_file ] && [ -f $output_file ];then
    input_file_md5=$(md5sum ${input_file} | awk '{print $1}')
    output_file_md5=$(md5sum ${output_file} | awk '{print $1}')
    if [ "${input_file_md5}"x == "${output_file_md5}"x ];then
        echo "ok" 
    fi
fi
