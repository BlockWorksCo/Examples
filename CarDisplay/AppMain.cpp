



#include "Platform.h"
#include "AppConfiguration.h"





//
//
//
RxQueueType                                 rxQueue;
TxQueueType                                 txQueue;
DataOutputType                              dataOutput;
LoadOutputType                              loadOutput;
ClockOutputType                             clockOutput;
DisplayType                                 display(dataOutput, loadOutput, clockOutput);

SPIRxQueueType                              spiRxQueue;
SPITxQueueType                              spiTxQueue;
SPIType                                     spi(spiRxQueue, spiTxQueue);
MCP2515ChipSelectType                       mcp2515ChipSelect;
MCP2515Type                                 mcp2515(spi, mcp2515ChipSelect);

UARTType                                    uart0(rxQueue,txQueue);

extern MessageHandlingPair::handlerType     carDisplay;
MessageHandlingPair::protocolType           protocol(uart0, display, rxQueue, txQueue, carDisplay);
MessageHandlingPair::handlerType            carDisplay(display, protocol);






ISR(USART_RX_vect)
{
    uart0.RxISR();
}


ISR(USART_UDRE_vect)
{
   uart0.TxISR();
}


ISR(SPI_STC_vect)
{
    spi.TransferCompleteISR();
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
    uart0.start();

    carDisplay.Run();
}

