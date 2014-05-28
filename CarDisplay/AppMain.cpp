


#include "Platform.h"
#include "SerialPort.h"
#include "Display.h"





static const unsigned char CH[] = 
{
3, 8, B0000000, B0000000, B0000000, B0000000, B0000000, // space
1, 8, B1011111, B0000000, B0000000, B0000000, B0000000, // !
3, 8, B0000011, B0000000, B0000011, B0000000, B0000000, // "
5, 8, B0010100, B0111110, B0010100, B0111110, B0010100, // #
4, 8, B0100100, B1101010, B0101011, B0010010, B0000000, // $
5, 8, B1100011, B0010011, B0001000, B1100100, B1100011, // %
5, 8, B0110110, B1001001, B1010110, B0100000, B1010000, // &
1, 8, B0000011, B0000000, B0000000, B0000000, B0000000, // '
3, 8, B0011100, B0100010, B1000001, B0000000, B0000000, // (
3, 8, B1000001, B0100010, B0011100, B0000000, B0000000, // )
5, 8, B0101000, B0011000, B0001110, B0011000, B0101000, // *
5, 8, B0001000, B0001000, B0111110, B0001000, B0001000, // +
2, 8, B10110000, B1110000, B0000000, B0000000, B0000000, // ,
4, 8, B0001000, B0001000, B0001000, B0001000, B0000000, // -
2, 8, B1100000, B1100000, B0000000, B0000000, B0000000, // .
4, 8, B1100000, B0011000, B0000110, B0000001, B0000000, // /
4, 8, B0111110, B1000001, B1000001, B0111110, B0000000, // 0
3, 8, B1000010, B1111111, B1000000, B0000000, B0000000, // 1
4, 8, B1100010, B1010001, B1001001, B1000110, B0000000, // 2
4, 8, B0100010, B1000001, B1001001, B0110110, B0000000, // 3
4, 8, B0011000, B0010100, B0010010, B1111111, B0000000, // 4
4, 8, B0100111, B1000101, B1000101, B0111001, B0000000, // 5
4, 8, B0111110, B1001001, B1001001, B0110000, B0000000, // 6
4, 8, B1100001, B0010001, B0001001, B0000111, B0000000, // 7
4, 8, B0110110, B1001001, B1001001, B0110110, B0000000, // 8
4, 8, B0000110, B1001001, B1001001, B0111110, B0000000, // 9
2, 8, B01010000, B0000000, B0000000, B0000000, B0000000, // :
2, 8, B10000000, B01010000, B0000000, B0000000, B0000000, // ;
3, 8, B0010000, B0101000, B1000100, B0000000, B0000000, // <
3, 8, B0010100, B0010100, B0010100, B0000000, B0000000, // =
3, 8, B1000100, B0101000, B0010000, B0000000, B0000000, // >
4, 8, B0000010, B1011001, B0001001, B0000110, B0000000, // ?
5, 8, B0111110, B1001001, B1010101, B1011101, B0001110, // @
4, 8, B1111110, B0010001, B0010001, B1111110, B0000000, // A
4, 8, B1111111, B1001001, B1001001, B0110110, B0000000, // B
4, 8, B0111110, B1000001, B1000001, B0100010, B0000000, // C
4, 8, B1111111, B1000001, B1000001, B0111110, B0000000, // D
4, 8, B1111111, B1001001, B1001001, B1000001, B0000000, // E
4, 8, B1111111, B0001001, B0001001, B0000001, B0000000, // F
4, 8, B0111110, B1000001, B1001001, B1111010, B0000000, // G
4, 8, B1111111, B0001000, B0001000, B1111111, B0000000, // H
3, 8, B1000001, B1111111, B1000001, B0000000, B0000000, // I
4, 8, B0110000, B1000000, B1000001, B0111111, B0000000, // J
4, 8, B1111111, B0001000, B0010100, B1100011, B0000000, // K
4, 8, B1111111, B1000000, B1000000, B1000000, B0000000, // L
5, 8, B1111111, B0000010, B0001100, B0000010, B1111111, // M
5, 8, B1111111, B0000100, B0001000, B0010000, B1111111, // N
4, 8, B0111110, B1000001, B1000001, B0111110, B0000000, // O
4, 8, B1111111, B0001001, B0001001, B0000110, B0000000, // P
4, 8, B0111110, B1000001, B1000001, B10111110, B0000000, // Q
4, 8, B1111111, B0001001, B0001001, B1110110, B0000000, // R
4, 8, B1000110, B1001001, B1001001, B0110010, B0000000, // S
5, 8, B0000001, B0000001, B1111111, B0000001, B0000001, // T
4, 8, B0111111, B1000000, B1000000, B0111111, B0000000, // U
5, 8, B0001111, B0110000, B1000000, B0110000, B0001111, // V
5, 8, B0111111, B1000000, B0111000, B1000000, B0111111, // W
5, 8, B1100011, B0010100, B0001000, B0010100, B1100011, // X
5, 8, B0000111, B0001000, B1110000, B0001000, B0000111, // Y
4, 8, B1100001, B1010001, B1001001, B1000111, B0000000, // Z
2, 8, B1111111, B1000001, B0000000, B0000000, B0000000, // [
4, 8, B0000001, B0000110, B0011000, B1100000, B0000000, // backslash
2, 8, B1000001, B1111111, B0000000, B0000000, B0000000, // ]
3, 8, B0000010, B0000001, B0000010, B0000000, B0000000, // hat
4, 8, B1000000, B1000000, B1000000, B1000000, B0000000, // _
2, 8, B0000001, B0000010, B0000000, B0000000, B0000000, // `
4, 8, B0100000, B1010100, B1010100, B1111000, B0000000, // a
4, 8, B1111111, B1000100, B1000100, B0111000, B0000000, // b
4, 8, B0111000, B1000100, B1000100, B0101000, B0000000, // c
4, 8, B0111000, B1000100, B1000100, B1111111, B0000000, // d
4, 8, B0111000, B1010100, B1010100, B0011000, B0000000, // e
3, 8, B0000100, B1111110, B0000101, B0000000, B0000000, // f
4, 8, B10011000, B10100100, B10100100, B01111000, B0000000, // g
4, 8, B1111111, B0000100, B0000100, B1111000, B0000000, // h
3, 8, B1000100, B1111101, B1000000, B0000000, B0000000, // i
4, 8, B1000000, B10000000, B10000100, B1111101, B0000000, // j
4, 8, B1111111, B0010000, B0101000, B1000100, B0000000, // k
3, 8, B1000001, B1111111, B1000000, B0000000, B0000000, // l
5, 8, B1111100, B0000100, B1111100, B0000100, B1111000, // m
4, 8, B1111100, B0000100, B0000100, B1111000, B0000000, // n
4, 8, B0111000, B1000100, B1000100, B0111000, B0000000, // o
4, 8, B11111100, B0100100, B0100100, B0011000, B0000000, // p
4, 8, B0011000, B0100100, B0100100, B11111100, B0000000, // q
4, 8, B1111100, B0001000, B0000100, B0000100, B0000000, // r
4, 8, B1001000, B1010100, B1010100, B0100100, B0000000, // s
3, 8, B0000100, B0111111, B1000100, B0000000, B0000000, // t
4, 8, B0111100, B1000000, B1000000, B1111100, B0000000, // u
5, 8, B0011100, B0100000, B1000000, B0100000, B0011100, // v
5, 8, B0111100, B1000000, B0111100, B1000000, B0111100, // w
5, 8, B1000100, B0101000, B0010000, B0101000, B1000100, // x
4, 8, B10011100, B10100000, B10100000, B1111100, B0000000, // y
3, 8, B1100100, B1010100, B1001100, B0000000, B0000000, // z
3, 8, B0001000, B0110110, B1000001, B0000000, B0000000, // {
1, 8, B1111111, B0000000, B0000000, B0000000, B0000000, // |
3, 8, B1000001, B0110110, B0001000, B0000000, B0000000, // }
4, 8, B0001000, B0000100, B0001000, B0000100, B0000000, // ~
};






