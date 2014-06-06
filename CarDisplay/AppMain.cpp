


#include "Platform.h"
#include "AppConfiguration.h"






RxQueueType         rxQueue;
TxQueueType         txQueue;
DisplayType         display;
UARTType            uart0(rxQueue,txQueue);
ProtocolType        protocol(uart0, display, rxQueue, txQueue);
CarDisplayType      carDisplay(display, protocol);


template < uint8_t dataIn,
            uint8_t load,
            uint8_t clock,
            uint8_t power,
            uint8_t NUMBER_OF_8x8_MATRICES >
            uint8_t Display<dataIn, load, clock, power, NUMBER_OF_8x8_MATRICES>::frameBuffer[8*NUMBER_OF_8x8_MATRICES];









ISR(USART_RX_vect)
{
    uart0.RxISR();
}





ISR(USART_UDRE_vect)
{
    uart0.TxISR();
}





void DebugOut(uint8_t ch)
{
    UDR0 = ch;
    delay(1);
}



void Fn1()
{
    bool dataAvailableFlag  = false;
    uint8_t ch = protocol.rxQ.Get(dataAvailableFlag);
    if(dataAvailableFlag == true)
    {
        DebugOut(ch);
    }
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
    /*
    Flash();
    for(volatile uint32_t i=0; i<20000; i++);
    Flash();
    for(volatile uint32_t i=0; i<20000; i++);
 */   
    //init();

    uart0.begin();

    //while(&protocol.rxQ == &rxQueue) {init();Flash();DebugOut('*');}

    while(true)
    {
        //Fn1();
        protocol.Fn1();
    }

    carDisplay.Run();
}










