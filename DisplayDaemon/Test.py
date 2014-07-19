


import time
import ctypes
import datetime
import struct
import random
import sys
import os
from Font import font



NUMBER_OF_8x8_MATRICES  = 5
fbName  = {'nt':'c:\\temp\\ledfb', 'posix':'/tmp/ledfb'}

cachedFrameBuffer   = [0]*40



def showFrame():
    """
    """
    global cachedFrameBuffer
    frame = cachedFrameBuffer

    frame = struct.pack('40B',*(frame) )
    open(fbName[os.name],'wb').write(frame)


def getFrame():
    """
    """
    return cachedFrameBuffer

    fbData      = open(fbName[os.name]).read(40)
    fbStruct    = struct.Struct('B'*40)
    try:
        values  = fbStruct.unpack_from(fbData)
    except struct.error:
        values  = [0x0]*40
    
    return list(values)



def clear():
    """
    """
    global cachedFrameBuffer
    cachedFrameBuffer   = [0]*40


def clearWithValue(value):
    """
    """
    showFrame(struct.pack('40B',*(40*[value]) ) )



def int8_to_uint8(i):
    return ctypes.c_uint8(i).value



def BitBlt(x, y, width, height, data):
    """
    Copy the source image into the framebuffer at the specified location.
    """
    
    #
    # Perform the clipping for x,y, width & height.
    #
    if x >= (NUMBER_OF_8x8_MATRICES*8):
        return
    
    if y < -height:    
        return    
    
    if x < -width:
        return
    
    if y >= 8:
        return
    
    if x+width > (NUMBER_OF_8x8_MATRICES*8) :
        width = (NUMBER_OF_8x8_MATRICES*8)-x
    
    startX = 0
    if x<0:
        width   = width + x
        startX  = -x
        x       = 0
    
    dest    = getFrame()

    #
    # Iterate over remaining data, copying it to the framebuffer.
    #
    if y >= 0:
        for i in range(0,width):
            #
            # dest[i] = 10101010 data[i] = 11001100 y=6 result = 101010xx | 00000011 = 10101011
            #
            mask = 0xff>>(8-y)
            previous = dest[x+i]
            dest[x+i]  = (previous & mask) | int8_to_uint8(data[startX+i]<<y);
                
    if y < 0:
        for i in range(0,width):
            #
            # dest[i] = 10101010  data[i] = 11001100 y = -2, result = 10xxxxxx | xx001100 = 10001100
            #
            absY = -y
            mask = 0xff<<(8-absY)
            previous = dest[x+i]
            dest[x+i]   = (previous & mask) | int8_to_uint8(data[startX+i]>>absY )

    #showFrame(dest)
            






def drawChar(x,y, char):
    """
    """
    width       = font[char]['Width']
    height      = font[char]['Height']
    data        = font[char]['Data']
    BitBlt(x,y, width, height, data)

    return width


def drawText(x, y, text):
    """
    """
    #print(text)
    for char in text:
        charWidth = drawChar(x,y, char)
        x   = x + charWidth










def VertScroll(topLine, bottomLine):
    """
    """
    for y in range(0,-8,-1):
        clear()
        drawText(0,y, topLine)
        drawText(0,y+8, bottomLine)
        showFrame()
        #time.sleep(0.01)


def VertDiffScroll(topLine, bottomLine):
    """
    """
    for y in range(0,-9,-1):

        x   = 0
        clear()
        for position in range( 0,len(topLine) ):

            if topLine[position] != bottomLine[position]:
                charWidth = drawChar(x,y, topLine[position])
                charWidth = drawChar(x,y+8, bottomLine[position])
            else:
                charWidth = drawChar(x,0, bottomLine[position])

            if topLine[position] == ' ':
                charWidth = 4
            else:
                charWidth = 5
            x   = x + charWidth

        showFrame()
        time.sleep(0.01)



def VertClock():
    """
    """
    prevTime = ''
    while True:
        currentTime = datetime.datetime.now().strftime('%I %M %S')
        print('[%s][%s]'%(prevTime,currentTime))
        VertDiffScroll(prevTime, currentTime)
        time.sleep(1.0)
        prevTime = currentTime
        showFrame()



def VertTest():
    """
    """

    while True:
        VertScroll('Hello     ','World     ')
        time.sleep(0.2)
        VertScroll('World     ','BlockWorks')
        time.sleep(0.2)
        VertScroll('BlockWorks','Hello     ')
        time.sleep(0.2)


def t0():
    while True:
        clear()
        y = 0
        #drawText(10,y+8, '>World<')
        #drawText(0,y, '>>> Hello <<<')
        drawText(0,y, 'BlockWorks')
        showFrame()
        time.sleep(0.5)


def t1():
    """
    """
    for x in range(0,30):
        #clear()
        showFrame(0,0, 'BlockWorks')
        time.sleep(0.0)


def BlockWorks():
    """
    """
    for x in range(-20, 1):
        clear();
        drawText(x,0, 'Block')
        showFrame()
        time.sleep(0.01)

    time.sleep(0.5)

    for x in range(40, 18, -1):
        #clear();
        #drawText(0,0, 'Block')
        drawText(x,0, 'Works')
        showFrame()
        time.sleep(0.01)



def BlockWorks2():
    """
    """
    for x in range(0, 20):
        #clear();
        drawText(-19+x,0, 'Block')
        drawText(38-x,0, 'Works')
        showFrame()
        time.sleep(0.01)


def BlockWorks3():
    """
    """
    for x in range(0, 9):
        clear();
        drawText(0,-x, 'Block')
        drawText(18,x, 'Works')
        showFrame()
        time.sleep(0.01)



def TextDemo():
    """
    """
    while True:
        BlockWorks2()
        time.sleep(0.5)
        BlockWorks()
        time.sleep(0.5)
        BlockWorks3()
        time.sleep(0.5)


def ImageTestOne():
    c = 0
    while True:
        clearWithValue(c)
        c = c + 1
        time.sleep(0.5)


def ImageTestTwo():
    c = 0
    frame = 40*[0xaa]
    while True:

        newFrame        = frame[1:]
        c = int(random.random() * 256)
        #c = (c  + 1) % 255
        newFrame.append(c)
        frame           = newFrame

        showFrame(frame)
        time.sleep(0.015)
        #print('------')


def ImageTestThree():
    c = 0
    frame = 40*[0xaa]
    while True:

        for i in range(0,40):
            c = int(random.random() * 256)
            frame[i] = c

        showFrame(frame)
        time.sleep(0.02)
        #print('------')



def JitterBug():
    """
    """

    while True:
        clear()
        dx = int(random.random()*3)
        dy = int(random.random()*3)
        drawText(1+dx-2,1+dy-2, 'BlockWorks')
        showFrame()
        time.sleep(0.03)


def Analyser():
    """
    """
    barValues   = [255,127,63,31,15,7,3,1,0]

    c = 0
    values  = 40*[1]
    frame = 40*[0xaa]
    while True:

        for i in range(0,40):
            values[i] = values[i]-1

        for i in range(0,40):
            if random.random() > 0.5:
                values[i] = values[i] + 2

            if values[i] > 8:
                values[i] = 8;
            if values[i] < 0:
                values[i] = 0

            frame[i] = 255-barValues[ values[i] ]

        showFrame(frame)
        time.sleep(0.02)
        #print('------')



#setIntensity(7)

#Analyser()
#JitterBug()
#ImageTestThree()
#ImageTestTwo()
#t1()
#TextDemo()
#t0()
#VertTest()
VertClock()


