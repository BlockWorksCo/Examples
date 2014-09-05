



#include "AppConfiguration.h"




StackType::UDPTransportLayerType   udpLayer;
StackType::ICMPTransportLayerType  icmpLayer;
StackType::ApplicationLayerType    tcpApplicationLayer;
StackType::ARPTransportLayerType   arpLayer;
StackType::TCPTransportLayerType   tcpLayer(tcpApplicationLayer);

StackType::InternetLayerType       internetLayer;

StackType::LinkLayerType           linkLayer(internetLayer, "./Traces/HTTPGET.pcap");


void IPv4NewPacket(int protocolType)
{
    switch(protocolType) { case 1: udpLayer.NewPacket();break; case 3:icmpLayer.NewPacket();break; }
}

PacketProcessingState IPv4LayerState(int protocolType)
{
    switch(protocolType) { case 1: return udpLayer.State();break; case 3:return icmpLayer.State();break; default:return Rejected;break;}
}

void IPv4PushIntoLayer(int protocolType, uint8_t byte)
{
    switch(protocolType) { case 1: udpLayer.PushInto(byte);break; case 3:icmpLayer.PushInto(byte);break; }     
}

uint8_t IPv4PullFromLayer(int protocolType, bool& dataAvailable,  uint16_t position)
{ 
    switch(protocolType) { case 1:return udpLayer.PullFrom(dataAvailable,position);break; case 3:return icmpLayer.PullFrom(dataAvailable,position);break; default:dataAvailable=false;return 0;break;}
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

