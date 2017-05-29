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

#define	_RX_BUFFER_COUNT		32		// “接收缓冲区”的数量。
#define	_TX_BUFFER_COUNT		8		// “发送缓冲区”的数量。

#define	_BUFFER_SIZE			1548	// 每个“接收缓冲区”和“发送缓冲区”的大小。

#define _E1000_REG_CTRL			0x0000
#define _E1000_REG_STATUS		0x0008
#define _E1000_REG_EEPROM		0x0014
#define _E1000_REG_CTRL_EXT		0x0018

#define _E1000_REG_RCTRL		0x0100
#define _E1000_REG_RXDESCLO		0x2800
#define _E1000_REG_RXDESCHI		0x2804
#define _E1000_REG_RXDESCLEN	0x2808
#define _E1000_REG_RXDESCHEAD	0x2810
#define _E1000_REG_RXDESCTAIL	0x2818

#define _E1000_REG_TCTRL		0x0400
#define _E1000_REG_TXDESCLO		0x3800
#define _E1000_REG_TXDESCHI		0x3804
#define _E1000_REG_TXDESCLEN	0x3808
#define _E1000_REG_TXDESCHEAD	0x3810
#define _E1000_REG_TXDESCTAIL	0x3818

#define _RCTL_EN				(1 << 1)	/* Receiver Enable */
#define _RCTL_SBP				(1 << 2)	/* Store Bad Packets */
#define _RCTL_UPE				(1 << 3)	/* Unicast Promiscuous Enabled */
#define _RCTL_MPE				(1 << 4)	/* Multicast Promiscuous Enabled */
#define _RCTL_LPE				(1 << 5)	/* Long Packet Reception Enable */
#define _RCTL_LBM_NONE			(0 << 6)	/* No Loopback */
#define _RCTL_LBM_PHY			(3 << 6)	/* PHY or external SerDesc loopback */
#define _RTCL_RDMTS_HALF		(0 << 8)	/* Free Buffer Threshold is 1/2 of RDLEN */
#define _RTCL_RDMTS_QUARTER		(1 << 8)	/* Free Buffer Threshold is 1/4 of RDLEN */
#define _RTCL_RDMTS_EIGHTH		(2 << 8)	/* Free Buffer Threshold is 1/8 of RDLEN */
#define _RCTL_MO_36				(0 << 12)	/* Multicast Offset - bits 47:36 */
#define _RCTL_MO_35				(1 << 12)	/* Multicast Offset - bits 46:35 */
#define _RCTL_MO_34				(2 << 12)	/* Multicast Offset - bits 45:34 */
#define _RCTL_MO_32				(3 << 12)	/* Multicast Offset - bits 43:32 */
#define _RCTL_BAM				(1 << 15)	/* Broadcast Accept Mode */
#define _RCTL_VFE				(1 << 18)	/* VLAN Filter Enable */
#define _RCTL_CFIEN				(1 << 19)	/* Canonical Form Indicator Enable */
#define _RCTL_CFI				(1 << 20)	/* Canonical Form Indicator Bit Value */
#define _RCTL_DPF				(1 << 22)	/* Discard Pause Frames */
#define _RCTL_PMCF				(1 << 23)	/* Pass MAC Control Frames */
#define _RCTL_SECRC				(1 << 26)	/* Strip Ethernet CRC */

#define _RCTL_BSIZE_256			(3 << 16)
#define _RCTL_BSIZE_512			(2 << 16)
#define _RCTL_BSIZE_1024		(1 << 16)
#define _RCTL_BSIZE_2048		(0 << 16)
#define _RCTL_BSIZE_4096		((3 << 16) | (1 << 25))
#define _RCTL_BSIZE_8192		((2 << 16) | (1 << 25))
#define _RCTL_BSIZE_16384		((1 << 16) | (1 << 25))

#define _TCTL_EN				(1 << 1)	/* Transmit Enable */
#define _TCTL_PSP				(1 << 3)	/* Pad Short Packets */
#define _TCTL_CT_SHIFT			4			/* Collision Threshold */
#define _TCTL_COLD_SHIFT		12			/* Collision Distance */
#define _TCTL_SWXOFF			(1 << 22)	/* Software XOFF Transmission */
#define _TCTL_RTLC				(1 << 24)	/* Re-transmit on Late Collision */

