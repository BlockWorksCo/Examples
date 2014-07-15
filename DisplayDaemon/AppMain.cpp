



#include "Platform.h"
#include "AppConfiguration.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h> /* mmap() is defined in this header */
#include <fcntl.h>



uint8_t* GetFB()
{
    int             fd;
    static uint8_t  fb[40];

    /* open/create the output file */
    fd   = open("/tmp/ledfb", O_RDONLY);
    read(fd, &fb[0], sizeof(fb));
    close(fd);

    return &fb[0];
}




//
//
//

RxQueueType        rxQueue;
TxQueueType        txQueue;

UARTType           uart0("/dev/ttyS0", rxQueue,txQueue);


void Send(TxQueueType& txq, uint8_t value)
{
    bool    elementDroppedFlag  = false;

    if(value == 27)
    {
        txQueue.Put(27, elementDroppedFlag);
        txQueue.Put(27, elementDroppedFlag);
        uart0.ProcessQueue();
    }
    else
    {
        txQueue.Put(value, elementDroppedFlag);
        uart0.ProcessQueue();
    }
}

void SendFBToDisplay(TxQueueType& txq, uint8_t* fb)
{
    bool    elementDroppedFlag  = false;
    
    txQueue.Put(27, elementDroppedFlag);
    txQueue.Put(0, elementDroppedFlag);
    uart0.ProcessQueue();

    Send(txq, 0);    // length
    Send(txq, 40+3); // checksum
    Send(txq, 6);    // type

    for(uint8_t i=0; i<40; i++)
    {
        Send(txq, fb[i]);    
    }

    txQueue.Put(27, elementDroppedFlag);
    txQueue.Put(255, elementDroppedFlag);
    uart0.ProcessQueue();
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

    while(true)
    {
        static uint32_t     fc  = 0;

        uint8_t* fb    = GetFB();
        SendFBToDisplay(txQueue, fb);

        printf("[%d]\n", fc++);
        sleep(1);
    }
}

