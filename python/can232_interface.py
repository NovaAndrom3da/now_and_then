import serial
from can import BusABC, Message

from can_configuration import CanConfiguration


class can232Bus(BusABC):

    def __init__(self, channel, can_filters=None, **kwargs):
        self.channel_info = "CAN232 Device"

        self.serial_port = serial.Serial(channel, 115200)

        super(can232Bus, self).__init__(channel=channel, can_filters=can_filters, **kwargs)

    def send(self, msg, timeout=None):
        pass

    def _recv_internal(self, timeout):
        rx = None

        x = str(self.serial_port.readline())[2:-6]
        if len(x) > 0 and x[0] == ':':
            is_extended_id = x[1] == 'X'
            fack = x.index('N')
            arb_id = int(x[2:fack], 16)
            data_len = int(len(x[fack+1:]) / 2)

            data = x[-data_len * 2:]
            balls = bytes.fromhex(data)

            bla = Message(timestamp=0,
                          is_remote_frame=False,
                          is_extended_id=is_extended_id,
                          is_error_frame=False,
                          arbitration_id=arb_id,
                          dlc=data_len,
                          data=balls)

            return bla, False

        return rx, False


if __name__ == "__main__":
    bus = can232Bus(channel='COM6')
    fuck = bytes(';', encoding='utf-8')

    for i in range(5):
        print(bus.recv())


