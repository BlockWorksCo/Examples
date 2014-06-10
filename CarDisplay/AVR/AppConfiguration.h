



#ifndef __APPCONFIGURATION_H__
#define __APPCONFIGURATION_H__



#include "Platform.h"
#include "Display.h"
#include "CarDisplay.h"
#include "UART.h"
#include "SimpleBinaryProtocol.h"
#include "Queue.h"


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
         


typedef Display< 2, //data
            	 3, // load
                 4, // clock
                 5 >     DisplayType;

typedef Queue<uint8_t, 128, uint8_t>     	RxQueueType;
typedef Queue<uint8_t, 64, uint8_t>     	TxQueueType;



typedef UART<19200,
             RxQueueType, TxQueueType,
             _UBRR0H, _UBRR0L, _UCSR0A, _UCSR0B, _UCSR0C, _UDR0, RXEN0, TXEN0, RXCIE0, UDRIE0, U2X0
             >     UARTType;

#if 0
typedef SimpleBinaryProtocol<UARTType, DisplayType, RxQueueType, TxQueueType, MessageHandlingPair>   ProtocolType;

typedef CarDisplay<DisplayType, ProtocolType, MessageHandlingPair>   CarDisplayType;


#endif


struct MessageHandlingPair 
{

	typedef CarDisplay<DisplayType, MessageHandlingPair>   handlerType;


	typedef SimpleBinaryProtocol<UARTType, DisplayType, RxQueueType, TxQueueType, MessageHandlingPair>   protocolType;

};







#endif


