









#ifndef __UART_H__
#define __UART_H__

#include "Platform.h"
#include "wiring_private.h"




// Define constants and variables for buffering incoming serial data.  We're
// using a ring buffer (I think), in which head is the index of the location
// to which to write the next incoming character and tail is the index of the
// location from which to read.
#define SERIAL_BUFFER_SIZE 64

struct ringbuffer
{
  unsigned char buffer[SERIAL_BUFFER_SIZE];
  volatile unsigned int head;
  volatile unsigned int tail;
};









template <  uint32_t baud, 
            typename rxQueueType,
            typename txQueueType,
            uint8_t _ubrrh, uint8_t _ubrrl,
            uint8_t _ucsra, uint8_t _ucsrb,
            uint8_t _ucsrc, uint8_t _udr,
            uint8_t rxen,  uint8_t txen, 
            uint8_t rxcie, uint8_t udrie, 
            uint8_t u2x
        >
class UART
{
  public:

    UART(ringbuffer* _rxbuffer, ringbuffer* _txbuffer, rxQueueType& _rxQueue, txQueueType& _txQueue) :
            rxbuffer(_rxbuffer),
            txbuffer(_txbuffer),
            ubrrh((uint8_t*)_ubrrh),
            ubrrl((uint8_t*)_ubrrl),
            ucsra((uint8_t*)_ucsra),
            ucsrb((uint8_t*)_ucsrb),
            ucsrc((uint8_t*)_ucsrc),
            udr((uint8_t*)_udr),
            transmitting(false),
            rxQueue(_rxQueue),
            txQueue(_txQueue)
    {
        uint16_t baudsetting;
        bool useu2x = true;

        //
        // hardcoded exception for compatibility with the bootloader shipped
        // with the Duemilanove and previous boards and the firmware on the 8U2
        // on the Uno and Mega 2560.
        //
        if (baud == 57600) 
        {
            useu2x = false;
        }

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

        transmitting = false;

        sbi(*ucsrb, rxen);
        sbi(*ucsrb, txen);
        sbi(*ucsrb, rxcie);
        cbi(*ucsrb, udrie);

    }


    //
    //
    //
    int available(void)
    {
      return (unsigned int)(SERIAL_BUFFER_SIZE + rxbuffer->head - rxbuffer->tail) % SERIAL_BUFFER_SIZE;
    }


    //
    //
    //
    int read(void)
    {
        //
        // if the head isn't ahead of the tail, we don't have any characters
        //
        if (rxbuffer->head == rxbuffer->tail) 
        {
            return -1;
        } 
        else 
        {
            unsigned char c = rxbuffer->buffer[rxbuffer->tail];
            rxbuffer->tail = (unsigned int)(rxbuffer->tail + 1) % SERIAL_BUFFER_SIZE;
            return c;
        }
    }


    //
    //
    //
    size_t write(uint8_t c)
    {
        int     i = (txbuffer->head + 1) % SERIAL_BUFFER_SIZE;

        //
        // If the output buffer is full, there's nothing for it other than to 
        // wait for the interrupt handler to empty it a bit
        // ???: return 0 here instead?
        //
        while (i == txbuffer->tail);

        txbuffer->buffer[txbuffer->head] = c;
        txbuffer->head = i;

        sbi(*ucsrb, udrie);
        // clear the TXC bit -- "can be cleared by writing a one to its bit location"
        transmitting = true;
        sbi(*ucsra, TXC0);

        return 1;
    }


    void store_char(unsigned char c, ringbuffer *buffer)
    {
      int i = (unsigned int)(buffer->head + 1) % SERIAL_BUFFER_SIZE;

      // if we should be storing the received character into the location
      // just before the tail (meaning that the head would advance to the
      // current location of the tail), we're about to overflow the buffer
      // and so we don't write the character or advance the head.
      if (i != buffer->tail) {
        buffer->buffer[buffer->head] = c;
        buffer->head = i;
      }
    }


    void RxISR()
    {
        if (bit_is_clear(UCSR0A, UPE0)) {
          unsigned char c = UDR0;
          store_char(c, rxbuffer);
        } else {
          unsigned char c = UDR0;
        };    
    }


    void TxISR()
    {
      if (txbuffer->head == txbuffer->tail) 
      {
        // Buffer empty, so disable interrupts
      cbi(UCSR0B, UDRIE0);
      }
      else {
        // There is more data in the output buffer. Send the next byte
        unsigned char c = txbuffer->buffer[txbuffer->tail];
        txbuffer->tail = (txbuffer->tail + 1) % SERIAL_BUFFER_SIZE;
        
        UDR0 = c;
      }    
    }

private:

    //
    //
    //
    ringbuffer*     rxbuffer;
    ringbuffer*     txbuffer;
    bool            transmitting;
    uint8_t*        ubrrh;
    uint8_t*        ubrrl;
    uint8_t*        ucsra;
    uint8_t*        ucsrb;
    uint8_t*        ucsrc;
    uint8_t*        udr;
    rxQueueType&    rxQueue;
    txQueueType&    txQueue;

};




#endif
