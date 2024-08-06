#include <assert.h>
#include <stdlib.h>
#include "lvgl.h"
#include "model/model.h"
#include "../view.h"
#include "../common.h"
#include "../theme/style.h"
#include "../intl/intl.h"
#include "config/app_config.h"
#include <esp_log.h>


#define SETTINGS_DRAG_WIDTH    32
#define SETTINGS_DRAG_HEIGHT   100
#define SETTINGS_DRAWER_WIDTH  128
#define SETTINGS_DRAWER_HEIGHT LV_HOR_RES
#define SETTINGS_BTN_WIDTH     64

LV_IMG_DECLARE(img_boiler_off);
LV_IMG_DECLARE(img_boiler_heat);
LV_IMG_DECLARE(img_water_still);
LV_IMG_DECLARE(img_pressostat_low);
LV_IMG_DECLARE(img_connection);


enum {
    OBJ_SETTINGS_ID,
    BTN_PLUS_ID,
    BTN_MINUS_ID,
    WATCHER_COMMUNICATION_ID,
    WATCHER_BOILER_ID,
};


struct page_data {
    lv_obj_t *img_boiler;
    lv_obj_t *img_heat;
    lv_obj_t *img_water;
    lv_obj_t *img_pressostat;

    lv_obj_t *lbl_setpoint;

    lv_obj_t *obj_handle;
    lv_obj_t *obj_drawer;
    lv_obj_t *obj_alarm;

    pman_timer_t *timer;
};


static void update_page(model_t *model, struct page_data *pdata);


static const char *TAG = "PageMain";


static void *create_page(pman_handle_t handle, void *extra) {
    (void)handle;
    (void)extra;

    struct page_data *pdata = lv_malloc(sizeof(struct page_data));
    assert(pdata != NULL);

    pdata->timer = pman_timer_create(handle, 500, NULL);

    ESP_LOGI(TAG, "Created");

    return pdata;
}


