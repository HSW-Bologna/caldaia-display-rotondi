#ifndef BSP_RS485_H_INCLUDED
#define BSP_RS485_H_INCLUDED


#include <stdint.h>
#include <stdlib.h>


void bsp_rs485_init(void);
void bsp_rs485_write(uint8_t *buffer, size_t len);
int  bsp_rs485_read(uint8_t *buffer, size_t len, unsigned long timeout_ms);
void bsp_rs485_flush(void);
void bsp_rs485_flush_input(void);
void bsp_rs485_wait_tx_done(void);


#endif
