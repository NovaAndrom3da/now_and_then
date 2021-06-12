//
// Created by Gijs Landwehr on 6/6/2021.
//

#include "inverter.h"

/*
 *
 *
        send_can_msg(CAN_ID_INV_COMMAND, &inverter_cmd, sizeof(CAN_MSG_INV_COMMAND_T));
        inverter_cmd.torque_command = 0x0;
    inverter_cmd.speed_command = 0x0;
    inverter_cmd.enable_flags.speed_mode_enable = 0;
    inverter_cmd.enable_flags.inverter_enable = 0;
    inverter_cmd.enable_flags.inverter_discharge = 0;
    inverter_cmd.enable_flags.pad3 = 0;
    inverter_cmd.enable_flags.pad4 = 0;
    inverter_cmd.enable_flags.pad5 = 0;
    inverter_cmd.enable_flags.pad6 = 0;
    inverter_cmd.enable_flags.pad7 = 0;
//        inverter_cmd.enable_flags.AS_UINT |= (inv_dumb_counter << 4);
    inv_dumb_counter++;
    if (inv_dumb_counter > 15) {
        inv_dumb_counter = 0;
    }
    inverter_cmd.direction = DIRECTION_REVERSE;
    inverter_cmd.torque_limit = 0xfff;
 *
 *
 */