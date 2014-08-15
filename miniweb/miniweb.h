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


#endif /* __MINIWEB_H__ */





