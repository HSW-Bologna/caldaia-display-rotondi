#include "lvgl.h"
#include "model/model.h"
#include "../view.h"
#include "../theme/style.h"
#include "../common.h"
#include "config/app_config.h"


LV_IMG_DECLARE(img_connection);


struct page_data {
    lv_obj_t *img_alarm;
    lv_obj_t *lbl_machine_version;
};


enum {
    BACK_BTN_ID,
    TEST_BTN_ID,
};


static void update_page(model_t *model, struct page_data *pdata);


static void *create_page(pman_handle_t handle, void *extra) {
    (void)handle;
    (void)extra;

    struct page_data *pdata = lv_malloc(sizeof(struct page_data));
    assert(pdata != NULL);

    return pdata;
}


static void open_page(pman_handle_t handle, void *state) {
    struct page_data *pdata = state;

    model_t *model = view_get_model(handle);


    view_common_create_title(lv_scr_act(), "Impostazioni", BACK_BTN_ID, -1, -1);

    lv_obj_t *img_alarm = lv_img_create(lv_scr_act());
    lv_img_set_src(img_alarm, &img_connection);
    lv_obj_align(img_alarm, LV_ALIGN_BOTTOM_RIGHT, -8, -8);
    pdata->img_alarm = img_alarm;

    lv_obj_t *cont = lv_obj_create(lv_scr_act());
    lv_obj_set_style_pad_column(cont, 16, LV_STATE_DEFAULT);
    lv_obj_set_size(cont, LV_HOR_RES, LV_VER_RES - 64);
    lv_obj_set_layout(cont, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_align(cont, LV_ALIGN_BOTTOM_MID, 0, 0);

    {
        lv_obj_t *btn = lv_btn_create(cont);
        lv_obj_set_size(btn, 180, 80);
        lv_obj_t *lbl = lv_label_create(btn);
        lv_label_set_text(lbl, "Test");
        lv_obj_set_style_text_font(lbl, STYLE_FONT_MEDIUM, LV_STATE_DEFAULT);
        lv_obj_center(lbl);
        view_register_object_default_callback(btn, TEST_BTN_ID);
    }

    lv_obj_t *lbl = lv_label_create(lv_scr_act());
    lv_obj_set_style_text_font(lbl, STYLE_FONT_SMALL, LV_STATE_DEFAULT);
    lv_obj_align(lbl, LV_ALIGN_BOTTOM_LEFT, 8, -8);
    pdata->lbl_machine_version = lbl;

    VIEW_ADD_WATCHED_VARIABLE(&model->run.machine_firmware_version_major, 0);
    VIEW_ADD_WATCHED_VARIABLE(&model->run.machine_firmware_version_minor, 0);
    VIEW_ADD_WATCHED_VARIABLE(&model->run.machine_firmware_version_patch, 0);
    VIEW_ADD_WATCHED_VARIABLE(&model->run.communication_error, 0);

    update_page(model, pdata);
}


static pman_msg_t page_event(pman_handle_t handle, void *state, pman_event_t event) {
    struct page_data *pdata = state;

    mut_model_t *model = view_get_model(handle);

    pman_msg_t msg = PMAN_MSG_NULL;

    switch (event.tag) {
        case PMAN_EVENT_TAG_OPEN:
            model_boiler_enable(model, 0);
            break;

        case PMAN_EVENT_TAG_USER: {
            view_event_t *view_event = event.as.user;
            switch (view_event->tag) {
                case VIEW_EVENT_TAG_PAGE_WATCHER: {
                    update_page(model, pdata);
                    break;
                }

                default:
                    break;
            }

            break;
        }

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
                            msg.stack_msg = PMAN_STACK_MSG_PUSH_PAGE(&page_test_adc);
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


static void update_page(model_t *model, struct page_data *pdata) {
    view_common_set_hidden(pdata->img_alarm, !model->run.communication_error);

    lv_label_set_text_fmt(pdata->lbl_machine_version, "D: v%i.%i.%i\nM: v%i.%i.%i",
                          (int)APP_CONFIG_FIRMWARE_VERSION_MAJOR, (int)APP_CONFIG_FIRMWARE_VERSION_MINOR,
                          (int)APP_CONFIG_FIRMWARE_VERSION_PATCH, model->run.machine_firmware_version_major,
                          model->run.machine_firmware_version_minor, model->run.machine_firmware_version_patch);
}


static void close_page(void *state) {
    (void)state;
    view_clear_watcher();
    lv_obj_clean(lv_scr_act());
}


const pman_page_t page_menu = {
    .create        = create_page,
    .destroy       = pman_destroy_all,
    .open          = open_page,
    .close         = close_page,
    .process_event = page_event,
};
