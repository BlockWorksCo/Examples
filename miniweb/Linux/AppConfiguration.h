


#ifndef __APPCONFIGURATION_H__
#define __APPCONFIGURATION_H__


#include <stdint.h>

#include "miniweb.h"
#include "tun_dev.h"
#include "pages.h"
#include "Map.h"



struct IPStackType
{
    typedef MiniWebServer<IPStackType>                                                          WebServerType;
    typedef TUNPacketInterface<IPStackType>                                                     PacketInterfaceType;
    typedef PacketGenerator<IPStackType>                                                        PacketGeneratorType;
    typedef uint16_t                                                                            PortType;
    typedef OffsetHash<uint8_t, PortType, 80>                                                   PortToPageIndexHashType;
    typedef Map<IPStackType::PacketGeneratorType, PortType, PortToPageIndexHashType>            PortToPageMapType;

    typedef OffsetHash<uint8_t, PortType, 80>                                                   PacketTypeToProtocolIndexHashType;
    typedef Map<IPStackType::PacketGeneratorType, PortType, PacketTypeToProtocolIndexHashType>  PacketTypeToProtocolMapType;

};




#endif



