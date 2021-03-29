//
// Created by gijsl on 3/17/2021.
//

#ifndef NOW_AND_THEN_TASK_IMD_TIMER_H
#define NOW_AND_THEN_TASK_IMD_TIMER_H

#include <stdint.h>
#include "FreeRTOS.h"
#include "queue.h"
#include "stm32f4xx_hal_tim.h"

typedef struct {
    uint32_t frequency;
    uint32_t duty_cycle;
} IMD_message;

extern QueueHandle_t IMD_Q;

void create_IMD_queue(void);

#endif //NOW_AND_THEN_TASK_IMD_TIMER_H
