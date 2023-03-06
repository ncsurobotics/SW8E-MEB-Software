#!/usr/bin/env python3

import serial
import sys


if __name__ == "__main__":
    if len(sys.argv) != 3:
        print("Usage:")
        print("  readraw.py port baud")
        exit(1)
    port = sys.argv[1]
    baud = int(sys.argv[2])
    ser = serial.Serial(port, baud)
    while True:
        # print(ser.read().decode(), end="")
        sys.stdout.buffer.write(ser.read())
