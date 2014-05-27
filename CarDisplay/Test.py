


import serial
import time


delay=0.001

ser = serial.Serial(
    port=2,
    baudrate=19200)




def clear():

	ser.write('%c'%(27))
	time.sleep(delay)
	ser.write('%c'%(1))
	time.sleep(delay)


def drawSprite(x,y,sprite):

	ser.write('%c'%(27))
	time.sleep(delay)
	ser.write('%c'%(0))
	time.sleep(delay)


	ser.write( '%c'%( int(x) ) )
	time.sleep(delay)
	ser.write( '%c'%( int(y) ) )
	time.sleep(delay)
	ser.write( '%c'%( int(sprite) ) )
	time.sleep(delay)

y=0
for x in range(-7,30):
	clear()
	drawSprite(x%256,y, '4')
	y = y + 1


#drawSprite(255,0, '4')
#drawSprite(8,1, '4')
#drawSprite(16,2, '3')
#drawSprite(24,4, '3')
ser.close() 


