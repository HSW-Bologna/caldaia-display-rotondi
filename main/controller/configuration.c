#include "bsp/storage.h"
#include "model/model.h"
#include "configuration.h"


const char *CONFIGURATION_SETPOINT_KEY        = "SETPOINT";
const char *CONFIGURATION_PRESSURE_OFFSET_KEY = "OFFSET";
const char *CONFIGURATION_PID_KP_KEY          = "PIDKPRO";
const char *CONFIGURATION_PID_KI_KEY          = "PIDKINT";
const char *CONFIGURATION_PID_KD_KEY          = "PIDKDER";


void configuration_load(mut_model_t *model) {
    uint16_t coefficent = 0;

    storage_load_uint16(&model->config.pressure_setpoint_decibar, CONFIGURATION_SETPOINT_KEY);
    storage_load_uint16(&model->config.pressure_offset_millibar, CONFIGURATION_PRESSURE_OFFSET_KEY);

    if (storage_load_uint16(&coefficent, CONFIGURATION_PID_KP_KEY) == 0) {
        model->config.pid_kp = ((float)coefficent) / 100.;
    }

    if (storage_load_uint16(&coefficent, CONFIGURATION_PID_KI_KEY) == 0) {

        model->config.pid_ki = ((float)coefficent) / 100.;
    }

    if (storage_load_uint16(&coefficent, CONFIGURATION_PID_KD_KEY) == 0) {
        model->config.pid_kd = ((float)coefficent) / 100.;
    }

    model_check_configuration(model);
}


void configuration_save_parameter(void *parameter, size_t size, const char *key) {
    switch (size) {
        case 1:
            storage_save_uint8(parameter, key);
            break;
        case 2:
            storage_save_uint16(parameter, key);
            break;
        case 4:
            storage_save_uint32(parameter, key);
            break;
        case 8:
            storage_save_uint64(parameter, key);
            break;
        default:
            storage_save_blob(parameter, size, key);
            break;
    }
}
