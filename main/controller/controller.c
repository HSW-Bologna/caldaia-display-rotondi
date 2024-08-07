#include "controller.h"
#include "model/model.h"
#include "adapters/view/view.h"
#include "gui.h"
#include "modbus.h"
#include "services/timestamp.h"
#include "observer.h"
#include "esp_log.h"
#include "configuration.h"


static const char *TAG = "Controller";


void controller_init(mut_model_t *model) {
    (void)model;

    configuration_load(model);
    observer_init(model);
    modbus_init();

    view_change_page(&page_main);

    ESP_LOGI(TAG, "Initialized");
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
                    model->run.communication_error = response.error;

                    if (!response.error) {
                        model->run.machine_firmware_version_major = response.as.state.version_major;
                        model->run.machine_firmware_version_minor = response.as.state.version_minor;
                        model->run.machine_firmware_version_patch = response.as.state.version_patch;
                        model->run.pressure_adc                   = response.as.state.analog_value_pressure;
                        model->run.adc_r1                         = response.as.state.analog_value_r1;
                        model->run.adc_s                          = response.as.state.analog_value_s;
                        model->run.adc_t                          = response.as.state.analog_value_t;
                        model->run.pressure_centibar              = response.as.state.pressure_centibar;
                        model->run.output_percentage              = response.as.state.output_percentage;
                        model->run.pid_p_output              = response.as.state.pid_p_output;
                        model->run.pid_i_output              = response.as.state.pid_i_output;
                        model->run.pid_d_output              = response.as.state.pid_d_output;
                        model->run.pid_error                      = response.as.state.pid_error;
                    }
                    break;
            }
        }
    }

    controller_gui_manage();
    observer_manage(model);

    if (timestamp_is_expired(minion_ts, 500)) {
        modbus_read_state();
        modbus_sync(model);
        minion_ts = timestamp_get();
    }
}
