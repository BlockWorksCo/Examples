



#include "SerialPort.h"





ringbuffer rx_buffer  =  { { 0 }, 0, 0 };
ringbuffer tx_buffer  =  { { 0 }, 0, 0 };



inline void store_char(unsigned char c, ringbuffer *buffer)
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




SIGNAL(USART_RX_vect)
  {
    if (bit_is_clear(UCSR0A, UPE0)) {
      unsigned char c = UDR0;
      store_char(c, &rx_buffer);
    } else {
      unsigned char c = UDR0;
    };
  }





ISR(USART_UDRE_vect)
{
  if (tx_buffer.head == tx_buffer.tail) 
  {
	// Buffer empty, so disable interrupts
  cbi(UCSR0B, UDRIE0);
  }
  else {
    // There is more data in the output buffer. Send the next byte
    unsigned char c = tx_buffer.buffer[tx_buffer.tail];
    tx_buffer.tail = (tx_buffer.tail + 1) % SERIAL_BUFFER_SIZE;
	
    UDR0 = c;
  }
}





