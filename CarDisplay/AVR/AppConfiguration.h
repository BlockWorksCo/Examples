



#ifndef __APPCONFIGURATION_H__
#define __APPCONFIGURATION_H__



#include "Display.h"
#include "CarDisplay.h"
#include "SerialPort.h"
#include "SimpleBinaryProtocol.h"



//
// Compile-time configuration.
// Define the configuration thru the use of typedefs and where needed c++11 template aliases or
// a substitute mechanism (inheritance).
//
/*
typedef AVROutput<0, 0, 0x01>                  GreenLEDType;
typedef AVROutput<0, 0, 0x02>                  OrangeLEDType;
typedef AVROutput<0, 0, 0x04>                  RedLEDType;
typedef AVROutput<0, 0, 0x08>                  BlueLEDType;
*/
typedef Display<2,
            3,
            4,
            5,
            4 > 	DisplayType;


typedef SerialPort<19200> 	SerialPortType;

typedef SimpleBinaryProtocol<SerialPortType, DisplayType> 	ProtocolType;

typedef CarDisplay<DisplayType, ProtocolType> 	CarDisplayType;







#endif


