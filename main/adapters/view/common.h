#ifndef VIEW_COMMON_H_INCLUDED
#define VIEW_COMMON_H_INCLUDED


#include <stdint.h>
#include "lvgl.h"
#include "view.h"


typedef struct {
    const char      *password;
    pman_stack_msg_t msg;
} password_page_options_t;


void                     view_common_set_hidden(lv_obj_t *obj, uint8_t hidden);
password_page_options_t *view_common_default_password_page_options(pman_stack_msg_t msg, const char *password);
lv_obj_t *view_common_create_title(lv_obj_t *root, const char *text, int back_id, int prev_id, int next_id);


#endif
