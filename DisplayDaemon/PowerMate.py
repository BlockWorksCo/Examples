#!/usr/bin/python
import struct
import time
import sys

infile_path = "/dev/input/by-id/usb-Griffin_Technology__Inc._Griffin_PowerMate-event-if00"

#long int, long int, unsigned short, unsigned short, unsigned int
FORMAT = 'llHHI'
EVENT_SIZE = struct.calcsize(FORMAT)

#open file in binary mode
in_file = open(infile_path, "rb")

event = in_file.read(EVENT_SIZE)

while event:
    (tv_sec, tv_usec, type, code, value) = struct.unpack(FORMAT, event)

    if type == 1 and value == 1:
    	print('down')

    if type == 1 and value == 0:
    	print('up')

    if type == 2 and value == 1:
    	print('right')

    if type == 2 and value == 4294967295:
    	print('left')

    #if type != 0 or code != 0 or value != 0:
    #    print("Event type %u, code %u, value: %u at %d, %d" % \
    #        (type, code, value, tv_sec, tv_usec))
    #else:
    #    # Events with code, type and value == 0 are "separator" events
    #    print("===========================================")

    event = in_file.read(EVENT_SIZE)

in_file.close()


