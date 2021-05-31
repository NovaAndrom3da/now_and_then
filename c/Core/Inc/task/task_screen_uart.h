//
// Created by gijsl on 3/28/2021.
//

#ifndef NOW_AND_THEN_TASK_SCREEN_UART_H
#define NOW_AND_THEN_TASK_SCREEN_UART_H

#include <stdint.h>
#include "FreeRTOS.h"
#include "queue.h"

_Noreturn void start_task_screen_uart(void *argument);

void task_screen_uart_setup(void);

typedef struct {
    void * placeholder;
} screen_data_t;

extern QueueHandle_t screen_uart_Q;

#endif //NOW_AND_THEN_TASK_SCREEN_UART_H
