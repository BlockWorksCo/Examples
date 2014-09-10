





#ifndef __TCP_H__
#define __TCP_H__



//
//
//
template <  typename LoggerType,
            typename StackType >
class TCP
{
    //
    // Break out the StackType helper types.
    //
    STACK_TYPES_BREAKOUT;


public:

    typedef enum
    {
        TCP_NONE          = 0x00,

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
        applicationLayer(_applicationLayer),
        packetToSend(TCP_NONE)
	{
		
	}


    //
    //
    //
    void Idle()
    {
        LoggerType::printf("(IPv4) Idle.\n");
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
                LoggerType::printf("(TCP) sourcePort: %d\n", sourcePort);
                break;

            case 2:
                destinationPort  = byte << 8;
                break;

            case 3:
                destinationPort  |= byte;
                LoggerType::printf("(TCP) destinationPort: %d\n", destinationPort);
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
                LoggerType::printf("(TCP) sequenceNumber: %08x\n", sequenceNumber);
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
                LoggerType::printf("(TCP) ackNumber: %08x\n", ackNumber);
                break;

            case 12:
                dataOffset  = byte >> 4;
                dataOffset  *= 4;
                LoggerType::printf("(TCP) dataOffset: %d\n", dataOffset);
                break;

            case 13:
                flags   = static_cast<TCPFlags>(byte);
                LoggerType::printf("(TCP) Flags: %02x\n", flags);
                break;

            case 14:
                windowSize  = byte << 8;
                break;

            case 15:
                windowSize  |= byte;
                LoggerType::printf("(TCP) windowSize: %d\n", windowSize);
                break;

            case 16:
                checksum  = byte << 8;
                break;

            case 17:
                checksum  |= byte;
                LoggerType::printf("(TCP) checksum: %04x\n", checksum);
                break;

            case 18:
                urgentPointer  = byte << 8;
                break;

            case 19:
                urgentPointer  |= byte;
                LoggerType::printf("(TCP) urgentPointer: %02x\n", urgentPointer);
                break;

            case 20:
                LoggerType::printf("(TCP) AppDataStart\n");
                applicationLayer.NewPacket();

            default:

                if(position >= dataOffset)
                {
                    LoggerType::printf("(TCP) AppData.\n");
                    
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
                    LoggerType::printf("(TCP) OptionData.\n");
                }

                //
                // Walk thru the state machine.
                //
                StateMachine();

                //
                // Reset the outputput packet generation while we're still receiving.
                //
                outputPosition  = 0;


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
        TCPState    currentState;
        TCPState    nextTCPState;  

        applicationLayer.GetTCPState(currentState, nextTCPState);

        switch(currentState)
        {
            case LISTEN:
                if( (flags&TCP_SYN) != 0)
                {
                    //
                    // Send a SynAck packet.
                    //
                    packetToSend    = static_cast<TCPFlags>(TCP_ACK | TCP_SYN);
                    nextTCPState    = SYN_SENT;
                }

                break;

            case SYN_SENT:
                if( (flags&TCP_SYN) != 0)
                {
                    //
                    // Send a SynAck packet.
                    //
                    packetToSend    = static_cast<TCPFlags>(TCP_ACK);
                    nextTCPState    = SYN_SENT;
                }

                if( (flags&TCP_ACK) != 0)
                {
                    //
                    // Connection established.
                    //
                    packetToSend    = static_cast<TCPFlags>(TCP_NONE);
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
    uint8_t PullFrom(bool& dataAvailable, uint16_t position)
    {
        const uint16_t  dataOffset          = sizeofTCPHeader;
        uint8_t         byteToSend          = 0x00;
        uint16_t        sourcePort          = 80;
        uint16_t        destinationPort     = 8080;
        uint32_t        sequenceNumber      = 0x70c5dc78;
        uint32_t        ackNumber           = 0xe3899124;
        const uint16_t  urgentPointer       = 0x0000;
        const uint16_t  windowSize          = 822;
        const uint16_t  checksum            = 0x0000;
        uint8_t         flags               = TCP_SYN;

        dataAvailable   = true;

        switch(position)
        {
            case 0:
                byteToSend  = sourcePort >> 8;
                break;

            case 1:
                byteToSend  = sourcePort & 0xff;
                break;

            case 2:
                byteToSend  = destinationPort >> 8;
                break;

            case 3:
                byteToSend  = destinationPort & 0xff;
                break;

            case 4:
                byteToSend  = (sequenceNumber >> 24) & 0xff;
                break;

            case 5:
                byteToSend  = (sequenceNumber >> 16) & 0xff;
                break;

            case 6:
                byteToSend  = (sequenceNumber >> 8) & 0xff;
                break;

            case 7:
                byteToSend  = (sequenceNumber) & 0xff;
                break;

            case 8:
                byteToSend  = (ackNumber >> 24) & 0xff;
                break;

            case 9:
                byteToSend  = (ackNumber >> 16) & 0xff;
                break;

            case 10:
                byteToSend  = (ackNumber >> 8) & 0xff;
                break;

            case 11:
                byteToSend  = (ackNumber) & 0xff;
                break;

            case 12:
                byteToSend  = (dataOffset / 4) << 4;
                byteToSend  = 0x50;
                break;

            case 13:
                byteToSend  = flags;
                break;

            case 14:
                byteToSend  = windowSize >> 8;
                break;

            case 15:
                byteToSend  = windowSize & 0xff;
                break;

            case 16:
                byteToSend  = checksum >> 8;
                break;

            case 17:
                byteToSend  = checksum & 0xff;
                break;

            case 18:
                byteToSend  = urgentPointer >> 8;
                break;

            case 19:
                byteToSend  = urgentPointer & 0xff;
                break;

            default:
                byteToSend  = applicationLayer.PullFrom(dataAvailable, position-sizeofTCPHeader);
                break;
        }

        //
        // TODO: Pull from all upper layers, one whole packet at a time.
        //
        return byteToSend;
    }


    uint32_t DestinationIP()
    {
        return applicationLayer.DestinationIP();
    }

    uint16_t PacketLength()
    {
        return applicationLayer.PacketLength() + sizeofTCPHeader;
    }

private:


    const uint16_t  sizeofTCPHeader     = 20;

    //
    //
    //
    uint16_t                position;
    PacketProcessingState   packetState;

    uint16_t                sourcePort;
    uint16_t                destinationPort;
    uint32_t                sequenceNumber;
    uint32_t                ackNumber;
    TCPFlags                flags;
    uint16_t                windowSize;
    uint16_t                checksum;
    uint16_t                urgentPointer;
    uint8_t                 dataOffset;

    ApplicationLayerType&   applicationLayer;
    TCPFlags                packetToSend;
    uint16_t                outputPosition;

};



#endif













