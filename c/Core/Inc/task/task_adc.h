//
// Created by gijsl on 5/30/2021.
//

#ifndef NOW_AND_THEN_TASK_ADC_H
#define NOW_AND_THEN_TASK_ADC_H

#include "stm32f4xx_hal_adc.h"

extern QueueHandle_t ADC_Q;

typedef struct {
    uint32_t vbat;
    uint32_t vcar;
    uint32_t current_plus;
    uint32_t current_minus;
} ADC_message;

_Noreturn void start_task_adc(void *argument);

#endif //NOW_AND_THEN_TASK_ADC_H
