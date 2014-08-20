





#ifndef __HELLOWORLDPAGEGENERATOR_H__
#define __HELLOWORLDPAGEGENERATOR_H__





//
//
//
template < typename StackType >
class HelloWorldPageGenerator
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

	HelloWorldPageGenerator() :
        position(0),
        packetState(Unknown)
	{
		
	}


    //
    //
    //
    void Idle()
    {
        printf("(App) Idle.\n");
    }

    //
    // Reset the packet detector.
    //
    void NewPacket()
    {
        position        = 0;
        packetState     = Claimed;
    }

    //
    // Push some received data into this packet processor...
    //
    void PushInto(uint8_t byte)
    {
        printf("(App) %02x\n", byte);

        //
        // Ready for next byte.
        //
        position++;
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













