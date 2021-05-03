//
// Created by gijsl on 3/15/2021.
//

#ifndef NOW_AND_THEN_LTC_TASK_H
#define NOW_AND_THEN_LTC_TASK_H

#include "FreeRTOS.h"
#include "queue.h"

/*
 * Define the number of battery segments (number of LTC6811 chips) here.
 * The main task will send instructions once, it is up to task_ltc to handle
 * repeating SPI commands the appropriate number of times.
 */
#define num_segments 1

void task_ltc_setup(void);

//void start_task_ltc(void *argument);
extern "C" [[noreturn]] void start_task_ltc(void *argument);

typedef struct {
    void * placeholder;
} ltc_spi_tx_request_t;

extern QueueHandle_t ltc_spi_tx_Q;

typedef struct {
    void * placeholder;
} ltc_spi_rx_t;

extern QueueHandle_t ltc_spi_rx_Q;

#endif //NOW_AND_THEN_LTC_TASK_H
