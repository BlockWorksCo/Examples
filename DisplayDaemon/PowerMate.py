#!/usr/bin/python
import struct
import time
import sys
import datetime



def PowermateReader():
    """
    """

    infile_path = "/dev/input/by-id/usb-Griffin_Technology__Inc._Griffin_PowerMate-event-if00"

    #long int, long int, unsigned short, unsigned short, unsigned int
    FORMAT = 'llHHI'
    EVENT_SIZE = struct.calcsize(FORMAT)

    timestampOfLastEvent    = datetime.datetime.now()

    #open file in binary mode
    in_file = open(infile_path, "rb")

    event = in_file.read(EVENT_SIZE)

    while event:
        (tv_sec, tv_usec, type, code, value) = struct.unpack(FORMAT, event)
        timestampOfThisEvent    = datetime.datetime.now()
        microsecondsSinceLastEvent  = (timestampOfThisEvent - timestampOfLastEvent).microseconds
        #print(microsecondsSinceLastEvent)

        if  microsecondsSinceLastEvent > 250000:

            timestampOfLastEvent    = timestampOfThisEvent

            if type == 2 and value == 1:
                print('right')

            if type == 2 and value == 4294967295:
                print('left')


        if type == 1 and value == 1:
            print('down')

        if type == 1 and value == 0:
            print('up')

        event = in_file.read(EVENT_SIZE)

    in_file.close()




if __name__ == '__main__':
    """
    """
    PowermateReader()



