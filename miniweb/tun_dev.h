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

#ifndef __TUN_DEV_H__
#define __TUN_DEV_H__


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

#include <linux/if_tun.h>
#include <netinet/in.h>

#include <linux/if.h>
#include <linux/if_tun.h>





//
//
//
template < typename IPStackType >
class TUNPacketInterface
{
    //
    // Break out the CombinationTypes for this class.
    //
    typedef typename IPStackType::WebServerType             WebServerType;
    typedef typename IPStackType::PacketInterfaceType       PacketInterfaceType;
    typedef typename IPStackType::PacketGeneratorType       PacketGeneratorType;
    typedef typename IPStackType::PortType                  PortType;
    typedef typename IPStackType::PortToPageIndexHashType   PortToPageIndexHashType;
    typedef typename IPStackType::PortToPageMapType         PortToPageMapType;



public:

    TUNPacketInterface(WebServerType& _webServer) :
        webServer(_webServer),
        dropFlag(0)
    {

        char tun_name[IFNAMSIZ];

        strcpy(tun_name, "tun0");
        fd = tun_alloc(tun_name, IFF_TUN);  /* tun interface */
        if(fd == -1)
        {
            perror("tun_dev: dev_init: open");
            exit(1);
        }


        //
        // sudo ip tuntap add dev tun0 mode tun
        // sudo ifconfig tun0 192.168.4.1 up
        //
        int r = system("ifconfig tun0 inet 192.168.0.2 192.168.0.1");

        r = system("route add -net 192.168.0.0 netmask 255.255.255.0 dev tun0");
        r++;

        bytes_left = 0;
        outptr = 0;
    }


    int tun_alloc(char *dev, int flags) {

      struct ifreq ifr;
      int fd, err;
      const char *clonedev = "/dev/net/tun";

      /* Arguments taken by the function:
       *
       * char *dev: the name of an interface (or '\0'). MUST have enough
       *   space to hold the interface name if '\0' is passed
       * int flags: interface flags (eg, IFF_TUN etc.)
       */

       /* open the clone device */
       if( (fd = open(clonedev, O_RDWR)) < 0 ) 
       {
         printf("opened clonedev\n");
         return fd;
       }

       /* preparation of the struct ifr, of type "struct ifreq" */
       memset(&ifr, 0, sizeof(ifr));

       ifr.ifr_flags = flags;   /* IFF_TUN or IFF_TAP, plus maybe IFF_NO_PI */

       if (*dev) 
       {
         printf("name %s supplied\n",dev);
         /* if a device name was specified, put it in the structure; otherwise,
          * the kernel will try to allocate the "next" device of the
          * specified type */
         strncpy(ifr.ifr_name, dev, IFNAMSIZ);
       }

       /* try to create the device */
       if( (err = ioctl(fd, TUNSETIFF, (void *) &ifr)) < 0 ) 
       {
         printf("ioctl failed. \n");
         close(fd);
         fd = -1;
         return err;
       }
       else
       {
         printf("ioctl ok, fd=%d. \n",fd);        
       }

      /* if the operation was successful, write back the name of the
       * interface to the variable "dev", so the caller can know
       * it. Note that the caller MUST reserve space in *dev (see calling
       * code below) */
      strcpy(dev, ifr.ifr_name);
     printf("dev name = %s. \n", dev);

      /* this is the special file descriptor that the caller will use to talk
       * with the virtual interface */
      return fd;
    }

    //
    //
    //
    unsigned char wait(void)
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

        if(ret == 0)
        {
            webServer.timerTick();
            goto again;
        }

        bytes_left = read(fd, inbuf, sizeof(inbuf));

        if(bytes_left == -1)
        {
            perror("tun_dev: dev_get: read");
        }
        else
        {
            printf("<got %d bytes:>",bytes_left);
            printf("<");
            for(int i=0; i<bytes_left; i++)
            {
                printf("%02x ", inbuf[i]);                
            }
            printf("<");
        }

        /*    printf("tun_dev: dev_get: read %d bytes\n", bytes_left);*/
        inptr = 0;

        return get();
    }


    //
    //
    //
    unsigned char get(void)
    {
        if(bytes_left > 0)
        {
            bytes_left--;
            printf("0x%02x ", (unsigned char)inbuf[inptr]); fflush(NULL);
            return inbuf[inptr++];
        }

        return 0;
    }


    //
    //
    //
#if 0    
    void put(unsigned char byte)
    {
        /*  printf("0x%02x ", byte); */
        outbuf[outptr++] = byte;
    }
#endif

    //
    //
    //
    template <typename T> void put(T data)
    {
        uint8_t*    byte    = &data;

        for(uint8_t i=0; i<sizeof(T); i++)
        {
            /*  printf("0x%02x ", byte); */
            outbuf[outptr++] = *byte;
        }
    }


    //
    //
    //
    void drop(void)
    {
        /*  printf("\n");*/
        bytes_left = 0;
    }


    //
    //
    //
    void done(void)
    {
        int ret;

        /*  check_checksum(outbuf, outptr);*/

        /*  drop++;*/
        if(dropFlag % 9 == 1)
        {
            dropFlag = 0;
            /*    printf("Dropped a packet!\n");*/
            outptr = 0;
            return;
        }

        ret = write(fd, outbuf, outptr);

        if(ret == -1)
        {
            perror("tun_dev: dev_done: write");
            exit(1);
        }

        outptr = 0;
        /*  printf("\n");*/
    }


private:

    WebServerType&  webServer;

    int             dropFlag;
    int             fd;
    int             bytes_left;
    char            inbuf[2048];
    char            outbuf[2048];
    int             inptr;
    int             outptr;
};




#endif


