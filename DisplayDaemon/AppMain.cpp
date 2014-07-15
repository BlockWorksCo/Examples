



#include "Platform.h"
#include "AppConfiguration.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h> /* mmap() is defined in this header */
#include <fcntl.h>



char* blaa()
{
    int            fdout;
    char*          dst;
    struct stat    statbuf;

    /* open/create the output file */
    fdout   = open("/tmp/ledfb", O_RDWR | O_CREAT | O_TRUNC, 0777);
    if(fdout == -1)
       printf("can't create for writing\n");

    /* write a dummy byte at the last location */
    for(int i=0; i<40; i++)
    {
        write (fdout, "a", 1);
    }

    /* mmap the file */
    dst     = (char*)mmap(0, 40, PROT_READ | PROT_WRITE, MAP_SHARED, fdout, 0);
    if ( dst == (caddr_t)-1 )
        printf("mmap error for output\n");

    /* this copies the input file to the output file */
    memset(dst, 'c', 40);
    strcpy(dst, "Hello World.");

    printf("mmap ptr = %08x\n", dst);

    return dst;
}




//
//
//

RxQueueType        rxQueue;
TxQueueType        txQueue;

UARTType           uart0("/dev/ttyS0", rxQueue,txQueue);





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
    bool    elementDroppedFlag  = false;
    
    char* fb    = blaa();

    while(true)
    {
        txQueue.Put('a', elementDroppedFlag);
        uart0.ProcessQueue();
        printf("[%s]\n", fb);
        sleep(1);
    }
}

