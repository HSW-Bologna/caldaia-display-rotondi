#include "model/model.h"
#include "adapters/view/view.h"
#include "controller.h"
#include "esp_log.h"
#include "services/timestamp.h"
#include "lvgl.h"


static const char *TAG = "Gui";


void controller_gui_manage(void) {
    (void)TAG;

#ifndef BUILD_CONFIG_SIMULATED_APP
    static timestamp_t last_invoked = 0;

    if (last_invoked != timestamp_get()) {
        if (last_invoked > 0) {
            lv_tick_inc(timestamp_interval(last_invoked, timestamp_get()));
        }
        last_invoked = timestamp_get();
    }
#endif

    view_manage();
    lv_timer_handler();
}
