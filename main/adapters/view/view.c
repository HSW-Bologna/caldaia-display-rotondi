#include <stdlib.h>
#include "page_manager.h"
#include "config/app_config.h"
#include "view.h"
#include "theme/style.h"
#include "theme/theme.h"
#include "esp_log.h"
#include "watcher.h"
#include "services/timestamp.h"


static const char   *TAG     = "View";
static pman_t        pman    = {0};
static lv_display_t *display = NULL;
static watcher_t     watcher = {0};


void view_init(model_t *p_model, pman_user_msg_cb_t controller_cb, lv_display_flush_cb_t flush_cb,
               lv_indev_read_cb_t read_cb) {
    (void)TAG;
    lv_init();

    WATCHER_INIT_STD(&watcher, NULL);

#ifdef BUILD_CONFIG_SIMULATED_APP
    (void)flush_cb;
    (void)read_cb;

    display =
        lv_sdl_window_create(BUILD_CONFIG_DISPLAY_HORIZONTAL_RESOLUTION, BUILD_CONFIG_DISPLAY_VERTICAL_RESOLUTION);
    lv_indev_t *touch_indev = lv_sdl_mouse_create();

#else

    display = lv_display_create(BUILD_CONFIG_DISPLAY_HORIZONTAL_RESOLUTION, BUILD_CONFIG_DISPLAY_VERTICAL_RESOLUTION);

    /*Static or global buffer(s). The second buffer is optional*/
    static lv_color_t buf_1[VIEW_LVGL_BUFFER_SIZE] = {0};

    /*Initialize `disp_buf` with the buffer(s). With only one buffer use NULL instead buf_2 */
    lv_display_set_buffers(display, buf_1, NULL, VIEW_LVGL_BUFFER_SIZE, LV_DISPLAY_RENDER_MODE_PARTIAL);
    lv_display_set_flush_cb(display, flush_cb);

    style_init();
    theme_init(display);

    lv_indev_t *touch_indev = lv_indev_create();
    lv_indev_set_type(touch_indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(touch_indev, read_cb);

#endif

    pman_init(&pman, (void *)p_model, touch_indev, controller_cb, NULL);
}


void view_manage(void) {
    watcher_watch(&watcher, timestamp_get());
}


void view_event(view_event_t event) {
    pman_event(&pman, (pman_event_t){.tag = PMAN_EVENT_TAG_USER, .as = {.user = &event}});
}


void view_change_page(const pman_page_t *page) {
    pman_change_page(&pman, *page);
}


mut_model_t *view_get_model(void *handle) {
    return pman_get_user_data(handle);
}


void view_display_flush_ready(void) {
    if (display) {
        lv_display_flush_ready(display);
    }
}


void view_register_object_default_callback(lv_obj_t *obj, int id) {
    view_register_object_default_callback_with_number(obj, id, 0);
}


void view_register_object_default_callback_with_number(lv_obj_t *obj, int id, int number) {
    view_obj_data_t *data = lv_malloc(sizeof(view_obj_data_t));
    assert(data != NULL);
    data->id     = id;
    data->number = number;

    lv_obj_set_user_data(obj, data);
    pman_register_obj_event(&pman, obj, LV_EVENT_PRESSED);
    pman_register_obj_event(&pman, obj, LV_EVENT_CLICKED);
    pman_register_obj_event(&pman, obj, LV_EVENT_VALUE_CHANGED);
    pman_register_obj_event(&pman, obj, LV_EVENT_LONG_PRESSED);
    pman_register_obj_event(&pman, obj, LV_EVENT_LONG_PRESSED_REPEAT);
    pman_register_obj_event(&pman, obj, LV_EVENT_RELEASED);
    pman_set_obj_self_destruct(obj);
}


static void view_watcher_cb(void *old_value, const void *memory, uint16_t size, void *user_ptr, void *arg) {
    (void)old_value;
    (void)memory;
    (void)size;
    (void)user_ptr;
    view_event((view_event_t){.tag = VIEW_EVENT_TAG_PAGE_WATCHER, .as.page_watcher.code = (uintptr_t)arg});
}


void view_clear_watcher(void) {
    watcher_destroy(&watcher);
    WATCHER_INIT_STD(&watcher, NULL);
}


void view_add_watched_variable(void *ptr, size_t size, int code) {
    watcher_add_entry(&watcher, ptr, size, view_watcher_cb, (void *)(uintptr_t)code);
}
