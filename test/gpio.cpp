
#include <iostream>
#include <unistd.h>

#include <Console.hpp>
#include "gpio_sun7i.h"

namespace cr = CppReadline;
using ret = cr::Console::ReturnCode;
	
//{
//	SUNXI_GPB(14),
//	SUNXI_GPB(15),
//	SUNXI_GPB(16),
//	SUNXI_GPB(17),
//	SUNXI_GPB(20)
//};

//	"SUNXI_GPB(14)",
//	"SUNXI_GPB(15)",
//	"SUNXI_GPB(16)",
//	"SUNXI_GPB(17)",
//	"SUNXI_GPB(20)"
//};

unsigned int gpioList[] =
{
	UEXT1_CE,
	UEXT1_CSN,
	UEXT2_CE,
	UEXT2_CSN
};

const char* gpioNameList[] =
{
	"UEXT1_CE",
	"UEXT1_CSN",
	"UEXT2_CE",
	"UEXT2_CSN"
};


unsigned gpioStart(const std::vector<std::string> &input)
{
	if (input.size() <= 1)
	{
		std::cout << "gpio command accepts following subCommands:" << std::endl;
		std::cout << "list" << std::endl;
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
	else if ( subCmd == "show" )
	{
		if (input.size() <= 2)
		{
			std::cout << "gpio show subcommand requires a parameter" << std::endl;
		}
		double pinNum = std::stod(input[2]);

	}
    return ret::Ok;
}

int main()
{
	GPIO test;
	
	if ( test.GetErr() != 0 )
	{
		std::cout << "Error during GPIO class initialization" << std::endl;
		return 1;
	}

    cr::Console c(">");

    c.registerCommand("gpio", gpioStart );
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
