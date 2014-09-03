

#ifndef __PCAP_H__
#define __PCAP_H__


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






//
//
//
template < typename StackType >
class PCAP
{
    //
    // Break out the StackType helper types.
    //
    STACK_TYPES_BREAKOUT;


public:

    PCAP(InternetLayerType& _internetLayer, const char* inputFileName) :
        internetLayer(_internetLayer),
        dropFlag(0)
    {

        char tun_name[IFNAMSIZ];

        strcpy(tun_name, "tun0");
        fd = open(inputFileName, O_RDWR);
        if(fd == -1)
        {
            perror("PCAP: dev_init: open");
            exit(1);
        }
        else
        {
            printf("PCAP device handle: %d", fd);
        }

    }



    void Iterate()
    {
        fd_set          fdset;
        struct timeval  tv;
        int             ret;

        tv.tv_sec       = 0;
        tv.tv_usec      = 500000;
        FD_ZERO(&fdset);
        FD_SET(fd, &fdset);

        printf("Waiting for data (%d)...\n",fd);
        ret = select(fd + 1, &fdset, NULL, NULL, &tv);
        if(ret == 0)
        {
            //
            // Timeout.
            //

            //
            // Send any packets that may have been produced.
            //
            PullFromStackAndSend();

            internetLayer.Idle();
        }
        else
        {
            //
            // Data available.
            //

            bytes_left = read(fd, inbuf, sizeof(inbuf));
            if(bytes_left == -1)
            {   
                perror("PCAP: dev_get: read\n");
            }
            else
            {
                //
                // Packet received, send it up the stack.
                //
                printf("<got %d bytes from tun>\n",bytes_left);
                printf("<");
                internetLayer.NewPacket();
                for(int i=0; i<bytes_left; i++)
                {
                    if(internetLayer.State() != Rejected)
                    {
                        internetLayer.PushInto( inbuf[i] );                        
                    }

                    printf("%02x ", inbuf[i]);                
                }
                printf(">\n");

                //
                // Send any packets that may have been produced.
                //
                PullFromStackAndSend();
            }

        }
    }


    //
    //
    //
    void PullFromStackAndSend()
    {
        uint16_t    i               = 0;
        bool        dataAvailable   = false;

        //
        // While there are still packets in the stack...
        //
        do
        {
            //
            // Form the new packet.
            //
            i   = 0;
            do
            {
                outbuf[i]   = internetLayer.PullFrom( dataAvailable, i );
                if(dataAvailable == true)
                {
                    i++;
                }

            } while(dataAvailable == true);

            //
            // Send the new packet.
            //
            if(i>0)
            {
                size_t  bytesWritten    = write(fd, outbuf, i);
                if(bytesWritten != i)
                {
                    printf("(PCAP) not all bytes written!\n");
                }
            }

        } while(i > 0);

    }


private:

    InternetLayerType&  internetLayer;

    int             dropFlag;
    int             fd;
    int             bytes_left;
    char            inbuf[2048];
    char            outbuf[2048];
    int             inptr;
    int             outptr;
};




#endif


