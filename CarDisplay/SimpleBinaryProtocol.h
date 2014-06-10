



#ifndef __SIMPLEBINARYPROTOCOL_H__
#define __SIMPLEBINARYPROTOCOL_H__







template <typename TransportType, typename DisplayType, typename RxQueueType, typename TxQueueType, typename pair>
class SimpleBinaryProtocol
{
    typedef typename pair::handlerType HandlerType;

public:

    SimpleBinaryProtocol(TransportType& _transport, DisplayType& _display, RxQueueType& _rxQ, TxQueueType& _txQ, HandlerType& _handler) :
            transport(_transport),
            display(_display),
            rxQ(_rxQ),
            txQ(_txQ),
            handler(_handler),
            position(0)
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
    //
    //
    void ProcessDataByte(uint8_t byte)
    {

        switch(position)
        {
            default:
                break;
        }

        position++;
    }

    //
    // 0 1 2 3 27 4 5 6 7 = D0 D1 D2 D3 C4 D5 D6 D7
    // 0 1 2 3 27 27 4 5 6 7 = D0 D1 D2 D3 D27 D4 D5 D6 D7
    //
    void ProcessRawByte(uint8_t byte)
    {
        const uint8_t   escapeByte      = 27;
        static uint8_t  previousByte    = 0;
        static uint8_t  message[16];

        if(previousByte == escapeByte)
        {
            if(byte == escapeByte)
            {
                message[position%sizeof(message)]   = byte;
                position++;
            }
            else
            {
                switch(byte)
                {
                    case 0:
                        //
                        // End of frame.
                        //
                        position    = 0;
                        break;

                    case 255:
                        //
                        // End of frame.
                        //
                        handler.ProcessMessage(message);
                        break;

                    default:
                        break;
                }
            }
        }
        else if(byte != escapeByte) 
        {
            message[position%sizeof(message)]   = byte;
            position++;
        }

        previousByte    = byte;
    }




private:

    TransportType&  transport;
    DisplayType&    display;
    RxQueueType&    rxQ;
    TxQueueType&    txQ;
    HandlerType&    handler;
    uint8_t         position;
};




#endif


