


#ifndef __CARDISPLAY_H__
#define __CARDISPLAY_H__


#include "AppConfiguration.h"






void ProcessRawByte(uint8_t byte);
void ProcessDataByte(uint8_t byte);


template <typename DisplayType, typename CommsProtocolType>
class CarDisplay
{
public:

	CarDisplay(DisplayType _display, CommsProtocolType& _commsProtocol) :
			display(_display),
		  	commsProtocol(_commsProtocol)
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

	    commsProtocol.Process();
	}




private:

	DisplayType& 		display;
	CommsProtocolType& 	commsProtocol;

};


#endif


