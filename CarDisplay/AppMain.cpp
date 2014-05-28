


#include "Platform.h"
#include "AppConfiguration.h"








DisplayType         display;
extern ring_buffer  rx_buffer; 
extern ring_buffer  tx_buffer; 
SerialPort          serial0;
ProtocolType        protocol(serial0, display);
CarDisplayType      carDisplay(display, protocol);











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

    serial0.attach(&rx_buffer, &tx_buffer, &UBRR0H, &UBRR0L, &UCSR0A, &UCSR0B, &UCSR0C, &UDR0, RXEN0, TXEN0, RXCIE0, UDRIE0, U2X0);
    serial0.begin(19200);
    serial0.println("Hello World.");

    display.setup();

    while(true)
    {
        carDisplay.Run();
    }
}










