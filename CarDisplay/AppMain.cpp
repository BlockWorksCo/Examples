




#if 0



#include <avr/io.h> 
#include <avr/interrupt.h> 
#include <util/delay.h> 

#define F_CPU 16000000UL
#define BAUD 9600

#include <util/setbaud.h>

void uart_init(void) {
    UBRR0H = UBRRH_VALUE;
    UBRR0L = UBRRL_VALUE;

#if USE_2X
    UCSR0A |= _BV(U2X0);
#else
    UCSR0A &= ~(_BV(U2X0));
#endif

    UCSR0C = _BV(UCSZ01) | _BV(UCSZ00); /* 8-bit data */
    UCSR0B = _BV(RXEN0) | _BV(TXEN0);   /* Enable RX and TX */
}



void uart_putchar(char c) {
    loop_until_bit_is_set(UCSR0A, UDRE0); /* Wait until data register empty. */
    UDR0 = c;
}

extern "C" void AppMain(void) {

    uart_init();

    while(1) {
        uart_putchar('A');
    }

}

#else




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
SPIRxQueueType                              spiRxQueue;
SPITxQueueType                              spiTxQueue;
SPIType                                     spi(spiRxQueue, spiTxQueue);
DisplayType                                 display(dataOutput, loadOutput, clockOutput);
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













#endif


