




#ifndef __IPV4_H__
#define __IPV4_H__


//
// Generic public IP definitions.
//
struct IP
{       
    //
    // IANA specified IP protocol numbers.
    // See: http://www.iana.org/assignments/protocol-numbers/protocol-numbers.xhtml
    //
    typedef enum
    {
        ICMP    = 1,
        TCP     = 6,
        UDP     = 17,

    } ProtocolType;    
};




//
// IPv4 class:
// Provides all the functionality of the IPv4/internet layer.
//
template <  typename LoggerType,
            typename StackType, 
            uint32_t IPAddress,
            void (*newPacket)(IP::ProtocolType) ,
            PacketProcessingState (*layerState)(IP::ProtocolType),
            void (*pushIntoLayer)(IP::ProtocolType, uint8_t), 
            uint8_t (*pullFromLayer)(IP::ProtocolType, bool&,  uint16_t),
            uint32_t destinationIP(IP::ProtocolType),
            uint16_t packetLength(IP::ProtocolType)
            >
class IPv4
{
    //
    // Break out the StackType helper types.
    //
    STACK_TYPES_BREAKOUT;


public:

    //
    // Constructor for the IPv4 layer. 
    // Simply resets the stack right now.
    //
    IPv4() :
        packetState(Unknown)
    {
    }

    //
    // Idle:
    // Provided for general timing/timeout behaviour.
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

        LoggerType::printf("\n\n-->NewPacket:\n");
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
                if( byte == 0x45)
                {
                    LoggerType::printf("(IPv4) Claimed, Header Length = %x.\n", byte&0xf);
                    packetState   = Claimed;
                }
                else
                {
                    LoggerType::printf("(IPv4) Rejected.\n");
                    packetState   = Rejected;
                }
                break;


            case 1:
                // DSCP
                break;

            case 2:
                length  = byte<<8;
                break;

            case 3:
                length  |= byte;
                LoggerType::printf("(IPv4) Packet Length = %d\n", length);
                break;

            case 4:
                // Identification field
                break;

            case 5:
                // Identification field
                break;

            case 6:
                fragmentOffset  = (byte & 0x1f) << 8;
                fragmentFlags   = (byte & 0xe0) >> 5;
                LoggerType::printf("(IPv4) Fragment flags: %d\n", fragmentFlags);
                break;

            case 7:
                fragmentOffset    |= byte;                
                LoggerType::printf("(IPv4) Fragment offset: %d\n", fragmentOffset);

                if(fragmentFlags != 2)
                {
                    //
                    // We don't support fragmentation yet...
                    //
                    packetState     = Rejected;
                }
                break;

            case 8:
                LoggerType::printf("(IPv4) TTL: %d\n",byte);
                break;

            case 9:
                protocol    = static_cast<IP::ProtocolType>(byte);
                LoggerType::printf("(IPv4) Protocol: %d\n",byte);
                break;

            case 10:
                headerChecksum  = byte << 8;
                break;

            case 11:
                headerChecksum  |= byte;
                LoggerType::printf("(IPv4) headerChecksum: %04x\n", headerChecksum);
                break;

            case 12:
                sourceIP    = byte << 24;
                break;

            case 13:
                sourceIP    |= byte<<16;
                break;

            case 14:
                sourceIP    |= byte<<8;
                break;

            case 15:
                sourceIP    |= byte;
                LoggerType::printf("(IPv4) SourceIP: %08x\n", sourceIP);
                break;

            case 16:
                if( byte != ((IPAddress>>24)&0xff) )
                {
                    Reject();
                }
                break;

            case 17:
                if( byte != ((IPAddress>>16)&0xff) )
                {
                    Reject();
                }
                break;

            case 18:
                if( byte != ((IPAddress>>8)&0xff) )
                {
                    Reject();
                }
                break;

            case 19:
                if( byte != (IPAddress&0xff) )
                {
                    Reject();
                }
                else
                {
                    LoggerType::printf("DestIP: %08x\n", IPAddress);
                }
                break;

            case 20:
                LoggerType::printf("(IPv4) TransportDataStart.\n");
                newPacket(protocol);
                // Fallthrough intended.
            default:
                LoggerType::printf("(IPv4) data.\n");

