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

#ifndef CAMERA_MANAGER_CALLBACK_NAPI_H_
#define CAMERA_MANAGER_CALLBACK_NAPI_H_

#include "camera_log.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"

#include "hilog/log.h"
#include "camera_napi_utils.h"

#include "input/camera_manager.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>

namespace OHOS {
namespace CameraStandard {
class CameraManagerCallbackNapi : public  CameraManagerCallback {
public:
    explicit CameraManagerCallbackNapi(napi_env env, napi_ref callbackRef_);
    virtual ~CameraManagerCallbackNapi();
    void OnCameraStatusChanged(const CameraStatusInfo &cameraStatusInfo) const override;
    void OnFlashlightStatusChanged(const std::string &cameraID, const FlashStatus flashStatus) const override;

private:
    void OnCameraStatusCallback(const CameraStatusInfo &cameraStatusInfo) const;
    void OnCameraStatusCallbackAsync(const CameraStatusInfo &cameraStatusInfo) const;

    napi_env env_ = nullptr;
    napi_ref callbackRef_;
};

struct CameraStatusCallbackInfo {
    CameraStatusInfo info_;
    const CameraManagerCallbackNapi* listener_;
    CameraStatusCallbackInfo(CameraStatusInfo info, const CameraManagerCallbackNapi* listener)
        : info_(info), listener_(listener) {}
    ~CameraStatusCallbackInfo()
    {
        listener_ = nullptr;
    }
};
} // namespace CameraStandard
} // namespace OHOS
#endif /* CAMERA_MANAGER_CALLBACK_NAPI_H_ */
