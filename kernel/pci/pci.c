/**
	@File:			pci.c
	@Author:		Ihsoh
	@Date:			2016-01-05
	@Description:
		对PCI总线进行操作。
*/

#include "pci.h"

#include "../types.h"
#include "../386.h"
#include "../memory.h"

static PCIDeviceInfoPtr _devices	= NULL;
static uint32 _devcount				= 0;

/**
	@Function:		_PciScanDevices
	@Access:		Private
	@Description:
		扫描所有总线的所有槽来检测设备。
	@Parameters:
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
static
BOOL
_PciScanDevices(void)
{
	uint32 bus;
	uint32 slot;
	for(bus = 0; bus < PCI_MAX_BUS_COUNT; bus++)
		for(slot = 0; slot < PCI_MAX_SLOT_COUNT; slot++)
		{
			/*
				配置空间地址寄存器格式：
					Bit0 ~ Bit1：恒为0。
					Bit2 ~ Bit7：寄存器号。
					Bit8 ~ Bit10：功能号。
					Bit11 ~ Bit15：设备号。
					Bit16 ~ Bit23：总线号。
					Bit24 ~ Bit30：保留。
					Bit31：使能位（1=允许，0=禁止）。
			*/	
			uint32 addr = 	0x80000000
							+ bus * 0x10000
							+ (slot * 8) * 0x100;
			KnlOutLong(PCI_CONFIG_ADDRESS, addr);
			uint32 data = KnlInLong(PCI_CONFIG_DATA);
			if(data == 0xffffffff)
				continue;
			PCIDeviceInfoPtr dev = &_devices[_devcount++];
			dev->bus = bus;
			dev->slot = slot;
			dev->header = (PCIHeaderPtr)MemAlloc(sizeof(PCIHeader));
			if(dev->header == NULL)
				goto err;
			uint32 ui;
			uint32 * header = (uint32 *)dev->header;
			for(ui = 0; ui < sizeof(PCIHeader) / 4; ui++)
			{
				KnlOutLong(PCI_CONFIG_ADDRESS, addr | (ui << 2));
				header[ui] = KnlInLong(PCI_CONFIG_DATA);
			}
			if(_devcount == PCI_MAX_DEVICE_COUNT)
				goto ok;
		}
ok:
	return TRUE;
err:
	return FALSE;
}

/**
	@Function:		PciInitDevices
	@Access:		Public
	@Description:
		初始化PCI设备所需的资源，并扫描总线获取设备信息。
	@Parameters:
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
BOOL
PciInitDevices(void)
{
	_devices = (PCIDeviceInfoPtr)MemAlloc(PCI_MAX_DEVICE_COUNT * sizeof(PCIDeviceInfo));
	if(_devices == NULL)
		return FALSE;
	return _PciScanDevices();
}

/**
	@Function:		PciFreeDevices
	@Access:		Public
	@Description:
		释放PCI设备所占用的资源。
	@Parameters:
	@Return:
*/
void
PciFreeDevices(void)
{
	if(_devices == NULL)
		return;
	uint32 ui;
	for(ui = 0; ui < _devcount; ui++)
		if(_devices[ui].header != NULL)
		{
			MemFree(_devices[ui].header);
			_devices[ui].header = NULL;
		}
	MemFree(_devices);
	_devices = NULL;
}

/**
	@Function:		PciGetDeviceCount
	@Access:		Public
	@Description:
		获取PCI设备的数量。
	@Parameters:
	@Return:
		uint32
			PCI设备的数量。
*/
uint32
PciGetDeviceCount(void)
{
	return _devcount;
}

/**
	@Function:		PciGetDevice
	@Access:		Public
	@Description:
		获取PCI设备的信息。
	@Parameters:
		index, uint32, IN
			PCI设备的索引。
	@Return:
		PCIDeviceInfoPtr
			指向PCI设备信息对象的指针。
*/
PCIDeviceInfoPtr
PciGetDevice(IN uint32 index)
{
	if(index >= _devcount)
		return NULL;
	return &_devices[index];
}
