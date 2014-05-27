


import serial
import time




ser = serial.Serial(
    port=2,
    baudrate=19200)




def clear():

	ser.write('%c'%(27))
	time.sleep(0.1)
	ser.write('%c'%(1))
	time.sleep(0.1)


def drawSprite(x,y,sprite):

	ser.write('%c'%(27))
	time.sleep(0.1)
	ser.write('%c'%(0))
	time.sleep(0.1)


	ser.write( '%c'%( int(x) ) )
	time.sleep(0.1)
	ser.write( '%c'%( int(y) ) )
	time.sleep(0.1)
	ser.write( '%c'%( int(sprite) ) )
	time.sleep(0.1)

clear()
drawSprite(255,0, '3')
drawSprite(8,1, '3')
drawSprite(16,2, '3')
drawSprite(24,4, '3')
ser.close() 


