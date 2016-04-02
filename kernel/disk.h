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

extern
void
get_disk_symbol(	IN uint32 index, 
					OUT int8 * symbol);

extern
uint32
get_disk_count(void);

extern
uint32
get_disk_size(IN int8 * symbol);

extern
BOOL
is_system_disk(IN int8 * symbol);

extern
void
init_disk(IN int8 * symbol);

extern
void
destroy_disk(IN int8 * symbol);

extern
uint32
sector_count(IN int8 * symbol);

extern
uint64
disk_wbytes(IN CASCTEXT symbol);

extern
uint64
disk_rbytes(IN CASCTEXT symbol);

extern
BOOL
read_sector(IN int8 * symbol, 
			IN uint32 pos, 
			OUT uint8 * buffer);

extern
BOOL
write_sector(	IN int8 * symbol, 
				IN uint32 pos, 
				IN uint8 * buffer);

extern
BOOL
read_sectors(	IN int8 * symbol, 
				IN uint32 pos, 
				IN uint8 count, 
				OUT uint8 * buffer);

extern
BOOL
write_sectors(	IN int8 * symbol, 
				IN uint32 pos, 
				IN uint8 count, 
				IN uint8 * buffer);

#endif
