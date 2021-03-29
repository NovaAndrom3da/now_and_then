#include <argz.h>
//
// Created by gijsl on 3/15/2021.
//

#include <stdbool.h>
#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include "task/task_ltc.h"
#include "util.h"
#include "main.h"
#include "spi.h"

void cs_low(void) {
    HAL_GPIO_WritePin(spi_cs_temp_GPIO_Port, spi_cs_temp_Pin, GPIO_PIN_RESET);
}

void cs_high(void) {
    HAL_GPIO_WritePin(spi_cs_temp_GPIO_Port, spi_cs_temp_Pin, GPIO_PIN_SET);
}


void send_cmd(uint16_t cmd) {
    uint8_t temp[2];
    temp[0] = cmd >> 8;
    temp[1] = cmd & 0x00FF;
    uint16_t temp_pec = pec15(temp, 2);

    uint8_t temp_pec_8[2];
    temp_pec_8[0] = temp_pec >> 8;
    temp_pec_8[1] = temp_pec & 0x00FF;

    HAL_SPI_Transmit(&hspi1, temp, 1, 100);
    HAL_SPI_Transmit(&hspi1, temp + 1, 1, 100);
    HAL_SPI_Transmit(&hspi1, temp_pec_8, 1, 100);
    HAL_SPI_Transmit(&hspi1, temp_pec_8 + 1, 1, 100);
}

_Noreturn void start_ltc_test(void *argument) {

    uint8_t data[8] = {0};
    uint16_t test_pec;
    uint8_t dummy[8] = {0x69, 0x42, 0x13, 0x23, 0x37, 0x73, 0x55, 0x68};
    uint16_t volt1, volt2, volt3;

    while (1) {
        taskENTER_CRITICAL();
        cs_low();
        delay_microseconds(240);
        cs_high();
        delay_microseconds(100);
        cs_low();
        send_cmd(
                COMMAND_ADCV_MD_DCP_CH | MD_7KHZ_NORMAL_3KHZ_MODE
                | DCP_DISCHARGE_NOT_PERMITTED | CH_ALL_CELLS);
        cs_high();
        delay_microseconds(100);
        cs_low();
        send_cmd(COMMAND_RDCVA);
        delay_microseconds(10);
        HAL_SPI_TransmitReceive(&hspi1, dummy, data, 8, 1000);
        test_pec = pec15(data, 6);
        uint16_t trash = (data[7] & 0xff) + (data[6] << 8);
        if (test_pec == trash) {
            volt1 = (data[0] & 0xff) + (data[1] << 8);
            volt2 = (data[2] & 0xff) + (data[3] << 8);
            volt3 = (data[4] & 0xff) + (data[5] << 8);
        } else {
            volt1 = 6969;
            volt2 = 420;
            volt3 = 123;
        }
        cs_high();
        taskEXIT_CRITICAL();

        vTaskDelay(10);
    }
}