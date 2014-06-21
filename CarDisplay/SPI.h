



#ifndef __AVRSPI_H__
#define __AVRSPI_H__



template <uint8_t portNum, typename RxQueueType, typename TxQueueType>
class AVRSPI
{

public:

    AVRSPI(RxQueueType& _rxQ, TxQueueType& _txQ) :
        txQ(_txQ),
        rxQ(_rxQ)
    {
        //
        // Set MOSI , SCK , and SS output
        //
        DDRB |= (1<<PB4) | (1<<PB5) | (1<<PB7);    

        //
        // Enable SPI, Master, set clock rate fck/128 
        //
        SPCR |= ((1<<SPE) | (1<<MSTR) | (1<<SPR1) | (1<<SPR0)); 
    }


    //
    //
    //
    void Process()
    {   
    }


    //
    //
    //
    uint8_t TransferByte(uint8_t out)
    {
        //
        // put the byteword into data register 
        //
        SPDR = out; 

        //
        // Wait for transmission complete 
        //
        while(!(SPSR & (1<<SPIF))); 

        //
        // read the data out.
        //
        uint8_t     rxByte    = SPDR;   
        return rxByte;
    }

    void TransferCompleteISR()
    {
        //SPDR = data;
        //return SPDR;        
    }

private:


#if 0
    void Writetoreg( int byteword)
    {
        SPDR = byteword; // put the byteword into data register 
        while(!(SPSR & (1<<SPIF)));  // Wait for transmission complete 
        byteword=SPDR;   //just for clearing spif
    }


    unsigned int Read(char reglength)
    {
        int b;
        for(b=0;b<reglength;b++)
        {
            SPDR = 0xFF;
            while(!(SPSR &(1<<SPIF))); // Wait for transmission complete
        
            if(b==0)
            {
                k=SPDR*256;
            }
            else
                k=k+SPDR;
        }

        return k;
    }


    unsigned char received_from_spi(unsigned char data)
    {
    }


    void send_message()
    {
        if (SPCR & (1<<MSTR)) 
        { // if we are still in master mode
            SELECT_OTHER; // tell other device to flash LED twice
            send_spi(FLASH_LED_COMMAND); send_spi(0x02); send_spi(0x00);
            DESELECT_OTHER;
        }
        setup_spi(SPI_MODE_1, SPI_MSB, SPI_INTERRUPT, SPI_SLAVE);
    }
#endif


private:

    RxQueueType&    rxQ;
    TxQueueType&    txQ;

};


#endif