#define _CMD_EOP				(1 << 0)	/* End of Packet */
#define _CMD_IFCS				(1 << 1)	/* Insert FCS */
#define _CMD_IC					(1 << 2)	/* Insert Checksum */
#define _CMD_RS					(1 << 3)	/* Report Status */
#define _CMD_RPS				(1 << 4)	/* Report Packet Sent */
#define _CMD_VLE				(1 << 6)	/* VLAN Packet Enable */
#define _CMD_IDE				(1 << 7)	/* Interrupt Delay Enable */

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
	uint32						index;			// 当为0xffffffff代表设备不存在。

	uint32						membase;		// NIC的MEM BASE。

	uint8						mac[6];			// MAC地址。

	int32						rx_buffer_idx;	// 储存着下一个可用的“接收缓冲区”的索引。
	int32						tx_buffer_idx;	// 储存着下一个可用的“发送缓冲区”的索引。

	E1000ReceiveDescriptorPtr	rx_desc;		// 指向第一个“接收缓冲区”的描述符。
	E1000TransmitDescriptorPtr	tx_desc;		// 指向第一个“发送缓冲区”的描述符。

	uint8 *						rx_buffer;		// 储存着“接收缓冲区”的地址，所有“接收缓冲区”均连续地分布在一块内存块中。
	uint8 *						tx_buffer;		// 储存着“发送缓冲区”的地址，所有“发送缓冲区”均连续地分布在一块内存块中。

	uint8						ip[4];			// IP地址，[0]=最高字节；...；[3]=最低字节。

	BOOL						has_eeprom;		// NIC是否有EEPROM。

	NetDevicePtr				netdev;

	BOOL		tx_lock;
} E1000Device, * E1000DevicePtr;

static E1000Device _devices[_MAX_COUNT];
static uint32 _count = 0;
static uint32 _pidx2didx[MAX_PERIPHERAL_COUNT];

/**
	@Function:		_E1000GetDevice
	@Access:		Private
	@Description:
		获取设备实例。
	@Parameters:
		index, uint32, IN
			设备索引。
	@Return:
		E1000DevicePtr
			指向设备实例的指针，返回NULL则设备不存在。
*/
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

static
uint16
_E1000EEPROMRead(
	IN uint32 index,
	IN uint8 addr)
{
	uint32 temp = 0;
	_E1000WriteCommand(index, _E1000_REG_EEPROM, 1 | ((uint32)addr << 8));
	while (!((temp = _E1000ReadCommand(index, _E1000_REG_EEPROM)) & (1 << 4)));
	return (uint16)((temp >> 16) & 0xFFFF);
}

static
void
_E1000Interrupt(
	IN uint32 peripheral,
	IN uint32 irq)
{
	uint32 devidx = _pidx2didx[peripheral];
	if (devidx != 0xffffffff)
	{
		E1000DevicePtr dev = _E1000GetDevice(devidx);
		if (dev != NULL)
		{
			uint32 status = _E1000ReadCommand(devidx, 0xc0);
			if (status & 0x04)
			{
				
			}
			else if (status & 0x10)
			{

			}
			else if (status & ((1 << 6) | (1 << 7)))
			{
				do
				{
					dev->rx_buffer_idx = _E1000ReadCommand(devidx, _E1000_REG_RXDESCTAIL);
					if (dev->rx_buffer_idx == (int32)_E1000ReadCommand(devidx, _E1000_REG_RXDESCHEAD))
					{
						break;
					}
					dev->rx_buffer_idx = (dev->rx_buffer_idx + 1) % _RX_BUFFER_COUNT;
					E1000ReceiveDescriptorPtr rx_desc = &dev->rx_desc[dev->rx_buffer_idx];
					if (rx_desc->status & 0x01)
					{
						uint8 * pbuf = (uint8 *)rx_desc->addr;
						uint16 plen = rx_desc->length;

						NetDevicePtr netdev = dev->netdev;
						if (netdev->ProcessPacket != NULL)
						{
							netdev->ProcessPacket(netdev, pbuf, plen - 4);
						}

						rx_desc->status = 0;

						_E1000WriteCommand(devidx, _E1000_REG_RXDESCTAIL, dev->rx_buffer_idx);
					}
					else
					{
						break;
					}
				} while (1);
			}
		}
	}
}

/**
	@Function:		E1000GetDeviceCount
	@Access:		Public
	@Description:
		获取设备的数量。
	@Parameters:
	@Return:
		uint32
			设备的数量。
*/
uint32
E1000GetDeviceCount(void)
{
	return _count;
}

