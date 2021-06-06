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
#include "shared_can_defs.h"
#include "subscribe.h"

uint32_t counter = 0;
uint32_t soc = 0;
TickType_t boot_time = 0;

_Noreturn void start_task_main(void *argument) {
    boot_time = xTaskGetTickCount();

    BaseType_t status = 0;
    IMD_message data = {0};

    TickType_t last_wake = xTaskGetTickCount();

    set_BMS_fault(false);
    CAN_MSG_placeholder_T spam = {0};
    spam.fake = 5;

    CAN_MSG_GPIO_T pins = {0};
    CAN_MSG_DRIVE_STATE_T drive_state_msg = { 0 };

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

        drive_state_msg.state = vehicle_state;

        send_can_msg(CAN_ID_DRIVE_STATE, &drive_state_msg, sizeof(CAN_MSG_DRIVE_STATE_T));

        run_state_machine();

        vTaskDelayUntil(&last_wake, pdMS_TO_TICKS(1000));
    }
}

uint8_t vehicle_state = VEHICLE_STATE_STARTUP;
uint8_t desired_state = VEHICLE_STATE_SHUTDOWN_OPEN;

bool go_to_fault() {
    return is_IMD_faulted() || is_IMD_fault_latched() || is_bms_fault_latch();
}

#define STARTUP_TIME_MS 2500

#define PRECHARGE_TIME_MS 5000
#define PRECHARGE_VOLT_DELTA_LIMIT 10
TickType_t precharge_start_time = 0;
bool precharge_started = false;

void run_state_machine(void) {
    switch (vehicle_state) {
        case VEHICLE_STATE_STARTUP:
            set_final_close(false);
            set_precharge(false);

            if (xTaskGetTickCount() > pdMS_TO_TICKS(STARTUP_TIME_MS) + boot_time) {
                // it's been long enough, go to next state
                desired_state = VEHICLE_STATE_SHUTDOWN_OPEN;
                vehicle_state = VEHICLE_STATE_SHUTDOWN_OPEN;
            } else {
                // stay here for a few seconds at startup
                // we ignore faults for a bit because they might be fake
            }

            break;
        case VEHICLE_STATE_SHUTDOWN_OPEN:
            set_final_close(false);
            set_precharge(false);

            if (go_to_fault()) {
                vehicle_state = VEHICLE_STATE_FAULTED;
                break;
            } else if (is_shutdown_closed()) {
                desired_state = VEHICLE_STATE_SHUTDOWN_CLOSED;
                vehicle_state = VEHICLE_STATE_SHUTDOWN_CLOSED;
            } else if (m_CAN_MSG_VCU_switches.switch_bitfield.button1) {
                // if want drive but shutdown not closed, deny and stay here
            } else {
                // stay here
            }
            break;
        case VEHICLE_STATE_SHUTDOWN_CLOSED:
            set_final_close(false);
            set_precharge(false);

            if (go_to_fault()) {
                vehicle_state = VEHICLE_STATE_FAULTED;
                break;
            } else if (!is_shutdown_closed()) {
                desired_state = VEHICLE_STATE_SHUTDOWN_OPEN;
                vehicle_state = VEHICLE_STATE_SHUTDOWN_OPEN;
            } else if (m_CAN_MSG_VCU_switches.switch_bitfield.button1) {
                // if want drive
                desired_state = VEHICLE_STATE_DRIVING;
                vehicle_state = VEHICLE_STATE_PRECHARGING;
            } else {
                // stay here
            }
            break;
        case VEHICLE_STATE_PRECHARGING:
            set_final_close(false);
            set_precharge(true);

            if (go_to_fault()) {
                vehicle_state = VEHICLE_STATE_FAULTED;
                break;
            } else if (!is_shutdown_closed()) {
                // if the shutdown circuit is not closed, go to that state
                desired_state = VEHICLE_STATE_SHUTDOWN_OPEN;
                vehicle_state = VEHICLE_STATE_SHUTDOWN_OPEN;
            } else if (desired_state == VEHICLE_STATE_DRIVING) {
                if (!precharge_started) {
                    precharge_start_time = xTaskGetTickCount();
                    precharge_started = true;
                }

                if (xTaskGetTickCount() > precharge_start_time + pdMS_TO_TICKS(PRECHARGE_TIME_MS) &&
                    adc_readings_to_volt_delta(0, 0) < PRECHARGE_VOLT_DELTA_LIMIT) {
                    // precharge enough time and volt delta is low enough
                    desired_state = VEHICLE_STATE_DRIVING;
                    vehicle_state = VEHICLE_STATE_DRIVING;

                }
            } else {
                // go back to shutdown closed state
                desired_state = VEHICLE_STATE_SHUTDOWN_CLOSED;
                vehicle_state = VEHICLE_STATE_SHUTDOWN_CLOSED;
            }
            break;
        case VEHICLE_STATE_PRECHARGE_DONE:
            set_final_close(false);
            set_precharge(true);

            if (go_to_fault()) {
                vehicle_state = VEHICLE_STATE_FAULTED;
                break;
            } else if (!is_shutdown_closed()) {
                // check if shutdown opened, go to that state
                vehicle_state = VEHICLE_STATE_SHUTDOWN_OPEN;
                desired_state = VEHICLE_STATE_SHUTDOWN_OPEN;
            } else if (!is_final_closed() || !is_HS_closed()) {
                vehicle_state = VEHICLE_STATE_SHUTDOWN_CLOSED;
                desired_state = VEHICLE_STATE_SHUTDOWN_CLOSED;
            } else if (m_CAN_MSG_VCU_switches.switch_bitfield.button1) {
                // push button to drive, do precharge if needed
                // HS contactor is already closed, go straight to drive
                vehicle_state = VEHICLE_STATE_DRIVING;
                desired_state = VEHICLE_STATE_DRIVING;
            } else {
                // stay here
            }
            break;
        case VEHICLE_STATE_DRIVING:
            set_final_close(true);
            set_precharge(true);

            if (go_to_fault()) {
                vehicle_state = VEHICLE_STATE_FAULTED;
                break;
            } else if (!is_shutdown_closed()) {
                desired_state = VEHICLE_STATE_SHUTDOWN_OPEN;
                vehicle_state = VEHICLE_STATE_SHUTDOWN_OPEN;
            } else if (m_CAN_MSG_VCU_switches.switch_bitfield.button2) {
                // push button to stop drive, go to precharge done
                desired_state = VEHICLE_STATE_PRECHARGE_DONE;
                vehicle_state = VEHICLE_STATE_PRECHARGE_DONE;
            } else {
                // keep driving
            }
            break;
        case VEHICLE_STATE_FAULTED:
        default:
            // just kinda sit here and don't leave until reboot
            set_final_close(false);
            set_precharge(false);
            set_BMS_fault(true);
            break;
    }

    if (vehicle_state != VEHICLE_STATE_PRECHARGING) {
        precharge_started = false;
    }
}
