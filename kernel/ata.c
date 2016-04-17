/**
	@File:			ata.c
	@Author:		Ihsoh
	@Date:			2016-04-04
	@Description:
		提供支持ATA协议的硬盘的操作功能。
*/

#include "ata.h"
#include "types.h"
#include "386.h"
#include "atapi.h"
#include "cmlock.h"
#include "tasks.h"

#include <ilib/string.h>

#define	ATA_SECTOR_SIZE	512

#define	READ(port, buffer, sectors)	\
	asm volatile ("cld;rep;insw"::"d"(port), "D"(buffer), "c"(sectors));

#define	WRITE(port, buffer, sectors)	\
	asm volatile ("cld;rep;outsw"::"d"(port), "S"(buffer), "c"(sectors));

typedef struct
{
	int32		io_base;
	int32		control;
	int32		slave;
	ATAIdentify	identity;
	BOOL		existed;
} ATADevice, * ATADevicePtr;

#define	MAX_RETRY		0x100
#define	MAX_LOCK_RETRY	0x00100000

#define	_LOCK_TID_NONE		-2	// 没有任何任务获取到锁。
#define	_LOCK_TID_KERNEL	-1	// 内核获取了锁。

// 如果为TRUE则说明当前有任务占用ATA驱动。
static BOOL _lock		= FALSE;

// 指示哪个任务正在占用ATA驱动。
static int32 _lock_tid	= _LOCK_TID_NONE;

static ATADevice _ata_primary_master	= {.io_base = 0x1F0, .control = 0x3F6, .slave = 0, .existed = FALSE};
static ATADevice _ata_primary_slave		= {.io_base = 0x1F0, .control = 0x3F6, .slave = 1, .existed = FALSE};
static ATADevice _ata_secondary_master	= {.io_base = 0x170, .control = 0x376, .slave = 0, .existed = FALSE};
static ATADevice _ata_secondary_slave	= {.io_base = 0x170, .control = 0x376, .slave = 1, .existed = FALSE};

/**
	@Function:		_AtaLock
	@Access:		Private
	@Description:
		锁住ATA驱动。
	@Parameters:
	@Return:
		BOOL
			返回TRUE则获取锁成功，否则获取锁失败。
*/
static
BOOL
_AtaLock(void)
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
	@Function:		_AtaUnlock
	@Access:		Private
	@Description:
		解锁ATA驱动。
	@Parameters:
	@Return:
*/
static
void
_AtaUnlock(void)
{
	_lock = FALSE;
	_lock_tid = _LOCK_TID_NONE;
}

/**
	@Function:		AtaIsLocked
	@Access:		Public
	@Description:
		获取ATA驱动的锁的状态。
	@Parameters:
	@Return:
		BOOL
			返回TRUE则代表ATA驱动的锁被锁定中。
*/
BOOL
AtaIsLocked(void)
{
	return _lock;
}

/**
	@Function:		AtaGetLockTid
	@Access:		Public
	@Description:
		获取正在占用ATA驱动的任务ID。
	@Parameters:
	@Return:
		int32
			正在占用ATA驱动的任务ID。
*/
int32
AtaGetLockTid(void)
{
	return _lock_tid;
}

/**
	@Function:		AtaAttemptToUnlock
	@Access:		Public
	@Description:
		尝试解除ATA驱动的锁。
	@Parameters:
		tid, int32, IN
			任务ID。如果该值等于当前正在占用ATA驱动的锁的任务ID，则解除锁。
	@Return:
*/
void
AtaAttemptToUnlock(IN int32 tid)
{
	if(tid == _lock_tid)
		_AtaUnlock();
}

/**
	@Function:		_AtaGetSectorCount
	@Access:		Private
	@Description:
		获取ATA设备的扇区数。
	@Parameters:
		dev, ATADevicePtr, IN
			指向ATA设备对象的指针。
	@Return:
		uint64
			ATA设备的扇区数。
*/
static
uint64
_AtaGetSectorCount(IN ATADevicePtr dev)
{
	uint64 sectors = dev->identity.sectors_48;
	if(sectors == 0)
		sectors = dev->identity.sectors_28;
	return sectors;
}

