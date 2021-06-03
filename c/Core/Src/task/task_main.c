//
// Created by gijsl on 3/17/2021.
//

#include "task/task_main.h"
#include "stm32f4xx_hal.h"
#include "task/task_can_bus.h"
#include "task/task_imd_timer.h"
#include <stdbool.h>
#include <gpio.h>
#include "shared_can_defs.h"


uint32_t counter = 0;

_Noreturn void start_task_main(void *argument) {
    BaseType_t status = 0;
    IMD_message data = {0};
    uint8_t dummy = 0x13;

    TickType_t last_wake = xTaskGetTickCount();

    bool on = true;

    set_BMS_fault(false);
    CAN_MSG_placeholder_T spam = { 0 };
    spam.fake = 5;

    CAN_MSG_GPIO_T pins = { 0 };

    set_final_close(false);

    while (1) {
//        status = xQueueReceive(IMD_Q, &data, 0);
//        if (status == pdPASS) {
//            send_can_msg(0x69, &data.frequency, sizeof(data.frequency));
//        }

        pins.bitfield.bms_fault = false;
        pins.bitfield.bms_fault_latch = is_bms_fault_latch();
        pins.bitfield.IMD_faulted = is_IMD_faulted();
        pins.bitfield.IMD_fault_latched = is_IMD_fault_latched();
        pins.bitfield.HS_closed = is_HS_closed();
        pins.bitfield.LS_closed = is_LS_closed();
        pins.bitfield.shutdown_closed = is_shutdown_closed();
        pins.bitfield.final_closed = is_final_closed();

        send_can_msg(CAN_ID_GPIO, &pins, sizeof(CAN_MSG_GPIO_T));

        set_final_close(true);
        set_precharge(true);


        set_led_4(is_shutdown_closed());

        vTaskDelayUntil(&last_wake, pdMS_TO_TICKS(1000));
    }
}