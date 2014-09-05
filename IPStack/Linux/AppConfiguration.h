


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

void IPv4NewPacket(int protocolType);
PacketProcessingState IPv4LayerState(int protocolType);
void IPv4PushIntoLayer(int protocolType, uint8_t byte);
uint8_t IPv4PullFromLayer(int protocolType, bool& dataAvailable,  uint16_t position);

struct StackType
{
    typedef HelloWorldPageGenerator<StackType>  ApplicationLayerType;
    typedef TCP<StackType>                      TCPTransportLayerType;
    typedef UDP<StackType>                      UDPTransportLayerType;
    typedef IPv4<StackType, IPv4NewPacket, IPv4LayerState, IPv4PushIntoLayer, IPv4PullFromLayer >              InternetLayerType;
    typedef PCAP<StackType>                     LinkLayerType;
    typedef ARP<StackType>                      ARPTransportLayerType;
    typedef ICMP<StackType>                     ICMPTransportLayerType;
};



#endif