/**
	@Function:		_AtaWaitIo
	@Access:		Private
	@Description:
		等待ATA设备的I/O。
	@Parameters:
		dev, ATADevicePtr, IN
			指向ATA设备对象的指针。
	@Return:
*/
static
void
_AtaWaitIo(IN ATADevicePtr dev)
{
	KnlInByte(dev->io_base + ATA_REG_ALTSTATUS);
	KnlInByte(dev->io_base + ATA_REG_ALTSTATUS);
	KnlInByte(dev->io_base + ATA_REG_ALTSTATUS);
	KnlInByte(dev->io_base + ATA_REG_ALTSTATUS);
}

/**
	@Function:		_AtaWaitStatus
	@Access:		Private
	@Description:
		获取ATA设备的状态。
	@Parameters:
		dev, ATADevicePtr, IN
			指向ATA设备对象的指针。
		timeout, int32, IN
			如果设备处于忙状态，这个指定重试次数。
	@Return:
		int32
			ATA设备的状态。
*/
static
int32
_AtaWaitStatus(	IN ATADevicePtr dev,
				IN int32 timeout)
{
	int32 status;
	if(timeout > 0)
	{
		int32 i = 0;
		while(	((status = KnlInByte(dev->io_base + ATA_REG_STATUS)) & ATA_SR_BSY)
				&& i++ < timeout);
	}
	else
		while((status = KnlInByte(dev->io_base + ATA_REG_STATUS)) & ATA_SR_BSY);
	return status;
}

/**
	@Function:		_AtaWait
	@Access:		Private
	@Description:
		等待ATA设备。
	@Parameters:
		dev, ATADevicePtr, IN
			指向ATA设备对象的指针。
		advanced, BOOL, IN
			指示是否检查ATA设备的状态。
	@Return:
		BOOL
			如果参数advanced为FALSE，恒为TRUE。
			如果参数advanced为TRUE并且设备未出错，则返回TRUE。
*/
static
BOOL
_AtaWait(	IN ATADevicePtr dev,
			IN BOOL advanced)
{
	uint8 status = 0;
	_AtaWaitIo(dev);
	status = _AtaWaitStatus(dev, 0);
	if(advanced)
	{
		status = KnlInByte(dev->io_base + ATA_REG_STATUS);
		if(	status & ATA_SR_ERR
			|| status & ATA_SR_DF
			|| !(status & ATA_SR_DRQ))
			return FALSE;
	}
	return TRUE;
}

/**
	@Function:		_AtaSoftReset
	@Access:		Private
	@Description:
		软重置ATA设备。
	@Parameters:
		dev, ATADevicePtr, IN
			指向ATA设备对象的指针。
	@Return:
*/
static
void
_AtaSoftReset(IN ATADevicePtr dev)
{
	KnlOutByte(dev->control, 0x04);
	_AtaWaitIo(dev);
	KnlOutByte(dev->control, 0x00);
}

/**
	@Function:		_AtaInitDevice
	@Access:		Private
	@Description:
		初始化ATA设备。
	@Parameters:
		dev, ATADevicePtr, IN OUT
			指向ATA设备对象的指针。
	@Return:
*/
static
void
_AtaInitDevice(IN OUT ATADevicePtr dev)
{
	int32 i;

	KnlOutByte(dev->io_base + 1, 1);
	KnlOutByte(dev->control, 0);

	KnlOutByte(dev->io_base + ATA_REG_HDDEVSEL, 0xa0 | dev->slave << 4);
	_AtaWaitIo(dev);

	KnlOutByte(dev->io_base + ATA_REG_COMMAND, ATA_CMD_IDENTIFY);
	_AtaWaitIo(dev);

	int32 status = KnlInByte(dev->io_base + ATA_REG_COMMAND);

	_AtaWait(dev, FALSE);

	uint16 * buf = (uint16 *)&dev->identity;
	for(i = 0; i < 256; i++)
		buf[i] = KnlInWord(dev->io_base);

	uint8 * ptr = (uint8 *)&dev->identity.model;
	for(i = 0; i < 39; i += 2)
	{
		uint8 tmp = ptr[i + 1];
		ptr[i + 1] = ptr[i];
		ptr[i] = tmp;
	}

	KnlOutByte(dev->io_base + ATA_REG_CONTROL, 0x02);
}

