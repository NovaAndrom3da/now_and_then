import unittest
from unittest import TestCase

from can_message import CanMessage


class TestCanMessage(TestCase):
    def test_get_c_data_struct_name(self):
        test_msg = CanMessage(msg_id=0x1, priority=0, interval=50, fields=None)
        test_name = "TEST_MSG"
        test_msg.set_name(test_name)
        assert test_msg.gen_c_data_struct_name() == "CAN_MSG_{0}_T".format(test_name)

    def test_get_c_id_define(self):
        test_msg = CanMessage(msg_id=0x1, priority=0, interval=50, fields=None)
        test_name = "TEST_MSG"
        test_msg.set_name(test_name)
        assert test_msg.gen_c_id_define() == "#define CAN_ID_{0} ({1})".format(test_name, "0x1")


if __name__ == '__main__':
    unittest.main()
