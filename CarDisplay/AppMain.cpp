


#include "Platform.h"
#include "AppConfiguration.h"








DisplayType         display;
extern ringbuffer   rx_buffer; 
extern ringbuffer   tx_buffer; 
SerialPortType      serial0(&rx_buffer, &tx_buffer);
ProtocolType        protocol(serial0, display);
CarDisplayType      carDisplay(display, protocol);


template < uint8_t dataIn,
            uint8_t load,
            uint8_t clock,
            uint8_t power,
            uint8_t NUMBER_OF_8x8_MATRICES >
            uint8_t Display<dataIn, load, clock, power, NUMBER_OF_8x8_MATRICES>::frameBuffer[8*NUMBER_OF_8x8_MATRICES];







void DebugOut(uint8_t c)
{
    serial0.write(c);
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
    DebugOut('>');

    carDisplay.Run();
}










