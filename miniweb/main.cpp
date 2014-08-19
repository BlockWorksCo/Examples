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

#include "AppConfiguration.h"

#define NUMBER_OF_ELEMENTS(a)       ( sizeof(a)/sizeof(a[0]) )


extern IPStackType::WebServerType::tcpip_header*    pages[];

IPStackType::PacketGeneratorType                    packetGenerator(pages);
extern IPStackType::PacketInterfaceType             packetInterface;

MiniWebServer<IPStackType>                          webServer(packetInterface, packetGenerator);
TUNPacketInterface<IPStackType>                     packetInterface(webServer);

typedef OffsetHash<uint8_t, uint16_t, 80>           PortToPageIndexHashType;
typedef Map<IPStackType::PacketGeneratorType, uint16_t, PortToPageIndexHashType>    PortToPageMapType;


IPStackType::PacketGeneratorType                    packetGenerator1(pages);
IPStackType::PacketGeneratorType                    packetGenerator2(pages);
IPStackType::PacketGeneratorType                    packetGenerator3(pages);
IPStackType::PacketGeneratorType                    packetGenerator4(pages);
IPStackType::PacketGeneratorType*                   generatorList[]     =
{
    &packetGenerator1,
    &packetGenerator2,
    &packetGenerator3,
    &packetGenerator4,
};

PortToPageIndexHashType                             portToPageIndexHash;
PortToPageMapType                                   portToPageMap(generatorList, portToPageIndexHash);



//
//
//
int main(int argc, char **argv)
{
    for(int i=80; i<84; i++)
    {
        printf("%d\n", portToPageMap.Lookup(i).isPortAccepted(80) );
    }



    while(true)
    {
        webServer.Iterate();    
    }

    return 0;
}

