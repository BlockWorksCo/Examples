




#ifndef __IPV4_H__
#define __IPV4_H__




//
//
//
template < typename StackType >
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
    IPv4(   void (*_newPacket)(int), 
            PacketProcessingState (*_layerState)(int),
            void (*_pushIntoLayer)(int, uint8_t), 
            uint8_t (*_pullFromLayer)(int, bool&,  uint16_t) ) :
        packetState(Unknown),
        newPacket(_newPacket),
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
                newPacket(protocol);
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
        const uint16_t  sizeofTCPHeader         = 20;
        uint16_t        length                  = 0x0000;
        const uint8_t   TTL                     = 0x8;
        uint8_t         protocol                = 0x06;
        uint16_t        headerChecksum          = 0x0000;
        uint32_t        sourceIP                = 0x00112233;
        uint32_t        destIP                  = 0x44556677;

        if( position < sizeofTCPHeader )
        {
            switch(position)
            {
                case 0:
                    byteToTransmit      = 0x40 | (sizeofTCPHeader/4); // IPv4 & header length.
                    break;

                case 1:
                    byteToTransmit      = 0x00; // DSCP
                    break;

                case 2:
                    byteToTransmit      = length >> 8; // length ms
                    break;

                case 3:
                    byteToTransmit      = length & 0xff; // length ls
                    break;

                case 4:
                    byteToTransmit      = 0xff; // ID
                    break;

                case 5:
                    byteToTransmit      = 0xff; // ID
                    break;

                case 6:
                    byteToTransmit      = 0x02; // Fragmentation flags 0x1 = dont fragment.
                    break;

                case 7:
                    byteToTransmit      = 0x00; // Fragmentation offset.
                    break;

                case 8:
                    byteToTransmit      = TTL;
                    break;

                case 9:
                    byteToTransmit      = protocol;
                    break;

                case 10:
                    byteToTransmit      =  headerChecksum >> 8;
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
                    // Dest IP.
                    byteToTransmit      = destIP >> 24;
                    break;

                case 17:
                    // Dest IP.
                    byteToTransmit      = (destIP >> 16) & 0xff;
                    break;

                case 18:
                    // Dest IP.
                    byteToTransmit      = (destIP >> 8) & 0xff;
                    break;

                case 19:
                    // Dest IP.
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
    uint16_t                position;
    PacketProcessingState   packetState;

    uint16_t                length;
    uint16_t                fragmentOffset;
    uint8_t                 fragmentFlags;
    uint16_t                headerChecksum;
    uint32_t                sourceIP;
    uint8_t                 protocol;

    void                    (*newPacket)(int);
    PacketProcessingState   (*layerState)(int);
    void                    (*pushIntoLayer)(int, uint8_t);
    uint8_t                 (*pullFromLayer)(int, bool&, uint16_t );

};





#endif