extern ring_buffer      rx_buffer; 
extern ring_buffer      tx_buffer; 
SerialPort              serial0;


Display<  2,
            3,
            4,
            5,
            4 >     display;

//
//
//
void BitBlt(uint8_t* frameBuffer, int8_t x, int8_t y, uint8_t width, uint8_t height, uint8_t* data)
{
    if(x >= 32)
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

    if(x+width > 32)
    {
        width = 32-x;
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
            dest[i]     = (dest[i] & (0xff<<y)) | (data[i]<<y);
        }        
    }
    else if(y<0)
    {
        for(int i=0; i<width; i++)
        {
            uint8_t     absY = -y;
            dest[i]     = (dest[i] & (0xff>>absY)) | (data[i]>>absY);
        }                
    }
}


//
//
//
void drawSprite(uint8_t* frameBuffer, uint8_t spriteId, int8_t x, int8_t y)
{
    uint8_t     width   = CH[(spriteId*7)+0];
    uint8_t     height  = CH[(spriteId*7)+1];
    uint8_t*    data    = (uint8_t*)&CH[(spriteId*7)+2];
    BitBlt(&frameBuffer[0], x,y, width,height,  data);
}








uint8_t  position        = 0;


//
//
//
void ProcessDataByte(uint8_t byte)
{
    static uint8_t  xPos     = 0;
    static uint8_t  yPos     = 0;
    static uint8_t  spriteId = 0;

    switch(position)
    {
        case 0:
            xPos    = byte;
            break;

        case 1:
            yPos    = byte;
            break;

        case 2:
        {
            uint8_t*    frameBuffer = display.getFrameBuffer(); 

            spriteId = byte;
            drawSprite(&frameBuffer[0], spriteId, xPos, yPos);

            break;            
        }

        default:
            break;
    }

    position++;
}

