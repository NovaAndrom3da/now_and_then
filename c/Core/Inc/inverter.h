//
// Created by Gijs Landwehr on 6/6/2021.
//

#ifndef NOW_AND_THEN_INVERTER_H
#define NOW_AND_THEN_INVERTER_H

#include <stdint.h>
#include "shared_can_defs.h"

class Inverter {
private:
    CAN_MSG_INV_COMMAND_T control_message;
};

#endif //NOW_AND_THEN_INVERTER_H
