#include "uart.h"

void uart_init()
{
    UART1_Init(115200, UART1_WORDLENGTH_8D,
               UART1_STOPBITS_1, UART1_PARITY_NO,
               UART1_SYNCMODE_CLOCK_DISABLE, UART1_MODE_TX_ENABLE); //only TX, normal settings.
    UART1_Cmd(ENABLE);
}

void uart_write(uint8_t data)
{
    while (UART1_GetFlagStatus(UART1_FLAG_TXE) == RESET)
        ;
    UART1_SendData8(data);
    while (UART1_GetFlagStatus(UART1_FLAG_TC) == RESET)
        ;
}

void uart_writeMulti(uint8_t *data, int length)
{
    int i;
    for (i = 0; i < length; i++)
    {
        uart_write(data[i]);
        Delay(20);
    }
}