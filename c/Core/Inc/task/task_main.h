//
// Created by gijsl on 3/17/2021.
//

#ifndef NOW_AND_THEN_TASK_MAIN_H
#define NOW_AND_THEN_TASK_MAIN_H

#include <stdint.h>
#include <stdbool.h>

#define VEHICLE_STATE_STARTUP 0
#define VEHICLE_STATE_SHUTDOWN_OPEN 1
#define VEHICLE_STATE_SHUTDOWN_CLOSED 2
#define VEHICLE_STATE_PRECHARGING 3
#define VEHICLE_STATE_PRECHARGE_DONE 4
#define VEHICLE_STATE_DRIVING 5
#define VEHICLE_STATE_FAULTED 6

_Noreturn void start_task_main(void *argument);

extern uint8_t vehicle_state;

void run_state_machine(void);

#endif //NOW_AND_THEN_TASK_MAIN_H