/**
	@Function:		_AtaDetectDevice
	@Access:		Private
	@Description:
		探测ATA设备。如果ATA设备存在，则初始化ATA设备。
	@Parameters:
		dev, ATADevicePtr, IN OUT
			指向ATA设备对象的指针。
	@Return:
		BOOL
			如果设备存在则返回TRUE。
*/
static
BOOL
_AtaDetectDevice(IN OUT ATADevicePtr dev)
{
	_AtaSoftReset(dev);
	_AtaWaitIo(dev);
	KnlOutByte(dev->io_base + ATA_REG_HDDEVSEL, 0xa0 | dev->slave << 4);
	_AtaWaitIo(dev);
	_AtaWaitStatus(dev, 10000);

	uint8 cl = KnlInByte(dev->io_base + ATA_REG_LBA1);
	uint8 ch = KnlInByte(dev->io_base + ATA_REG_LBA2);

	// 没有设备。
	if(cl == 0xff && ch == 0xff)
		return FALSE;

	// 并行ATA设备或模拟的SATA设备。
	if(	(cl == 0x00 && ch == 0x00)
		|| (cl == 0x3c && ch == 0xc3))
	{
		dev->existed = TRUE;
		_AtaInitDevice(dev);
		return TRUE;
	}

	return FALSE;
}

/**
	@Function:		_AtaReadSectors
	@Access:		Private
	@Description:
		读取ATA设备的扇区。
		该函数不允许在一次调用未结束时再次调用，以及当ATAPI驱动正在进行I/O时，也不允许调用该函数。
	@Parameters:
		dev, ATADevicePtr, IN
			指向ATA设备对象的指针。
		lba, uint32, IN
			LBA地址。
		count, uint8, IN
			读取的扇区数。如果等于0，则代表读取256个扇区。
		buf, uint8 *, OUT
			指向用于保存数据的缓冲区的指针。
	@Return:
		BOOL
			返回TRUE则读取成功。
*/
static
BOOL
_AtaReadSectors(IN ATADevicePtr dev,
				IN uint32 lba,
				IN uint8 count,
				OUT uint8 * buf)
{
	uint16 bus = dev->io_base;
	uint8 slave = dev->slave;

	KnlOutByte(bus + ATA_REG_CONTROL, 0x02);

	_AtaWait(dev, FALSE);

	KnlOutByte(bus + ATA_REG_HDDEVSEL, 0xe0 | slave << 4 | (lba & 0x0f000000) >> 24);
	KnlOutByte(bus + ATA_REG_FEATURES, 0x00);
	KnlOutByte(bus + ATA_REG_SECCOUNT0, count);
	KnlOutByte(bus + ATA_REG_LBA0, (lba & 0x000000ff) >>  0);
	KnlOutByte(bus + ATA_REG_LBA1, (lba & 0x0000ff00) >>  8);
	KnlOutByte(bus + ATA_REG_LBA2, (lba & 0x00ff0000) >> 16);
	KnlOutByte(bus + ATA_REG_COMMAND, ATA_CMD_READ_PIO);

	int32 count_i32 = count == 0 ? 256 : count;
	int32 size = ATA_SECTOR_SIZE / 2;
	int32 i;
	for(i = 0; i < count_i32; i++)
	{
		if(!_AtaWait(dev, TRUE))
			goto err;
		READ(bus, buf, size);
		buf += ATA_SECTOR_SIZE;
	}
	_AtaWait(dev, FALSE);

	return TRUE;
err:
	return FALSE;
}

