//
// Created by gijsl on 3/15/2021.
//

#include <stdbool.h>
#include <segment_board.h>
#include <gpio.h>
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



SegmentBoard* segments;

[[noreturn]] void start_task_ltc(void *argument) {

    segments = static_cast<SegmentBoard *>(pvPortMalloc(sizeof(SegmentBoard) * num_segments));
    segments[0] = SegmentBoard();

    uint8_t led_state = 0;
    uint8_t mux_state = 0;

    bool need_balance = false;
    bool want_balance = false;
    uint16_t diff_limit = 50;

    while (1) {
        if (need_balance) {
            led_state = 1;
        } else {
            led_state = 0;
        }





        taskENTER_CRITICAL();

        wake_ltc_routine();

        delay_microseconds(100);

        cs_low();
        cmd_68(COMMAND_ADAX_MD_CHG | MD_27HKHZ_FAST_14KHZ_MODE | DCP_DISCHARGE_NOT_PERMITTED | CHG_GPIO_ALL);
        cs_high();

        delay_microseconds(5000);

        read_temp_routine();

        delay_microseconds(100);

        mux_state++;
        if (mux_state >= 16) {
            mux_state = 0;
        }
        for (int i = 0; i < num_segments; i++) {
            // segment 0 is the one furthest away from master on the chain
            segments[i].set_leds(led_state);
            segments[i].set_mux(mux_state);
        }
        write_comm_routine();

        delay_microseconds(100);

        bool err_led_on = false;
        for (int i = 0; i < 24; i++) {
            if (segments[0].cell_temps[i] < 5 || segments[0].cell_temps[i] > 100) {
                err_led_on = true;
            }
        }

//        set_led_1(err_led_on);

//        cs_low();
//        cmd_68(COMMAND_ADCV_MD_DCP_CH | MD_7KHZ_NORMAL_3KHZ_MODE
//               | DCP_DISCHARGE_NOT_PERMITTED | CH_ALL_CELLS);
//        cs_high();
//
//        delay_microseconds(2000);
//
//        read_cvr_routine();
//
//        for (int i = 0; i < num_segments; i++) {
//            segments[i].calculate_balance();
//        }
//
//        cs_low();
//        cmd_68(COMMAND_WRCFGA);
//        for (int i = 0; i < num_segments; i++) {
//            write_68(segments[i].registers.CFGR, 6);
//        }
//        cs_high();

        taskEXIT_CRITICAL();

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void wake_ltc_routine(void) {
    cs_low();
    delay_microseconds(240);
    cs_high();
}

void write_comm_routine(void) {
    cs_low();
    cmd_68(COMMAND_WRCOMM);
    for (int i = 0; i < num_segments; i++) {
        write_68(segments[i].registers.COMM, 6);
    }
    cs_high();

    delay_microseconds(100);

    cs_low();
    clock_68(COMMAND_STCOMM, 3);
    cs_high();
}

void read_cvr_routine(void) {
    uint8_t buffer[8 * num_segments] = {0};

    cs_low();
    cmd_68(COMMAND_RDCVA);
    bool match = true;
    for (int i = 0; i < num_segments; i++) {
        match &= read_68(buffer + i * 8);
    }
    cs_high();
    if (match) {
        for (int i = 0; i < num_segments; i++) {
            segments[num_segments - 1 - i].update_volts(COMMAND_RDCVA, buffer + i * 8);
        }
    }

    cs_low();
    cmd_68(COMMAND_RDCVB);
    match = true;
    for (int i = 0; i < num_segments; i++) {
        match &= read_68(buffer + i * 8);
    }
    cs_high();
    if (match) {
        for (int i = 0; i < num_segments; i++) {
            segments[num_segments - 1 - i].update_volts(COMMAND_RDCVB, buffer + i * 8);
        }
    }

    cs_low();
    cmd_68(COMMAND_RDCVC);
    match = true;
    for (int i = 0; i < num_segments; i++) {
        match &= read_68(buffer + i * 8);
    }
    cs_high();
    if (match) {
        for (int i = 0; i < num_segments; i++) {
            segments[num_segments - 1 - i].update_volts(COMMAND_RDCVC, buffer + i * 8);
        }
    }

    cs_low();
    cmd_68(COMMAND_RDCVD);
    match = true;
    for (int i = 0; i < num_segments; i++) {
        match &= read_68(buffer + i * 8);
    }
    cs_high();
    if (match) {
        for (int i = 0; i < num_segments; i++) {
            segments[num_segments - 1 - i].update_volts(COMMAND_RDCVD, buffer + i * 8);
        }
    }
}

void read_temp_routine(void) {
    uint8_t buffer[8 * num_segments] = {0};

    cs_low();
    bool match = true;
    cmd_68(COMMAND_RDAUXA);
    delay_microseconds(100);
    for (int i = 0; i < num_segments; i++) {
        match &= read_68(buffer + i * 8);
    }
    cs_high();
    if (match) {
        for (int i = 0; i < num_segments; i++) {
            segments[num_segments - 1 - i].update_temps(buffer + i * 8);
        }
    }
}