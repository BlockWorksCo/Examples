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
#include "dev.h"
#include <stdint.h>



#define IPADDR0 192
#define IPADDR1 168
#define IPADDR2 0
#define IPADDR3 2

#define TCP_CHECK0 0x3f
#define TCP_CHECK1 0x63

#define IPADDR htonl((IPADDR0 << 24) + (IPADDR1 << 16) + (IPADDR2 << 8) + IPADDR3)

#define PORTHIGH 85
#define PORTLOW  80

#define IP_PROTO_TCP 6

enum packetflags
{
    NONE,
    WAIT
};

struct tcpip_header
{
    struct tcpip_header* next;
    enum packetflags flag;
    unsigned char length;  /* The size of the data contained
                               within the packet (i.e., minus TCP and IP
                               headers.) */
    /* This is the IP header. */
    unsigned char vhl,  /* IP version and header length. */
             tos,              /* Type of service. */
             len[2],           /* Total length. */
             id[2],            /* IP identification. */
             ipoffset[2],      /* IP fragmentation offset. */
             ttl,              /* Time to live. */
             protocol,         /* Protocol. */
             ipchksum[2],      /* IP header checksum. */
             srcipaddr[4];     /* The source IP address. */
    /* destipaddr[4]     We don't store the destination IP address here. */
    /* This is the TCP header. */
    unsigned char srcport[2],  /* TCP source port. */
             /* destport[2]       We don't store the destination TCP port here. */
             seqno[4],         /* TCP sequence number. */
             /* ackno[4]          We don't store acknowledgement number here. */
             tcpoffset,        /* 4 unused bits and TCP data offset. */
             flags,            /* TCP flags. */
             wnd[2],           /* Advertised receiver's window. */
             tcpchksum[2],     /* TCP checksum. */
             urgp[2];          /* Urgent pointer. */
    unsigned char data[0];
};

void miniweb_main_loop(void);
void miniweb_timer(void);
void miniweb_init(void);

#define TCP_MSS 200
#define TCP_WIN 4096

#define TCP_FIN 0x01
#define TCP_SYN 0x02
#define TCP_RST 0x04
#define TCP_PSH 0x08
#define TCP_ACK 0x10
#define TCP_URG 0x20

#define CLOSED      0
#define LISTEN      1
#define SYN_SENT    2
#define SYN_RCVD    3
#define ESTABLISHED 4
#define FIN_WAIT_1  5
#define FIN_WAIT_2  6
#define CLOSE_WAIT  7
#define CLOSING     8
#define LAST_ACK    9
#define TIME_WAIT   10





    #define DPRINTF   printf
    #define CHKSUMFLAG_BYTE 2
    #define CHKSUMFLAG_CARRY 1

    #define Y_NORESPONSE    0
    #define Y_RESPONSE      1
    #define Y_NEWDATA       2

    #define ADD_CHK1(x)     ADC(chksum[0], c, x);
    #define ADD_CHK2(x)     ADC(chksum[1], c, x);
    #define ADC(a, c, x)    adc(&(a), &(c), x)
    #define ADD_CHK(x)      add_chk(x)
    #define DEV_GETC(x)     x = dev_getc()
    #define DEV_WAITC(x)    DEV_WAIT(x); ADD_CHK(x)





template < typename PacketInterfaceType >
class MiniWebServer
{

public:
    MiniWebServer(struct tcpip_header** _pages) :
        cwnd(1),
        tcpstate(LISTEN),
        inflight(0),
        nrtx(0),
        pages(_pages)
    {
    }


