





#ifndef __ICMP_H__
#define __ICMP_H__





//
//
//
template <  typename LoggerType,
            typename StackType >
class ICMP
{
    //
    // Break out the StackType helper types.
    //
    STACK_TYPES_BREAKOUT;



public:



	ICMP() :
        position(0),
        packetState(Unknown)
	{
		
	}


    //
    //
    //
    void Idle()
    {
        LoggerType::printf("(ICMP) Idle.\n");
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
        LoggerType::printf("(ICMP)\n");
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


    uint32_t DestinationIP()
    {
        return 0x00000000;
    }

    uint16_t PacketLength()
    {
        return 10;
    }


private:

    //
    //
    //
    uint16_t                position;
    PacketProcessingState   packetState;



};



#endif













