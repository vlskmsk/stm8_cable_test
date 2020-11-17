#include "stm8s_gpio.h"
#include "mpl115a1.h"
#include "uart.h"
#include "system.h"
#define DEBUG 			1
#define TEST            0   

GPIO_TypeDef *led_port = GPIOC;
GPIO_Pin_TypeDef led_pin = GPIO_PIN_3;
              
int main(void)
{
    int i = 0;
    uint8_t coeff_array[32] = {0};
    uint8_t tx_data_8;
    clk_init();
    spi_init();
    uart_init();
    GPIO_Init(led_port, led_pin, GPIO_MODE_OUT_PP_LOW_SLOW);

    for (i = 0; i < 4; i++)
	{
		coeff_read(i, coeff_array);
	}
    
    GPIO_WriteHigh(led_port,led_pin);
    Delay(10000);
    GPIO_WriteLow(led_port,led_pin);
    Delay(10000);
    GPIO_WriteHigh(led_port,led_pin); 
    Delay(10000);
    GPIO_WriteLow(led_port,led_pin);
    Delay(10000);
    GPIO_WriteHigh(led_port,led_pin); 
    Delay(10000);
    GPIO_WriteLow(led_port,led_pin);

	//GPIO_WriteHigh(led_port, led_pin);

    while (1)
    {
		get_pressure();
		
	//	uart_write('s');
		Delay(10); 
		for(i=0;i<NUM_SENSORS;i++)
		{
			uint16_t tx_data = (uint16_t)(pres_data[i] * 385.5);
            tx_data_8 = (tx_data & 0xFF00) >> 8; 
            if(tx_data_8 == 's')
            {
              tx_data_8 = 's' - 1;
            }
		//	uart_write(tx_data_8);
			Delay(1);
            tx_data_8 = (tx_data & 0x00FF); 
            if(tx_data_8 == 's')
            {
              tx_data_8 = 's' - 1;
            }
	//		uart_write(tx_data_8);
            
			Delay(1); //frame buffer, legacy from yangge
		}

//		uart_write('s');
//		Delay(10);
//		for(i=0;i<8;i++)
//		{
//			uart_write(uart_array[i]);
//			Delay(1);
//		}
		
		
//        if(TEST)
//        {
//            counter ++;
//            itoa(counter, &uart_array[15]);
//            uart_writeMulti("s000000000000000", 16);
//            uart_writeMulti(&uart_array[15], 5);
//        }
//        else
//        {
//            uart_write('s');
//            uart_writeMulti(uart_array, 20);
//        }
//        if(DEBUG)
//        {
//            Delay(200);
//        }
		
		asm("NOP");
    }

}