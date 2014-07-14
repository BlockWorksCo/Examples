



#include "Platform.h"
#include "AppConfiguration.h"





//
//
//

RxQueueType        rxQueue;
TxQueueType        txQueue;

UARTType           uart0("/dev/ttyS0", rxQueue,txQueue);





//
// Entry point for the application.
// 
// Note1: The main application object is created static within main instead
// of being global in order to control initialisation-time.
// Note2: 'Run' methods are one-shot. The loop is implicit to allow for transition
// between threaded and non-threaded mechanisms.
//
extern "C" void AppMain()
{
    bool    elementDroppedFlag  = false;
    
    while(true)
    {
        txQueue.Put('a', elementDroppedFlag);
        uart0.ProcessQueue();
    }
}

