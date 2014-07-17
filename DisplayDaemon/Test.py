


import time
import ctypes
import datetime
import struct
import random
import sys
import os




SpriteLUT = \
{
    ' ':0,
    '!':1,
    '"':2,
    '#':3,
    '$':4,
    '%':5,
    '&':6,
    '\'':7,
    '(':8,
    ')':9,
    '*':10,
    '+':11,
    ',':12,
    '-':13,
    '.':14,
    '/':15,
    '0':16,
    '1':17,
    '2':18,
    '3':19,
    '4':20,
    '5':21,
    '6':22,
    '7':23,
    '8':24,
    '9':25,
    ':':26,
    ';':27,
    '<':28,
    '=':29,
    '>':30,
    '?':31,
    '@':32,
    'A':33,
    'B':34,
    'C':35,
    'D':36,
    'E':37,
    'F':38,
    'G':39,
    'H':40,
    'I':41,
    'J':42,
    'K':43,
    'L':44,
    'M':45,
    'N':46,
    'O':47,
    'P':48,
    'Q':49,
    'R':50,
    'S':51,
    'T':52,
    'U':53,
    'V':54,
    'W':55,
    'X':56,
    'Y':57,
    'Z':58,
    '[':59,
    '\\':60,
    ']':61,
    '^':62,
    '_':63,
    '`':64,
    'a':65,
    'b':66,
    'c':67,
    'd':68,
    'e':69,
    'f':70,
    'g':71,
    'h':72,
    'i':73,
    'j':74,
    'k':75,
    'l':76,
    'm':77,
    'n':78,
    'o':79,
    'p':80,
    'q':81,
    'r':82,
    's':83,
    't':84,
    'u':85,
    'v':86,
    'w':87,
    'x':88,
    'y':89,
    'z':90,
    '{':91,
    '|':92,
    '}':93,
    '~':94,
}


SpriteWidths = \
[
    2,
    1,
    3,
    5,
    4,
    5,
    5,
    1,
    3,
    3,
    5,
    5,
    2,
    4,
    2,
    4,
    4,
    3,
    4,
    4,
    4,
    4,
    4,
    4,
    4,
    4,
    2,
    2,
    3,
    3,
    3,
    4,
    5,
    4,
    4,
    4,
    4,
    4,
    4,
    4,
    4,
    3,
    4,
    4,
    4,
    5,
    5,
    4,
    4,
    4,
    4,
    4,
    5,
    4,
    5,
    5,
    5,
    5,
    4,
    2,
    4,
    2,
    3,
    4,
    2,
    4,
    4,
    4,
    4,
    4,
    3,
    4,
    4,
    3,
    4,
    4,
    3,
    5,
    4,
    4,
    4,
    4,
    4,
    4,
    3,
    4,
    5,
    5,
    5,
    4,
    3,
    3,
    1,
    3,
    4,
]





def lowIntensity():
    """
    """
    #SendMessage(2, struct.pack('B',1) )
    pass

def highIntensity():
    """
    """
    #SendMessage(2, struct.pack('B',15) )
    pass


def setIntensity(value):
    """
    """
    #SendMessage(2, struct.pack('B',value) )
    pass


def clear():
    """
    """
    showFrame(struct.pack('40B',*(40*[0]) ) )


def clearWithValue(value):
    """
    """
    showFrame(struct.pack('40B',*(40*[value]) ) )


def showFrame(frame):
    """
    """
    frame = struct.pack('40B',*(frame) )
    #frame = struct.pack('40B',*(40*[127]) )
    fbName  = {'nt':'c:\\temp\\ledfb', 'posix':'/tmp/ledfb'}
    open(fbName[os.name],'wb').write(frame)


def drawFrame():
    """
    """
    showFrame()


def int8_to_uint8(i):
    return ctypes.c_uint8(i).value


def drawSprite(x,y,sprite):
    """
    """
    pass
    #Send('%c%c'%(27,0))
    #Send('%c%c%c'%( int8_to_uint8(x),int8_to_uint8(y),int(sprite) ))


def drawChar(x,y, char):
    """
    """
    try:
        asciiValue   = int(ord(char))
    except ValueError:
        asciiValue   = ord(char)

    char    = chr(asciiValue)
    spriteValue = SpriteLUT[char]
    #print('drawChar(%c): ascii:%02d sprite:%02d'%(char,asciiValue,spriteValue))
    drawSprite(x,y, spriteValue)
    return SpriteWidths[ spriteValue ]


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
        drawFrame()
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

        drawFrame()
        time.sleep(0.0)



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
        lowIntensity()
        y = 0
        #drawText(10,y+8, '>World<')
        #drawText(0,y, '>>> Hello <<<')
        drawText(0,y, 'BlockWorks')
        drawFrame()
        time.sleep(0.5)


def t1():
    """
    """
    for x in range(0,30):
        #clear()
        drawText(0,0, 'BlockWorks')
        time.sleep(1.0)


def BlockWorks():
    """
    """
    for x in range(-20, 1):
        clear();
        drawText(x,0, 'Block')
        drawFrame()
        time.sleep(0.01)

    time.sleep(0.5)

    for x in range(40, 18, -1):
        #clear();
        #drawText(0,0, 'Block')
        drawText(x,0, 'Works')
        drawFrame()
        time.sleep(0.01)



def BlockWorks2():
    """
    """
    for x in range(0, 20):
        #clear();
        drawText(-19+x,0, 'Block')
        drawText(38-x,0, 'Works')
        drawFrame()
        time.sleep(0.01)


def BlockWorks3():
    """
    """
    for x in range(0, 9):
        clear();
        drawText(0,-x, 'Block')
        drawText(18,x, 'Works')
        drawFrame()
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
        drawFrame()
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



setIntensity(7)

Analyser()
#JitterBug()
#ImageTestThree()
ImageTestTwo()
#t1()
TextDemo()
#t0()
#VertTest()
VertClock()


