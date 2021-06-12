//
// Created by gijsl on 5/30/2021.
//

#include <stdint.h>
#include <task/task_can_bus.h>
#include "FreeRTOS.h"
#include "queue.h"
#include "adc.h"
#include "task/task_adc.h"
#include "shared_can_defs.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_can.h"

QueueHandle_t ADC_Q;

uint32_t adc_dma_buffer[4];

[[noreturn]] void start_task_adc(void *argument) {
    HAL_ADC_Start(&hadc1);

    ADC_Q = xQueueCreate(1, sizeof(ADC_message));

    while (1) {
        if (HAL_DMA_GetState(hadc1.DMA_Handle) == HAL_DMA_STATE_READY) {
            HAL_ADC_Start_DMA(&hadc1, adc_dma_buffer, 4);
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}


void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {
    ADC_message m = {0};
    m.vbat = adc_dma_buffer[0];
    m.vcar = adc_dma_buffer[1];
    m.current_plus = adc_dma_buffer[2];
    m.current_minus = adc_dma_buffer[3];

    BaseType_t ret = xQueueOverwriteFromISR(ADC_Q, &m, NULL);

    return;
}
