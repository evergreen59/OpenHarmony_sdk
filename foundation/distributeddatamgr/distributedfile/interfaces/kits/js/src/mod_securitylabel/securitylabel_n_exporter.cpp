/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "securitylabel_n_exporter.h"

#include <tuple>

#include "../common/napi/n_class.h"
#include "../common/napi/n_func_arg.h"
#include "../common/napi/n_val.h"
#include "../common/uni_error.h"
#include "n_async_work_callback.h"
#include "n_async_work_promise.h"
#include "security_label.h"


#include <singleton.h>


namespace OHOS {
namespace DistributedFS {
namespace ModuleSecurityLabel {
using namespace std;

napi_value SetSecurityLabel(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(static_cast<int>(NARG_CNT::TWO), static_cast<int>(NARG_CNT::THREE))) {
        UniError(EINVAL).ThrowErr(env, "Number of arguments unmatched");
        return nullptr;
    }

    bool succ = false;
    std::unique_ptr<char []> path;
    std::unique_ptr<char []> dataLevel;
    tie(succ, path, std::ignore) = NVal(env, funcArg[static_cast<int>(NARG_POS::FIRST)]).ToUTF8String();
    if (!succ) {
        UniError(EINVAL).ThrowErr(env, "Invalid path");
        return nullptr;
    }
    tie(succ, dataLevel, std::ignore) = NVal(env, funcArg[static_cast<int>(NARG_POS::SECOND)]).ToUTF8String();
    if (!succ) {
        UniError(EINVAL).ThrowErr(env, "Invalid dataLevel");
        return nullptr;
    }
    std::string pathString(path.get());
    std::string dataLevelString(dataLevel.get());
    if (DATA_LEVEL.find(dataLevelString) == DATA_LEVEL.end()) {
        UniError(EINVAL).ThrowErr(env, "Invalid Argument of dataLevelEnum");
        return nullptr;
    }
    auto cbExec = [pathString, dataLevelString](napi_env env) -> UniError {
        bool ret = SecurityLabel::SetSecurityLabel(pathString, dataLevelString);
        if (!ret) {
            return UniError(-1);
        } else {
            return UniError(ERRNO_NOERR);
        }
    };
    auto cbComplete = [](napi_env env, UniError err) -> NVal {
        if (err) {
            return { env, err.GetNapiErr(env) };
        } else {
            return NVal::CreateUndefined(env);
        }
    };
    std::string procedureName = "SetSecurityLabel";
    NVal thisVar(env, funcArg.GetThisVar());
    if (funcArg.GetArgc() == static_cast<size_t>(NARG_CNT::TWO)) {
        return NAsyncWorkPromise(env, thisVar).Schedule(procedureName, cbExec, cbComplete).val_;
    } else {
        NVal cb(env, funcArg[static_cast<int>(NARG_POS::THIRD)]);
        if (cb.TypeIs(napi_function)) {
            return NAsyncWorkCallback(env, thisVar, cb).Schedule(procedureName, cbExec, cbComplete).val_;
        }
    }
    return NVal::CreateUndefined(env).val_;
}

napi_value SetSecurityLabelSync(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(static_cast<int>(NARG_CNT::TWO), static_cast<int>(NARG_CNT::THREE))) {
        UniError(EINVAL).ThrowErr(env, "Number of arguments unmatched");
        return nullptr;
    }

    bool succ = false;
    std::unique_ptr<char []> path;
    std::unique_ptr<char []> dataLevel;
    tie(succ, path, std::ignore) = NVal(env, funcArg[static_cast<int>(NARG_POS::FIRST)]).ToUTF8String();
    if (!succ) {
        UniError(EINVAL).ThrowErr(env, "Invalid path");
        return nullptr;
    }
    tie(succ, dataLevel, std::ignore) = NVal(env, funcArg[static_cast<int>(NARG_POS::SECOND)]).ToUTF8String();
    if (!succ) {
        UniError(EINVAL).ThrowErr(env, "Invalid dataLevel");
        return nullptr;
    }
    if (DATA_LEVEL.find(dataLevel.get()) == DATA_LEVEL.end()) {
        UniError(EINVAL).ThrowErr(env, "Invalid Argument of dataLevelEnum");
        return nullptr;
    }
    bool ret = SecurityLabel::SetSecurityLabel(path.get(), dataLevel.get());
    if (!ret) {
        return UniError(-1).GetNapiErr(env);
    } else {
        return NVal::CreateUndefined(env).val_;
    }
}

napi_value GetSecurityLabel(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(static_cast<int>(NARG_CNT::ONE), static_cast<int>(NARG_CNT::TWO))) {
        UniError(EINVAL).ThrowErr(env, "Number of arguments unmatched");
        return nullptr;
    }

    bool succ = false;
    std::unique_ptr<char []> path;
    tie(succ, path, std::ignore) = NVal(env, funcArg[static_cast<int>(NARG_POS::FIRST)]).ToUTF8String();
    if (!succ) {
        UniError(EINVAL).ThrowErr(env, "Invalid path");
        return nullptr;
    }
    auto result = std::make_shared<string>();
    std::string pathString(path.get());
    auto cbExec = [pathString, result](napi_env env) -> UniError {
        *result = SecurityLabel::GetSecurityLabel(pathString);
        return UniError(ERRNO_NOERR);
    };
    auto cbComplete = [result](napi_env env, UniError err) -> NVal {
        if (err) {
            return { env, err.GetNapiErr(env) };
        }
        return { NVal::CreateUTF8String(env, *result) };
    };

    std::string procedureName = "GetSecurityLabel";
    NVal thisVar(env, funcArg.GetThisVar());
    if (funcArg.GetArgc() == static_cast<size_t>(NARG_CNT::ONE)) {
        return NAsyncWorkPromise(env, thisVar).Schedule(procedureName, cbExec, cbComplete).val_;
    } else {
        NVal cb(env, funcArg[static_cast<int>(NARG_POS::SECOND)]);
        if (cb.TypeIs(napi_function)) {
            return NAsyncWorkCallback(env, thisVar, cb).Schedule(procedureName, cbExec, cbComplete).val_;
        }
    }
    return NVal::CreateUndefined(env).val_;
}

napi_value GetSecurityLabelSync(napi_env env, napi_callback_info info)
{
    NFuncArg funcArg(env, info);
    if (!funcArg.InitArgs(static_cast<int>(NARG_CNT::ONE), static_cast<int>(NARG_CNT::TWO))) {
        UniError(EINVAL).ThrowErr(env, "Number of arguments unmatched");
        return nullptr;
    }

    bool succ = false;
    std::unique_ptr<char []> path;
    tie(succ, path, std::ignore) = NVal(env, funcArg[static_cast<int>(NARG_POS::FIRST)]).ToUTF8String();
    if (!succ) {
        UniError(EINVAL).ThrowErr(env, "Invalid path");
        return nullptr;
    }
    auto result = std::make_shared<string>();
    *result = SecurityLabel::GetSecurityLabel(path.get());
    return NVal::CreateUTF8String(env, *result).val_;
}
} // namespace ModuleSecurityLabel
} // namespace DistributedFS
} // namespace OHOS