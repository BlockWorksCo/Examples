





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

	TCP() :
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
        if(position < 20)
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
                    flags   = byte;
                    printf("(TCP) Flags: %d\n", flags);
                    break;

                case 13:
                    windowSize  = byte << 8;
                    break;

                case 14:
                    windowSize  |= byte;
                    printf("(TCP) windowSize: %d\n", windowSize);
                    break;

                case 15:
                    checksum  = byte << 8;
                    break;

                case 16:
                    checksum  |= byte;
                    printf("(TCP) checksum: %d\n", checksum);
                    break;

                case 17:
                    urgentPointer  = byte << 8;
                    break;

                case 18:
                    urgentPointer  |= byte;
                    printf("(TCP) urgentPointer: %d\n", urgentPointer);
                    break;

                default:
                    break;
            }
            
        }
        else
        {
            //
            // Data portion of the IP packet.
            //
            //tcpLayer.PushInto(byte);

            #if 0
            ProtocolDispatch( protocol, 
                                6,tcpLayer,
                                17,udpLayer);
            #endif
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

};



#endif













