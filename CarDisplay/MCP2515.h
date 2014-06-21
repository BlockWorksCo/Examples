

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

	}

private:

	SPIType& 		spi;
	ChipSelectType 	chipSelect;

};



#endif


