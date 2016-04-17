/**
	@File:			ahci.c
	@Author:		Ihsoh
	@Date:			2015-04-24
	@Description:
		AHCI。
*/

#include "ahci.h"
#include "types.h"

#include "pci/pci.h"

#include "memory.h"
#include "cmlock.h"
#include "tasks.h"

#define SATA_SIG_ATA			0x00000101  // SATA drive
#define SATA_SIG_ATAPI			0xEB140101  // SATAPI drive
#define SATA_SIG_SEMB			0xC33C0101  // Enclosure management bridge
#define SATA_SIG_PM				0x96690101  // Port multiplier
#define AHCI_DEV_NULL			0
#define AHCI_DEV_SATA			1
#define AHCI_DEV_SATAPI			4
#define AHCI_DEV_SEMB			2
#define AHCI_DEV_PM				3
#define HBA_PORT_DET_PRESENT	3
#define HBA_PORT_IPM_ACTIVE		1
#define AHCI_BASE				(_ahci_base)
#define HBA_PxCMD_CR            (1 << 15) /* CR - Command list Running */
#define HBA_PxCMD_FR            (1 << 14) /* FR - FIS receive Running */
#define HBA_PxCMD_FRE           (1 <<  4) /* FRE - FIS Receive Enable */
#define HBA_PxCMD_SUD           (1 <<  1) /* SUD - Spin-Up Device */
#define HBA_PxCMD_ST            (1 <<  0) /* ST - Start (command processing) */

#define ATA_DEV_BUSY			0x80
#define ATA_DEV_DRQ				0x08
#define HBA_PxIS_TFES			(1 << 30)       /* TFES - Task File Error Status */
#define ATA_CMD_READ_DMA_EX		0x25
#define ATA_CMD_WRITE_DMA_EX	0x35

#define	ATA_ID_SIZE				(256 * sizeof(uint16))		// IDENTIFY结果的大小。

#define	ATA_ID_LBA0				100							// 最大LBA48地址（LSW）。
#define	ATA_ID_LBA1				101							// 最大LBA48地址。
#define	ATA_ID_LBA2				102							// 最大LBA48地址。
#define	ATA_ID_LBA3				103							// 最大LBA48地址（MSW）。	

#define	ATA_ID_LSS0				118							// 逻辑扇区大小（LSW）。
#define	ATA_ID_LSS1				119							// 逻辑扇区大小（MSW）。

#define	ATA_CMD_IDENTIFY		0xec		

#define	AHCI_FIS_SIZE			256				// FIS接受缓冲区的大小。
#define	AHCI_CL_SIZE			1024			// 命令列表缓冲区的大小。
#define	AHCI_TMP_SIZE			ATA_ID_SIZE		// 临时储存缓冲区的大小。
#define	AHCI_TMP_ALIGN			2				// 临时储存缓冲区的对齐。

#define	_MAX_DEVICE_COUNT	4
#define	_MAX_PORT_COUNT		16

typedef struct
{
	HBA_PORT *	port;
	HBA_MEM *	abar;
} AHCIPort, * AHCIPortPtr;

static uint32 _devices[_MAX_DEVICE_COUNT];
static uint32 _devcnt = 0;
static AHCIPort _ports[_MAX_PORT_COUNT];
static uint32 _prtcnt = 0;
static void * _ahci_base = NULL;

static
HBA_MEM *
_AhciGetAbarByPort(IN HBA_PORT * port);

static
void
_AhciCheckAllDevices(void);

static
int32
_AhciCheckType(IN HBA_PORT * port);

static
void
_AhciStartCommand(IN OUT HBA_PORT * port);

static
void
_AhciStopCommand(IN OUT HBA_PORT * port);

static
void
_AhciRebasePort(IN OUT HBA_PORT * port,
				IN uint32 portno);

static
void
_AhciProbePort(void);

static
int32
_AhciFindCommandSlot(IN HBA_PORT * port);

#define	MAX_RETRY		0x100
#define	MAX_LOCK_RETRY	0x00100000

#define	_LOCK_TID_NONE		-2	// 没有任何任务获取到锁。
#define	_LOCK_TID_KERNEL	-1	// 内核获取了锁。

