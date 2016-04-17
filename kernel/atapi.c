/**
	@File:			atapi.c
	@Author:		Ihsoh
	@Date:			2015-02-26
	@Description:
		ATAPI。
*/

#include "atapi.h"
#include "types.h"
#include "386.h"
#include "ata.h"
#include "kernel.h"
#include "cmlock.h"

#define	MAX_RETRY	0x100
#define	MAX_LOCK_RETRY	0x00100000

#define	_LOCK_TID_NONE		-2	// 没有任何任务获取到锁。
#define	_LOCK_TID_KERNEL	-1	// 内核获取了锁。

// 如果为TRUE则说明当前有任务占用ATAPI驱动。
static BOOL _lock = FALSE;

// 指示哪个任务正在占用ATAPI驱动。
static int32 _lock_tid	= _LOCK_TID_NONE;

/**
	@Function:		AtapiInit
	@Access:		Public
	@Description:
		初始化ATAPI。
	@Parameters:
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
BOOL
AtapiInit(void)
{
	return TRUE;
}

/**
	@Function:		_AtapiLock
	@Access:		Private
	@Description:
		锁住ATAPI驱动。
	@Parameters:
	@Return:
		BOOL
			返回TRUE则获取锁成功，否则获取锁失败。
*/
static
BOOL
_AtapiLock(void)
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
	@Function:		_AtapiUnlock
	@Access:		Private
	@Description:
		解锁ATAPI驱动。
	@Parameters:
	@Return:
*/
static
void
_AtapiUnlock(void)
{
	_lock = FALSE;
	_lock_tid = _LOCK_TID_NONE;
}

/**
	@Function:		AtapiIsLocked
	@Access:		Public
	@Description:
		获取ATAPI驱动的锁的状态。
	@Parameters:
	@Return:
		BOOL
			返回TRUE则代表ATAPI驱动的锁被锁定中。
*/
BOOL
AtapiIsLocked(void)
{
	return _lock;
}

/**
	@Function:		AtapiGetLockTid
	@Access:		Public
	@Description:
		获取正在占用ATAPI驱动的任务ID。
	@Parameters:
	@Return:
		int32
			正在占用ATAPI驱动的任务ID。
*/
int32
AtapiGetLockTid(void)
{
	return _lock_tid;
}

/**
	@Function:		AtapiAttemptToUnlock
	@Access:		Public
	@Description:
		尝试解除ATAPI驱动的锁。
	@Parameters:
		tid, int32, IN
			任务ID。如果该值等于当前正在占用ATAPI驱动的锁的任务ID，则解除锁。
	@Return:
*/
void
AtapiAttemptToUnlock(IN int32 tid)
{
	if(tid == _lock_tid)
		_AtapiUnlock();
}

