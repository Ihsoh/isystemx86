/**
	@File:			pcnet2.c
	@Author:		Ihsoh
	@Date:			2017-01-23
	@Description:
		PCnet-PCI II 驱动。
*/

#include "pcnet2.h"

#include "../types.h"
#include "../memory.h"
#include "../kernel.h"

#include "../pci/pci.h"

#define	_MAX_COUNT	4
#define	_ID			0x20001022	// Vendor ID = 0x1022，Device ID = 0x2000。

#define	_RX_BUFFER_COUNT		32		// “接收缓冲区”的数量。
#define	_TX_BUFFER_COUNT		8		// “发送缓冲区”的数量。

#define	_BUFFER_SIZE			1548	// 每个“接收缓冲区”和“发送缓冲区”的大小。

#define	_DE_SIZE				16		// Descriptor Entry的大小。

typedef struct
{
	uint16		mode;
	uint8		rlen;
	uint8		tlen;
	uint8		mac[6];
	uint16		reserved;
	uint8		ladr[8];
	uint32		rdes;
	uint32		tdes;
} PCNet2InitializationBlock, * PCNet2InitializationBlockPtr;

typedef struct
{
	uint32		index;				// 当为0xffffffff代表设备不存在。

	uint32		iobase;				// NIC的IO BASE。

	uint8		mac[6];				// MAC地址，以小端储存，即mac[0]为MAC地址最低字节，mac[5]为MAC地址最高字节。

	int32		rx_buffer_idx;		// 储存着下一个可用的“接收缓冲区”的索引。
	int32		tx_buffer_idx;		// 储存着下一个可用的“发送缓冲区”的索引。

	uint8 *		rdes;				// 指向第一个“接收缓冲区”的Descriptor Entry。
	uint8 *		tdes;				// 指向第一个“发送缓冲区”的Descriptor Entry。

	uint8 *		rx_buffer;			// 储存着“接收缓冲区”的地址，所有“接收缓冲区”均连续地分布在一块内存块中。
	uint8 *		tx_buffer;			// 储存着“发送缓冲区”的地址，所有“发送缓冲区”均连续地分布在一块内存块中。

	PCNet2InitializationBlock iniblk;
} PCNet2Device, * PCNet2DevicePtr;

static PCNet2Device _devices[_MAX_COUNT];
static uint32 _count = 0;

static
void
_PCNET2Free(
	OUT PCNet2DevicePtr device)
{
	device->index = 0xffffffff;
	
	device->iobase = 0xffffffff;
	
	device->mac[0] = 0xff;
	device->mac[1] = 0xff;
	device->mac[2] = 0xff;
	device->mac[3] = 0xff;
	device->mac[4] = 0xff;
	device->mac[5] = 0xff;

	device->rx_buffer_idx = 0;
	device->tx_buffer_idx = 0;

	if (device->rdes != NULL)
	{
		DELETE(device->rdes);
		device->rdes = NULL;
	}
	if (device->tdes != NULL)
	{
		DELETE(device->tdes);
		device->tdes = NULL;
	}

	if (device->rx_buffer != NULL)
	{
		DELETE(device->rx_buffer);
		device->rx_buffer = NULL;
	}
	if (device->tx_buffer != NULL)
	{
		DELETE(device->tx_buffer);
		device->tx_buffer = NULL;
	}
}

