//
// Created by gijsl on 3/28/2021.
//

#include "spi.h"
#include "main.h"
#include "util.h"
#include "cmsis_os.h"
#include "stm32f4xx_hal.h"
#include "ltc6811.h"

uint32_t PEC_match_count = 0;
uint32_t PEC_error_count = 0;

// 8 bytes of trash to pump out to make clock cycles for reading
uint8_t dummy[8] = {0x4C, 0x6F, 0x72, 0x61, 0x69, 0x6E, 0x65, 0x21};

void cmd_68(uint16_t cmd) {
    uint8_t spi_cmd[4];

    spi_cmd[0] = cmd >> 8;
    spi_cmd[1] = cmd & 0x00FF;

    uint16_t temp_pec = pec15(spi_cmd, 2);

    spi_cmd[2] = temp_pec >> 8;
    spi_cmd[3] = temp_pec & 0x00FF;

    HAL_SPI_Transmit(&hspi1, spi_cmd, 4, 100);
}

void write_68(uint8_t *data, uint8_t data_len) {
//    cmd_68(cmd);

    if (data_len > 0) {
        uint16_t temp_pec = pec15(data, 6);
        uint8_t pec[2];
        pec[0] = temp_pec >> 8;
        pec[1] = temp_pec & 0x00FF;

        HAL_SPI_Transmit(&hspi1, data, data_len, 100);
        HAL_SPI_Transmit(&hspi1, pec, 2, 100);
    }
}

void clock_68(uint16_t cmd, uint8_t len) {
    cmd_68(cmd);

    HAL_SPI_Transmit(&hspi1, dummy, len, 100);
}

bool read_68(uint8_t *data) {
    HAL_SPI_TransmitReceive(&hspi1, dummy, data, 8, 100);
    uint16_t received_pec = (data[7] & 0xff) + (data[6] << 8);

    if (received_pec == pec15(data, 6)) {
        PEC_match_count++;
        return true;
    } else {
        PEC_error_count++;
        return false;
    }
}


//bool read_cell_volts(uint16_t *data) {
//    bool success = true;
//    uint8_t data_internal[32];
//
//    cs_low();
//    cmd_68(COMMAND_RDCVA);
//    delay_microseconds(10);
//    success &= read_68(data_internal);
//    cs_high();
//
//    delay_microseconds(10);
//
//    cs_low();
//    cmd_68(COMMAND_RDCVB);
//    delay_microseconds(10);
//    success &= read_68(data_internal + 8);
//    cs_high();
//
//    delay_microseconds(10);
//
//    cs_low();
//    cmd_68(COMMAND_RDCVC);
//    delay_microseconds(10);
//    success &= read_68(data_internal + 16);
//    cs_high();
//
//    delay_microseconds(10);
//
//    cs_low();
//    cmd_68(COMMAND_RDCVD);
//    delay_microseconds(10);
//    success &= read_68(data_internal + 24);
//    cs_high();
//
//    if (success) {
//        data[0] = (data_internal[0] & 0xff) + (data_internal[1] << 8);
//        data[1] = (data_internal[2] & 0xff) + (data_internal[3] << 8);
//        data[2] = (data_internal[4] & 0xff) + (data_internal[5] << 8);
//
//        data[3] = (data_internal[8] & 0xff) + (data_internal[9] << 8);
//        data[4] = (data_internal[10] & 0xff) + (data_internal[11] << 8);
//        data[5] = (data_internal[12] & 0xff) + (data_internal[13] << 8);
//
//        data[6] = (data_internal[16] & 0xff) + (data_internal[17] << 8);
//        data[7] = (data_internal[18] & 0xff) + (data_internal[19] << 8);
//        data[8] = (data_internal[20] & 0xff) + (data_internal[21] << 8);
//
//        data[9] = (data_internal[24] & 0xff) + (data_internal[25] << 8);
//        data[10] = (data_internal[26] & 0xff) + (data_internal[27] << 8);
//        data[11] = (data_internal[28] & 0xff) + (data_internal[29] << 8);
//    }
//
//    return success;
//}

bool read_temps(uint16_t *data) {
    bool success = true;
    uint8_t data_internal[8];

    cs_low();
    cmd_68(COMMAND_RDAUXA);
    delay_microseconds(100);
    success &= read_68(data_internal);
    cs_high();

    if (success) {
        data[0] = (data_internal[0] & 0xff) + (data_internal[1] << 8);
        data[1] = (data_internal[2] & 0xff) + (data_internal[3] << 8);
    }

    return success;
}

//void make_comm_reg_bytes(uint8_t mux_state, uint8_t led_state, uint8_t out[8]) {
//    out[0] = 0x80 + mux_state;
//    out[1] = (led_state << 4) + 0x09;
//    out[2] = 0xF0;
//    out[3] = 0x09;
//    out[4] = 0xF0;
//    out[5] = 0x09;
//
//    uint16_t pec = pec15(out, 6);
//
//    out[6] = pec >> 8;
//    out[7] = pec & 0xFF;
//}
//
//void set_mux_cmd(uint8_t mux_state) {
//    uint8_t data[8] = {0};
//    make_comm_reg_bytes(mux_state, 0b0, data);
//
//    taskENTER_CRITICAL();
//
//    cs_low();
//    write_68(COMMAND_WRCOMM, data, 6);
//    cs_high();
//
//    delay_microseconds(100);
//
//    cs_low();
//    clock_68(COMMAND_STCOMM, 3);
//    cs_high();
//
//    taskEXIT_CRITICAL();
//}
