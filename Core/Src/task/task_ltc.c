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

float temp_conversion(uint16_t raw) {
    return ((raw / 10.0f) - 400.0f) / 19.5f;
}

_Noreturn void start_task_ltc(void *argument) {
    uint8_t mux_state = 0;
    uint16_t temp[2];
    uint16_t volts[12];
    float temperature[32];

    uint8_t config[8] = {CFGR0, CFGR1, CFGR2, CFGR3, CFGR4, CFGR5, 0, 0};

    uint8_t real_mux_states[16] = {
            0b0000,
            0b1000,
            0b0100,
            0b1100,
            0b0010,
            0b1010,
            0b0110,
            0b1110,
            0b0001,
            0b1001,
            0b0101,
            0b1101,
            0b0011,
            0b1011,
            0b0111,
            0b1111
    };

    set_mux_cmd(mux_state);


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

        delay_microseconds(2000);

        bool goodagain = read_temps(temp);

        if (goodagain) {
            temperature[mux_state] = temp_conversion(temp[0]);
            temperature[mux_state + 16] = temp_conversion(temp[1]);
        }

        mux_state++;
        if (mux_state >= 16) {
            mux_state = 0;
        }

        set_mux_cmd(real_mux_states[mux_state]);

        taskEXIT_CRITICAL();

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
