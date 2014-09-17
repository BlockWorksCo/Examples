//
// Copyright (C) BlockWorks Consulting Ltd - All Rights Reserved.
// Unauthorized copying of this file, via any medium is strictly prohibited.
// Proprietary and confidential.
// Written by Steve Tickle <Steve@BlockWorks.co>, September 2014.
//








#ifndef __TCP_H__
#define __TCP_H__



//
//
//
template <  typename LoggerType,
            typename StackType,
            uint32_t IPAddress >
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
    //
    //
    void UpdateAccumulatedChecksum(uint16_t value)
    {
        LoggerType::printf("- %04x - %04x -", value, accumulatedChecksum);
        accumulatedChecksum     += value;
        if( accumulatedChecksum > 0xffff )
        {
            accumulatedChecksum -= 0xffff;
        }
    }    

    //
    // Pull some packet data out of the processor for transmission.
    //
    uint8_t PullFrom(bool& dataAvailable, uint16_t position)
    {
        const uint8_t   dataOffset          = (sizeofTCPHeader / 4) << 4;
        uint8_t         byteToSend          = 0x00;
        uint16_t        sourcePort          = 0;
        uint16_t        destinationPort     = 0;
        uint32_t        sequenceNumber      = 0;
        uint32_t        ackNumber           = 0;
        const uint16_t  urgentPointer       = 0x0000;
        const uint16_t  windowSize          = 0;
        uint8_t         flags               = 0;

        dataAvailable   = true;

        connectionState.sourceIP    = 0;

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
                byteToSend  = dataOffset;
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

                accumulatedChecksum     = 0;                    

                //
                // Psuedo header portion of the checksum.
                //
                UpdateAccumulatedChecksum( IPAddress >> 16 );                   // source IP, us.
                UpdateAccumulatedChecksum( IPAddress & 0xffff );                //
                UpdateAccumulatedChecksum( connectionState.sourceIP >> 16 );    // Dest IP, remote.
                UpdateAccumulatedChecksum( connectionState.sourceIP & 0xffff ); //
                UpdateAccumulatedChecksum( IP::TCP );                           // always 6, TCP
                UpdateAccumulatedChecksum( PacketLength() );                    // *Note: the whole TCP segment, length, not just the applications.

                //
                // TCP header portion of the checksum
                //
                UpdateAccumulatedChecksum( sourcePort );
                UpdateAccumulatedChecksum( destinationPort );
                UpdateAccumulatedChecksum( sequenceNumber >> 16 );
                UpdateAccumulatedChecksum( sequenceNumber &0xffff );
                UpdateAccumulatedChecksum( ackNumber >> 16 );
                UpdateAccumulatedChecksum( ackNumber & 0xffff );
                UpdateAccumulatedChecksum( ((uint16_t)dataOffset<<16) | (uint16_t)flags );
                UpdateAccumulatedChecksum( windowSize );
                UpdateAccumulatedChecksum( urgentPointer );

                //
                // Data portion of the checksum
                //
                for(uint16_t i=0; i<applicationLayer.PacketLength(); i++)
                {
                    bool        moreDataAvailable   = false;
                    uint8_t     hiByte              = applicationLayer.PullFrom(moreDataAvailable, i);
                    uint8_t     loByte              = applicationLayer.PullFrom(moreDataAvailable, i);
                    UpdateAccumulatedChecksum( ((uint16_t)hiByte<<16) | (uint16_t)loByte  );
                }
                accumulatedChecksum    = ~accumulatedChecksum;
                LoggerType::printf("TCP Checksum: %04x %04x", accumulatedChecksum, ~accumulatedChecksum );

                //accumulatedChecksum = 0xd0d3;
                // 
                byteToSend  = accumulatedChecksum >> 8;
                break;

            case 17:
                byteToSend  = accumulatedChecksum & 0xff;
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

    IP::ConnectionState& ConnectionState()
    {
        return connectionState;
    }

private:

#if 0

    http://www.netfor2.com/tcpsum.htm

    u16 tcp_sum_calc(u16 len_tcp, u16 src_addr[],u16 dest_addr[], BOOL padding, u16 buff[])
    {
    u16 prot_tcp=6;
    u16 padd=0;
    u16 word16;
    u32 sum;    
        
        // Find out if the length of data is even or odd number. If odd,
        // add a padding byte = 0 at the end of packet
        if (padding&1==1){
            padd=1;
            buff[len_tcp]=0;
        }
        
        //initialize sum to zero
        sum=0;
        
        // make 16 bit words out of every two adjacent 8 bit words and 
        // calculate the sum of all 16 vit words
        for (i=0;i<len_tcp+padd;i=i+2){
            word16 =((buff[i]<<8)&0xFF00)+(buff[i+1]&0xFF);
            sum = sum + (unsigned long)word16;
        }   
        // add the TCP pseudo header which contains:
        // the IP source and destinationn addresses,
        for (i=0;i<4;i=i+2){
            word16 =((src_addr[i]<<8)&0xFF00)+(src_addr[i+1]&0xFF);
            sum=sum+word16; 
        }
        for (i=0;i<4;i=i+2){
            word16 =((dest_addr[i]<<8)&0xFF00)+(dest_addr[i+1]&0xFF);
            sum=sum+word16;     
        }
        // the protocol number and the length of the TCP packet
        sum = sum + prot_tcp + len_tcp;

        // keep only the last 16 bits of the 32 bit calculated sum and add the carries
            while (sum>>16)
            sum = (sum & 0xFFFF)+(sum >> 16);
            
        // Take the one's complement of sum
        sum = ~sum;

    return ((unsigned short) sum);
    }
#endif

    const uint16_t  sizeofTCPHeader     = 20;

    //
    //
    //
    uint16_t                position;
    PacketProcessingState   packetState;

    uint32_t                accumulatedChecksum;

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

    IP::ConnectionState     connectionState;
};



#endif













