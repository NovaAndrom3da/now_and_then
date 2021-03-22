//
// Created by gijsl on 3/17/2021.
//

#include "bms_main.h"
#include "stm32f4xx_hal.h"
#include "can_bus_task.h"
#include "IMD_timer_task.h"

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