#include <stdio.h>
#include <string.h>
#include <wayland-client.h>
#include <screen-info-client-protocol.h>

struct display {
    struct wl_display *display;
    struct wl_registry *registry;
    struct screen_info_module* lm;
};

static int running = 1;

void lminfo(void *data, struct screen_info_module *lm, uint32_t number)
{
    printf("lminfo: %d\n", number);
    running = 0;
}

struct screen_info_module_listener lmlistener = {lminfo};

static void
registry_handle_global(void *data, struct wl_registry *registry,
               uint32_t id, const char *interface, uint32_t version)
{
    struct display *d = data;

    if (strcmp(interface, "screen_info_module") == 0) {
        d->lm = wl_registry_bind(registry, id, &screen_info_module_interface, 1);
        screen_info_module_add_listener(d->lm, &lmlistener, d);
        screen_info_module_get(d->lm);
    }
}

static void
registry_handle_global_remove(void *a, struct wl_registry *b, uint32_t c)
{
}

static const struct wl_registry_listener registry_listener = {
    registry_handle_global,
    registry_handle_global_remove
};

int
main(int argc, char **argv)
{
    int ret = 0;
    struct display d;

    d.display = wl_display_connect(NULL);
    if (!d.display) {
        printf("wl_display_connect failed\n");
        return 1;
    }

    d.registry = wl_display_get_registry(d.display);
    wl_registry_add_listener(d.registry, &registry_listener, &d);
    wl_display_roundtrip(d.display);
    wl_display_roundtrip(d.display);

    while (running && ret != -1)
        ret = wl_display_dispatch(d.display);

    if (d.lm)
        screen_info_module_destroy(d.lm);

    wl_registry_destroy(d.registry);
    wl_display_flush(d.display);
    wl_display_disconnect(d.display);
    return 0;
}
