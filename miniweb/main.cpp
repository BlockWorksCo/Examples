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

#include "miniweb.h"
#include "tun_dev.h"
#include "pages.h"


struct IPStackType
{
	typedef MiniWebServer<IPStackType> 		WebServerType;
	typedef TUNPacketInterface<IPStackType> PacketInterfaceType;
	typedef PacketGenerator<IPStackType> 	PacketGeneratorType;
};



extern struct tcpip_header* 			pages[];
extern TUNPacketInterface<IPStackType> 	tunPacketInterface;

MiniWebServer<IPStackType> 				webServer(pages, tunPacketInterface);
TUNPacketInterface<IPStackType> 		tunPacketInterface(webServer);

//
//
//
int main(int argc, char **argv)
{
	webServer.miniweb_main_loop();

	return 0;
}

