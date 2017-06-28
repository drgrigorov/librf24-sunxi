/*
 * File:   spi.h
 * Author: Purinda Gunasekara <purinda@gmail.com>
 *
 * Created on 24 June 2012, 11:00 AM
 */

#ifndef SPI_H
#define	SPI_H

#include <string>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <inttypes.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))

using namespace std;

class SPI;
class SPIIOBuf
{
	//Only SPI can write to the RXBuf
	friend class SPI;
public:
	//Creating empty buffer with size nSize. Note that if more than 32 it will be set to 32.
	SPIIOBuf( const int& nSize ) throw();
	//Creating buffer from string. Used for issuing commands and getting results in TX Data member
	//For now the TXData will be truncated to the 32rd byte without notice.
	SPIIOBuf( const std::string& sTXData ) throw();
	virtual ~SPIIOBuf() throw();

	std::string GetTXData() const throw();
	std::string GetRXData() const throw();

	void Print() const throw();

	SPIIOBuf& SetTXData( std::string& sTXData ) throw();

	uint8_t GetSize() const throw() { return m_nSize; };
protected:
	//Because in the SPI the tx and rx buffers are equal the sRXData should be <= sTXData
	//The sRXData will be truncated to the nSize member for now
	void SetResult( const std::string& sRXData );

private:
	std::string m_sRXData;
	std::string m_sTXData;
	//Currently max size is 32 bytes
	uint8_t m_nSize;
};

class SPI {
public:
	SPI(string spidev, int speed, int bits);
	//uint8_t transfer(uint8_t tx_);
	uint8_t transfer(SPIIOBuf& trxData);
	uint8_t read(SPIIOBuf& trxData);
	virtual ~SPI();
private:
	string device;
	uint8_t mode;
	uint8_t bits;
	uint32_t speed;
	int fd;
	void init();
};

#endif	/* SPI_H */
