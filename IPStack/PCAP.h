

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




#define PCAP_VERSION_MAJOR 2
#define PCAP_VERSION_MINOR 4

#define PCAP_ERRBUF_SIZE 256




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

        //
        //
        //
        int r   = read(fd, &fileHeader, sizeof(fileHeader));
        printf("r: %d\n", r);
        printf("magic %04d\n", fileHeader.magic);
        printf("major %04d\n", fileHeader.version_major);
        printf("minor %04d\n", fileHeader.version_minor);
#if 0
        uint32_t magic;
        u_short version_major;
        u_short version_minor;
        int32_t thiszone; /* gmt to local correction */
        uint32_t sigfigs;  /* accuracy of timestamps */
        uint32_t snaplen;  /* max length saved portion of each pkt */
        uint32_t linktype; /* data link type (LINKTYPE_*) */
#endif
    }



    void Iterate()
    {
        pcap_pkthdr     packetHeader;

        if(0)
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
            int r = read(fd, &packetHeader, sizeof(packetHeader));
            if( r != -1 )
            {
                bytes_left = read(fd, inbuf, packetHeader.caplen);
                if(bytes_left == -1)
                {   
                    perror("PCAP: dev_get: read\n");
                }
                else
                {
                    //
                    // Packet received, send it up the stack.
                    //
                    printf("<got %d bytes from pcap>\n",packetHeader.len);
                    printf("<");
                    internetLayer.NewPacket();
                    for(int i=14; i<packetHeader.len; i++)
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

    /*
     * Describe at http://wiki.wireshark.org/Development/LibpcapFileFormat
     * 
     * The first record in the file contains saved values for some
     * of the flags used in the printout phases of tcpdump.
     * Many fields here are 32 bit ints so compilers won't insert unwanted
     * padding; these files need to be interchangeable across architectures.
     *
     * Do not change the layout of this structure, in any way (this includes
     * changes that only affect the length of fields in this structure).
     *
     * Also, do not change the interpretation of any of the members of this
     * structure, in any way (this includes using values other than
     * LINKTYPE_ values, as defined in "savefile.c", in the "linktype"
     * field).
     *
     * Instead:
     *
     *  introduce a new structure for the new format, if the layout
     *  of the structure changed;
     *
     *  send mail to "tcpdump-workers@tcpdump.org", requesting a new
     *  magic number for your new capture file format, and, when
     *  you get the new magic number, put it in "savefile.c";
     *
     *  use that magic number for save files with the changed file
     *  header;
     *
     *  make the code in "savefile.c" capable of reading files with
     *  the old file header as well as files with the new file header
     *  (using the magic number to determine the header format).
     *
     * Then supply the changes to "patches@tcpdump.org", so that future
     * versions of libpcap and programs that use it (such as tcpdump) will
     * be able to read your new capture file format.
     */
    struct pcap_file_header 
    {
        uint32_t    magic;
        uint16_t    version_major;
        uint16_t    version_minor;
        int32_t     thiszone;       /* gmt to local correction */
        uint32_t    sigfigs;        /* accuracy of timestamps */
        uint32_t    snaplen;        /* max length saved portion of each pkt */
        uint32_t    linktype;       /* data link type (LINKTYPE_*) */
    };

    /*
     * Each packet in the dump file is prepended with this generic header.
     * This gets around the problem of different headers for different
     * packet interfaces.
     */
    struct pcap_pkthdr 
    {
        struct timeval ts;  /* time stamp */
        uint32_t caplen; /* length of portion present */
        uint32_t len;  /* length this packet (off wire) */
    };


    InternetLayerType&  internetLayer;

    struct pcap_file_header fileHeader;

    int             dropFlag;
    int             fd;
    int             bytes_left;
    uint8_t         inbuf[2048];
    uint8_t         outbuf[2048];
    int             inptr;
    int             outptr;
};




#endif


