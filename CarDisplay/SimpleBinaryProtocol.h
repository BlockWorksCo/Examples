



#ifndef __SIMPLEBINARYPROTOCOL_H__
#define __SIMPLEBINARYPROTOCOL_H__







template <typename TransportType, typename DisplayType, typename RxQueueType, typename TxQueueType, typename pair>
class SimpleBinaryProtocol
{
    typedef typename pair::handlerType HandlerType;

public:

    SimpleBinaryProtocol(TransportType& _transport, DisplayType& _display, RxQueueType& _rxQ, TxQueueType& _txQ) :
            transport(_transport),
            position(0),
            display(_display),
            rxQ(_rxQ),
            txQ(_txQ)
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
        static uint8_t  xPos     = 0;
        static uint8_t  yPos     = 0;
        static uint8_t  spriteId = 0;

        switch(position)
        {
            case 0:
                xPos    = byte;
                break;

            case 1:
                yPos    = byte;
                break;

            case 2:
            {
                spriteId = byte;
                display.drawSprite(spriteId, xPos, yPos);

                break;            
            }

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

        if(previousByte == escapeByte)
        {
            if(byte == escapeByte)
            {
                ProcessDataByte(byte);
            }
            else
            {
                switch(byte)
                {
                    case 0:
                        position    = 0;
                        break;

                    case 1:
                        display.clear();
                        break;

                    case 2:
                        display.setIntensity(1);
                        break;

                    case 3:
                        display.setIntensity(15);
                        break;

                    case 4:
                        display.drawFrame();
                        break;

                    default:
                        break;
                }
            }
        }
        else if(byte != escapeByte) 
        {
            ProcessDataByte(byte);
        }

        previousByte    = byte;
    }




private:

    TransportType&  transport;
    DisplayType&    display;
    RxQueueType&    rxQ;
    TxQueueType&    txQ;

    uint8_t         position;



};




#endif


