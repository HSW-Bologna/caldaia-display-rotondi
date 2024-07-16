#ifndef VIEW_H_INCLUDED
#define VIEW_H_INCLUDED


#include "model/model.h"
#include "page_manager.h"


#define VIEW_LVGL_BUFFER_SIZE                (BUILD_CONFIG_DISPLAY_HORIZONTAL_RESOLUTION * 50)
#define VIEW_ADD_WATCHED_VARIABLE(ptr, code) view_add_watched_variable((void *)ptr, sizeof(*ptr), code)


typedef struct {
    int16_t id;
    int16_t number;
} view_obj_data_t;


typedef enum {
    VIEW_EVENT_TAG_PAGE_WATCHER,
} view_event_tag_t;


typedef struct {
    view_event_tag_t tag;
    union {
        struct {
            int code;
        } page_watcher;
    } as;
} view_event_t;


typedef enum {
    VIEW_CONTROLLER_MESSAGE_TAG_NOTHING = 0,
} view_controller_msg_tag_t;


typedef struct {
    view_controller_msg_tag_t tag;
    union {
    } as;
} view_controller_msg_t;


void view_init(model_t *p_model, pman_user_msg_cb_t controller_cb, lv_display_flush_cb_t flush_cb,
               lv_indev_read_cb_t read_cb);

void         view_change_page(const pman_page_t *page);
void         view_display_flush_ready(void);
mut_model_t *view_get_model(void *handle);
void         view_register_object_default_callback_with_number(lv_obj_t *obj, int id, int number);
void         view_register_object_default_callback(lv_obj_t *obj, int id);
void         view_manage(void);
void         view_clear_watcher(void);
void         view_add_watched_variable(void *ptr, size_t size, int code);


extern const pman_page_t page_main;


#endif