/**
	@Function:		_AtaWriteSectors
	@Access:		Private
	@Description:
		写入ATA设备的扇区。
		该函数不允许在一次调用未结束时再次调用，以及当ATAPI驱动正在进行I/O时，也不允许调用该函数。
	@Parameters:
		dev, ATADevicePtr, IN
			指向ATA设备对象的指针。
		lba, uint32, IN
			LBA地址。
		count, uint8, IN
			写入的扇区数。如果等于0，则代表写入256个扇区。
		buf, uint8 *, IN
			指向用于保存数据的缓冲区的指针。
	@Return:
		BOOL
			返回TRUE则写入成功。
*/
static
BOOL
_AtaWriteSectors(	IN ATADevicePtr dev,
					IN uint32 lba,
					IN uint8 count,
					IN uint8 * buf)
{
	uint16 bus = dev->io_base;
	uint8 slave = dev->slave;

	KnlOutByte(bus + ATA_REG_CONTROL, 0x02);

	_AtaWait(dev, FALSE);
	KnlOutByte(bus + ATA_REG_HDDEVSEL, 0xe0 | slave << 4 | (lba & 0x0f000000) >> 24);
	_AtaWait(dev, FALSE);

	KnlOutByte(bus + ATA_REG_FEATURES, 0x00);
	KnlOutByte(bus + ATA_REG_SECCOUNT0, count);
	KnlOutByte(bus + ATA_REG_LBA0, (lba & 0x000000ff) >>  0);
	KnlOutByte(bus + ATA_REG_LBA1, (lba & 0x0000ff00) >>  8);
	KnlOutByte(bus + ATA_REG_LBA2, (lba & 0x00ff0000) >> 16);
	KnlOutByte(bus + ATA_REG_COMMAND, ATA_CMD_WRITE_PIO);

	int32 count_i32 = count == 0 ? 256 : count;
	int32 size = ATA_SECTOR_SIZE / 2;
	int32 i;
	for(i = 0; i < count_i32; i++)
	{
		if(!_AtaWait(dev, TRUE))
			goto err;
		WRITE(bus, buf, size);
		KnlOutByte(bus + 0x07, ATA_CMD_CACHE_FLUSH);
		buf += ATA_SECTOR_SIZE;
	}
	_AtaWait(dev, FALSE);

	return TRUE;
err:
	return FALSE;
}

/**
	@Function:		_AtaGetDeviceBySymbol
	@Access:		Private
	@Description:
		根据ATA设备符号获取ATA设备对象。
	@Parameters:
		symbol, CASCTEXT, IN
			ATA设备符号。
	@Return:
		ATADevicePtr
			指向ATA设备对象的指针。
*/
static
ATADevicePtr
_AtaGetDeviceBySymbol(IN CASCTEXT symbol)
{
	if(strcmp(symbol, ATA_DISK0_SYMBOL) == 0)
		return &_ata_primary_master;
	else if(strcmp(symbol, ATA_DISK1_SYMBOL) == 0)
		return &_ata_primary_slave;
	else if(strcmp(symbol, ATA_DISK2_SYMBOL) == 0)
		return &_ata_secondary_master;
	else if(strcmp(symbol, ATA_DISK3_SYMBOL) == 0)
		return &_ata_secondary_slave;
	else
		return NULL;
}

/**
	@Function:		AtaInit
	@Access:		Public
	@Description:
		初始化ATA驱动。
	@Parameters:
	@Return:
		uint32
			这些值的组合：ATA_DISK0、ATA_DISK1、ATA_DISK2、ATA_DISK3。
			如果返回值的指定位被设置，则代表对应的ATA设备存在。
*/
uint32
AtaInit(void)
{
	uint32 r = 0;

	if(_AtaDetectDevice(&_ata_primary_master))
		r |= ATA_DISK0;
	if(_AtaDetectDevice(&_ata_primary_slave))
		r |= ATA_DISK1;
	if(_AtaDetectDevice(&_ata_secondary_master))
		r |= ATA_DISK2;
	if(_AtaDetectDevice(&_ata_secondary_slave))
		r |= ATA_DISK3;

	return r;
}

/**
	@Function:		AtaDeviceExisted
	@Access:		Public
	@Description:
		确认ATA设备是否存在。
	@Parameters:
		symbol, CASCTEXT, IN
			ATA设备符号。
	@Return:
		BOOL
			返回TRUE则存在，否则不存在。
*/
BOOL
AtaDeviceExisted(IN CASCTEXT symbol)
{
	ATADevicePtr dev = _AtaGetDeviceBySymbol(symbol);
	if(dev != NULL)
		return dev->existed;
	else
		return FALSE;
}

