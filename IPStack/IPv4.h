




#ifndef __IPV4_H__
#define __IPV4_H__




//
//
//
template < typename StackType, void (*NewPacketType)(int) >
class IPv4
{
    //
    // Break out the StackType helper types.
    //
    STACK_TYPES_BREAKOUT;


public:

    //
    //
    //
    IPv4(   PacketProcessingState (*_layerState)(int),
            void (*_pushIntoLayer)(int, uint8_t), 
            uint8_t (*_pullFromLayer)(int, bool&,  uint16_t) ) :
        packetState(Unknown),
        layerState(_layerState),
        pushIntoLayer(_pushIntoLayer),
        pullFromLayer(_pullFromLayer)
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
        //
        // Header portion of the packet.
        //
        switch(position)
        {
            case 0:
                if( byte == 0x45)
                {
                    printf("(IPv4) Claimed, Header Length = %x.\n", byte&0xf);
                    packetState   = Claimed;
                }
                else
                {
                    printf("(IPv4) Rejected.\n");
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
                printf("(IPv4) Packet Length = %d\n", length);
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
                printf("(IPv4) Fragment flags: %d\n", fragmentFlags);
                break;

            case 7:
                fragmentOffset    |= byte;                
                printf("(IPv4) Fragment offset: %d\n", fragmentOffset);

                if(fragmentFlags != 2)
                {
                    //
                    // We don't support fragmentation yet...
                    //
                    packetState     = Rejected;
                }
                break;

            case 8:
                printf("(IPv4) TTL: %d\n",byte);
                break;

            case 9:
                protocol    = byte;
                printf("(IPv4) Protocol: %d\n",byte);
                break;

            case 10:
                headerChecksum  = byte << 8;
                break;

            case 11:
                headerChecksum  |= byte;
                printf("(IPv4) headerChecksum: %04x\n", headerChecksum);
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
                printf("(IPv4) SourceIP: %08x\n", sourceIP);
                break;

            case 16:
                // Dest IP.
                break;

            case 17:
                // Dest IP.
                break;

            case 18:
                // Dest IP.
                break;

            case 19:
                // Dest IP.
                break;

            case 20:
                printf("(IPv4) TransportDataStart.\n");
                NewPacketType(protocol);
                // Fallthrough intented.
            default:
                printf("(IPv4) data.\n");

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
    // Pull some packet data out of the processor for transmission.
    //
    uint8_t PullFrom(bool& dataAvailable, uint16_t position)
    {
        bool            dataAvailableFromAbove  = false;
        uint8_t         byteToTransmit          = 0x00;
        const uint16_t  sizeofTCPHeader         = 20;                                   // standard/minimum size.

        const uint8_t   versionAndIHL           = (0x04 << 4)| (sizeofTCPHeader/4);     // IPv4 + 20 byte header.
        const uint8_t   DSCP                    = 0x00;                                 // ununsed.
        uint16_t        length                  = 0x0000;                               // unknown. Assume a constant size greater than the actual size and pad with zeroes. Checksum is not affected by zeroes.
        const uint16_t  fragmentationID         = 0x0000;                               // unused.
        const uint8_t   fragmentationFlags      = 0x02;                                 // Dont Fragment.
        const uint8_t   fragmentationOffset     = 0x00;                                 // unused.
        const uint8_t   TTL                     = 0x8;                                  // Seconds/hops
        uint8_t         protocol                = 0x06;                                 // 6=TCP, 11=UDP, etc...
        const uint32_t  sourceIP                = 0x00112233;                           // us... static.
        uint32_t        destIP                  = 0x44556677;                           // target... dynamic.
        uint16_t        headerChecksum          = ((versionAndIHL<<8) | DSCP) + 
                                                  length + 
                                                  fragmentationID +
                                                  ((fragmentationFlags<<8) | fragmentationOffset) +
                                                  ((TTL<<8) | protocol) +
                                                  (sourceIP >> 16) +
                                                  (sourceIP & 0xffff) + 
                                                  (destIP >> 16) +
                                                  (destIP & 0xffff);

        if( position < sizeofTCPHeader )
        {
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
                    byteToTransmit      = fragmentationID >> 16;
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
                    byteToTransmit      = headerChecksum >> 8;
                    break;

                case 11:
                    byteToTransmit      = headerChecksum & 0xff;
                    break;

                case 12:
                    byteToTransmit      = sourceIP >> 24;
                    break;

                case 13:
                    byteToTransmit      = (sourceIP >> 16) & 0xff;
                    break;

                case 14:
                    byteToTransmit      = (sourceIP >> 8) & 0xff;
                    break;

                case 15:
                    byteToTransmit      = sourceIP & 0xff;
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
                    byteToTransmit      = destIP >> 24;
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
            byteToTransmit  = pullFromLayer(protocol, dataAvailableFromAbove, position-sizeofTCPHeader );            
        }

        return byteToTransmit;
    }

private:

    //
    //
    //
    typedef struct
    {
        uint8_t             protocol;
        uint32_t            ip;    
    } PacketState;

    //
    //
    //
    uint16_t                position;
    PacketProcessingState   packetState;

    uint16_t                length;
    uint16_t                fragmentOffset;
    uint8_t                 fragmentFlags;
    uint16_t                headerChecksum;
    uint32_t                sourceIP;
    uint8_t                 protocol;

    PacketProcessingState   (*layerState)(int);
    void                    (*pushIntoLayer)(int, uint8_t);
    uint8_t                 (*pullFromLayer)(int, bool&, uint16_t );

};





#endif



