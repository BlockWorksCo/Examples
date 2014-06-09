


#include "Platform.h"
#include "AppConfiguration.h"






RxQueueType         rxQueue;
TxQueueType         txQueue;
DisplayType         display;
UARTType            uart0(rxQueue,txQueue);
ProtocolType        protocol(uart0, display, rxQueue, txQueue);
CarDisplayType      carDisplay(display, protocol);






ISR(USART_RX_vect)
{
    uart0.RxISR();
}





ISR(USART_UDRE_vect)
{
   uart0.TxISR();////
}





void DebugOut(uint8_t ch)
{
    bool blaa;
    txQueue.Put(ch, blaa);
    uart0.Process();
}




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
    uart0.begin();

    carDisplay.Run();
}










