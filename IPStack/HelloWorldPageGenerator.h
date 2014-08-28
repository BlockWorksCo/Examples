





#ifndef __HELLOWORLDPAGEGENERATOR_H__
#define __HELLOWORLDPAGEGENERATOR_H__





//
//
//
template < typename StackType >
class HelloWorldPageGenerator
{
    //
    // Break out the StackType helper types.
    //
    STACK_TYPES_BREAKOUT;

    typedef typename TCPTransportLayerType::TCPState 	TCPState;



public:

	HelloWorldPageGenerator() :
        position(0),
        packetState(Unknown),
        tcpState(TCPTransportLayerType::LISTEN),
        nextTCPState(TCPTransportLayerType::LISTEN)
	{
        //
        // Close the port to restart in LISTEN/PassiveOpen mode.
        //
        Close();
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


    //
    //
    //
    void GetTCPState(TCPState& currentState, TCPState& nextState)
    {
    	currentState       = tcpState;
        nextState           = nextTCPState;
    }

    void SetTCPState(TCPState newState)
    {
    	tcpState 	= newState;
    }



    //
    // Pull some packet data out of the processor for transmission.
    //
    uint8_t PullFrom(bool& dataAvailable, uint16_t position)
    {
        //
        // TODO: Pull from all upper layers, one whole packet at a time.
        //
        dataAvailable 	= false;
        return 0;
    }

private:


    //
    //
    //
    void Close()
    {
        //
        // Start off in the passive-open state.
        // We wait for a SYN to be received, then send out a SYN packet.
        //
        tcpState    = TCPTransportLayerType::LISTEN;    
    }

    //
    //
    //
    uint16_t                position;
    PacketProcessingState   packetState;
    TCPState 				tcpState;
    TCPState                nextTCPState;

};



#endif













