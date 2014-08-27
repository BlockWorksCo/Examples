




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
auto NewPacket 		= [](int protocolType) -> void { switch(protocolType) { case 1: udpLayer.NewPacket();break; case 2:arpLayer.NewPacket();break; case 3:icmpLayer.NewPacket();break; } };
auto LayerState 	= [](int protocolType) -> PacketProcessingState { switch(protocolType) { case 1: return udpLayer.State();break; case 2:return arpLayer.State();break; case 3:return icmpLayer.State();break; default:return Rejected;break;} };
auto PushIntoLayer 	= [](int protocolType, uint8_t byte) -> void { switch(protocolType) { case 1: udpLayer.PushInto(byte);break; case 2:arpLayer.PushInto(byte);break; case 3:icmpLayer.PushInto(byte);break; } };
auto PullFromLayer 	= [](int protocolType, bool& dataAvailable) -> uint8_t { switch(protocolType) { case 1:return udpLayer.PullFrom(dataAvailable);break; case 2:return arpLayer.PullFrom(dataAvailable);break; case 3:return icmpLayer.PullFrom(dataAvailable);break; default:dataAvailable=false;return 0;break;} };

ApplicationLayerType    tcpApplicationLayer;
TCPTransportLayerType   tcpLayer(tcpApplicationLayer);
InternetLayerType       internetLayer( NewPacket, LayerState, PushIntoLayer, PullFromLayer);
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

