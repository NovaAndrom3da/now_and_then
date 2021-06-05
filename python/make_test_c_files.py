from can_configuration import CanConfiguration

"""
Use this file to generate new C code for the 2020 car, Loraine.
"""

if __name__ == "__main__":
    config = CanConfiguration()
    config.load('2021.yml')
    config.assign_missing_ids()
    config.write_shared_defs("../c/Core/Inc/shared_can_defs.h")

    for node in config.nodes:
        print(node.name)
        if node.name == "BMS":
            node.write_subscribe_files("../c/Core/Inc", "../c/Core/Src")
