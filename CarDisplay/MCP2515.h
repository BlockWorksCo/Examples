

#ifndef __MCP2515_H__
#define __MCP2515_H__



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
    int8_t begin(int8_t speedset)
    {
        return 0;
    }                              

    //
    // init Masks                 
    //
    int8_t init_Mask(int8_t num, int8_t ext, int32_t ulData)
    {
        return 0;

    }           

    //
    // init filters               
    //
    int8_t init_Filt(int8_t num, int8_t ext, int32_t ulData)
    {

        return 0;
    }           

    //
    // send buf                    
    //
    int8_t sendMsgBuf(int32_t id, int8_t ext, int8_t len, int8_t *buf)
    {

        return 0;
    }  

    //
    // read buf                    
    //
    int8_t readMsgBuf(int8_t *len, int8_t *buf)
    {

        return 0;
    }                       

    //
    // if something received       
    //
    int8_t checkReceive(void)
    {

        return 0;
    }                                       

    //
    // if something error       
    //
    int8_t checkError(void)
    {

        return 0;
    }                                         

    //
    // get can id when receive
    //
    int32_t getCanId(void)
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
    int8_t readRegister(const uint8_t address)                                                                     
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



    SPIType&        spi;
    ChipSelectType  chipSelect;

};



#endif


