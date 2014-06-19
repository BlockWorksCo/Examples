



#ifndef __SIMPLEBINARYPROTOCOL_H__
#define __SIMPLEBINARYPROTOCOL_H__







template <typename TransportType, typename DisplayType, typename RxQueueType, typename TxQueueType, typename pair>
class SimpleBinaryProtocol
{
    typedef typename pair::handlerType  HandlerType;
    typedef typename pair::MessageType  MessageType;


public:

    SimpleBinaryProtocol(TransportType& _transport, DisplayType& _display, RxQueueType& _rxQ, TxQueueType& _txQ, HandlerType& _handler) :
            transport(_transport),
            display(_display),
            rxQ(_rxQ),
            txQ(_txQ),
            handler(_handler)
    {
    }


    //
    //
    //
    void Process()
    {
        bool dataAvailableFlag  = false;
        uint8_t ch = rxQ.Get(dataAvailableFlag);
        if(dataAvailableFlag == true)
        {            
            //DebugOut(ch);
            ProcessRawByte(ch);
        }
    }



    //
    // 0 1 2 3 27 4 5 6 7 = D0 D1 D2 D3 C4 D5 D6 D7
    // 0 1 2 3 27 27 4 5 6 7 = D0 D1 D2 D3 D27 D4 D5 D6 D7
    //
    void ProcessRawByte(uint8_t byte)
    {
        const uint8_t   escapeByte      = 27;
        static uint8_t  previousByte    = 0;
        static uint8_t  message[64];
        static uint8_t  position        = 0;

        if(previousByte == escapeByte)
        {
            //
            // Escaped Control character.
            //
            switch(byte)
            {
                case 27:
                    //
                    // Escaped escape-byte case.
                    //
                    message[position%sizeof(message)]   = 27;
                    position = (position + 1) % (sizeof(message));
                    byte    = 0; // So we don't stay in 'escape' mode by assigning this to the previous value. 
                    break;

                case 0:
                    //
                    // End of frame.
                    //
                    position    = 0;
                    break;

                case 255:
                {
                    //
                    // End of frame.
                    //
                    uint8_t     length          = message[0];
                    uint8_t     checksum        = message[1];
                    if( (length == position) && 
                        (checksum == 0) )
                    {
                        MessageType&    msg = *((MessageType*)&message[2]);
                        handler.ProcessMessage( msg, position-2);                            
                    }
                    position    = 0;
                    break;
                }

                default:
                    break;
            }
        }
        else if(byte != escapeByte) 
        {
            //
            // Normal, non-escaped case.
            //
            message[position%sizeof(message)]   = byte;
            position = (position + 1) % (sizeof(message));
        }

        previousByte    = byte;
    }




private:

    TransportType&  transport;
    DisplayType&    display;
    RxQueueType&    rxQ;
    TxQueueType&    txQ;
    HandlerType&    handler;
};




#endif


