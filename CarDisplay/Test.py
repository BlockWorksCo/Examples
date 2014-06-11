


import serial
import time
import ctypes
import datetime
import struct




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




ser = serial.Serial(port=2, baudrate=38400)


def ByteOut(value):
    """
    """
    ser.write('%c'%(value))


def Send(data):
    for ch in data:
        if ch == 27:
            ByteOut(27)
            ByteOut(27)
        else:
            ByteOut(ch)




def SendMessage(type, payload):
    """
    """

    ByteOut(27)
    ByteOut(0)

    checksum    = 0
    length      = len(payload)
    Send( struct.pack('BBB',length, checksum, type) )
    Send( payload )

    ser.write('%c%c'%(27,255))

    ByteOut(27)
    ByteOut(255)



def lowIntensity():
    """
    """
    SendMessage(2, struct.pack('B',1) )

def highIntensity():
    """
    """
    SendMessage(2, struct.pack('B',15) )


def clear():
    """
    """
    SendMessage(1, struct.pack('B',0) )


def drawFrame():
    """
    """
    SendMessage(4, struct.pack('B',0) )


def int8_to_uint8(i):
    return ctypes.c_uint8(i).value


def drawSprite(x,y,sprite):
    """
    """
    SendMessage(5, struct.pack('BBB',int8_to_uint8(x),int8_to_uint8(y),int(sprite)) )

    #Send('%c%c'%(27,0))
    #Send('%c%c%c'%( int8_to_uint8(x),int8_to_uint8(y),int(sprite) ))


def drawChar(x,y, char):
    """
    """
    drawSprite(x,y, SpriteLUT[char])
    return SpriteWidths[ SpriteLUT[char] ]


def drawText(x, y, text):
    """
    """
    for char in text:
        charWidth = drawChar(x,y, char)
        x   = x + charWidth










def TestOne():
    """
    """
    y=0
    for x in range(-7,32):
        clear()
        drawSprite(x%256,y, '4')




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
        time.sleep(0.5)
        VertScroll('World     ','BlockWorks')
        time.sleep(0.5)
        VertScroll('BlockWorks','Hello     ')
        time.sleep(0.5)


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
    time.sleep(1.0)
    for x in range(0,30):
        clear()
        drawText(x,0, 'Hello World')
        time.sleep(1.0)


def BlockWorks():
    """
    """
    while True:
        clear()
        drawFrame()

        for x in range(-20, 1):
            clear();
            drawText(x,0, 'Block')
            drawFrame()
            time.sleep(0.01)

        time.sleep(0.5)

        for x in range(40, 18, -1):
            clear();
            drawText(0,0, 'Block')
            drawText(x,0, 'Works')
            drawFrame()
            time.sleep(0.01)

        time.sleep(0.5)


#t0()
#BlockWorks()
#VertTest()
VertClock()
#ser.close() 


