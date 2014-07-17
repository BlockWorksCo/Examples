


import serial
import time
import struct




try:
    ser = serial.Serial(port='/dev/ttyUSB0', baudrate=115200)
except serial.serialutil.SerialException:
    ShowDisplay()



outBuffer   = []


def Flush():
    """
    """
    global outBuffer
    data    = struct.pack('B'*len(outBuffer), *outBuffer)
    ser.write(data)
    outBuffer   = []


def ByteOut(value):
    """
    """
    outBuffer.append(value)
    #ser.write('%c'%(value))


def Send(data):
    for ch in data:
        try:
            value   = int(ord(ch))
        except ValueError:
            value   = ord(ch)

        if value == 27:
            ByteOut(27)
            ByteOut(27)
        else:
            ByteOut(value)




def SendMessage(type, payload):
    """
    """

    ByteOut(27)
    ByteOut(0)

    checksum    = 0
    length      = len(payload)+3
    Send( struct.pack('BBB',length, checksum, type) )
    Send( payload )

    ByteOut(27)
    ByteOut(255)

    Flush()



def setIntensity(value):
    """
    """
    SendMessage(2, struct.pack('B',value) )


def showFrame(frame):
    """
    """
    SendMessage(6, struct.pack('40B',*frame ) )














def DisplayDaemon(fileName):
    """
    """
    fd = open(fileName,'rb')
    fbStruct    = struct.Struct('B'*40)
    while True:

        fd.seek(0,0)
        fbData  = fd.read(40)
        values  = fbStruct.unpack_from(fbData)

        showFrame(values)
        time.sleep(0.02)




setIntensity(15)
DisplayDaemon('/tmp/ledfb')

