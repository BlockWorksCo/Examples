


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
#include "PCAP.h"
#include "IPv4.h"
#include "TCP.h"
#include "UDP.h"
#include "ARP.h"
#include "ICMP.h"
#include "HelloWorldPageGenerator.h"

void blaaa(int);

struct StackType
{
    typedef HelloWorldPageGenerator<StackType>  ApplicationLayerType;
    typedef TCP<StackType>                      TCPTransportLayerType;
    typedef UDP<StackType>                      UDPTransportLayerType;
    typedef IPv4<StackType, blaaa>              InternetLayerType;
    typedef PCAP<StackType>                     LinkLayerType;
    typedef ARP<StackType>                      ARPTransportLayerType;
    typedef ICMP<StackType>                     ICMPTransportLayerType;
};



#endif



