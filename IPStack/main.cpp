



#include "AppConfiguration.h"



//
// Instantiations.
//
StackType::UDPTransportLayerType   udpLayer;
StackType::ICMPTransportLayerType  icmpLayer;
StackType::ApplicationLayerType    tcpApplicationLayer;
StackType::ARPTransportLayerType   arpLayer;
StackType::TCPTransportLayerType   tcpLayer(tcpApplicationLayer);
StackType::InternetLayerType       internetLayer;
StackType::LinkLayerType           linkLayer("./Traces/HTTPGET.pcap");


//
// IPv4 linking.
//
void StackType::IPv4NewPacket(IP::ProtocolType protocolType)
{
    switch(protocolType) { case IP::TCP:tcpLayer.NewPacket();break; case IP::UDP:udpLayer.NewPacket();break; case IP::ICMP:icmpLayer.NewPacket();break; }
}

PacketProcessingState StackType::IPv4LayerState(IP::ProtocolType protocolType)
{
    switch(protocolType) { case IP::TCP: return tcpLayer.State();break; case IP::UDP: return udpLayer.State();break; case IP::ICMP:return icmpLayer.State();break; default:return Rejected;break;}
}

void StackType::IPv4PushIntoLayer(IP::ProtocolType protocolType, uint8_t byte)
{
    switch(protocolType) { case IP::TCP:tcpLayer.PushInto(byte);break; case IP::UDP:udpLayer.PushInto(byte);break; case IP::ICMP:icmpLayer.PushInto(byte);break; }     
}

uint8_t StackType::IPv4PullFromLayer(IP::ProtocolType protocolType, bool& dataAvailable,  uint16_t position)
{ 
    switch(protocolType) {case IP::TCP:return tcpLayer.PullFrom(dataAvailable,position);break;  case IP::UDP:return udpLayer.PullFrom(dataAvailable,position);break; case IP::ICMP:return icmpLayer.PullFrom(dataAvailable,position);break; default:dataAvailable=false;return 0;break;}
} 


//
// LinkLayer linking.
//
void StackType::LinkIdle()
{
    internetLayer.Idle();    
    arpLayer.Idle();
}

void StackType::LinkNewPacket()
{
    internetLayer.NewPacket();    
    arpLayer.NewPacket();
}

PacketProcessingState StackType::LinkLayerState()
{
    return internetLayer.State();
}

void StackType::LinkPushIntoLayer(uint8_t byte)
{
    internetLayer.PushInto(byte);
    arpLayer.PushInto(byte);
}

uint8_t StackType::LinkPullFromLayer(bool& dataAvailable,  uint16_t position)
{ 
    return internetLayer.PullFrom(dataAvailable, position);
} 


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

