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

#include "tcuOhosPlatform.hpp"

#include "deUniquePtr.hpp"


#include "egluGLContextFactory.hpp"
#include "display/tcuOhosEglDisplayFactory.hpp"
#include "context/tcuOhosEglContextFactory.hpp"

using de::MovePtr;
using de::UniquePtr;

using namespace tcu;
using namespace OHOS_ROSEN;

OhosPlatform::OhosPlatform (void)
{
    printf("OhosPlatform construct!\n");
    m_nativeDisplayFactoryRegistry.registerFactory(new OHOS_ROSEN::egl::OhosDisplayFactory());
    m_contextFactoryRegistry.registerFactory(new OHOS_ROSEN::egl::OhosContextFactory());
}

tcu::Platform* createOhosPlatform (void)
{
    return new tcu::OHOS_ROSEN::OhosPlatform();
}
