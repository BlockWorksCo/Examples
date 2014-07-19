

import sys
from Display import *






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




if __name__ == '__main__':
    """
    """
    UseDisplay(sys.argv[1])    
    VertClock()


