

#ifndef __LEDCONTROLLER_H__
#define __LEDCONTROLLER_H__


#include "Platform.h"
#include "Debug.h"


//
// Simple state machine demo for controlling LEDs in a particular
// pattern.
//
template <  class LEDAType,
            class LEDBType,
            class LEDCType,
            class LEDDType
         >
class LEDController 
{
public:
    void Run()
    {
        static volatile int y=0;
        y++;
        
        DEBUG_TEXT("LEDController started.\n");

        DEBUG_TEXT("On...\n");
        DELAY_MS(500);

        DEBUG_TEXT("Off...\n");
        DELAY_MS(200);

        ledA.Toggle();
        ledB.Toggle();
        ledC.Toggle();
        ledD.Toggle();
    }

private:

    LEDAType    ledA;
    LEDBType    ledB;
    LEDCType    ledC;
    LEDDType    ledD;
};



#endif


