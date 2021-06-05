import os

import yaml

import can_configuration
from can_message import CanMessage


class CanNode(yaml.YAMLObject):
    yaml_tag = '!Node'

    def __init__(self, subscribe, publish):
        self.subscribe = subscribe
        self.publish = publish
        self.node_id = 0
        self.name = None

    def set_name(self, name: str) -> None:
        self.name = name

    def get_reserved_msg_ids(self) -> list:
        """
        :return: A list of all the message id's that are pre-set in the yaml file for this node.
        """
        ret = []
        for message in self.publish:
            if message.id != 0:
                ret.append(message.id)
        return ret

    def gen_c_id_define(self) -> str:
        return "#define CAN_NODE_ID_{0} ({1})".format(self.name, self.node_id)

    def gen_shared_message_defines(self) -> str:
        """
        :return: The header file block of defines for each message published by this node.
        """
        return "\n\n\n".join([msg.gen_shared_define() for msg in self.publish])

    def get_message_dict(self) -> dict:
        """
        :return: A dict of the form {id: CanMessage} containing each message published by this node.
        """
        return {msg.id: msg for msg in self.publish}

    def write_subscribe_files(self, header_path: str, source_path: str) -> None:
        """
        :param path: The path to the directory to put the files into. The filenames will be subscribe.c and subscribe.h.
        :return: None
        """

        include_guard_name = "__{0}_SUBSCRIBE_H_".format(self.name.upper())

        header_f = open("{0}/subscribe.h".format(header_path), 'w')
        source_f = open("{0}/subscribe.c".format(source_path), 'w')

        header_f.write("/*\n"
                       " * Generated by {0}. Do not manually edit this file.\n"
                       " */\n".format(os.path.basename(__file__)))
        header_f.write("\n")
        header_f.write("#ifndef {0}\n"
                       "#define {0}\n".format(include_guard_name))
        header_f.write("\n")
        header_f.write("#include <stdint.h>\n"
                       "#include <stdbool.h>\n"
                       "#include \"stm32f4xx_hal.h\"\n"
                       "#include \"stm32f4xx_hal_can.h\"\n"
                       "#include \"task/task_can_bus.h\"\n"
                       "#include <shared_can_defs.h>\n")
        header_f.write("\n")

        header_f.write("void eat_new_data(CAN_rx_t* recv);\n\n")

        source_f.write("/*\n"
                       " * Generated by {0}. Do not manually edit this file.\n"
                       " */\n".format(os.path.basename(__file__)))
        source_f.write("\n")
        source_f.write("#include <stdint.h>\n"
                       "#include <stdbool.h>\n"
                       "#include \"stm32f4xx_hal.h\"\n"
                       "#include \"stm32f4xx_hal_can.h\"\n"
                       "#include \"task/task_can_bus.h\"\n"
                       "#include <shared_can_defs.h>\n"
                       "#include <string.h>\n"
                       "#include \"util.h\"\n"
                       "#include \"subscribe.h\"\n")
        source_f.write("\n")

        for msg in self.subscribe:
            struct_type = CanMessage.make_c_data_struct_type(msg)
            struct_name = CanMessage.make_c_data_struct_instance_name(msg)

            # Write the declaration of the data variable
            header_f.write("extern {0} {1};\n".format(struct_type, struct_name))
            source_f.write("{0} {1};\n".format(struct_type, struct_name))

            # Write the declaration of the timestamp variable
            header_f.write("extern uint32_t {0}_rx_time;\n".format(msg))
            source_f.write("uint32_t {0}_rx_time;\n".format(msg))

            # Blank line for spacing
            header_f.write("\n")
            source_f.write("\n")

        header_f.write("#endif\n")

        source_f.write("void eat_new_data(CAN_rx_t* recv)\n"
                       "{\n"
                       "\tvoid* dest = 0;\n"
                       "\tuint32_t* time_address = 0;\n")

        if_block_template = "if (recv->header.ExtId == {0})\n" \
                            "\t{{\n" \
                            "\t\tdest = &{1};\n" \
                            "\t\ttime_address = &{2};\n" \
                            "\t}}"

        blocks = []
        for msg in self.subscribe:
            id_name = "CAN_ID_" + msg
            struct_name = CanMessage.make_c_data_struct_instance_name(msg)
            time_name = msg + "_rx_time"
            blocks.append(if_block_template.format(id_name, struct_name, time_name))

        source_f.write("\n\t")
        source_f.write("\n\telse ".join(blocks))
        source_f.write("\n\n")
        source_f.write("\tif (dest != 0) {\n"
                       "\t\tmemcpy(dest, &(recv->data), recv->header.DLC);\n"
                       "\t\t*time_address = get_timestamp();\n"
                       "\t}")

        source_f.write("\n}\n")

        header_f.close()
        source_f.close()

    def __repr__(self):
        return "name={0}\n node_id={1} \n subscribe={2} \n publish={3}".format(self.name, self.node_id, self.subscribe,
                                                                               self.publish)


def can_node_constructor(loader, node) -> CanNode:
    can_node_data = loader.construct_mapping(node, deep=True)
    keys = can_node_data.keys()

    pub = can_configuration.dict2list(can_node_data['publish']) if 'publish' in keys else []
    sub = can_node_data['subscribe'] if 'subscribe' in keys else []

    return CanNode(subscribe=sub, publish=pub)


if __name__ == "__main__":
    test_node = CanNode(None, None)
    test_node.node_id = 50
    test_node.name = "test"

    print(test_node.gen_c_id_define())