/**
	@Function:		E1000SetMAC
	@Access:		Public
	@Description:
		设置设备的MAC地址。
	@Parameters:
		index, uint32, IN
			设备索引。
		mac, const uint8 *, IN
			设备的MAC地址，长度为6字节。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
BOOL
E1000SetMAC(
	IN uint32 index,
	IN const uint8 * mac)
{
	E1000DevicePtr device = _E1000GetDevice(index);
	if (device == NULL)
	{
		goto err;
	}

	if (mac == NULL)
	{
		goto err;
	}
	MemCopy(mac, device->mac, sizeof(device->mac));

	return TRUE;
err:
	return FALSE;
}

/**
	@Function:		E1000GetMAC
	@Access:		Public
	@Description:
		获取设备的MAC地址。
	@Parameters:
		index, uint32, IN
			设备索引。
	@Return:
		const uint8 *
			设备的MAC地址，长度为6字节。
*/
const uint8 *
E1000GetMAC(
	IN uint32 index)
{
	E1000DevicePtr device = _E1000GetDevice(index);
	if (device == NULL)
	{
		goto err;
	}

	return device->mac;
err:
	return NULL;
}

/**
	@Function:		E1000SetIP
	@Access:		Public
	@Description:
		设置设备的IP地址。
	@Parameters:
		index, uint32, IN
			设备索引。
		ip, const uint8 *, IN
			设备的IP地址，长度为4字节。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
BOOL
E1000SetIP(
	IN uint32 index,
	IN const uint8 * ip)
{
	E1000DevicePtr device = _E1000GetDevice(index);
	if (device == NULL)
	{
		goto err;
	}

	if (ip == NULL)
	{
		goto err;
	}
	MemCopy(ip, device->ip, sizeof(device->ip));

	return TRUE;
err:
	return FALSE;
}

/**
	@Function:		E1000GetIP
	@Access:		Public
	@Description:
		获取设备的IP地址。
	@Parameters:
		index, uint32, IN
			设备索引。
	@Return:
		const uint8 *
			设备的IP地址，长度为4字节。
*/
const uint8 *
E1000GetIP(
	IN uint32 index)
{
	E1000DevicePtr device = _E1000GetDevice(index);
	if (device == NULL)
	{
		goto err;
	}

	return device->ip;
err:
	return NULL;
}

/**
	@Function:		E1000GetName
	@Access:		Public
	@Description:
		获取设备的名称。
	@Parameters:
	@Return:
		CASCTEXT
			设备的名称。
*/
CASCTEXT
E1000GetName(void)
{
	return _NAME;
}

BOOL
E1000SendPacket(
	IN uint32 index,
	IN const void * packet,
	IN uint16 len)
{
	E1000DevicePtr device = _E1000GetDevice(index);
	if (device == NULL)
	{
		goto err;
	}

	device->tx_buffer_idx = _E1000ReadCommand(index, _E1000_REG_TXDESCTAIL);
	E1000TransmitDescriptorPtr tx_desc = &device->tx_desc[device->tx_buffer_idx];
	MemCopy(packet, tx_desc->addr, len);

	tx_desc->length = len;
	tx_desc->cmd = _CMD_EOP | _CMD_IFCS | _CMD_RS; //| _CMD_RPS;
	tx_desc->status = 0;

	device->tx_buffer_idx = (device->tx_buffer_idx + 1) % _TX_BUFFER_COUNT;
	_E1000WriteCommand(index, _E1000_REG_TXDESCTAIL, device->tx_buffer_idx);

	return TRUE;
err:
	return FALSE;
}

/**
	@Function:		_NetSetIP
	@Access:		Private
	@Description:
		E1000SetIP的包装函数。
	@Parameters:
		device, void *, IN
			实际类型为NetDevicePtr。
		ip, const uint8 *, IN
			见E1000SetIP。
	@Return:
		BOOL
			见E1000SetIP。
*/
static BOOL _NetSetIP(IN void * device, IN const uint8 * ip)
{
	NetDevicePtr netdev = (NetDevicePtr)device;
	return E1000SetIP(netdev->id, ip);
}

/**
	@Function:		_NetGetIP
	@Access:		Private
	@Description:
		E1000GetIP的包装函数。
	@Parameters:
		device, void *, IN
			实际类型为NetDevicePtr。
	@Return:
		const uint8 *
			见E1000GetIP。
*/
static const uint8 * _NetGetIP(IN void * device)
{
	NetDevicePtr netdev = (NetDevicePtr)device;
	return E1000GetIP(netdev->id);
}

/**
	@Function:		_NetSetMAC
	@Access:		Private
	@Description:
		E1000SetMAC的包装函数。
	@Parameters:
		device, void *, IN
			实际类型为NetDevicePtr。
		mac, const uint8 *, IN
			见E1000SetMAC。
	@Return:
		BOOL
			见E1000SetMAC。
*/
static BOOL _NetSetMAC(IN void * device, IN const uint8 * mac)
{
	NetDevicePtr netdev = (NetDevicePtr)device;
	return E1000SetMAC(netdev->id, mac);
}

