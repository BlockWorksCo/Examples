/*
  SerialPort.h - Hardware serial library for Wiring
  Copyright (c) 2006 Nicholas Zambetti.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

  Modified 28 September 2010 by Mark Sproul
  Modified 14 August 2012 by Alarus
*/

#ifndef SerialPort_h
#define SerialPort_h

#include <inttypes.h>


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "Platform.h"
#include "wiring_private.h"
#include "SerialPort.h"

#include "Stream.h"




// Define config for Serial.begin(baud, config);
#define SERIAL_5N1 0x00
#define SERIAL_6N1 0x02
#define SERIAL_7N1 0x04
#define SERIAL_8N1 0x06
#define SERIAL_5N2 0x08
#define SERIAL_6N2 0x0A
#define SERIAL_7N2 0x0C
#define SERIAL_8N2 0x0E
#define SERIAL_5E1 0x20
#define SERIAL_6E1 0x22
#define SERIAL_7E1 0x24
#define SERIAL_8E1 0x26
#define SERIAL_5E2 0x28
#define SERIAL_6E2 0x2A
#define SERIAL_7E2 0x2C
#define SERIAL_8E2 0x2E
#define SERIAL_5O1 0x30
#define SERIAL_6O1 0x32
#define SERIAL_7O1 0x34
#define SERIAL_8O1 0x36
#define SERIAL_5O2 0x38
#define SERIAL_6O2 0x3A
#define SERIAL_7O2 0x3C
#define SERIAL_8O2 0x3E



// Define constants and variables for buffering incoming serial data.  We're
// using a ring buffer (I think), in which head is the index of the location
// to which to write the next incoming character and tail is the index of the
// location from which to read.
#define SERIAL_BUFFER_SIZE 64

struct ring_buffer
{
  unsigned char buffer[SERIAL_BUFFER_SIZE];
  volatile unsigned int head;
  volatile unsigned int tail;
};


template <uint32_t baud>
class SerialPort
{
  public:




    SerialPort()
    {
    }


    void attach(ring_buffer *rx_buffer, ring_buffer *tx_buffer,
                volatile uint8_t *ubrrh, volatile uint8_t *ubrrl,
                volatile uint8_t *ucsra, volatile uint8_t *ucsrb,
                volatile uint8_t *ucsrc, volatile uint8_t *udr,
                uint8_t rxen, uint8_t txen, uint8_t rxcie, uint8_t udrie, uint8_t u2x)
    {
        _rx_buffer = rx_buffer;
        _tx_buffer = tx_buffer;
        _ubrrh = ubrrh;
        _ubrrl = ubrrl;
        _ucsra = ucsra;
        _ucsrb = ucsrb;
        _ucsrc = ucsrc;
        _udr = udr;
        _rxen = rxen;
        _txen = txen;
        _rxcie = rxcie;
        _udrie = udrie;
        _u2x = u2x;  
    }

    


    void begin()
    {
      uint16_t baud_setting;
      bool use_u2x = true;

      // hardcoded exception for compatibility with the bootloader shipped
      // with the Duemilanove and previous boards and the firmware on the 8U2
      // on the Uno and Mega 2560.
      if (baud == 57600) 
      {
        use_u2x = false;
      }

    try_again:

      if (use_u2x) 
      {
        *_ucsra = 1 << _u2x;
        baud_setting = (F_CPU / 4 / baud - 1) / 2;
      } 
      else 
      {
        *_ucsra = 0;
        baud_setting = (F_CPU / 8 / baud - 1) / 2;
      }
      
      if ((baud_setting > 4095) && use_u2x)
      {
        use_u2x = false;
        goto try_again;
      }

      // assign the baud_setting, a.k.a. ubbr (USART Baud Rate Register)
      *_ubrrh = baud_setting >> 8;
      *_ubrrl = baud_setting;

      transmitting = false;

      sbi(*_ucsrb, _rxen);
      sbi(*_ucsrb, _txen);
      sbi(*_ucsrb, _rxcie);
      cbi(*_ucsrb, _udrie);


    }





    void begin(byte config)
    {
      uint16_t baud_setting;
      uint8_t current_config;
      bool use_u2x = true;

      // hardcoded exception for compatibility with the bootloader shipped
      // with the Duemilanove and previous boards and the firmware on the 8U2
      // on the Uno and Mega 2560.
      if (baud == 57600) {
        use_u2x = false;
      }

    try_again:
      
      if (use_u2x) {
        *_ucsra = 1 << _u2x;
        baud_setting = (F_CPU / 4 / baud - 1) / 2;
      } else {
        *_ucsra = 0;
        baud_setting = (F_CPU / 8 / baud - 1) / 2;
      }
      
      if ((baud_setting > 4095) && use_u2x)
      {
        use_u2x = false;
        goto try_again;
      }

      // assign the baud_setting, a.k.a. ubbr (USART Baud Rate Register)
      *_ubrrh = baud_setting >> 8;
      *_ubrrl = baud_setting;

      //set the data bits, parity, and stop bits
      *_ucsrc = config;
      
      sbi(*_ucsrb, _rxen);
      sbi(*_ucsrb, _txen);
      sbi(*_ucsrb, _rxcie);
      cbi(*_ucsrb, _udrie);
    }






    int available(void)
    {
      return (unsigned int)(SERIAL_BUFFER_SIZE + _rx_buffer->head - _rx_buffer->tail) % SERIAL_BUFFER_SIZE;
    }



    int read(void)
    {
      // if the head isn't ahead of the tail, we don't have any characters
      if (_rx_buffer->head == _rx_buffer->tail) {
        return -1;
      } else {
        unsigned char c = _rx_buffer->buffer[_rx_buffer->tail];
        _rx_buffer->tail = (unsigned int)(_rx_buffer->tail + 1) % SERIAL_BUFFER_SIZE;
        return c;
      }
    }


    size_t write(uint8_t c)
    {
      int i = (_tx_buffer->head + 1) % SERIAL_BUFFER_SIZE;
      
      // If the output buffer is full, there's nothing for it other than to 
      // wait for the interrupt handler to empty it a bit
      // ???: return 0 here instead?
      while (i == _tx_buffer->tail)
        ;
      
      _tx_buffer->buffer[_tx_buffer->head] = c;
      _tx_buffer->head = i;
      
      sbi(*_ucsrb, _udrie);
      // clear the TXC bit -- "can be cleared by writing a one to its bit location"
      transmitting = true;
      sbi(*_ucsra, TXC0);
      
      return 1;
    }



private:
    ring_buffer *_rx_buffer;
    ring_buffer *_tx_buffer;
    volatile uint8_t *_ubrrh;
    volatile uint8_t *_ubrrl;
    volatile uint8_t *_ucsra;
    volatile uint8_t *_ucsrb;
    volatile uint8_t *_ucsrc;
    volatile uint8_t *_udr;
    uint8_t _rxen;
    uint8_t _txen;
    uint8_t _rxcie;
    uint8_t _udrie;
    uint8_t _u2x;
    bool transmitting;


};




#endif
