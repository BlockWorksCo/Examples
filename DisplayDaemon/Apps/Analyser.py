

import sys
from Display import *






def Analyser():
    """
    """
    barValues   = [255,127,63,31,15,7,3,1,0]

    c = 0
    values  = 40*[1]
    while True:
        frame = getFrame()

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

        showFrame()
        time.sleep(0.02)
        #print('------')





if __name__ == '__main__':
    """
    """
    UseDisplay(sys.argv[1])    
    Analyser()


