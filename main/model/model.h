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
        uint16_t language;
    } config;

    struct {
        uint8_t communication_error;
        uint8_t override_duty_cycle;
        uint8_t overridden_duty_cycle;

        uint8_t output_percentage;

        uint16_t pressure_setpoint_decibar;
        uint16_t pressure_decibar;
        uint16_t pressure_adc;
    } run;
};


typedef const struct model model_t;
typedef struct model       mut_model_t;


void model_init(mut_model_t *pmodel);
void model_modify_pressure_setpoint(mut_model_t *model, int16_t change);


GETTER(language, config.language);


#endif
