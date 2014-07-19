


import time
import ctypes
import datetime
import struct
import random
import sys
import os
from Font import font



frameBufferId   = ''
NUMBER_OF_8x8_MATRICES  = 5
fbName  = {'nt':'c:\\temp\\ledfb', 'posix':'/tmp/ledfb'}

cachedFrameBuffer   = [0]*40



def showFrame():
    """
    """
    global cachedFrameBuffer
    frame = cachedFrameBuffer

    frame = struct.pack('40B',*(frame) )
    fileName = '%s%s'%(fbName[os.name], frameBufferId)
    open(fileName,'wb').write(frame)


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



def UseDisplay(id):
    """
    """
    global frameBufferId
    frameBufferId   = id




