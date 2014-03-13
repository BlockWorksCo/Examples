



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
typedef LinuxOutput<GreenLED>                               GreenLEDType;
typedef LinuxOutput<OrangeLED>                              OrangeLEDType;
typedef LinuxOutput<RedLED>                                 RedLEDType;
typedef LinuxOutput<BlueLED>                                BlueLEDType;
typedef LEDController<  GreenLEDType, 
                        OrangeLEDType, 
                        RedLEDType, 
                        BlueLEDType>                        LEDControllerType;
class ButtonPressedDelegate;
typedef LinuxInput<ButtonA, ButtonPressedDelegate>          ButtonType;

typedef RocketBloxDemoOne<LEDControllerType>                AppType;
DELEGATE_TYPE(AppType, ButtonPressed,                       ButtonPressedDelegate);


#endif


