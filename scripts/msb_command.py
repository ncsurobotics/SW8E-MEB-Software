#!/usr/bin/env python3

import sys
import serial
import comm
from typing import Tuple


EXIT_OK = 0
EXIT_USAGE = 1
EXIT_SERIAL = 2

msb_commands = {
    "t1_trig": 0x03,
    "t2_trig": 0x04,
    "d1_trig": 0x01,
    "d2_trig": 0x02,
    "reset": 0x00
}


def run(ser: serial.Serial, cmd: int):
    # Modify this function to implement the script
    # Use comm.write_msg and comm.read_msg
    msg = bytearray()
    msg.extend(b'MSB')
    msg.append(cmd)
    comm.write_msg(ser, bytes(msg))
    print("WAITING FOR ACK")
    comm.read_msg(ser)
    print("GOT ACK")
    

def usage():
    print("Usage: test_comm.py port baud command")
    print("    port: Serial port to use.")
    print("    baud: Baud rate to use. Integer.")
    print("    command: Command to send to mech board.")
    for cmd in msb_commands.keys():
        print("             {}".format(cmd))


def main() -> int:
    # Parse arguments
    if len(sys.argv) != 4:
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
    run(ser, int(msb_commands[sys.argv[3]]))


if __name__ == "__main__":
    try:
        sys.exit(main())
    except KeyboardInterrupt:
        sys.exit(EXIT_OK)
    except serial.SerialException:
        print("Serial communication error!")
        sys.exit(EXIT_SERIAL)
