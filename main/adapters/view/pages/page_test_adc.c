#include <assert.h>
#include <stdlib.h>
#include "lvgl.h"
#include "model/model.h"
#include "../view.h"
#include "../common.h"
#include "../theme/style.h"
#include "../intl/intl.h"
#include <esp_log.h>


LV_IMG_DECLARE(img_connection);


enum {
    BTN_BACK_ID,
    BTN_PREV_ID,
    BTN_NEXT_ID,
    BTN_OFFSET_ID,
};


struct page_data {
    lv_obj_t *lbl_adc;

    lv_obj_t *img_alarm;
};


static void update_page(model_t *model, struct page_data *pdata);


static const char *TAG = "PageTestAdc";


static void *create_page(pman_handle_t handle, void *extra) {
    (void)handle;
    (void)extra;

    struct page_data *pdata = lv_malloc(sizeof(struct page_data));
    assert(pdata != NULL);

    ESP_LOGI(TAG, "Created");

    return pdata;
}


static void open_page(pman_handle_t handle, void *state) {
    struct page_data *pdata = state;

    model_t *model = view_get_model(handle);

    view_common_create_title(lv_scr_act(), "ADC", BTN_BACK_ID, BTN_PREV_ID, BTN_NEXT_ID);

    lv_obj_t *img_alarm = lv_img_create(lv_scr_act());
    lv_img_set_src(img_alarm, &img_connection);
    lv_obj_align(img_alarm, LV_ALIGN_BOTTOM_RIGHT, -8, -8);
    pdata->img_alarm = img_alarm;

    {
        lv_obj_t *lbl = lv_label_create(lv_scr_act());
        lv_obj_align(lbl, LV_ALIGN_CENTER, 0, -16);
        lv_obj_set_style_text_font(lbl, STYLE_FONT_SMALL, LV_STATE_DEFAULT);
        pdata->lbl_adc = lbl;
    }

    {
        lv_obj_t *btn = lv_btn_create(lv_scr_act());
        lv_obj_t *lbl = lv_label_create(btn);
        view_register_object_default_callback(btn, BTN_OFFSET_ID);
        lv_label_set_text(lbl, "Azzera offset");
        lv_obj_align(btn, LV_ALIGN_BOTTOM_MID, 0, -16);
    }

    VIEW_ADD_WATCHED_VARIABLE(&model->run.pressure_millibar, 0);
    VIEW_ADD_WATCHED_VARIABLE(&model->run.communication_error, 0);
    VIEW_ADD_WATCHED_VARIABLE(&model->run.pressure_adc, 0);
    VIEW_ADD_WATCHED_VARIABLE(&model->run.adc_r1, 0);
    VIEW_ADD_WATCHED_VARIABLE(&model->run.adc_s, 0);
    VIEW_ADD_WATCHED_VARIABLE(&model->run.adc_t, 0);

    update_page(model, pdata);
}


static pman_msg_t page_event(pman_handle_t handle, void *state, pman_event_t event) {
    pman_msg_t msg = PMAN_MSG_NULL;

    struct page_data *pdata = state;

    mut_model_t *model = view_get_model(handle);

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
                        case BTN_BACK_ID:
                            model->run.override_duty_cycle = 0;
                            msg.stack_msg                  = PMAN_STACK_MSG_BACK();
                            break;

                        case BTN_PREV_ID:
                            model->run.override_duty_cycle = 0;
                            msg.stack_msg                  = PMAN_STACK_MSG_SWAP(&page_test_phase_cut);
                            break;

                        case BTN_NEXT_ID:
                            model->run.override_duty_cycle = 0;
                            msg.stack_msg                  = PMAN_STACK_MSG_SWAP(&page_test_pid);
                            break;

                        case BTN_OFFSET_ID:
                            model_calibrate_pressure(model);
                            update_page(model, pdata);
                            break;

                        default:
                            break;
                    }
                    break;
                }

                case LV_EVENT_VALUE_CHANGED: {
                    switch (obj_data->id) {
                        default:
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
    float pressure = ((float)model_get_calibrated_pressure(model)) / 1000.;
    float offset   = ((float)model->config.pressure_offset_millibar) / 1000.;

    lv_label_set_text_fmt(pdata->lbl_adc, "Pressione: %2.3f Bar (-%2.3f) [%04i]\nFasi: r1=%04i, s=%04i, t=%04i",
                          pressure, offset, model->run.pressure_adc, model->run.adc_r1, model->run.adc_s,
                          model->run.adc_t);

    view_common_set_hidden(pdata->img_alarm, !model->run.communication_error);
}


static void close_page(void *state) {
    (void)state;
    view_clear_watcher();
    lv_obj_clean(lv_scr_act());
}


const pman_page_t page_test_adc = {
    .create        = create_page,
    .destroy       = pman_destroy_all,
    .open          = open_page,
    .close         = close_page,
    .process_event = page_event,
};
