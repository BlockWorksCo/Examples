
#define CYCLE_DELAY     


#ifndef __DISPLAY_H__
#define __DISPLAY_H__


#include "Platform.h"



extern "C" const unsigned char SPRITES[7];

//
//
//
template < uint8_t dataIn,
            uint8_t load,
            uint8_t clock,
            uint8_t power,
            uint8_t NUMBER_OF_8x8_MATRICES >
class Display
{
public:

    //
    //
    //
    Display()
    {
    }

    //
    //
    //
    void setup()
    {
        //
        // Set directions of pins.
        //
        pinMode(dataIn, OUTPUT);
        pinMode(clock,  OUTPUT);
        pinMode(load,   OUTPUT);
        pinMode(power,  OUTPUT);

        //
        // initialisation of the max 7219
        //
        setAll(max7219_reg_displayTest, 0);
        setAll(max7219_reg_shutdown,    1);
        setAll(max7219_reg_scanLimit,   7);
        setAll(max7219_reg_intensity,   0xf);
        setAll(max7219_reg_decodeMode,  0);
        setAll(max7219_reg_digit0,      0xaa);
        setAll(max7219_reg_digit1,      0xaa);
        setAll(max7219_reg_digit2,      0xaa);
        setAll(max7219_reg_digit3,      0xaa);
        setAll(max7219_reg_digit4,      0xaa);
        setAll(max7219_reg_digit5,      0xaa);
        setAll(max7219_reg_digit6,      0xaa);
        setAll(max7219_reg_digit7,      0xaa);
    }


    //
    //
    //
    void setIntensity(uint8_t intensity)
    {
        setAll(max7219_reg_intensity,   intensity);        
    }

    //
    //
    //
    void drawFrame()
    {
        //
        //
        //
        for(int j=0; j<8; j++)
        {
            digitalWrite(load, LOW);    

            for(int k=(NUMBER_OF_8x8_MATRICES-1); k>=0; k--)
            {
                shiftOutByte( (MAX7219Register)(max7219_reg_digit0+j) );
                shiftOutByte( frameBuffer[(k*8)+j] );
            }

            digitalWrite(load, LOW);
            CYCLE_DELAY;
            digitalWrite(load,HIGH); 
        }
    }

    //
    //
    //
    void clear()
    {
        memset(&frameBuffer[0], 0x00, sizeof(frameBuffer));
    }

    //
    //
    //
    void BitBlt(int8_t x, int8_t y, uint8_t width, uint8_t height, uint8_t* data)
    {
        if(x >= (NUMBER_OF_8x8_MATRICES*8) )
        {
            return;
        }

        if(y<-(int8_t)height)
        {
            return;
        }

        if(x<-(int8_t)width)
        {
            return;
        }

        if(y >= 8)
        {
            return;
        }

        if(x+width > (NUMBER_OF_8x8_MATRICES*8) )
        {
            width = (NUMBER_OF_8x8_MATRICES*8)-x;
        }

        if(x<0)
        {
            width   += x;
            data    += -x;
            x       = 0;
        }

        uint8_t*    dest    = &frameBuffer[x];

        if(y>=0)
        {
            for(int i=0; i<width; i++)
            {
                //
                // dest[i] = 10101010 data[i] = 11001100 y=6 result = 101010xx | 00000011 = 10101011
                //
                dest[i]     = (dest[i] & (0xff>>(8-y) )) | (data[i]<<y);
            }        
        }
        else if(y<0)
        {
            for(int i=0; i<width; i++)
            {
                //
                // dest[i] = 10101010  data[i] = 11001100 y = -2, result = 10xxxxxx | xx001100 = 10001100
                //
                uint8_t     absY = -y;
                //dest[i]     = (dest[i] & (0xff<<(8-absY) )) | (data[i]>>absY );
                dest[i]     = (dest[i] & (0xff<<(8-absY) )) | (data[i]>>absY );
            }                
        }
    }


    //
    //
    //
    void drawSprite(uint8_t spriteId, int8_t x, int8_t y)
    {
        uint8_t     width   = SPRITES[(spriteId*7)+0];
        uint8_t     height  = SPRITES[(spriteId*7)+1];
        uint8_t*    data    = (uint8_t*)&SPRITES[(spriteId*7)+2];
        BitBlt(x,y, width,height,  data);
    }




private:


    //
    //
    //
    typedef enum
    {
        max7219_reg_noop        = 0x00,
        max7219_reg_digit0      = 0x01,
        max7219_reg_digit1      = 0x02,
        max7219_reg_digit2      = 0x03,
        max7219_reg_digit3      = 0x04,
        max7219_reg_digit4      = 0x05,
        max7219_reg_digit5      = 0x06,
        max7219_reg_digit6      = 0x07,
        max7219_reg_digit7      = 0x08,
        max7219_reg_decodeMode  = 0x09,
        max7219_reg_intensity   = 0x0a,
        max7219_reg_scanLimit   = 0x0b,
        max7219_reg_shutdown    = 0x0c,
        max7219_reg_displayTest = 0x0f,    

    } MAX7219Register;


    //
    //
    //
    void shiftOutByte (uint8_t  data) 
    {
        uint8_t     i       = 8;
        uint8_t     mask    = 0;

        CYCLE_DELAY;
        while(i > 0) 
        {
            mask = 0x01 << (i - 1);      // get bitmask
            digitalWrite( clock, LOW);   // tick
            CYCLE_DELAY;

            if (data & mask)
            {            // choose bit
                digitalWrite(dataIn, HIGH);// send 1
            }
            else
            {
                digitalWrite(dataIn, LOW); // send 0
            }

            digitalWrite(clock, HIGH);   // tock
            --i;                         // move to lesser bit

            CYCLE_DELAY;
        }

        CYCLE_DELAY;
    }





    void setAll(MAX7219Register reg, uint8_t value)
    {
        /*
        //
        */
        digitalWrite(load, LOW);    
        for(int k=0; k<NUMBER_OF_8x8_MATRICES; k++)
        {
            shiftOutByte( reg );
            shiftOutByte( value );
        }
        digitalWrite(load, LOW);
        CYCLE_DELAY;
        digitalWrite(load,HIGH); 
        CYCLE_DELAY;
    }



    static uint8_t      frameBuffer[8*NUMBER_OF_8x8_MATRICES];
};



#endif



