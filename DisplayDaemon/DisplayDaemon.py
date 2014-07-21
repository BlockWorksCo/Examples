


import time
import struct
import random
import sys
import os
import zmq
import ctypes
import thread



frameBuffer     = [0xaa]*40
pubSocket       = None
subSocket       = None




class GUIDisplay:
    """
    """

    topWindow       = None
    C               = None


    def __init__(self):
        """
        """
        import Tkinter
        self.ShowDisplay()


    def redraw(self):
        """
        """
        self.C.delete('all')
        offsetX     = 20
        offsetY     = 20
        for x in range(0,40):

            columnByte  = frameBuffer[x]

            for y in range(0,8):
                sx = x*12
                sy = y*12

                if ((columnByte) & (1<<y)) == 0:
                    colour  = 'gray'
                else:
                    colour  = 'red'
                b = self.C.create_oval( offsetX+sx,offsetY+sy,  offsetX+sx+10,offsetY+sy+10, fill=colour, outline='')

        self.top.after(10, self.redraw)


    def key(self, event):
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


    def ShowDisplay(self):
        """
        """
        import Tkinter

        self.top = Tkinter.Tk()
        self.C = Tkinter.Canvas(self.top, bg="gray", height=200, width=550)

        self.top.bind_all('<Key>', self.key)

        self.C.pack()
        self.top.after(100, self.redraw)



    def setIntensity(self, value):
        """
        """
        pass


    def showFrame(self, frame):
        """
        """
        global frameBuffer
        frameBuffer = frame


    def Run(self):
        """
        """
        self.top.mainloop()



        

class LEDDisplay:
    """
    """

    def __init__(self):
        """
        """
        import serial
        self.ser = serial.Serial(port='/dev/ttyUSB0', baudrate=115200)
        

    def setIntensity(self, value):
        """
        """
        self.SendMessage(2, struct.pack('B',value) )


    def showFrame(self, frame):
        """
        """
        self.SendMessage(6, struct.pack('40B',*frame ) )


    def Flush(self):
        """
        """
        global outBuffer
        data    = struct.pack('B'*len(outBuffer), *outBuffer)
        self.ser.write(data)
        outBuffer   = []


    def ByteOut(self, value):
        """
        """
        outBuffer.append(value)


    def Send(self, data):
        """
        """
        for ch in data:
            try:
                value   = int(ord(ch))
            except ValueError:
                value   = ord(ch)

            if value == 27:
                self.ByteOut(27)
                self.ByteOut(27)
            else:
                self.ByteOut(value)


    def SendMessage(self, type, payload):
        """
        """

        self.ByteOut(27)
        self.ByteOut(0)

        checksum    = 0
        length      = len(payload)+3
        self.Send( struct.pack('BBB',length, checksum, type) )
        self.Send( payload )

        self.ByteOut(27)
        self.ByteOut(255)

        self.Flush()


    def Run(self):
        """
        """
        while True:
            time.sleep(1.0)







class TextDisplay:
    """
    """


    def __init__(self):
        """
        """
        pass
        

    def setIntensity(self, value):
        """
        """
        pass


    def showFrame(self, frame):
        """
        """
        global frameBuffer
        frameBuffer = frame
        
        sys.stdout.write('\x1b[2J')
        sys.stdout.write('\x1b[1;1H')
        for y in range(0,8):

            for x in range(0,40):

                columnByte  = frameBuffer[x]
                sx = x*12
                sy = y*12

                if ((columnByte) & (1<<y)) == 0:
                    character   = '.'
                else:
                    character   = 'O'
                sys.stdout.write(character)

            sys.stdout.write('\n')

    def Run(self):
        """
        """
        while True:
            time.sleep(1.0)












outBuffer   = []





def DoTransition(display, fromId, toId, event):
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

        display.showFrame(values)
        time.sleep(0.05)



def DisplayDaemon(display, frameBufferList):
    """
    """
    #setIntensity(15)

    frameBufferId   = frameBufferList[0]
    fileName = '%s%s'%(fbName[os.name], frameBufferId)
    fd = open(fileName,'rb')

    fbStruct    = struct.Struct('B'*40)

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

            DoTransition(display, frameBufferList[oldFrameIndex], frameBufferList[frameIndex], msg)
            frameBufferId   = frameBufferList[frameIndex]
            fileName = '%s%s'%(fbName[os.name], frameBufferId)

        except zmq.error.Again:
            pass


        fbData  = open(fileName,'rb').read(40)
        try:
            values  = fbStruct.unpack_from(fbData)
        except struct.error:
            values  = [0x0]*40

        display.showFrame(values)
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

    #display     = GUIDisplay()
    #display     = LEDDisplay()
    display     = TextDisplay()

    thread.start_new_thread(DisplayDaemon, (display,frameBufferList) )

    display.Run()





