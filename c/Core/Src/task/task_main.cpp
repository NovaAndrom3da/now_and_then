//
// Created by gijsl on 3/17/2021.
//

#include "task/task_main.h"
#include "stm32f4xx_hal.h"
#include "task/task_can_bus.h"
#include "task/task_imd_timer.h"
#include <stdbool.h>
#include <gpio.h>
#include <util.h>
#include <stdlib.h>
#include <task/task_adc.h>
#include "shared_can_defs.h"
#include "subscribe.h"
#include "task/task_ltc.h"

uint32_t counter = 0;
uint32_t soc = 0;
TickType_t boot_time = 0;

ADC_message m_adc_message = {0};
CAN_MSG_BMS_current_T m_current = {0};
CAN_MSG_BMS_contactor_volt_delta_T m_contactor_volts = {0};

int16_t contactor_volts;
int16_t current_deciamps;

vehicle_state_t active_state = VEHICLE_STATE_STARTUP;
vehicle_state_t desired_state = VEHICLE_STATE_SHUTDOWN_OPEN;

[[noreturn]] void start_task_main(void *argument) {
    boot_time = xTaskGetTickCount();

    BaseType_t status = 0;
    IMD_message data = {0};

    TickType_t last_wake = xTaskGetTickCount();

    set_BMS_fault(false);

    CAN_MSG_GPIO_T pins = {0};
    CAN_MSG_DRIVE_STATE_T drive_state_msg = {0};

    set_final_close(false);

    while (1) {
        pins.bitfield.bms_fault = false;
        pins.bitfield.bms_fault_latch = is_bms_fault_latch();
        pins.bitfield.IMD_faulted = is_IMD_faulted();
        pins.bitfield.IMD_fault_latched = is_IMD_fault_latched();
        pins.bitfield.HS_closed = is_HS_closed();
        pins.bitfield.LS_closed = is_LS_closed();
        pins.bitfield.shutdown_closed = is_shutdown_closed();
        pins.bitfield.final_closed = is_final_closed();

        send_can_msg(CAN_ID_GPIO, &pins, sizeof(CAN_MSG_GPIO_T));

        drive_state_msg.vehicle_state = active_state;

        send_can_msg(CAN_ID_DRIVE_STATE, &drive_state_msg, sizeof(CAN_MSG_DRIVE_STATE_T));

        run_state_machine();

        if (ADC_Q != NULL) {
            ADC_message buf = {0};
            xQueuePeek(ADC_Q, &buf, 50);
            contactor_volts = adc_readings_to_volt_delta(buf.vcar, buf.vbat);
            current_deciamps = adc_readings_to_deciamps(buf.current_plus, buf.current_minus);
            m_current.amps = current_deciamps / 10.0;
            send_can_msg(CAN_ID_BMS_current, &m_current, sizeof(CAN_MSG_BMS_current_T));

            m_contactor_volts.volts = contactor_volts;
            send_can_msg(CAN_ID_BMS_contactor_volt_delta, &m_contactor_volts,
                         sizeof(CAN_MSG_BMS_contactor_volt_delta_T));
        }

        vTaskDelayUntil(&last_wake, pdMS_TO_TICKS(100));
    }
}

bool go_to_hard_fault() {
    return is_IMD_faulted() || is_IMD_fault_latched() || is_bms_fault_latch();
}

bool go_to_soft_fault() {
    // go to soft fault for over temp
    return false;
}

#define STARTUP_TIME_MS 2500

#define PRECHARGE_TIME_MS 5000
#define PRECHARGE_VOLT_DELTA_LIMIT 10
TickType_t precharge_start_time = 0;
bool precharge_started = false;

vehicle_state_t bla = VEHICLE_STATE_STARTUP;

