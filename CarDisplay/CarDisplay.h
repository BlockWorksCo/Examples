


#ifndef __CARDISPLAY_H__
#define __CARDISPLAY_H__


#include "AppConfiguration.h"






template <typename DisplayType, typename pair>
class CarDisplay
{

    typedef typename pair::protocolType CommsProtocolType;
public:

    CarDisplay(DisplayType& _display, CommsProtocolType& _commsProtocol) :
            display(_display),
            commsProtocol(_commsProtocol)
    {
    }


    //
    //
    //
    void ProcessMessage(uint8_t (&rawMessage)[16])
    {
        typedef union
        {
            uint8_t     uint8Value;
            uint16_t    uint16Value;
            uint8_t     byteValues[1];

        } PayloadType;

        typedef struct
        {
            uint8_t     length;
            uint8_t     checksum;
            uint8_t     type;
            PayloadType payload;
        } MessageType;

        MessageType&    message     = *((MessageType*)&rawMessage);
        static uint8_t  xPos        = 0;
        static uint8_t  yPos        = 0;
        static uint8_t  spriteId    = 0;

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
                display.drawFrame();
                break;

            case 5:
                xPos        = message.payload.byteValues[0];
                yPos        = message.payload.byteValues[1];
                spriteId    = message.payload.byteValues[2];
                display.drawSprite(spriteId, xPos, yPos);

                break;

            default:
                break;
        }
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


