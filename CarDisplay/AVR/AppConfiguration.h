



#ifndef __APPCONFIGURATION_H__
#define __APPCONFIGURATION_H__



#include "Display.h"
#include "CarDisplay.h"
#include "UART.h"
#include "SimpleBinaryProtocol.h"
#include "Platform.h"
#include "wiring_private.h"
#include "Queue.h"


//
// Compile-time configuration.
// Define the configuration thru the use of typedefs and where needed c++11 template aliases or
// a substitute mechanism (inheritance).
//
typedef Display<2,
            3,
            4,
            5,
            4 >     DisplayType;



#define _UBRR0H     0xC5
#define _UBRR0L     0xC4
#define _UCSR0A     0xC0
#define _UCSR0B     0xC1
#define _UCSR0C     0xC2
#define _UDR0       0xC6
         



typedef Queue<uint8_t, 64, uint8_t>     RxQueueType;
typedef Queue<uint8_t, 64, uint8_t>     TxQueueType;

typedef UART<19200,
             RxQueueType, TxQueueType,
             _UBRR0H, _UBRR0L, _UCSR0A, _UCSR0B, _UCSR0C, _UDR0, RXEN0, TXEN0, RXCIE0, UDRIE0, U2X0>     UARTType;

typedef SimpleBinaryProtocol<UARTType, DisplayType>   ProtocolType;

typedef CarDisplay<DisplayType, ProtocolType>   CarDisplayType;







#endif


