//
// Created by gijsl on 3/17/2021.
//

#include <stdbool.h>
#include "can.h"
#include "stm32f4xx_hal_can.h"
#include "task/task_can_bus.h"

QueueHandle_t CAN_tx_Q;
HAL_StatusTypeDef stat = 0;
uint32_t mailbox = 0;

_Noreturn void start_task_can_bus(void *argument) {
    HAL_CAN_Start(&hcan1);
    while (1) {
        if (uxQueueMessagesWaiting(CAN_tx_Q) > 0 && HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) > 0) {
            CAN_tx_request req = {0};

            xQueueReceive(CAN_tx_Q, &req, 10);
            stat = HAL_CAN_AddTxMessage(&hcan1, &req.header, (uint8_t *) &req.data, &mailbox);
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void setup_can_bus_task(void) {
    CAN_tx_Q = xQueueCreate(50, sizeof(CAN_tx_request));
}

BaseType_t send_can_msg(uint32_t msg_id, void *data, uint8_t data_len) {
    CAN_tx_request req = {0};
    req.header.ExtId = msg_id;
    req.header.IDE = CAN_ID_EXT;
    req.header.RTR = CAN_RTR_DATA;
    req.header.DLC = data_len;
    req.header.TransmitGlobalTime = false;

    return xQueueSend(CAN_tx_Q, &req, 1);
}

//void TxMailboxCompleteCallback(CAN_HandleTypeDef *hcan) {
//    if (uxQueueMessagesWaitingFromISR(CAN_tx_Q) > 0) {
//        CAN_tx_request req = {0};
//        uint32_t mailbox = 0;
//        xQueueReceiveFromISR(CAN_tx_Q, &req, NULL);
//        HAL_CAN_AddTxMessage(hcan, &req.header, (uint8_t *) &req.data, &mailbox);
//    }
//}
//
//void HAL_CAN_TxMailbox0CompleteCallback(CAN_HandleTypeDef *hcan) {
//    TxMailboxCompleteCallback(hcan);
//}
//
//void HAL_CAN_TxMailbox1CompleteCallback(CAN_HandleTypeDef *hcan) {
//    TxMailboxCompleteCallback(hcan);
//}
//
//void HAL_CAN_TxMailbox2CompleteCallback(CAN_HandleTypeDef *hcan) {
//    TxMailboxCompleteCallback(hcan);
//}