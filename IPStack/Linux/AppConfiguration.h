


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



struct StackType
{
    //
    // TODO: Remove ProtocolType parameter from these...
    //
    static void IPv4NewPacket(IP::ProtocolType protocolType);
    static PacketProcessingState IPv4LayerState(IP::ProtocolType protocolType);
    static void IPv4PushIntoLayer(IP::ProtocolType protocolType, uint8_t byte);
    static uint8_t IPv4PullFromLayer(IP::ProtocolType protocolType, bool& dataAvailable,  uint16_t position);

    static void LinkNewPacket();
    static PacketProcessingState LinkLayerState();
    static void LinkPushIntoLayer(uint8_t byte);
    static uint8_t LinkPullFromLayer(bool& dataAvailable,  uint16_t position);

    typedef HelloWorldPageGenerator<StackType>  ApplicationLayerType;
    typedef TCP<StackType>                      TCPTransportLayerType;
    typedef UDP<StackType>                      UDPTransportLayerType;
    typedef IPv4<   StackType, 
                    0xc0a802fd, 
                    IPv4NewPacket, 
                    IPv4LayerState, 
                    IPv4PushIntoLayer, 
                    IPv4PullFromLayer >         InternetLayerType;
    typedef PCAP<   StackType,
                    LinkNewPacket, 
                    LinkLayerState, 
                    LinkPushIntoLayer, 
                    LinkPullFromLayer >         LinkLayerType;
    typedef ARP<StackType>                      ARPTransportLayerType;
    typedef ICMP<StackType>                     ICMPTransportLayerType;
};



#endif



