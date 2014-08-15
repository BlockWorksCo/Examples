


#ifndef __APPCONFIGURATION_H__
#define __APPCONFIGURATION_H__


#include "miniweb.h"
#include "tun_dev.h"
#include "pages.h"


struct IPStackType
{
    typedef MiniWebServer<IPStackType>          WebServerType;
    typedef TUNPacketInterface<IPStackType>     PacketInterfaceType;
    typedef PacketGenerator<IPStackType>        PacketGeneratorType;
};




#endif



