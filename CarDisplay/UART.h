









#ifndef __UART_H__
#define __UART_H__

#include "Platform.h"
#include "wiring_private.h"








template <  uint32_t baud, 
            typename rxQueueType,
            typename txQueueType,
            uint8_t _ubrrh, uint8_t _ubrrl,
            uint8_t _ucsra, uint8_t _ucsrb,
            uint8_t _ucsrc, uint8_t _udr,
            uint8_t rxen,  uint8_t txen, 
            uint8_t rxcie, uint8_t udrie, 
            uint8_t u2x,
            typename EventEngineType,
            typename ComboType
        >
class UART
{
    typedef typename ComboType::ByteReceivedEventType    ByteReceivedEventType;

  public:

    UART(rxQueueType& _rxQueue, txQueueType& _txQueue, EventEngineType& _eventEngine, ByteReceivedEventType& _byteReceivedEvent) :
            ubrrh((uint8_t*)_ubrrh),
            ubrrl((uint8_t*)_ubrrl),
            ucsra((uint8_t*)_ucsra),
            ucsrb((uint8_t*)_ucsrb),
            ucsrc((uint8_t*)_ucsrc),
            udr((uint8_t*)_udr),
            rxQueue(_rxQueue),
            txQueue(_txQueue),
            eventEngine(_eventEngine),
            byteReceivedEvent(_byteReceivedEvent),
            eventDroppedFlag(false)
    {
    }


    void start()
    {
        uint16_t baudsetting;
        bool useu2x = true;

        //
        // hardcoded exception for compatibility with the bootloader shipped
        // with the Duemilanove and previous boards and the firmware on the 8U2
        // on the Uno and Mega 2560.
        //
        tryagain:

        if (useu2x) 
        {
            *ucsra = 1 << u2x;
            baudsetting = (F_CPU / 4 / baud - 1) / 2;
        } 
        else 
        {
            *ucsra = 0;
            baudsetting = (F_CPU / 8 / baud - 1) / 2;
        }

        if ((baudsetting > 4095) && useu2x)
        {
            useu2x = false;
            goto tryagain;
        }

        //
        // assign the baudsetting, a.k.a. ubbr (USART Baud Rate Register)
        //
        *ubrrh  = baudsetting >> 8;
        *ubrrl  = baudsetting;

        sbi(*ucsrb, rxen);
        sbi(*ucsrb, txen);
        sbi(*ucsrb, rxcie);
        cbi(*ucsrb, udrie);

        UCSR0C  = 3 << UCSZ00;

        sei();
    }


    //
    //
    //
    void Process()
    {
        sbi(*ucsrb, udrie);
        sbi(*ucsra, TXC0);    
    }


    //
    //
    //
    void RxISR()
    {
        bool    elementDroppedFlag  = false;

        if (bit_is_set(UCSR0A, UPE0)) 
        {
            rxQueue.Put('@', elementDroppedFlag);        
        }
        else if (bit_is_set(UCSR0A, FE0)) 
        {
            rxQueue.Put('#', elementDroppedFlag);        
        }
        else if (bit_is_set(UCSR0A, DOR0)) 
        {
            rxQueue.Put('*', elementDroppedFlag);        
        }
        else
        {
            uint8_t     c = UDR0;
            rxQueue.Put(c, elementDroppedFlag);                    
        } 

        eventEngine.Put(byteReceivedEvent, eventDroppedFlag);
    }


    //
    //
    //
    void TxISR()
    {
        if(txQueue.IsEmpty() == true)
        {
            // Buffer empty, so disable interrupts
            cbi(UCSR0B, UDRIE0);
        }
        else
        {
            bool        dataAvailableFlag   = false;
            uint8_t     ch  = txQueue.Get(dataAvailableFlag);
            UDR0 = ch;
        }
    }

private:

    //
    //
    //
    uint8_t*        ubrrh;
    uint8_t*        ubrrl;
    uint8_t*        ucsra;
    uint8_t*        ucsrb;
    uint8_t*        ucsrc;
    uint8_t*        udr;
    rxQueueType&    rxQueue;
    txQueueType&    txQueue;

    EventEngineType&        eventEngine;
    ByteReceivedEventType&  byteReceivedEvent;
    bool                    eventDroppedFlag;
};




#endif
