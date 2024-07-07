/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "ast/ast_string_type.h"

namespace OHOS {
namespace Idl {
String ASTStringType::GetSignature()
{
    return "T";
}

bool ASTStringType::IsStringType()
{
    return true;
}

String ASTStringType::ToString()
{
    return "String";
}
} // namespace Idl
} // namespace OHOS
