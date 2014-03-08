
#ifndef __DEMOONE_H__
#define __DEMOONE_H__


#include "Platform.h"
#include "Debug.h"


//
// Main application class.
//
template <  typename LEDControllerType
         >
class RocketBloxDemoOne
{
public:

    RocketBloxDemoOne(LEDControllerType& _ledController) :
            ledController(_ledController)
    {
        static volatile int z = 0;
        z+=3;
    }

    void Run()
    {
        ledController.Run();
    }

    static void ButtonPressed()
    {
        static volatile int x=0;
        x+=2;
        DEBUG_TEXT("Button Pressed.");
    }

    void SomethingHappened()
    {
        DEBUG_TEXT("Something Happened.");
    }

private:

    LEDControllerType&      ledController;

};





#endif


