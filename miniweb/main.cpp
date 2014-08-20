

#include "AppConfiguration.h"


//
// Break out the CombinationTypes for this class.
//
typedef typename StackType::ApplicationLayerType    ApplicationLayerType;
typedef typename StackType::TCPTransportLayerType   TCPTransportLayerType;
typedef typename StackType::UDPTransportLayerType   UDPTransportLayerType;
typedef typename StackType::InternetLayerType       InternetLayerType;
typedef typename StackType::LinkLayerType           LinkLayerType;


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

