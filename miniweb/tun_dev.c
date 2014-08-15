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

#include "dev.h"
#include "miniweb.h"

#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/uio.h>
#include <sys/socket.h>

#include <net/if_tun.h>

static int drop = 0;

static int fd;
static int bytes_left;
static char inbuf[2048], outbuf[2048];
static int inptr, outptr;

struct tcpip_hdr {
  unsigned char vhl,
    tos,          
    len[2],       
    id[2],        
    ipoffset[2],  
    ttl,          
    proto;     
  unsigned short ipchksum;
  unsigned long srcipaddr, 
    destipaddr;
  unsigned char srcport[2],
    destport[2],
    seqno[4],  
    ackno[4],
    tcpoffset,
    flags,
    wnd[2],     
    tcpchksum[2],
    urgp[2]; 
  unsigned char data[0];

};
/*-----------------------------------------------------------------------------------*/
static unsigned short
chksum(void *data, int len)
{
  unsigned short *sdata = data;
  unsigned short acc;
  unsigned int sum;
    
  for(acc = 0; len > 1; len -= 2) {
    acc += *sdata++;
  }

  /* add up any odd byte */
  if(len == 1) {
    acc += (unsigned short)(*(unsigned char *)sdata);
  }

  sum = (acc & 0xffff) + (acc >> 16);
  sum += (sum >> 16);
  
  return ~(sum & 0xffff);
}
/*-----------------------------------------------------------------------------------*/
static unsigned short
chksum_pseudo(void *data, int len, unsigned long ipaddr1,
              unsigned long ipaddr2,
              unsigned char proto,
              unsigned short protolen)
{
  unsigned int sum;
  unsigned short *sdata = data;
  
  for(sum = 0; len > 1; len -= 2) {
    sum += *sdata++;
  }

  /* add up any odd byte */
  if(len == 1) {
    sum += (unsigned short)(*(unsigned char *)sdata);
  }

  sum += (ipaddr1 & 0xffff);
  sum += ((ipaddr1 >> 16) & 0xffff);
  sum += (ipaddr2 & 0xffff);
  sum += ((ipaddr2 >> 16) & 0xffff);
  sum += (unsigned short)htons((unsigned short)proto);
  sum += (unsigned short)htons(protolen);
  
  while(sum >> 16) {
    sum = (sum & 0xffff) + (sum >> 16);
  }
  
  return ~sum & 0xffff;
}
/*-----------------------------------------------------------------------------------*/
static void
check_checksum(void *data, int len)
{
  struct tcpip_hdr *hdr;
  unsigned short sum;
  
  hdr = data;

  sum = chksum(data, 20);  
  printf("IP header checksum check 0x%x\n", sum);
  
  sum = chksum_pseudo(&(hdr->srcport[0]), len - 20,
                      hdr->srcipaddr, hdr->destipaddr,
                      hdr->proto, len - 20);  
  printf("TCP checksum check 0x%x len %d protolen %d\n", sum, len,
         len - 20);
  
}
/*-----------------------------------------------------------------------------------*/
void
dev_init(void)
{
  int val;
  fd = open("/dev/tun0", O_RDWR);
  if(fd == -1) {
    perror("tun_dev: dev_init: open");
    exit(1);
  }
#ifdef linux
  system("ifconfig tun0 inet 192.168.0.2 192.168.0.1");
  
  system("route add -net 192.168.0.0 netmask 255.255.255.0 dev tun0");
#else
  /*  system("ifconfig tun0 inet sidewalker rallarsnus");  */
  system("ifconfig tun0 inet 192.168.0.1 192.168.0.2");
#endif /* linux */

  val = 0;
  ioctl(fd, TUNSIFHEAD, &val);
  bytes_left = 0;
  outptr = 0;
}
/*-----------------------------------------------------------------------------------*/
unsigned char
dev_wait(void)
{
  fd_set fdset;
  struct timeval tv;
  int ret;

again:  
  tv.tv_sec = 0;
  tv.tv_usec = 500000;
  FD_ZERO(&fdset);
  FD_SET(fd, &fdset);
  
  ret = select(fd + 1, &fdset, NULL, NULL, &tv);
  if(ret == 0) {
    miniweb_timer();
    goto again;
  } 
  bytes_left = read(fd, inbuf, sizeof(inbuf));
  if(bytes_left == -1) {
    perror("tun_dev: dev_get: read");
  }
  /*    printf("tun_dev: dev_get: read %d bytes\n", bytes_left);*/
  inptr = 0;

  return dev_get();
}
/*-----------------------------------------------------------------------------------*/
unsigned char
dev_get(void)
{
  if(bytes_left > 0) {
    bytes_left--;
    /*    printf("0x%02x ", (unsigned char)inbuf[inptr]); fflush(NULL);*/
    return inbuf[inptr++];
  }
  return 0;
}
/*-----------------------------------------------------------------------------------*/
void
dev_put(unsigned char byte)
{
  /*  printf("0x%02x ", byte); */
  outbuf[outptr++] = byte;
}
/*-----------------------------------------------------------------------------------*/
void
dev_drop(void)
{
  /*  printf("\n");*/
  bytes_left = 0;
}
/*-----------------------------------------------------------------------------------*/
void
dev_done(void)
{
  int ret;
  
  /*  check_checksum(outbuf, outptr);*/

  /*  drop++;*/
  if(drop % 9 == 1) {
    drop = 0;
    /*    printf("Dropped a packet!\n");*/
    outptr = 0;
    return;
  }

  ret = write(fd, outbuf, outptr);
  if(ret == -1) {
    perror("tun_dev: dev_done: write");
    exit(1);
  }
  outptr = 0;
  /*  printf("\n");*/
}  
/*-----------------------------------------------------------------------------------*/
