/**
	@File:			pcnet2.c
	@Author:		Ihsoh
	@Date:			2017-01-23
	@Description:
		AMD PCnet-PCI II 驱动。
*/

#include "pcnet2.h"
#include "net.h"
#include "endian.h"

#include "../types.h"
#include "../memory.h"
#include "../kernel.h"
#include "../screen.h"

#include "../pci/pci.h"

#define	_NAME		"AMD PCnet-PCI II"

#define	_MAX_COUNT	4
#define	_ID			0x20001022	// Vendor ID = 0x1022，Device ID = 0x2000。

#define	_RX_BUFFER_COUNT		32		// “接收缓冲区”的数量。
#define	_TX_BUFFER_COUNT		8		// “发送缓冲区”的数量。

#define	_BUFFER_SIZE			1548	// 每个“接收缓冲区”和“发送缓冲区”的大小。

#define	_DE_SIZE				16		// Descriptor Entry的大小。

#define	_CSR0_INIT		(1 << 0)
#define	_CSR0_STRT		(1 << 1)
#define	_CSR0_STOP		(1 << 2)
#define	_CSR0_TDMD		(1 << 3)
#define	_CSR0_TXON		(1 << 4)
#define	_CSR0_RXON		(1 << 5)
#define	_CSR0_IENA		(1 << 6)
#define	_CSR0_INTR		(1 << 7)
#define	_CSR0_IDON		(1 << 8)
#define	_CSR0_TINT		(1 << 9)
#define	_CSR0_RINT		(1 << 10)
#define	_CSR0_MERR		(1 << 11)
#define	_CSR0_MISS		(1 << 12)
#define	_CSR0_CERR		(1 << 13)
#define	_CSR0_BABL		(1 << 14)
#define	_CSR0_ERR		(1 << 15)

#define	_CSR3_BSWP		(1 << 2)
#define	_CSR3_EMBA		(1 << 3)
#define	_CSR3_DXMT2PD	(1 << 4)
#define	_CSR3_LAPPEN	(1 << 5)
#define	_CSR3_DXSUFLO	(1 << 6)
#define	_CSR3_IDONM		(1 << 8)
#define	_CSR3_TINTM		(1 << 9)
#define	_CSR3_RINTM		(1 << 10)
#define	_CSR3_MERRM		(1 << 11)
#define	_CSR3_MISSM		(1 << 12)
#define	_CSR3_BABLM		(1 << 14)

#define	_CSR15_DRX		(1 << 0)
#define	_CSR15_DTX		(1 << 1)
#define	_CSR15_LOOP		(1 << 2)
#define	_CSR15_DXMTFCS	(1 << 3)
#define	_CSR15_FCOLL	(1 << 4)
#define	_CSR15_DRTY		(1 << 5)
#define	_CSR15_INTL		(1 << 6)
#define	_CSR15_PORTSEL0	(1 << 7)
#define	_CSR15_PORTSEL1	(1 << 8)
#define	_CSR15_LRT		(1 << 9)
#define	_CSR15_TSEL		(1 << 9)
#define	_CSR15_MENDECL	(1 << 10)
#define	_CSR15_DAPC		(1 << 11)
#define	_CSR15_DLNKTST	(1 << 12)
#define	_CSR15_DRCVPA	(1 << 13)
#define	_CSR15_DRCVBC	(1 << 14)
#define	_CSR15_PROM		(1 << 15)

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

	uint8		mac[6];				// MAC地址。

	int32		rx_buffer_idx;		// 储存着下一个可用的“接收缓冲区”的索引。
	int32		tx_buffer_idx;		// 储存着下一个可用的“发送缓冲区”的索引。

	uint8 *		rdes;				// 指向第一个“接收缓冲区”的Descriptor Entry。
	uint8 *		tdes;				// 指向第一个“发送缓冲区”的Descriptor Entry。

	uint8 *		rx_buffer;			// 储存着“接收缓冲区”的地址，所有“接收缓冲区”均连续地分布在一块内存块中。
	uint8 *		tx_buffer;			// 储存着“发送缓冲区”的地址，所有“发送缓冲区”均连续地分布在一块内存块中。

	uint8		ip[4];				// IP地址，[0]=最高字节；...；[3]=最低字节。

	PCNet2InitializationBlock	iniblk;
	NetDevicePtr				netdev;
} PCNet2Device, * PCNet2DevicePtr;

static PCNet2Device _devices[_MAX_COUNT];
static uint32 _count = 0;
static uint32 _pidx2didx[MAX_PERIPHERAL_COUNT];

