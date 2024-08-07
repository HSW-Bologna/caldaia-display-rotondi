#include <stdlib.h>
#include <assert.h>
#include "model.h"
#include "config/app_config.h"


#define MODIFY_WITH_LIMITS(Par, Min, Max, Change, Cast)                                                                \
    if ((Cast)(Par) + (Change) > (Max)) {                                                                              \
        (Par) = (Max);                                                                                                 \
    } else if ((Cast)(Par) + (Change) < (Min)) {                                                                       \
        (Par) = (Min);                                                                                                 \
    } else {                                                                                                           \
        (Par) += (Change);                                                                                             \
    }

#define CHECK_LIMITS(Par, Min, Max, Cast) MODIFY_WITH_LIMITS(Par, Min, Max, 0, Cast)


void model_init(mut_model_t *model) {
    assert(model != NULL);

    model->run.boiler_enabled                 = 0;
    model->run.output_percentage              = 0;
    model->run.override_duty_cycle            = 0;
    model->run.overridden_duty_cycle          = 0;
    model->run.communication_error            = 0;
    model->run.pressure_centibar              = 0;
    model->run.pressure_adc                   = 0;
    model->run.adc_r1                         = 0;
    model->run.adc_s                          = 0;
    model->run.adc_t                          = 0;
    model->run.machine_firmware_version_major = 0;
    model->run.machine_firmware_version_minor = 0;
    model->run.machine_firmware_version_patch = 0;
    model->run.pid_error = 0;
    model->run.pid_p_output = 0;
    model->run.pid_i_output = 0;
    model->run.pid_d_output = 0;

    model->config.pid_kp = APP_CONFIG_DEFAULT_PID_KP;
    model->config.pid_ki = APP_CONFIG_DEFAULT_PID_KI;
    model->config.pid_kd = APP_CONFIG_DEFAULT_PID_KD;
    model->config.pressure_setpoint_decibar = APP_CONFIG_MIN_PRESSURE_SETPOINT_DECIBAR;
    model->config.pressure_offset_adc = 0;
}


void model_check_configuration(mut_model_t *model) {
    assert(model != NULL);

    CHECK_LIMITS(model->config.pressure_setpoint_decibar, APP_CONFIG_MIN_PRESSURE_SETPOINT_DECIBAR,
                 APP_CONFIG_MAX_PRESSURE_SETPOINT_DECIBAR, int16_t);
    CHECK_LIMITS(model->config.pid_kp, APP_CONFIG_MIN_PID_KP, APP_CONFIG_MAX_PID_KP, float);
    CHECK_LIMITS(model->config.pid_ki, APP_CONFIG_MIN_PID_KI, APP_CONFIG_MAX_PID_KI, float);
    CHECK_LIMITS(model->config.pid_kd, APP_CONFIG_MIN_PID_KD, APP_CONFIG_MAX_PID_KD, float);
}


void model_boiler_enable(mut_model_t *model, uint8_t enable) {
    assert(model != NULL);

    model->run.boiler_enabled = enable;
}


void model_modify_pressure_setpoint(mut_model_t *model, int16_t change) {
    assert(model != NULL);

    MODIFY_WITH_LIMITS(model->config.pressure_setpoint_decibar, APP_CONFIG_MIN_PRESSURE_SETPOINT_DECIBAR,
                       APP_CONFIG_MAX_PRESSURE_SETPOINT_DECIBAR, change, int16_t);
}


void model_modify_pid_kp(mut_model_t *model, float change) {
    assert(model != NULL);

    MODIFY_WITH_LIMITS(model->config.pid_kp, APP_CONFIG_MIN_PID_KP, APP_CONFIG_MAX_PID_KP, change, float);
}


void model_modify_pid_ki(mut_model_t *model, float change) {
    assert(model != NULL);

    MODIFY_WITH_LIMITS(model->config.pid_ki, APP_CONFIG_MIN_PID_KI, APP_CONFIG_MAX_PID_KI, change, float);
}


void model_modify_pid_kd(mut_model_t *model, float change) {
    assert(model != NULL);

    MODIFY_WITH_LIMITS(model->config.pid_kd, APP_CONFIG_MIN_PID_KD, APP_CONFIG_MAX_PID_KD, change, float);
}


uint8_t model_get_language(model_t *model) {
    (void)model;
    return 0;
}


void model_calibrate_pressure(mut_model_t *model) {
    assert(model != NULL);

    model->config.pressure_offset_adc = model->run.pressure_adc;
}
