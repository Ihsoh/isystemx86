/**
	@File:			pcnet2.c
	@Author:		Ihsoh
	@Date:			2017-01-23
	@Description:
		PCnet-PCI II 驱动。
*/

#include "pcnet2.h"

#include "../types.h"

#include "../pci/pci.h"

#define	_MAX_COUNT	4
#define	_ID			0x20001022	// Vendor ID = 0x1022，Device ID = 0x2000。

typedef struct
{
	// 当为0xffffffff代表设备不存在。
	uint32 index;

	// NIC的IO BASE。
	uint32 iobase;

	// MAC地址，以小端储存，即mac[0]为MAC地址最低字节，mac[5]为MAC地址最高字节。
	uint8 mac[6];

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
}

void
PCNET2Init(void)
{
	uint32 i;
	for (i = 0; i < _MAX_COUNT; i++)
	{
		_PCNET2Free(&_devices[i]);
	}
	uint32 c = PciGetDeviceCount();
	for (i = 0; i < c && _count < _MAX_COUNT; i++)
	{
		PCIDeviceInfoPtr dev = PciGetDevice(i);
		if (dev != NULL
			&& dev->header->cfg_hdr.id == _ID)
		{
			PCNet2DevicePtr device = &_devices[_count++];
			device->index = i;
			device->iobase = dev->header->cfg_hdr.bars[0] & 0xfffffffe;

			// 开启IO端口以及Bus Mastering。
			uint32 conf = PciReadCfgRegValue(i, 0x04);
			conf &= 0xffff0000;
			conf |= 0x5;
			PciWriteCfgRegValue(i, 0x04, conf);

			// 获取MAC地址。
			// 
			// IO_BASE + 5	---->	MAC地址最低字节
			// ...
			// IO_BASE + 0	---->	MAC地址最高字节
			device->mac[0] = KnlInByte(device->iobase + 5);
			device->mac[1] = KnlInByte(device->iobase + 4);
			device->mac[2] = KnlInByte(device->iobase + 3);
			device->mac[3] = KnlInByte(device->iobase + 2);
			device->mac[4] = KnlInByte(device->iobase + 1);
			device->mac[5] = KnlInByte(device->iobase + 0);

			// 启用32位模式。
			KnlOutLong(device->iobase + 0x10, 0);
		}
	}
}

uint32
PCNET2GetDeviceCount(void)
{
	return _count;
}

