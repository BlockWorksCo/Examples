


#ifndef __CARDISPLAY_H__
#define __CARDISPLAY_H__


#include "AppConfiguration.h"






void ProcessRawByte(uint8_t byte);
void ProcessDataByte(uint8_t byte);


template <typename DisplayType, typename SerialPortType>
class CarDisplay
{
public:

	CarDisplay(DisplayType _display, SerialPortType _serialPort)
		: display(_display),
		  serialPort(_serialPort)
	{

	}

	//
	//
	//
	void Run() 
	{
	    //
	    // Draw the frame to the display.
	    //
	    display.drawFrame();
	    serialPort.println(".");

	    //
	    //
	    //
	    while(serialPort.available() != 0)
	    {
	        uint8_t     ch  = serialPort.read();

	        ProcessRawByte(ch);
	    }

	}




private:

	DisplayType& 	display;
	SerialPortType 	serialPort;

};


#endif


