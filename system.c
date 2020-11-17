#include "system.h"

void clk_init()
{
	CLK_SYSCLKConfig(CLK_PRESCALER_HSIDIV1);

    CLK_PeripheralClockConfig(CLK_PERIPHERAL_TIMER1, ENABLE); // initialize clock for TIM1
    TIM1_TimeBaseInit(168, TIM1_COUNTERMODE_UP, 0xffff, 0);    // set counter frequency to about 10us
    TIM1->EGR |= 0X01;
    TIM1_Cmd(ENABLE);
}

uint32_t Delay(uint32_t cnt)
{
	TIM1_SetCounter(0x0000);
	volatile uint32_t i = TIM1_GetCounter();
	i = 0;
    int count = 0;
	while(i < cnt)
	{
		i = TIM1_GetCounter();
		count++;
	}
	return i;
}

/**float to char array. Because of the high code size cost of using sprintf,
  *here we introduce a simplified version of the conversion*/
void itoa(float input, uint8_t * result)
{
    int input_int = (int)(input * 100);
    uint8_t i =0;
    result[0] = '0';
    while(input_int)
    {
        result[4-i] = input_int % 10 + '0';
        input_int = input_int / 10;
        i++;
    }
}