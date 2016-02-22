#include <cstdlib>
#include <iostream>
#include "RF24.h"
#include "nRF24L01.h"
#include "gpio_sun7i.h"

using namespace std;
const uint64_t pipes[2] = {0xF0F0F0F0E1LL, 0xF0F0F0F0D2LL};

void setup(RF24& radio)
{
        radio.begin();
        // enable dynamic payloads
        radio.enableDynamicPayloads();
        // optionally, increase the delay between retries & # of retries
        radio.setRetries(15, 15);
        radio.setDataRate(RF24_250KBPS);
        // Open pipes to other nodes for communication
        // Open 'our' pipe for writing
        // Open the 'other' pipe for reading, in position #1 (we can have up to 5 pipes open for reading)
        radio.openWritingPipe(pipes[0]);
        radio.openReadingPipe(1, pipes[1]);
        // Start listening
        radio.startListening();
        // Dump the configuration of the rf unit for debugging
        //radio.printDetails();
}

void PrintUsage( const char* szName )
{
	printf("Usage: %s <-d device_path>\n", szName );
}

int main(int argc, char** argv)
{
	char* szDeviceName = 0;
	bool bTransmitter = false;

	if (argc < 2)
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

	//SPI(string spidev, int speed, int bits);
	SPI testSPI(szDeviceName, 1000000, 8 );
	GPIO testGPIO;

	int CE = 0, CSN = 0;

	//lame but works for now
	if ( strstr( szDeviceName, "spidev1" ) )
	{
		CE = UEXT1_CE;
		CSN = UEXT1_CSN;
	}
	else
	{
		CE = UEXT2_CE;
		CSN = UEXT2_CSN;
	}

	testGPIO.sunxi_gpio_output(CE, LOW);
	testGPIO.sunxi_gpio_output(CSN, HIGH);

	uint8_t reg = 0;
	SPIIOBuf testBuf( 2 );
	std::string s2bReg( 2, 0 );
	std::string s6bReg( 6, 0 );
	
	reg = CONFIG;
	std::cout << "=============================================" << std::endl;
	std::cout << "Requesting CONFIG register " << (unsigned int)reg << std::endl;
	s2bReg[0] = ( R_REGISTER | (REGISTER_MASK & reg) );
	testGPIO.sunxi_gpio_output(CSN, LOW);
	testSPI.transfer( testBuf.SetTXData( s2bReg ) );
	testGPIO.sunxi_gpio_output(CSN, HIGH);
	testBuf.Print();
	ConfigReg testConfReg(testBuf.GetRXData()[1]);
	testConfReg.Print();

	reg = CONFIG;
	std::cout << "=============================================" << std::endl;
	std::cout << "Writing CONFIG register " << (unsigned int)reg << std::endl;
	s2bReg[0] = ( W_REGISTER | (REGISTER_MASK & reg) );
	s2bReg[1] = 0x00;
	testGPIO.sunxi_gpio_output(CSN, LOW);
	testSPI.transfer( testBuf.SetTXData( s2bReg ) );
	testGPIO.sunxi_gpio_output(CSN, HIGH);
	testBuf.Print();

	reg = CONFIG;
	std::cout << "=============================================" << std::endl;
	std::cout << "Requesting CONFIG register 6B " << (unsigned int)reg << std::endl;
	s6bReg[0] = ( R_REGISTER | (REGISTER_MASK & reg) );
	testGPIO.sunxi_gpio_output(CSN, LOW);
	testSPI.transfer( testBuf.SetTXData( s6bReg ) );
	testGPIO.sunxi_gpio_output(CSN, HIGH);
	testBuf.Print();

	reg = STATUS;
	std::cout << "=============================================" << std::endl;
	std::cout << "Requesting STATUS register " << (unsigned int)reg << std::endl;
	s2bReg[0] = ( R_REGISTER | (REGISTER_MASK & reg) );
	testGPIO.sunxi_gpio_output(CSN, LOW);
	testSPI.transfer( testBuf.SetTXData( s2bReg ) );
	testGPIO.sunxi_gpio_output(CSN, HIGH);
	testBuf.Print();

	reg = SETUP_AW;
	std::cout << "=============================================" << std::endl;
	std::cout << "Requesting SETUP_AW register " << (unsigned int)reg << std::endl;
	s2bReg[0] = ( R_REGISTER | (REGISTER_MASK & reg) );
	testGPIO.sunxi_gpio_output(CSN, LOW);
	testSPI.transfer( testBuf.SetTXData( s2bReg ) );
	testGPIO.sunxi_gpio_output(CSN, HIGH);
	testBuf.Print();

	return 0;
}