// 如果为TRUE则说明当前有任务占用AHCI驱动。
static BOOL _lock = FALSE;

// 指示哪个任务正在占用AHCI驱动。
static int32 _lock_tid	= _LOCK_TID_NONE;

/**
	@Function:		_AhciLock
	@Access:		Private
	@Description:
		锁住AHCI驱动。
	@Parameters:
	@Return:
		BOOL
			返回TRUE则获取锁成功，否则获取锁失败。
*/
static
BOOL
_AhciLock(void)
{
	// 在规定重试次数内尝试获取锁。
	int32 i;
	for(i = 0; i < MAX_LOCK_RETRY; i++)
	{
		if(!_lock)
			break;
		int32 i1;
		for(i1 = 0; i1 < 0x1000; i1++)
			asm volatile ("pause");
	}

	// 检测是否获取到锁，如果未获取则返回FALSE。
	if(_lock)
		return FALSE;

	common_lock();
	_lock = TRUE;
	_lock_tid = get_running_tid();
	common_unlock();
	return TRUE;
}

/**
	@Function:		_AhciUnlock
	@Access:		Private
	@Description:
		解锁AHCI驱动。
	@Parameters:
	@Return:
*/
static
void
_AhciUnlock(void)
{
	_lock = FALSE;
	_lock_tid = _LOCK_TID_NONE;
}

/**
	@Function:		AhciIsLocked
	@Access:		Public
	@Description:
		获取AHCI驱动的锁的状态。
	@Parameters:
	@Return:
		BOOL
			返回TRUE则代表AHCI驱动的锁被锁定中。
*/
BOOL
AhciIsLocked(void)
{
	return _lock;
}

/**
	@Function:		AhciGetLockTid
	@Access:		Public
	@Description:
		获取正在占用AHCI驱动的任务ID。
	@Parameters:
	@Return:
		int32
			正在占用AHCI驱动的任务ID。
*/
int32
AhciGetLockTid(void)
{
	return _lock_tid;
}

/**
	@Function:		AhciAttemptToUnlock
	@Access:		Public
	@Description:
		尝试解除AHCI驱动的锁。
	@Parameters:
		tid, int32, IN
			任务ID。如果该值等于当前正在占用AHCI驱动的锁的任务ID，则解除锁。
	@Return:
*/
void
AhciAttemptToUnlock(IN int32 tid)
{
	if(tid == _lock_tid)
		_AhciUnlock();
}

/**
	@Function:		AhciGetPortCount
	@Access:		Public
	@Description:
		获取AHCI端口的数量。
	@Parameters:
	@Return:
		uint32
			AHCI端口的数量。
*/
uint32
AhciGetPortCount(void)
{
	return _prtcnt;
}

/**
	@Function:		AhciGetPort
	@Access:		Public
	@Description:
		通过索引获取AHCI端口。
	@Parameters:
		index, uint32, IN
			索引。
	@Return:
		HBA_PORT *
			指向AHCI端口对象的指针。
*/
HBA_PORT *
AhciGetPort(IN uint32 index)
{
	if(index >= _prtcnt)
		return NULL;
	return _ports[index].port;
}

/**
	@Function:		_AhciGetAbarByPort
	@Access:		Private
	@Description:
		获取AHCI端口的AHCI Base Address。
	@Parameters:
		port, HBA_PORT *, IN
			AHCI端口。
	@Return:
		HBA_MEM *
			AHCI端口的AHCI Base Address。
*/
static
HBA_MEM *
_AhciGetAbarByPort(IN HBA_PORT * port)
{
	uint32 ui;
	for(ui = 0; ui < _prtcnt; ui++)
		if(_ports[ui].port == port)
			return _ports[ui].abar;
	return NULL;
}

/**
	@Function:		_AhciCheckAllDevices
	@Access:		Private
	@Description:
		获取所有AHCI设备。
	@Parameters:
	@Return:
*/
static
void
_AhciCheckAllDevices(void)
{
	_devcnt = 0;
	uint32 count = PciGetDeviceCount();
	uint32 index;
	for(index = 0; index < count; index++)
	{
		PCIDeviceInfoPtr device = PciGetDevice(index);
		if(	device != NULL
			&& device->header->cfg_hdr.cc[2] == PCI_CLS_MASS_STORAGE_CONTROLLER
			&& device->header->cfg_hdr.cc[1] == 0x06
			&& device->header->cfg_hdr.cc[0] == 0x01)	// Serial ATA (AHCI 1.0)。
			_devices[_devcnt++] = index;
	}
}

