import math


class MessageField:
    def __init__(self, k, v):
        self.name = k
        self.bits = None
        self.enum_values = None

        # If there is a scale, the number sent over the bus = actual value * scale
        # For example, a voltage field with scale 10 would send value 120 for 12 volts
        self.scale = None

        if type(v) is str:
            # simplest case, the field looks something like fieldname: 'uin32_t'
            self.datatype = v
        elif type(v) is list:
            # this is a bitfield, each flag is listed out
            num_bits = math.ceil(len(v) / 8.0) * 8
            self.datatype = 'bitfield{}'.format(num_bits)
            self.bits = v
            for i in range(len(self.bits), num_bits):
                self.bits.append("pad{0}".format(i))
        elif type(v) is dict:
            # most complicated case, there is more information than just a type
            self.datatype = v['type']
            self.scale = v.get('scale', self.scale)
            self.enum_values = v.get('enum', None)

    def set_name(self, name: str) -> None:
        self.name = name

    def gen_c_struct(self) -> str:
        if "bitfield" in self.datatype:
            uint_type = 'uint{0}_t'.format(self.datatype[8:])

            union_template = "\tunion {0} {{" \
                             "\n\t\tstruct __attribute__((__packed__)) {{" \
                             "{1}" \
                             "\n\t\t}};" \
                             "\n\t\t{2} AS_UINT;" \
                             "\n\t}} {0};"

            bit_template = "\n\t\t\tunsigned int {0}:1;"

            bits = "".join([bit_template.format(b) for b in self.bits])

            return union_template.format(self.name, bits, uint_type)
        else:
            return "\t{0} {1};".format(self.datatype, self.name)

    def gen_c_enum(self) -> str:
        if self.enum_values:
            template = "typedef enum {{" \
                       "{0}" \
                       "\n}} {1}_t;"
            lines = "".join(["\n\t{0}_{1} = {2},".format(self.name.upper(), self.enum_values[index], index) for index in
                             range(len(self.enum_values))])
            return template.format(lines, self.name)
        else:
            return ""

    def get_python_struct_string(self) -> str:
        struct_format_dict = {
            'float': 'f',
            'uint64_t': 'Q',
            'int64_t': 'q',
            'uint32_t': 'I',
            'int32_t': 'i',
            'uint16_t': 'H',
            'int16_t': 'h',
            'uint8_t': 'B',
            'int8_t': 'b',
            'bitfield16': '2s',
            'bitfield8': '1s'
        }

        return struct_format_dict[self.datatype]

    def __repr__(self):
        ret = "\n\t\tname={0} type={1}".format(self.name, self.datatype)
        if self.bits:
            ret += " bits={}".format(self.bits)
        if self.enum_values:
            ret += " enum_values={}".format(self.enum_values)

        return ret