//
// 0 1 2 3 27 4 5 6 7 = D0 D1 D2 D3 C4 D5 D6 D7
// 0 1 2 3 27 27 4 5 6 7 = D0 D1 D2 D3 D27 D4 D5 D6 D7
//
void ProcessRawByte(uint8_t byte)
{
    const uint8_t   escapeByte      = 27;
    static uint8_t  previousByte    = 0;

    if(previousByte == escapeByte)
    {
        if(byte == escapeByte)
        {
            ProcessDataByte(byte);
        }
        else
        {
            switch(byte)
            {
                case 0:
                    position    = 0;
                    break;

                case 1:
                    display.clear();
                    break;

                default:
                    break;
            }
        }
    }
    else if(byte != escapeByte) 
    {
        ProcessDataByte(byte);
    }

    previousByte    = byte;
}





//
//
//
void loop() 
{
    uint8_t*            frameBuffer = display.getFrameBuffer();
    static int         i           = 0;
    static int8_t      frameCount  = 0;
    static int         dir         = 1;
    static uint8_t     sprite      = 16;
    const int           minX        = -20;
    const int           minY        = -8;
    const int           maxX        = 20;
    const int           maxY        = 8;
    static int8_t       xPos        = 0;
    static int8_t       yPos        = 0;
    static int8_t       xDir        = 1;
    static int8_t       yDir        = 1;

    //
    // Draw the frame to the display.
    //
    display.drawFrame();
    serial0.println(".");

#if 0    
    //
    // Modify the frame for next time.
    //
    xPos    += xDir;
    yPos    += yDir;

    if(xPos >= maxX)
    {
        xDir    = -1;
    }
    if(xPos < minX)
    {
        xDir    = 1;
    }

    if(yPos >= maxY)
    {
        yDir    = -1;
    }
    if(yPos < minY)
    {
        yDir    = 1;
    }

    display.clear();

    drawSprite(&frameBuffer[0], sprite+0, xPos+0, yPos);
    drawSprite(&frameBuffer[0], sprite+1, xPos+8, yPos);
    drawSprite(&frameBuffer[0], sprite+2, xPos+16, yPos);
    drawSprite(&frameBuffer[0], sprite+3, xPos+24, yPos);

    //
    // Wait for a frame period.
    //
    delay(30);
#endif

    //
    //
    //
    while(serial0.available() != 0)
    {
        uint8_t     ch  = serial0.read();

        ProcessRawByte(ch);
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

    serial0.attach(&rx_buffer, &tx_buffer, &UBRR0H, &UBRR0L, &UCSR0A, &UCSR0B, &UCSR0C, &UDR0, RXEN0, TXEN0, RXCIE0, UDRIE0, U2X0);
    serial0.begin(19200);
    serial0.println("Hello World.");

    display.setup();

    while(true)
    {
        loop();
    }
}










