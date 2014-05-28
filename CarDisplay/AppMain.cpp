


#include "Platform.h"
#include "SerialPort.h"
#include "Display.h"








extern ring_buffer      rx_buffer; 
extern ring_buffer      tx_buffer; 
SerialPort              serial0;


Display<  2,
            3,
            4,
            5,
            4 >     display;







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
            spriteId = byte;
            display.drawSprite(spriteId, xPos, yPos);

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










