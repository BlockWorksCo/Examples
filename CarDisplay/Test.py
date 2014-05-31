


import serial
import time





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


ser = serial.Serial(
    port=2,
    baudrate=19200)




def clear():

	ser.write('%c%c'%(27,1))
	#ser.write('%c'%(27))
	#time.sleep(delay)
	#ser.write('%c'%(1))
	#time.sleep(delay)


def drawSprite(x,y,sprite):

	ser.write('%c%c'%(27,0))
	#ser.write('%c'%(27))
	#time.sleep(delay)
	#ser.write('%c'%(0))
	#time.sleep(delay)


	ser.write('%c%c%c'%( int(x),int(y),int(sprite) ))
	#ser.write( '%c'%( int(x) ) )
	#time.sleep(delay)
	#ser.write( '%c'%( int(y) ) )
	#time.sleep(delay)
	#ser.write( '%c'%( int(sprite) ) )
	#time.sleep(delay)




def drawChar(x,y, char):
	"""
	"""
	drawSprite(x,y, SpriteLUT[char])
	return 5


def drawText(x, y, text):
	"""
	"""
	for char in text:
		charWidth = drawChar(x,y, char)
		x 	= x + charWidth



def TestOne():
	"""
	"""
	y=0
	for x in range(-7,32):
		clear()
		drawSprite(x%256,y, '4')




while True:
	clear()
	drawText(0,0, 'Hello')
	time.sleep(0.5)
	clear()
	drawText(0,0, 'World')
	time.sleep(0.5)



ser.close() 


