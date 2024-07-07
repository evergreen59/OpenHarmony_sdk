#include <stdio.h>
#include <string.h>
#include <weston.h>
#include <ivi-layout-export.h>
#include <ivi-layout-private.h>
#include "ivi-controller.h"
#include "screen-info.h"

#define LOG(fmt, ...)  weston_log("screen-info " fmt "\n", ##__VA_ARGS__)

static struct ivishell *shell;

struct iviscreen {
    struct wl_list link;
    struct ivishell *shell;
    uint32_t id_screen;
    struct weston_output *output;
    struct wl_list resource_list;
};

struct screen_info **get_screens_info()
{
    struct iviscreen *iviscrn = NULL;
    int screen_count_with_null = 1;
    wl_list_for_each(iviscrn, &shell->list_screen, link) {
        screen_count_with_null++;
    }

    struct screen_info **screens = (struct screen_info **)malloc(
            screen_count_with_null * sizeof(struct screen_info *));

    for (int i = 0; i < screen_count_with_null - 1; i++) {
        screens[i] = (struct screen_info *)malloc(sizeof(struct screen_info));
    }
    screens[screen_count_with_null - 1] = NULL;

    struct ivi_layout_layer **layer_list = NULL;
    const struct ivi_layout_interface *lyt = shell->interface;
    int layer_count, surf_cnt;

    int screen_i = 0;
    wl_list_for_each(iviscrn, &shell->list_screen, link) {
        lyt->get_layers_on_screen(
                iviscrn->output, &layer_count, &layer_list);
        struct screen_info *iscreen = screens[screen_i++];

        iscreen->screen_id = iviscrn->id_screen;
        iscreen->connector_name = (char *)malloc(
                strlen(iviscrn->output->name) + 1);
        strcpy(iscreen->connector_name, iviscrn->output->name);
        iscreen->width = iviscrn->output->width;
        iscreen->height = iviscrn->output->height;
        iscreen->nlayers = layer_count;
        iscreen->layers = (struct layer_info **)malloc(
                layer_count * sizeof(struct layer_info *));
        for (int i = 0; i < layer_count; i++) {
            iscreen->layers[i] = (struct layer_info *)malloc(
                sizeof(struct layer_info));
        }

        for (int i = 0; i < layer_count; i++) {
            struct ivi_layout_layer *layer = layer_list[i];
            struct ivi_layout_layer_properties *p = &layer->prop;
            struct ivi_layout_surface **surf_list = NULL;
            lyt->get_surfaces_on_layer(layer, &surf_cnt, &surf_list);

            struct layer_info *ilayer = iscreen->layers[i];
            ilayer->layer_id = lyt->get_id_of_layer(layer);
            ilayer->dst_x = p->dest_x;
            ilayer->dst_y = p->dest_y;
            ilayer->dst_w = p->dest_width;
            ilayer->dst_h = p->dest_height;
            ilayer->src_x = p->source_x;
            ilayer->src_y = p->source_y;
            ilayer->src_w = p->source_width;
            ilayer->src_h = p->source_height;
            ilayer->opacity = wl_fixed_to_double(p->opacity);
            ilayer->visibility = p->visibility;
            ilayer->on_screen_id = iscreen->screen_id;
            ilayer->nsurfaces = surf_cnt;
            ilayer->surfaces = (struct surface_info **)malloc(
                    surf_cnt * sizeof(struct surface_info *));
            for (int j = 0; j < surf_cnt; j++) {
                ilayer->surfaces[j] = (struct surface_info *)malloc(
                        sizeof(struct surface_info));
            }

            for (int j = 0; j < surf_cnt; j++) {
                struct ivi_layout_surface *surface = surf_list[j];
                struct ivi_layout_surface_properties *p = &surface->prop;
                struct surface_info *isurface = ilayer->surfaces[j];

                isurface->surface_id = lyt->get_id_of_surface(surface);
                isurface->dst_x = p->dest_x;
                isurface->dst_y = p->dest_y;
                isurface->dst_w = p->dest_width;
                isurface->dst_h = p->dest_height;
                isurface->src_x = p->source_x;
                isurface->src_y = p->source_y;
                isurface->src_w = p->source_width;
                isurface->src_h = p->source_height;
                isurface->opacity = wl_fixed_to_double(p->opacity);
                isurface->visibility = p->visibility;
                isurface->on_layer_id = ilayer->layer_id;
            }
        }
    }

    return screens;
}

void free_screens_info(struct screen_info **screens)
{
    for (int i = 0; screens[i]; i++) {
        struct screen_info *iscreen = screens[i];
        for (int j = 0; j < iscreen->nlayers; j++) {
            struct layer_info *ilayer = iscreen->layers[j];
            for (int k = 0; k < ilayer->nsurfaces; k++) {
                struct surface_info *isurface = ilayer->surfaces[k];
                free(isurface);
            }
            free(ilayer->surfaces);
            free(ilayer);
        }
        free(iscreen->layers);
        free(iscreen->connector_name);
        free(iscreen);
    }
    free(screens);
}

static int
load_test_screen_info_module(struct ivishell *shell)
{
    struct weston_config *config = wet_get_config(shell->compositor);
    struct weston_config_section *section;
    char *test_screen_info_module = NULL;

    int (*test_screen_info_module_init)(struct ivishell *shell);

    section = weston_config_get_section(config, "screen-info", NULL, NULL);

    if (weston_config_section_get_string(section, "test-screen-info-module",
                                         &test_screen_info_module, NULL) < 0) {
        /* input events are handled by weston's default grabs */
        weston_log("ivi-controller: No test-screen-info-module set\n");
        return 0;
    }

    test_screen_info_module_init =
        wet_load_module_entrypoint(test_screen_info_module, "ivishell_module_init");
    if (!test_screen_info_module_init)
        return -1;

    if (test_screen_info_module_init(shell) != 0) {
        weston_log("ivi-controller: Initialization of test-screen-info-module fails");
        return -1;
    }

    free(test_screen_info_module);

    return 0;
}

WL_EXPORT int
ivishell_module_init(struct ivishell *shell_)
{
    shell = shell_;

    if (load_test_screen_info_module(shell) < 0) {
        return -1;
    }

    return 0;
}