/**
	@Function:		_NetGetMAC
	@Access:		Private
	@Description:
		E1000GetMAC的包装函数。
	@Parameters:
		device, void *, IN
			实际类型为NetDevicePtr。
	@Return:
		const uint8 *
			见E1000GetMAC。
*/
static const uint8 * _NetGetMAC(IN void * device)
{
	NetDevicePtr netdev = (NetDevicePtr)device;
	return E1000GetMAC(netdev->id);
}

/**
	@Function:		_NetGetName
	@Access:		Private
	@Description:
		E1000GetName的包装函数。
	@Parameters:
		device, void *, IN
			实际类型为NetDevicePtr。
	@Return:
		CASCTEXT
			见E1000GetName。
*/
static CASCTEXT _NetGetName(IN void * device)
{
	return E1000GetName();
}

/**
	@Function:		_NetSendPacket
	@Access:		Private
	@Description:
		E1000SendPacket的包装函数。
	@Parameters:
		device, void *, IN
			实际类型为NetDevicePtr。
		packet, const void *, IN
			见E1000SendPacket。
		len, uint16, IN
			见E1000SendPacket。
	@Return:
		BOOL
			见E1000SendPacket。
*/
static BOOL _NetSendPacket(IN void * device, IN const void * packet, IN uint16 len)
{
	NetDevicePtr netdev = (NetDevicePtr)device;
	return E1000SendPacket(netdev->id, packet, len);
}

