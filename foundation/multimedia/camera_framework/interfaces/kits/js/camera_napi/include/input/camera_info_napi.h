/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef CAMERA_INFO_NAPI_H_
#define CAMERA_INFO_NAPI_H_

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include "display_type.h"
#include "camera_log.h"
#include "camera_napi_utils.h"
#include "input/camera_manager.h"
#include "input/camera_device.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "output/capture_output.h"
#include "hilog/log.h"
namespace OHOS {
namespace CameraStandard {
static const char CAMERA_OBJECT_NAPI_CLASS_NAME[] = "Camera";

class CameraDeviceNapi {
public:
    static napi_value Init(napi_env env, napi_value exports);
    static napi_value CreateCameraObj(napi_env env, sptr<CameraDevice> cameraInfo);
    CameraDeviceNapi();
    ~CameraDeviceNapi();
    sptr<CameraDevice> cameraDevice_;

private:
    static void CameraDeviceNapiDestructor(napi_env env, void* nativeObject, void* finalize_hint);
    static napi_value CameraDeviceNapiConstructor(napi_env env, napi_callback_info info);

    static napi_value GetCameraId(napi_env env, napi_callback_info info);
    static napi_value GetCameraPosition(napi_env env, napi_callback_info info);
    static napi_value GetCameraType(napi_env env, napi_callback_info info);
    static napi_value GetConnectionType(napi_env env, napi_callback_info info);

    napi_env env_;
    napi_ref wrapper_;

    static thread_local napi_ref sConstructor_;
    static thread_local sptr<CameraDevice> sCameraDevice_;
};
} // namespace CameraStandard
} // namespace OHOS
#endif /* CAMERA_INFO_NAPI_H_ */
