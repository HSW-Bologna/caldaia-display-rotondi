#include <stdlib.h>
#include <assert.h>
#include "model.h"


void model_init(mut_model_t *model) {
    assert(model != NULL);

    model->config.language = 0;

    model->run.override_duty_cycle   = 0;
    model->run.overridden_duty_cycle = 0;
    model->run.communication_error   = 0;
}
