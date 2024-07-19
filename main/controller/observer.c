#include "observer.h"
#include "watcher.h"
#include "model/model.h"
#include "services/timestamp.h"


static watcher_t watcher = {0};


void observer_init(model_t *pmodel) {
    WATCHER_INIT_STD(&watcher, pmodel);
}


void observer_manage(model_t *pmodel) {
    (void)pmodel;
    watcher_watch(&watcher, timestamp_get());
}
