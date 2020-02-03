
#include <iostream>
#include <iomanip>
#include <bitset>
#include <unistd.h>

#include <cstdint>

#include <Console.hpp>
#include "gpio_sun7i.h"

namespace cr = CppReadline;
using ret = cr::Console::ReturnCode;
	
uint16_t gpioList[] =
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

class PinID
{
public:
	class PinIDEx
	{
		public:
			PinIDEx( const std::string& sErrMessage )
			:
				m_sErrmessage( sErrMessage )
			{
			}
			std::string what() const throw() { return m_sErrmessage; }

		private:
			std::string m_sErrmessage;
	};
public:
	PinID( uint16_t nPin )
	{
		if (nPin >= 9*32)
		{
			throw ( PinIDEx( "Pin out of range" ) );
		}
		uint8_t nBank = nPin/32;
		uint8_t nSmPin = nPin%32;
		m_sPin = "P";
		m_sPin.append(1, 'A' + nBank);
		m_sPin.append( std::to_string( nSmPin ) );
		m_nPin = nPin;
	}

	PinID( const std::string& sInput )
	{
		if (sInput.size() < 3)
		{
			throw( PinIDEx( "Pin string too short" ) );
		}

		if (sInput[0] != 'P')
		{
			throw( PinIDEx( "Pin string prefix wrong" ) );
		}

		char Bank = sInput[1];
		if (Bank < 'A' || Bank > 'I')
		{
			throw( PinIDEx( "Pin string bank letter out of range" ) );
		}
		Bank = Bank - 'A';

		uint16_t pinNum = std::stoul(sInput.substr(2));
		if (pinNum > 31)
		{
			throw( PinIDEx( "Pin string number out of range" ) );
		}
		m_nPin = Bank*32 + pinNum;
		m_sPin = sInput;
	}

	static PinID FromString( const std::string& sStr )
	{
		//Check if string is representing integer
		char* test;
		uint16_t nNum = std::strtoul( sStr.c_str(), &test, 10 );
		if ( *test == 0 )
		{
			//all characters of the string are part of number representation
			return PinID( nNum );
		}
		else
		{
			return PinID( sStr );
		}
	}

	uint16_t GetInt() const throw() { return m_nPin; }
	std::string GetStr() const throw() { return m_sPin; }
	operator uint16_t () const throw() { return GetInt(); }
	operator std::string () const throw() { return GetStr(); }

private:
	uint16_t m_nPin;
	std::string m_sPin;
};

std::ostream& operator<<( std::ostream& Out, const PinID& Pin )
{
	Out << Pin.GetStr() << "(" << Pin.GetInt() << ")";
	return Out;
}

class GPIOHandler
{
public:
	//Note: keeping a pointer to the object is stupid but for the test it will do
	//Later autoprt should be used for the correct handling of the GPIO manager
	GPIOHandler( GPIO* GPIOMgr ) : m_GPIOMgr( GPIOMgr ) {}

	int8_t operator ()(const std::vector<std::string> &input)
	{
		if (input.size() <= 1)
		{
			std::cout << "gpio command accepts following subCommands:" << std::endl;
			std::cout << "list" << std::endl;
			std::cout << "cget" << std::endl;
			std::cout << "cset" << std::endl;
			std::cout << "set" << std::endl;
			std::cout << "get" << std::endl;
			std::cout << "pget" << std::endl;
			std::cout << "lget" << std::endl;
			std::cout << "trans" << std::endl;
			return ret::Ok;
		}

		const std::string& subCmd = input[1];
		if ( subCmd == "trans" )
		{
			if (input.size() <= 2)
			{
				std::cout << "gpio trans subcommand requires a parameter" << std::endl;
				return 1;
			}

			for ( auto i = 2; i < input.size(); i++ )
			{
				try
				{
					PinID Pin = PinID::FromString( input[i] );
					std::cout << Pin << std::endl;
				}
				catch( PinID::PinIDEx& e )
				{
					std::cout << "Failed to parse pin ID[" << input[i] << "]: " << e.what() << std::endl;
					return 1;
				}
			}
		}
		else if ( subCmd == "list" )
		{
			std::cout << "List the available GPIO" << std::endl;
			for (uint8_t i=0; i< sizeof(gpioList)/sizeof(gpioList[0]); i++)
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
			for ( auto i = 2; i < input.size(); i++ )
			{
				try
				{
					PinID Pin = PinID::FromString( input[i] );
					std::cout << "Pin " << Pin << " : " <<
						std::bitset<4>((unsigned int)m_GPIOMgr->GetCfgpin( Pin ))
						<< std::dec << std::endl;
				}
				catch( PinID::PinIDEx& e )
				{
					std::cout << "Failed to parse pin ID[" << input[i] << "]: " << e.what() << std::endl;
					return 1;
				}
			}
		}
		else if ( subCmd == "cset" )
		{
			if (input.size() <= 3)
			{
				std::cout << "gpio cset subcommand requires parameters <gpionum> <value>" << std::endl;
				return 1;
			}
			try
			{
				PinID Pin = PinID::FromString( input[2] );
				uint16_t val = std::stoul(input[3]);
				if ( 0 == m_GPIOMgr->SetCfgpin( Pin, val & 0x07 ) )
				{
					std::cout << "Setting not changed" << std::endl;
				}
				else
				{
					std::cout << Pin << " value set to: " <<
						std::bitset<4>((unsigned int)m_GPIOMgr->GetCfgpin( Pin )) << std::endl;
				}
			}
			catch( PinID::PinIDEx& e )
			{
				std::cout << "Failed to parse pin ID[" << input[2] << "]: " << e.what() << std::endl;
				return 1;
			}
		}
		else if ( subCmd == "get" )
		{
			if (input.size() <= 2)
			{
				std::cout << "gpio get subcommand requires a parameter" << std::endl;
				return 1;
			}
			uint16_t pinNum = std::stoul(input[2]);
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
			uint16_t pinNum = std::stoul(input[2]);
			uint16_t val = std::stoul(input[3]);
			m_GPIOMgr->SetCfgpin( pinNum, val & 0x01 );
		}
		else if ( subCmd == "pget" )
		{
			if (input.size() <= 2)
			{
				std::cout << "gpio pget subcommand requires a parameter" << std::endl;
				return 1;
			}
			for ( auto i = 2; i < input.size(); i++ )
			{
				try
				{
					PinID Pin = PinID::FromString( input[i] );
					std::cout << "Pin " << Pin << " : " <<
						std::bitset<2>((unsigned int)m_GPIOMgr->GetPullUp( Pin ))
						<< std::dec << std::endl;
				}
				catch( PinID::PinIDEx& e )
				{
					std::cout << "Failed to parse pin ID[" << input[i] << "]: " << e.what() << std::endl;
					return 1;
				}
			}
		}
		else if ( subCmd == "lget" )
		{
			if (input.size() <= 2)
			{
				std::cout << "gpio lget subcommand requires a parameter" << std::endl;
				return 1;
			}
			for ( auto i = 2; i < input.size(); i++ )
			{
				try
				{
					PinID Pin = PinID::FromString( input[i] );
					std::cout << "Pin " << Pin << " : " <<
						std::bitset<2>((unsigned int)m_GPIOMgr->GetLevel( Pin ))
						<< std::dec << std::endl;
				}
				catch( PinID::PinIDEx& e )
				{
					std::cout << "Failed to parse pin ID[" << input[i] << "]: " << e.what() << std::endl;
					return 1;
				}
			}
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

    int8_t retCode;
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
