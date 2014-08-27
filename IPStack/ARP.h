





#ifndef __ARP_H__
#define __ARP_H__





//
//
//
template < typename StackType >
class ARP
{

    //
    // Break out the StackType helper types.
    //
    STACK_TYPES_BREAKOUT;



public:



	ARP() :
        position(0),
        packetState(Unknown)
	{
		
	}


    //
    //
    //
    void Idle()
    {
        printf("(ARP) Idle.\n");
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
        printf("(ARP)\n");
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
    uint8_t PullFrom(bool& dataAvailable)
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













