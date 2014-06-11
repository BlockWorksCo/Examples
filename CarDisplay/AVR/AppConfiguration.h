



#ifndef __APPCONFIGURATION_H__
#define __APPCONFIGURATION_H__



#include "Platform.h"
#include "Display.h"
#include "CarDisplay.h"
#include "UART.h"
#include "SimpleBinaryProtocol.h"
#include "Queue.h"
#include "GPIO.h"

//
// Compile-time configuration.
// Define the configuration thru the use of typedefs and where needed c++11 template aliases or
// a substitute mechanism (inheritance).
//



#define _UBRR0H     0xC5
#define _UBRR0L     0xC4
#define _UCSR0A     0xC0
#define _UCSR0B     0xC1
#define _UCSR0C     0xC2
#define _UDR0       0xC6
         



typedef AVROutput<2> 	DataOutputType;
typedef AVROutput<3> 	LoadOutputType;
typedef AVROutput<4> 	ClockOutputType;


typedef Display< 2, //data
            	 3, // load
                 4, // clock
                 5,
                 DataOutputType,
                 LoadOutputType,
                 ClockOutputType>     DisplayType;

typedef Queue<uint8_t, 128, uint8_t>     	RxQueueType;
typedef Queue<uint8_t, 64, uint8_t>     	TxQueueType;



typedef UART<115200,
             RxQueueType, TxQueueType,
             _UBRR0H, _UBRR0L, _UCSR0A, _UCSR0B, _UCSR0C, _UDR0, RXEN0, TXEN0, RXCIE0, UDRIE0, U2X0
             >     UARTType;


struct MessageHandlingPair 
{

	typedef union
	{
	    uint8_t     uint8Value;
	    uint16_t    uint16Value;
	    uint8_t     byteValues[4];

	} PayloadType;

	typedef struct
	{
	    uint8_t     type;
	    PayloadType payload;
	} MessageType;


	typedef CarDisplay<DisplayType, MessageHandlingPair>   handlerType;


	typedef SimpleBinaryProtocol<UARTType, DisplayType, RxQueueType, TxQueueType, MessageHandlingPair>   protocolType;

};







#endif


