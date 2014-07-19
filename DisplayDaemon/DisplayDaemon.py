


import serial
import time
import struct
import thread
import Tkinter
import random


frameBuffer     = [0xaa]*40
topWindow       = None
C               = None

def redraw():
    """
    """
    C.delete('all')
    offsetX     = 20
    offsetY     = 20
    for x in range(0,40):

        columnByte  = frameBuffer[x]

        for y in range(0,8):
            sx = x*12
            sy = y*12

            if (columnByte & (1<<y)) == 0:
                colour  = 'gray'
            else:
                colour  = 'red'
            #b = C.create_polygon( offsetX+sx,offsetY+sy, offsetX+sx+10,offsetY+sy, offsetX+sx+10,offsetY+sy+10, offsetX+sx,offsetY+sy+10, fill=colour)
            b = C.create_oval( offsetX+sx,offsetY+sy,  offsetX+sx+10,offsetY+sy+10, fill=colour, outline='')
    top.after(10, redraw)



def ShowDisplay():
    """
    """
    global top
    global C

    top = Tkinter.Tk()
    C = Tkinter.Canvas(top, bg="gray", height=200, width=550)

    C.pack()
    top.after(100, redraw)
    top.mainloop()



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
    if ser != []:
        SendMessage(2, struct.pack('B',value) )
    else:
        pass


def showFrame(frame):
    """
    """
    if ser != []:
        SendMessage(6, struct.pack('40B',*frame ) )
    else:
        global frameBuffer
        frameBuffer = frame















def DisplayDaemon(fileName):
    """
    """
    #setIntensity(15)

    fd = open(fileName,'rb')
    fbStruct    = struct.Struct('B'*40)
    while True:

        fd.seek(0,0)
        fbData  = fd.read(40)
        try:
            values  = fbStruct.unpack_from(fbData)
            #print('good data')
        except struct.error:
            #print('bad data')
            values  = [0x0]*40

        showFrame(values)
        time.sleep(0.02)








try:
    ser = serial.Serial(port='/dev/ttyUSB0', baudrate=115200)
    DisplayDaemon('/tmp/ledfb')
except serial.serialutil.SerialException:
    ser     = []
    thread.start_new_thread(DisplayDaemon, ('c:\\temp\ledfb',) )
    ShowDisplay()




