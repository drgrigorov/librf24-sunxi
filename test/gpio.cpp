
#include <iostream>
#include <iomanip>
#include <bitset>
#include <unistd.h>

#include <Console.hpp>
#include "gpio_sun7i.h"

namespace cr = CppReadline;
using ret = cr::Console::ReturnCode;
	
unsigned int gpioList[] =
{
	UEXT1_CE,
	UEXT1_CSN,
	UEXT2_CE,
	UEXT2_CSN,
	SUNXI_GPB(14),
	SUNXI_GPB(15),
	SUNXI_GPB(16),
	SUNXI_GPB(17)
};

const char* gpioNameList[] =
{
	"UEXT1_CE",
	"UEXT1_CSN",
	"UEXT2_CE",
	"UEXT2_CSN",
	"UEXT1_SPI2_CS0",
	"UEXT1_SPI2_CLK",
	"UEXT1_SPI2_MOSI",
	"UEXT1_SPI2_MISO"
};

//UEXT1_1  - 3.3V
//UEXT1_2  - GND
//UEXT1_3  - UART6-TX - PA12
//UEXT1_4  - UART6-RX - PA13
//UEXT1_5  - TWI2-SCK - PB20
//UEXT1_6  - TWI2-SDA - PB21
//UEXT1_7  - SPI2-MISO - PB17 .
//UEXT1_8  - SPI2-MOSI - PB16 .
//UEXT1_9  - SPI2-CLK - PB15
//UEXT1_10 - SPI2-CS0 - PB14
//
//UEXT2_1  - 3.3V
//UEXT2_2  - GND
//UEXT2_3  - UART7-TX - PI20
//UEXT2_4  - UART7-RX - PI21
//UEXT2_5  - TWI1-SCK - PB18
//UEXT2_6  - TWI1-SDA - PB19
//UEXT2_7  - SPI1-MISO - PI19
//UEXT2_8  - SPI1-MOSI - PI18
//UEXT2_9  - SPI1-CLK - PI17
//UEXT2_10 - SPI1-CS0 - PI16

class GPIOHandler
{
public:
	//Note: keeping a pointer to the object is stupid but for the test it will do
	//Later autoprt should be used for the correct handling of the GPIO manager
	GPIOHandler( GPIO* GPIOMgr ) : m_GPIOMgr( GPIOMgr ) {}

	int operator ()(const std::vector<std::string> &input)
	{
		if (input.size() <= 1)
		{
			std::cout << "gpio command accepts following subCommands:" << std::endl;
			std::cout << "list" << std::endl;
			std::cout << "cget" << std::endl;
			std::cout << "cset" << std::endl;
			std::cout << "set" << std::endl;
			std::cout << "get" << std::endl;
			return ret::Ok;
		}

		const std::string& subCmd = input[1];
		if ( subCmd == "list" )
		{
			std::cout << "List the available GPIO" << std::endl;
			for (unsigned i=0; i< sizeof(gpioList)/sizeof(gpioList[0]); i++)
			{
				std::cout << gpioList[i] << " => " << gpioNameList[i] << std::endl;
			}
		}
		else if ( subCmd == "cget" )
		{
			if (input.size() <= 2)
			{
				std::cout << "gpio cget subcommand requires a parameter" << std::endl;
				return 1;
			}
			unsigned long pinNum = std::stoul(input[2]);
			std::cout << "Pin " << pinNum << " : " <<
				std::bitset<4>((unsigned int)m_GPIOMgr->GetCfgpin( pinNum ))
				<< std::dec << std::endl;
		}
		else if ( subCmd == "cset" )
		{
			if (input.size() <= 3)
			{
				std::cout << "gpio cset subcommand requires parameters <gpionum> <value>" << std::endl;
				return 1;
			}
			unsigned long pinNum = std::stoul(input[2]);
			unsigned long val = std::stoul(input[3]);
			int ret = m_GPIOMgr->SetCfgpin( pinNum, val & 0x07 );
			if ( ret == 0 )
			{
				std::cout << "Setting not changed" << std::endl;
			}
		}
		else if ( subCmd == "get" )
		{
			if (input.size() <= 2)
			{
				std::cout << "gpio get subcommand requires a parameter" << std::endl;
				return 1;
			}
			unsigned long pinNum = std::stoul(input[2]);
			std::cout << "Pin " << pinNum << " : " << std::boolalpha <<
				m_GPIOMgr->GetVal( pinNum ) << std::endl;
		}
		else if ( subCmd == "set" )
		{
			if (input.size() <= 3)
			{
				std::cout << "gpio cset subcommand requires parameters <gpionum> <value>" << std::endl;
				return 1;
			}
			unsigned long pinNum = std::stoul(input[2]);
			unsigned long val = std::stoul(input[3]);
			m_GPIOMgr->SetCfgpin( pinNum, val & 0x01 );
		}
		return ret::Ok;
	}
private:
	GPIO* m_GPIOMgr;
};

int main()
{
	GPIO test;
	GPIOHandler Mgr( &test );
	
	if ( test.GetErr() != 0 )
	{
		std::cout << "Error during GPIO class initialization" << std::endl;
		return 1;
	}

    cr::Console c(">");

    c.registerCommand("gpio", Mgr );
    c.executeCommand("help");

    int retCode;
    do {
        retCode = c.readLine();
        // We can also change the prompt based on last return value:
        if ( retCode == ret::Ok )
            c.setGreeting(">");
        else
		{
            c.setGreeting("!>");
            std::cout << "Received error code "<< retCode <<"\n";
		}
    }
    while ( retCode != ret::Quit );

	return 0;
}
