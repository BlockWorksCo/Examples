





#ifndef __UDP_H__
#define __UDP_H__





//
//
//
template < typename StackType >
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
        printf("(UDP) Idle.\n");
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













