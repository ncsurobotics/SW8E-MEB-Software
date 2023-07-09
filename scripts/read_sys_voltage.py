#!/usr/bin/env python3

import sys
import serial
import struct
import comm
from typing import Tuple


EXIT_OK = 0
EXIT_USAGE = 1
EXIT_SERIAL = 2


def run(ser: serial.Serial):
    # Modify this function to implement the script
    # Use comm.write_msg and comm.read_msg
    while True:
        msg_id, msg = comm.read_msg(ser)
        if msg.startswith(b'VSYS'):
            voltage = struct.unpack_from("<f", msg, 4)
            print("System Voltage: {0}".format(voltage))
    

def usage():
    print("Usage: read_sys_voltage.py port baud")
    print("    port: Serial port to use.")
    print("    baud: Baud rate to use. Integer.")


def main() -> int:
    # Parse arguments
    if len(sys.argv) != 3:
        usage()
        return EXIT_USAGE
    port = sys.argv[1]
    baud = 0
    try:
        baud = int(sys.argv[2])
    except:
        usage()
        return EXIT_USAGE

    ser = serial.Serial(port, baud)
    run(ser)


if __name__ == "__main__":
    try:
        sys.exit(main())
    except KeyboardInterrupt:
        sys.exit(EXIT_OK)
    except serial.SerialException:
        print("Serial communication error!")
        sys.exit(EXIT_SERIAL)
