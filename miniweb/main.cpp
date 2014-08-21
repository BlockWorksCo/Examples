




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


ApplicationLayerType    tcpApplicationLayer;
TCPTransportLayerType   tcpLayer(tcpApplicationLayer);
UDPTransportLayerType   udpLayer;
InternetLayerType       internetLayer(tcpLayer, udpLayer);
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

