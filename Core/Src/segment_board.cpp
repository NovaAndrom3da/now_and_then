//
// Created by gijsl on 5/1/2021.
//

#include <ltc6811_config_register.h>
#include <string.h>
#include "segment_board.h"

SegmentBoard::SegmentBoard() {
    registers.CFGR[0] = CFGR0;
    registers.CFGR[1] = CFGR1;
    registers.CFGR[2] = CFGR2;
    registers.CFGR[3] = CFGR3;
    registers.CFGR[4] = CFGR4;
    registers.CFGR[5] = CFGR5;

    registers.COMM[0] = 0x80;
    registers.COMM[1] = 0b11100000 + 0x09;
    registers.COMM[2] = 0xF0;
    registers.COMM[3] = 0x09;
    registers.COMM[4] = 0xF0;
    registers.COMM[5] = 0x09;

    open_wires = 0;
}

void SegmentBoard::set_leds(uint8_t state) {
    registers.COMM[1] = (state << 5) + 0x09;
}

void SegmentBoard::set_mux(uint8_t state) {
    mux_state = state;
    registers.COMM[0] = 0x80 + (state & 0x0F);
}

void SegmentBoard::update_volts(uint8_t cmd, uint8_t *buffer) {
    if (cmd == COMMAND_RDCVA) {
        memcpy(registers.CVAR, buffer, 6);
        cell_volts[0] = (registers.CVAR[0] & 0xff) + (registers.CVAR[1] << 8);
        cell_volts[1] = (registers.CVAR[2] & 0xff) + (registers.CVAR[3] << 8);
        cell_volts[2] = (registers.CVAR[4] & 0xff) + (registers.CVAR[5] << 8);
    } else if (cmd == COMMAND_RDCVB) {
        memcpy(registers.CVBR, buffer, 6);
        cell_volts[3] = (registers.CVBR[0] & 0xff) + (registers.CVBR[1] << 8);
        cell_volts[4] = (registers.CVBR[2] & 0xff) + (registers.CVBR[3] << 8);
        cell_volts[5] = (registers.CVBR[4] & 0xff) + (registers.CVBR[5] << 8);
    } else if (cmd == COMMAND_RDCVC) {
        memcpy(registers.CVCR, buffer, 6);
        cell_volts[6] = (registers.CVCR[0] & 0xff) + (registers.CVCR[1] << 8);
        cell_volts[7] = (registers.CVCR[2] & 0xff) + (registers.CVCR[3] << 8);
        cell_volts[8] = (registers.CVCR[4] & 0xff) + (registers.CVCR[5] << 8);
    } else if (cmd == COMMAND_RDCVD) {
        memcpy(registers.CVDR, buffer, 6);
        cell_volts[9] = (registers.CVDR[0] & 0xff) + (registers.CVDR[1] << 8);
        cell_volts[10] = (registers.CVDR[2] & 0xff) + (registers.CVDR[3] << 8);
        cell_volts[11] = (registers.CVDR[4] & 0xff) + (registers.CVDR[5] << 8);
    }
}

SegmentBoard::~SegmentBoard() = default;