/**
	@Function:		_AtapiReadSector
	@Access:		Private
	@Description:
		从ATAPI设备读取一个扇区。
	@Parameters:
		bus, uint32, IN
			总线。ATA_BUS_PRIMARY或ATA_BUS_SECONDARY。
		drive, uint32, IN
			驱动器。ATA_DRIVE_MASTER或ATA_DRIVE_SLAVE。
		lba, uint32, IN
			LBA地址。
		buffer, uint8 *, OUT
			指向用于储存从ATAPI设备读取扇区的数据的缓冲区的指针。
	@Return:
		int32
			实际读取的字节数。
*/
static
int32
_AtapiReadSector(	IN uint32 bus,
					IN uint32 drive,
					IN uint32 lba,
					OUT uint8 * buffer)
{
	// 0xA8是读取扇区的命令字节。
	uint8 read_cmd[12] = { 0xA8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	uint8 status;
	int32 size;

	// 清除IDE信号。
	RESET_ALL_IDE_SIGNAL();

	// 选择驱动器，并且选择是Master还是Slave。
	KnlOutByte(ATA_DRIVE_SELECT(bus), drive & (1 << 4));      

	// 延迟400ns。
	ATA_SELECT_DELAY(bus);

	// PIO模式.
	KnlOutByte(ATA_FEATURES(bus), 0x0);
	KnlOutByte(ATA_ADDRESS2(bus), ATAPI_SECTOR_SIZE & 0xFF);
	KnlOutByte(ATA_ADDRESS3(bus), ATAPI_SECTOR_SIZE >> 8);

	// ATA Packet命令。
	KnlOutByte(ATA_COMMAND(bus), 0xA0);

	// 如果驱动器处于BUSY状态，等待。
	while((status = KnlInByte(ATA_COMMAND(bus))) & 0x80)
		asm volatile ("pause");

	// 等待数据。
	while (!((status = KnlInByte(ATA_COMMAND(bus))) & 0x8) && !(status & 0x1))
		asm volatile ("pause");

	// 发生错误则退出。
	if(status & 0x1)
		return -1;

	// 读取一个扇区。
	read_cmd[9] = 1;

	read_cmd[2] = (lba >> 0x18) & 0xFF;		// LBA最高位。
	read_cmd[3] = (lba >> 0x10) & 0xFF;
	read_cmd[4] = (lba >> 0x08) & 0xFF;
	read_cmd[5] = (lba >> 0x00) & 0xFF;		// LBA最低位。

	// 发送ATAPI/SCSI命令
	KnlOutWords(ATA_DATA(bus), (uint16 *)read_cmd, 6);

	// 等待驱动器发送IRQ命令。
	while(!GET_ALL_IDE_SIGNAL())
		asm volatile ("pause");
	CLEAR_ALL_IDE_SIGNAL();
	
	// 获取实际读取的字节数。
	size = 	(((int32)KnlInByte(ATA_ADDRESS3(bus))) << 8)
			| (int32)(KnlInByte(ATA_ADDRESS2(bus)));
	
	if(size != ATAPI_SECTOR_SIZE)
		return -1;
	
	// 读取数据。
	/*int32 i;
	for(i = 0; i < size / 2; i++)
	{
		while (!((status = KnlInByte(ATA_COMMAND(bus))) & 0x8) && !(status & 0x1))
			asm volatile ("pause");
		*(uint16 *)buffer = KnlInWord(ATA_DATA(bus));
		buffer += 2;
	}*/
	while (!((status = KnlInByte(ATA_COMMAND(bus))) & 0x8) && !(status & 0x1))
		asm volatile ("pause");
	KnlInWords(ATA_DATA(bus), buffer, size / 2);

	/* The controller will send another IRQ even though we've read all
	* the data we want.  Wait for it -- so it doesn't interfere with
	* subsequent operations: */
	while(!GET_ALL_IDE_SIGNAL())
		asm volatile ("pause");
	CLEAR_ALL_IDE_SIGNAL();
	
	// 等待BUSY和DRQ标志被清除，表示命令结束。
	while((status = KnlInByte(ATA_COMMAND(bus))) & 0x88)
		asm volatile ("pause");
	
	return size;
}

/**
	@Function:		AtapiReadSector
	@Access:		Public
	@Description:
		从ATAPI设备读取一个扇区。
	@Parameters:
		bus, uint32, IN
			总线。ATA_BUS_PRIMARY或ATA_BUS_SECONDARY。
		drive, uint32, IN
			驱动器。ATA_DRIVE_MASTER或ATA_DRIVE_SLAVE。
		lba, uint32, IN
			LBA地址。
		buffer, uint8 *, OUT
			指向用于储存从ATAPI设备读取扇区的数据的缓冲区的指针。
	@Return:
		int32
			实际读取的字节数。
*/
int32
AtapiReadSector(IN uint32 bus,
				IN uint32 drive,
				IN uint32 lba,
				OUT uint8 * buffer)
{
	if(AtaIsLocked())
		return -1;
	if(!_AtapiLock())
		return -1;
	uint32 ui;
	for(ui = 0; ui < MAX_RETRY; ui++)
	{
		int32 size = _AtapiReadSector(bus, drive, lba, buffer);
		if(size == ATAPI_SECTOR_SIZE)
		{
			_AtapiUnlock();
			return size;
		}
	}
	_AtapiUnlock();
	return -1;
}

/**
	@Function:		AtapiReadSector512
	@Access:		Public
	@Description:
		从ATAPI设备读取512字节的数据，模拟一个扇区为512字节。
		把一个2048字节的扇区分成4个512字节的模拟扇区。
	@Parameters:
		bus, uint32, IN
			总线。ATA_BUS_PRIMARY或ATA_BUS_SECONDARY。
		drive, uint32, IN
			驱动器。ATA_DRIVE_MASTER或ATA_DRIVE_SLAVE。
		pos, uint32, IN
			模拟扇区的地址。
		buffer, uint8 *, OUT
			指向用于储存从ATAPI设备读取的数据的缓冲区的指针。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
BOOL
AtapiReadSector512(	IN uint32 bus,
					IN uint32 drive,
					IN uint32 pos, 
					OUT uint8 * buffer)
{
	uint32 lba = pos / 4;
	uint32 part = pos % 4;
	uint8 tmp[ATAPI_SECTOR_SIZE];
	if(AtapiReadSector(bus, drive, lba, tmp) != ATAPI_SECTOR_SIZE)
		return FALSE;
	memcpy(buffer, tmp + part * 512, 512);
	return TRUE;
}

/**
	@Function:		AtapiReadSector512s
	@Access:		Public
	@Description:
		从ATAPI设备读取指定数量的模拟扇区的数据。
		把一个2048字节的扇区分成4个512字节的模拟扇区。
	@Parameters:
		bus, uint32, IN
			总线。ATA_BUS_PRIMARY或ATA_BUS_SECONDARY。
		drive, uint32, IN
			驱动器。ATA_DRIVE_MASTER或ATA_DRIVE_SLAVE。
		pos, uint32, IN
			模拟扇区的地址。
		count, uint32, IN
			模拟扇区的数量。
		buffer, uint8 *, OUT
			指向用于储存从ATAPI设备读取的数据的缓冲区的指针。
	@Return:
		BOOL
			返回TRUE则成功，否则失败。
*/
BOOL
AtapiReadSector512s(IN uint32 bus,
					IN uint32 drive,
					IN uint32 pos,
					IN uint32 count,
					OUT uint8 * buffer)
{
	uint32 ui;
	for(ui = 0; ui < count; ui++, pos++, buffer += 512)
		if(!AtapiReadSector512(bus, drive, pos, buffer))
			return FALSE;
	return TRUE;
}
