#!/usr/bin/env python3

import sys
import struct

if len(sys.argv) < 2:
    print("Usage: {0} formatted-fw-file".format(sys.argv[0]))
    exit(0)

def get_addr_and_byte(line):
    try:
        addr, byte = line.rstrip().split(":")
    except ValueError:
        addr, byte, byte2 = line.split(":")

    return int(addr, 16), int(byte, 16)


with open(sys.argv[1], "r") as f:
    lines = f.readlines()
    for i in range(0, 0xffff, 0x10):
        array = []
        inc = True
        for j in range(16):
            array.append(get_addr_and_byte(lines[i + j])[1])
            if j and array[j] < array[j - 1]:
                inc = False
                break
        if inc:
            all_the_same = True
            for j in range(16):
                if j and array[j] != array[j - 1]:
                    all_the_same = False
                    break
            if not all_the_same:
                print(hex(i), array)



