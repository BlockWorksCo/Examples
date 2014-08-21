




#ifndef __IPV4_H__
#define __IPV4_H__











//
// EnumeratedDispatch
//

template <typename First, typename EnumerationType> void ProtocolDispatch(const EnumerationType enumeration, const EnumerationType firstEnumeration, First first) 
{
    if(enumeration == firstEnumeration)
    {
        first();
    }
}

template <typename First, typename EnumerationType, typename... Rest> void ProtocolDispatch(const EnumerationType enumeration, const EnumerationType firstEnumeration, First& first, Rest... rest) 
{
    if(enumeration == firstEnumeration)
    {
        first();
    }
    ProtocolDispatch(enumeration, rest...);
}







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
    IPv4(TCPTransportLayerType& _tcpLayer, UDPTransportLayerType _udpLayer, ICMPTransportLayerType& _icmpLayer, ARPTransportLayerType& _arpLayer) :
        packetState(Unknown),
        tcpLayer(_tcpLayer),
        udpLayer(_udpLayer),
        icmpLayer(_icmpLayer),
        arpLayer(_arpLayer)
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
                ObjectSelect(protocolIndex, layerList, protocolIndex ).NewPacket();
            default:
                printf("(IPv4) data.\n");

                //
                // Data portion of the IP packet.
                //
                if(tcpLayer.State() != Rejected)
                {
                    //PushInto(byte, protocol, tcpLayer,udpLayer,icmpLayer,arpLayer);
                    ObjectSelect!(protocolIndex, layerList, protocolIndex ).PushInto(byte);
                }

                //
                // Macro:
                // ObjectList()
                //
                layerList   = "tcpLayer,udpLayer,icmpLayer,arpLayer";

                //
                // Macro:
                // ObjectSelect()
                //
                
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
    uint8_t PullFrom(bool& dataAvailable)
    {
        //
        // TODO: Pull from all upper layers, one whole packet at a time.
        //
        return tcpLayer.PullFrom( dataAvailable );
    }

private:

    //
    //
    //
    template < typename T0, typename T1, typename T2, typename T3 > void PushInto(uint8_t byte, uint8_t index,  T0 p0, T1 p1, T2 p2, T3 p3 )
    {
        switch(index)
        {
            case 6:
                p0.PushInto(byte);
                break;

            case 17:
                p1.PushInto(byte);
                break;

            case 1:
                p2.PushInto(byte);
                break;

            case 54:
                p3.PushInto(byte);
                break;
        }
    }




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

    TCPTransportLayerType&  tcpLayer;
    UDPTransportLayerType&  udpLayer;
    ICMPTransportLayerType& icmpLayer;
    ARPTransportLayerType&  arpLayer;

};





#endif



