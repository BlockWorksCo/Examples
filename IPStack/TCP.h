//
// Copyright (C) BlockWorks Consulting Ltd - All Rights Reserved.
// Unauthorized copying of this file, via any medium is strictly prohibited.
// Proprietary and confidential.
// Written by Steve Tickle <Steve@BlockWorks.co>, September 2014.
//








#ifndef __TCP_H__
#define __TCP_H__





//
// Generic public IP definitions.
//
struct TCPIP
{       

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


    //
    //
    //
    typedef struct
    {
        IP::ConnectionState     ipState;

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

        TCPFlags                packetToSend;

    } ConnectionState;
};




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

    TCP(ApplicationLayerType& _applicationLayer) :
        applicationLayer(_applicationLayer)
    {
        NewPacket();   
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
        applicationLayer.ConnectionState().position          = 0;
        applicationLayer.ConnectionState().packetState       = Unknown;
        applicationLayer.ConnectionState().dataOffset        = 20;   // 5 words minimum.
        applicationLayer.ConnectionState().position          = 0;
        applicationLayer.ConnectionState().packetState       = Unknown;
        applicationLayer.ConnectionState().packetToSend      = TCPIP::TCP_NONE;
    }

    //
    // Push some received data into this packet processor...
    //
    void PushInto(uint8_t byte)
    {
        //
        // Header portion of the packet.
        //
        switch( applicationLayer.ConnectionState().position )
        {
            case 0:
                applicationLayer.ConnectionState().sourcePort  = byte << 8;
                break;

            case 1:
                applicationLayer.ConnectionState().sourcePort  |= byte;
                LoggerType::printf("(TCP) sourcePort: %d\n", applicationLayer.ConnectionState().sourcePort);
                break;

            case 2:
                applicationLayer.ConnectionState().destinationPort  = byte << 8;
                break;

            case 3:
                applicationLayer.ConnectionState().destinationPort  |= byte;

                //
                // TODO: Switch which applicationLayer & ConnectionState() we use here, based on the destinationPort.
                //
                LoggerType::printf("(TCP) destinationPort: %d\n", applicationLayer.ConnectionState().destinationPort);
                break;

            case 4:
                applicationLayer.ConnectionState().sequenceNumber  = byte << 24;
                break;

            case 5:
                applicationLayer.ConnectionState().sequenceNumber  |= byte << 16;
                break;

            case 6:
                applicationLayer.ConnectionState().sequenceNumber  |= byte << 8;
                break;

            case 7:
                applicationLayer.ConnectionState().sequenceNumber  |= byte;
                LoggerType::printf("(TCP) sequenceNumber: %08x\n", applicationLayer.ConnectionState().sequenceNumber);
                break;

            case 8:
                applicationLayer.ConnectionState().ackNumber  = byte << 24;
                break;

            case 9:
                applicationLayer.ConnectionState().ackNumber  |= byte << 16;
                break;

            case 10:
                applicationLayer.ConnectionState().ackNumber  |= byte << 8;
                break;

            case 11:
                applicationLayer.ConnectionState().ackNumber  |= byte;
                LoggerType::printf("(TCP) ackNumber: %08x\n", applicationLayer.ConnectionState().ackNumber);
                break;

            case 12:
                applicationLayer.ConnectionState().dataOffset  = byte >> 4;
                applicationLayer.ConnectionState().dataOffset  *= 4;
                LoggerType::printf("(TCP) dataOffset: %d\n", applicationLayer.ConnectionState().dataOffset);
                break;

            case 13:
                applicationLayer.ConnectionState().flags   = static_cast<TCPIP::TCPFlags>(byte);
                LoggerType::printf("(TCP) Flags: %02x\n", applicationLayer.ConnectionState().flags);
                break;

            case 14:
                applicationLayer.ConnectionState().windowSize  = byte << 8;
                break;

            case 15:
                applicationLayer.ConnectionState().windowSize  |= byte;
                LoggerType::printf("(TCP) windowSize: %d\n", applicationLayer.ConnectionState().windowSize);
                break;

            case 16:
                applicationLayer.ConnectionState().checksum  = byte << 8;
                break;

            case 17:
                applicationLayer.ConnectionState().checksum  |= byte;
                LoggerType::printf("(TCP) checksum: %04x\n", applicationLayer.ConnectionState().checksum);
                break;

            case 18:
                applicationLayer.ConnectionState().urgentPointer  = byte << 8;
                break;

            case 19:
                applicationLayer.ConnectionState().urgentPointer  |= byte;
                LoggerType::printf("(TCP) urgentPointer: %02x\n", applicationLayer.ConnectionState().urgentPointer);
                break;

            case 20:
                LoggerType::printf("(TCP) AppDataStart\n");
                applicationLayer.NewPacket();

            default:

                if( applicationLayer.ConnectionState().position >= applicationLayer.ConnectionState().dataOffset )
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

                break;
        }
        

        //
        // Ready for next byte.
        //
        applicationLayer.ConnectionState().position++;
    }


    //
    //
    //
    PacketProcessingState State()
    {
        return applicationLayer.ConnectionState().packetState;
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
        TCPIP::TCPState    currentState;
        TCPIP::TCPState    nextTCPState;  

        applicationLayer.GetTCPState(currentState, nextTCPState);

        switch( currentState )
        {
            case TCPIP::LISTEN:
                if( (applicationLayer.ConnectionState().flags&TCPIP::TCP_SYN) != 0)
                {
                    //
                    // Send a SynAck packet.
                    //
                    applicationLayer.ConnectionState().packetToSend    = static_cast<TCPIP::TCPFlags>(TCPIP::TCP_ACK | TCPIP::TCP_SYN);
                    nextTCPState    = TCPIP::SYN_SENT;
                }

                break;

            case TCPIP::SYN_SENT:
                if( (applicationLayer.ConnectionState().flags&TCPIP::TCP_SYN) != 0)
                {
                    //
                    // Send a SynAck packet.
                    //
                    applicationLayer.ConnectionState().packetToSend    = static_cast<TCPIP::TCPFlags>(TCPIP::TCP_ACK);
                    nextTCPState    = TCPIP::SYN_SENT;
                }

                if( (applicationLayer.ConnectionState().flags&TCPIP::TCP_ACK) != 0)
                {
                    //
                    // Connection established.
                    //
                    applicationLayer.ConnectionState().packetToSend    = static_cast<TCPIP::TCPFlags>(TCPIP::TCP_NONE);
                    currentState    = TCPIP::ESTABLISHED;
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
        LoggerType::printf("- %04x - %04x -", value, applicationLayer.ConnectionState().accumulatedChecksum);
        applicationLayer.ConnectionState().accumulatedChecksum     += value;
        if( applicationLayer.ConnectionState().accumulatedChecksum > 0xffff )
        {
            applicationLayer.ConnectionState().accumulatedChecksum -= 0xffff;
        }
    }    

    //
    // Pull some packet data out of the processor for transmission.
    //
    uint8_t PullFrom(bool& dataAvailable, uint16_t position)
    {
        const uint8_t   dataOffset          = (sizeofTCPHeader / 4) << 4;
        uint8_t         byteToSend          = 0x00;
        /*
        applicationLayer.ConnectionState().sourcePort          = 80;
        applicationLayer.ConnectionState().destinationPort     = 8080;
        applicationLayer.ConnectionState().sequenceNumber      = 0x123;
        applicationLayer.ConnectionState().ackNumber           = 0x0000;
        applicationLayer.ConnectionState().urgentPointer       = 0x0000;
        applicationLayer.ConnectionState().windowSize          = 822;
        applicationLayer.ConnectionState().flags               = static_cast<TCPIP::TCPFlags>(0);
        */

        dataAvailable   = true;

        switch(position)
        {
            case 0:
                byteToSend  = applicationLayer.ConnectionState().sourcePort >> 8;
                break;

            case 1:
                byteToSend  = applicationLayer.ConnectionState().sourcePort & 0xff;
                break;

            case 2:
                byteToSend  = applicationLayer.ConnectionState().destinationPort >> 8;
                break;

            case 3:
                byteToSend  = applicationLayer.ConnectionState().destinationPort & 0xff;
                break;

            case 4:
                byteToSend  = (applicationLayer.ConnectionState().sequenceNumber >> 24) & 0xff;
                break;

            case 5:
                byteToSend  = (applicationLayer.ConnectionState().sequenceNumber >> 16) & 0xff;
                break;

            case 6:
                byteToSend  = (applicationLayer.ConnectionState().sequenceNumber >> 8) & 0xff;
                break;

            case 7:
                byteToSend  = (applicationLayer.ConnectionState().sequenceNumber) & 0xff;
                break;

            case 8:
                byteToSend  = (applicationLayer.ConnectionState().ackNumber >> 24) & 0xff;
                break;

            case 9:
                byteToSend  = (applicationLayer.ConnectionState().ackNumber >> 16) & 0xff;
                break;

            case 10:
                byteToSend  = (applicationLayer.ConnectionState().ackNumber >> 8) & 0xff;
                break;

            case 11:
                byteToSend  = (applicationLayer.ConnectionState().ackNumber) & 0xff;
                break;

            case 12:
                byteToSend  = dataOffset;
                break;

            case 13:
                byteToSend  = applicationLayer.ConnectionState().flags;
                break;

            case 14:
                byteToSend  = applicationLayer.ConnectionState().windowSize >> 8;
                break;

            case 15:
                byteToSend  = applicationLayer.ConnectionState().windowSize & 0xff;
                break;

            case 16:

                applicationLayer.ConnectionState().accumulatedChecksum     = 0;                    

                //
                // Psuedo header portion of the checksum.
                //
                UpdateAccumulatedChecksum( ConnectionState().destinationIP >> 16 );       // source IP, us.
                UpdateAccumulatedChecksum( ConnectionState().destinationIP & 0xffff );    //
                UpdateAccumulatedChecksum( ConnectionState().sourceIP >> 16 );            // Dest IP, remote.
                UpdateAccumulatedChecksum( ConnectionState().sourceIP & 0xffff );         //
                UpdateAccumulatedChecksum( IP::TCP );                                   // always 6, TCP
                UpdateAccumulatedChecksum( PacketLength() );                            // *Note: the whole TCP segment, length, not just the applications.

                //
                // TCP header portion of the checksum
                //
                UpdateAccumulatedChecksum( applicationLayer.ConnectionState().sourcePort );
                UpdateAccumulatedChecksum( applicationLayer.ConnectionState().destinationPort );
                UpdateAccumulatedChecksum( applicationLayer.ConnectionState().sequenceNumber >> 16 );
                UpdateAccumulatedChecksum( applicationLayer.ConnectionState().sequenceNumber &0xffff );
                UpdateAccumulatedChecksum( applicationLayer.ConnectionState().ackNumber >> 16 );
                UpdateAccumulatedChecksum( applicationLayer.ConnectionState().ackNumber & 0xffff );
                UpdateAccumulatedChecksum( ((uint16_t)dataOffset<<8) | (uint16_t)applicationLayer.ConnectionState().flags );
                UpdateAccumulatedChecksum( applicationLayer.ConnectionState().windowSize );
                UpdateAccumulatedChecksum( applicationLayer.ConnectionState().urgentPointer );

                //
                // Data portion of the checksum
                //
                for(uint16_t i=0; i<applicationLayer.PacketLength(); i+=2)
                {
                    bool        moreDataAvailable   = false;
                    uint8_t     hiByte              = applicationLayer.PullFrom(moreDataAvailable, i);
                    uint8_t     loByte              = applicationLayer.PullFrom(moreDataAvailable, i+1);
                    UpdateAccumulatedChecksum( ((uint16_t)hiByte<<8) | (uint16_t)loByte  );
                }
                applicationLayer.ConnectionState().accumulatedChecksum    = ~applicationLayer.ConnectionState().accumulatedChecksum;
                LoggerType::printf("TCP Checksum: %04x", applicationLayer.ConnectionState().accumulatedChecksum );

                // 
                byteToSend  = applicationLayer.ConnectionState().accumulatedChecksum >> 8;
                break;

            case 17:
                byteToSend  = applicationLayer.ConnectionState().accumulatedChecksum & 0xff;
                break;

            case 18:
                byteToSend  = applicationLayer.ConnectionState().urgentPointer >> 8;
                break;

            case 19:
                byteToSend  = applicationLayer.ConnectionState().urgentPointer & 0xff;
                break;

            default:
                byteToSend  = applicationLayer.PullFrom(dataAvailable, position-sizeofTCPHeader);
                break;
        }

        return byteToSend;
    }


    uint32_t DestinationIP()
    {
        return ConnectionState().sourceIP;
        //return applicationLayer.DestinationIP();
    }

    uint16_t PacketLength()
    {
        return applicationLayer.PacketLength() + sizeofTCPHeader;
    }

    IP::ConnectionState& ConnectionState()
    {
        return applicationLayer.ConnectionState().ipState;
    }

private:


    const uint16_t  sizeofTCPHeader     = 20;

    //
    //
    //

    ApplicationLayerType&   applicationLayer;

};















#if 0

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>





uint32_t accumulatedChecksum    = 0;

//
//
//
void UpdateAccumulatedChecksum(uint16_t value)
{
    printf("- %04x - %04x -\n", value, accumulatedChecksum);
    accumulatedChecksum     += value;
    if( accumulatedChecksum > 0xffff )
    {
        accumulatedChecksum -= 0xffff;
    }
}    


uint16_t tcp_accumulatedChecksum_calc(uint16_t len_tcp, uint32_t src_addr,uint32_t dest_addr, int padding, uint16_t buff[])
{
    uint16_t prot_tcp=6;
    uint16_t padd=0;
    uint16_t word16;
    int     i;
    
    // Find out if the length of data is even or odd number. If odd,
    // add a padding byte = 0 at the end of packet
    if (padding&1==1)
    {
        padd=1;
        buff[len_tcp]=0;
    }
    
    //initialize accumulatedChecksum to zero
    accumulatedChecksum=0;

    // add the TCP pseudo header which contains:
    // the IP source and destinationn addresses,
    UpdateAccumulatedChecksum(src_addr >> 16);
    UpdateAccumulatedChecksum(src_addr & 0xffff);
    UpdateAccumulatedChecksum(dest_addr >> 16);
    UpdateAccumulatedChecksum(dest_addr & 0xffff);
    UpdateAccumulatedChecksum(6);
    UpdateAccumulatedChecksum(len_tcp);
    
    // make 16 bit words out of every two adjacent 8 bit words and 
    // calculate the accumulatedChecksum of all 16 vit words
    for (i=0; i<len_tcp+padd; i=i+2)
    {
        UpdateAccumulatedChecksum( ((buff[i]<<8)&0xFF00) + (buff[i+1]&0xFF) );
    }   

    // Take the one's complement of accumulatedChecksum
    accumulatedChecksum = ~accumulatedChecksum;

    return ((unsigned short) accumulatedChecksum);
}


void main()
{ 
    uint16_t    data[] = {0x00 , 0x50 , 0x1f , 0x90 , 0x00 , 0x00 , 0x01 , 0x23 , 0x00 , 0x00 , 0x00 , 0x00 , 0x50 , 0x00 , 0x03 , 0x36 , 0xf0 , 0xc1 , 0x00, 0x00, 0xdc ,0x7a ,0x76 ,0xe9};
/*
    memset(&data[0], 0, sizeof(data));
    data[12]    = 0x50;

    data[20]    = 0x00;
    data[21]    = 0x01;
    data[22]    = 0x02;
    data[23]    = 0x03;
    data[24]    = 0x04;
    data[25]    = 0x05;
    data[26]    = 0x06;
    data[27]    = 0x07;
    data[28]    = 0x08;
    data[29]    = 0x09;
*/
    uint16_t     c   = tcp_accumulatedChecksum_calc( sizeof(data), 0xc0a80279, 0xc0a802fd, 0, data);

    printf("%04x\n",c);
}



#endif



#endif













