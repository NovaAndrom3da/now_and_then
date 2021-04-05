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

void write_data(uint16_t cmd, uint8_t data[]) {

}

uint8_t dummy[8] = {0x69, 0x42, 0x13, 0x23, 0x37, 0x73, 0x55, 0x68};

bool read_data(uint8_t* data) {
    HAL_SPI_TransmitReceive(&hspi1, dummy, data, 8, 100);
    uint16_t received_pec = (data[7] & 0xff) + (data[6] << 8);

    return received_pec == pec15(data, 6);
}

bool read_cell_volts(uint16_t* data) {
    bool success = true;
    uint8_t data_internal[32];

    cs_low();
    send_cmd(COMMAND_RDCVA);
    delay_microseconds(10);
    success &= read_data(data_internal);
    cs_high();

    delay_microseconds(10);

    cs_low();
    send_cmd(COMMAND_RDCVB);
    delay_microseconds(10);
    success &= read_data(data_internal + 8);
    cs_high();

    delay_microseconds(10);

    cs_low();
    send_cmd(COMMAND_RDCVC);
    delay_microseconds(10);
    success &= read_data(data_internal + 16);
    cs_high();

    delay_microseconds(10);

    cs_low();
    send_cmd(COMMAND_RDCVD);
    delay_microseconds(10);
    success &= read_data(data_internal + 24);
    cs_high();

    if (success) {
        data[0] = (data_internal[0] & 0xff) + (data_internal[1] << 8);
        data[1] = (data_internal[2] & 0xff) + (data_internal[3] << 8);
        data[2] = (data_internal[4] & 0xff) + (data_internal[5] << 8);

        data[3] = (data_internal[8] & 0xff) + (data_internal[9] << 8);
        data[4] = (data_internal[10] & 0xff) + (data_internal[11] << 8);
        data[5] = (data_internal[12] & 0xff) + (data_internal[13] << 8);

        data[6] = (data_internal[16] & 0xff) + (data_internal[17] << 8);
        data[7] = (data_internal[18] & 0xff) + (data_internal[19] << 8);
        data[8] = (data_internal[20] & 0xff) + (data_internal[21] << 8);

        data[9] = (data_internal[24] & 0xff) + (data_internal[25] << 8);
        data[10] = (data_internal[26] & 0xff) + (data_internal[27] << 8);
        data[11] = (data_internal[28] & 0xff) + (data_internal[29] << 8);
    }

    return success;
}