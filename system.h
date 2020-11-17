#ifndef SYSTEM_H
#define SYSTEM_H

#include "stm8s.h"
#include <stdlib.h>
#include "stm8s_clk.h"
#include "stm8s_tim1.h"

void clk_init();
uint32_t Delay(uint32_t cnt);
void itoa(float input, uint8_t * result);

#endif