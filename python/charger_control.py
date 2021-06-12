import can
from can.interfaces.usb2can.usb2canabstractionlayer import *

from can_configuration import CanConfiguration


def open_bus(bitrate):
    return can.interfaces.usb2can.Usb2canBus(channel=None, bitrate=bitrate)


def make_charger_control_message(volts, amps, go):
    volt_data = volts * 10
    amp_data = amps * 10
    byte1 = (volt_data & 0xff00) >> 8
    byte2 = (volt_data & 0x00ff)
    byte3 = (amp_data & 0xff00) >> 8
    byte4 = (amp_data & 0x00ff)
    byte5 = 0 if go else 1
    byte6 = 0
    byte7 = 0
    byte8 = 0

    return [byte1, byte2, byte3, byte4, byte5, byte6, byte7, byte8]


if __name__ == "__main__":
    config = CanConfiguration()
    config.load('2021.yml')
    config.assign_missing_ids()
    msg_dict = config.get_message_dict()

    bus = open_bus(config.bitrate)

    battery_voltage = 0

    while True:
        msg = next(iter(bus), None)

        if msg is not None:
            try:
                message = msg_dict[msg.arbitration_id]
                decoded = message.decode(msg.data)
                print(decoded, flush=True)

            except Exception as e:
                # print(e, flush=True)
                pass

        send_data = make_charger_control_message(350, 3, True)
        bus.send(can.Message(arbitration_id=0x1806E5F4, data=send_data, extended_id=True))
