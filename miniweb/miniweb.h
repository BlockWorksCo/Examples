/* Copyright (c) 2001, Adam Dunkels. All rights reserved.
 *
 * Redistribution or modification is prohibited.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
#ifndef __MINIWEB_H__
#define __MINIWEB_H__



#include <stdio.h>





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
template < typename IPStackType >
class MiniWebServer
{
    //
    // Break out the CombinationTypes for this class.
    //
    typedef typename IPStackType::WebServerType         WebServerType;
    typedef typename IPStackType::PacketInterfaceType   PacketInterfaceType;
    typedef typename IPStackType::PacketGeneratorType   PacketGeneratorType;



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

    //
    //
    //
    MiniWebServer( PacketInterfaceType& _packetInterface, PacketGeneratorType& _packetGenerator) :
        cwnd(1),
        tcpstate(LISTEN),
        inflight(0),
        nrtx(0),
        packetGenerator(_packetGenerator),
        packetInterface(_packetInterface)
    {
    }

    //
    //
    //
    void resetStack()
    {
        cwnd        = 1;
        inflight    = 0;
        nrtx        = 0;
    }

    //
    //
    //
    void Iterate(void)
    {
    drop:
        packetInterface.drop();


        /* The content of the y register signals whether we should send
           out a new packet once the input processing is done. y = 0 means
           that we should not send out a packet and y != 0 means that we
           should send out a packet. */
        y = Y_NORESPONSE;


        chksum[0] = chksum[1] = 0;
        chksumflags = 0;

        /* Get first byte of IP packet, which is the IP version number and
           IP header length. */
        a   = packetInterface.wait();
        ADD_CHK(x);

        /* We discard every packet that isn't IP version 4 and that has IP
           options. */
        if(a != 0x45)
        {
            printf("Packet dropped due to options or version mismatch\n");
            goto drop;
        }

        /* IP Type of Service field, discard. */
        a  = next<uint8_t>();

        /* IP packet length. */
        //a  = nextByteIn();
        //len = a << 8;
        //a  = nextByteIn();
        //len |= a;
        next<uint16_t>();

        /* IP ID, discard. */
        a  = next<uint8_t>();
        a  = next<uint8_t>();

        /* Fragmentation offset. */
        a  = next<uint8_t>();

        if((a & 0x20) || (a & 0x1f) != 0)
        {
            printf("Got IP fragment, dropping\n");
            goto drop;
        }

        a  = next<uint8_t>();

        if(a != 0)
        {
            printf("Got IP fragment, dropping\n");
            goto drop;
        }

        /* TTL, discard. */
        a  = next<uint8_t>();

        /* Get the IP protocol field. If this isn't a TCP packet, we drop
           it. */
        a  = next<uint8_t>();

        if(a != IP_PROTO_TCP)
        {
            printf("Not a TCP packet, dropping\n");
            goto drop;
        }

        /* Get the IP checksum field, and discard it. */
        //a  = nextByteIn();
        //a  = nextByteIn();
        next<uint16_t>();

        /* Get the source address of the packet, which we will use as the
           destination address for our replies. */
#if 1
        a  = next<uint8_t>();
        ipaddr[0] = a;
        a  = next<uint8_t>();
        ipaddr[1] = a;
        a  = next<uint8_t>();
        ipaddr[2] = a;
        a  = next<uint8_t>();
        ipaddr[3] = a;
#else        
        uint32_t ipaddr = next<uint32_t>();
