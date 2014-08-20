


#include <stdint.h>
#include "Utilities.h"
#include "Dispatch.h"







void PutString(const char* string)
{
	static volatile char* output = (volatile char*)string;
	output = output;
}









class One
{
public:
    void operator()()
    {
        PutString("One.\n");
    }
};



class Two
{
public:
    void operator()()
    {
        PutString("Two.\n");
    }
};



class Three
{
public:
    void operator()()
    {
        PutString("Three.\n");
    }
};











typedef enum 
{
    eOne,
    eTwo,
    eThree,
} ObjectType;



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
    One     one;
    Two     two;
    Three   three;

    //IndexedDispatch(0,  one,two,three);


    ObjectType  order[]     = {eThree, eTwo, eOne};

    for(uint8_t i=0; i<NUMBER_OF_ELEMENTS(order); i++)
    {
        EnumeratedDispatch(order[i],    eOne,   one, 
                                        eTwo,   two, 
                                        eThree, three);

    }

}


