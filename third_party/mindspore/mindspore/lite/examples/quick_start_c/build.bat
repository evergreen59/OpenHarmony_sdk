@rem Copyright 2021 Huawei Technologies Co., Ltd
@rem
@rem Licensed under the Apache License, Version 2.0 (the "License");
@rem you may not use this file except in compliance with the License.
@rem You may obtain a copy of the License at
@rem
@rem http://www.apache.org/licenses/LICENSE-2.0
@rem
@rem Unless required by applicable law or agreed to in writing, software
@rem distributed under the License is distributed on an "AS IS" BASIS,
@rem WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
@rem See the License for the specific language governing permissions and
@rem limitations under the License.
@rem ============================================================================
@echo off
@title mindspore_lite_quick_start_c_demo_build

set BASEPATH=%CD%

set ARCH=%1
set OHOS_TOOLCHAIN_FILE=%2

IF NOT EXIST "%BASEPATH%/build" (
    md build
)

set CMAKETOOLCHAINFILE=%OHOS_TOOLCHAIN_FILE%

cd %BASEPATH%/build
set CMAKEARGS=-G "Kate - Ninja" -DCMAKE_TOOLCHAIN_FILE=%CMAKETOOLCHAINFILE% 
if  %ARCH% == arm32 (
    set CMAKEARGS=%CMAKEARGS% -DCMAKE_TOOLCHAIN_FILE=%CMAKETOOLCHAINFILE% -DOHOS_ARCH=armeabi-v7a
) else if %ARCH% == arm64 (
    set CMAKEARGS=%CMAKEARGS% -DCMAKE_TOOLCHAIN_FILE=%CMAKETOOLCHAINFILE% -DOHOS_ARCH=arm64-v8a
) else (
    echo "Error arch, the first input must be 'arm32' or 'arm64' "
    exit
)
cmake %CMAKEARGS% %BASEPATH%
ninja