void run_state_machine(void) {
    if (!is_shutdown_closed()) {
        // if shutdown is not closed, can not desire another state
        desired_state = VEHICLE_STATE_SHUTDOWN_OPEN;
    } else if (m_CAN_MSG_charger_control.charger_enabled == CHARGER_ENABLED_charge &&
               (xTaskGetTickCount() - charger_control_rx_time) < pdMS_TO_TICKS(1000)) {
        // want to charge
        desired_state = VEHICLE_STATE_BATTERY_CHARGE;
    } else if (m_CAN_MSG_VCU_switches.switch_bitfield.button1) {
        // want drive
        desired_state = VEHICLE_STATE_DRIVING;
    } else if (m_CAN_MSG_VCU_switches.switch_bitfield.button2 && active_state == VEHICLE_STATE_DRIVING) {
        // want to pause drive
        desired_state = VEHICLE_STATE_PRECHARGE_DONE;
    } else {
        // leave desired state alone
    }

    switch (active_state) {
        case VEHICLE_STATE_STARTUP:
            set_final_close(false);
            set_precharge(false);
            send_stop_balance_cmd();

            if (xTaskGetTickCount() > pdMS_TO_TICKS(STARTUP_TIME_MS) + boot_time) {
                // it's been long enough, go to next state
                active_state = VEHICLE_STATE_SHUTDOWN_OPEN;
            } else {
                // stay here for a few seconds at startup
                // we ignore faults for a bit because they might be fake
            }

            break;
        case VEHICLE_STATE_SHUTDOWN_OPEN:
            set_final_close(false);
            set_precharge(false);
            send_stop_balance_cmd();

            // if we are here, reject desired state and set to shutdown open
            desired_state = VEHICLE_STATE_SHUTDOWN_OPEN;

            if (go_to_hard_fault()) {
                active_state = VEHICLE_STATE_HARD_FAULT;
                break;
            } else if (is_shutdown_closed()) {
                active_state = VEHICLE_STATE_SHUTDOWN_CLOSED;
            } else {
                // stay here
            }
            break;
        case VEHICLE_STATE_SHUTDOWN_CLOSED:
            set_final_close(false);
            set_precharge(false);
            send_stop_balance_cmd();

            if (go_to_hard_fault()) {
                active_state = VEHICLE_STATE_HARD_FAULT;
                break;
            } else if (!is_shutdown_closed()) {
                active_state = VEHICLE_STATE_SHUTDOWN_OPEN;
            } else {
                // stay here
                if (desired_state == VEHICLE_STATE_DRIVING || desired_state == VEHICLE_STATE_BATTERY_CHARGE) {
                    active_state = VEHICLE_STATE_PRECHARGING;
                }
            }
            break;
        case VEHICLE_STATE_PRECHARGING:
            set_final_close(false);
            set_precharge(true);
            send_stop_balance_cmd();

            if (go_to_hard_fault()) {
                active_state = VEHICLE_STATE_HARD_FAULT;
                break;
            } else if (!is_shutdown_closed()) {
                // if the shutdown circuit is not closed, go to that state
                active_state = VEHICLE_STATE_SHUTDOWN_OPEN;
            } else if (desired_state == VEHICLE_STATE_DRIVING) {
                if (!precharge_started) {
                    precharge_start_time = xTaskGetTickCount();
                    precharge_started = true;
                }

                if (xTaskGetTickCount() > precharge_start_time + pdMS_TO_TICKS(PRECHARGE_TIME_MS) &&
                    abs(contactor_volts) < PRECHARGE_VOLT_DELTA_LIMIT) {
                    // precharge enough time and volt delta is low enough
                    active_state = VEHICLE_STATE_DRIVING;
                }
            } else if (desired_state == VEHICLE_STATE_BATTERY_CHARGE) {
                if (!precharge_started) {
                    precharge_start_time = xTaskGetTickCount();
                    precharge_started = true;
                }

                if (xTaskGetTickCount() > precharge_start_time + pdMS_TO_TICKS(PRECHARGE_TIME_MS)) {
                    // precharge enough time and volt delta is low enough
                    active_state = VEHICLE_STATE_BATTERY_CHARGE;
                }
            } else {
                // go back to shutdown closed state
                active_state = VEHICLE_STATE_SHUTDOWN_CLOSED;
            }
            break;
        case VEHICLE_STATE_PRECHARGE_DONE:
            set_final_close(true);
            set_precharge(true);
            send_stop_balance_cmd();

            if (go_to_hard_fault()) {
                active_state = VEHICLE_STATE_HARD_FAULT;
                break;
            } else if (!is_shutdown_closed()) {
                // check if shutdown opened, go to that state
                active_state = VEHICLE_STATE_SHUTDOWN_OPEN;
            } else if (!is_final_closed() || !is_HS_closed()) {
                active_state = VEHICLE_STATE_SHUTDOWN_CLOSED;
            } else if (desired_state == VEHICLE_STATE_DRIVING) {
                active_state = VEHICLE_STATE_DRIVING;
            } else {
                // stay here
            }
            break;
        case VEHICLE_STATE_DRIVING:
            set_final_close(true);
            set_precharge(true);
            send_stop_balance_cmd();

            if (go_to_hard_fault()) {
                active_state = VEHICLE_STATE_HARD_FAULT;
                break;
            } else if (!is_shutdown_closed()) {
                active_state = VEHICLE_STATE_SHUTDOWN_OPEN;
            } else {
                // keep driving
            }
            break;
        case VEHICLE_STATE_BATTERY_CHARGE:
            set_final_close(true);
            set_precharge(true);
            // make balance go
            send_start_balance_cmd();

            if (desired_state == VEHICLE_STATE_SHUTDOWN_OPEN) {
                active_state = VEHICLE_STATE_SHUTDOWN_OPEN;
            } else if ((xTaskGetTickCount() - charger_control_rx_time) > pdMS_TO_TICKS(2500) ||
                       desired_state != VEHICLE_STATE_BATTERY_CHARGE) {
                if (is_shutdown_closed()) {
                    active_state = VEHICLE_STATE_SHUTDOWN_CLOSED;
                } else {
                    active_state = VEHICLE_STATE_SHUTDOWN_OPEN;
                }
                desired_state = VEHICLE_STATE_SHUTDOWN_OPEN;
            }

            break;
        case VEHICLE_STATE_SOFT_FAULT:
            set_final_close(false);
            set_precharge(false);
            send_stop_balance_cmd();
            // sit here, wait until ok to go back to desired state
            break;
        case VEHICLE_STATE_HARD_FAULT:
        default:
            // just kinda sit here and don't leave until reboot
            set_final_close(false);
            set_precharge(false);
            set_BMS_fault(true);
            send_stop_balance_cmd();
            break;
    }

    if (active_state != VEHICLE_STATE_PRECHARGING) {
        precharge_started = false;
    }
}
