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

#ifndef _ROSEN_CONTEXT_IMPL_H_
#define _ROSEN_CONTEXT_IMPL_H_

#include "ohos_context_i.h"

#include "ui/rs_display_node.h"
#include "ui/rs_surface_node.h"

#include "EGL/egl.h"
#include "EGL/eglext.h"
#include "GLES3/gl32.h"

namespace OHOS {
namespace Rosen {

class RosenContextImpl : public OHOS::OhosContextI {
public:
    RosenContextImpl();

    bool SetConfig(int32_t w,int32_t h,RCI_GLES_VERSION ver,RCI_PIXEL_FORMAT pf,RCI_SURFACE_TYPE st,RCI_PROFILE tp,RCI_CONTEXT_FLAG flags) override;
    bool InitNativeWindow() override;
    bool InitEglSurface() override;
    bool InitEglContext() override;

    void MakeCurrent() override;
    void SwapBuffer() override;

    int32_t GetAttrib(int32_t attrType) override;
private:
    void InitProducer();
    bool InitEgl();
    void ShowConfig(EGLConfig cfg);

    RCI_GLES_VERSION glesVersion_;
    RCI_PROFILE typeProfile_;
    RCI_CONTEXT_FLAG contextFlags_;
    RCI_SURFACE_TYPE surfaceType_;
    int32_t width_;
    int32_t height_;
    RCI_PIXEL_FORMAT pixelFormat_;
    EGLConfig *allConfigs_;
    EGLint configCount_;

    RSDisplayNode::SharedPtr displayNode_;
    std::shared_ptr<RSSurfaceNode> surfaceNode_;
    sptr<Surface> producer_;

    EGLDisplay eglDisplay_;

    EGLConfig config_;

    struct NativeWindow* nativeWindow_ = nullptr;

    EGLSurface eglSurface_ = EGL_NO_SURFACE;

    EGLContext eglContext_ = EGL_NO_CONTEXT;
};

}
}

#endif