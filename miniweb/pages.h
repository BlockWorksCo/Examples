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

#ifndef __PACKETGENERATOR_H__
#define __PACKETGENERATOR_H__





//
//
//
template < typename IPStackType >
class PacketGenerator
{
    //
    // Break out the CombinationTypes for this class.
    //
    typedef typename IPStackType::WebServerType         WebServerType;
    typedef typename IPStackType::PacketInterfaceType   PacketInterfaceType;
    typedef typename IPStackType::PacketGeneratorType   PacketGeneratorType;


public:

	//
	//
	//
	PacketGenerator( typename WebServerType::tcpip_header* _pages[] ) :
	        pages(_pages)
	{

	}


	//
	//
	//
	typename WebServerType::tcpip_header* packetForPort(uint8_t portNumber)
	{
		return pages[portNumber - PORTLOW];
	}

private:

    /* This is just a declaration, and does not use RAM. */
    typename WebServerType::tcpip_header**        pages;


};


#endif