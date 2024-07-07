struct surface_info {
    int surface_id;
    int dst_x;
    int dst_y;
    int dst_w;
    int dst_h;
    int src_x;
    int src_y;
    int src_w;
    int src_h;
    double opacity;
    int visibility; // 0 or 1
    int on_layer_id;
};

struct layer_info {
    int layer_id;
    int dst_x;
    int dst_y;
    int dst_w;
    int dst_h;
    int src_x;
    int src_y;
    int src_w;
    int src_h;
    double opacity;
    int visibility; // 0 or 1
    int on_screen_id;
    int nsurfaces;
    struct surface_info **surfaces;
};

struct screen_info {
    int screen_id;
    char *connector_name;
    int width;
    int height;
    int nlayers;
    struct layer_info **layers;
};

// end of null
struct screen_info **get_screens_info();
void free_screens_info(struct screen_info **screens);

typedef void (*screen_listener_t)();
void set_screen_listener(screen_listener_t listener);
