

#ifndef __MCP2515_H__
#define __MCP2515_H__

#include "Platform.h"



//
//
//
template <typename SPIType, typename ChipSelectType>
class MCP2515
{

public:

    MCP2515(SPIType& _spi, ChipSelectType& _chipSelect) :
        spi(_spi),
        chipSelect(_chipSelect)
    {
        //
        //
        //
    }

    //
    // init can                  
    //
    uint8_t begin(uint8_t speedset)
    {
        return 0;
    }                              


    void SetCANCTRL_Mode(const uint8_t newmode)
    {
        uint8_t i;

        ModifyRegister(MCP_CANCTRL, MODE_MASK, newmode);

        i = ReadRegister(MCP_CANCTRL);
        i &= MODE_MASK;

        if ( i != newmode ) 
        {
            PANIC(1);
        }
    }


    //
    // init Masks                 
    //
    uint8_t init_Mask(uint8_t num, uint8_t ext, uint8_t ulData)
    {
        return 0;

    }           

    //
    // init filters               
    //
    uint8_t init_Filt(uint8_t num, uint8_t ext, uint32_t ulData)
    {

        return 0;
    }           

    //
    // send buf                    
    //
    uint8_t sendMsgBuf(uint32_t id, uint8_t ext, uint8_t len, uint8_t *buf)
    {

        return 0;
    }  

    //
    // read buf                    
    //
    uint8_t readMsgBuf(uint8_t *len, uint8_t *buf)
    {

        return 0;
    }                       

    //
    // if something received       
    //
    uint8_t checkReceive(void)
    {

        return 0;
    }                                       

    //
    // if something error       
    //
    uint8_t checkError(void)
    {

        return 0;
    }                                         

    //
    // get can id when receive
    //
    uint32_t getCanId(void)
    {

        return 0;
    }                                          

private:

    //
    //
    //
    void SetRegister(const uint8_t address, const uint8_t value)
    {
        chipSelect.Set();
        spi.readwrite(MCP_WRITE);
        spi.readwrite(address);
        spi.readwrite(value);
        chipSelect.Clear();
    }


    //
    //
    //
    void SetRegisters(const uint8_t address, const uint8_t values[], const uint8_t n)
    {
        uint8_t i;

        chipSelect.Set();
        spi.readwrite(MCP_WRITE);
        spi.readwrite(address);
           
        for (i=0; i<n; i++) 
        {
            spi.readwrite(values[i]);
        }
        chipSelect.Clear();
    }

    //
    //
    //
    uint8_t readRegister(const uint8_t address)                                                                     
    {
        uint8_t ret;

        chipSelect.Set();
        spi.readwrite(MCP_READ);
        spi.readwrite(address);
        ret = spi.read();
        chipSelect.Clear();

        return ret;
    }   

    //
    //
    //
    void readRegisters(const uint8_t address, uint8_t values[], const uint8_t n)
    {
        uint8_t i;

        chipSelect.Set();
        spi.readwrite(MCP_READ);
        spi.readwrite(address);

        //      
        // mcp2515 has auto-increment of address-pointer
        //
        for (i=0; i<n; i++) 
        {
            values[i] = spi.read();
        }
        chipSelect.Clear();
    }   


    //
    //
    //
    void ModifyRegister(const uint8_t address, const uint8_t mask, const uint8_t data)
    {
        chipSelect.Set();
        spi.readwrite(MCP_BITMOD);
        spi.readwrite(address);
        spi.readwrite(mask);
        spi.readwrite(data);
        chipSelect.Clear();
    }

    //
    //
    //
    uint8_t ReadStatus(void)                             
    {
        uint8_t i;

        chipSelect.Set();
        spi.readwrite(MCP_READ_STATUS);
        i = spi.read();
        chipSelect.Clear();
        
        return i;
    }


    //
    //
    //
    void Reset(void)                                      
    {
        chipSelect.Set();
        spi.readwrite(MCP_RESET);
        chipSelect.Clear();
        delay(10);
    }


    //
    //
    //                                                                                                                                                                                                                ,vvvvvvvvvvvvvvlg,mm   
    typedef enum 
    {
        TXRTSCTRL   = 0x0d,

        TXB0CTRL    = 0x30,
        TXB1CTRL    = 0x40,
        TXB2CTRL    = 0x50,

        TXB0SIDH    = 0x31,
        TXB1SIDH    = 0x41,
        TXB2SIDH    = 0x51,

        TXB0SIDL    = 0x32,
        TXB1SIDL    = 0x42,
        TXB2SIDL    = 0x52,

        TXB0EID8    = 0x33,
        TXB1EID8    = 0x43,
        TXB2EID8    = 0x53,

        TXB0EID0    = 0x34,
        TXB1EID0    = 0x44,
        TXB2EID0    = 0x54,

        TXB0DLC     = 0x35,
        TXB1DLC     = 0x45,
        TXB2DLC     = 0x55,

        TXB0D0      = 0x36,
        TXB1D0      = 0x46,
        TXB2D0      = 0x56,

    } RegisterAddress;



