

#ifndef __MCP2515_H__
#define __MCP2515_H__




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

private:

	//
	//
	//
	typedef enum 
	{
		TXRTSCTRL 	= 0x0d,

		TXB0CTRL 	= 0x30,
		TXB1CTRL 	= 0x40,
		TXB2CTRL 	= 0x50,

		TXB0SIDH 	= 0x31,
		TXB1SIDH 	= 0x41,
		TXB2SIDH 	= 0x51,

		TXB0SIDL 	= 0x32,
		TXB1SIDL 	= 0x42,
		TXB2SIDL 	= 0x52,

		TXB0EID8	= 0x33,
		TXB1EID8	= 0x43,
		TXB2EID8	= 0x53,

		TXB0EID0	= 0x34,
		TXB1EID0	= 0x44,
		TXB2EID0	= 0x54,

		TXB0DLC		= 0x35,
		TXB1DLC		= 0x45,
		TXB2DLC		= 0x55,

		TXB0D0 		= 0x36,
		TXB1D0 		= 0x46,
		TXB2D0 		= 0x56,

	} RegisterAddress;


	SPIType& 		spi;
	ChipSelectType 	chipSelect;

};



#endif


