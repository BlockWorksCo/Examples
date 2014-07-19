
import sys
from Display import *




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



if __name__ == '__main__':
    """
    """
    UseDisplay(sys.argv[1])
    TextDemo()


