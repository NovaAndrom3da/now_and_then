import os

import yaml

import can_message
import can_node


def dict2list(input_dict: dict) -> list:
    if input_dict is None:
        return []

    ret = []
    for key, value in input_dict.items():
        value.set_name(key)
        ret.append(value)

    return ret


class CanConfiguration:
    def __init__(self):
        self.nodes = []
        self.priority_levels = {}
        self.bitrate = 0

    def load(self, yaml_filename):
        config_file = open(yaml_filename)

        yaml.add_constructor('!Node', can_node.can_node_constructor)
        yaml.add_constructor('!Message', can_message.can_message_constructor)

        loaded = yaml.load(config_file, Loader=yaml.Loader)

        node_counter = 0
        self.nodes = dict2list(loaded['nodes'])
        for n in self.nodes:
            n.node_id = node_counter
            node_counter += 1

        self.priority_levels = loaded['priority_levels']
        self.bitrate = loaded['bitrate']

    def get_reserved_msg_ids(self) -> list:
        """
        :return: A list of all the message id's that are pre-set in the yaml file for all nodes.
        """
        ret = []
        for node in self.nodes:
            ret.extend(node.get_reserved_msg_ids())
        if len(ret) != len(set(ret)):
            raise UserWarning("There are duplicate message id's in the yaml file!")
        return ret

    def gen_priority_defines(self) -> str:
        level_template = "#define CAN_PRIORITY_LEVEL_{0} ({1})"
        levels = "\n".join([level_template.format(*item) for item in self.priority_levels.items()])

        num_priorities = len(self.priority_levels)
        levels += "\n" \
                  "\n#define CAN_MAX_PRIORITY_LEVEL ({0})" \
                  "\n#define CAN_NUM_PRIORITY_LEVELS ({1})" \
                  "\n".format(num_priorities - 1, num_priorities)

        return levels

    def write_shared_defs(self, filename: str) -> None:
        """
        Portions of file to make:
        -Warning not to manually edit
        -Include guard
        -Include statements
        -Bitrate
        -Macro for msg id -> priority level
        -Priority level defines
        -Node ID defines
        -Per message defines
        -End of include guard
        """

        f = open(filename, 'w')

        f.write("/*\n"
                " * Generated by {0}. Do not manually edit this file.\n"
                " */\n".format(os.path.basename(__file__)))
        f.write("\n")
        f.write("#ifndef LHRE_SHARED_CAN_DEFS_H_\n"
                "#define LHRE_SHARED_CAN_DEFS_H_\n")
        f.write("\n")
        f.write("#include <stdint.h>\n")
        f.write("\n")
        f.write("#define CAN_BITRATE ({0})\n".format(self.bitrate))
        f.write("\n")
        f.write(self.gen_priority_defines())
        f.write("\n")
        for node in self.nodes:
            f.write(node.gen_c_id_define())
            f.write("\n")
        f.write("\n")
        for node in self.nodes:
            f.write("// Messages from node ({0}): {1}\n".format(node.node_id, node.name))
            f.write(node.gen_shared_message_defines())
            f.write("\n\n\n")
        f.write("\n")
        f.write("#endif /* LHRE_SHARED_CAN_DEFS_H_ */\n")

        f.close()

    def assign_missing_ids(self) -> None:
        """
        Loop over each message in each node and give it an id if one is not given in the yaml.
        id's look like this: PPNNIIII, where PP is the priority, NN is the node id, and IIII counts from 0
        for each node.

        :return: None
        """
        used_ids = self.get_reserved_msg_ids()

        for node in self.nodes:
            for msg in node.publish:
                if msg.id is 0:
                    for i in range(0xFFFF):
                        new_id = (int(self.priority_levels[msg.priority]) << 24) + (int(node.node_id) << 16) + i
                        if new_id not in used_ids:
                            msg.id = new_id
                            used_ids.append(new_id)
                            break

    def get_message_dict(self) -> dict:
        """
        :return: A dict of the form {id: CanMessage} containing every message published by every node.
        """
        ret = {}
        for node in self.nodes:
            ret.update(node.get_message_dict())

        return ret


if __name__ == "__main__":
    config = CanConfiguration()
    config.load('2020.yml')
    config.assign_missing_ids()

    count = 0
    for node in config.nodes:
        for msg in node.publish:
            print(msg.get_python_struct_string())
            count += 1

    print(count)
    print(len(config.get_message_dict()))
