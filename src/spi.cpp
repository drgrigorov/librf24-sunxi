/*
 * File:   spi.cpp
 * Author: Purinda Gunasekara <purinda@gmail.com>
 *
 * Created on 24 June 2012, 11:00 AM
 *
 * Inspired from spidev test in linux kernel documentation
 * www.kernel.org/doc/Documentation/spi/spidev_test.c
 */

#include "spi.h"
#include <cstring>

SPIIOBuf::SPIIOBuf( const int& nSize ) throw()
:
	m_nSize( nSize )
{
	if (m_nSize > 32) m_nSize = 32;
}

//------------------------------------------------------------------------------

SPIIOBuf::SPIIOBuf( const std::string& sTXData ) throw()
:
	m_nSize( 0 )
{
	m_sTXData = sTXData;
	if ( m_sTXData.size() > 32 ) m_sTXData = m_sTXData.substr(0,32);
	m_nSize = m_sTXData.size();
}

//------------------------------------------------------------------------------

SPIIOBuf::~SPIIOBuf() throw()
{
}

//------------------------------------------------------------------------------

std::string SPIIOBuf::GetTXData() const throw()
{
	return m_sTXData;
}

//------------------------------------------------------------------------------

std::string SPIIOBuf::GetRXData() const throw()
{
	return m_sRXData;
}

//------------------------------------------------------------------------------

SPIIOBuf& SPIIOBuf::SetTXData( std::string& sTXData ) throw()
{
	m_sTXData = sTXData;
	if ( m_sTXData.size() > 32 ) m_sTXData = m_sTXData.substr(0,32);
	m_nSize = m_sTXData.size();
	std::string sResetResult( m_sTXData.size(), 0 );
	SetResult( sResetResult );
	return *this;
}

//------------------------------------------------------------------------------

void SPIIOBuf::SetResult( const std::string& sRXData )
{
	m_sRXData = sRXData;
	if ( m_sRXData.size() > m_nSize ) m_sRXData = m_sRXData.substr(0,m_nSize - 1);
}

//------------------------------------------------------------------------------

void SPIIOBuf::Print() const throw()
{
	//For now just use old stuff. If this function is actually required out of
	//the debuggin purposes should be reworked.
	printf("SPIIOBuf| size %d\n", m_nSize );
	printf("SPIIOBuf| tx (size=%d): ", m_sTXData.size() );
	for (uint8_t i=0; i < m_sTXData.size(); i++ )
	{
		printf("%02x, ", m_sTXData[i] );
	}
	printf("\nSPIIOBuf| rx (size=%d): ", m_sRXData.size() );
	for (uint8_t i=0; i < m_sRXData.size(); i++ )
	{
		printf("%02x, ", m_sRXData[i] );
	}
	printf("\n");
}

//------------------------------------------------------------------------------

SPI::SPI(string spidev, int speed, int bits)
{
	this->device = spidev;
	this->bits = bits;
	this->speed = speed;
	this->mode = 0;
	this->init();
}

void SPI::init()
{
	int ret;
	this->fd = open(this->device.c_str(), O_RDWR);
	if (this->fd < 0)
	{
		perror("can't open device");
		abort();
	}

	ret = ioctl(this->fd, SPI_IOC_WR_MODE, &this->mode);
	if (ret == -1)
	{
		perror("can't set spi mode");
		abort();
	}

	ret = ioctl(this->fd, SPI_IOC_RD_MODE, &this->mode);
	if (ret == -1)
	{
		perror("can't set spi mode");
		abort();
	}

	ret = ioctl(this->fd, SPI_IOC_WR_BITS_PER_WORD, &this->bits);
	if (ret == -1)
	{
		perror("can't set bits per word");
		abort();
	}

	ret = ioctl(this->fd, SPI_IOC_RD_BITS_PER_WORD, &this->bits);
	if (ret == -1)
	{
		perror("can't set bits per word");
		abort();
	}

	ret = ioctl(this->fd, SPI_IOC_WR_MAX_SPEED_HZ, &this->speed);
	if (ret == -1)
	{
		perror("can't set max speed hz");
		abort();
	}

	ret = ioctl(this->fd, SPI_IOC_RD_MAX_SPEED_HZ, &this->speed);
	if (ret == -1)
	{
		perror("can't set max speed hz");
		abort();
	}
}

