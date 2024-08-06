#include "lvgl.h"
#include "model/model.h"
#include "../view.h"
#include "../theme/style.h"
#include "../common.h"


LV_IMG_DECLARE(img_door);


enum {
    BACK_BTN_ID,
    TEST_BTN_ID,
};


static void *create_page(pman_handle_t handle, void *extra) {
    (void)handle;
    (void)extra;
    return NULL;
}


static void open_page(pman_handle_t handle, void *state) {
    (void)handle;
    (void)state;

    lv_obj_t *cont, *lbl, *btn;

    view_common_create_title(lv_scr_act(), "Impostazioni", BACK_BTN_ID, -1, -1);

    cont = lv_obj_create(lv_scr_act());
    lv_obj_set_style_pad_column(cont, 16, LV_STATE_DEFAULT);
    lv_obj_set_size(cont, LV_HOR_RES, LV_VER_RES - 64);
    lv_obj_set_layout(cont, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_align(cont, LV_ALIGN_BOTTOM_MID, 0, 0);

    btn = lv_btn_create(cont);
    lv_obj_set_size(btn, 180, 80);
    lbl = lv_label_create(btn);
    lv_label_set_text(lbl, "Test");
    lv_obj_set_style_text_font(lbl, STYLE_FONT_MEDIUM, LV_STATE_DEFAULT);
    lv_obj_center(lbl);
    view_register_object_default_callback(btn, TEST_BTN_ID);
}


static pman_msg_t page_event(pman_handle_t handle, void *state, pman_event_t event) {
    (void)handle;
    (void)state;

    pman_msg_t msg = PMAN_MSG_NULL;

    switch (event.tag) {
        case PMAN_EVENT_TAG_OPEN:
            break;

        case PMAN_EVENT_TAG_LVGL: {
            lv_obj_t        *target   = lv_event_get_current_target_obj(event.as.lvgl);
            view_obj_data_t *obj_data = lv_obj_get_user_data(target);

            switch (lv_event_get_code(event.as.lvgl)) {
                case LV_EVENT_CLICKED: {
                    switch (obj_data->id) {
                        case BACK_BTN_ID:
                            msg.stack_msg = PMAN_STACK_MSG_BACK();
                            break;

                        case TEST_BTN_ID:
                            msg.stack_msg = PMAN_STACK_MSG_PUSH_PAGE(&page_test_phase_cut);
                            break;
                    }
                    break;
                }

                default:
                    break;
            }
            break;
        }

        default:
            break;
    }

    return msg;
}


const pman_page_t page_menu = {
    .create        = create_page,
    .destroy       = pman_destroy_all,
    .open          = open_page,
    .close         = pman_close_all,
    .process_event = page_event,
};