static
void
_PCNET2Free(
	OUT PCNet2DevicePtr device)
{
	device->index = 0xffffffff;

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

static
void
_PCNET2Interrupt(
	IN uint32 peripheral,
	IN uint32 irq)
{
	uint32 devidx = _pidx2didx[peripheral];
	if (devidx != 0xffffffff)
	{
		PCNet2DevicePtr dev = _PCNET2GetDevice(devidx);
		if (dev != NULL)
		{
			while ((dev->rdes[_DE_SIZE * dev->rx_buffer_idx + 7] & 0x80) == 0)
			{
				uint16 len = *(uint16 *)&dev->rdes[_DE_SIZE * dev->rx_buffer_idx + 8];

				uint8 * buffer = dev->rx_buffer + dev->rx_buffer_idx * _BUFFER_SIZE;

				NetDevicePtr netdev = dev->netdev;
				if (netdev->ProcessPacket != NULL)
				{
					netdev->ProcessPacket(netdev, buffer, len - 4);
				}

				dev->rdes[_DE_SIZE * dev->rx_buffer_idx + 7] = 0x80;

				if (++dev->rx_buffer_idx == _RX_BUFFER_COUNT)
				{
					dev->rx_buffer_idx = 0;	
				}	
			}
		}

		_PCNET2WriteCSR(devidx, 0, _PCNET2ReadCSR(devidx, 0) | 0x0400);
	}
}

uint32
PCNET2GetDeviceCount(void)
{
	return _count;
}

BOOL
PCNET2SetMAC(
	IN uint32 index,
	IN uint8 * mac)
{
	PCNet2DevicePtr device = _PCNET2GetDevice(index);
	if (device == NULL)
	{
		goto err;
	}

	MemCopy(mac, device->mac, sizeof(device->mac));

	return TRUE;
err:
	return FALSE;
}

uint8 *
PCNET2GetMAC(
	IN uint32 index)
{
	PCNet2DevicePtr device = _PCNET2GetDevice(index);
	if (device == NULL)
	{
		goto err;
	}

	return device->mac;
err:
	return NULL;
}

BOOL
PCNET2SetIP(
	IN uint32 index,
	IN uint8 * ip)
{
	PCNet2DevicePtr device = _PCNET2GetDevice(index);
	if (device == NULL)
	{
		goto err;
	}

	MemCopy(ip, device->ip, sizeof(device->ip));

	return TRUE;
err:
	return FALSE;
}

uint8 *
PCNET2GetIP(
	IN uint32 index)
{
	PCNet2DevicePtr device = _PCNET2GetDevice(index);
	if (device == NULL)
	{
		goto err;
	}

	return device->ip;
err:
	return NULL;
}

CASCTEXT
PCNET2GetName(void)
{
	return _NAME;
}

BOOL
PCNET2SendPacket(
	IN uint32 index,
	IN void * packet,
	IN uint16 len)
{
	PCNet2DevicePtr device = _PCNET2GetDevice(index);
	if (device == NULL)
	{
		goto err;
	}

	uint32 idx = device->tx_buffer_idx;
	if ((device->tdes[idx * _DE_SIZE + 7] & 0x80) != 0)
	{
		goto err;
	}

	MemCopy(packet, device->tx_buffer + _BUFFER_SIZE * idx, len);

	device->tdes[idx * _DE_SIZE + 7] |= 0x02;

	device->tdes[idx * _DE_SIZE + 7] |= 0x01;

	uint16 bcnt = ~len + 1;
	bcnt &= 0x0fff;
	bcnt |= 0xf000;
	*(uint16 *) &device->tdes[idx * _DE_SIZE + 4] = bcnt;
	
	device->tdes[idx * _DE_SIZE + 7] |= 0x80;

	if (++idx == _TX_BUFFER_COUNT)
	{
		idx = 0;
	}
	device->tx_buffer_idx = idx;

	return TRUE;
err:
	return FALSE;
}

void
PCNET2PrintStatus(
	IN uint32 index)
{
	if (_PCNET2GetDevice(index) == NULL)
	{
		ScrPrintStringToStderrWithProperty(
			"PCNET2 ERROR: The device dose not exists.",
			CC_RED);
		return;
	}
	ScrPrintString("PCNET2:\n");
	#define	_COLOR(n, f) (csr##n & _CSR##n##_##f ? CC_GREEN : CC_RED)
	uint32 csr0 = _PCNET2ReadCSR(index, 0);
	ScrPrintString("CSR0: ");
	ScrPrintStringWithProperty("INIT ", _COLOR(0, INIT));
	ScrPrintStringWithProperty("STRT ", _COLOR(0, STRT));
	ScrPrintStringWithProperty("STOP ", _COLOR(0, STOP));
	ScrPrintStringWithProperty("TDMD ", _COLOR(0, TDMD));
	ScrPrintStringWithProperty("TXON ", _COLOR(0, TXON));
	ScrPrintStringWithProperty("RXON ", _COLOR(0, RXON));
	ScrPrintStringWithProperty("IENA ", _COLOR(0, IENA));
	ScrPrintStringWithProperty("INTR ", _COLOR(0, INTR));
	ScrPrintStringWithProperty("IDON ", _COLOR(0, IDON));
	ScrPrintStringWithProperty("TINT ", _COLOR(0, TINT));
	ScrPrintStringWithProperty("RINT ", _COLOR(0, RINT));
	ScrPrintStringWithProperty("MERR ", _COLOR(0, MERR));
	ScrPrintStringWithProperty("MISS ", _COLOR(0, MISS));
	ScrPrintStringWithProperty("CERR ", _COLOR(0, CERR));
	ScrPrintStringWithProperty("BABL ", _COLOR(0, BABL));
	ScrPrintStringWithProperty("ERR ", _COLOR(0, ERR));
	ScrPrintString("\n");

	uint32 csr3 = _PCNET2ReadCSR(index, 3);
	ScrPrintString("CSR3: ");
	ScrPrintStringWithProperty("BSWP ", _COLOR(3, BSWP));
	ScrPrintStringWithProperty("EMBA ", _COLOR(3, EMBA));
	ScrPrintStringWithProperty("DXMT2PD ", _COLOR(3, DXMT2PD));
	ScrPrintStringWithProperty("LAPPEN ", _COLOR(3, LAPPEN));
	ScrPrintStringWithProperty("DXSUFLO ", _COLOR(3, DXSUFLO));
	ScrPrintStringWithProperty("IDONM ", _COLOR(3, IDONM));
	ScrPrintStringWithProperty("TINTM ", _COLOR(3, TINTM));
	ScrPrintStringWithProperty("RINTM ", _COLOR(3, RINTM));
	ScrPrintStringWithProperty("MERRM ", _COLOR(3, MERRM));
	ScrPrintStringWithProperty("MISSM ", _COLOR(3, MISSM));
	ScrPrintStringWithProperty("BABLM ", _COLOR(3, BABLM));
	ScrPrintString("\n");

	uint32 csr15 = _PCNET2ReadCSR(index, 15);
	ScrPrintString("CSR15: ");
	ScrPrintStringWithProperty("DRX ", _COLOR(15, DRX));
	ScrPrintStringWithProperty("DTX ", _COLOR(15, DTX));
	ScrPrintStringWithProperty("LOOP ", _COLOR(15, LOOP));
	ScrPrintStringWithProperty("DXMTFCS ", _COLOR(15, DXMTFCS));
	ScrPrintStringWithProperty("FCOLL ", _COLOR(15, FCOLL));
	ScrPrintStringWithProperty("DRTY ", _COLOR(15, DRTY));
	ScrPrintStringWithProperty("INTL ", _COLOR(15, INTL));
	ScrPrintStringWithProperty("PORTSEL0 ", _COLOR(15, PORTSEL0));
	ScrPrintStringWithProperty("PORTSEL1 ", _COLOR(15, PORTSEL1));
	ScrPrintStringWithProperty("LRT ", _COLOR(15, LRT));
	ScrPrintStringWithProperty("TSEL ", _COLOR(15, TSEL));
	ScrPrintStringWithProperty("MENDECL ", _COLOR(15, MENDECL));
	ScrPrintStringWithProperty("DAPC ", _COLOR(15, DAPC));
	ScrPrintStringWithProperty("DLNKTST ", _COLOR(15, DLNKTST));
	ScrPrintStringWithProperty("DRCVPA ", _COLOR(15, DRCVPA));
	ScrPrintStringWithProperty("DRCVBC ", _COLOR(15, DRCVBC));
	ScrPrintStringWithProperty("PROM ", _COLOR(15, PROM));
	ScrPrintString("\n");
	#undef	_COLOR
}

static BOOL _NetSetIP(IN void * device, IN uint8 * ip)
{
	NetDevicePtr netdev = (NetDevicePtr)device;
	return PCNET2SetIP(netdev->id, ip);
}

static uint8 * _NetGetIP(IN void * device)
{
	NetDevicePtr netdev = (NetDevicePtr)device;
	return PCNET2GetIP(netdev->id);
}

static BOOL _NetSetMAC(IN void * device, IN uint8 * mac)
{
	NetDevicePtr netdev = (NetDevicePtr)device;
	return PCNET2SetMAC(netdev->id, mac);
}

static uint8 * _NetGetMAC(IN void * device)
{
	NetDevicePtr netdev = (NetDevicePtr)device;
	return PCNET2GetMAC(netdev->id);
}

static CASCTEXT _NetGetName(IN void * device)
{
	return PCNET2GetName();
}

static BOOL _NetSendPacket(IN void * device, IN void * packet, IN uint16 len)
{
	NetDevicePtr netdev = (NetDevicePtr)device;
	return PCNET2SendPacket(netdev->id, packet, len);
}

void
PCNET2Init(void)
{
	uint32 i;
	for (i = 0; i < _MAX_COUNT; i++)
	{
		_devices[i].index = 0xffffffff;
	}
	for (i = 0; i < MAX_PERIPHERAL_COUNT; i++)
	{
		_pidx2didx[i] = 0xffffffff;
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

			// 构造NetDevice对象。
			NetDevicePtr netdev = NEW(NetDevice);
			netdev->id = devidx;
			netdev->SetIP = _NetSetIP;
			netdev->GetIP = _NetGetIP;
			netdev->SetMAC = _NetSetMAC;
			netdev->GetMAC = _NetGetMAC;
			netdev->GetName = _NetGetName;
			netdev->SendPacket = _NetSendPacket;
			NetAdd(netdev);
			device->netdev = netdev;

			// 开启IO端口以及Bus Mastering。
			uint32 conf = PciReadCfgRegValue(devidx, 0x04);
			conf &= 0xffff0000;
			conf |= 0x5;
			PciWriteCfgRegValue(devidx, 0x04, conf);

			// 获取MAC地址。
			device->mac[5] = KnlInByte(device->iobase + 5);
			device->mac[4] = KnlInByte(device->iobase + 4);
			device->mac[3] = KnlInByte(device->iobase + 3);
			device->mac[2] = KnlInByte(device->iobase + 2);
			device->mac[1] = KnlInByte(device->iobase + 1);
			device->mac[0] = KnlInByte(device->iobase + 0);

			// 设置初始IP地址。
			MemClear(device->ip, 0xff, sizeof(device->ip));

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
			MemClear(device->rdes, 0, _DE_SIZE * _RX_BUFFER_COUNT);
			for (j = 0; j < _RX_BUFFER_COUNT; j++)
			{
				*(uint32 *) &device->rdes[j * _DE_SIZE + 0] = (uint32) (device->rx_buffer + j * _BUFFER_SIZE);
				*(uint16 *) &device->rdes[j * _DE_SIZE + 4] = bcnt;
				device->rdes[j * _DE_SIZE + 7] = 0x80;
			}

			// 初始化“发送缓冲区”。
			device->tx_buffer_idx = 0;
			device->tx_buffer = (uint8 *) MemAlloc(_BUFFER_SIZE * _TX_BUFFER_COUNT);
			device->tdes = (uint8 *) MemAlloc(_DE_SIZE * _TX_BUFFER_COUNT);
			MemClear(device->tdes, 0, _DE_SIZE * _TX_BUFFER_COUNT);
			for (j = 0; j < _TX_BUFFER_COUNT; j++)
			{
				*(uint32 *) &device->tdes[j * _DE_SIZE + 0] = (uint32) (device->tx_buffer + j * _BUFFER_SIZE);
				*(uint16 *) &device->tdes[j * _DE_SIZE + 4] = bcnt;
				device->tdes[j * _DE_SIZE + 7] = 0x00;
			}

			// 设置中断处理程序。
			uint32 irq = dev->header->cfg_hdr.intr & 0xff;
			uint32 pindex = KnlConvertIrqToPeripheralIndex(irq);
			_pidx2didx[pindex] = devidx;
			KnlSetPeripheralInterrupt(pindex, _PCNET2Interrupt);

			// 设置NIC。
			uint32 csr3 = _PCNET2ReadCSR(devidx, 3);
			csr3 &= 0xfffff8fb;
			_PCNET2WriteCSR(devidx, 3, csr3);

			uint32 csr4 = _PCNET2ReadCSR(devidx, 4);
			csr4 |= (1 << 11);
			_PCNET2WriteCSR(devidx, 4, csr4);

			PCNet2InitializationBlockPtr iniblk = &device->iniblk;
			iniblk->mode = 0;
			iniblk->rlen = _PCNET2EncodeRTLen(_RX_BUFFER_COUNT) << 4;
			iniblk->tlen = _PCNET2EncodeRTLen(_TX_BUFFER_COUNT) << 4;
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
			csr0 |= 0x00000043;
			_PCNET2WriteCSR(devidx, 0, csr0);
		}
	}
}