/**
	@Function:		AtaGetSectorCount
	@Access:		Public
	@Description:
		获取ATA设备的扇区数。
	@Parameters:
		symbol, CASCTEXT, IN
			ATA设备符号。
	@Return:
		uint64
			ATA设备的扇区数。
*/
uint64
AtaGetSectorCount(IN CASCTEXT symbol)
{
	ATADevicePtr dev = _AtaGetDeviceBySymbol(symbol);
	if(dev != NULL)
		return _AtaGetSectorCount(dev);
	else
		return 0UL;
}

/**
	@Function:		AtaReadSectors
	@Access:		Public
	@Description:
		读取ATA设备的扇区。
	@Parameters:
		symbol, CASCTEXT, IN
			ATA设备符号。
		lba, uint32, IN
			LBA地址。
		count, uint8, IN
			读取的扇区数。如果等于0，则代表读取256个扇区。
		buf, uint8 *, OUT
			指向用于保存数据的缓冲区的指针。
	@Return:
		BOOL
			返回TRUE则读取成功。
*/
BOOL
AtaReadSectors(	IN CASCTEXT symbol,
				IN uint32 lba,
				IN uint8 count,
				OUT uint8 * buf)
{
	if(AtapiIsLocked())
		return FALSE;
	if(!_AtaLock())
		return FALSE;
	ATADevicePtr dev = _AtaGetDeviceBySymbol(symbol);
	uint32 ui;
	for(ui = 0; ui < MAX_RETRY; ui++)
	{
		BOOL r = _AtaReadSectors(dev, lba, count, buf);
		if(r)
		{
			_AtaUnlock();
			return TRUE;
		}
	}
	_AtaUnlock();
	return FALSE;
}

/**
	@Function:		AtaWriteSectors
	@Access:		Public
	@Description:
		写入ATA设备的扇区。
	@Parameters:
		symbol, CASCTEXT, IN
			ATA设备符号。
		lba, uint32, IN
			LBA地址。
		count, uint8, IN
			写入的扇区数。如果等于0，则代表写入256个扇区。
		buf, uint8 *, IN
			指向用于保存数据的缓冲区的指针。
	@Return:
		BOOL
			返回TRUE则写入成功。
*/
BOOL
AtaWriteSectors(IN CASCTEXT symbol,
				IN uint32 lba,
				IN uint8 count,
				IN uint8 * buf)
{
	if(AtapiIsLocked())
		return FALSE;
	if(!_AtaLock())
		return FALSE;
	ATADevicePtr dev = _AtaGetDeviceBySymbol(symbol);
	uint32 ui;
	for(ui = 0; ui < MAX_RETRY; ui++)
	{
		BOOL r = _AtaWriteSectors(dev, lba, count, buf);
		if(r)
		{
			_AtaUnlock();
			return TRUE;
		}
	}
	_AtaUnlock();
	return FALSE;
}

/**
	@Function:		AtaReadSector
	@Access:		Public
	@Description:
		读取ATA设备的一个扇区。
	@Parameters:
		symbol, CASCTEXT, IN
			ATA设备符号。
		lba, uint32, IN
			LBA地址。
		buf, uint8 *, OUT
			指向用于保存数据的缓冲区的指针。
	@Return:
		BOOL
			返回TRUE则读取成功。
*/
BOOL
AtaReadSector(	IN CASCTEXT symbol,
				IN uint32 lba,
				OUT uint8 * buf)
{
	return AtaReadSectors(symbol, lba, 1, buf);
}

/**
	@Function:		AtaWriteSector
	@Access:		Public
	@Description:
		写入ATA设备的一个扇区。
	@Parameters:
		symbol, CASCTEXT, IN
			ATA设备符号。
		lba, uint32, IN
			LBA地址。
		buf, uint8 *, IN
			指向用于保存数据的缓冲区的指针。
	@Return:
		BOOL
			返回TRUE则写入成功。
*/
BOOL
AtaWriteSector(	IN CASCTEXT symbol,
				IN uint32 lba,
				IN uint8 * buf)
{
	return AtaWriteSectors(symbol, lba, 1, buf);
}
