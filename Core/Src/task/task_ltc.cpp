//
// Created by gijsl on 3/15/2021.
//

#include <stdbool.h>
#include <segment_board.h>
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
    float raw_float = raw;
    return ((raw_float / 10.0f) - 400.0f) / 19.5f;
}

SegmentBoard* segments;

[[noreturn]] void start_task_ltc(void *argument) {

    segments = static_cast<SegmentBoard *>(pvPortMalloc(sizeof(SegmentBoard) * num_segments));
    segments[0] = SegmentBoard();

    uint8_t led_state = 0;
    uint8_t buffer[8 * num_segments] = {0};

    while (1) {
        segments[0].set_leds(led_state);
        segments[1].set_leds(led_state);

//        segment 0 is the one furthest away from master on the chain

        taskENTER_CRITICAL();

        cs_low();
        delay_microseconds(240);
        cs_high();

        delay_microseconds(100);

        cs_low();
        cmd_68(COMMAND_WRCFGA);
        write_68(segments[0].registers.CFGR, 6);
        write_68(segments[1].registers.CFGR, 6);
        cs_high();

        cs_low();
        cmd_68(COMMAND_WRCOMM);
        write_68(segments[0].registers.COMM, 6);
        write_68(segments[1].registers.COMM, 6);
        cs_high();

        delay_microseconds(100);

        cs_low();
        clock_68(COMMAND_STCOMM, 3);
        cs_high();

        delay_microseconds(100);

        cs_low();
        cmd_68(COMMAND_ADCV_MD_DCP_CH | MD_27HKHZ_FAST_14KHZ_MODE
               | DCP_DISCHARGE_PERMITTED | CH_ALL_CELLS);
        cs_high();

        delay_microseconds(1000);

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

        taskEXIT_CRITICAL();

        led_state++;
        if (led_state >= 8) {
            led_state = 0;
        }

        vTaskDelay(pdMS_TO_TICKS(500));
    }
}
