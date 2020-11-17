#include "spi.h"

//Left
GPIO_TypeDef *cs_port[4] = {GPIOD, GPIOD, GPIOD, GPIOA};
GPIO_Pin_TypeDef cs_pin[4] = {GPIO_PIN_3, GPIO_PIN_4, GPIO_PIN_6, GPIO_PIN_2};

//Right
//GPIO_TypeDef *cs_port[4] = {GPIOD, GPIOD, GPIOD, GPIOC};
//GPIO_Pin_TypeDef cs_pin[4] = {GPIO_PIN_3, GPIO_PIN_4, GPIO_PIN_2, GPIO_PIN_4};

//Top
//GPIO_TypeDef *cs_port[4] = {GPIOD, GPIOD, GPIOD, GPIOD};
//GPIO_Pin_TypeDef cs_pin[4] = {GPIO_PIN_3, GPIO_PIN_4, GPIO_PIN_6, GPIO_PIN_2};

GPIO_TypeDef *sck_port = GPIOC;
GPIO_Pin_TypeDef sck_pin = GPIO_PIN_5;

GPIO_TypeDef *mosi_port = GPIOC;
GPIO_Pin_TypeDef mosi_pin = GPIO_PIN_6;

GPIO_TypeDef *miso_port = GPIOC;
GPIO_Pin_TypeDef miso_pin = GPIO_PIN_7;

void spi_init()
{
    GPIO_DeInit(GPIOA);
    GPIO_DeInit(GPIOD);
    SPI_DeInit();
    int i = 2;
    CLK_PeripheralClockConfig(CLK_PERIPHERAL_SPI, ENABLE); // initialize clock for SPI
    SPI_Init(SPI_FIRSTBIT_MSB,                             // the sensor is MSB first
             SPI_BAUDRATEPRESCALER_16,                      //
             SPI_MODE_MASTER,                              // the stm8 is the master
             SPI_CLOCKPOLARITY_LOW,                       // the sensor require high idle clock
             SPI_CLOCKPHASE_1EDGE,                         // sdi and sdo are driven at the falling edge of SPC and should be captured at the rising edge of spc
             SPI_DATADIRECTION_2LINES_FULLDUPLEX,          // two line, have both transmitter and receiver
             SPI_NSS_SOFT,
             0x07);
    for (i = 0; i < 4; i++)
    {
        GPIO_Init(cs_port[i], cs_pin[i], GPIO_MODE_OUT_PP_HIGH_FAST);
    }
    SPI_Cmd(ENABLE);
}

uint8_t spi_read(int sensor, uint8_t reg)
{
    uint8_t val = 0;
	while (SPI_GetFlagStatus(SPI_FLAG_TXE) == RESET || SPI_GetFlagStatus(SPI_FLAG_BSY) == SET);
    SPI_SendData((uint8_t)(reg));
	while (SPI_GetFlagStatus(SPI_FLAG_TXE) == RESET || SPI_GetFlagStatus(SPI_FLAG_BSY) == SET);
    SPI_SendData(0x00);
    while (SPI_GetFlagStatus(SPI_FLAG_RXNE) == RESET);
    val = SPI_ReceiveData(); // Read while having received data in buffer
    while (SPI_GetFlagStatus(SPI_FLAG_BSY) == SET || SPI_GetFlagStatus(SPI_FLAG_TXE) == RESET);
    return val;
}

void spi_write(int sensor, uint8_t reg, uint8_t data)
{
    GPIO_WriteLow(cs_port[sensor], cs_pin[sensor]);
    SPI_SendData(reg | 0x00);
    while (SPI_GetFlagStatus(SPI_FLAG_TXE) == RESET || SPI_GetFlagStatus(SPI_FLAG_BSY) == SET)
        ;
    SPI_SendData(data);
    while (SPI_GetFlagStatus(SPI_FLAG_TXE) == RESET || SPI_GetFlagStatus(SPI_FLAG_BSY) == SET)
        ;
    GPIO_WriteHigh(cs_port[sensor], cs_pin[sensor]);
}

void assert_failed(uint8_t *file, uint32_t line)
{
    while (1)
        ;
}