                //
                // Data portion of the IP packet.
                //
                PacketProcessingState state   = layerState(protocol);
                if(state != Rejected)
                {
                    pushIntoLayer(protocol, byte);
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
    //
    //
    void UpdateAccumulatedChecksum(uint16_t value)
    {
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
        uint8_t         byteToTransmit          = 0x00;
        const uint16_t  sizeofIPHeader          = 20;                                       // standard/minimum size.

        const uint8_t   versionAndIHL           = (0x04 << 4)| (sizeofIPHeader/4);          // IPv4 + 20 byte header.
        const uint8_t   DSCP                    = 0x00;                                     // ununsed.
        uint16_t        length                  = packetLength(protocol)+sizeofIPHeader;    // unknown. Assume a constant size greater than the actual size and pad with zeroes. Checksum is not affected by zeroes.
        const uint16_t  fragmentationID         = 0x1234;                                   // unused.
        const uint8_t   fragmentationFlags      = 0x40;                                     // Dont Fragment.
        const uint8_t   fragmentationOffset     = 0x00;                                     // unused.
        const uint8_t   TTL                     = 64;                                       // Seconds/hops
        IP::ProtocolType protocol               = IP::TCP;                                  // 6=TCP, 11=UDP, etc...
        uint32_t        destIP                  = destinationIP(protocol);                  // target... dynamic.
        uint32_t        sourceIP                = 0xca0802fd;
        uint32_t        headerChecksum          = ((versionAndIHL<<8) | DSCP) + 
                                                  length + 
                                                  fragmentationID +
                                                  ((fragmentationFlags<<8) | fragmentationOffset) +
                                                  ((TTL<<8) | protocol) +
                                                  (sourceIP >> 16) +
                                                  (sourceIP & 0xffff) + 
                                                  (destIP >> 16) +
                                                  (destIP & 0xffff);


        while ( (headerChecksum >> 16) != 0 ) 
        {
            headerChecksum  = (headerChecksum&0xffff) + (headerChecksum>>16);
        }
        headerChecksum    = ~headerChecksum;

        if( position < sizeofIPHeader )
        {

            dataAvailable   = true;

            switch(position)
            {
                case 0:
                    byteToTransmit      = versionAndIHL;
                    break;

                case 1:
                    byteToTransmit      = DSCP;
                    break;

                case 2:
                    byteToTransmit      = length >> 8;
                    break;

                case 3:
                    byteToTransmit      = length & 0xff;
                    break;

                case 4:
                    byteToTransmit      = fragmentationID >> 8;
                    break;

                case 5:
                    byteToTransmit      = fragmentationID & 0xff;
                    break;

                case 6:
                    byteToTransmit      = fragmentationFlags; 
                    break;

                case 7:
                    byteToTransmit      = fragmentationOffset;
                    break;

                case 8:
                    byteToTransmit      = TTL;
                    break;

                case 9:
                    byteToTransmit      = protocol;
                    break;

                case 10:

                    uint16_t    t;
                    //static uint16_t  ttt[] = {0x4500, 0x0073, 0x0000, 0x4000, 0x4011, 0xb861, 0xc0a8, 0x0001, 0xc0a8, 0x00c7 };
                    //static uint16_t    ttt[] = {0x4500, 0x0073, 0x0000, 0x4000, 0x4011, 0x0000, 0xc0a8, 0x0001, 0xc0a8, 0x00c7 };
                    //static uint16_t  ttt[] = {0x4500, 0x0032, 0x1234, 0x4000, 0x4006, 0x432d, 0xc0a8, 0x02fd, 0x0011, 0x2233 };
                    static uint16_t    ttt[] = {0x4500, 0x0032, 0x1234, 0x4000, 0x4006, 0x0000, 0xc0a8, 0x02fd, 0x0011, 0x2233 };
                    accumulatedChecksum     = 0;
                    for(int i=0; i<10; i++)
                    {
                        t   = ttt[i];
                        LoggerType::printf("(%04x) = %04x\n", t, accumulatedChecksum);
                        UpdateAccumulatedChecksum( t );
                    }
                    accumulatedChecksum    = ~accumulatedChecksum;
                    LoggerType::printf("Check = %04x\n", accumulatedChecksum );


                    LoggerType::printf("ip=%08x\n", sourceIP );

                    accumulatedChecksum     = 0;                    

                    t   = ( ((uint16_t)versionAndIHL<<8) | (uint16_t)DSCP);
                    LoggerType::printf("..%04x = %04x\n", t, accumulatedChecksum);
                    UpdateAccumulatedChecksum( t );

                    t   = length;
                    LoggerType::printf("..%04x = %04x\n", t, accumulatedChecksum);
                    UpdateAccumulatedChecksum( t );

                    t   = fragmentationID;
                    LoggerType::printf("..%04x = %04x\n", t, accumulatedChecksum);
                    UpdateAccumulatedChecksum( t );

                    t   = (( ((uint16_t)fragmentationFlags)<<8) | (uint16_t)fragmentationOffset);
                    LoggerType::printf("..%04x = %04x\n", t, accumulatedChecksum);
                    UpdateAccumulatedChecksum( t );

                    t   = (( ((uint16_t)TTL)<<8) | (uint16_t)protocol);
                    LoggerType::printf("..%04x = %04x\n", t, accumulatedChecksum);
                    UpdateAccumulatedChecksum( t );

                    t   = 0;
                    LoggerType::printf("..%04x = %04x\n", t, accumulatedChecksum);
                    UpdateAccumulatedChecksum( t );

                    t   = (uint16_t)(sourceIP >> 16);
                    LoggerType::printf("..%04x = %04x\n", t, accumulatedChecksum);
                    UpdateAccumulatedChecksum( t );

                    t   = (uint16_t)(sourceIP & 0xffff);
                    LoggerType::printf("..%04x = %04x\n", t, accumulatedChecksum);
                    UpdateAccumulatedChecksum( t );

                    t   = (uint16_t)(destIP >> 16);
                    LoggerType::printf("..%04x = %04x\n", t, accumulatedChecksum);
                    UpdateAccumulatedChecksum( t );

                    t   = (uint16_t)(destIP & 0xffff);
                    LoggerType::printf("..%04x = %04x\n", t, accumulatedChecksum);
                    UpdateAccumulatedChecksum( t );

                    accumulatedChecksum    = ~accumulatedChecksum;

                    LoggerType::printf("Check = %04x\n", accumulatedChecksum );

                    byteToTransmit      = accumulatedChecksum >> 8;
                    break;

                case 11:
                    byteToTransmit      = accumulatedChecksum & 0xff;
                    break;

                case 12:
                    byteToTransmit      = IPAddress >> 24;
                    break;

                case 13:
                    byteToTransmit      = (IPAddress >> 16) & 0xff;
                    break;

                case 14:
                    byteToTransmit      = (IPAddress >> 8) & 0xff;
                    break;

                case 15:
                    byteToTransmit      = IPAddress & 0xff;
                    break;

                case 16:
                    byteToTransmit      = destIP >> 24;
                    break;

                case 17:
                    byteToTransmit      = (destIP >> 16) & 0xff;
                    break;

                case 18:
                    byteToTransmit      = (destIP >> 8) & 0xff;
                    break;

                case 19:
                    byteToTransmit      = destIP & 0xff;
                    break;

                default:
                    break;
            }
        }
        else
        {
            //
            // Not the header, lets pull the data from the layer above...
            //
            byteToTransmit  = pullFromLayer(protocol, dataAvailable, position-sizeofIPHeader );            
        }

        return byteToTransmit;
    }


private:

    //
    // IPv4 Checksum alg.
    //
    uint16_t checksum1(const uint8_t* buf, uint16_t size)
    {
        uint32_t    sum = 0;
        int         i;

        /* Accumulate checksum */
        for (i=0; i<size-1; i+=2)
        {
            uint16_t word16 = * ((uint16_t *)&buf[i]);
            sum += word16;
        }

        /* Handle odd-sized case */
        if ( (size&1) != 0 )
        {
            uint16_t word16 = (unsigned char) buf[i];
            sum += word16;
        }

        /* Fold to get the ones-complement result */
        while ( (sum>>16) != 0 ) 
        {
            sum = (sum & 0xFFFF)+(sum >> 16);
        }

        /* Invert to get the negative in ones-complement arithmetic */
        return ~sum;
    }

    //
    // Set the current packet as rejected.
    //
    void Reject()
    {
        packetState     = Rejected;
    }



    //
    //
    //
    typedef struct
    {
        IP::ProtocolType  protocol;
        uint32_t        ip;    

    } PacketState;

    //
    //
    //
    uint16_t                position;
    PacketProcessingState   packetState;

    uint32_t                accumulatedChecksum;

    uint16_t                length;
    uint16_t                fragmentOffset;
    uint8_t                 fragmentFlags;
    uint16_t                headerChecksum;
    uint32_t                sourceIP;
    IP::ProtocolType          protocol;

};





#endif



