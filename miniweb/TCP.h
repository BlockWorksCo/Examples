





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
        NONE,
        WAIT
    } packetflags;


    typedef enum
    {
        TCP_FIN           = 0x01,
        TCP_SYN           = 0x02,
        TCP_RST           = 0x04,
        TCP_PSH           = 0x08,
        TCP_ACK           = 0x10,
        TCP_URG           = 0x20,        
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


    typedef struct _tcpip_header
    {
        struct _tcpip_header*   next;
        packetflags     flag;
        uint8_t         length;  /* The size of the data contained
                                   within the packet (i.e., minus TCP and IP
                                   headers.) */
        /* This is the IP header. */
        uint8_t         vhl;  /* IP version and header length. */
        uint8_t         tos;              /* Type of service. */
        uint8_t         len[2];           /* Total length. */
        uint8_t         id[2];            /* IP identification. */
        uint8_t         ipoffset[2];      /* IP fragmentation offset. */
        uint8_t         ttl;              /* Time to live. */
        uint8_t         protocol;         /* Protocol. */
        uint8_t         ipchksum[2];      /* IP header checksum. */
        uint8_t         srcipaddr[4];     /* The source IP address. */
        /* destipaddr[4]     We don't store the destination IP address here. */
        /* This is the TCP header. */
        uint8_t         srcport[2];  /* TCP source port. */
                 /* destport[2]       We don't store the destination TCP port here. */
        uint8_t         seqno[4];         /* TCP sequence number. */
                 /* ackno[4]          We don't store acknowledgement number here. */
        uint8_t         tcpoffset;        /* 4 unused bits and TCP data offset. */
        uint8_t         flags;            /* TCP flags. */
        uint8_t         wnd[2];          /* Advertised receiver's window. */
        uint8_t         tcpchksum[2];     /* TCP checksum. */
        uint8_t         urgp[2];          /* Urgent pointer. */
        uint8_t         data[0];
    } tcpip_header;

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
        printf("(TCP)\n");
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



};



#endif













