/*!
  ltc681x hardware library
@verbatim
  This library contains all of the hardware dependant functions used by the bms
  code
@endverbatim

Copyright 2018(c) Analog Devices, Inc.

All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
 - Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
 - Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in
   the documentation and/or other materials provided with the
   distribution.
 - Neither the name of Analog Devices, Inc. nor the names of its
   contributors may be used to endorse or promote products derived
   from this software without specific prior written permission.
 - The use of this software may or may not infringe the patent rights
   of one or more patent holders.  This license does not release you
   from the requirement that you obtain separate licenses from these
   patent holders to use this software.
 - Use of the software either in source or binary form, must be run
   on or directly connected to an Analog Devices Inc. component.

THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

Copyright 2017 Linear Technology Corp. (LTC)
*/
#include <stdint.h>
#include "bms_hardware.h"
#include "gpio.h"
#include "spi.h"
#include "cmsis_os.h"

void spi_cs_low() {
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_10, GPIO_PIN_RESET);
}

void spi_cs_high() {
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_10, GPIO_PIN_SET);
}

void delay_u(uint16_t micro) {
    delay_m(1);
}

void delay_m(uint16_t milli) {
    vTaskDelay(pdMS_TO_TICKS(milli));
}

/*
 * Writes an array of bytes out of the SPI port
 * @param len Number of bytes to be written on the SPI port
 * @param data Array of bytes to be written on the SPI port
*/
void spi_write_array(uint8_t len, uint8_t data[]) {
    HAL_SPI_Transmit(&hspi1, data, len, 1);
//    for (uint8_t i = 0; i < len; i++) {

//    }
}

/*
 * Writes and read a set number of bytes using the SPI port.
 * @param tx_Data array of data to be written on SPI port
 * @param tx_len length of the tx data array
 * @param rx_data array that will store the data read by the SPI port
 * @param rx_len number of bytes to be read from the SPI port
*/

void spi_write_read(uint8_t tx_Data[], uint8_t tx_len, uint8_t *rx_data, uint8_t rx_len) {
    spi_write_array(tx_len, tx_Data);

    for (uint8_t i = 0; i < rx_len; i++) {
        rx_data[i] = spi_read_byte();
    }

}

uint8_t spi_read_byte() {
    uint8_t data;
    HAL_SPI_Receive(&hspi1, &data, 1, 10000);
    return (data);
}
