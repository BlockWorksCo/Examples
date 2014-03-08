



#ifndef __APPCONFIGURATION_H__
#define __APPCONFIGURATION_H__


#include "Platform.h"
#include "LEDController.h"
#include "GPIO.h"
#include "DemoOne.h"
#include "Delegate.h"




//
// Compile-time configuration.
// Define the configuration thru the use of typedefs and where needed c++11 template aliases or
// a substitute mechanism (inheritance).
//
typedef STM32Output<GPIOD_BASE, GPIO_Pin_12>                GreenLEDType;
typedef STM32Output<GPIOD_BASE, GPIO_Pin_13>                OrangeLEDType;
typedef STM32Output<GPIOD_BASE, GPIO_Pin_14>                RedLEDType;
typedef STM32Output<GPIOD_BASE, GPIO_Pin_15>                BlueLEDType;
typedef LEDController<  GreenLEDType, 
                        OrangeLEDType, 
                        RedLEDType, 
                        BlueLEDType>                        LEDControllerType;
class ButtonPressedDelegate;
typedef STM32Input<0x00000000,0x10, ButtonPressedDelegate>  ButtonType;

typedef RocketBloxDemoOne<LEDControllerType>                AppType;
DELEGATE_TYPE(AppType, ButtonPressed,                       ButtonPressedDelegate);


#endif


