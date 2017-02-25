/**
	@File:			e1000.c
	@Author:		Ihsoh
	@Date:			2017-02-25
	@Description:
		E1000 驱动。
*/

#include "e1000.h"
#include "net.h"
#include "endian.h"

#include "../types.h"
#include "../memory.h"
#include "../kernel.h"
#include "../screen.h"

#include "../pci/pci.h"

#define	_NAME		"E1000"

#define	_MAX_COUNT	4
#define	_ID0		0x100e8086
#define	_ID1		0x10048086
#define	_ID2		0x100f8086

typedef struct
{
	volatile uint64		addr;
	volatile uint16		length;
	volatile uint16		checksum;
	volatile uint8		status;
	volatile uint8		errors;
	volatile uint16		special;
} __attribute__((packed)) E1000ReceiveDescriptor, * E1000ReceiveDescriptorPtr;

typedef struct
{
	volatile uint64		addr;
	volatile uint16		length;
	volatile uint8		cso;
	volatile uint8		cmd;
	volatile uint8		status;
	volatile uint8		css;
	volatile uint16		special;
} __attribute__((packed)) E1000TransmitDescriptor, * E1000TransmitDescriptorPtr;

typedef struct
{
	uint32		index;

	uint32		membase;

	uint8		mac[6];

	int32		rx_buffer_idx;
	int32		tx_buffer_idx;

	uint8 *		rx_buffer;
	uint8 *		tx_buffer;

	uint8		ip[4];

	NetDevicePtr				netdev;

	BOOL		tx_lock;
} E1000Device, * E1000DevicePtr;

static E1000Device _devices[_MAX_COUNT];
static uint32 _count = 0;

static
E1000DevicePtr
_E1000GetDevice(
	IN uint32 index)
{
	if (index >= _MAX_COUNT
		|| _devices[index].index == 0xffffffff)
	{
		return NULL;
	}
	return &_devices[index];
}

static
uint32
_E1000MMIORead32(
	IN uint32 addr)
{
	return *((volatile uint32 *)addr);
}

static
void
_E1000MMIOWrite32(
	IN uint32 addr,
	IN uint32 value)
{
	*((volatile uint32 *)addr) = value;
}

static
void
_E1000WriteCommand(
	IN uint32 index,
	IN uint32 addr,
	IN uint32 value)
{
	E1000DevicePtr device = _E1000GetDevice(index);
	if (device == NULL)
	{
		return;
	}
	_E1000MMIOWrite32(device->membase + addr, value);
}

static
uint32
_E1000ReadCommand(
	IN uint32 index,
	IN uint32 addr)
{
	E1000DevicePtr device = _E1000GetDevice(index);
	if (device == NULL)
	{
		return;
	}
	return _E1000MMIORead32(device->membase + addr);
}





void
E1000Init(void)
{
	uint32 i;
	for (i = 0; i < _MAX_COUNT; i++)
	{
		_devices[i].index = 0xffffffff;
	}
	uint32 c = PciGetDeviceCount();
	for (i = 0; i < c && _count < _MAX_COUNT; i++)
	{
		PCIDeviceInfoPtr dev = PciGetDevice(i);
		uint32 id = dev->header->cfg_hdr.id;
		if (dev != NULL
			&& (id == _ID0 || id == _ID1 || id == _ID2))
		{
			uint32 devidx = _count++;
			E1000DevicePtr device = &_devices[devidx];
			device->index = i;
			device->membase = dev->header->cfg_hdr.bars[0];
			device->tx_lock = FALSE;


		}
	}
}
