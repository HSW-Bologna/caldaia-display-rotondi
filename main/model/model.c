#include <stdlib.h>
#include <assert.h>
#include "model.h"
#include "config/app_config.h"


void model_init(mut_model_t *model) {
    assert(model != NULL);

    model->config.language = 0;

    model->run.output_percentage         = 0;
    model->run.override_duty_cycle       = 0;
    model->run.overridden_duty_cycle     = 0;
    model->run.communication_error       = 0;
    model->run.pressure_setpoint_decibar = APP_CONFIG_MIN_PRESSURE_SETPOINT_DECIBAR;
    model->run.pressure_decibar          = 0;
}


void model_modify_pressure_setpoint(mut_model_t *model, int16_t change) {
    assert(model != NULL);

    if ((int16_t)model->run.pressure_setpoint_decibar + change > APP_CONFIG_MAX_PRESSURE_SETPOINT_DECIBAR) {
        model->run.pressure_setpoint_decibar = APP_CONFIG_MAX_PRESSURE_SETPOINT_DECIBAR;
    } else if ((int16_t)model->run.pressure_setpoint_decibar + change < APP_CONFIG_MIN_PRESSURE_SETPOINT_DECIBAR) {
        model->run.pressure_setpoint_decibar = APP_CONFIG_MIN_PRESSURE_SETPOINT_DECIBAR;
    } else {
        model->run.pressure_setpoint_decibar += change;
    }
}
