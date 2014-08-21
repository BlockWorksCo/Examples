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

#include "../miniweb.h"

#include <fcntl.h>
#include <unistd.h>
#include <sys/param.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>

static char *htmlpages[] = {"ports/80.html",
                            "ports/81.html"};

struct tcpip_header *pages[100];

struct tcpip_header reset;

unsigned char packets[6000000];

/* a + c << 8 = a + x + c */
#define ADC(a, c, x) do { \
                        unsigned short __tmp; \
			__tmp = a + (x) + c; \
			a = __tmp & 0xff; \
			c = __tmp >> 8; \
                     } while(0)

static FILE *cfile, *hfile;
/*-----------------------------------------------------------------------------------*/
static unsigned short
chksum(void *data, int len)
{
  unsigned short *sdata = data;
  unsigned int acc;
    
  for(acc = 0; len > 1; len -= 2) {
    acc += *sdata++;
  }

  /* add up any odd byte */
  if(len == 1) {
    acc += (unsigned short)(*(unsigned char *)sdata);
  }

  while(acc >> 16) {
    acc = (acc & 0xffff) + (acc >> 16);
  }
  
  return (acc & 0xffff);

}
/*-----------------------------------------------------------------------------------*/
static unsigned short
chksum_pseudo(void *data, int len, unsigned long ipaddr, unsigned char proto,
              unsigned short protolen)
{
  unsigned int sum;

  sum = chksum(data, len);
  sum += (ipaddr & 0xffff);
  sum += ((ipaddr >> 16) & 0xffff);
  sum += (unsigned short)htons((unsigned short)proto);
  sum += (unsigned short)htons(protolen);
  
  while(sum >> 16) {
    sum = (sum & 0xffff) + (sum >> 16);
  }
  
  return sum;
}
/*-----------------------------------------------------------------------------------*/
static void
build_packet(struct tcpip_header *p, unsigned short ipid,
             unsigned int seqno, unsigned short port, unsigned char flags,
             unsigned char *data,
             int datalen, unsigned char *options, int optlen,
             enum packetflags flag)
{
  unsigned short sum;
  int i, j;

  p->flag = flag;
  /* First, bulid the IP header. */
  p->length = datalen + optlen;  /* Four bytes for the MSS option. */
  p->vhl = 0x45;
  p->tos = 0x00;
  p->len[0] = 0x00;
  p->len[1] = 40 + datalen + optlen;
  p->id[0] = (ipid >> 8) & 0xff;
  p->id[1] = ipid & 0xff;
  p->ipoffset[0] = 0x00;
  p->ipoffset[1] = 0x00;
  p->ttl = 0x40;
  p->protocol = IP_PROTO_TCP;
  p->ipchksum[0] = 0x00;
  p->ipchksum[1] = 0x00;
  p->srcipaddr[0] = IPADDR0;
  p->srcipaddr[1] = IPADDR1;
  p->srcipaddr[2] = IPADDR2;
  p->srcipaddr[3] = IPADDR3;
  
  /* Calculate partial IP header checksum. */
  sum = chksum(&(p->vhl), 16);
  *(unsigned short *)&(p->ipchksum) = sum;
  
  /* Next, build the TCP header. */
  p->srcport[0] = port >> 8;
  p->srcport[1] = port & 0xff;
  p->seqno[0] = (seqno >> 24) & 0xff;
  p->seqno[1] = (seqno >> 16) & 0xff;
  p->seqno[2] = (seqno >>  8) & 0xff;
  p->seqno[3] = (seqno >>  0) & 0xff;
  p->tcpoffset = (5 + optlen / 4) << 4;
  p->flags = flags;
  p->wnd[0] = 0x10;
  p->wnd[1] = 0x00;
  p->tcpchksum[0] = 0x00;
  p->tcpchksum[1] = 0x00;
  p->urgp[0] = 0x00;
  p->urgp[1] = 0x00;

  /* Fill in options. */
  for(i = 0; i < optlen; i++) {
    p->data[i] = options[i];
  }

  /* Fill in data. */
  for(i = 0; i < datalen; i++) {
    p->data[i + optlen] = data[i];
  }
  
  /* Calculate partial TCP checksum. */
  sum = chksum_pseudo(&(p->srcport[0]), 14 + optlen + datalen,
                      IPADDR, IP_PROTO_TCP, 20 + optlen + datalen);
  *(unsigned short *)&(p->tcpchksum) = sum;

  
  
