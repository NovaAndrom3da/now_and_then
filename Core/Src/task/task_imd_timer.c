//
// Created by gijsl on 3/17/2021.
//


#include "tim.h"
#include "task/task_imd_timer.h"

QueueHandle_t IMD_Q;

void create_IMD_queue(void )
{
    if (IMD_Q != NULL){
        return;
    }
    IMD_Q = xQueueCreate(1, sizeof(IMD_message));
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim) {
    if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2) {
        /* Get the Input Capture value */
        uint32_t uwIC2Value = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_2);

        if (uwIC2Value != 0) {
            IMD_message m = { 0 };
            m.duty_cycle = (HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1) * 100) / uwIC2Value;
            m.frequency = (HAL_RCC_GetHCLKFreq() / 2) / uwIC2Value;

            xQueueSendToFrontFromISR(IMD_Q, &m, NULL);
        } else {
        }
    }
}