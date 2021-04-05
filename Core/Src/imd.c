//
// Created by gijsl on 3/28/2021.
//

#include "main.h"
#include "imd.h"
#include "stm32f4xx_hal.h"

bool is_IMD_faulted(void) {
    GPIO_PinState state;
    state = HAL_GPIO_ReadPin(D_BMS_fault_cmd_GPIO_Port, D_BMS_fault_cmd_Pin);

    return state == GPIO_PIN_SET;
}
