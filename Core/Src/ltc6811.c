//
// Created by gijsl on 3/28/2021.
//

#include "spi.h"
#include "main.h"
#include "util.h"
#include "task/task_ltc.h"
#include "cmsis_os.h"
#include "stm32f4xx_hal.h"
#include <stdbool.h>
#include "ltc6811.h"

void cs_low(void) {
    HAL_GPIO_WritePin(spi_cs_temp_GPIO_Port, spi_cs_temp_Pin, GPIO_PIN_RESET);
}

void cs_high(void) {
    HAL_GPIO_WritePin(spi_cs_temp_GPIO_Port, spi_cs_temp_Pin, GPIO_PIN_SET);
}

void send_cmd(uint16_t cmd) {
    uint8_t spi_cmd[4];

    spi_cmd[0] = cmd >> 8;
    spi_cmd[1] = cmd & 0x00FF;

    uint16_t temp_pec = pec15(spi_cmd, 2);

    spi_cmd[2] = temp_pec >> 8;
    spi_cmd[3] = temp_pec & 0x00FF;

    HAL_SPI_Transmit(&hspi1, spi_cmd, 4, 100);
}
