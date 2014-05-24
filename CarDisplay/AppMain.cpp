







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

int maxInUse = 5;    //change this variable to set how many MAX7219's you'll use

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





#ifndef HWSPI
void putuint8_t (uint8_t  data) 
{
  uint8_t  i = 8;
  uint8_t  mask;

    CYCLE_DELAY;
  while(i > 0) {
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
#else

void putuint8_t (uint8_t  data) 
{
  uint8_t  i = 8;
  uint8_t  mask;
  
  SPI.transfer(data);
}

#endif




void maxSingle( uint8_t  reg, uint8_t  col) {    
//maxSingle is the "easy"  function to use for a     //single max7219

  digitalWrite(load, LOW);       // begin     
  putuint8_t (reg);                  // specify register
  putuint8_t (col);//((data & 0x01) * 256) + data >> 1); // put data   
  digitalWrite(load, LOW);       // and load da shit
  digitalWrite(load,HIGH); 
}





void maxAll (uint8_t  reg, uint8_t  col) 
{    // initialize  all  MAX7219's in the system
  int c = 0;
  digitalWrite(load, LOW);  // begin     
  for ( c =1; c<= maxInUse; c++) 
  {
    putuint8_t (reg);  // specify register
    putuint8_t (col);//((data & 0x01) * 256) + data >> 1); // put data
  }
  digitalWrite(load, LOW);
  digitalWrite(load,HIGH);
}



void maxOne(uint8_t  maxNr, uint8_t  reg, uint8_t  col) 
{    
//maxOne is for adressing different MAX7219's, 
//whilele having a couple of them cascaded

  int c = 0;
  digitalWrite(load, LOW);  // begin     

  for ( c = maxInUse; c > maxNr; c--) 
  {
    putuint8_t (0);    // means no operation
    putuint8_t (0);    // means no operation
  }

  putuint8_t (reg);  // specify register
  putuint8_t (col);//((data & 0x01) * 256) + data >> 1); // put data 

  for ( c =maxNr-1; c >= 1; c--) 
  {
    putuint8_t (0);    // means no operation
    putuint8_t (0);    // means no operation
  }

  digitalWrite(load, LOW); // and load da shit
  digitalWrite(load,HIGH); 
}




void drawFrame(uint8_t* frameBuffer)
{
    maxAll(max7219_reg_shutdown, 0x01);    // not in shutdown mode

    for(int j=0; j<8; j++)
    {
      maxAll(j+1, frameBuffer[j]);
    }
}

void setup () 
{

#ifdef HWSPI
  // Reverse the SPI transfer to send the MSB first  
  SPI.setBitOrder(MSBFIRST);
  
  // Start SPI
  SPI.begin();
#endif

  pinMode(dataIn, OUTPUT);
  pinMode(clock,  OUTPUT);
  pinMode(load,   OUTPUT);
  pinMode(power,   OUTPUT);
  
  pinMode(10,   OUTPUT);

  //Serial.begin(19200);
  //Serial.println("\r\n> ");
  digitalWrite(13, HIGH);
  
  digitalWrite(power, LOW);
  delay(500);  
  digitalWrite(power, HIGH);  

//initiation of the max 7219

  maxAll(max7219_reg_scanLimit, 0x07);      
  maxAll(max7219_reg_decodeMode, 0x00);  // using an led matrix (not digits)
  maxAll(max7219_reg_shutdown, 0x01);    // not in shutdown mode
  maxAll(max7219_reg_displayTest, 0x00); // no display test
   for (e=1; e<=8; e++) {    // empty registers, turn all LEDs off 
    maxAll(e,0);
  }
  maxAll(max7219_reg_intensity, 0x04 & 0x0f);    // the first 0x0f is the value you can set
                                                  // range: 0x00 to 0x0f

}  

void loop () 
{
    static int i=0;
    static uint8_t   frameBuffer[8];
    static uint32_t         frameCount = 0;
    
    
    drawFrame(&frameBuffer[0]);


    frameCount++;
    if(frameCount>=8)
    {
      frameCount  = 0;
    }
    memset(&frameBuffer[0], 0x00, sizeof(frameBuffer));
    frameBuffer[frameCount] = 0xff;
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








