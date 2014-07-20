


import serial
import time
import struct
import random
import sys
import os
import zmq
import ctypes
import thread



frameBuffer     = [0xaa]*40
topWindow       = None
C               = None
pubSocket       = None
subSocket       = None




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

            if type(columnByte) == 'str':
                print(columnByte)

            try:
                if ((columnByte) & (1<<y)) == 0:
                    colour  = 'gray'
                else:
                    colour  = 'red'
            except:
                print(type(columnByte))
                print(frameBuffer)
                sys.exit(-1)
            #b = C.create_polygon( offsetX+sx,offsetY+sy, offsetX+sx+10,offsetY+sy, offsetX+sx+10,offsetY+sy+10, offsetX+sx,offsetY+sy+10, fill=colour)
            b = C.create_oval( offsetX+sx,offsetY+sy,  offsetX+sx+10,offsetY+sy+10, fill=colour, outline='')
    top.after(10, redraw)


def key(event):
    global pubSocket

    if event.char == event.keysym:
        #msg = 'Normal Key %r' % event.char
        pass
    elif len(event.char) == 1:
        #msg = 'Punctuation Key %r (%r)' % (event.keysym, event.char)
        pass
    else:
        if event.keysym == 'Left':
            pubSocket.send('LeftMsg')
        if event.keysym == 'Right':
            pubSocket.send('RightMsg')
        if event.keysym == 'Down':
            pubSocket.send('DownMsg')



def int8_to_uint8(i):
    try:
        return ctypes.c_uint8(i).value
    except:
        print('[%s,%s]'%(str(i),type(i)))


def ShowDisplay():
    """
    """
    global top
    global C

    top = Tkinter.Tk()
    C = Tkinter.Canvas(top, bg="gray", height=200, width=550)

    top.bind_all('<Key>', key)

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











def DoTransition(fromId, toId, event):
    """
    """
    fromFileName    = '%s%s'%(fbName[os.name], fromId)
    toFileName      = '%s%s'%(fbName[os.name], toId)

    values = 40*[0xaa]

    fbStruct    = struct.Struct('B'*40)
    try:
        fromData    = fbStruct.unpack_from(open(fromFileName,'rb').read(40))
    except struct.error:
        fromData    = 40*[0]

    try:
        toData      = fbStruct.unpack_from(open(toFileName,'rb').read(40))
    except struct.error:
        toData    = 40*[0]

    for i in range(10):


        if event == 'RightMsg':
            split   = i*4
            values[0:split]     = toData[(40-split):]
            values[split:39]     = fromData[0:(40-split)]

        if event == 'LeftMsg':
            split   = 40-(i*4)
            values[0:split]     = fromData[(40-split):]
            values[split:39]     = toData[0:(40-split)]

        showFrame(values)
        time.sleep(0.05)



def DisplayDaemon(frameBufferList):
    """
    """
    #setIntensity(15)

    frameBufferId   = frameBufferList[0]
    fileName = '%s%s'%(fbName[os.name], frameBufferId)
    fd = open(fileName,'rb')

    fbStruct    = struct.Struct('B'*40)

    frameCount = 0
    frameIndex = 0
    while True:


        try:
            msg     = subSocket.recv(zmq.NOBLOCK)

            oldFrameIndex = frameIndex

            if msg == 'LeftMsg':
                frameIndex = (frameIndex + 1) % len(frameBufferList)
            if msg == 'RightMsg':
                frameIndex = (frameIndex - 1) % len(frameBufferList)

            print('Switch to %d'%frameIndex)

            DoTransition(frameBufferList[oldFrameIndex], frameBufferList[frameIndex], msg)
            frameBufferId   = frameBufferList[frameIndex]
            fileName = '%s%s'%(fbName[os.name], frameBufferId)

        except zmq.error.Again:
            pass


        fbData  = open(fileName,'rb').read(40)
        try:
            values  = fbStruct.unpack_from(fbData)
            #print('good data')
        except struct.error:
            #print('bad data')
            values  = [0x0]*40

        frameCount = frameCount + 1
        showFrame(values)
        time.sleep(0.02)



def PeriodicSwitch(pubSocket):
    """
    """
    while True:
        time.sleep(10.0)
        pubSocket.send('LeftMsg')



frameBufferId   = ''
fbName  = {'nt':'c:\\temp\\ledfb', 'posix':'/tmp/ledfb'}
frameBufferList     = []


if __name__ == '__main__':
    """
    """
    frameBufferList = sys.argv[1:]
    print(frameBufferList)

    zmqContext  = zmq.Context()
    pubSocket   = zmqContext.socket(zmq.PUB)
    pubSocket.bind('tcp://*:6633')

    subSocket   = zmqContext.socket(zmq.SUB)
    subSocket.connect("tcp://127.0.0.1:6633")
    subSocket.setsockopt(zmq.SUBSCRIBE,'')

    thread.start_new_thread(PeriodicSwitch, (pubSocket,) )


    try:
        ser = serial.Serial(port='/dev/ttyUSB0', baudrate=115200)
        DisplayDaemon(frameBufferList)
    except serial.serialutil.SerialException:
        import Tkinter

        ser     = []
        thread.start_new_thread(DisplayDaemon, (frameBufferList,) )
        ShowDisplay()




