#!/usr/bin/env python3

import sys
import struct
import numpy
import matplotlib.pyplot as plt
from matplotlib import cm
from mpl_toolkits.mplot3d import Axes3D

if len(sys.argv) < 2:
    print("Usage: {0} formatted-fw-file".format(sys.argv[0]))
    exit(0)

def get_addr_and_byte(line):
    try:
        addr, byte = line.rstrip().split(":")
    except ValueError:
        addr, byte, byte2 = line.split(":")

    return int(addr, 16), int(byte, 16)

def print_map(x, y, values, title):

    valuespy = numpy.array(values).reshape(len(x), len(y))

    max_val = max(values)
    min_val = min(values)

    idx = 0
    for y_val in reversed(y):
        print("{0}".format(str(y_val).rjust(4)), end=" | ")
        for val in range(idx * len(x), (idx + 1) * len(x)):
            if values[val] == max_val:
                print("\033[31;1m{0}\033[0m".format(str(values[val]).rjust(4)), end=" ")
            elif values[val] == min_val:
                print("\033[34;1m{0}\033[0m".format(str(values[val]).rjust(4)), end=" ")
            else:
                print("{0}".format(str(values[val]).rjust(4)), end=" ")
        idx += 1
        print("")

    print("      " + "-----" * (len(x)))
    print("      ", end=" ")
    for i in x:
        print("{0}".format(str(i).rjust(4)), end=" ")
    print("")
    print("")

    valuespy = numpy.rot90(valuespy)
    valuespy = numpy.flip(valuespy, 0)
    valuespy = numpy.flip(valuespy, 1)
    fig = plt.figure()
    fig.suptitle(title)
    ha = fig.add_subplot(111, projection='3d')
    ha.set_xlabel('RPM')
    ha.set_ylabel('Engine Load')

    X, Y = numpy.meshgrid(x, y, indexing="ij")
    ha.plot_surface(X, Y, valuespy, cmap=cm.YlOrRd)

    ha.view_init(90, -90) # Upper view
    ha.view_init(50, -135) # Classic 3D view
    plt.show()
    #plt.savefig("../../resources/map-{0}.png".format(title))


def extract_map(lines, x_axis_addr, y_axis_addr, values_addr):
    array_len = abs(y_axis_addr - x_axis_addr)
    # X axis
    x_axis = []
    for line in lines[x_axis_addr:x_axis_addr + array_len]:
        x_axis.append(get_addr_and_byte(line)[1])

    # Y axis
    y_axis = []
    for line in lines[y_axis_addr:y_axis_addr + array_len]:
        y_axis.append(get_addr_and_byte(line)[1])

    values = []
    for x in range(array_len):
        for y in range(array_len):
            values.append(get_addr_and_byte(lines[values_addr + x * array_len + y])[1])

    return x_axis, y_axis, values

def extract_and_print_map(lines, x_base, y_base, values_base, title="N/A"):
    x_axis, y_axis, values = extract_map(lines, x_base, y_base, values_base)
    print("---> ", title, hex(x_base), hex(y_base), hex(values_base))
    print_map(x_axis, y_axis, values, title)

with open(sys.argv[1], "r") as f:
    lines = f.readlines()

    # Confirmed from code
    extract_and_print_map(lines, 0x1100, 0x1110, 0x1120, title="fuel_map (A/F ratio)")
    extract_and_print_map(lines, 0x1100, 0x1110, 0x2300, title="unknown1")
    extract_and_print_map(lines, 0x1100, 0x1110, 0x2400, title="unknown2")
    extract_and_print_map(lines, 0x14d0, 0x14e0, 0x2000, title="ignition_advance_map")
    extract_and_print_map(lines, 0x14d0, 0x14e0, 0x2100, title="ignition_base_map1")
    extract_and_print_map(lines, 0x14d0, 0x14e0, 0x26a0, title="ignition_base_map2")
    extract_and_print_map(lines, 0x1cb8, 0x1cb0, 0x1cc0, title="small_unknown_3")
    extract_and_print_map(lines, 0x28f0, 0x28f8, 0x28b0, title="maybe_boost_target")
    extract_and_print_map(lines, 0x2900, 0x2908, 0x28b0, title="maybe_injector_battery_table")
    extract_and_print_map(lines, 0x2e88, 0x2e80, 0x2e40, title="maybe_volatile_knock_correction")

    # Strange
    '''
    extract_and_print_map(lines, 0x1100, 0x1110, 0x2300)
    extract_and_print_map(lines, 0x14d0, 0x14e0, 0x2aa0)
    extract_and_print_map(lines, 0x14d0, 0x14e0, 0x2aa0)
    extract_and_print_map(lines, 0x14d0, 0x14e0, 0x27a0)
    extract_and_print_map(lines, 0x14d0, 0x14e0, 0x1e20)
    extract_and_print_map(lines, 0x2a10, 0x2a20, 0x2a40)
    '''


