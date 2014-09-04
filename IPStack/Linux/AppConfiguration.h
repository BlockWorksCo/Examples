


#ifndef __APPCONFIGURATION_H__
#define __APPCONFIGURATION_H__






//
// Break out the CombinationTypes for this class.
//
#define STACK_TYPES_BREAKOUT    \
    typedef typename StackType::ApplicationLayerType    ApplicationLayerType;   \
    typedef typename StackType::TCPTransportLayerType   TCPTransportLayerType;  \
    typedef typename StackType::UDPTransportLayerType   UDPTransportLayerType;  \
    typedef typename StackType::ICMPTransportLayerType  ICMPTransportLayerType; \
    typedef typename StackType::ARPTransportLayerType   ARPTransportLayerType;  \
    typedef typename StackType::InternetLayerType       InternetLayerType;      \
    typedef typename StackType::LinkLayerType           LinkLayerType;



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