void
E1000Init(void)
{
	uint32 i, j;
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
		uint32 id = dev->header->cfg_hdr.id;
		if (dev != NULL
			&& (id == _ID0 || id == _ID1 || id == _ID2))
		{
			uint32 devidx = _count++;
			E1000DevicePtr device = &_devices[devidx];
			device->index = i;
			device->membase = dev->header->cfg_hdr.bars[0];
			device->tx_lock = FALSE;

			// 构造NetDevice对象。
			NetDevicePtr netdev = NEW(NetDevice);
			MemClear(netdev, 0, sizeof(NetDevice));
			netdev->id = devidx;
			netdev->SetIP = _NetSetIP;
			netdev->GetIP = _NetGetIP;
			netdev->SetMAC = _NetSetMAC;
			netdev->GetMAC = _NetGetMAC;
			netdev->GetName = _NetGetName;
			netdev->SendPacket = _NetSendPacket;
			NetAdd(netdev);
			device->netdev = netdev;

			device->rx_buffer = (uint8 *)MemAlloc(_BUFFER_SIZE * _RX_BUFFER_COUNT);
			device->rx_desc = (E1000ReceiveDescriptorPtr)MemAlloc(sizeof(E1000ReceiveDescriptor) * _RX_BUFFER_COUNT);
			MemClear(device->rx_desc, 0, sizeof(E1000ReceiveDescriptor) * _RX_BUFFER_COUNT);
			for (j = 0; j < _RX_BUFFER_COUNT; j++)
			{
				E1000ReceiveDescriptorPtr rx_desc = &device->rx_desc[j];
				rx_desc->addr = device->rx_buffer + _BUFFER_SIZE * j;
				rx_desc->status = 0;
			}

			device->tx_buffer = (uint8 *)MemAlloc(_BUFFER_SIZE * _TX_BUFFER_COUNT);
			device->tx_desc = (E1000TransmitDescriptorPtr)MemAlloc(sizeof(E1000TransmitDescriptor) * _TX_BUFFER_COUNT);
			MemClear(device->tx_desc, 0, sizeof(E1000TransmitDescriptor) * _TX_BUFFER_COUNT);
			for (j = 0; j < _TX_BUFFER_COUNT; j++)
			{
				E1000TransmitDescriptorPtr tx_desc = &device->tx_desc[j];
				tx_desc->addr = device->tx_desc + _BUFFER_SIZE * j;
				tx_desc->status = 0;
				tx_desc->cmd = (1 << 0);
			}

			// 开启IO端口以及Bus Mastering。
			uint32 conf = PciReadCfgRegValue(i, 0x04);
			conf |= (1 << 2);
			conf |= (1 << 0);
			PciWriteCfgRegValue(i, 0x04, conf);

			// 探测EEPROM。
			device->has_eeprom = FALSE;
			_E1000WriteCommand(devidx, _E1000_REG_EEPROM, 1);
			for (j = 0; j < 100000 && !device->has_eeprom; j++)
			{
				uint32 value = _E1000ReadCommand(devidx, _E1000_REG_EEPROM);
				if (value & 0x10)
				{
					device->has_eeprom = TRUE;
				}
			}

			// 获取MAC地址。
			if (device->has_eeprom)
			{
				uint32 t;
				t = _E1000EEPROMRead(devidx, 0);
				device->mac[0] = t & 0xFF;
				device->mac[1] = t >> 8;
				t = _E1000EEPROMRead(devidx, 1);
				device->mac[2] = t & 0xFF;
				device->mac[3] = t >> 8;
				t = _E1000EEPROMRead(devidx, 2);
				device->mac[4] = t & 0xFF;
				device->mac[5] = t >> 8;
			}
			else
			{
				uint8 * mac_addr = (uint8 *)(device->membase + 0x5400);
				for (j = 0; j < 6; j++)
				{
					device->mac[i] = mac_addr[i];
				}
			}

			uint32 status = _E1000ReadCommand(devidx, _E1000_REG_CTRL);
			status |= (1 << 5);		/* set auto speed detection */
			status |= (1 << 6);		/* set link up */
			status &= ~(1 << 3);	/* unset link reset */
			status &= ~(1 << 31);	/* unset phy reset */
			status &= ~(1 << 7);	/* unset invert loss-of-signal */
			_E1000WriteCommand(devidx, _E1000_REG_CTRL, status);

			/* Disables flow control */
			_E1000WriteCommand(devidx, 0x0028, 0);
			_E1000WriteCommand(devidx, 0x002c, 0);
			_E1000WriteCommand(devidx, 0x0030, 0);
			_E1000WriteCommand(devidx, 0x0170, 0);

			/* Unset flow control */
			status = _E1000ReadCommand(devidx, _E1000_REG_CTRL);
			status &= ~(1 << 30);
			_E1000WriteCommand(devidx, _E1000_REG_CTRL, status);
			
			// 设置中断处理程序。
			uint32 irq = dev->header->cfg_hdr.intr & 0xff;
			uint32 pindex = KnlConvertIrqToPeripheralIndex(irq);
			_pidx2didx[pindex] = devidx;
			KnlSetPeripheralInterrupt(pindex, _E1000Interrupt);

			for (j = 0; j < 128; ++j)
			{
				_E1000WriteCommand(devidx, 0x5200 + j * 4, 0);
			}

			for (j = 0; j < 64; ++j)
			{
				_E1000WriteCommand(devidx, 0x4000 + i * 4, 0);
			}

			_E1000WriteCommand(devidx, _E1000_REG_RCTRL, (1 << 4));

			// 初始化RX。
			_E1000WriteCommand(devidx, _E1000_REG_RXDESCLO, device->rx_desc);
			_E1000WriteCommand(devidx, _E1000_REG_RXDESCHI, 0);
			_E1000WriteCommand(devidx, _E1000_REG_RXDESCLEN, _RX_BUFFER_COUNT * sizeof(E1000ReceiveDescriptor));
			_E1000WriteCommand(devidx, _E1000_REG_RXDESCHEAD, 0);
			_E1000WriteCommand(devidx, _E1000_REG_RXDESCTAIL, _RX_BUFFER_COUNT - 1);
			device->rx_buffer_idx = 0;
			_E1000WriteCommand(
				devidx,
				_E1000_REG_RCTRL,
				_RCTL_EN | (_E1000ReadCommand(devidx, _E1000_REG_RCTRL) & (~((1 << 17) | (1 << 16)))));

			// 初始化TX。
			_E1000WriteCommand(devidx, _E1000_REG_TXDESCLO, device->tx_desc);
			_E1000WriteCommand(devidx, _E1000_REG_TXDESCHI, 0);
			_E1000WriteCommand(devidx, _E1000_REG_TXDESCLEN, _TX_BUFFER_COUNT * sizeof(E1000TransmitDescriptor));
			_E1000WriteCommand(devidx, _E1000_REG_TXDESCHEAD, 0);
			_E1000WriteCommand(devidx, _E1000_REG_TXDESCTAIL, 0);
			device->tx_buffer_idx = 0;
			_E1000WriteCommand(
				devidx,
				_E1000_REG_TCTRL,
				_TCTL_EN | _TCTL_PSP | _E1000ReadCommand(devidx, _E1000_REG_TCTRL));

			/* Twiddle interrupts */
			_E1000WriteCommand(devidx, 0x00D0, 0xFF);
			_E1000WriteCommand(devidx, 0x00D8, 0xFF);
			_E1000WriteCommand(devidx, 0x00D0, (1 << 2) | (1 << 6) | (1 << 7) | (1 << 1) | (1 << 0));

			int32 link_is_up = _E1000ReadCommand(devidx, _E1000_REG_STATUS) & (1 << 1);

		}
	}
}