uint8_t SPI::read(SPIIOBuf& trxData)
{
	int ret = 0;
	struct spi_ioc_transfer tr;
	memset(&tr, 0, sizeof(tr));

	std::string sTmp = trxData.GetTXData();
	const int len = sTmp.size();
	uint8_t rx[len] = {0};
	//uint8_t tx[len] = {0};
	//tr.tx_buf = (unsigned long)tx;
	tr.rx_buf = (unsigned long)rx;
	tr.len = len;
	tr.delay_usecs = 0;
	tr.speed_hz = this->speed;
	tr.bits_per_word = this->bits;

	ret = ioctl(this->fd, SPI_IOC_MESSAGE(1), &tr);
	if ( ret > 0 )
	{
		trxData.SetResult( std::string( (const char*)tr.rx_buf, tr.len ) );
	}
	else
	{
		perror("ioctl spi_transfer");
	}
	return ret;
}

void PrintSPIMsg( const struct spi_ioc_transfer val )
{
        printf( "len: %d\n", val.len );
        if (val.tx_buf)
        {
                printf( "\ntx_buf: " );
                for (size_t i = 0; i < val.len; i++ )
                {
                        printf("%02x ", ((char*)val.tx_buf)[i] );
                }
        }
        if (val.rx_buf)
        {
                printf( "\nrx_buf: " );
                for (size_t i = 0; i < val.len; i++ )
                {
                        printf("%02x ", ((char*)val.rx_buf)[i] );
                }
        }
        printf("\n");
}

uint8_t SPI::transfer(SPIIOBuf& trxData)
{
	int ret = 0;
	struct spi_ioc_transfer tr;
	memset(&tr, 0, sizeof(tr) );

	const int len = trxData.GetSize();
	uint8_t rx[len] = {0};
	uint8_t tx[len] = {0};

	memcpy( tx, trxData.GetTXData().c_str(), len );

	tr.tx_buf = (unsigned long)tx;
	tr.rx_buf = (unsigned long)rx;
	tr.len = len;
	//tr.delay_usecs = 0;
	//tr.speed_hz = this->speed;
	//tr.bits_per_word = this->bits;

	//printf("#####\n");
	//trxData.Print();
	ret = ioctl(this->fd, SPI_IOC_MESSAGE(1), &tr);
	if ( ret > 0 )
	{
		trxData.SetResult( std::string( (const char*)tr.rx_buf, tr.len ) );
	}
	else
	{
		perror("ioctl spi_transfer");
	}

	//printf("tr\n" );
	//PrintSPIMsg( tr );

	//trxData.Print();
	//printf("#####\n");
	return ret;
}

//uint8_t SPI::transfer(uint8_t tx_)
//{
//	int ret;
//	// One byte is transfered at once
//	uint8_t tx[] = {0};
//	tx[0] = tx_;
//
//	uint8_t rx[ARRAY_SIZE(tx)] = {0};
//	struct spi_ioc_transfer tr;
//	tr.tx_buf = (unsigned long)tx;
//	tr.rx_buf = (unsigned long)rx;
//	tr.len = ARRAY_SIZE(tx);
//	tr.delay_usecs = 0;
////	tr.interbyte_usecs = 10;
//	tr.speed_hz = this->speed;
//	tr.bits_per_word = this->bits;
//
//	ret = ioctl(this->fd, SPI_IOC_MESSAGE(1), &tr);
//	if (ret < 1)
//	{
//		perror("can't send spi message");
//		abort();
//	}
//
//	return rx[0];
//}

SPI::~SPI() {
	close(this->fd);
}
