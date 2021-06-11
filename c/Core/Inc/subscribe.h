/*
 * Generated by can_node.py. Do not manually edit this file.
 */

#ifndef __BMS_SUBSCRIBE_H_
#define __BMS_SUBSCRIBE_H_

#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_can.h"
#include "task/task_can_bus.h"
#include <shared_can_defs.h>

void eat_new_data(CAN_rx_t* recv);

extern CAN_MSG_VCU_pedals_T m_CAN_MSG_VCU_pedals;
extern uint32_t VCU_pedals_rx_time;

extern CAN_MSG_VCU_switches_T m_CAN_MSG_VCU_switches;
extern uint32_t VCU_switches_rx_time;

#endif