    typedef enum 
    {
        MCP_WRITE           = 0x02,
        MCP_READ            = 0x03,
        MCP_BITMOD          = 0x05,
        MCP_LOAD_TX0        = 0x40,
        MCP_LOAD_TX1        = 0x42,
        MCP_LOAD_TX2        = 0x44,
        MCP_RTS_TX0         = 0x81,
        MCP_RTS_TX1         = 0x82,
        MCP_RTS_TX2         = 0x84,
        MCP_RTS_ALL         = 0x87,
        MCP_READ_RX0        = 0x90,
        MCP_READ_RX1        = 0x94,
        MCP_READ_STATUS     = 0xA0,
        MCP_RX_STATUS       = 0xB0,
        MCP_RESET           = 0xC0,

    } Instruction;


    //
    // CAN control values.
    //
    typedef enum
    {
        MODE_NORMAL     = 0x00,
        MODE_SLEEP      = 0x20,
        MODE_LOOPBACK   = 0x40,
        MODE_LISTENONLY = 0x60,
        MODE_CONFIG     = 0x80,
        MODE_POWERUP    = 0xE0,
        MODE_MASK       = 0xE0,
        ABORT_TX        = 0x10,
        MODE_ONESHOT    = 0x08,
        CLKOUT_ENABLE   = 0x04,
        CLKOUT_DISABLE  = 0x00,
        CLKOUT_PS1      = 0x00,
        CLKOUT_PS2      = 0x01,
        CLKOUT_PS4      = 0x02,
        CLKOUT_PS8      = 0x03,
    };


    //
    //
    //
    typedef enum 
    {
        MCP_RXF0SIDH    = 0x00,
        MCP_RXF0SIDL    = 0x01,
        MCP_RXF0EID8    = 0x02,
        MCP_RXF0EID0    = 0x03,
        MCP_RXF1SIDH    = 0x04,
        MCP_RXF1SIDL    = 0x05,
        MCP_RXF1EID8    = 0x06,
        MCP_RXF1EID0    = 0x07,
        MCP_RXF2SIDH    = 0x08,
        MCP_RXF2SIDL    = 0x09,
        MCP_RXF2EID8    = 0x0A,
        MCP_RXF2EID0    = 0x0B,
        MCP_CANSTAT     = 0x0E,
        MCP_CANCTRL     = 0x0F,
        MCP_RXF3SIDH    = 0x10,
        MCP_RXF3SIDL    = 0x11,
        MCP_RXF3EID8    = 0x12,
        MCP_RXF3EID0    = 0x13,
        MCP_RXF4SIDH    = 0x14,
        MCP_RXF4SIDL    = 0x15,
        MCP_RXF4EID8    = 0x16,
        MCP_RXF4EID0    = 0x17,
        MCP_RXF5SIDH    = 0x18,
        MCP_RXF5SIDL    = 0x19,
        MCP_RXF5EID8    = 0x1A,
        MCP_RXF5EID0    = 0x1B,
        MCP_TEC         = 0x1C,
        MCP_REC         = 0x1D,
        MCP_RXM0SIDH    = 0x20,
        MCP_RXM0SIDL    = 0x21,
        MCP_RXM0EID8    = 0x22,
        MCP_RXM0EID0    = 0x23,
        MCP_RXM1SIDH    = 0x24,
        MCP_RXM1SIDL    = 0x25,
        MCP_RXM1EID8    = 0x26,
        MCP_RXM1EID0    = 0x27,
        MCP_CNF3        = 0x28,
        MCP_CNF2        = 0x29,
        MCP_CNF1        = 0x2A,
        MCP_CANINTE     = 0x2B,
        MCP_CANINTF     = 0x2C,
        MCP_EFLG        = 0x2D,
        MCP_TXB0CTRL    = 0x30,
        MCP_TXB1CTRL    = 0x40,
        MCP_TXB2CTRL    = 0x50,
        MCP_RXB0CTRL    = 0x60,
        MCP_RXB0SIDH    = 0x61,
        MCP_RXB1CTRL    = 0x70,
        MCP_RXB1SIDH    = 0x71,
        MCP_TX_INT      = 0x1C,                                    // Enable all transmit interrup ts
        MCP_TX01_INT    = 0x0C,                                    // Enable TXB0 and TXB1 interru pts
        MCP_RX_INT      = 0x03,                                    // Enable receive interrupts
        MCP_NO_INT      = 0x00,                                    // Disable all interrupts
        MCP_TX01_MASK   = 0x14,
        MCP_TX_MASK     = 0x54,

    } Registers;    

    SPIType&        spi;
    ChipSelectType  chipSelect;

};



#endif