#endif        

        /* And we discard the destination IP address. */
        //a  = nextByteIn();
        //a  = nextByteIn();
        //a  = nextByteIn();
        //a  = nextByteIn();
        next<uint32_t>();

        /* Check the computed IP header checksum. If it fails, we go ahead
           and drop the packet. */
        while(c)
        {
            ADD_CHK(0);
        }

        if(chksum[0] != 0xff || chksum[1] != 0xff)
        {
            printf("Failed IP header checksum, dropping\n");
            goto drop;
        }

        /* Thus the IP processing is done with, and we carry on with the
           TCP layer. */

        chksum[0] = chksum[1] = 0;
        c = 0;
        chksumflags = 0;
        /* Get the source TCP port and store it for our replies. */

        a  = next<uint8_t>();

        if(tcpstate == LISTEN || tcpstate == TIME_WAIT)
        {
            srcport[0] = a;
        }
        else if(srcport[0] != a)
        {
            printf("Got new port and not in LISTEN or TIME_WAIT, dropping packet\n");
            goto drop;
        }

        a  = next<uint8_t>();

        if(tcpstate == LISTEN || tcpstate == TIME_WAIT)
        {
            srcport[1] = a;
        }
        else if(srcport[1] != a)
        {
            printf("Got new port and not in LISTEN or TIME_WAIT, dropping packet\n");
            goto drop;
        }



        /* Get the TCP destination port. */
        a  = next<uint8_t>();
        a  = next<uint8_t>();


        if(tcpstate == LISTEN || tcpstate == TIME_WAIT)
        {
            port = a;
        }



        if(packetGenerator.isPortAccepted(port) == true)
        {
            printf("Port outside range %d\n", port);
            goto drop;
        }

        /* Get the TCP sequence number. */
        a  = next<uint8_t>();
        seqno[0] = a;
        a  = next<uint8_t>();
        seqno[1] = a;
        a  = next<uint8_t>();
        seqno[2] = a;
        a  = next<uint8_t>();
        seqno[3] = a;
        //uint32_t seqNum = next<uint32_t>();

        /* Next, check the acknowledgement. If it acknowledges outstanding
           data, we move the state pointer upwards. (This has room for
           massive assembler optimizations.) Since we never send out any
           sequence numbers that wrap, we can use standard arithmetic
           here. */



        if(tcpstate != LISTEN)
        {


            for(x = 0; x < 4; x ++)
            {
                a  = next<uint8_t>();

                while(stateptr != NULL && a > stateptr->seqno[x])
                {
                    stateptr = stateptr->next;

                    y = Y_RESPONSE;
                    inflight--;

                }

                if(stateptr == NULL)
                {

                    y = Y_NORESPONSE;
                    tcpstate = LISTEN;
                    printf("Stateptr == NULL, connection dropped\n");
                }
            }


        }
        else
        {
            for(x = 0; x < 4; x++)
            {
                a  = next<uint8_t>();
            }
        }



        /* Get the TCP offset and use it in the following computation. */
        a  = next<uint8_t>();

        /* If this segment contains TCP data, we increase the sequence
           number we acknowledge by the size of this data. */
        if(len - 20 - (a >> 4) * 4 > 0)   /* a is the TCP data offset. */
        {
            chksumflags = (chksumflags & CHKSUMFLAG_BYTE) | (c & 1);
            c = 0;
            ADC(seqno[3], c, (len - 20 - (a >> 4) * 4) & 0xff);
            ADC(seqno[2], c, (len - 20 - (a >> 4) * 4) >> 8);
            ADC(seqno[1], c, 0);
            ADC(seqno[0], c, 0);
            c = chksumflags & CHKSUMFLAG_CARRY;

            y = Y_NEWDATA;

        }

        /* TCP flags. */
        a  = next<uint8_t>();

        if(a & TCP_RST)
        {
            stateptr = (tcpip_header*)NULL;
            goto drop;
        }



        /* If this is an ACK, increase congestion window by one segment
           (this is slow start). */
        if(y == 1 && (a & TCP_ACK))
        {
            cwnd++;
        }




        if(a & TCP_SYN || a & TCP_FIN)
        {

            cwnd = 1;

            if(a & TCP_FIN)
            {
                tcpstate = TIME_WAIT;
            }
            else if(a & TCP_SYN)
            {
                tcpstate = ESTABLISHED;
                //stateptr = pages[port - PORTLOW];
                stateptr    = packetGenerator.packetForPort(port);
                tmpstateptr = stateptr;
#if 1
                printf("New connection from %d.%d.%d.%d:%d\n", ipaddr[0], ipaddr[1], ipaddr[2], ipaddr[3], (srcport[0] << 8) + srcport[1]);
#else
                printf("New connection from %08x:%d\n", ipaddr, (srcport[0] << 8) + srcport[1]);
#endif                
                inflight = 0;
            }

            y = Y_NEWDATA;

            /* Increase the seqno we acknowledge by 1 */
            chksumflags = (chksumflags & CHKSUMFLAG_BYTE) | (c & 1);
            c = 0;
            ADC(seqno[3], c, 1);
            ADC(seqno[2], c, 0);
            ADC(seqno[1], c, 0);
            ADC(seqno[0], c, 0);
            c = chksumflags & CHKSUMFLAG_CARRY;
        }

        /* Get the high byte of the TCP window and limit our sending rate
           if needed. */
        a  = next<uint8_t>();


        if(a < cwnd + inflight)
        {
            /*      printf("Limited by receiver's window %d new window %d.\n", a, a - inflight);*/
            cwnd = a - inflight;
        }



        /* Discard the low byte of the TCP window, checksum and the urgent
           pointer. */
        for(x = 0; x < 5; x ++)
        {
            a  = next<uint8_t>();
        }

        /* We continue checksumming the rest of the packet. */

        /* Add the changing part of the pseudo checksum. */
        ADD_CHK1(ipaddr[0]);
        ADD_CHK2(ipaddr[1]);
        ADD_CHK1(ipaddr[2]);
        ADD_CHK2(ipaddr[3]);
        ADD_CHK1((len >> 8) & 0xff);
        ADD_CHK2(len & 0xff);

        for(len = len - 40; len > 0; len--)
        {
            a  = next<uint8_t>();
        }

        while(c)
        {
            ADD_CHK(0);
        }

        /*    printf("TCP checksum 0x%02x%02x\n", chksum[0], chksum[1]);*/

        /* We compare the calculated checksum with the precalculated
           pseudo header checksum. If they don't match, don't send. */
        if(chksum[0] == TCP_CHECK0 && chksum[1] == TCP_CHECK1)
        {


            /* If y is larger than Y_RESPONSE, we should send a packet in
               response to the incoming one. If we are told to wait for new
               data (as indicated by stateptr->flag == WAIT), y has to be set
               to Y_NEWDATA in order for us to respond. */
            if(tmpstateptr != NULL && y >= Y_RESPONSE)
            {
                if(tmpstateptr->flag != WAIT || y == Y_NEWDATA )
                {
                    nrtx = 0;
                    tcpip_output();
                }

            }

        }
        else
        {
            printf("Packet dropped due to failing TCP checksum.\n");
        }
    }





    //
    //
    //
    void timerTick(void)
    {
        timer++;

        if(stateptr != NULL)
        {
            if(timer - txtime > 4 &&
                    stateptr->flag != WAIT)
            {

                cwnd = 1;
                inflight = 0;
                tmpstateptr = stateptr;

                printf("Retransmitting\n");
                tcpip_output();
                nrtx++;

                if(nrtx == 8)
                {
                    resetStack();
                }
            }

            else if(timer - txtime > 8 &&
                    stateptr->flag == WAIT)
            {
                printf("Connection dropped\n");
                stateptr = (tcpip_header*)NULL;
                resetStack();
            }
        }
    }




