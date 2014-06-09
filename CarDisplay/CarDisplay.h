


#ifndef __CARDISPLAY_H__
#define __CARDISPLAY_H__


#include "AppConfiguration.h"






template <typename DisplayType, typename CommsProtocolType>
class CarDisplay
{
public:

    CarDisplay(DisplayType& _display, CommsProtocolType& _commsProtocol) :
            display(_display),
            commsProtocol(_commsProtocol)
    {
    }

    //
    //
    //
    void Run() 
    {
        static uint8_t  text[]      = {34,76,79,67,75,55,79,82,75,83};
        static uint8_t  widths[]    = {4, 3, 4, 4, 4, 5, 4, 4, 4, 4};

        //
        //
        //
        display.setup();

        //
        // 34,76,79,67,75,55,79,82,75,83
        // 4, 3, 4, 4, 4, 5, 4, 4, 4, 4
        //        
        display.clear();
        uint8_t x   = 0;
        for(uint8_t i=0; i<sizeof(text); i++ )
        {
            display.drawSprite(text[i], x,0);
            x   += widths[i];
        }
        display.drawFrame();

        //
        // Draw the frame to the display.
        //
        sei();
        while(true)
        {
            sei();
            commsProtocol.Process();        
        }
    }




private:

    DisplayType&        display;
    CommsProtocolType&  commsProtocol;

};


#endif


