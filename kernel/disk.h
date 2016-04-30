/**
	@File:			disk.h
	@Author:		Ihsoh
	@Date:			2014-6-5
	@Description:
*/

#ifndef	_DISK_H_
#define	_DISK_H_

#include "types.h"

#define MAX_DISK_COUNT				16
#define	DISK_SYMBOL_BUFFER_SIZE		3

#define	DISK_BYTES_PER_SECTOR		512

#define	DISK_TYPE_ATA				1
#define	DISK_TYPE_SATA				2

#define	DISK_HARD_DISK_TYPE			(disk_hard_disk_type)

extern
void
DskGetBySymbol(	IN uint32 index, 
				OUT int8 * symbol);

extern
uint32
DskGetCount(void);

extern
uint32
DskGetSize(IN int8 * symbol);

extern
BOOL
DskHasSystem(IN int8 * symbol);

extern
void
DskInit(IN int8 * symbol);

extern
void
DskDestroy(IN int8 * symbol);

extern
uint32
DskGetSectorCount(IN int8 * symbol);

extern
uint64
DskGetWBytes(IN CASCTEXT symbol);

extern
uint64
DskGetRBytes(IN CASCTEXT symbol);

extern
BOOL
DskReadSector(	IN int8 * symbol, 
				IN uint32 pos, 
				OUT uint8 * buffer);

extern
BOOL
DskWriteSector(	IN int8 * symbol, 
				IN uint32 pos, 
				IN uint8 * buffer);

extern
BOOL
DskReadSectors(	IN int8 * symbol, 
				IN uint32 pos, 
				IN uint8 count, 
				OUT uint8 * buffer);

extern
BOOL
DskWriteSectors(IN int8 * symbol, 
				IN uint32 pos, 
				IN uint8 count, 
				IN uint8 * buffer);

#endif
