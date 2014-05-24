







/* code for max 7219 from maxim, 
reduced and optimised for useing more then one 7219 in a row,
______________________________________

 Code History:
 --------------

The orginal code was written for the Wiring board by:
 * Nicholas Zambetti and Dave Mellis /Interaction Design Institute Ivrea /Dec 2004
 * http://www.potemkin.org/uploads/Wiring/MAX7219.txt

First modification by:
 * Marcus Hannerstig/  K3, malmï¿½ hï¿½gskola /2006
 * http://www.xlab.se | http://arduino.berlios.de

This version is by:
 * tomek ness /FH-Potsdam / Feb 2007
 * http://design.fh-potsdam.de/ 

 * @acknowledgements: eric f. 

-----------------------------------

General notes: 


-if you are only using one max7219, then use the function maxSingle to control
 the little guy ---maxSingle(register (1-8), collum (0-255))

-if you are using more then one max7219, and they all should work the same, 
then use the function maxAll ---maxAll(register (1-8), collum (0-255))

-if you are using more than one max7219 and just want to change something
at one little guy, then use the function maxOne
---maxOne(Max you wane controll (1== the first one), register (1-8), 
collum (0-255))

 During initiation, be sure to send every part to every max7219 and then
 upload it.
For example, if you have five max7219's, you have to send the scanLimit 5 times
before you load it-- other wise not every max7219 will get the data. the
function maxInUse  keeps track of this, just tell it how many max7219 you are
using.
*/

#ifdef HWSPI
#include <SPI.h>
#endif
//#include <WProgram.h>
#include <stdint.h>
#include <Arduino.h>

int dataIn = 2;
int load = 3;
int clock = 4;
int power=5;

#define maxInUse  (5)    //change this variable to set how many MAX7219's you'll use

int e = 0;           // just a varialble

#define CYCLE_DELAY  delay(0)

                     // define max7219 registers
uint8_t  max7219_reg_noop        = 0x00;
uint8_t  max7219_reg_digit0      = 0x01;
uint8_t  max7219_reg_digit1      = 0x02;
uint8_t  max7219_reg_digit2      = 0x03;
uint8_t  max7219_reg_digit3      = 0x04;
uint8_t  max7219_reg_digit4      = 0x05;
uint8_t  max7219_reg_digit5      = 0x06;
uint8_t  max7219_reg_digit6      = 0x07;
uint8_t  max7219_reg_digit7      = 0x08;
uint8_t  max7219_reg_decodeMode  = 0x09;
uint8_t  max7219_reg_intensity   = 0x0a;
uint8_t  max7219_reg_scanLimit   = 0x0b;
uint8_t  max7219_reg_shutdown    = 0x0c;
uint8_t  max7219_reg_displayTest = 0x0f;



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



//
// maxSingle is the "easy"  function to use for a     //single max7219
//
void maxSingle( uint8_t  reg, uint8_t  col) 
{    
    digitalWrite(load, LOW);      // begin     
    shiftOutByte (reg);           // specify register
    shiftOutByte (col);         //((data & 0x01) * 256) + data >> 1); // put data   
    digitalWrite(load, LOW);      // and load da shit
    digitalWrite(load,HIGH); 
}




//
// initialize  all  MAX7219's in the system
//
void maxAll (uint8_t  reg, uint8_t  col) 
{    
    int     c = 0;

    digitalWrite(load, LOW);

    for ( c =1; c<= maxInUse; c++) 
    {
        shiftOutByte (reg);  // specify register
        shiftOutByte (col);//((data & 0x01) * 256) + data >> 1); // put data
    }

    digitalWrite(load, LOW);
    digitalWrite(load,HIGH);
}


//
// maxOne is for adressing different MAX7219's, 
// while having a couple of them cascaded
//
void maxOne(uint8_t  maxNr, uint8_t  reg, uint8_t  col) 
{    
    int c = 0;
    digitalWrite(load, LOW);  // begin     

    for ( c = maxInUse; c > maxNr; c--) 
    {
        shiftOutByte (0);    // means no operation
        shiftOutByte (0);    // means no operation
    }

    shiftOutByte (reg);  // specify register
    shiftOutByte (col);//((data & 0x01) * 256) + data >> 1); // put data 

    for ( c =maxNr-1; c >= 1; c--) 
    {
        shiftOutByte (0);    // means no operation
        shiftOutByte (0);    // means no operation
    }

    digitalWrite(load, LOW); // and load da shit
    digitalWrite(load,HIGH); 
}



//
//
//
void drawFrame(uint8_t* frameBuffer)
{
    maxAll(max7219_reg_shutdown, 0x01);    // not in shutdown mode

    for(int j=0; j<8; j++)
    {
        maxAll(j+1, frameBuffer[j]);
    }
}



//
//
//
void setup () 
{
    //
    // Set directions of pins.
    //
    pinMode(dataIn, OUTPUT);
    pinMode(clock,  OUTPUT);
    pinMode(load,   OUTPUT);
    pinMode(power,   OUTPUT);
    pinMode(10,   OUTPUT); // SS is output so SPI knows we're master, not slave.

    //Serial.begin(19200);
    //Serial.println("\r\n> ");
    digitalWrite(13, HIGH);

    //
    // Power cycle the displays.
    //
    digitalWrite(power, LOW);
    delay(500);  
    digitalWrite(power, HIGH);  

    //
    // initiation of the max 7219
    //
    maxAll(max7219_reg_scanLimit, 0x07);      
    maxAll(max7219_reg_decodeMode, 0x00);  // using an led matrix (not digits)
    maxAll(max7219_reg_shutdown, 0x01);    // not in shutdown mode
    maxAll(max7219_reg_displayTest, 0x00); // no display test
    maxAll(max7219_reg_intensity, 0x04 & 0x0f);    // the first 0x0f is the value you can set

    //
    // empty registers, turn all LEDs off 
    //
    for (e=1; e<=8; e++) 
    {    
        maxAll(e,0);
    }
}  


//
//
//
void loop() 
{
    static  int         i           = 0;
    static  uint8_t     frameBuffer[8];
    static  uint32_t    frameCount  = 0;
    
    //
    // Draw the frame to the display.
    //
    drawFrame(&frameBuffer[0]);

    //
    // Modify the frame for next time.
    //
    frameCount++;
    if(frameCount>=8)
    {
        frameCount  = 0;
    }
    memset(&frameBuffer[0], 0x00, sizeof(frameBuffer));
    frameBuffer[frameCount] = 0xff;

    //
    // Wait for a frame period.
    //
    delay(15);
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
    init();
    setup();
    while(1)
    {
        loop();
    }
}






#if 0


#include "LEDController.h"
#include "DemoOne.h"
#include "Delegate.h"
#include "AppConfiguration.h"


extern AppType          app;
ButtonPressedDelegate   buttonDelegate(app);
ButtonType              button(buttonDelegate);
LEDControllerType       ledController;
AppType                 app(ledController);





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
    button.Poll();
    
    while(true)
    {
        app.Run();
    }
}


#endif








