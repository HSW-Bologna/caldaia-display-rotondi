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
    BTN_ONOFF_ID,
    BTN_BACK_ID,
    BTN_PREV_ID,
    BTN_NEXT_ID,
    PAR_SETPOINT_ID,
    PAR_KP_ID,
    PAR_KI_ID,
    PAR_KD_ID,
    SLIDER_PERCENTAGE_ID,
};


struct page_data {
    lv_obj_t *btn_onoff;

    lv_obj_t *lbl_setpoint;
    lv_obj_t *lbl_kp;
    lv_obj_t *lbl_ki;
    lv_obj_t *lbl_kd;
    lv_obj_t *lbl_info;

    lv_obj_t *img_alarm;
};


static void      update_page(model_t *model, struct page_data *pdata);
static lv_obj_t *parameter_control_create(lv_obj_t *parent, lv_obj_t **lbl, int id);


static const char *TAG = "PageTestPid";


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

    view_common_create_title(lv_scr_act(), "PID", BTN_BACK_ID, BTN_PREV_ID, BTN_NEXT_ID);

    lv_obj_t *img_alarm = lv_img_create(lv_scr_act());
    lv_img_set_src(img_alarm, &img_connection);
    lv_obj_align(img_alarm, LV_ALIGN_BOTTOM_RIGHT, -8, -8);
    pdata->img_alarm = img_alarm;

    lv_obj_t *cont = lv_obj_create(lv_scr_act());
    lv_obj_add_style(cont, &style_transparent_cont, LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(cont, 8, LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(cont, 4, LV_STATE_DEFAULT);
    lv_obj_set_size(cont, LV_HOR_RES, LV_VER_RES - 68);
    lv_obj_set_layout(cont, LV_LAYOUT_FLEX);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_ROW_WRAP);
    lv_obj_set_flex_align(cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_START);
    lv_obj_align(cont, LV_ALIGN_BOTTOM_MID, 0, 0);

    parameter_control_create(cont, &pdata->lbl_setpoint, PAR_SETPOINT_ID);
    parameter_control_create(cont, &pdata->lbl_kp, PAR_KP_ID);
    parameter_control_create(cont, &pdata->lbl_ki, PAR_KI_ID);
    parameter_control_create(cont, &pdata->lbl_kd, PAR_KD_ID);

    {
        lv_obj_t *obj = parameter_control_create(cont, &pdata->lbl_info, -1);
        lv_obj_set_width(obj, 250);
        lv_obj_set_width(pdata->lbl_info, 240);
    }

    {
        lv_obj_t *btn = lv_btn_create(cont);
        view_register_object_default_callback(btn, BTN_ONOFF_ID);
        lv_obj_t *lbl = lv_label_create(btn);
        lv_obj_center(lbl);
        lv_obj_align(btn, LV_ALIGN_BOTTOM_MID, 96, -8);
        pdata->btn_onoff = btn;
    }

    VIEW_ADD_WATCHED_VARIABLE(&model->run.communication_error, 0);
    VIEW_ADD_WATCHED_VARIABLE(&model->run.output_percentage, 0);
    VIEW_ADD_WATCHED_VARIABLE(&model->run.pid_error, 0);
    VIEW_ADD_WATCHED_VARIABLE(&model->run.pressure_centibar, 0);

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
                        case BTN_ONOFF_ID: {
                            model_boiler_enable(model, !model->run.boiler_enabled);
                            update_page(model, pdata);
                            break;
                        }

                        case BTN_BACK_ID:
                            msg.stack_msg = PMAN_STACK_MSG_BACK();
                            break;

                        case BTN_PREV_ID:
                            msg.stack_msg = PMAN_STACK_MSG_SWAP(&page_test_adc);
                            break;

                        case BTN_NEXT_ID:
                            msg.stack_msg = PMAN_STACK_MSG_SWAP(&page_test_phase_cut);
                            break;

                        case PAR_SETPOINT_ID:
                            model_modify_pressure_setpoint(model, obj_data->number);
                            update_page(model, pdata);
                            break;

                        case PAR_KP_ID:
                            model_modify_pid_kp(model, obj_data->number * 0.01);
                            update_page(model, pdata);
                            break;

                        case PAR_KI_ID:
                            model_modify_pid_ki(model, obj_data->number * 0.01);
                            update_page(model, pdata);
                            break;

                        case PAR_KD_ID:
                            model_modify_pid_kd(model, obj_data->number * 0.01);
                            update_page(model, pdata);
                            break;

                        default:
                            break;
                    }
                    break;
                }

                case LV_EVENT_LONG_PRESSED_REPEAT: {
                    switch (obj_data->id) {
                        case PAR_SETPOINT_ID:
                            model_modify_pressure_setpoint(model, obj_data->number*5);
                            update_page(model, pdata);
                            break;

                        case PAR_KP_ID:
                            model_modify_pid_kp(model, obj_data->number * 0.05);
                            update_page(model, pdata);
                            break;

                        case PAR_KI_ID:
                            model_modify_pid_ki(model, obj_data->number * 0.05);
                            update_page(model, pdata);
                            break;

                        case PAR_KD_ID:
                            model_modify_pid_kd(model, obj_data->number * 0.05);
                            update_page(model, pdata);
                            break;

                        default:
                            break;
                    }
                    break;
                }

                case LV_EVENT_VALUE_CHANGED: {
                    switch (obj_data->id) {
                        case SLIDER_PERCENTAGE_ID: {
                            model->run.overridden_duty_cycle = lv_slider_get_value(target);
                            update_page(model, pdata);
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

            break;
        }

        default:
            break;
    }

    return msg;
}