    //
    //
    //
    void miniweb_main_loop(void)
    {
        while(true)
        {
    drop:
            DEV_DROP();


            /* The content of the y register signals whether we should send
               out a new packet once the input processing is done. y = 0 means
               that we should not send out a packet and y != 0 means that we
               should send out a packet. */
            y = Y_NORESPONSE;


            chksum[0] = chksum[1] = 0;
            chksumflags = 0;

            /* Get first byte of IP packet, which is the IP version number and
               IP header length. */
            DEV_WAITC(a);

            /* We discard every packet that isn't IP version 4 and that has IP
               options. */
            if(a != 0x45)
            {
                DPRINTF("Packet dropped due to options or version mismatch\n");
                goto drop;
            }

            /* IP Type of Service field, discard. */
            DEV_GETC(a);

            /* IP packet length. */
            DEV_GETC(a);
            len = a << 8;
            DEV_GETC(a);
            len |= a;

            /* IP ID, discard. */
            DEV_GETC(a);
            DEV_GETC(a);

            /* Fragmentation offset. */
            DEV_GETC(a);

            if((a & 0x20) || (a & 0x1f) != 0)
            {
                DPRINTF("Got IP fragment, dropping\n");
                goto drop;
            }

            DEV_GETC(a);

            if(a != 0)
            {
                DPRINTF("Got IP fragment, dropping\n");
                goto drop;
            }

            /* TTL, discard. */
            DEV_GETC(a);

            /* Get the IP protocol field. If this isn't a TCP packet, we drop
               it. */
            DEV_GETC(a);

            if(a != IP_PROTO_TCP)
            {
                DPRINTF("Not a TCP packet, dropping\n");
                goto drop;
            }

            /* Get the IP checksum field, and discard it. */
            DEV_GETC(a);
            DEV_GETC(a);

            /* Get the source address of the packet, which we will use as the
               destination address for our replies. */
            DEV_GETC(a);
            ipaddr[0] = a;
            DEV_GETC(a);
            ipaddr[1] = a;
            DEV_GETC(a);
            ipaddr[2] = a;
            DEV_GETC(a);
            ipaddr[3] = a;

            /* And we discard the destination IP address. */
            DEV_GETC(a);
            DEV_GETC(a);
            DEV_GETC(a);
            DEV_GETC(a);

            /* Check the computed IP header checksum. If it fails, we go ahead
               and drop the packet. */
            while(c)
            {
                ADD_CHK(0);
            }

            if(chksum[0] != 0xff || chksum[1] != 0xff)
            {
                DPRINTF("Failed IP header checksum, dropping\n");
                goto drop;
            }

            /* Thus the IP processing is done with, and we carry on with the
               TCP layer. */

            chksum[0] = chksum[1] = 0;
            c = 0;
            chksumflags = 0;
            /* Get the source TCP port and store it for our replies. */

            DEV_GETC(a);

            if(tcpstate == LISTEN || tcpstate == TIME_WAIT)
            {
                srcport[0] = a;
            }
            else if(srcport[0] != a)
            {
                DPRINTF("Got new port and not in LISTEN or TIME_WAIT, dropping packet\n");
                goto drop;
            }

            DEV_GETC(a);

            if(tcpstate == LISTEN || tcpstate == TIME_WAIT)
            {
                srcport[1] = a;
            }
            else if(srcport[1] != a)
            {
                DPRINTF("Got new port and not in LISTEN or TIME_WAIT, dropping packet\n");
                goto drop;
            }



            /* Get the TCP destination port. */
            DEV_GETC(a);
            DEV_GETC(a);


            if(tcpstate == LISTEN || tcpstate == TIME_WAIT)
            {
                port = a;
            }



            if(port < PORTLOW || port >= PORTHIGH)
            {
                DPRINTF("Port outside range %d\n", port);
                goto drop;
            }

            /* Get the TCP sequence number. */
            DEV_GETC(a);
            seqno[0] = a;
            DEV_GETC(a);
            seqno[1] = a;
            DEV_GETC(a);
            seqno[2] = a;
            DEV_GETC(a);
            seqno[3] = a;

            /* Next, check the acknowledgement. If it acknowledges outstanding
               data, we move the state pointer upwards. (This has room for
               massive assembler optimizations.) Since we never send out any
               sequence numbers that wrap, we can use standard arithmetic
               here. */



            if(tcpstate != LISTEN)
            {


                for(x = 0; x < 4; x ++)
                {
                    DEV_GETC(a);

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
                        DPRINTF("Stateptr == NULL, connection dropped\n");
                    }
                }


            }
            else
            {
                for(x = 0; x < 4; x++)
                {
                    DEV_GETC(a);
                }
            }



            /* Get the TCP offset and use it in the following computation. */
            DEV_GETC(a);

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
            DEV_GETC(a);

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
                    tmpstateptr = stateptr = pages[port - PORTLOW];
                    DPRINTF("New connection from %d.%d.%d.%d:%d\n",
                           ipaddr[0], ipaddr[1], ipaddr[2], ipaddr[3],
                           (srcport[0] << 8) + srcport[1]);
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
            DEV_GETC(a);


            if(a < cwnd + inflight)
            {
                /*      DPRINTF("Limited by receiver's window %d new window %d.\n", a, a - inflight);*/
                cwnd = a - inflight;
            }



            /* Discard the low byte of the TCP window, checksum and the urgent
               pointer. */
            for(x = 0; x < 5; x ++)
            {
                DEV_GETC(a);
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
                DEV_GETC(a);
            }

            while(c)
            {
                ADD_CHK(0);
            }

            /*    DPRINTF("TCP checksum 0x%02x%02x\n", chksum[0], chksum[1]);*/

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
                DPRINTF("Packet dropped due to failing TCP checksum.\n");
            }
        }
    }