private:
  

    //
    //
    //
    void ADD_CHK1(uint8_t x)
    {
        ADC(chksum[0], c, x);
    }     

    //
    //
    //
    void ADD_CHK2(uint8_t x)
    {
        ADC(chksum[1], c, x);
    }     

    //
    //
    //
    void ADC(uint8_t a, uint8_t c, uint8_t x)
    {
        adc(&a, &c, x);    
    }    

    //
    //
    //
    void ADD_CHK(uint8_t x)
    {
        add_chk(x);
    }   

    //
    //
    //
    void adc(uint8_t* a, uint8_t* c, uint8_t x)
    {
        uint16_t tmp;

        tmp = *a + x + *c;
        *a = tmp & 0xff;
        *c = tmp >> 8;
    }

    //
    //
    //
    void add_chk(uint8_t x)
    {
        ADC(chksum[(chksumflags & CHKSUMFLAG_BYTE) >> 1], c, x);
        chksumflags ^= CHKSUMFLAG_BYTE;
    }

    //
    //
    //
    template <typename T> T next(void)
    {
        T   value;
        uint8_t*    ptr     = (uint8_t*)&value;

        for(uint8_t i=0; i<sizeof(T); i++)
        {
            *ptr  = packetInterface.get();
            ADD_CHK( *ptr );
            ptr++;
        }

        return value;
    }


    //
    //
    //
    template <typename T> void put(T value)
    {
        packetInterface.template put<T>(value);
    }


    //
    //
    //
    void tcpip_output(void)
    {
        txtime = timer;



        if(tmpstateptr == NULL)
        {
            printf("tmpstateptr == NULL!\n");
            return;
        }

        do
        {
            tmpptr = &(tmpstateptr->vhl);

            /* Send vhl, tos, len, id, ipoffset, ttl and protocol. */
            for(x = 0; x < 10; x++)
            {
                uint8_t value = *(tmpptr++);
                put<uint8_t>(value);
            }

            /* Fiddle with the checksum. This can be done more efficiently
               in assembler, where we have the option of adding with
               carry. */
            chksum[1] = *(tmpptr++);
            chksum[0] = *(tmpptr++);
            c = 0;
            ADD_CHK1(ipaddr[3]);
            ADD_CHK2(ipaddr[2]);
            ADD_CHK1(ipaddr[1]);
            ADD_CHK2(ipaddr[0]);

            while(c)
            {
                ADD_CHK1(0);
                ADD_CHK2(0);
            }

            /* Send bytes. */
            a = ~(chksum[1]);
            put<uint8_t>(a);
            a = ~(chksum[0]);
            put<uint8_t>(a);

            /* Send source IP address. */
            for(x = 4; x > 0; x--)
            {
                put<uint8_t>(*(tmpptr++));
            }

            /* Send destination address. */
            for(; x < 4; x++)
            {
                put<uint8_t>(ipaddr[x]);
            }

            /* Send TCP source port. */
            for(x = 0; x < 2; x++)
            {
                put<uint8_t>(*(tmpptr++));
            }

            /* Send TCP destination port. */
            put<uint8_t>(srcport[0]);
            put<uint8_t>(srcport[1]);
            /* Send TCP sequence number. */
            put<uint8_t>(*(tmpptr++));
            put<uint8_t>(*(tmpptr++));
            put<uint8_t>(*(tmpptr++));
            put<uint8_t>(*(tmpptr++));

            /* Send TCP acknowledgement number. */
            put<uint8_t>(seqno[0]);
            put<uint8_t>(seqno[1]);
            put<uint8_t>(seqno[2]);
            put<uint8_t>(seqno[3]);

            /* Send offset, flags and window. */
            put<uint8_t>(*(tmpptr++));
            put<uint8_t>(*(tmpptr++));
            put<uint8_t>(*(tmpptr++));
            put<uint8_t>(*(tmpptr++));

            /* Fiddle with the checksum. This can be done more efficiently
               in assembler, where we have the option of adding with
               carry. */
            chksum[1] = *(tmpptr++);
            chksum[0] = *(tmpptr++);
            c = 0;
            ADD_CHK1(ipaddr[3]);
            ADD_CHK2(ipaddr[2]);
            ADD_CHK1(ipaddr[1]);
            ADD_CHK2(ipaddr[0]);

            ADD_CHK1(srcport[1]);
            ADD_CHK2(srcport[0]);

            ADD_CHK1(seqno[3]);
            ADD_CHK2(seqno[2]);
            ADD_CHK1(seqno[1]);
            ADD_CHK2(seqno[0]);

            while(c)
            {
                ADD_CHK1(0);
                ADD_CHK2(0);
            }

            /* Send bytes. */
            a = ~(chksum[1]);
            put<uint8_t>(a);
            a = ~(chksum[0]);
            put<uint8_t>(a);

            /* Send urgent pointer. */
            for(x = 0; x < 2; x++)
            {
                put<uint8_t>(*(tmpptr++));
            }

            /* Send the rest of the packet. */


            for(x = 0; x < tmpstateptr->length; x++)
            {
                put<uint8_t>(*(tmpptr++));
            }


            packetInterface.done();

            inflight++;
            tmpstateptr = tmpstateptr->next;
        }
        while(inflight < cwnd && tmpstateptr != NULL &&
                tmpstateptr->flag != WAIT);


    }



private:

    //
    //
    //
    /* These are kept in CPU registers. */
    uint8_t               a;
    uint8_t               x;
    uint8_t               y;
    uint8_t               c;

    /* These are kept in RAM. */
    uint8_t               ipaddr[4];
    uint8_t               srcport[2];
    uint8_t               port;
    uint8_t               seqno[4];
    tcpip_header*         stateptr;

    uint8_t               chksum[2];
    unsigned short        len;
    uint8_t*              tmpptr;

    tcpip_header*         tmpstateptr;
    uint8_t               cwnd;
    uint8_t               tcpstate;
    uint8_t               inflight;

    /* These actually only need four bits each. */
    uint8_t               timer;
    uint8_t               txtime;
    uint8_t               nrtx;

    /* Only two bits of state needed here. */
    uint8_t               chksumflags;


    PacketGeneratorType&  packetGenerator;
    PacketInterfaceType&  packetInterface;



};



#endif /* __MINIWEB_H__ */





