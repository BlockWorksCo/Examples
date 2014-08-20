





#ifndef __TCP_H__
#define __TCP_H__



//
//
//
template < typename StackType >
class TCP
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

    typedef enum
    {
        TCP_FIN           = 0x01,
        TCP_SYN           = 0x02,
        TCP_RST           = 0x04,
        TCP_PSH           = 0x08,
        TCP_ACK           = 0x10,
        TCP_URG           = 0x20,        

        TCP_ECE           = 0x40,        
        TCP_CWR           = 0x80,        

    } TCPFlags;


    typedef enum
    {
        CLOSED            = 0,
        LISTEN            = 1,
        SYN_SENT          = 2,
        SYN_RCVD          = 3,
        ESTABLISHED       = 4,
        FIN_WAIT_1        = 5,
        FIN_WAIT_2        = 6,
        CLOSE_WAIT        = 7,
        CLOSING           = 8,
        LAST_ACK          = 9,
        TIME_WAIT         = 10,    

    } TCPState;


public:

	TCP(ApplicationLayerType& _applicationLayer) :
        position(0),
        packetState(Unknown),
        applicationLayer(_applicationLayer)
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
        dataOffset      = 20;   // 5 words minimum.
    }

    //
    // Push some received data into this packet processor...
    //
    void PushInto(uint8_t byte)
    {
        //
        // Header portion of the packet.
        //
        switch(position)
        {
            case 0:
                sourcePort  = byte << 8;
                break;

            case 1:
                sourcePort  |= byte;
                printf("(TCP) sourcePort: %d\n", sourcePort);
                break;

            case 2:
                destinationPort  = byte << 8;
                break;

            case 3:
                destinationPort  |= byte;
                printf("(TCP) destinationPort: %d\n", destinationPort);
                break;

            case 4:
                sequenceNumber  = byte << 24;
                break;

            case 5:
                sequenceNumber  |= byte << 16;
                break;

            case 6:
                sequenceNumber  |= byte << 8;
                break;

            case 7:
                sequenceNumber  |= byte;
                printf("(TCP) sequenceNumber: %d\n", sequenceNumber);
                break;

            case 8:
                ackNumber  = byte << 24;
                break;

            case 9:
                ackNumber  |= byte << 16;
                break;

            case 10:
                ackNumber  |= byte << 8;
                break;

            case 11:
                ackNumber  |= byte;
                printf("(TCP) ackNumber: %d\n", ackNumber);
                break;

            case 12:
                dataOffset  = byte >> 4;
                dataOffset  *= 4;
                printf("(TCP) dataOffset: %d\n", dataOffset);
                break;

            case 13:
                flags   = byte;
                printf("(TCP) Flags: %d\n", flags);
                break;

            case 14:
                windowSize  = byte << 8;
                break;

            case 15:
                windowSize  |= byte;
                printf("(TCP) windowSize: %d\n", windowSize);
                break;

            case 16:
                checksum  = byte << 8;
                break;

            case 17:
                checksum  |= byte;
                printf("(TCP) checksum: %d\n", checksum);
                break;

            case 18:
                urgentPointer  = byte << 8;
                break;

            case 19:
                urgentPointer  |= byte;
                printf("(TCP) urgentPointer: %d\n", urgentPointer);
                break;

            case 20:
                printf("(TCP) AppDataStart\n");
                applicationLayer.NewPacket();

            default:

                if(position >= dataOffset)
                {
                    printf("(TCP) AppData.\n");
                    
                    //
                    // Data portion of the IP packet.
                    //
                    if(applicationLayer.State() != Rejected)
                    {
                        applicationLayer.PushInto(byte);
                    }                    
                }
                else
                {
                    //
                    // Variable length option data.
                    //
                    printf("(TCP) OptionData.\n");
                }

                break;
        }
        

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
    void StateMachine()
    {
        TCPState    currentState    = applicationLayer.GetTCPState();

        switch(currentState)
        {
            case LISTEN:
                if( (flags&TCP_SYN) != 0)
                {
                    //
                    // Send a SynAck packet.
                    //
                }

                if( (flags&TCP_ACK) != 0)
                {
                    //
                    // Connection established.
                    //
                    currentState    = ESTABLISHED;
                }

                break;

            default:
                break;
        }

        applicationLayer.SetTCPState(currentState);
    }

    //
    // Pull some packet data out of the processor for transmission.
    //
    uint8_t PullFrom(bool& dataAvailable)
    {
        //
        // TODO: Pull from all upper layers, one whole packet at a time.
        //
        return applicationLayer.PullFrom( dataAvailable );
    }

private:

    //
    //
    //
    uint16_t                position;
    PacketProcessingState   packetState;

    uint16_t                sourcePort;
    uint16_t                destinationPort;
    uint32_t                sequenceNumber;
    uint32_t                ackNumber;
    uint8_t                 flags;
    uint16_t                windowSize;
    uint16_t                checksum;
    uint16_t                urgentPointer;
    uint8_t                 dataOffset;

    ApplicationLayerType&   applicationLayer;

};



#endif