private:


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
    uint8_t dev_getc(void)
    {
        uint8_t x;
        DEV_GET(x);
        ADD_CHK(x);
        return x;
    }


    //
    //
    //
    void tcpip_output(void)
    {
        txtime = timer;



        if(tmpstateptr == NULL)
        {
            DPRINTF("tmpstateptr == NULL!\n");
            return;
        }

        do
        {
            tmpptr = &(tmpstateptr->vhl);

            /* Send vhl, tos, len, id, ipoffset, ttl and protocol. */
            for(x = 0; x < 10; x++)
            {
                DEV_PUT(*(tmpptr++));
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
            DEV_PUT(a);
            a = ~(chksum[0]);
            DEV_PUT(a);

            /* Send source IP address. */
            for(x = 4; x > 0; x--)
            {
                DEV_PUT(*(tmpptr++));
            }

            /* Send destination address. */
            for(; x < 4; x++)
            {
                DEV_PUT(ipaddr[x]);
            }

            /* Send TCP source port. */
            for(x = 0; x < 2; x++)
            {
                DEV_PUT(*(tmpptr++));
            }

            /* Send TCP destination port. */
            DEV_PUT(srcport[0]);
            DEV_PUT(srcport[1]);
            /* Send TCP sequence number. */
            DEV_PUT(*(tmpptr++));
            DEV_PUT(*(tmpptr++));
            DEV_PUT(*(tmpptr++));
            DEV_PUT(*(tmpptr++));

            /* Send TCP acknowledgement number. */
            DEV_PUT(seqno[0]);
            DEV_PUT(seqno[1]);
            DEV_PUT(seqno[2]);
            DEV_PUT(seqno[3]);

            /* Send offset, flags and window. */
            DEV_PUT(*(tmpptr++));
            DEV_PUT(*(tmpptr++));
            DEV_PUT(*(tmpptr++));
            DEV_PUT(*(tmpptr++));

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
            DEV_PUT(a);
            a = ~(chksum[0]);
            DEV_PUT(a);

            /* Send urgent pointer. */
            for(x = 0; x < 2; x++)
            {
                DEV_PUT(*(tmpptr++));
            }

            /* Send the rest of the packet. */


            for(x = 0; x < tmpstateptr->length; x++)
            {
                DEV_PUT(*(tmpptr++));
            }


            DEV_DONE();

            inflight++;
            tmpstateptr = tmpstateptr->next;
        }
        while(inflight < cwnd && tmpstateptr != NULL &&
                tmpstateptr->flag != WAIT);


    }




    //
    //
    //
    void miniweb_timer(void)
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

                DPRINTF("Retransmitting\n");
                tcpip_output();
                nrtx++;

                if(nrtx == 8)
                {
                    miniweb_init();
                }
            }

            else if(timer - txtime > 8 &&
                    stateptr->flag == WAIT)
            {
                DPRINTF("Connection dropped\n");
                stateptr = (tcpip_header*)NULL;
                miniweb_init();
            }
        }
    }




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
    struct tcpip_header*  stateptr;

    uint8_t               chksum[2];
    unsigned short        len;
    uint8_t*              tmpptr;

    struct tcpip_header*  tmpstateptr;
    uint8_t               cwnd;
    uint8_t               tcpstate;
    uint8_t               inflight;

    /* These actually only need four bits each. */
    uint8_t               timer;
    uint8_t               txtime;
    uint8_t               nrtx;

    /* Only two bits of state needed here. */
    uint8_t               chksumflags;


    /* This is just a declaration, and does not use RAM. */
    struct tcpip_header** pages;



};



#endif /* __MINIWEB_H__ */





