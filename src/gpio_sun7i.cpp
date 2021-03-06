/*
 * gpio_lib.c
 *
 * Copyright 2013 Stefan Mavrodiev <support@olimex.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 */

#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <assert.h>

#include "gpio_sun7i.h"

GPIO::GPIO(void)
:
    gpio_map( NULL ),
    m_nErr( 0 )
{
    int32_t fd;
    intptr_t addr_start, addr_offset;
    uint32_t PageSize, PageMask;
    fd = open("/dev/mem", O_RDWR);

    if(fd < 0) {
        m_nErr = -1;
	return;
    }

    PageSize = sysconf(_SC_PAGESIZE);
    PageMask = (~(PageSize-1));

    addr_start = SW_PORTC_IO_BASE & PageMask;
    addr_offset = SW_PORTC_IO_BASE & ~PageMask;

    gpio_map = (long int*)(void *)mmap(0, PageSize*2, PROT_READ|PROT_WRITE, MAP_SHARED, fd, addr_start);
    if(gpio_map == MAP_FAILED) {
        m_nErr = -2;
	return;
    }

    SUNXI_PIO_BASE = (intptr_t)gpio_map;
    SUNXI_PIO_BASE += addr_offset;

    close(fd);
}

struct GPIO::sunxi_gpio* GPIO::GetBank(uint16_t pin) const throw()
{
    intptr_t bank = GPIO_BANK(pin);

	struct sunxi_gpio_reg* base = (struct sunxi_gpio_reg *)SUNXI_PIO_BASE;

	return &base->gpio_bank[bank];
}

uint8_t* GPIO::GetCfgAddr( uint16_t pin ) const throw()
{
    struct sunxi_gpio *pio = GetBank( pin );
	//new structure has 16 bytes of cfg fields
	//index should be pin%32 (to remove the banks) / 2
	//(because each byte has 2 pins configured in it)
	const unsigned char index = (pin%32)/2; //should always return 0-15
	return &(pio->cfg[index]);
}

int8_t GPIO::SetCfgpin(uint16_t pin, uint8_t val) throw()
{
    if(SUNXI_PIO_BASE == 0)
	{
        return -1;
    }

	//Check if pin is out of range. Max pin is 9 banks by 32 pins.
	if ( pin < 0 || pin >= 9*32 )
	{
		return -2;
	}

	uint8_t* cfgAddr = GetCfgAddr( pin );
	uint8_t nBeforeChange = *cfgAddr;

	if (pin%2)
	{
		//Clear the most sigificant semi octet
		*cfgAddr &= 0x0f;
		*cfgAddr |= (val << 4) & 0xf0;
	}
	else
	{
		//Clear the least sigificant semi octet
		*cfgAddr &= 0xf0;
		*cfgAddr |= val & 0x0f;
	}

	if (nBeforeChange == *cfgAddr)
	{
		//Change was not done
		return 0;
	}

	//Change was done
    return 1;
}

int8_t GPIO::GetCfgpin(uint16_t pin) throw()
{
    if(SUNXI_PIO_BASE == 0)
	{
        return -1;
    }

	//Check if pin is out of range. Max pin is 9 banks of 32 pins.
	if ( pin < 0 || pin >= 9*32 )
	{
		return -2;
	}

	uint8_t* cfgAddr = GetCfgAddr( pin );

	if (pin%2)
	{
		return (*cfgAddr >> 4) & 0x0f;
	}
	else
	{
		return *cfgAddr & 0x0f;
	}
}

int8_t GPIO::SetVal(uint16_t pin, uint8_t val) throw()
{
    unsigned int bank = GPIO_BANK(pin);
    unsigned int num = GPIO_NUM(pin);

    if(SUNXI_PIO_BASE == 0)
    {
        return -1;
    }
    struct sunxi_gpio *pio =&((struct sunxi_gpio_reg *)SUNXI_PIO_BASE)->gpio_bank[bank];

    if(val)
        *(&pio->dat) |= 1 << num;
    else
        *(&pio->dat) &= ~(1 << num);

    return 0;
}

int8_t GPIO::GetVal(uint16_t pin) throw()
{
    unsigned int dat;
    unsigned int bank = GPIO_BANK(pin);
    unsigned int num = GPIO_NUM(pin);

    if(SUNXI_PIO_BASE == 0)
    {
        return -1;
    }

    struct sunxi_gpio *pio =&((struct sunxi_gpio_reg *)SUNXI_PIO_BASE)->gpio_bank[bank];

    dat = *(&pio->dat);
    dat >>= num;

    return (dat & 0x1);
}

int8_t GPIO::GetPullUp(uint16_t pin) throw()
{
	struct sunxi_gpio *pio = GetBank( pin );
	uint8_t index = (pin % 32)/16;

	//should be impossible
	assert( index < 2 );

	uint32_t pull = pio->pull[ index ];
	
	//0 -> 14
	//1 -> 12
	//2 -> 10 ...
	//30 - 2*(( x % 32 ) % 16) =
	return ( 0x03 & (pull >> ( 30 - 2*(( pin % 32 )%16 ))));
}

int8_t GPIO::GetLevel(uint16_t pin) throw()
{
	struct sunxi_gpio *pio = GetBank( pin );
	uint8_t index = (pin % 32)/16;

	//should be impossible
	assert( index < 2 );

	uint32_t drv = pio->drv[ index ];
	
	return ( 0x03 & (drv >> ( 30 - 2*(( pin % 32 )%16 ))));
}

void GPIO::Cleanup(void) throw()
{
    unsigned int PageSize;
    if (gpio_map == NULL)
        return;

    PageSize = sysconf(_SC_PAGESIZE);
    munmap((void*)gpio_map, PageSize*2);
}

GPIO::~GPIO()
{
	Cleanup();
}
