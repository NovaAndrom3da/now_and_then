from can.interfaces.usb2can.usb2canabstractionlayer import *

from can_configuration import CanConfiguration


def open_bus(bitrate):
    return can.interfaces.usb2can.Usb2canBus(channel=None, bitrate=bitrate)


if __name__ == "__main__":
    config = CanConfiguration()
    config.load('2021.yml')
    config.assign_missing_ids()
    msg_dict = config.get_message_dict()

    bus = open_bus(config.bitrate)

    print(msg_dict)

    counter = 0
    for msg in bus:
        try:
            message = msg_dict[msg.arbitration_id]
            decoded = message.decode(msg.data)
            print(decoded, flush=True)
            counter += 1

        except Exception as e:
            # print(e, flush=True)
            pass
