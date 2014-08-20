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


//
// Break out the CombinationTypes for this class.
//
typedef typename StackType::ApplicationLayerType    ApplicationLayerType;
typedef typename StackType::TCPTransportLayerType   TCPTransportLayerType;
typedef typename StackType::UDPTransportLayerType   UDPTransportLayerType;
typedef typename StackType::InternetLayerType       InternetLayerType;
typedef typename StackType::LinkLayerType           LinkLayerType;


ApplicationLayerType    tcpApplicationLayer;
TCPTransportLayerType   tcpLayer(tcpApplicationLayer);
UDPTransportLayerType   udpLayer;
InternetLayerType       internetLayer(tcpLayer, udpLayer);
LinkLayerType           linkLayer(internetLayer);



//
//
//
int main(int argc, char **argv)
{
    printf("\nBLOCK::WORKS IPStack Demo\n");
    while(true)
    {
        linkLayer.Iterate();    
    }

    return 0;
}

