//
// Created by gijsl on 5/1/2021.
//

#ifndef NOW_AND_THEN_SEGMENT_BOARD_H
#define NOW_AND_THEN_SEGMENT_BOARD_H

#include <stdint.h>
#include "ltc6811.h"


class SegmentBoard {
private:
    uint8_t mux_state = 0;
public:
    SegmentBoard();
    ~SegmentBoard();

    void set_leds(uint8_t state);
    void set_mux(uint8_t state);
    void update_volts(uint8_t cmd, uint8_t* buffer);
    void calculate_balance();
    void set_balance_transistor(uint8_t cell_n, bool discharging);

    ltc68_reg_t registers = {0};
    uint16_t cell_volts[12] = {0};
    uint16_t cell_temps[12] = {0};
    uint16_t open_wires;
    uint16_t segment_voltage = 0;
};

#endif //NOW_AND_THEN_SEGMENT_BOARD_H
