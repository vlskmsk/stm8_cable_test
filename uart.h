#ifndef UART_H
#define UART_H

#include "stm8s.h"
#include "stm8s_uart1.h"
#include "system.h"

void uart_init();
void uart_write(uint8_t data);
void uart_writeMulti(uint8_t *data, int length);

#endif