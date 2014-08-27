




//
// Break out the CombinationTypes for this class.
//
#define STACK_TYPES_BREAKOUT 	\
	typedef typename StackType::ApplicationLayerType    ApplicationLayerType; 	\
	typedef typename StackType::TCPTransportLayerType   TCPTransportLayerType;  \
	typedef typename StackType::UDPTransportLayerType   UDPTransportLayerType;	\
	typedef typename StackType::ICMPTransportLayerType  ICMPTransportLayerType;	\
	typedef typename StackType::ARPTransportLayerType   ARPTransportLayerType;	\
	typedef typename StackType::InternetLayerType       InternetLayerType;		\
	typedef typename StackType::LinkLayerType           LinkLayerType;



#include "AppConfiguration.h"



STACK_TYPES_BREAKOUT;

UDPTransportLayerType   udpLayer;
ARPTransportLayerType	arpLayer;
ICMPTransportLayerType	icmpLayer;
auto NewPacket = [](int protocolType) -> void { switch(protocolType) { case 1: udpLayer.NewPacket();break; case 2:arpLayer.NewPacket();break; case 3:icmpLayer.NewPacket();break; } };

ApplicationLayerType    tcpApplicationLayer;
TCPTransportLayerType   tcpLayer(tcpApplicationLayer);
InternetLayerType       internetLayer(tcpLayer, NewPacket);
LinkLayerType           linkLayer(internetLayer);





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

