#include <cstdlib>
#include <cassert>
#include <iostream>
#include "RF24.h"
#include "nRF24L01.h"
#include "gpio_sun7i.h"

using namespace std;

void PrintUsage( const char* szName )
{
	printf("Usage: %s <-d device_path>\n", szName );
}

int main(int argc, char** argv)
{
	char* szDeviceName = 0;
	bool bTransmitter = false;

	if (argc < 3)
	{
		printf("Not enough arguments\n");
		PrintUsage( argv[0] );
		return 1;
	}

	char opt = 0;
	while(opt != -1)
	{
		opt = getopt( argc, argv, "d:" );
		if ( -1 == opt || 255 == opt ) break;
		switch(opt)
		{
			case 'd':
				szDeviceName = optarg;
				break;
			case '?':
			default:
				printf("Unknown adgument %c[%d]\n", opt,opt );
				PrintUsage( argv[0] );
				return 1;
				break;
		}
	}
	assert( szDeviceName != 0 );

	// CE - PI14
	// CSN - PI15
//	RF24 radio(SUNXI_GPI(14), SUNXI_GPI(15), szDeviceName);
//	setup(radio);
//	radio.stopListening();

//UEXT1
//PB20 	C8 		I/O 	TWI2_SCK - CE
//PB14 	A10 	I/O 	SPI2_CS0 - CSN

//UEXT2
//PB18 	A8 		I/O 	TWI1_SCK - CE
//PI16 	E17 	I/O 	SPI1_CS0 - CSN

	SPI testSPI(szDeviceName, 1000000, 8 );
	GPIO testGPIO;

	int CE = 0, CSN = 0;

	//lame but works for now
	//NOTE: At the time of testing the spidev2 is linked to the device on UEXT1
	if ( strstr( szDeviceName, "spidev2" ) )
	{
		CE = UEXT1_CE;
		CSN = UEXT1_CSN;
	}
	else
	{
		CE = UEXT2_CE;
		CSN = UEXT2_CSN;
	}

	testGPIO.SetVal(CE, LOW);
	testGPIO.SetVal(CSN, HIGH);

	uint8_t reg = 0;
	SPIIOBuf testBuf( 2 );
	std::string s2bReg( 2, 0 );
	std::string s6bReg( 6, 0 );
	
	reg = CONFIG;
	std::cout << "=============================================" << std::endl;
	std::cout << "Requesting CONFIG register " << (unsigned int)reg << std::endl;
	s2bReg[0] = ( R_REGISTER | (REGISTER_MASK & reg) );
	testGPIO.SetVal(CSN, LOW);
	testSPI.transfer( testBuf.SetTXData( s2bReg ) );
	testGPIO.SetVal(CSN, HIGH);
	testBuf.Print();
	ConfigReg testConfReg(testBuf.GetRXData()[1]);
	testConfReg.Print( std::cout );

	reg = CONFIG;
	std::cout << "=============================================" << std::endl;
	std::cout << "Writing CONFIG register " << (unsigned int)reg << std::endl;
	s2bReg[0] = ( W_REGISTER | (REGISTER_MASK & reg) );
	s2bReg[1] = 0x03;
	testGPIO.SetVal(CSN, LOW);
	testSPI.transfer( testBuf.SetTXData( s2bReg ) );
	testGPIO.SetVal(CSN, HIGH);
	testBuf.Print();

	//reg = CONFIG;
	//std::cout << "=============================================" << std::endl;
	//std::cout << "Requesting CONFIG register 6B " << (unsigned int)reg << std::endl;
	//s6bReg[0] = ( R_REGISTER | (REGISTER_MASK & reg) );
	//testGPIO.SetVal(CSN, LOW);
	//testSPI.transfer( testBuf.SetTXData( s6bReg ) );
	//testGPIO.SetVal(CSN, HIGH);
	//testBuf.Print();
	//ConfigReg testConfReg2(testBuf.GetRXData()[1]);
	//testConfReg2.Print( std::cout );

	reg = CONFIG;
	std::cout << "=============================================" << std::endl;
	std::cout << "Requesting CONFIG register " << (unsigned int)reg << std::endl;
	s2bReg[0] = ( R_REGISTER | (REGISTER_MASK & reg) );
	testGPIO.SetVal(CSN, LOW);
	testSPI.transfer( testBuf.SetTXData( s2bReg ) );
	testGPIO.SetVal(CSN, HIGH);
	testBuf.Print();
	ConfigReg testConfReg3(testBuf.GetRXData()[1]);
	testConfReg3.Print( std::cout );

	reg = STATUS;
	std::cout << "=============================================" << std::endl;
	std::cout << "Requesting STATUS register " << (unsigned int)reg << std::endl;
	s2bReg[0] = ( R_REGISTER | (REGISTER_MASK & reg) );
	testGPIO.SetVal(CSN, LOW);
	testSPI.transfer( testBuf.SetTXData( s2bReg ) );
	testGPIO.SetVal(CSN, HIGH);
	testBuf.Print();
	StatusReg testStatusReg(testBuf.GetRXData()[1]);
	testStatusReg.Print( std::cout );

	reg = SETUP_AW;
	std::cout << "=============================================" << std::endl;
	std::cout << "Requesting SETUP_AW register " << (unsigned int)reg << std::endl;
	s2bReg[0] = ( R_REGISTER | (REGISTER_MASK & reg) );
	testGPIO.SetVal(CSN, LOW);
	testSPI.transfer( testBuf.SetTXData( s2bReg ) );
	testGPIO.SetVal(CSN, HIGH);
	testBuf.Print();

	return 0;
}
