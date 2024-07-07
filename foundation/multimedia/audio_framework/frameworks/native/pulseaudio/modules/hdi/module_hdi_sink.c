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

#include <config.h>
#include <pulsecore/log.h>
#include <pulsecore/modargs.h>
#include <pulsecore/module.h>
#include <pulsecore/sink.h>
#include <stddef.h>
#include <stdbool.h>

pa_sink *PaHdiSinkNew(pa_module *m, pa_modargs *ma, const char *driver);
void PaHdiSinkFree(pa_sink *s);

PA_MODULE_AUTHOR("OpenHarmony");
PA_MODULE_DESCRIPTION("OpenHarmony HDI Sink");
PA_MODULE_VERSION(PACKAGE_VERSION);
PA_MODULE_LOAD_ONCE(false);
PA_MODULE_USAGE(
        "sink_name=<name for the sink> "
        "device_class=<name for the device class> "
        "sink_properties=<properties for the sink> "
        "format=<sample format> "
        "rate=<sample rate> "
        "channels=<number of channels> "
        "channel_map=<channel map> "
        "buffer_size=<custom buffer size>"
        "file_path=<file path for data writing>"
        "adapter_name=<primary>"
        "fixed_latency=<latency measure>"
        "sink_latency=<hdi latency>"
        "render_in_idle_state<renderer state>"
        "open_mic_speaker<open mic and speaker>"
        "test_mode_on<is test mode on>"
        "network_id<device network id>"
        "device_type<device type or port>"
        );

static const char * const VALID_MODARGS[] = {
    "sink_name",
    "device_class",
    "sink_properties",
    "format",
    "rate",
    "channels",
    "channel_map",
    "buffer_size",
    "file_path",
    "adapter_name",
    "fixed_latency",
    "sink_latency",
    "render_in_idle_state",
    "open_mic_speaker",
    "test_mode_on",
    "network_id",
    "device_type",
    NULL
};

int pa__init(pa_module *m)
{
    pa_modargs *ma = NULL;

    pa_assert(m);

    if (!(ma = pa_modargs_new(m->argument, VALID_MODARGS))) {
        pa_log("Failed to parse module arguments");
        goto fail;
    }

    if (!(m->userdata = PaHdiSinkNew(m, ma, __FILE__))) {
        goto fail;
    }

    pa_modargs_free(ma);

    return 0;

fail:
    if (ma) {
        pa_modargs_free(ma);
    }

    pa__done(m);

    return -1;
}

int pa__get_n_used(pa_module *m)
{
    pa_sink *sink = NULL;

    pa_assert(m);
    pa_assert_se(sink = m->userdata);

    return pa_sink_linked_by(sink);
}

void pa__done(pa_module *m)
{
    pa_sink *sink = NULL;

    pa_assert(m);

    if ((sink = m->userdata)) {
        PaHdiSinkFree(sink);
    }
}
