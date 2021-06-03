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


uint32_t adc_dma_buffer[4];

uint16_t vbat = 0;
uint16_t vcar = 0;
uint16_t current_hs = 0;
uint16_t current_ls = 0;

uint8_t ready_for_next = 1;

_Noreturn void start_task_adc(void *argument) {
    HAL_ADC_Start(&hadc1);

    CAN_MSG_BMS_analog_in_T analog_in = { 0 };

    while (1) {
        if (1 > 0) {
            HAL_ADC_Start_DMA(&hadc1, adc_dma_buffer, 4);
            ready_for_next--;
        }

        analog_in.Vbat = vbat;
        analog_in.Vcar = vcar;
        analog_in.current_neg = current_ls;
        analog_in.current_pos = current_hs;

        send_can_msg(CAN_ID_BMS_analog_in, &analog_in, sizeof(CAN_MSG_BMS_analog_in_T));

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}


void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {
    vbat = adc_dma_buffer[0] & 0xFFFF;
    vcar = adc_dma_buffer[1] & 0xFFFF;
    current_hs = adc_dma_buffer[2] & 0xFFFF;
    current_ls = adc_dma_buffer[3] & 0xFFFF;

    ready_for_next++;
}

