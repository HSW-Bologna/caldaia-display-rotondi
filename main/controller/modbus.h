#ifndef MODBUS_H_INCLUDED
#define MODBUS_H_INCLUDED


#include <stdint.h>
#include "model/model.h"


typedef enum {
    MODBUS_RESPONSE_TAG_OK,
    MODBUS_RESPONSE_TAG_READ_STATE,
} modbus_response_tag_t;


typedef struct {
    modbus_response_tag_t tag;
    uint8_t               error;
    union {
        struct {
            uint16_t machine_model;
            uint16_t version_major;
            uint16_t version_minor;
            uint16_t version_patch;
            uint16_t output_percentage;
            uint16_t pid_p_output;
            uint16_t pid_i_output;
            uint16_t pid_d_output;
            uint16_t analog_value_r1;
            uint16_t analog_value_s;
            uint16_t analog_value_t;
            uint16_t analog_value_pressure;
            uint16_t pressure_centibar;
            int16_t pid_error;
        } state;
    } as;
} modbus_response_t;


void    modbus_init(void);
uint8_t modbus_get_response(modbus_response_t *response);
void    modbus_read_state(void);
void    modbus_sync(model_t *pmodel);


#endif
