#ifndef MPL115A1_H
#define MPL115A1_H

#include "spi.h"
#include "system.h"

#define NUM_SENSORS 4

#define START_CONV_REG	0x24
#define PADC_MSB_REG	0x80
#define PADC_LSB_REG	0x82
#define TADC_MSB_REG	0x84
#define TADC_LSB_REG	0x86
#define A0_MSB_REG		0x88
#define A0_LSB_REG		0x8A
#define B1_MSB_REG		0x8C
#define B1_LSB_REG		0x8E
#define B2_MSB_REG		0x90
#define B2_LSB_REG		0x92
#define C12_MSB_REG		0x94
#define C12_LSB_REG		0x96
#define DUMMY_REG		0x00

/*
global array containing the pressure data, formatted in a 16 bit format
*/
extern float pres_data[NUM_SENSORS];

void coeff_read(uint32_t idx, uint8_t * coeff);
float sensor_read(uint32_t idx);
float pressure_conv(uint32_t idx, uint16_t Padc, uint16_t Tadc);
void get_pressure();


#endif