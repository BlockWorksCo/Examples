


#ifndef __APPCONFIGURATION_H__
#define __APPCONFIGURATION_H__





//
// TODO: put somewhere proper.
//
typedef enum 
{
    Unknown,
    Claimed,
    Rejected,

} PacketProcessingState;


#include <stdint.h>

#include "TUN.h"
#include "IPv4.h"
#include "TCP.h"
#include "UDP.h"
#include "HelloWorldPageGenerator.h"



struct StackType
{
    typedef HelloWorldPageGenerator<StackType>  ApplicationLayerType;
    typedef TCP<StackType>                      TCPTransportLayerType;
    typedef UDP<StackType>                      UDPTransportLayerType;
    typedef IPv4<StackType>                     InternetLayerType;
    typedef TUN<StackType>                      LinkLayerType;
};




#endif



