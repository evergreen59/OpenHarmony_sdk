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

#ifndef _IVI_INPUT_EXPORT_H_
#define _IVI_INPUT_EXPORT_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stdint.h>
#include <limits.h>

#include "stdbool.h"
#include <libweston/libweston.h>
#include <libweston/plugin-registry.h>

#define IVI_INPUT_API_NAME_FOR_WMS "ivi_input_api_for_wms"

#define INPUT_DEVICE_KEYBOARD   ((unsigned int) 1 << 0)
#define INPUT_DEVICE_POINTER    ((unsigned int) 1 << 1)
#define INPUT_DEVICE_TOUCH      ((unsigned int) 1 << 2)
#define INPUT_DEVICE_ALL        ((unsigned int) ~0)

struct ivi_input_interface_for_wms {
    void (*set_focus)(uint32_t surface, uint32_t device, int32_t enabled);
};

static inline const struct ivi_input_interface_for_wms *
ivi_input_get_api_for_wms(struct weston_compositor *compositor)
{
    const void *api;
    api = weston_plugin_api_get(compositor, IVI_INPUT_API_NAME_FOR_WMS,
                    sizeof(struct ivi_input_interface_for_wms));

    return (const struct ivi_input_interface_for_wms *)api;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _IVI_INPUT_EXPORT_H_ */
