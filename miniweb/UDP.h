





#ifndef __UDP_H__
#define __UDP_H__





//
//
//
template < typename StackType >
class UDP
{
    //
    // Break out the CombinationTypes for this class.
    //
    typedef typename StackType::ApplicationLayerType    ApplicationLayerType;
    typedef typename StackType::TCPTransportLayerType   TCPTransportLayerType;
    typedef typename StackType::UDPTransportLayerType   UDPTransportLayerType;
    typedef typename StackType::InternetLayerType       InternetLayerType;
    typedef typename StackType::LinkLayerType           LinkLayerType;



public:



	UDP() :
        position(0),
        packetState(Unknown)
	{
		
	}


    //
    //
    //
    void Idle()
    {
        printf("(IPv4) Idle.\n");
    }

    //
    // Reset the packet detector.
    //
    void NewPacket()
    {
        position        = 0;
        packetState     = Unknown;
    }

    //
    // Push some received data into this packet processor...
    //
    void PushInto(uint8_t byte)
    {
        printf("(UDP)\n");
    }


    //
    //
    //
    PacketProcessingState State()
    {
        return packetState;
    }

    //
    // Pull some packet data out of the processor for transmission.
    //
    uint8_t PullFrom()
    {
        return 0;
    }


private:

    //
    //
    //
    uint16_t                position;
    PacketProcessingState   packetState;



};



#endif













