//
// Created by gijsl on 3/17/2021.
//

#include "task/task_main.h"
#include "stm32f4xx_hal.h"
#include "task/task_can_bus.h"
#include "task/task_imd_timer.h"

uint32_t counter = 0;

_Noreturn void start_task_main(void *argument) {
    BaseType_t status = 0;
    IMD_message data = {0};
    uint8_t dummy = 0x13;

    TickType_t last_wake = xTaskGetTickCount();

    while (1) {
        status = xQueueReceive(IMD_Q, &data, 0);
        if (status == pdPASS) {
            send_can_msg(0x69, &data.frequency, sizeof(data.frequency));
        }

        send_can_msg(0x01, &dummy, 1);



        vTaskDelayUntil(&last_wake, pdMS_TO_TICKS(100));
    }
}