static void open_page(pman_handle_t handle, void *state) {
    struct page_data *pdata = state;

    model_t *model = view_get_model(handle);

    lv_obj_t *background = lv_obj_create(lv_scr_act());
    lv_obj_set_size(background, LV_PCT(100), LV_PCT(100));
    lv_obj_add_style(background, (lv_style_t *)&style_padless_cont, LV_STATE_DEFAULT);
    lv_obj_add_style(background, (lv_style_t *)&style_borderless_cont, LV_STATE_DEFAULT);
    lv_obj_clear_flag(background, LV_OBJ_FLAG_SCROLLABLE);

    {
        lv_obj_t *btn = lv_btn_create(background);
        lv_obj_set_size(btn, 320 - 16, 320 - 16);
        lv_obj_add_style(btn, (lv_style_t *)&style_black_border, LV_STATE_DEFAULT);
        lv_obj_clear_flag(btn, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_align(btn, LV_ALIGN_LEFT_MID, 8, 0);

        lv_obj_t *obj_alarm = lv_obj_create(background);
        lv_obj_set_size(obj_alarm, 320 - 16, 320 - 16);
        lv_obj_align(obj_alarm, LV_ALIGN_LEFT_MID, 8, 0);
        lv_obj_set_style_bg_opa(obj_alarm, LV_OPA_50, LV_STATE_DEFAULT);
        lv_obj_set_style_border_opa(obj_alarm, LV_OPA_50, LV_STATE_DEFAULT);

        lv_obj_t *cont = lv_obj_create(obj_alarm);
        lv_obj_set_style_radius(cont, LV_RADIUS_CIRCLE, LV_STATE_DEFAULT);
        lv_obj_set_size(cont, 96, 96);
        lv_obj_center(cont);

        lv_obj_t *img_alarm = lv_img_create(cont);
        lv_img_set_src(img_alarm, &img_connection);
        lv_obj_center(img_alarm);

        pdata->obj_alarm = obj_alarm;

        lv_obj_t *img_boiler = lv_img_create(btn);
        lv_img_set_src(img_boiler, &img_boiler_off);
        lv_obj_align(img_boiler, LV_ALIGN_CENTER, -12, 0);
        pdata->img_boiler = img_boiler;

        lv_obj_t *img_water = lv_img_create(btn);
        lv_img_set_src(img_water, &img_water_still);
        lv_obj_align(img_water, LV_ALIGN_CENTER, -29, 14);
        pdata->img_water = img_water;

        lv_obj_t *img_heat = lv_img_create(btn);
        lv_img_set_src(img_heat, &img_boiler_heat);
        lv_obj_set_style_img_recolor_opa(img_heat, LV_OPA_COVER, LV_STATE_DEFAULT);
        lv_obj_set_style_img_recolor(img_heat, VIEW_STYLE_COLOR_RED, LV_STATE_DEFAULT);
        lv_obj_align(img_heat, LV_ALIGN_CENTER, -29, 116);
        pdata->img_heat = img_heat;

        lv_obj_t *img_pressostat = lv_img_create(btn);
        lv_obj_align(img_pressostat, LV_ALIGN_CENTER, 86, 45);
        pdata->img_pressostat = img_pressostat;
    }

    {
        lv_obj_t *cont = lv_obj_create(background);
        lv_obj_set_size(cont, 160, LV_PCT(100));
        lv_obj_add_style(cont, &style_transparent_cont, LV_STATE_DEFAULT);
        lv_obj_add_style(cont, &style_padless_cont, LV_STATE_DEFAULT);
        lv_obj_align(cont, LV_ALIGN_RIGHT_MID, 0, 0);

        lv_obj_t *lbl = lv_label_create(cont);
        lv_obj_center(lbl);
        pdata->lbl_setpoint = lbl;

        {
            lv_obj_t *btn = lv_btn_create(cont);
            lv_obj_add_style(btn, (lv_style_t *)&style_black_border, LV_STATE_DEFAULT);
            lv_obj_set_size(btn, 128, 96);
            lv_obj_align(btn, LV_ALIGN_TOP_MID, 0, 8);
            lv_obj_clear_flag(btn, LV_OBJ_FLAG_SCROLLABLE);
            view_register_object_default_callback(btn, BTN_PLUS_ID);

            lv_obj_t *lbl = lv_label_create(btn);
            lv_label_set_text(lbl, LV_SYMBOL_PLUS);
            lv_obj_center(lbl);
        }

        {
            lv_obj_t *btn = lv_btn_create(cont);
            lv_obj_add_style(btn, (lv_style_t *)&style_black_border, LV_STATE_DEFAULT);
            lv_obj_set_size(btn, 128, 96);
            lv_obj_align(btn, LV_ALIGN_BOTTOM_MID, 0, -8);
            lv_obj_clear_flag(btn, LV_OBJ_FLAG_SCROLLABLE);
            view_register_object_default_callback(btn, BTN_MINUS_ID);

            lv_obj_t *lbl = lv_label_create(btn);
            lv_label_set_text(lbl, LV_SYMBOL_MINUS);
            lv_obj_center(lbl);
        }
    }

    {
        lv_obj_t *obj = lv_obj_create(background);
        lv_obj_set_size(obj, SETTINGS_DRAG_WIDTH, SETTINGS_DRAG_HEIGHT);
        lv_obj_align(obj, LV_ALIGN_RIGHT_MID, 0, 0);
        lv_obj_add_style(obj, (lv_style_t *)&style_transparent_cont, LV_STATE_DEFAULT);
        lv_obj_add_style(obj, (lv_style_t *)&style_padless_cont, LV_STATE_DEFAULT);
        lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
        view_register_object_default_callback(obj, OBJ_SETTINGS_ID);
        pdata->obj_handle = obj;

        lv_obj_t *cont = lv_obj_create(background);
        lv_obj_set_size(cont, SETTINGS_DRAWER_WIDTH, SETTINGS_DRAWER_HEIGHT);
        lv_obj_align_to(cont, obj, LV_ALIGN_OUT_RIGHT_MID, 0, 0);
        pdata->obj_drawer = cont;

        lv_obj_t *btn = lv_btn_create(cont);
        lv_obj_set_size(btn, SETTINGS_BTN_WIDTH, SETTINGS_BTN_WIDTH);
        lv_obj_t *lbl = lv_label_create(btn);
        lv_label_set_text(lbl, LV_SYMBOL_SETTINGS);
        lv_obj_center(lbl);
        lv_obj_align(btn, LV_ALIGN_CENTER, 0, 0);
    }

    VIEW_ADD_WATCHED_VARIABLE(&model->run.communication_error, WATCHER_COMMUNICATION_ID);
    VIEW_ADD_WATCHED_VARIABLE(&model->run.output_percentage, WATCHER_BOILER_ID);

    update_page(model, pdata);
}


static pman_msg_t page_event(pman_handle_t handle, void *state, pman_event_t event) {
    pman_msg_t msg = PMAN_MSG_NULL;

    struct page_data *pdata = state;

    mut_model_t *model = view_get_model(handle);

    switch (event.tag) {
        case PMAN_EVENT_TAG_TIMER: {
            pman_stack_msg_t         pw_msg = PMAN_STACK_MSG_SWAP(&page_test_phase_cut);
            password_page_options_t *opts =
                view_common_default_password_page_options(pw_msg, (const char *)APP_CONFIG_PASSWORD);
            msg.stack_msg = PMAN_STACK_MSG_PUSH_PAGE_EXTRA(&page_password, opts);
            break;
        }

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
                case LV_EVENT_PRESSING: {
                    switch (obj_data->id) {
                        case OBJ_SETTINGS_ID: {
                            lv_indev_t *indev = lv_indev_get_act();
                            if (indev != NULL) {
                                lv_point_t vect;
                                lv_indev_get_vect(indev, &vect);

                                lv_coord_t x = lv_obj_get_x_aligned(target) + vect.x;

                                if (x <= 0 && x >= -SETTINGS_DRAWER_WIDTH) {
                                    lv_obj_set_x(target, x);
                                    lv_obj_align_to(pdata->obj_drawer, target, LV_ALIGN_OUT_RIGHT_MID, 0, 0);
                                }
                            }
                            break;
                        }

                        default:
                            break;
                    }
                    break;
                }

                case LV_EVENT_RELEASED: {
                    switch (obj_data->id) {
                        case OBJ_SETTINGS_ID: {
                            lv_coord_t x = lv_obj_get_x_aligned(target);

                            if (x <= -SETTINGS_DRAWER_WIDTH / 2) {
                                lv_obj_align(target, LV_ALIGN_RIGHT_MID, -SETTINGS_DRAWER_WIDTH, 0);
                                pman_timer_reset(pdata->timer);
                                pman_timer_resume(pdata->timer);
                            } else {
                                lv_obj_align(target, LV_ALIGN_RIGHT_MID, 0, 0);
                            }
                            lv_obj_align_to(pdata->obj_drawer, target, LV_ALIGN_OUT_RIGHT_MID, 0, 0);
                            break;
                        }

                        default:
                            break;
                    }
                    break;
                }

                case LV_EVENT_CLICKED: {
                    switch (obj_data->id) {
                        case BTN_MINUS_ID:
                            model_modify_pressure_setpoint(model, -1);
                            update_page(model, pdata);
                            break;

                        case BTN_PLUS_ID:
                            model_modify_pressure_setpoint(model, 1);
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


static void close_page(void *state) {
    struct page_data *pdata = state;
    pman_timer_pause(pdata->timer);
    view_clear_watcher();
    lv_obj_clean(lv_scr_act());
}


static void update_page(model_t *model, struct page_data *pdata) {
    lv_img_set_src(pdata->img_boiler, &img_boiler_off);

    lv_img_set_src(pdata->img_water, &img_water_still);
    lv_img_set_src(pdata->img_pressostat, &img_pressostat_low);

    float setpoint = ((float)model->run.pressure_setpoint_decibar) / 10.;
    lv_label_set_text_fmt(pdata->lbl_setpoint, "%.1f", setpoint);

    lv_obj_set_style_opa(pdata->img_heat, (LV_OPA_COVER * model->run.output_percentage) / 100, LV_STATE_DEFAULT);

    ESP_LOGI(TAG, "%i", model->run.communication_error);

    if (model->run.communication_error) {
        view_common_set_hidden(pdata->obj_alarm, 0);
    } else {
        view_common_set_hidden(pdata->obj_alarm, 1);
    }
}


const pman_page_t page_main = {
    .create        = create_page,
    .destroy       = pman_destroy_all,
    .open          = open_page,
    .close         = close_page,
    .process_event = page_event,
};
