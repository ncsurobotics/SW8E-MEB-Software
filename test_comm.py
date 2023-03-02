#!/usr/bin/env python3

import sys
import serial
import struct
from typing import Tuple


EXIT_OK = 0
EXIT_USAGE = 1
EXIT_SERIAL = 2

START_BYTE = b'\xfd'
END_BYTE = b'\xfe'
ESCAPE_BYTE = b'\xff'

curr_msg_id = 0


def crc16_ccitt_false(msg: bytes, initial = 0xFFFF):
    crc = initial
    pos = 0
    while pos < len(msg):
        b = msg[pos]
        for i in range(8):
            bit = ((b >> (7 - i) & 1) == 1)
            c15 = ((crc >> 15 & 1) == 1)
            crc <<= 1
            crc &= 0xFFFF
            if c15 ^ bit:
                crc ^= 0x1021
                crc &= 0xFFFF
        pos += 1
    return crc & 0xFFFF


# Returns msg_id
def write_msg(ser: serial.Serial, msg: bytes) -> int:
    global START_BYTE, END_BYTE, ESCAPE_BYTE, curr_msg_id

    # Generate the ID for this message and increment the global ID counter
    msg_id = curr_msg_id
    curr_msg_id += 1

    # Write start byte
    ser.write(START_BYTE)

    # Write message ID (unsigned 16-bit int big endian). Escape as needed.
    id_dat = struct.pack(">H", msg_id)
    b = id_dat[0:1]
    if b == START_BYTE or b == END_BYTE or b == ESCAPE_BYTE:
        ser.write(ESCAPE_BYTE)
    ser.write(b)
    b = id_dat[1:2]
    if b == START_BYTE or b == END_BYTE or b == ESCAPE_BYTE:
        ser.write(ESCAPE_BYTE)
    ser.write(b)

    # Write each byte of msg (escaping it as necessary)
    for i in range(len(msg)):
        b = msg[i:i+1]
        if b == START_BYTE or b == END_BYTE or b == ESCAPE_BYTE:
            ser.write(ESCAPE_BYTE)
        ser.write(b)
    
    # Calculate CRC and write it. CRC INCLUDES MESSAGE ID BYTES.
    # Each byte of CRC must also be escaped
    crc = crc16_ccitt_false(msg, crc16_ccitt_false(id_dat))
    high_byte = ((crc >> 8) & 0xFF).to_bytes(1, 'little')
    low_byte = (crc & 0xFF).to_bytes(1, 'little')
    if high_byte == START_BYTE or high_byte == END_BYTE or high_byte == ESCAPE_BYTE:
        ser.write(ESCAPE_BYTE)
    ser.write(high_byte)
    if low_byte == START_BYTE or low_byte == END_BYTE or low_byte == ESCAPE_BYTE:
        ser.write(ESCAPE_BYTE)
    ser.write(low_byte)

    # Write end byte
    ser.write(END_BYTE)

    return msg_id

# Returns msg_id, msg
def read_msg(ser: serial.Serial) -> Tuple[int, bytes]:
    # Holds message being received
    msg = bytearray()

    # Track parse state
    parse_escaped = False
    parse_started = True

    while True:
        # Blocks until  a byte is available
        b = ser.read()

        # Parse the meaning of this byte
        if parse_escaped:
            # Currently escaped (previous byte was ESCAPE_BYTE)
            # Handle **valid** escape sequences (only special bytes can be escaped)
            # Ignore invalid sequences
            if b == START_BYTE or b == END_BYTE or b == ESCAPE_BYTE:
                msg.extend(b)
            
            # Handled byte after escape byte. No longer escaped.
            parse_escaped = False
        elif parse_started:
            if b == START_BYTE:
                # Handle start byte (special meaning when not escaped)
                # Discard old data when start byte received
                msg = bytearray()
            elif b == END_BYTE:
                # Handle end byte (special meaning when not escaped)
                # End byte means the buffer now holds the entire message
                
                # Calculate CRC of read data. Exclude last two bytes.
                # Last two bytes are the CRC (big endian) appended to the original data
                # First two bytes are message ID. These are INCLUDED in CRC calc.
                calc_crc = crc16_ccitt_false(bytes(msg[0:len(msg)-2]))
                read_crc = struct.unpack(">H", msg[len(msg)-2:])[0]

                if calc_crc == read_crc:
                    # This is a complete, valid message.
                    read_id = struct.unpack(">H", msg[0:2])[0]
                    return read_id, bytes(msg[2:len(msg)-2])
                else:
                    # Got a complete message, but it is invalid. Ignore it.
                    parse_started = False
            elif b == ESCAPE_BYTE:
                # Handle escape byte (special meaning when not escaped)
                parse_escaped = True
            else:
                # handle normal bytes (these are just data)
                msg.extend(b)
        elif b == START_BYTE:
            # Received a start byte. Start parsing. Discard old data.
            parse_started = True
            msg = bytearray()


def run(ser: serial.Serial):
    # NOTE: Edit this function to send / receive messages as desired
    write_msg(ser, b'MSB\x01')
    while True:
        id, msg = read_msg(ser)
        print("Receive {}: {}".format(id, msg))
    

def usage():
    print("Usage: test_comm.py port baud")
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
