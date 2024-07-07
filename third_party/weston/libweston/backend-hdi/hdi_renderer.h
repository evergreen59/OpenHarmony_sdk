/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial
 * portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT.  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef LIBWESTON_BACKEND_HDI_HDI_RENDERER_H
#define LIBWESTON_BACKEND_HDI_HDI_RENDERER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "buffer_handle.h"

struct weston_compositor;
struct weston_output;
struct hdi_surface_state;

int
hdi_renderer_init(struct weston_compositor *compositor);

struct hdi_renderer_output_options {
    int unused;
};

int
hdi_renderer_output_create(struct weston_output *output,
    const struct hdi_renderer_output_options *options);

void
hdi_renderer_output_destroy(struct weston_output *output);

void
hdi_renderer_output_set_gpu_buffer(struct weston_output *output, BufferHandle *buffer);

#ifdef __cplusplus
}
#endif

#endif // LIBWESTON_BACKEND_HDI_HDI_RENDERER_H
