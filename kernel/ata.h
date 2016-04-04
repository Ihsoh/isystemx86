/**
	@File:			ata.h
	@Author:		Ihsoh
	@Date:			2016-04-04
	@Description:
		提供支持ATA协议的硬盘的操作功能。
*/

#ifndef	_ATA_H_
#define	_ATA_H_

#include "types.h"

#define ATA_SR_BSY     0x80
#define ATA_SR_DRDY    0x40
#define ATA_SR_DF      0x20
#define ATA_SR_DSC     0x10
#define ATA_SR_DRQ     0x08
#define ATA_SR_CORR    0x04
#define ATA_SR_IDX     0x02
#define ATA_SR_ERR     0x01

#define ATA_ER_BBK      0x80
#define ATA_ER_UNC      0x40
#define ATA_ER_MC       0x20
#define ATA_ER_IDNF     0x10
#define ATA_ER_MCR      0x08
#define ATA_ER_ABRT     0x04
#define ATA_ER_TK0NF    0x02
#define ATA_ER_AMNF     0x01

#define ATA_CMD_READ_PIO          0x20
#define ATA_CMD_READ_PIO_EXT      0x24
#define ATA_CMD_READ_DMA          0xC8
#define ATA_CMD_READ_DMA_EXT      0x25
#define ATA_CMD_WRITE_PIO         0x30
#define ATA_CMD_WRITE_PIO_EXT     0x34
#define ATA_CMD_WRITE_DMA         0xCA
#define ATA_CMD_WRITE_DMA_EXT     0x35
#define ATA_CMD_CACHE_FLUSH       0xE7
#define ATA_CMD_CACHE_FLUSH_EXT   0xEA
#define ATA_CMD_PACKET            0xA0
#define ATA_CMD_IDENTIFY_PACKET   0xA1
#define ATA_CMD_IDENTIFY          0xEC

#define ATAPI_CMD_READ       0xA8
#define ATAPI_CMD_EJECT      0x1B

#define ATA_IDENT_DEVICETYPE   0
#define ATA_IDENT_CYLINDERS    2
#define ATA_IDENT_HEADS        6
#define ATA_IDENT_SECTORS      12
#define ATA_IDENT_SERIAL       20
#define ATA_IDENT_MODEL        54
#define ATA_IDENT_CAPABILITIES 98
#define ATA_IDENT_FIELDVALID   106
#define ATA_IDENT_MAX_LBA      120
#define ATA_IDENT_COMMANDSETS  164
#define ATA_IDENT_MAX_LBA_EXT  200

#define IDE_ATA        0x00
#define IDE_ATAPI      0x01
 
#define ATA_MASTER     0x00
#define ATA_SLAVE      0x01

#define ATA_REG_DATA       0x00
#define ATA_REG_ERROR      0x01
#define ATA_REG_FEATURES   0x01
#define ATA_REG_SECCOUNT0  0x02
#define ATA_REG_LBA0       0x03
#define ATA_REG_LBA1       0x04
#define ATA_REG_LBA2       0x05
#define ATA_REG_HDDEVSEL   0x06
#define ATA_REG_COMMAND    0x07
#define ATA_REG_STATUS     0x07
#define ATA_REG_SECCOUNT1  0x08
#define ATA_REG_LBA3       0x09
#define ATA_REG_LBA4       0x0A
#define ATA_REG_LBA5       0x0B
#define ATA_REG_CONTROL    0x0C
#define ATA_REG_ALTSTATUS  0x0C
#define ATA_REG_DEVADDRESS 0x0D

// 通道。
#define ATA_PRIMARY      0x00
#define ATA_SECONDARY    0x01

// 方向。
#define ATA_READ      0x00
#define ATA_WRITE     0x01

typedef struct
{
	uint16	base;
	uint16	ctrl;
	uint16	bmide;
	uint16	nien;
} IDEChannelRegs, * IDEChannelRegsPtr;

typedef struct
{
	uint8	reserved;
	uint8	channel;
	uint8	drive;
	uint16	type;
	uint16	signature;
	uint16	capabilities;
	uint32	command_sets;
	uint32	size;
	uint8	model[41];
} IDEDevice, * IDEDevicePtr;

typedef struct
{
	uint8	status;
	uint8	chs_first_sector[3];
	uint8	type;
	uint8	chs_last_sector[3];
	uint32	lba_first_sector;
	uint32	sector_count;
} Partition, * PartitionPtr;

typedef struct
{
	uint16	flags;
	uint16	unused1[9];
	int8	serial[20];
	uint16	unused2[3];
	int8	firmware[8];
	int8	model[40];
	uint16	sectors_per_int;
	uint16	unused3;
	uint16	capabilities[2];
	uint16	unused4[2];
	uint16	valid_ext_data;
	uint16	unused5[5];
	uint16	size_of_rw_mult;
	uint32	sectors_28;
	uint16	unused6[38];
	uint64	sectors_48;
	uint16	unused7[152];
} __attribute__((packed)) ATAIdentify, * ATAIdentifyPtr;

typedef struct
{
	uint8		boostrap[446];
	Partition	partitions[4];
	uint8		signature[2];
} __attribute__((packed)) MBR, * MBRPtr;

#define	ATA_DISK0_SYMBOL	"DA"
#define	ATA_DISK1_SYMBOL	"DB"
#define	ATA_DISK2_SYMBOL	"DC"
#define	ATA_DISK3_SYMBOL	"DD"

#define	ATA_DISK0	0x00000001
#define	ATA_DISK1	0x00000002
#define	ATA_DISK2	0x00000004
#define	ATA_DISK3	0x00000008

extern
BOOL
ata_locked(void);

extern
int32
ata_lock_tid(void);

extern
void
ata_attempt_to_unlock(IN int32 tid);

extern
uint32
ata_init(void);

extern
BOOL
ata_is_existed(IN CASCTEXT symbol);

extern
uint64
ata_sector_count(IN CASCTEXT symbol);

extern
BOOL
ata_device_read_sectors(IN CASCTEXT symbol,
						IN uint32 lba,
						IN uint8 count,
						OUT uint8 * buf);

extern
BOOL
ata_device_write_sectors(	IN CASCTEXT symbol,
							IN uint32 lba,
							IN uint8 count,
							IN uint8 * buf);

extern
BOOL
ata_device_read_sector(	IN CASCTEXT symbol,
						IN uint32 lba,
						OUT uint8 * buf);

extern
BOOL
ata_device_write_sector(IN CASCTEXT symbol,
						IN uint32 lba,
						IN uint8 * buf);

#endif
