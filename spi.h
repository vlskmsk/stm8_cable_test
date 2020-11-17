#ifndef SPI_H
#define SPI_H

#include "stm8s.h"
#include "stm8s_spi.h"

void spi_init();
uint8_t spi_read(int sensor, uint8_t reg);
void spi_write(int sensor, uint8_t reg, uint8_t data);

#endif