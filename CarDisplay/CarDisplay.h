


#ifndef __CARDISPLAY_H__
#define __CARDISPLAY_H__


#include "AppConfiguration.h"






void ProcessRawByte(uint8_t byte);
void ProcessDataByte(uint8_t byte);

void DebugOut(uint8_t c);


template <typename DisplayType, typename CommsProtocolType>
class CarDisplay
{
public:

    CarDisplay(DisplayType _display, CommsProtocolType& _commsProtocol) :
            display(_display),
            commsProtocol(_commsProtocol)
    {

    }

    //
    //
    //
    void Run() 
    {
        //
        //
        //
        display.setup();
        DebugOut('*');            

        //
        //
        //
        display.clear();
        display.drawSprite('0', 0,0);
        display.drawSprite('1', 8,0);
        display.drawSprite('2', 16,0);
        display.drawSprite('3', 24,0);
        delay(1000);
        display.clear();

        //
        // Draw the frame to the display.
        //
        while(true)
        {
            display.drawFrame();

            commsProtocol.Process();        
        }
    }




private:

    DisplayType&        display;
    CommsProtocolType&  commsProtocol;

};


#endif