static
PCNet2DevicePtr
_PCNET2GetDevice(
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
void
_PCNET2WriteRAP(
	IN uint32 index,
	IN uint32 value)
{
	PCNet2DevicePtr device = _PCNET2GetDevice(index);
	if (device == NULL)
	{
		return;
	}
	KnlOutLong(device->iobase + 0x14, value);
}

static
uint32
_PCNET2ReadCSR(
	IN uint32 index,
	IN uint32 csr)
{
	PCNet2DevicePtr device = _PCNET2GetDevice(index);
	if (device == NULL)
	{
		return 0;
	}
	_PCNET2WriteRAP(index, csr);
	return KnlInLong(device->iobase + 0x10);
}

static
void
_PCNET2WriteCSR(
	IN uint32 index,
	IN uint32 csr,
	IN uint32 value)
{
	PCNet2DevicePtr device = _PCNET2GetDevice(index);
	if (device == NULL)
	{
		return;
	}
	_PCNET2WriteRAP(index, csr);
	KnlOutLong(device->iobase + 0x10, value);
}

static
uint32
_PCNET2ReadBCR(
	IN uint32 index,
	IN uint32 bcr)
{
	PCNet2DevicePtr device = _PCNET2GetDevice(index);
	if (device == NULL)
	{
		return 0;
	}
	_PCNET2WriteRAP(index, bcr);
	return KnlInLong(device->iobase + 0x1c);
}

static
void
_PCNET2WriteBCR(
	IN uint32 index,
	IN uint32 bcr,
	IN uint32 value)
{
	PCNet2DevicePtr device = _PCNET2GetDevice(index);
	if (device == NULL)
	{
		return;
	}
	_PCNET2WriteRAP(index, bcr);
	KnlOutLong(device->iobase + 0x1c, value);
}

static
uint8
_PCNET2EncodeRTLen(uint32 count)
{
	switch (count)
	{
		case 1:
			return 0x00;
		case 2:
			return 0x01;
		case 4:
			return 0x02;
		case 8:
			return 0x03;
		case 16:
			return 0x04;
		case 32:
			return 0x05;
		case 64:
			return 0x06;
		case 128:
			return 0x07;
		case 256:
			return 0x08;
		case 512:
			return 0x09;
		default:
			return 0xff;
	}
}

#include "../screen.h"

static
void
_PCNET2Interrupt(
	IN uint32 peripheral,
	IN uint32 irq)
{
	ScrPrintString("#");
	printn(peripheral);
	ScrPrintString(",");
	printn(irq);
	ScrPrintString("\n");
}

void
PCNET2Init(void)
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
		if (dev != NULL
			&& dev->header->cfg_hdr.id == _ID)
		{
			uint32 devidx = _count++;
			PCNet2DevicePtr device = &_devices[devidx];
			device->index = i;
			device->iobase = dev->header->cfg_hdr.bars[0] & 0xfffffffe;

			// 开启IO端口以及Bus Mastering。
			uint32 conf = PciReadCfgRegValue(devidx, 0x04);
			conf &= 0xffff0000;
			conf |= 0x5;
			PciWriteCfgRegValue(devidx, 0x04, conf);

			// 获取MAC地址。
			// 
			// IO_BASE + 5	---->	MAC地址最低字节
			// ...
			// IO_BASE + 0	---->	MAC地址最高字节
			device->mac[5] = KnlInByte(device->iobase + 5);
			device->mac[4] = KnlInByte(device->iobase + 4);
			device->mac[3] = KnlInByte(device->iobase + 3);
			device->mac[2] = KnlInByte(device->iobase + 2);
			device->mac[1] = KnlInByte(device->iobase + 1);
			device->mac[0] = KnlInByte(device->iobase + 0);

			// 启用32位模式。
			KnlOutLong(device->iobase + 0x10, 0);

			// SWSTYLE。
			uint32 csr58 = _PCNET2ReadCSR(devidx, 58);
			csr58 &= 0xfff0;
			csr58 |= 2;
			_PCNET2WriteCSR(devidx, 58, csr58);

			// ASEL。
			uint32 bcr2 = _PCNET2ReadBCR(devidx, 2);
			bcr2 |= 0x2;
			_PCNET2WriteBCR(devidx, 2, bcr2);

			int32 j;
			uint16 bcnt = (uint16) -_BUFFER_SIZE;
			bcnt &= 0x0fff;
			bcnt |= 0xf000;

			// 初始化“接收缓冲区”。
			device->rx_buffer_idx = 0;
			device->rx_buffer = (uint8 *) MemAlloc(_BUFFER_SIZE * _RX_BUFFER_COUNT);
			device->rdes = (uint8 *) MemAlloc(_DE_SIZE * _RX_BUFFER_COUNT);
			for (j = 0; j < _RX_BUFFER_COUNT; j++)
			{
				*(uint32 *) &device->rdes = (uint32) (device->rx_buffer + j * _BUFFER_SIZE);
				*(uint16 *) &device->rdes[j * _DE_SIZE + 4] = bcnt;
				device->rdes[j * _DE_SIZE + 7] = 0x80;
			}

			// 初始化“发送缓冲区”。
			device->tx_buffer_idx = 0;
			device->tx_buffer = (uint8 *) MemAlloc(_BUFFER_SIZE * _TX_BUFFER_COUNT);
			device->tdes = (uint8 *) MemAlloc(_DE_SIZE * _TX_BUFFER_COUNT);
			for (j = 0; j < _TX_BUFFER_COUNT; j++)
			{
				*(uint32 *) &device->tdes = (uint32) (device->tx_buffer + j * _BUFFER_SIZE);
				*(uint16 *) &device->tdes[j * _DE_SIZE + 4] = bcnt;
			}

			KnlSetPeripheralInterrupt(0, _PCNET2Interrupt);
			KnlSetPeripheralInterrupt(1, _PCNET2Interrupt);
			KnlSetPeripheralInterrupt(2, _PCNET2Interrupt);
			KnlSetPeripheralInterrupt(3, _PCNET2Interrupt);

			// 设置NIC。
			uint32 csr3 = _PCNET2ReadCSR(devidx, 3);
			csr3 &= 0xfffff8fb;
			_PCNET2WriteCSR(devidx, 3, csr3);

			PCNet2InitializationBlockPtr iniblk = &device->iniblk;
			iniblk->mode = 0;
			iniblk->rlen = _PCNET2EncodeRTLen(_RX_BUFFER_COUNT);
			iniblk->tlen = _PCNET2EncodeRTLen(_TX_BUFFER_COUNT);
			MemCopy(device->mac, iniblk->mac, 6);
			iniblk->reserved = 0;
			MemClear(iniblk->ladr, 0, 8);
			iniblk->rdes = (uint32) device->rdes;
			iniblk->tdes = (uint32) device->tdes;
			uint32 iadr = (uint32) iniblk;
			_PCNET2WriteCSR(devidx, 1, iadr & 0x0000ffff);
			_PCNET2WriteCSR(devidx, 2, (iadr >> 16) & 0x0000ffff);
			uint32 csr0 = _PCNET2ReadCSR(devidx, 0);
			csr0 &= 0xfffffff8;
			csr0 |= 0x00000003;
			_PCNET2WriteCSR(devidx, 0, csr0);


			device->tdes[0 * _DE_SIZE + 7] |= 0x02;

			device->tdes[0 * _DE_SIZE + 7] |= 0x01;
			
			bcnt = (uint16)-123;
			bcnt &= 0x0fff;
			bcnt |= 0xf000;
			*(uint16 *) &device->tdes[0 * _DE_SIZE + 4] = bcnt;
			
			device->tdes[0 * _DE_SIZE + 7] |= 0x80;
		}
	}
}

uint32
PCNET2GetDeviceCount(void)
{
	return _count;
}
