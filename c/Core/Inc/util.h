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

#ifdef __cplusplus
}
#endif

#endif //NOW_AND_THEN_UTIL_H