/**
	@Function:		_AhciCheckType
	@Access:		Private
	@Description:
		检查AHCI设备的类型。
	@Parameters:
		port, HBA_PORT *, IN
			AHCI端口。
	@Return:
		AHCI_DEV_NULL、AHCI_DEV_SATAPI、AHCI_DEV_SEMB、AHCI_DEV_PM、AHCI_DEV_SATA。
*/
static
int32
_AhciCheckType(IN HBA_PORT * port)
{
	DWORD ssts = port->ssts;
	BYTE ipm = (ssts >> 8) & 0x0f;
	BYTE det = ssts & 0x0f;

	if(det != HBA_PORT_DET_PRESENT)
		return AHCI_DEV_NULL;
	if(ipm != HBA_PORT_IPM_ACTIVE)
		return AHCI_DEV_NULL;

	switch(port->sig)
	{
		case SATA_SIG_ATAPI:
			return AHCI_DEV_SATAPI;
		case SATA_SIG_SEMB:
			return AHCI_DEV_SEMB;
		case SATA_SIG_PM:
			return AHCI_DEV_PM;
		default:
			return AHCI_DEV_SATA;
	}

	return AHCI_DEV_NULL;
}

/**
	@Function:		_AhciStartCommand
	@Access:		Private
	@Description:
		启动命令执行引擎。
	@Parameters:
		port, HBA_PORT *, IN OUT
			AHCI端口。
	@Return:
*/
static
void
_AhciStartCommand(IN OUT HBA_PORT * port)
{
	// 等待CR被清除。如果未被清除，则代表命令列表运行中。
	while (port->cmd & HBA_PxCMD_CR);

	// 开启接受FIS（FRE）以及开始命令的执行（ST）。
	port->cmd |= HBA_PxCMD_FRE;
	port->cmd |= HBA_PxCMD_ST; 
}

/**
	@Function:		_AhciStopCommand
	@Access:		Private
	@Description:
		停止命令执行引擎。
	@Parameters:
		port, HBA_PORT *, IN OUT
			AHCI端口。
	@Return:
*/
static
void
_AhciStopCommand(IN OUT HBA_PORT * port)
{
	// 清除ST，关闭命令执行。
	port->cmd &= ~HBA_PxCMD_ST;

	// 清除FRE，不允许接受FIS。
	port->cmd &= ~HBA_PxCMD_FRE;
 
	// 等待FR和CR被清除。
	while(1)
	{
		if (port->cmd & HBA_PxCMD_FR)
			continue;
		if (port->cmd & HBA_PxCMD_CR)
			continue;
		break;
	}
}

