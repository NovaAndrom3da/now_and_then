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
#include "ltc6811.h"

QueueHandle_t ltc_spi_tx_Q;
QueueHandle_t ltc_spi_rx_Q;

void task_ltc_setup(void) {
    ltc_spi_tx_Q = xQueueCreate(1, sizeof(ltc_spi_tx_request_t));
    ltc_spi_rx_Q = xQueueCreate(1, sizeof(ltc_spi_rx_t));
}


_Noreturn void start_task_ltc(void *argument) {

//    uint8_t data[8] = {0};
//    uint16_t test_pec;
//    uint16_t volt1, volt2, volt3;

    uint16_t volts[12];

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

        bool good = read_cell_volts(volts);

//        cs_low();
//        send_cmd(COMMAND_RDCVA);
//        delay_microseconds(10);
//        bool success = read_data(data);
//        cs_high();
//
//        if (success) {
//            volt1 = (data[0] & 0xff) + (data[1] << 8);
//            volt2 = (data[2] & 0xff) + (data[3] << 8);
//            volt3 = (data[4] & 0xff) + (data[5] << 8);
//        } else {
//            volt1 = 6969;
//            volt2 = 420;
//            volt3 = 123;
//        }





        taskEXIT_CRITICAL();

        vTaskDelay(10);
    }
}
