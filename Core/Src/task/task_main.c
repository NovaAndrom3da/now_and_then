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


    while (1) {
        status = xQueueReceive(IMD_Q, &data, portMAX_DELAY);
        if (status == pdPASS) {
            send_can_msg(0x69, &data.frequency, sizeof(data.frequency));
        }
    }
}