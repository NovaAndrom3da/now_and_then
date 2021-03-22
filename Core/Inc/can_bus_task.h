//
// Created by gijsl on 3/17/2021.
//

#ifndef NOW_AND_THEN_CAN_BUS_TASK_H
#define NOW_AND_THEN_CAN_BUS_TASK_H

#include <stdint.h>
#include "FreeRTOS.h"
#include "queue.h"
#include "stm32f4xx_hal_can.h"

_Noreturn void start_task_can_bus(void *argument);

void setup_can_bus_task(void);

BaseType_t send_can_msg(uint32_t msg_id, void* data, uint8_t data_len);

typedef struct {
    CAN_TxHeaderTypeDef header;
    uint64_t data;
} CAN_tx_request;

extern QueueHandle_t CAN_tx_Q;

#endif //NOW_AND_THEN_CAN_BUS_TASK_H
