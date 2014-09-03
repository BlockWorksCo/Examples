




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



#include "AppConfiguration.h"



STACK_TYPES_BREAKOUT;


UDPTransportLayerType   udpLayer;
ICMPTransportLayerType  icmpLayer;
ApplicationLayerType    tcpApplicationLayer;
ARPTransportLayerType   arpLayer;
TCPTransportLayerType   tcpLayer(tcpApplicationLayer);


auto IPv4NewPacket      = [](int protocolType) -> void { switch(protocolType) { case 1: udpLayer.NewPacket();break; case 3:icmpLayer.NewPacket();break; } };
auto IPv4LayerState     = [](int protocolType) -> PacketProcessingState { switch(protocolType) { case 1: return udpLayer.State();break; case 3:return icmpLayer.State();break; default:return Rejected;break;} };
auto IPv4PushIntoLayer  = [](int protocolType, uint8_t byte) -> void { switch(protocolType) { case 1: udpLayer.PushInto(byte);break; case 3:icmpLayer.PushInto(byte);break; } };
auto IPv4PullFromLayer  = [](int protocolType, bool& dataAvailable, uint16_t position) -> uint8_t { switch(protocolType) { case 1:return udpLayer.PullFrom(dataAvailable,position);break; case 3:return icmpLayer.PullFrom(dataAvailable,position);break; default:dataAvailable=false;return 0;break;} };




InternetLayerType       internetLayer( IPv4NewPacket, IPv4LayerState, IPv4PushIntoLayer, IPv4PullFromLayer);
LinkLayerType           linkLayer(internetLayer, "../Traces/HTTPGET.pcap");





//
//
//
int main(int argc, char **argv)
{
    printf("\nBLOCK::WORKS IPStack Demo\n");
    while(true)
    {
        linkLayer.Iterate();    
    }

    return 0;
}

