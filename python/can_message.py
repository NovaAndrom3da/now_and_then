import struct

import yaml

from message_field import MessageField


class CanMessage(yaml.YAMLObject):
    yaml_tag = '!Message'

    def __init__(self, msg_id, priority, interval, fields):
        self.name = None
        self.id = msg_id if msg_id else 0
        self.priority = priority
        self.period = interval if interval else 0
        self.fields = fields

    def set_name(self, name: str) -> None:
        self.name = name

    @staticmethod
    def make_c_data_struct_type(name: str) -> str:
        return "CAN_MSG_{0}_T".format(name)

    def gen_c_data_struct_name(self) -> str:
        return CanMessage.make_c_data_struct_type(self.name)

    @staticmethod
    def make_c_data_struct_instance_name(name: str) -> str:
        return "m_CAN_MSG_{0}".format(name)

    def gen_c_data_struct(self) -> str:
        if self.fields:
            field_str = "\n".join([field.gen_c_struct() for field in self.fields])
        else:
            field_str = ""

        template = "typedef struct __attribute__((__packed__)) {{" \
                   "\n{0}" \
                   "\n}} {1};"

        return template.format(field_str, self.gen_c_data_struct_name())

    def gen_c_id_define(self) -> str:
        return "#define CAN_ID_{0} ({1})".format(self.name, hex(self.id))

    def gen_c_period_define(self) -> str:
        return "#define CAN_PERIOD_{0} ({1})".format(self.name, self.period)

    def gen_c_field_enums(self) -> str:
        if self.fields:
            enum_fields = [f for f in self.fields if f.enum_values]
            if len(enum_fields) > 0:
                return "\n".join([f.gen_c_enum() for f in enum_fields if f.enum_values])
        return ""

    def gen_c_scale_defines(self) -> str:
        if self.fields:
            template = "#define CAN_SCALE_{0}_{1} ((float) {2})"
            scaled_fields = [f for f in self.fields if f.scale]
            if len(scaled_fields) > 0:
                return "\n".join([template.format(self.name, f.name, f.scale) for f in scaled_fields])

        return ""

    def gen_shared_define(self) -> str:
        """
        :return: The header file block of defines for this message.
        """
        lines = [
            self.gen_c_id_define(),
            self.gen_c_period_define(),
            self.gen_c_data_struct(),
            self.gen_c_scale_defines(),
            self.gen_c_field_enums()
        ]

        return "\n".join([line for line in lines if line])

    def get_python_struct_string(self) -> str:
        """
        :return: A string describing the order of the dataypes of the data sent by this message.
        """
        return "<" + "".join([f.get_python_struct_string() for f in self.fields])

    def decode(self, data_bytes: bytes) -> dict:
        """
        :param data_bytes: A bytes object from the CAN bus.
        :return: A dict of the form {field name: value} for the fields of this message.
        """
        # If the message has no fields, can't decode shit
        if not self.fields:
            return {}

        # If no bytes are given, can't decode shit
        if data_bytes is None:
            return {}

        unpacker = struct.Struct(self.get_python_struct_string())
        unpacked = dict(zip([f.name for f in self.fields], unpacker.unpack(data_bytes)))
        for f in self.fields:
            if 'bitfield' in f.datatype:
                packed_bitfield = unpacked[f.name]
                for i, bitname in enumerate(reversed(f.bits)):
                    unpacked[bitname] = packed_bitfield[i // 8] >> (7 - i % 8) & 1

                unpacked.pop(f.name)
            if f.enum_values != None:
                unpacked[f.name] = f.enum_values[unpacked[f.name]]

        return {k: v for k, v in unpacked.items() if 'pad' not in k}

    def __repr__(self):
        return "\n\tname={0} id={1} priority={2} interval={3}" \
               "\n\tfields={4}\n".format(self.name, self.id, self.priority, self.period, self.fields)


def can_message_constructor(loader, node):
    pub_msg_data = loader.construct_mapping(node, deep=True)
    keys = pub_msg_data.keys()

    msg_id = None
    priority = None
    exp_int = None
    fields = None
    if 'id' in keys:
        msg_id = pub_msg_data['id']
    if 'priority' in keys:
        priority = pub_msg_data['priority']
    if 'period' in keys:
        exp_int = pub_msg_data['period']
    if 'fields' in keys:
        if pub_msg_data['fields']:
            fields = [MessageField(k, v) for k, v in pub_msg_data['fields'].items()]

    return CanMessage(msg_id=msg_id, priority=priority, interval=exp_int, fields=fields)
