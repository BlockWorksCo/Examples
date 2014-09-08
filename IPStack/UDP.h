





#ifndef __UDP_H__
#define __UDP_H__





//
//
//
template <  typename LoggerType,
            typename StackType >
class UDP
{
    //
    // Break out the StackType helper types.
    //
    STACK_TYPES_BREAKOUT;


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
        LoggerType::printf("(UDP) Idle.\n");
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
        LoggerType::printf("(UDP)\n");
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
    uint8_t PullFrom(bool& dataAvailable, uint16_t position)
    {
        //
        // TODO: Pull from all upper layers, one whole packet at a time.
        //
        dataAvailable   = false;
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













