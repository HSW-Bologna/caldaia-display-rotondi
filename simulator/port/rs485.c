#include "bsp/rs485.h"


void bsp_rs485_write(uint8_t *buffer, size_t len) {
    (void)buffer;
    (void)len;
}


int bsp_rs485_read(uint8_t *buffer, size_t len, unsigned long timeout_ms) {
    (void)buffer;
    (void)len;
    (void)timeout_ms;
    return 0;
}


void bsp_rs485_flush() {}
