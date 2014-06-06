



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


typedef Display<2,
            3,
            4,
            5,
            5 >     DisplayType;

typedef Queue<uint8_t, 8, uint8_t>     RxQueueType;
typedef Queue<uint8_t, 8, uint8_t>     TxQueueType;

typedef UART<3,
             RxQueueType, TxQueueType,
             19200>     UARTType;

typedef SimpleBinaryProtocol<UARTType, DisplayType, RxQueueType, TxQueueType>   ProtocolType;

typedef CarDisplay<DisplayType, ProtocolType>   CarDisplayType;


#endif


