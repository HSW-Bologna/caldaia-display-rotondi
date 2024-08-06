#include <stdlib.h>
#include <assert.h>
#include "model.h"
#include "config/app_config.h"


void model_init(mut_model_t *model) {
    assert(model != NULL);

    model->run.boiler_enabled                 = 0;
    model->run.output_percentage              = 0;
    model->run.override_duty_cycle            = 0;
    model->run.overridden_duty_cycle          = 0;
    model->run.communication_error            = 0;
    model->run.pressure_millibar               = 0;
    model->run.pressure_adc                   = 0;
    model->run.adc_r1                         = 0;
    model->run.adc_s                          = 0;
    model->run.adc_t                          = 0;
    model->run.machine_firmware_version_major = 0;
    model->run.machine_firmware_version_minor = 0;
    model->run.machine_firmware_version_patch = 0;

    model->config.pressure_setpoint_decibar = APP_CONFIG_MIN_PRESSURE_SETPOINT_DECIBAR;
}



void model_boiler_enable(mut_model_t *model, uint8_t enable) {
    assert(model != NULL);

    model->run.boiler_enabled = enable;
}


void model_modify_pressure_setpoint(mut_model_t *model, int16_t change) {
    assert(model != NULL);

    if ((int16_t)model->config.pressure_setpoint_decibar + change > APP_CONFIG_MAX_PRESSURE_SETPOINT_DECIBAR) {
        model->config.pressure_setpoint_decibar = APP_CONFIG_MAX_PRESSURE_SETPOINT_DECIBAR;
    } else if ((int16_t)model->config.pressure_setpoint_decibar + change < APP_CONFIG_MIN_PRESSURE_SETPOINT_DECIBAR) {
        model->config.pressure_setpoint_decibar = APP_CONFIG_MIN_PRESSURE_SETPOINT_DECIBAR;
    } else {
        model->config.pressure_setpoint_decibar += change;
    }
}


uint8_t model_get_language(model_t *model) {
    (void)model;
    return 0;
}
