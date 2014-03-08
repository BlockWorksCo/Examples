



#ifndef __APPCONFIGURATION_H__
#define __APPCONFIGURATION_H__



#include "Platform.h"
#include "GPIO.h"
#include "Debug.h"
#include "LEDController.h"
#include "DemoOne.h"
#include "Delegate.h"





//
// Compile-time configuration.
// Define the configuration thru the use of typedefs and where needed c++11 template aliases or
// a substitute mechanism (inheritance).
//
typedef Win32Output<GreenLED>                               GreenLEDType;
typedef Win32Output<OrangeLED>                              OrangeLEDType;
typedef Win32Output<RedLED>                                 RedLEDType;
typedef Win32Output<BlueLED>                                BlueLEDType;
typedef LEDController<  GreenLEDType, 
                        OrangeLEDType, 
                        RedLEDType, 
                        BlueLEDType>                        LEDControllerType;

class ButtonPressedDelegate;
typedef Win32Input<ButtonA, ButtonPressedDelegate>          ButtonType;

typedef RocketBloxDemoOne<LEDControllerType>                AppType;
DELEGATE_TYPE(AppType, ButtonPressed,                       ButtonPressedDelegate);


#endif


