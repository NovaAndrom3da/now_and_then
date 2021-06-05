//
// Created by gijsl on 3/17/2021.
//

#include <stdbool.h>
#include <string.h>
#include <subscribe.h>
#include <gpio.h>
#include "can.h"
#include "stm32f4xx_hal_can.h"
#include "task/task_can_bus.h"

QueueHandle_t CAN_tx_Q;
QueueHandle_t CAN_rx_Q;

HAL_StatusTypeDef stat = 0;
uint32_t mailbox = 0;

_Noreturn void start_task_can_bus(void *argument) {
    CAN_FilterTypeDef filter;
    filter.FilterActivation = CAN_FILTER_ENABLE;
    filter.FilterBank = 0;
    filter.FilterMode = CAN_FILTERMODE_IDMASK;
    filter.FilterScale = CAN_FILTERSCALE_32BIT;
    filter.FilterIdHigh = 0x0;
    filter.FilterIdLow = 0x0;
    filter.FilterMaskIdHigh = 0x0;
    filter.FilterMaskIdLow = 0x0;
    filter.FilterFIFOAssignment = CAN_FilterFIFO0;
    HAL_CAN_ConfigFilter(&hcan1, &filter);

//    filter.FilterFIFOAssignment = CAN_FilterFIFO1;
//    HAL_CAN_ConfigFilter(&hcan1, &filter);

    HAL_CAN_Start(&hcan1);
//    HAL_CAN_ActivateNotification(&hcan1, CAN_IT_TX_MAILBOX_EMPTY);
//    HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_FULL);
//    HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);
//    HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_OVERRUN);
//    HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO1_FULL);
//    HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO1_MSG_PENDING);
//    HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO1_OVERRUN);
    HAL_CAN_ActivateNotification(&hcan1, CAN_IT_TX_MAILBOX_EMPTY | CAN_IT_RX_FIFO0_FULL | CAN_IT_RX_FIFO0_MSG_PENDING |
                                         CAN_IT_RX_FIFO0_OVERRUN | CAN_IT_RX_FIFO1_FULL | CAN_IT_RX_FIFO1_MSG_PENDING |
                                         CAN_IT_RX_FIFO1_OVERRUN);
    while (1) {
        // process tx
        if (uxQueueMessagesWaiting(CAN_tx_Q) > 0 && HAL_CAN_GetTxMailboxesFreeLevel(&hcan1) > 0) {
            CAN_tx_request_t req = {0};

            xQueueReceive(CAN_tx_Q, &req, 10);
            stat = HAL_CAN_AddTxMessage(&hcan1, &req.header, (uint8_t *) &req.data, &mailbox);
        }
        set_led_2(true);
        // process rx
        if (uxQueueMessagesWaiting(CAN_rx_Q) > 0) {

            CAN_rx_t recv = {0};
            xQueueReceive(CAN_rx_Q, &recv, 10);
            eat_new_data(&recv);
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void setup_can_bus_task(void) {
    CAN_tx_Q = xQueueCreate(CAN_tx_Q_size, sizeof(CAN_tx_request_t));
    CAN_rx_Q = xQueueCreate(CAN_rx_Q_size, sizeof(CAN_rx_t));
}

BaseType_t send_can_msg(uint32_t msg_id, void *data, uint8_t data_len) {
    CAN_tx_request_t req = {0};
    req.header.ExtId = msg_id;
    req.header.IDE = CAN_ID_EXT;
    req.header.RTR = CAN_RTR_DATA;
    req.header.DLC = data_len;
    req.header.TransmitGlobalTime = false;
    memcpy(&req.data, data, data_len);

    return xQueueSend(CAN_tx_Q, &req, 1);
}

void TxMailboxCompleteCallback(CAN_HandleTypeDef *hcan) {
    if (uxQueueMessagesWaitingFromISR(CAN_tx_Q) > 0) {
        CAN_tx_request_t req = {0};
//        uint32_t mailbox = 0;
        xQueueReceiveFromISR(CAN_tx_Q, &req, NULL);
        HAL_CAN_AddTxMessage(hcan, &req.header, (uint8_t *) &req.data, &mailbox);
    }
}

void HAL_CAN_TxMailbox0CompleteCallback(CAN_HandleTypeDef *hcan) {
    TxMailboxCompleteCallback(hcan);
}

void HAL_CAN_TxMailbox1CompleteCallback(CAN_HandleTypeDef *hcan) {
    TxMailboxCompleteCallback(hcan);
}

void HAL_CAN_TxMailbox2CompleteCallback(CAN_HandleTypeDef *hcan) {
    TxMailboxCompleteCallback(hcan);
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
    set_led_1(true);
    CAN_rx_t rx_msg = {0};
    HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rx_msg.header, (uint8_t *) &rx_msg.data);

    xQueueSendFromISR(CAN_rx_Q, &rx_msg, NULL);
}

void HAL_CAN_RxFifo0FullCallback(CAN_HandleTypeDef *hcan) {
    HAL_CAN_RxFifo0MsgPendingCallback(hcan);
}

void HAL_CAN_RxFifo1MsgPendingCallback(CAN_HandleTypeDef *hcan) {
    set_led_1(true);
    CAN_rx_t rx_msg = {0};
    HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO1, &rx_msg.header, (uint8_t *) &rx_msg.data);

    xQueueSendFromISR(CAN_rx_Q, &rx_msg, NULL);
}

void HAL_CAN_RxFifo1FullCallback(CAN_HandleTypeDef *hcan) {
    HAL_CAN_RxFifo1MsgPendingCallback(hcan);
}