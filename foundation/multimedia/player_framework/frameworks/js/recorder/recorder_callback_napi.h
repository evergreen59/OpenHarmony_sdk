/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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

#ifndef RECORDER_CALLBACK_NAPI_H_
#define RECORDER_CALLBACK_NAPI_H_

#include "audio_recorder_napi.h"
#include "recorder.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "common_napi.h"

namespace OHOS {
namespace Media {
const std::string ERROR_CALLBACK_NAME = "error";
const std::string PREPARE_CALLBACK_NAME = "prepare";
const std::string START_CALLBACK_NAME = "start";
const std::string PAUSE_CALLBACK_NAME = "pause";
const std::string RESUME_CALLBACK_NAME = "resume";
const std::string STOP_CALLBACK_NAME = "stop";
const std::string RESET_CALLBACK_NAME = "reset";
const std::string RELEASE_CALLBACK_NAME = "release";
class RecorderCallbackNapi : public RecorderCallback {
public:
    explicit RecorderCallbackNapi(napi_env env, bool isVideo);
    virtual ~RecorderCallbackNapi();

    void SaveCallbackReference(const std::string &name, std::weak_ptr<AutoRef> ref);
    void ClearCallbackReference();
    void SendErrorCallback(int32_t errCode);
    void SendStateCallback(const std::string &callbackName);

protected:
    void OnError(RecorderErrorType errorType, int32_t errCode) override;
    void OnInfo(int32_t type, int32_t extra) override;

private:
    struct RecordJsCallback {
        std::weak_ptr<AutoRef> autoRef;
        std::string callbackName = "unknown";
        std::string errorMsg = "unknown";
        int32_t errorCode = MSERR_EXT_UNKNOWN;
    };
    void OnJsErrorCallBack(RecordJsCallback *jsCb) const;
    void OnJsStateCallBack(RecordJsCallback *jsCb) const;
    napi_env env_ = nullptr;
    std::mutex mutex_;
    std::map<std::string, std::weak_ptr<AutoRef>> refMap_;
    bool isVideo_ = false;
};
} // namespace Media
} // namespace OHOS
#endif // RECORDER_CALLBACK_NAPI_H_
