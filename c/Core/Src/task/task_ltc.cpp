//
// Created by gijsl on 3/15/2021.
//

#include <stdbool.h>
#include <segment_board.h>
#include <gpio.h>
#include <task/task_can_bus.h>
#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include "task/task_ltc.h"
#include "util.h"
#include "main.h"
#include "spi.h"
#include "ltc6811.h"
#include "ltc6811_config_register.h"
#include "shared_can_defs.h"

QueueHandle_t ltc_spi_tx_Q;
QueueHandle_t ltc_spi_rx_Q;

QueueHandle_t balance_cmd_Q;

void send_start_balance_cmd(void) {
    if (balance_cmd_Q == NULL) {
        return;
    }
    ltc_balance_cmd_t cmd = {0};
    cmd.do_balance = true;
    xQueueOverwrite(balance_cmd_Q, &cmd);
}

void send_stop_balance_cmd(void) {
    if (balance_cmd_Q == NULL) {
        return;
    }
    ltc_balance_cmd_t cmd = {0};
    cmd.do_balance = false;
    xQueueOverwrite(balance_cmd_Q, &cmd);
}

void task_ltc_setup(void) {
    ltc_spi_tx_Q = xQueueCreate(1, sizeof(ltc_spi_tx_request_t));
    ltc_spi_rx_Q = xQueueCreate(1, sizeof(ltc_spi_rx_t));

    balance_cmd_Q = xQueueCreate(1, sizeof(ltc_balance_cmd_t));
}

#define temp_measure_interval_ms 2500
TickType_t last_temp_measurement;

SegmentBoard *segments;

uint8_t balance_counter = 0;
uint8_t mux_state_cmd = 0;

#define balance_delay_ms 2000

CAN_MSG_BMS_status_1_T m_status_can_msg = {0};

[[noreturn]] void start_task_ltc(void *argument) {

    segments = static_cast<SegmentBoard *>(pvPortMalloc(sizeof(SegmentBoard) * num_segments));
    segments[0] = SegmentBoard();

    uint8_t led_state = 0;

    bool need_balance = false;
    bool want_balance = false;

    TickType_t last_wake = xTaskGetTickCount();

    while (1) {
        // step 1: read sensors
        // intentionally make the ltc's time out and turn off to clear config registers
        vTaskDelay(pdMS_TO_TICKS(2500));
        read_all_temp_routine();
        read_all_temp_routine();
        read_all_temp_routine();
        read_cvr_routine();
        read_cvr_routine();
        read_cvr_routine(); // multiple times because balancing screws with readings

        uint32_t total_volts = 0;
        for (int i = 0; i < num_segments; i++) {
            total_volts += segments[i].segment_voltage;
        }
        m_status_can_msg.pack_voltage = total_volts / 1000.0;
        send_can_msg(CAN_ID_BMS_status_1, &m_status_can_msg, sizeof(CAN_MSG_BMS_status_1_T));

        // step 2: calculate desired balancing
        if (balance_cmd_Q != NULL) {
            set_led_4(true);
            ltc_balance_cmd_t want_balance_cmd = {0};
            xQueuePeek(balance_cmd_Q, &want_balance_cmd, 0);
            want_balance = want_balance_cmd.do_balance;
        }

        uint16_t pack_lowest_cell_volt = 0xffff;
        uint16_t pack_highest_cell_volt = 0x0;
        uint16_t segment_lowest_volt;
        uint16_t segment_highest_volt;
        for (int i = 0; i < num_segments; i++) {
            segment_highest_volt = segments[i].highest_cell_volt();
            segment_lowest_volt = segments[i].lowest_cell_volt();

            if (segment_lowest_volt < pack_lowest_cell_volt) {
                pack_lowest_cell_volt = segment_lowest_volt;
            }
            if (segment_highest_volt > pack_highest_cell_volt) {
                pack_highest_cell_volt = segment_highest_volt;
            }
        }

        need_balance = pack_highest_cell_volt - pack_lowest_cell_volt > balance_volt_target;

        // step 3: set balancing
        if (need_balance && want_balance) {
            for (int i = 0; i < num_segments; i++) {
                segments[i].set_allow_balance(true);
                segments[i].calculate_balance(pack_lowest_cell_volt);
            }
        }
        write_config_routine();

        // step 4: wait for some time
        vTaskDelay(pdMS_TO_TICKS(balance_delay_ms / 2));
        wake_ltc_routine();
        vTaskDelay(pdMS_TO_TICKS(balance_delay_ms / 2));

        // step 5: unset balancing
        for (int i = 0; i < num_segments; i++) {
            segments[i].disable_balance();
        }
        write_config_routine();

        vTaskDelayUntil(&last_wake, pdMS_TO_TICKS(4000));
    }
}

void wake_ltc_routine(void) {
    for (int i = 0; i < num_segments; i++) {
        cs_low();
        delay_microseconds(240);
        cs_high();
    }
}

void write_config_routine(void) {
    wake_ltc_routine();

    delay_microseconds(500);

    taskENTER_CRITICAL();

    cs_low();
    cmd_68(COMMAND_WRCFGA);
    for (int i = 0; i < num_segments; i++) {
        write_68(segments[i].registers.CFGR, 6);
    }
    cs_high();

    taskEXIT_CRITICAL();
}

void write_comm_routine(void) {
    cs_low();
    delay_microseconds(100);
    cmd_68(COMMAND_WRCOMM);
    delay_microseconds(100);
    for (int i = 0; i < num_segments; i++) {
        write_68(segments[i].registers.COMM, 6);
    }
    delay_microseconds(100);
    cs_high();

    delay_microseconds(100);

    cs_low();
    clock_68(COMMAND_STCOMM, 3);
    cs_high();
}

void update_mux_routine(void) {
    mux_state_cmd++;
    if (mux_state_cmd >= 16) {
        mux_state_cmd = 0;
    }
    for (int i = 0; i < num_segments; i++) {
        // segment 0 is the one furthest away from master on the chain
        segments[i].set_mux(mux_state_cmd);
        segments[i].set_leds(mux_state_cmd);
    }
}

void read_cvr_routine(void) {

    taskENTER_CRITICAL();

    wake_ltc_routine();

    delay_microseconds(500);

    cs_low();
    cmd_68(COMMAND_ADCV_MD_DCP_CH | MD_7KHZ_NORMAL_3KHZ_MODE
           | DCP_DISCHARGE_NOT_PERMITTED | CH_ALL_CELLS);
    cs_high();

    delay_microseconds(2000);

    uint8_t buffer[8 * num_segments] = {0};

    cs_low();
    delay_microseconds(100);
    cmd_68(COMMAND_RDCVA);
    delay_microseconds(800);
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
}

void read_temp_routine(void) {
    cs_low();
    cmd_68(COMMAND_ADAX_MD_CHG | MD_27HKHZ_FAST_14KHZ_MODE | DCP_DISCHARGE_NOT_PERMITTED | CHG_GPIO_ALL);
    cs_high();

    delay_microseconds(5000);

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

void read_all_temp_routine(void) {
    // cycle through all mux states real quick and measure all temps
    taskENTER_CRITICAL();

    wake_ltc_routine();

    delay_microseconds(500);

    for (int mux = 0; mux < 16; mux++) {
        for (int seg = 0; seg < num_segments; seg++) {
            segments[seg].set_mux(mux);
            segments[seg].set_leds(mux);
        }
        write_comm_routine();
        read_temp_routine();

        delay_microseconds(1000);
    }

    taskEXIT_CRITICAL();
}