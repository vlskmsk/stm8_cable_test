#include "mpl115a1.h"

float pres_data[NUM_SENSORS];

extern GPIO_TypeDef *cs_port[4];
extern GPIO_Pin_TypeDef cs_pin[4];
float calibration_coeff[4][4];

void coeff_read(uint32_t idx, uint8_t * coeff)
{
	uint8_t dummy;
	GPIO_WriteLow(cs_port[idx], cs_pin[idx]);
	dummy = spi_read(idx, A0_MSB_REG);
	coeff[8*idx + 0] = spi_read(idx, A0_LSB_REG);
	coeff[8*idx + 1] = spi_read(idx, B1_MSB_REG);
	coeff[8*idx + 2] = spi_read(idx, B1_LSB_REG);
	coeff[8*idx + 3] = spi_read(idx, B2_MSB_REG);
	coeff[8*idx + 4] = spi_read(idx, B2_LSB_REG);
	coeff[8*idx + 5] = spi_read(idx, C12_MSB_REG);
	coeff[8*idx + 6] = spi_read(idx, C12_LSB_REG);
	coeff[8*idx + 7] = (uint32_t)spi_read(idx, DUMMY_REG);
	GPIO_WriteHigh(cs_port[idx], cs_pin[idx]);
    calibration_coeff[0][idx] = (float)((coeff[8*idx] << 8 | coeff[8*idx + 1])) / 8.0;
    calibration_coeff[1][idx] = (float)((coeff[8*idx+ 2] << 8 | coeff[8*idx + 3])) / 8192.0;
    calibration_coeff[2][idx] = (float)((coeff[8*idx+ 4] << 8 | coeff[8*idx + 5])) / 16384.0;
    calibration_coeff[3][idx] = (float)(((coeff[8*idx+ 6] << 8 | coeff[8*idx + 7])) >> 2) / 4194304.0;
}

float sensor_read(uint32_t idx)
{
    uint16_t press_out_h, press_out_l, temp_out_h, temp_out_l, dummy;
    uint16_t press_out, temp_out;
    float val;
	GPIO_WriteLow(cs_port[idx], cs_pin[idx]);
	while (SPI_GetFlagStatus(SPI_FLAG_TXE) == RESET || SPI_GetFlagStatus(SPI_FLAG_BSY) == SET);
    SPI_SendData((uint8_t)(START_CONV_REG));
	while (SPI_GetFlagStatus(SPI_FLAG_TXE) == RESET || SPI_GetFlagStatus(SPI_FLAG_BSY) == SET);
	SPI_SendData((uint8_t)(DUMMY_REG));
	while (SPI_GetFlagStatus(SPI_FLAG_TXE) == RESET || SPI_GetFlagStatus(SPI_FLAG_BSY) == SET);
	GPIO_WriteHigh(cs_port[idx], cs_pin[idx]);
	Delay(1000);
	GPIO_WriteLow(cs_port[idx], cs_pin[idx]);
    
    dummy = spi_read(idx, PADC_MSB_REG);
	press_out_h = (uint16_t)spi_read(idx, PADC_LSB_REG);	
	press_out_l = (uint16_t)spi_read(idx, TADC_MSB_REG);
	temp_out_h = (uint16_t)spi_read(idx, TADC_LSB_REG);
	temp_out_l = (uint16_t)spi_read(idx, DUMMY_REG);
    press_out = (press_out_h << 8 | press_out_l) >> 6;
    temp_out = (temp_out_h << 8 | temp_out_l) >> 6;
	GPIO_WriteHigh(cs_port[idx], cs_pin[idx]);
    val = pressure_conv(idx, press_out, temp_out);
    return val;
}

/**a0 = calibration_coeff[[0]idx]
  *b1 = calibration_coeff[1][idx]
  *b2 = calibration_coeff[2][idx]
  *c12 = calibration_coeff[3][idx]*/
      
float pressure_conv(uint32_t idx, uint16_t Padc, uint16_t Tadc)
{
    float pcomp, pressure ;
    pcomp = calibration_coeff[0][idx]+(calibration_coeff[1][idx]+calibration_coeff[3][idx]*(Tadc))*(Padc)+calibration_coeff[2][idx]*(Tadc);
    if(pcomp <= 0)
    {
      pcomp = calibration_coeff[0][idx]+calibration_coeff[2][idx]*(Tadc);
    }
    pressure = pcomp * 65 / 1023 + 50;
    return pressure;
}

/*
gets pressure data from all 4 sensors in one shot. should be faster than sensor_read 
because it works during sensor conversion time instead of spinning in the delay
*/
void get_pressure()
{
	int idx = 0;
	for(idx=0; idx < NUM_SENSORS; idx++)
	{
		GPIO_WriteLow(cs_port[idx], cs_pin[idx]);
		while (SPI_GetFlagStatus(SPI_FLAG_TXE) == RESET || SPI_GetFlagStatus(SPI_FLAG_BSY) == SET);
		SPI_SendData((uint8_t)(START_CONV_REG));
		while (SPI_GetFlagStatus(SPI_FLAG_TXE) == RESET || SPI_GetFlagStatus(SPI_FLAG_BSY) == SET);
		SPI_SendData((uint8_t)(DUMMY_REG));
		while (SPI_GetFlagStatus(SPI_FLAG_TXE) == RESET || SPI_GetFlagStatus(SPI_FLAG_BSY) == SET);
		GPIO_WriteHigh(cs_port[idx], cs_pin[idx]);
	}
	Delay(150);	//optional small delay here, to account for the extra processing time sensors 1-3 get over sensor 0
	uint16_t press_out_h, press_out_l, temp_out_h, temp_out_l, dummy;
    uint16_t press_out, temp_out;
	for(idx = 0; idx < NUM_SENSORS; idx++)
	{
		if(idx > 0)
			Delay(100);		//delay may be necessary. must be less than 250 for there to be any runtime benefit to this method	
		GPIO_WriteLow(cs_port[idx], cs_pin[idx]);
		dummy = spi_read(idx, PADC_MSB_REG);
		press_out_h = (uint16_t)spi_read(idx, PADC_LSB_REG);	
		press_out_l = (uint16_t)spi_read(idx, TADC_MSB_REG);
		temp_out_h = (uint16_t)spi_read(idx, TADC_LSB_REG);
		temp_out_l = (uint16_t)spi_read(idx, DUMMY_REG);
		press_out = (press_out_h << 8 | press_out_l) >> 6;
		temp_out = (temp_out_h << 8 | temp_out_l) >> 6;
		GPIO_WriteHigh(cs_port[idx], cs_pin[idx]);
		
		pres_data[idx] = pressure_conv(idx, press_out, temp_out);
	}
	
}