/**
	@Function:		_AhciGetSectorCount
	@Access:		Private
	@Description:
		获取扇区数。
	@Parameters:
		port, HBA_PORT *, IN OUT
			AHCI端口。
	@Return:
		uint64
			扇区数。
*/
static
uint64
_AhciGetSectorCount(IN OUT HBA_PORT * port)
{
	// 用于保存ATA_CMD_IDENTIFY命令的结果。
	uint8 id_buf[ATA_ID_SIZE];
	memset(id_buf, 0, ATA_ID_SIZE);
	uint16 * buf = (uint16 *)id_buf;

	// 获取空闲的命令槽并初始化命令头。
	int32 slot = _AhciFindCommandSlot(port);
	HBA_CMD_HEADER * cmdheader = (HBA_CMD_HEADER *)port->clb;
	cmdheader += slot;
	cmdheader->cfl = sizeof(FIS_REG_H2D) / sizeof(DWORD);
	cmdheader->w = 0;
	cmdheader->c = 1;
	cmdheader->p = 1;
	cmdheader->prdtl = 1;

	HBA_CMD_TBL * cmdtbl = (HBA_CMD_TBL *)cmdheader->ctba;
	memset(	cmdtbl,
			0,
			sizeof(HBA_CMD_TBL) + (cmdheader->prdtl - 1) * sizeof(HBA_PRDT_ENTRY));

	// 设置第一个PRD指向用于保存ATA_CMD_IDENTIFY命令的结果的缓冲区。
	cmdtbl->prdt_entry[0].dba = (DWORD)id_buf;
	cmdtbl->prdt_entry[0].dbau = 0;
	cmdtbl->prdt_entry[0].dbc = ATA_ID_SIZE;
	cmdtbl->prdt_entry[0].i = 0;

	// 设置FIS，命令为ATA_CMD_IDENTIFY。
	FIS_REG_H2D * cmdfis = (FIS_REG_H2D *)&cmdtbl->cfis;
	cmdfis->fis_type = FIS_TYPE_REG_H2D;
	cmdfis->c = 1;
	cmdfis->command = ATA_CMD_IDENTIFY;

	// 如果AHCI端口处于忙状态或DMA请求状态，则等待这些状态被清除。
	uint32 spin = 0;
	while((port->tfd & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && spin < 1000000)
		spin++;
	if(spin == 1000000)
		return FALSE;

	// 发出命令。
	port->ci = 1 << slot;

	// 等待命令执行结束。
	while(1)
	{
		if((port->ci & (1 << slot)) == 0)
			break;
		if(port->is & HBA_PxIS_TFES)
			return FALSE; 
	}
	if(port->is & HBA_PxIS_TFES)
		return FALSE;
	while(port->ci != 0);

	// 从命令执行结果中获取扇区数。
	uint64 sector_count = ((uint64)buf[ATA_ID_LBA3] << 48)
						| ((uint64)buf[ATA_ID_LBA2] << 32)
						| ((uint64)buf[ATA_ID_LBA1] << 16)
						| (uint64)buf[ATA_ID_LBA0];

	return sector_count;	
}

/**
	@Function:		AhciGetSectorCount
	@Access:		Public
	@Description:
		获取扇区数。
	@Parameters:
		port, uint32, IN
			AHCI端口号。
	@Return:
		uint64
			扇区数。
*/
uint64
AhciGetSectorCount(IN uint32 portno)
{
	HBA_PORT * port = AhciGetPort(portno);
	if(port == NULL)
		return 0;
	if(!_AhciLock())
		return 0;
	uint32 sector_count = _AhciGetSectorCount(port);
	_AhciUnlock();
	return sector_count;
}

/**
	@Function:		_AhciRebasePort
	@Access:		Private
	@Description:
		重置AHCI端口的地址。
	@Parameters:
		port, HBA_PORT *, IN OUT
			AHCI端口。
		portno, uint32, IN
			端口号。
	@Return:
*/
static
void
_AhciRebasePort(	IN OUT HBA_PORT * port,
				IN uint32 portno)
{
	_AhciStopCommand(port);	// Stop command engine
 
	uint32 fis_off, tmp_off;
	
	fis_off = AHCI_CL_SIZE + AHCI_FIS_SIZE - 1;
	fis_off -= fis_off % AHCI_FIS_SIZE;

	tmp_off = fis_off + AHCI_FIS_SIZE + AHCI_TMP_ALIGN - 1;
	tmp_off -= tmp_off % AHCI_TMP_ALIGN;

	// Command list offset: 1K*portno
	// Command list entry size = 32
	// Command list entry maxim count = 32
	// Command list maxim size = 32*32 = 1K per port
	port->clb = AHCI_BASE + (portno << 10);
	port->clbu = 0;
	memset((void *)(port->clb), 0, 1024);
 
	// FIS offset: 32K+256*portno
	// FIS entry size = 256 bytes per port
	port->fb = AHCI_BASE + (32 << 10) + (portno << 8);
	port->fbu = 0;
	memset((void *)(port->fb), 0, 256);
 
	// Command table offset: 40K + 8K*portno
	// Command table size = 256*32 = 8K per port
	HBA_CMD_HEADER * cmdheader = (HBA_CMD_HEADER *)(port->clb);
	uint32 ui;
	for(ui = 0; ui < 32; ui++)
	{
		cmdheader[ui].prdtl = 8;	// 8 prdt entries per command table
									// 256 bytes per command table, 64+16+48+16*8
									// Command table offset: 40K + 8K*portno + cmdheader_index*256
		cmdheader[ui].ctba = AHCI_BASE + (40 << 10) + (portno << 13) + (ui << 8);
		cmdheader[ui].ctbau = 0;
		memset((void *)cmdheader[ui].ctba, 0, 256);
	}
 
	_AhciStartCommand(port);	// Start command engine
}

/**
	@Function:		_AhciProbePort
	@Access:		Private
	@Description:
		探测AHCI端口。
	@Parameters:
	@Return:
*/
static
void
_AhciProbePort(void)
{
	uint32 devidx;
	for(devidx = 0; devidx < _devcnt; devidx++)
	{
		PCIDeviceInfoPtr device = PciGetDevice(_devices[devidx]);
		if(device == NULL)
			return;
		HBA_MEM * abar = (HBA_MEM *)device->header->cfg_hdr.abar;
		DWORD pi = abar->pi;
		uint32 ui;
		for(ui = 0; ui < 32; ui++)
			if((pi >> ui) & 0x1)
			{
				int32 dt = _AhciCheckType((HBA_PORT *)&abar->ports[ui]);
				switch(dt)
				{
					case AHCI_DEV_SATA:
					{
						AHCIPortPtr port = &_ports[_prtcnt++];
						port->abar = abar;
						port->port = (HBA_PORT *)&abar->ports[ui];
						_AhciRebasePort(port->port, ui);
						if(_prtcnt == _MAX_PORT_COUNT)
							return;
						break;
					}
					case AHCI_DEV_SATAPI:
					{
						break;
					}
					case AHCI_DEV_SEMB:
					{
						break;
					}
					case AHCI_DEV_PM:
					{
						break;
					}
				}
			}
	}
}

/**
	@Function:		_AhciFindCommandSlot
	@Access:		Private
	@Description:
		查找AHCI端口可用的命令槽。
	@Parameters:
		port, HBA_PORT *, IN
			AHCI设备的端口。
	@Return:
		int32
			返回值的范围为0 ~ 31，代表AHCI端口可用的命令槽的索引。
			如果返回-1则代表没有可用的命令槽。
*/
static
int32
_AhciFindCommandSlot(IN HBA_PORT * port)
{
	// 如果SACT和CI都没有被设置，则代表该槽为空闲。
	DWORD slots = (port->sact | port->ci);

	// 获取AHCI端口支持的命令槽数量。
	HBA_MEM * abar = _AhciGetAbarByPort(port);
	int32 n = (abar->cap & 0x0f00) >> 8;

	// 查找可用的命令槽（SACT和CI都没有被设置）。
	int32 i;
	for(i = 0; i < n; i++)
	{
		if ((slots & 1) == 0)
			return i;
		slots >>= 1;
	}

	return -1;
}

/**
	@Function:		_AhciRead
	@Access:		Private
	@Description:
		读AHCI设备。
	@Parameters:
		port, HBA_PORT *, IN OUT
			AHCI设备的端口。
		startl, DWORD, IN
			LBA地址的低32位地址。
		starth, DWORD, IN
			LBA地址的高32位地址。
		buf, WORD *, OUT
			指向保存数据的缓冲区的指针。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
static
BOOL
_AhciRead(	IN OUT HBA_PORT * port,
			IN DWORD startl,
			IN DWORD starth,
			IN DWORD count,
			OUT WORD * buf)
{
	port->is = (DWORD)-1;
	int32 spin = 0;
	int32 slot = _AhciFindCommandSlot(port);
	if(slot == -1)
		return FALSE;

	HBA_CMD_HEADER * cmdheader = (HBA_CMD_HEADER *)port->clb;
	cmdheader += slot;
	cmdheader->cfl = sizeof(FIS_REG_H2D) / sizeof(DWORD);
	cmdheader->w = 0;
	cmdheader->c = 1;
	cmdheader->p = 1;
	cmdheader->prdtl = (WORD)((count - 1) >> 4) + 1;

	HBA_CMD_TBL * cmdtbl = (HBA_CMD_TBL *)cmdheader->ctba;
	memset(	cmdtbl,
			0,
			sizeof(HBA_CMD_TBL) + cmdheader->prdtl * sizeof(HBA_PRDT_ENTRY));

	int32 i;
	DWORD c = count;
	for(i = 0; i < cmdheader->prdtl - 1; i++)
	{
		cmdtbl->prdt_entry[i].dba = (DWORD)buf;
		cmdtbl->prdt_entry[i].dbau = 0;
		cmdtbl->prdt_entry[i].dbc = 8 * 1024 - 1;	// 长度是Zero-based。
		cmdtbl->prdt_entry[i].i = 0;
		buf += 4 * 1024;
		c -= 16;
	}

	if((c << 9) != 0)
	{
		cmdtbl->prdt_entry[i].dba = (DWORD)buf;
		cmdtbl->prdt_entry[i].dbau = 0;
		cmdtbl->prdt_entry[i].dbc = (c << 9) - 1;	// 长度是Zero-based。
		cmdtbl->prdt_entry[i].i = 0;
	}

	FIS_REG_H2D * cmdfis = (FIS_REG_H2D *)&cmdtbl->cfis;

	cmdfis->fis_type = FIS_TYPE_REG_H2D;
	cmdfis->c = 1;
	cmdfis->command = ATA_CMD_READ_DMA_EX;

	cmdfis->lba0 = (BYTE)startl;
	cmdfis->lba1 = (BYTE)(startl >> 8);
	cmdfis->lba2 = (BYTE)(startl >> 16);
	cmdfis->device = 1 << 6;

	cmdfis->lba3 = (BYTE)(startl >> 24);
	cmdfis->lba4 = (BYTE)starth;
	cmdfis->lba5 = (BYTE)(starth >> 8);

	cmdfis->countl = (BYTE)count;
	cmdfis->counth = (BYTE)(count >> 8);

	while((port->tfd & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && spin < 1000000)
		spin++;
	if(spin == 1000000)
		return FALSE;

	port->ci = 1 << slot;

	while(1)
	{
		if((port->ci & (1 << slot)) == 0)
			break;
		if(port->is & HBA_PxIS_TFES)
			return FALSE; 
	}

	if(port->is & HBA_PxIS_TFES)
		return FALSE;

	while(port->ci != 0);

	return TRUE;
}

/**
	@Function:		AhciRead
	@Access:		Public
	@Description:
		读AHCI设备。
	@Parameters:
		portno, uint32, IN
			AHCI设备的端口号。
		startl, DWORD, IN
			LBA地址的低32位地址。
		starth, DWORD, IN
			LBA地址的高32位地址。
		buf, WORD *, OUT
			指向保存数据的缓冲区的指针。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
BOOL
AhciRead(	IN uint32 portno,
			IN DWORD startl,
			IN DWORD starth,
			IN DWORD count,
			OUT WORD * buf)
{
	HBA_PORT * port = AhciGetPort(portno);
	if(port == NULL)
		return FALSE;
	if(!_AhciLock())
		return FALSE;
	uint32 ui;
	for(ui = 0; ui < MAX_RETRY; ui++)
	{
		BOOL r = _AhciRead(port, startl, starth, count, buf);
		if(r)
		{
			_AhciUnlock();
			return TRUE;
		}
	}
	_AhciUnlock();
	return FALSE;
}

/**
	@Function:		_AhciWrite
	@Access:		Private
	@Description:
		写AHCI设备。
	@Parameters:
		port, HBA_PORT *, IN OUT
			AHCI设备的端口。
		startl, DWORD, IN
			LBA地址的低32位地址。
		starth, DWORD, IN
			LBA地址的高32位地址。
		buf, WORD *, IN
			指向保存数据的缓冲区的指针。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
static
BOOL
_AhciWrite(	IN OUT HBA_PORT * port,
			IN DWORD startl,
			IN DWORD starth,
			IN DWORD count,
			IN WORD * buf)
{
	port->is = (DWORD)-1;
	int32 spin = 0;
	int32 slot = _AhciFindCommandSlot(port);
	if(slot == -1)
		return FALSE;

	HBA_CMD_HEADER * cmdheader = (HBA_CMD_HEADER *)port->clb;
	cmdheader += slot;
	cmdheader->cfl = sizeof(FIS_REG_H2D) / sizeof(DWORD);
	cmdheader->w = 1;
	cmdheader->c = 1;
	cmdheader->p = 1;
	cmdheader->prdtl = (WORD)((count - 1) >> 4) + 1;

	HBA_CMD_TBL * cmdtbl = (HBA_CMD_TBL *)cmdheader->ctba;
	memset(	cmdtbl,
			0,
			sizeof(HBA_CMD_TBL) + cmdheader->prdtl * sizeof(HBA_PRDT_ENTRY));

	int32 i;
	DWORD c = count;
	for(i = 0; i < cmdheader->prdtl - 1; i++)
	{
		cmdtbl->prdt_entry[i].dba = (DWORD)buf;
		cmdtbl->prdt_entry[i].dbau = 0;
		cmdtbl->prdt_entry[i].dbc = 8 * 1024 - 1;	// 长度是Zero-based。
		cmdtbl->prdt_entry[i].i = 0;
		buf += 4 * 1024;
		c -= 16;
	}

	if((c << 9) != 0)
	{
		cmdtbl->prdt_entry[i].dba = (DWORD)buf;
		cmdtbl->prdt_entry[i].dbau = 0;
		cmdtbl->prdt_entry[i].dbc = (c << 9) - 1;	// 长度是Zero-based。
		cmdtbl->prdt_entry[i].i = 0;
	}

	FIS_REG_H2D * cmdfis = (FIS_REG_H2D *)&cmdtbl->cfis;

	cmdfis->fis_type = FIS_TYPE_REG_H2D;
	cmdfis->c = 1;
	cmdfis->command = ATA_CMD_WRITE_DMA_EX;

	cmdfis->lba0 = (BYTE)startl;
	cmdfis->lba1 = (BYTE)(startl >> 8);
	cmdfis->lba2 = (BYTE)(startl >> 16);
	cmdfis->device = 1 << 6;

	cmdfis->lba3 = (BYTE)(startl >> 24);
	cmdfis->lba4 = (BYTE)starth;
	cmdfis->lba5 = (BYTE)(starth >> 8);

	cmdfis->countl = (BYTE)count;
	cmdfis->counth = (BYTE)(count >> 8);

	while((port->tfd & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && spin < 1000000)
		spin++;
	if(spin == 1000000)
		return FALSE;

	port->ci = 1 << slot;

	while(1)
	{
		if((port->ci & (1 << slot)) == 0)
			break;
		if(port->is & HBA_PxIS_TFES)
			return FALSE; 
	}

	if(port->is & HBA_PxIS_TFES)
		return FALSE;

	while(port->ci != 0);

	return TRUE;
}

/**
	@Function:		AhciWrite
	@Access:		Public
	@Description:
		写AHCI设备。
	@Parameters:
		portno, uint32, IN
			AHCI设备的端口号。
		startl, DWORD, IN
			LBA地址的低32位地址。
		starth, DWORD, IN
			LBA地址的高32位地址。
		buf, WORD *, IN
			指向保存数据的缓冲区的指针。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
BOOL
AhciWrite(	IN uint32 portno,
			IN DWORD startl,
			IN DWORD starth,
			IN DWORD count,
			IN WORD * buf)
{
	HBA_PORT * port = AhciGetPort(portno);
	if(port == NULL)
		return FALSE;
	if(!_AhciLock())
		return FALSE;
	uint32 ui;
	for(ui = 0; ui < MAX_RETRY; ui++)
	{
		BOOL r = _AhciWrite(port, startl, starth, count, buf);
		if(r)
		{
			_AhciUnlock();
			return TRUE;
		}
	}
	_AhciUnlock();
	return FALSE;
}

/**
	@Function:		AhciInit
	@Access:		Public
	@Description:
		初始化AHCI。
	@Parameters:
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
BOOL
AhciInit(void)
{
	_ahci_base = alloc_memory(MB(1));
	if(_ahci_base == NULL)
		return NULL;
	_AhciCheckAllDevices();
	_AhciProbePort();
	return TRUE;
}
