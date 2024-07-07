/*
 * Copyright (c) 2022 Shenzhen Kaihong Digital Industry Development Co., Ltd.
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

#include <climits>
#include <gtest/gtest.h>
#include "../Khrgles3BaseFunc.h"
#include "../ActsKhrgles30002TestSuite.h"
#include "shrinkdefine.h"

using namespace std;
using namespace testing::ext;
using namespace OHOS;

static SHRINK_HWTEST_F(ActsKhrgles30002TestSuite, TestCase_001210,
        "KHR-GLES3.shaders.preproce",
        "ssor.pragmas.pragma_vertex");

static SHRINK_HWTEST_F(ActsKhrgles30002TestSuite, TestCase_001211,
        "KHR-GLES3.shaders.preproces",
        "sor.pragmas.pragma_fragment");

static SHRINK_HWTEST_F(ActsKhrgles30002TestSuite, TestCase_001212,
        "KHR-GLES3.shaders.preprocessor.",
        "pragmas.pragma_macro_exp_vertex");

static SHRINK_HWTEST_F(ActsKhrgles30002TestSuite, TestCase_001213,
        "KHR-GLES3.shaders.preprocessor.p",
        "ragmas.pragma_macro_exp_fragment");
