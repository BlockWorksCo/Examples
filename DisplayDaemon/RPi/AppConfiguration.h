



#ifndef __APPCONFIGURATION_H__
#define __APPCONFIGURATION_H__


#include "Platform.h"
#include "UART.h"
#include "SimpleBinaryProtocol.h"
#include "Queue.h"






//
// Compile-time configuration.
// Define the configuration thru the use of typedefs and where needed c++11 template aliases or
// a substitute mechanism (inheritance).
//

typedef Queue<uint8_t, 8, uint8_t>     RxQueueType;
typedef Queue<uint8_t, 8, uint8_t>     TxQueueType;

typedef UART<RxQueueType, TxQueueType,
             115200>                    UARTType;


#endif


