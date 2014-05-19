



#ifndef __APPCONFIGURATION_H__
#define __APPCONFIGURATION_H__



#include "LEDController.h"
#include "DemoOne.h"
#include "Delegate.h"
#include "GPIO.h"





//
// Compile-time configuration.
// Define the configuration thru the use of typedefs and where needed c++11 template aliases or
// a substitute mechanism (inheritance).
//
typedef MSP430Output<P1OUT_, P1DIR_, 0x01>                  GreenLEDType;
typedef MSP430Output<P1OUT_, P1DIR_, 0x02>                  OrangeLEDType;
typedef MSP430Output<P1OUT_, P1DIR_, 0x04>                  RedLEDType;
typedef MSP430Output<P1OUT_, P1DIR_, 0x08>                  BlueLEDType;
typedef LEDController<  GreenLEDType, 
                        OrangeLEDType, 
                        RedLEDType, 
                        BlueLEDType>                        LEDControllerType;
class ButtonPressedDelegate;
typedef MSP430Input<P2IN_, P2DIR_, 0x01, ButtonPressedDelegate> ButtonType;

typedef RocketBloxDemoOne<LEDControllerType>                AppType;
DELEGATE_TYPE(AppType, ButtonPressed,                       ButtonPressedDelegate);





#endif


