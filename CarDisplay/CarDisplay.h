


#ifndef __CARDISPLAY_H__
#define __CARDISPLAY_H__


#include "AppConfiguration.h"






template <typename DisplayType, typename pair>
class CarDisplay
{

    typedef typename pair::protocolType CommsProtocolType;
    typedef typename pair::MessageType  MessageType;

public:

    CarDisplay(DisplayType& _display, CommsProtocolType& _commsProtocol) :
            display(_display),
            commsProtocol(_commsProtocol)
    {
    }


    //
    //
    //
    void ProcessMessage(MessageType& message, uint8_t numberOfBytes)
    {
        switch(message.type)
        {
            case 0:
                break;

            case 1:
                display.clear();
                break;

            case 2:
                display.setIntensity(message.payload.byteValues[0]);
                break;

            case 4:
                display.drawFrame( display.frameBuffer );
                break;

            case 5:
            {
                uint8_t     xPos        = message.payload.byteValues[0];
                uint8_t     yPos        = message.payload.byteValues[1];
                uint8_t     spriteId    = message.payload.byteValues[2];
                display.drawSprite(spriteId, xPos, yPos);                
                break;
            }

            case 6:
            {
                display.drawFrame( message.payload.frame );
                break;
            }


            default:
                break;
        }
    }


    //
    //
    //
    void Initialise()
    {
        static uint8_t  text[]      = {34,76,79,67,75,55,79,82,75,83};
        static uint8_t  widths[]    = {4, 3, 4, 4, 4, 5, 4, 4, 4, 4};

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
        display.drawFrame( display.frameBuffer );        
    }


private:

    DisplayType&        display;
    CommsProtocolType&  commsProtocol;

};


#endif


