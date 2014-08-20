


#include <stdint.h>




#define NUMBER_OF_ELEMENTS(a)   sizeof(a)/sizeof(a[0])




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









template <typename First, typename EnumerationType> void EnumeratedDispatch(const EnumerationType enumeration, const EnumerationType firstEnumeration, First first) 
{
    if(enumeration == firstEnumeration)
    {
        first();
    }
}

template <typename First, typename EnumerationType, typename... Rest> void EnumeratedDispatch(const EnumerationType enumeration, const EnumerationType firstEnumeration, First& first, Rest... rest) 
{
    if(enumeration == firstEnumeration)
    {
        first();
    }
    EnumeratedDispatch(enumeration, rest...);
}










#define NUMBER_OF_ELEMENTS(a)   sizeof(a)/sizeof(a[0])



typedef enum 
{
    eOne,
    eTwo,
    eThree,
} ObjectType;

int test()
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
    return 0;
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
	test();
    while(true)
    {
    }
}




