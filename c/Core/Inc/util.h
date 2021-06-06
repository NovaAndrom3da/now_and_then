//
// Created by gijsl on 3/21/2021.
//

#ifndef NOW_AND_THEN_UTIL_H
#define NOW_AND_THEN_UTIL_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void delay_microseconds(uint32_t u_sec);

void delay_microseconds_ISR(uint32_t u_sec);

extern const uint16_t crc15Table[256];

uint16_t pec15(uint8_t *data, uint16_t len);

uint32_t get_timestamp(void);

int16_t adc_readings_to_deciamps(uint32_t adc_plus, uint32_t adc_minus);

int16_t adc_readings_to_volt_delta(uint32_t adc_vcar, uint32_t adc_vbat);

#ifdef __cplusplus
}
#endif

#endif //NOW_AND_THEN_UTIL_H