  fprintf(cfile, "static const unsigned char data_%d_%d[] = { ", port, ipid);
  j = 0;
  for(i = 0; i < 40 + datalen + optlen; i++) {
    fprintf(cfile, "0x%x, ", (unsigned char *)(&(p->vhl))[i]);
  }
  fprintf(cfile, "};\n");
}
/*-----------------------------------------------------------------------------------*/
static int
build_packets(char *fname, void *mem, unsigned short port)
{
  int fd;
  unsigned int seqno;
  unsigned short ipid;
  unsigned char options[4];
  unsigned char data[TCP_MSS];
  int datalen;
  enum packetflags flag;
  struct tcpip_header *p, *pnext;
 
  fd = open(fname, O_RDONLY);
  
  p = (struct tcpip_header *)mem;

  seqno = 6502;
  ipid = 0;
  
  /* Build first SYNACK packet. */
  /* Build a TCP MSS option */
  options[0] = 2;   /* option kind (MSS) */
  options[1] = 4;   /* option length */
  options[2] = TCP_MSS / 256;
  options[3] = TCP_MSS & 255;

  fprintf(cfile, "static const struct tcpip_header_ packet_%d_%d = { ",
	  port, ipid);
  fprintf(hfile, "static const struct tcpip_header_ packet_%d_%d;\n",
	  port, ipid);
  fprintf(cfile, "&packet_%d_%d, ", port, ipid + 1);
  fprintf(cfile, "NONE, ");
  fprintf(cfile, "%d}; ", 4);  
  build_packet(p, ipid, seqno, port, TCP_SYN|TCP_ACK, NULL, 0,
               options, 4, NONE);
  seqno++;
  ipid++;
  
  pnext = (struct tcpip_header *)(p->data + 4);
  p->next = pnext;
  p = pnext;
  
  flag = WAIT;
  /* Build data packets. */
  while((datalen = read(fd, data, TCP_MSS)) > 0) {
    /*    printf("building packet, length %d\n", datalen);*/
    fprintf(cfile, "static const struct tcpip_header_ packet_%d_%d = { ",
	    port, ipid);
    fprintf(hfile, "static const struct tcpip_header_ packet_%d_%d;\n",
	    port, ipid);
    fprintf(cfile, "&packet_%d_%d, ", port, ipid + 1);
    if(flag == NONE) {
      fprintf(cfile, "NONE, ");
    } else if(flag == WAIT) {
      fprintf(cfile, "WAIT, ");
    }
    fprintf(cfile, "%d}; ", datalen);
    build_packet(p, ipid, seqno, port, TCP_ACK, data, datalen,
                 NULL, 0, flag);
      
    seqno += datalen;
    ipid++;
    
    pnext = (struct tcpip_header *)(p->data + datalen);
    p->next = pnext;
    p = pnext;
    flag = NONE;
  }
  close(fd);
  
  /* Build FIN packet. */
  fprintf(cfile, "static const struct tcpip_header_ packet_%d_%d = { ",
	  port, ipid);
  fprintf(hfile, "static const struct tcpip_header_ packet_%d_%d;\n",
	  port, ipid);
  fprintf(cfile, "&packet_%d_%d, ", port, ipid + 1);
  fprintf(cfile, "NONE, ");
  fprintf(cfile, "%d}; ", 0);  
  build_packet(p, ipid, seqno, port, TCP_FIN|TCP_ACK, NULL, 0,
               NULL, 0, NONE);
  seqno++;
  ipid++;
  
  pnext = (struct tcpip_header *)(p->data);
  p->next = pnext;
  p = pnext;

  fprintf(cfile, "static const struct tcpip_header_ packet_%d_%d = { ",
	  port, ipid);
  fprintf(hfile, "static const struct tcpip_header_ packet_%d_%d;\n",
	  port, ipid);
  fprintf(cfile, "&packet_%d_%d, ", port, ipid);
  fprintf(cfile, "WAIT, ");
  fprintf(cfile, "%d}; ", 0);
  
  /* Build ACK of FIN packet. */
  build_packet(p, ipid, seqno, port, TCP_ACK, NULL, 0,
               NULL, 0, WAIT);
  
  p->next = NULL;
  p->next = p;
  return (int)p + sizeof(struct tcpip_header) - (int)mem;
}
/*-----------------------------------------------------------------------------------*/
int
main(int argc, char **argv)
{
  unsigned char *mem;
  int len;
  int i;
  unsigned char chksum[2], c;

  
  cfile = fopen("pages.c", "w");
  hfile = fopen("pages.h", "w");

  /*  fprintf(cfile, "#define NULL (void *)0\n");
  fprintf(cfile, "#include \"miniweb.h\"\n");
  fprintf(cfile, "struct tcpip_header_ {\n");
  fprintf(cfile, "\tconst struct tcpip_header_ *next;\n");
  fprintf(cfile, "\tconst enum packetflags flag;\n");
  fprintf(cfile, "\tconst unsigned char length;\n");
  fprintf(cfile, "};\n");
  fprintf(cfile, "#include \"pages.h\"\n");  
  */

  fprintf(cfile, "\t};\n");
  
  for(i = PORTHIGH - PORTLOW - 1; i >= 0; i--) {
    fprintf(cfile, "\t(struct tcpip_header *)&packet_%d_0,\n", i + PORTLOW);
  }
  fprintf(cfile, "const struct tcpip_header *pages[%d] = {\n", PORTHIGH - PORTLOW);  
  
  mem = packets;
  for(i = 0; i < PORTHIGH - PORTLOW; i++) {
    pages[i] = (struct tcpip_header *)mem;
    len = build_packets(htmlpages[i], mem, 80 + i);
    mem += len;
  }
  
  fprintf(cfile, "};\n");
  fprintf(cfile, "\tconst unsigned char length;\n");
  fprintf(cfile, "\tconst enum packetflags flag;\n");
  fprintf(cfile, "\tconst struct tcpip_header_ *next;\n");
  fprintf(cfile, "struct tcpip_header_ {\n");
  fprintf(cfile, "#include \"miniweb.h\"\n");
  fprintf(cfile, "#define NULL (void *)0\n");


  chksum[0] = chksum[1] = c = 0;
  ADC(chksum[0], c, IPADDR0);
  ADC(chksum[1], c, IPADDR1);
  ADC(chksum[0], c, IPADDR2);
  ADC(chksum[1], c, IPADDR3);
  ADC(chksum[0], c, 0);
  ADC(chksum[1], c, IP_PROTO_TCP);
  ADC(chksum[0], c, 0xff);
  ADC(chksum[1], c, 0xff - 20); 
  while(c) {
    ADC(chksum[0], c, 0);
    ADC(chksum[1], c, 0);
  }
  
  fprintf(cfile, "/* chksum[0] 0x%x chksum[1] 0x%x */\n", ~chksum[0] & 0xff,
	  ~chksum[1] & 0xff);
  return 0;
}





