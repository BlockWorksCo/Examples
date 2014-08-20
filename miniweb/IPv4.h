




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







const uint8_t TCP_MSS           = 200;
const uint16_t TCP_WIN          = 4096;

const uint8_t CHKSUMFLAG_BYTE   = 2;
const uint8_t CHKSUMFLAG_CARRY  = 1;

const uint8_t Y_NORESPONSE      = 0;
const uint8_t Y_RESPONSE        = 1;
const uint8_t Y_NEWDATA         = 2;

const uint8_t IPADDR0           = 192;
const uint8_t IPADDR1           = 168;
const uint8_t IPADDR2           = 0;
const uint8_t IPADDR3           = 2;

const uint8_t TCP_CHECK0        = 0x3f;
const uint8_t TCP_CHECK1        = 0x63;

const uint8_t IP_PROTO_TCP      = 6;




//
//
//
template < typename StackType >
class IPv4
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

    //
    //
    //
    IPv4(TCPTransportLayerType& _tcpLayer, UDPTransportLayerType _udpLayer) :
        packetState(Unknown),
        tcpLayer(_tcpLayer),
        udpLayer(_udpLayer)
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

                default:
                    break;
            }
            
        }
        else
        {
            printf("(IPv4) data.\n");
            
            if(position == 20)
            {
                //
                // Start of payload data.
                //
                tcpLayer.NewPacket();
            }

            //
            // Data portion of the IP packet.
            //
            if(tcpLayer.State() != Rejected)
            {
                tcpLayer.PushInto(byte);
            }


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

    uint16_t                length;
    uint16_t                fragmentOffset;
    uint8_t                 fragmentFlags;
    uint16_t                headerChecksum;
    uint32_t                sourceIP;
    uint8_t                 protocol;

    TCPTransportLayerType&  tcpLayer;
    UDPTransportLayerType&  udpLayer;

};





#endif



