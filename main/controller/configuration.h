#ifndef CONFIGURATION_H_INCLUDED
#define CONFIGURATION_H_INCLUDED


#include "model/model.h"


void configuration_load(mut_model_t *model);
void configuration_save_parameter(void *parameter, size_t size, const char *key);


extern const char *CONFIGURATION_SETPOINT_KEY;
extern const char *CONFIGURATION_PID_KP_KEY;
extern const char *CONFIGURATION_PID_KI_KEY;
extern const char *CONFIGURATION_PID_KD_KEY;
extern const char *CONFIGURATION_PRESSURE_OFFSET_KEY;


#endif
