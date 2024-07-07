#include <stdio.h>
#include <string.h>
#include <weston.h>
#include <screen-info-server-protocol.h>
#include <ivi-layout-export.h>
#include <ivi-layout-private.h>
#include "ivi-controller.h"
#include "screen-info.h"

#define LOG(fmt, ...)  weston_log("test-screen-info " fmt "\n", ##__VA_ARGS__)

void screen_info_module_get(struct wl_client* client, struct wl_resource* resource)
{
    struct screen_info **screens = get_screens_info();
    for (int i = 0; screens[i]; i++) {
        struct screen_info *iscreen = screens[i];

        LOG("screen %d (0x%x)", iscreen->screen_id, iscreen->screen_id);
        LOG("---------------------------------------");
        LOG("- connector name:       %s", iscreen->connector_name);
        LOG("- resolution:           w=%d, h=%d",
                iscreen->width, iscreen->height);
        static char buffer[256] = {};
        for (int j = 0; j < iscreen->nlayers; j++) {
            struct layer_info *ilayer = iscreen->layers[j];
            sprintf(buffer + strlen(buffer), "%d(0x%x)%s",
                    ilayer->layer_id, ilayer->layer_id,
                    i != iscreen->nlayers - 1 ? "," : "");
        }
        LOG("- layer render order:   %s", buffer);
        LOG("");

        for (int j = 0; j < iscreen->nlayers; j++) {
            struct layer_info *ilayer = iscreen->layers[j];

            LOG("    layer %d (0x%x)", ilayer->layer_id, ilayer->layer_id);
            LOG("    ---------------------------------------");
            LOG("    - destination region:   x=%d, y=%d, w=%d, h=%d",
                    ilayer->dst_x, ilayer->dst_y, ilayer->dst_w, ilayer->dst_h);
            LOG("    - source region:        x=%d, y=%d, w=%d, h=%d",
                    ilayer->src_x, ilayer->src_y, ilayer->src_w, ilayer->src_h);
            LOG("    - opacity:              %lf", ilayer->opacity);
            LOG("    - visibility:           %d", ilayer->visibility);

            static char buffer[256] = {};
            for (int k = 0; k < ilayer->nsurfaces; k++) {
                struct surface_info *isurface = ilayer->surfaces[k];
                sprintf(buffer + strlen(buffer), "%d(0x%x)%s",
                        isurface->surface_id, isurface->surface_id,
                        i != ilayer->nsurfaces - 1 ? "," : "");
            }
            LOG("    - surface render order: %s", buffer);
            LOG("    - on screen:            %d(0x%x)",
                    ilayer->on_screen_id, ilayer->on_screen_id);
            LOG("");

            for (int k = 0; k < ilayer->nsurfaces; k++) {
                struct surface_info *isurface = ilayer->surfaces[k];

                LOG("        surface %d (0x%x)",
                        isurface->surface_id, isurface->surface_id);
                LOG("        ---------------------------------------");
                LOG("        - destination region: x=%d, y=%d, w=%d, h=%d",
                    isurface->dst_x, isurface->dst_y,
                    isurface->dst_w, isurface->dst_h);
                LOG("        - source region:      x=%d, y=%d, w=%d, h=%d",
                    isurface->src_x, isurface->src_y,
                    isurface->src_w, isurface->src_h);
                LOG("        - opacity:            %lf", isurface->opacity);
                LOG("        - visibility:         %d", isurface->visibility);
                LOG("        - on layer:           %d(0x%x)",
                        isurface->on_layer_id, isurface->on_layer_id);
                LOG("");
            }
        }
    }
    free_screens_info(screens);

    // here send to client result
    screen_info_module_send_info(resource, 666);
}

// screen_info module interface implementation
static const
struct screen_info_module_interface screen_info_module_ii = {
    screen_info_module_get
};

static void
bind_screen_info_module(struct wl_client *client, void *data,
        uint32_t version, uint32_t id)
{
    struct ivishell *shell = data;
    struct wl_resource *resource;

    resource = wl_resource_create(client, &screen_info_module_interface, version, id);
    if (resource == NULL) {
        LOG("bind_screen_info_module: wl_resource_create failed");
        return;
    }

    wl_resource_set_implementation(resource, &screen_info_module_ii, shell, NULL);
}

static void
on_property_change()
{
    LOG("property changed");
}

WL_EXPORT int
ivishell_module_init(struct ivishell *shell)
{
    LOG("loading test-screen-info-module");
    if (!wl_global_create(shell->compositor->wl_display,
                &screen_info_module_interface, 1, shell, bind_screen_info_module)) {
        LOG("test-screen-info-module: wl_global_create failed");
        return 1;
    }
    LOG("loaded test-screen-info-module");

    set_screen_listener(on_property_change);
    return 0;
}