static void update_page(model_t *model, struct page_data *pdata) {
    if (model->run.boiler_enabled) {
        lv_obj_set_style_bg_color(pdata->btn_onoff, VIEW_STYLE_COLOR_GREEN, LV_STATE_DEFAULT);
        lv_label_set_text(lv_obj_get_child(pdata->btn_onoff, 0), "ON");
    } else {
        lv_obj_set_style_bg_color(pdata->btn_onoff, VIEW_STYLE_COLOR_RED, LV_STATE_DEFAULT);
        lv_label_set_text(lv_obj_get_child(pdata->btn_onoff, 0), "OFF");
    }

    lv_label_set_text_fmt(pdata->lbl_setpoint, "%4i cBar", model->config.pressure_setpoint_decibar*10);

    lv_label_set_text_fmt(pdata->lbl_kp, "Kp %2.2f", model->config.pid_kp);
    lv_label_set_text_fmt(pdata->lbl_ki, "Ki %2.2f", model->config.pid_ki);
    lv_label_set_text_fmt(pdata->lbl_kd, "Kd %2.2f", model->config.pid_kd);

    lv_label_set_text_fmt(pdata->lbl_info, "%4i cBar, Error %i,\nP %i, I %i, D %i, Tot %i", model->run.pressure_centibar, model->run.pid_error,
        model->run.pid_p_output, model->run.pid_i_output, model->run.pid_d_output, model->run.output_percentage);

    view_common_set_hidden(pdata->img_alarm, !model->run.communication_error);
}


static lv_obj_t *parameter_control_create(lv_obj_t *parent, lv_obj_t **lbl, int id) {
    lv_obj_t *cont = lv_obj_create(parent);
    lv_obj_set_size(cont, 220, 60);
    lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_pad_left(cont, 4, LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(cont, 4, LV_STATE_DEFAULT);

    *lbl = lv_label_create(cont);
    lv_obj_set_style_text_font(*lbl, STYLE_FONT_SMALL, LV_STATE_DEFAULT);
    lv_obj_center(*lbl);
    lv_label_set_long_mode(*lbl, LV_LABEL_LONG_WRAP);
    lv_obj_set_style_text_align(*lbl, LV_TEXT_ALIGN_CENTER, LV_STATE_DEFAULT);

    if (id >= 0) {
        {
            lv_obj_t *btn = lv_btn_create(cont);
            lv_obj_set_size(btn, 56, 48);
            lv_obj_t *lbl = lv_label_create(btn);
            lv_obj_set_style_text_font(lbl, STYLE_FONT_SMALL, LV_STATE_DEFAULT);
            lv_label_set_text(lbl, LV_SYMBOL_MINUS);
            lv_obj_center(lbl);
            lv_obj_align(btn, LV_ALIGN_LEFT_MID, 0, 0);

            view_register_object_default_callback_with_number(btn, id, -1);
        }

        {
            lv_obj_t *btn = lv_btn_create(cont);
            lv_obj_set_size(btn, 56, 48);
            lv_obj_t *lbl = lv_label_create(btn);
            lv_obj_set_style_text_font(lbl, STYLE_FONT_SMALL, LV_STATE_DEFAULT);
            lv_label_set_text(lbl, LV_SYMBOL_PLUS);
            lv_obj_center(lbl);
            lv_obj_align(btn, LV_ALIGN_RIGHT_MID, 0, 0);

            view_register_object_default_callback_with_number(btn, id, +1);
        }
        lv_obj_set_width(*lbl, 90);
    } else {
        lv_obj_set_width(*lbl, 200);
    }

    return cont;
}


static void close_page(void *state) {
    (void)state;
    view_clear_watcher();
    lv_obj_clean(lv_scr_act());
}


const pman_page_t page_test_pid = {
    .create        = create_page,
    .destroy       = pman_destroy_all,
    .open          = open_page,
    .close         = close_page,
    .process_event = page_event,
};
