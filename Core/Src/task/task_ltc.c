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
#include "ltc6811_config_register.h"

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
    uint16_t temps[2];

    uint8_t config[8] = {CFGR0, CFGR1, CFGR2, CFGR3, CFGR4, CFGR5, 0, 0};

    uint8_t mux_bytes[8] = { 0 };

    while (1) {
        taskENTER_CRITICAL();


        cs_low();
        delay_microseconds(240);
        cs_high();

        delay_microseconds(100);

//        cs_low();
//        write_data(COMMAND_WRCFGA, config);
//        cs_high();

//        delay_microseconds(100);

        set_mux_cmd(3);

//        delay_microseconds(100);


//        cs_low();
//        send_cmd(COMMAND_ADCV_MD_DCP_CH | MD_27HKHZ_FAST_14KHZ_MODE
//                | DCP_DISCHARGE_NOT_PERMITTED | CH_ALL_CELLS);
//        cs_high();
//
//        delay_microseconds(200);
//
//        bool good = read_cell_volts(volts);
//
//        delay_microseconds(10);

        cs_low();
        send_cmd(COMMAND_ADAX_MD_CHG | MD_27HKHZ_FAST_14KHZ_MODE | DCP_DISCHARGE_NOT_PERMITTED | CHG_GPIO_ALL);
        cs_high();

        delay_microseconds(3000);

        bool goodagain = read_temps(temps);

        taskEXIT_CRITICAL();

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
