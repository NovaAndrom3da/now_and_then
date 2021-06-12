//
// Created by gijsl on 3/17/2021.
//

#ifndef NOW_AND_THEN_TASK_CAN_BUS_H
#define NOW_AND_THEN_TASK_CAN_BUS_H

#include <stdint.h>
#include "FreeRTOS.h"
#include "queue.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_can.h"

#ifdef __cplusplus
extern "C" {
#endif

[[noreturn]] void start_task_can_bus(void *argument);

void setup_can_bus_task(void);

BaseType_t send_can_msg(uint32_t msg_id, void* data, uint8_t data_len);

typedef struct {
    CAN_TxHeaderTypeDef header;
    uint64_t data;
} CAN_tx_request_t;

extern QueueHandle_t CAN_tx_Q;

#define CAN_tx_Q_size 50

typedef struct {
    CAN_RxHeaderTypeDef header;
    uint64_t data;
} CAN_rx_t;

extern QueueHandle_t CAN_rx_Q;

#define CAN_rx_Q_size 50

#ifdef __cplusplus
}
#endif

#endif //NOW_AND_THEN_TASK_CAN_BUS_H
