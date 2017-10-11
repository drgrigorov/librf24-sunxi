#include <iostream>
#include <iomanip>

#include "gpio_sun7i.h"

void DumpGPIOBank( GPIO::sunxi_gpio* gBank )
{
	std::ios_base::fmtflags bkp = std::cout.flags();
	for (int i=0; i < 16; i++)
	{
		std::cout <<
			"cfg[" << std::setw(2) << std::setfill('0') << i << "] = " <<
			std::hex << std::setw(2) << std::setfill('0')
			<< (unsigned int)gBank->cfg[i] << " " << std::dec;
		if (  (i + 1 )%8 == 0 ) { std::cout << std::endl; }
	}
	
	std::cout << "dat: " << std::hex << std::setw(8) << std::setfill('0') << gBank->dat << std::endl;
	std::cout << "dat_dec: ";
	for (int i=31; i >= 0; i--)
	{
		std::cout << ((gBank->dat >> i) & 0x01 ) << " ";
	}
	std::cout << std::endl;

	for (int i=0; i < 2; i++)
	{
		std::cout << "drv[" << i << "] = " << std::hex << std::setw(8) << std::setfill('0') <<
			gBank->drv[i] << std::endl;
		std::cout << "drv_dec[" << i << "]:  ";
		for (int n=15; n >= 0; n--)
		{
			std::cout << std::setw(2) << std::setfill('0') << ((gBank->drv[i] >> (n*2)) & 0x03 ) << " ";
		}
		std::cout << std::endl;
	}
	for (int i=0; i < 2; i++)
	{
		std::cout << "pull[" << i << "] = " << std::hex << std::setw(8) << std::setfill('0') <<
			gBank->pull[i] << std::endl;
		std::cout << "pull_dec[" << i << "]:  ";
		for (int n=15; n >= 0; n--)
		{
			std::cout << std::setw(2) << std::setfill('0') << ((gBank->pull[i] >> (n*2)) & 0x03 ) << " ";
		}
		std::cout << std::endl;
	}
	std::cout.flags( bkp );
}


int main()
{
	GPIO test;
	
	if ( test.GetErr() != 0 )
	{
		std::cout << "Error during GPIO class initialization" << std::endl;
		return 1;
	}

	for ( unsigned i = 0; i < 9; i++ )
	{
		std::cout << "-------------------------------------------" << std::endl;
		std::cout << "Printing bank: [" << i << "]" << std::endl;
		GPIO::sunxi_gpio* testStr = test.GetBank( i*32 );
		DumpGPIOBank( testStr );
	}
	return 0;
}
