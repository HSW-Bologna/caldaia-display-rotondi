#ifndef MODEL_H_INCLUDED
#define MODEL_H_INCLUDED

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>


#define GETTER(name, field)                                                                                            \
    static inline                                                                                                      \
        __attribute__((always_inline, const)) typeof(((mut_model_t *)0)->field) model_get_##name(model_t *pmodel) {    \
        assert(pmodel != NULL);                                                                                        \
        return pmodel->field;                                                                                          \
    }


struct model {
    struct {
        uint16_t pressure_offset_adc;
        uint16_t pressure_setpoint_decibar;
        float    pid_kp;
        float    pid_ki;
        float    pid_kd;
    } config;

    struct {
        uint8_t communication_error;
        uint8_t boiler_enabled;
        uint8_t override_duty_cycle;
        uint8_t overridden_duty_cycle;

        uint16_t output_percentage;
        uint16_t pid_p_output;
        uint16_t pid_i_output;
        uint16_t pid_d_output;
        int16_t pid_error;

        uint16_t pressure_centibar;
        uint16_t pressure_adc;

        uint16_t adc_r1;
        uint16_t adc_s;
        uint16_t adc_t;

        uint16_t machine_firmware_version_major;
        uint16_t machine_firmware_version_minor;
        uint16_t machine_firmware_version_patch;
    } run;
};


typedef const struct model model_t;
typedef struct model       mut_model_t;


void     model_init(mut_model_t *pmodel);
void     model_modify_pressure_setpoint(mut_model_t *model, int16_t change);
void     model_boiler_enable(mut_model_t *model, uint8_t enable);
uint8_t  model_get_language(model_t *model);
void     model_calibrate_pressure(mut_model_t *model);
void     model_check_configuration(mut_model_t *model);
void     model_modify_pid_kp(mut_model_t *model, float change);
void     model_modify_pid_ki(mut_model_t *model, float change);
void     model_modify_pid_kd(mut_model_t *model, float change);


#endif
