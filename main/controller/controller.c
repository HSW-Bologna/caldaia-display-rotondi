#include "controller.h"
#include "model/model.h"
#include "adapters/view/view.h"
#include "gui.h"
#include "modbus.h"
#include "services/timestamp.h"
#include "observer.h"


void controller_init(mut_model_t *model) {
    (void)model;

    observer_init(model);
    modbus_init();

    view_change_page(&page_main);
}


void controller_process_message(pman_handle_t handle, void *msg) {
    (void)handle;
    (void)msg;
}


void controller_manage(mut_model_t *model) {
    static timestamp_t minion_ts = 0;

    {
        modbus_response_t response = {0};
        if (modbus_get_response(&response)) {
            switch (response.tag) {
                case MODBUS_RESPONSE_TAG_OK: {
                    model->run.communication_error = response.error;
                    break;
                }

                case MODBUS_RESPONSE_TAG_READ_STATE:
                    break;
            }
        }
    }

    controller_gui_manage();
    observer_manage(model);

    if (timestamp_is_expired(minion_ts, 500)) {
        //modbus_read_state();
        //modbus_sync(model);
        minion_ts = timestamp_get();
    }
}
