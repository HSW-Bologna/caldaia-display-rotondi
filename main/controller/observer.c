#include "observer.h"
#include "watcher.h"
#include "model/model.h"
#include "services/timestamp.h"
#include "configuration.h"


void save_parameter(void *old_value, const void *new_value, watcher_size_t size, void *user_ptr, void *arg);
void save_float_parameter(void *old_value, const void *new_value, watcher_size_t size, void *user_ptr, void *arg);


static watcher_t watcher = {0};


void observer_init(model_t *model) {
    WATCHER_INIT_STD(&watcher, (void *)model);
    WATCHER_ADD_ENTRY_DELAYED(&watcher, &model->config.pressure_setpoint_decibar, save_parameter,
                              (void *)CONFIGURATION_SETPOINT_KEY, 4000);
    WATCHER_ADD_ENTRY_DELAYED(&watcher, &model->config.pressure_offset_adc, save_parameter,
                              (void *)CONFIGURATION_PRESSURE_OFFSET_KEY, 4000);
    WATCHER_ADD_ENTRY_DELAYED(&watcher, &model->config.pid_kp, save_float_parameter, (void *)CONFIGURATION_PID_KP_KEY, 4000);
    WATCHER_ADD_ENTRY_DELAYED(&watcher, &model->config.pid_ki, save_float_parameter, (void *)CONFIGURATION_PID_KI_KEY, 4000);
    WATCHER_ADD_ENTRY_DELAYED(&watcher, &model->config.pid_kd, save_float_parameter, (void *)CONFIGURATION_PID_KD_KEY, 4000);
}


void observer_manage(model_t *model) {
    (void)model;
    watcher_watch(&watcher, timestamp_get());
}


void save_parameter(void *old_value, const void *new_value, watcher_size_t size, void *user_ptr, void *arg) {
    (void)old_value;
    (void)user_ptr;

    configuration_save_parameter((void *)new_value, size, arg);
}


void save_float_parameter(void *old_value, const void *new_value, watcher_size_t size, void *user_ptr, void *arg) {
    (void)old_value;
    (void)user_ptr;
    (void)size;

    uint16_t fixed_point_value = (uint16_t)(*((float *)new_value) * 100.);

    configuration_save_parameter((void *)&fixed_point_value, 2, arg);
}
