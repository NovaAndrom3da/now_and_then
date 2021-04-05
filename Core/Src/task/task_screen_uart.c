//
// Created by gijsl on 3/28/2021.
//

#include "task/task_screen_uart.h"

QueueHandle_t screen_uart_Q;

void task_screen_uart_setup(void) {
    screen_uart_Q = xQueueCreate(1, sizeof(screen_data_t));
}

_Noreturn void start_task_screen_uart(void *argument) {
    while (1) {

    }
}
