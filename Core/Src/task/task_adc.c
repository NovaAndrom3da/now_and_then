//
// Created by gijsl on 5/30/2021.
//

#include <stdint.h>
#include "FreeRTOS.h"
#include "queue.h"
#include "adc.h"
#include "task/task_adc.h"


uint32_t adc_dma_buffer[4];

uint32_t vbat = 0;
uint32_t vcar = 0;
uint32_t current_hs = 0;
uint32_t current_ls = 0;

uint8_t ready_for_next = 1;

_Noreturn void start_task_adc(void *argument) {
    HAL_ADC_Start(&hadc1);


    while (1) {
        if (ready_for_next > 0) {
            HAL_ADC_Start_DMA(&hadc1, adc_dma_buffer, 4);
            ready_for_next--;
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}


void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {
    vbat = adc_dma_buffer[0];
    vcar = adc_dma_buffer[1];
    current_hs = adc_dma_buffer[2];
    current_ls = adc_dma_buffer[3];

    ready_for_next++;
}

